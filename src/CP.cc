#include "CP.h"
#define FEEDBACK 1600
Define_Module(CP);

void CP::initialize()
{
    char print_msg[100];
    cpPoint = new CPalg((cModule*) this,getIndex());
    cpPoint->resetQlen();
    selfEvent = new cMessage("sendEvent");

    /* for statistics */
    qlenSig = registerSignal("QLen");

    lossSig = registerSignal("LossSg");

    fbSig = registerSignal("fbCnt");
    interval = par("interval");
    lastTime = simTime().dbl();
    fbCnt = 0;
    CP_id=getIndex();

    sprintf(print_msg, "CP_id=%d w=%d qeq=%lf qlen=%lf qlenold=%lf fb=%d",CP_id,cpPoint->w, cpPoint->qeq, cpPoint->qlen,
            cpPoint->qlenOld, cpPoint->fb);
    EV << "\ninit CPalg():" << print_msg;
}

void CP::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage())
        processSelfTimer(msg);
    else
        // message arrived from Message Control
        processMsg(check_and_cast<Eth_pck *>(msg));
}

void CP::processSelfTimer(cMessage *msg)
{
    if (!strcmp(msg->getName(), "sendEvent"))
    {
        //TODO check here. If fb Message queue is not empty, the program is sending a fb message
        if (fbMsgQueue.size() != 0)
            msgTransmit(msg, 0);
        else
        {
            if (genMsgQueue.size() != 0)
            {
                cpPoint->popQlen(genMsgQueue[0]->getByteLength() / 1000.0); //subtract length from qlen
                msgTransmit(msg, 1);
            }
        }
    }
}
/* Due to type the function transmits the message:
 * type = 0 for feedback frame
 * type = 1 for general message
 */
void CP::msgTransmit(cMessage *selfMsg, int type)
{
    Eth_pck *tMsg;
    if (type == 0)
    {
        tMsg = fbMsgQueue[0];
        fbMsgQueue.erase(fbMsgQueue.begin()); //pop from FeedBack Message queue
    }
    else
    {
        tMsg = genMsgQueue[0];
        genMsgQueue.erase(genMsgQueue.begin()); //pop from General Message queue
    }
    send(tMsg, "out"); //transmit the General or FeedBack Frame
    cChannel* cha = gate("out")->getTransmissionChannel();

//	if (getParentModule()->getIndex()==0 && getIndex()==0)
//	EV << "the time is: "<< simTime() << "line is not busy scheduled to: "<< cha->getTransmissionFinishTime() << endl;
    scheduleAt(cha->getTransmissionFinishTime(), selfMsg);

}

void CP::processMsg(Eth_pck *msg)
{
    char print_msg[50];
    if (msg->getLength() == FEEDBACK)
    {
        fbMsgQueue.push_back(msg);
    }
    else
    {
        if (cpPoint->addQlen(msg->getByteLength() / 1000.0)) //qlen in KBits
        {
            genMsgQueue.push_back(msg);
            Eth_pck *fbMsg = cpPoint->receivedFrame(msg);
            //bubble("CP processMsg: Not feedback");

            if (fbMsg != NULL)
            {
                //debug message
                sprintf(print_msg,"CP_id=%d:Generated Fb",CP_id);
                bubble(print_msg);

                send(fbMsg, "mc$o"); //send my Feed Back Message back to Message controller
            }
            else
            {
                //bubble("CP ProcessMsg: fb is null");
            }
        }
        else
            delete msg;
    }
    cChannel* cha = gate("out")->getTransmissionChannel();
    if (!cha->isBusy())
    {
//		if (getParentModule()->getIndex()==0 && getIndex()==0)
//			EV << "the time is: "<< simTime() << "line is not busy scheduled to NOW" << endl;
        cancelEvent(selfEvent);
        scheduleAt(simTime(), selfEvent);
    }

}

/*
 * Function implementation of CPalg class
 */
CPalg::CPalg(cModule* fatherM,int CP_id)
{
    char print_msg[50];
    fatherModul = fatherM;
    this->CP_id=CP_id;
    double tempQlength = fatherModul->getAncestorPar("Q_LENGTH");
    double tempQpercentage = fatherModul->getAncestorPar("Q_EQ_STABLE_PERCENT");
    //Initialize variables
    w = fatherModul->getAncestorPar("W");
    qeq = tempQlength * tempQpercentage / 100.0;
    qlen = 0;
    qntzFb = 0;
    qlenOld = 0;
    fb = 0;
    timeToMark = markTable[0];

    /* statistic init */
    //pckLoss = 0;
    maxLen = fatherM->par("MaxQlen");
}
/*
 * 6 bit quantize get 6 MSB from param toQuan
 */
unsigned int CPalg::quantitize(int toQuan)
{
    unsigned int temp = 1;
    unsigned int qntzFb = 0;
    qntzFb = -1 * toQuan;
    if (toQuan >= 0)
        return 0;
    if (qntzFb < 64)
        return qntzFb;
    temp = (temp << 31);
    while (!(qntzFb & temp))
        qntzFb = (qntzFb << 1);
    temp = (temp >> 5);
    return qntzFb / temp;
}

/*
 * This is the main function of CP algorithm. For each incoming frame this function return FeedBack frame
 * if algorithm decide to generate FB frame with parameters that calculated in algorithm
 * else return NULL pointer
 */
Eth_pck *CPalg::receivedFrame(Eth_pck *incomeFrame)
{
    char print_msg[100];
    double nextPeriod;
    double rnd;

    fb = (qeq - qlen) - w * (qlen - qlenOld);
    if (fb < -qeq * (2 * w + 1))
        fb = -qeq * (2 * w + 1);
    else if (fb > 0)
        fb = 0;

    //fb=-2;
    //debug message
    sprintf(print_msg, "\nCPalg::receivedFrame CP_id=%d qlen=%lf qeq=%lf w=%d fb=%d",CP_id, qlen, qeq, w, fb);
    EV << print_msg;

    qntzFb = quantitize(fb);

    //Sampling probability is a function of FB
    generateFbFrame = 0;

    //debug message
    sprintf(print_msg, "\nCPalg::receivedFrame CP_id=%d TimeToMark=%lf incoming pkt length=%llu",CP_id, timeToMark,incomeFrame->getByteLength());
    EV << print_msg;

    timeToMark -= incomeFrame->getByteLength(); // Previous:timeToMark -=incomeFrame->getByteLength()/1000.0;//length in KB

    if (timeToMark < 0)
    {
        //generate a feedback Frame if fb is negative (qntzFb = 0 if fb =0 or positive)
        if (qntzFb > 0)
            generateFbFrame = 1;
        //update qlenOld
        qlenOld = qlen;
        nextPeriod = markTable[qntzFb / 8];
        //tToMarkRnd parameter that will return random number defined in .ini file
        rnd = fatherModul->par("tToMarkRnd");
        timeToMark = rnd * nextPeriod;
    }

    bool isQcn = fatherModul->par("QCN_ON");
    if (generateFbFrame && isQcn)
    {
        FeedBack* pckFb = new FeedBack("Feed Back");
        pckFb->setFb(qntzFb);
        pckFb->setQOff(qeq - qlen);
        pckFb->setQDelta(qlen - qlenOld);
        Eth_pck* pck = new Eth_pck("FeedBack");
        pck->setKind(5); // will mark feedBack messages with different color
        for (unsigned int i = 0; i < pck->getMacDestArraySize(); i++)
        {
            pck->setMacDest(i, incomeFrame->getMacSrc(i));
        }
        pck->setLength(FEEDBACK); // a feedback message which has no src adress yet
        pck->setByteLength(30); // TODO calculate this properly
        pck->encapsulate(pckFb);

        //printing debug message
        sprintf(print_msg, "fb packet generated: CP_id=%d packet length=%d qntzfb=%d",CP_id, pck->getLength(), qntzFb);
        EV << "\nCPalg::receivedFrame:" << print_msg;
        /* statistics */
        CP* temp = (CP*) fatherModul;
        if (simTime().dbl() - temp->lastTime > temp->interval)
        {
            temp->emit(temp->fbSig, temp->fbCnt);
            temp->fbCnt = 1;
            temp->lastTime = simTime().dbl();
        }
        else
        {
            temp->fbCnt++;
        }
        return pck;
    }
    return NULL;
}
/*
 *
 */
void CP::finish()
{
    /* statistics printing */
    EV
            << "**** Final statistics for CP[" << getIndex() << "] at switch[" << getParentModule()->getIndex() << "]"
                    << endl;
    EV << endl;
    /* deleting stuff */
    delete cpPoint;
    cancelAndDelete(selfEvent);
    for (unsigned int i = 0; i < fbMsgQueue.size(); i++)
    {
        Eth_pck* temp = fbMsgQueue[i];
        delete temp->decapsulate();
        delete temp;
    }
    for (unsigned int i = 0; i < genMsgQueue.size(); i++)
    {
        Eth_pck* temp = genMsgQueue[i];
        delete temp;
    }

}
/*
 * add len to qlen
 */
bool CPalg::addQlen(double len)
{
    CP* temp = (CP*) fatherModul;
    if (qlen + len > maxLen)
    {
        tLoss = simTime().dbl();
        temp->emit(temp->lossSig, tLoss);
        return false;
    }

    qlen += len;
    temp->emit(temp->qlenSig, qlen);
    //qLenStat.record(qlen);
    return true;
}
/*
 * pop len ftom qlen
 */
void CPalg::popQlen(double len)
{
    qlen -= len;
    if (qlen < 0)
        qlen = 0;
    //qLenStat.record(qlen);
}
/*
 * add len to qlen
 */
void CPalg::resetQlen()
{
    qlen = 0;
}
/*
 * Destructor
 */
CPalg::~CPalg()
{
//TODO check what memory need to free
}
