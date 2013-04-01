#include "host.h"
#include "Eth_pck_m.h"
#include "feedBack_m.h"
#include <math.h>
#define FEEDBACK 1600
#define ARP_REPLY_LENGTH 10
#define ARP_REQUEST_LENGTH 6
#define ETH_LENGTH 14
#define UNDEFINED -1
// for QCN algorithm
Define_Module(Host);
void Host::initialize()
{
    /*
     * initializing my mac adress by using the id on the parmeters on ini file
     */
    host_id=getIndex();
    int myId = getIndex();
    myMac = new unsigned char[6];
    myMac[0] = 0xFF;
    myMac[1] = 0xFF;
    myMac[2] = 0xFF;
    myMac[3] = 0xFF;
    myMac[4] = 0xFF;
    myMac[5] = getIndex();
    EV << "Initialized Host with MAC(index)=" << myMac[5] << "\n";
    msgIdCnt = 0;
    decideCnt = 0;
    // init rand array for ip randomize, array will hold all adresses of all host but myself
    // on later stages will random one of the cells.
    int size = par("hostNum");
    size = size - 1;
    randArr = new int[size]; //this array will hold the address of all host except the current host.
    for (int i = 0, j = 0; j < size; i++)
    {
        if (i != myId)
        {
            randArr[j] = i;
            j++;
        }
    }
    cMessage* msg = new cMessage("sendEvent");
    scheduleAt(simTime(), msg);
    /* initializing statistics variables */
    requestMsgGenCnt = 0;
    replyMsgGenCnt = 0;
    generalMsgGenCnt = 0;
    replyMsgRecCnt = 0;
    RTTSig = registerSignal("RTT");
    dataRateSig = registerSignal("dataRate");
    /* initializing variables for QCN algorithm */

    RL = new RP((cDatarateChannel*) gate("out")->getTransmissionChannel(), this); //TODO check deletion

    RL->host_id=host_id;
}
/*
 * Description:	seperating the self messages and messages from lower layer i.e the channel itself
 */
void Host::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage())
        processSelfTimer(msg);
    else
        // message arrived from switch
        processMsgFromLowerLayer(check_and_cast<Eth_pck *>(msg));

    //debug message
    char print_msg[100];
    sprintf(print_msg,"handleMessage: Host_id=%d cRate=%lf tRate=%lf",host_id,RL->cRate,RL->tRate);
    EV<<"\nhost.cc:"<<print_msg;
}
/*
 * Description:	this function handles messages that were received from CP
 */
void Host::processMsgFromLowerLayer(Eth_pck *packet)
{
    bool isMine = true;

    //this checks if the packet is for me or not
    for (unsigned int i = 0; i < packet->getMacDestArraySize() && isMine; i++)
    {
        if (myMac[i] != packet->getMacDest(i))
            isMine = false;
    }
    if (isMine) // message is mine
    {
        char print_msg[50];
        sprintf(print_msg, "Host.cc Host_id=% Pkt len=%d",host_id, packet->getLength());
        bubble(print_msg);
        EV<<"\n"<<print_msg;

        if (packet->getLength() == FEEDBACK)
        {
            bubble("received feedback message");
            RL->FeedbackMsg(packet);
        }
        else // regular message need to pass to check if its mine. and do stuff
        {

            bubble("received regular message");
            handleRegularMsg(packet);
        }
        delete packet;
    }
    else // message is not mine, should never be here
    {
        // TODO do stuff
        bubble("received message that is not mine");
        delete packet;
    }
}
/*
 * Description: this function handles self messages,
 * 				this is the function that sends messages to out gate
 */
void Host::processSelfTimer(cMessage *msg)
{
    if (!strcmp(msg->getName(), "sendEvent"))
    {
        Eth_pck* pck;
        if (msgQueue.size() == 0)
        {
            unsigned char destination = decideSend();
            pck = generateMessage(intuniform(0, 1), destination, 0);
        }
        else // sending msg from queue
        {
            pck = msgQueue[0];
            msgQueue.erase(msgQueue.begin());
        }
        RL->afterTransmit(pck);
        cChannel* cha = gate("out")->getTransmissionChannel();
        cDatarateChannel * cha1 = (cDatarateChannel*) cha;
        cha1->setDatarate(RL->cRate);
        send(pck, "out");
        scheduleAt(cha->getTransmissionFinishTime(), msg); //scheduling the event again exactly when the channel stops being busy

        /* statistic calculation */
        emit(dataRateSig, RL->cRate);

    }
    if (!strcmp(msg->getName(), "timeExpired"))
    {
        RL->timeExpired();
    }
}
/*
 * Description:	this function generates a message to send.
 * 				Types - 0 - general, 1- Request , 2- reply
 * 				destination- the msg destination.
 */
Eth_pck* Host::generateMessage(int type, unsigned char destination, unsigned int id)
{
    Eth_pck* pck = new Eth_pck("sending");
    unsigned int i;
    unsigned short length = par("dataLength");
    while (length < 200 || length > 1500)
    {
        length = par("dataLength");
    }
    for (i = 0; i < pck->getMacDestArraySize() - 1; i++)
    {
        pck->setMacDest(i, myMac[i]);
    }
    pck->setMacDest(i, destination);
    for (i = 0; i < pck->getMacSrcArraySize(); i++)
    {
        pck->setMacSrc(i, myMac[i]);
    }
    // creating the type of the message
    pck->setLength(length);
    pck->setByteLength(length);
    // giving the message a number
    pck->setType(type); // 0 - General, 1- Request
    pck->setKind(type);
    /* counting statistics*/
    switch (type)
    {
        case general:
            generalMsgGenCnt++;
            break;
        case request:
            if (msgIdCnt == 30000)
                msgIdCnt = 0;
            pck->setMsgNumber(msgIdCnt);
            timeStamps[msgIdCnt++] = simTime().dbl();

            requestMsgGenCnt++;
            break;
        case reply:
            pck->setMsgNumber(id);
            replyMsgGenCnt++;
            break;

    }
    return pck;
}
/*
 *	Description:	this function handles regular messages,
 *					if a request message is recieved then a reply message is prepared in the queue
 *					stats are counted here too.
 *
 */
void Host::handleRegularMsg(Eth_pck* msg)
{
    if (msg->getType() == request)
    {
        Eth_pck* pck = generateMessage(reply, msg->getMacSrc(5), msg->getMsgNumber());
        msgQueue.push_back(pck);
    }
    else if (msg->getType() == reply)
    {
        double delay = simTime().dbl() - timeStamps[msg->getMsgNumber()];
        emit(RTTSig, delay);
        replyMsgRecCnt++;
    }
}
/*
 * Description: this function decides to which host to send data
 * 				TODO need to think more about this function
 */
unsigned char Host::decideSend()
{
    int choice = par("destChoice");
    unsigned char destination;
    switch (choice)
    {
        case 0: // uniformly distributed
            destination = randArr[intuniform(0, getVectorSize() - 2)];
            break;
        case 1: // send always through obvious bottle neck
            if (getIndex() < (getVectorSize() / 2))
                destination = intuniform(getVectorSize() / 2, getVectorSize() - 1);
            else
                destination = intuniform(0, getVectorSize() / 2 - 1);
            break;

//	case 2: // never send through bottle neck
//		if (getIndex()<(getVectorSize()/2-1))
//			destination=intuniform(0,getVectorSize()/2-1);
//		else
//			destination=intuniform(getVectorSize()/2,getVectorSize()-1);
//		break;

        case 2: // first randomize uniformly then target will recieve par("consecutive") msges in a row
            if (decideCnt == 0)
            {
                decideCnt = par("consecutive");
                target = randArr[intuniform(0, getVectorSize() - 2)];
            }
            destination = target;
            decideCnt--;
            break;
    };
    return destination;
}
/*
 * Description:	the constructor of RP, initializing the RL and several other variables.
 */
RP::RP(cDatarateChannel* channel, cModule* me)
{
    state = false;
    cRate = channel->getDatarate();
    tRate = channel->getDatarate();
    MAX_DATA_RATE = channel->getDatarate();
    TXBCount = me->getAncestorPar("BC_LIMIT");
    SICount = 0;
    timer = false;
    timerSCount = 0;
    mySelf = me;
    selfTimer = new cMessage("timeExpired");
    // calculated parameters
    double percent = me->getAncestorPar("Q_EQ_STABLE_PERCENT");
    double length = me->getAncestorPar("Q_LENGTH");
    int w = me->getAncestorPar("W");
    Q_EQ = (percent * length) / 100.0;
    FB_MIN = -Q_EQ * (2 * w + 1);
    GD = 1.0 / (double) (2 * abs(FB_MIN));

}
RP::~RP()
{
    delete selfTimer;
}

void RP::FeedbackMsg(Eth_pck* msg)
{
    /* taking information from the message and deleting it when finished*/
    FeedBack * FB = check_and_cast<FeedBack*>(msg->decapsulate());
    /* info */
    if (mySelf->getIndex() == 6)
        EV << "break point" << endl;
    int fb = FB->getFb();
    double qOff = FB->getQOff();

    // checking if the rate limiter is inactive. need to be initialized
    if (state == false)
    {
        if (fb != 0 && qOff < 0)
        {
            state = true;
            cRate = MAX_DATA_RATE;
            tRate = MAX_DATA_RATE;
            SICount = 0;
        }
        else
        {
            return;
        }
    }
    // at this stage the rate limiter is already active
    if (fb != 0)
    {
        // using the current rate as the next target rate.
        // in the first cycle of fast recovery. fb<0 singal wuld ot reset the target rate.
        if (SICount != 0)
        {
            tRate = cRate;
            TXBCount = mySelf->getAncestorPar("BC_LIMIT");
        }
        // setting the stage counters
        SICount = 0;
        timerSCount = 0;

        // updating the current rate, multiplicative decrease
        double minDecFactor = mySelf->getAncestorPar("MIN_DEC_FACTOR");
        double decFactor = (1 - GD * fb);
        if (decFactor < minDecFactor)
            decFactor = minDecFactor;
        cRate = cRate * decFactor;
        double minRate = mySelf->getAncestorPar("MIN_RATE");
        minRate = minRate * pow(10, 6);
        if (cRate < minRate)
            cRate = minRate;

        bool usingTimer = mySelf->getAncestorPar("USING_TIMER");
        if (usingTimer)
        {
            double period = mySelf->getAncestorPar("TIMER_PERIOD");
            simtime_t time = period * pow(10, -3);
            Host * temp = (Host*) mySelf;
            temp->cancelEvent(selfTimer);
            temp->scheduleAt(simTime() + time, selfTimer);
            timer = true;
        }
    }
    delete FB;
}
/*
 * Description:	this function makes calculation on each frame that is about to get transmitted
 * 				increases the datarate if needed. counts how many bytes were sent, and counts stages when needed
 */
void RP::afterTransmit(Eth_pck* msg)
{

    int fastRecoveryThreshold = mySelf->getAncestorPar("FAST_RECOVERY_TH");
    double bcLimit = mySelf->getAncestorPar("BC_LIMIT");
    double expireThreshold = 0;
    // Rate limiter should be inactive if the current rate reached the maximum value
    if (cRate == MAX_DATA_RATE)
    {
        state = false;
        cRate = MAX_DATA_RATE;
        tRate = MAX_DATA_RATE;
        TXBCount = bcLimit;
        SICount = 0;
        if (timer)
        {
            Host * temp = (Host*) mySelf;
            temp->cancelAndDelete(selfTimer);
            selfTimer = new cMessage("timeExpired");
        }
        timer = false;
    }
    else
    {
        TXBCount -= msg->getByteLength() / 1000.0; // in Kbits
        if (TXBCount < 0)
        {
            SICount++;
            /* if a negative FBframe has not been received after transmitting BC_LIMIT bytes, trigger self_increase; margin of randomness 30%*/
            double temp = mySelf->par("expThr");
            if (SICount < fastRecoveryThreshold)
                expireThreshold = temp * bcLimit;
            else
                expireThreshold = temp * (bcLimit / 2.0);
            TXBCount = expireThreshold;
            selfIncrease();
        }
    }
}
/*
 * Description: this is the self increase function. this function increases
 * 				the rate of the rate limiter according to the current cycle,
 * 				hyperactive increase: if both the timer counter and byte counter are above FAST_RECOVERY_TH (which is 5)
 * 				active increase: if timer Counter or byte counter are above FAST_RECOVERY_TH
 * 				fast recovery: if both are below FAST_RECOVERY_TH
 * 				when timer is disabled hyperactive increase is not possible option.
 */
void RP::selfIncrease()
{
    int fastRecoveryThreshold = mySelf->getAncestorPar("FAST_RECOVERY_TH");
    int toCount = min(SICount, timerSCount);
    double RHai = mySelf->getAncestorPar("R_HAI");
    double Rai = mySelf->getAncestorPar("R_AI");
    RHai = RHai * pow(10, 6);
    Rai = Rai * pow(10, 6);
    double Ri = 0;
    /* if in the active probing stages, increase the target rate */
    if (SICount > fastRecoveryThreshold || timerSCount > fastRecoveryThreshold)
    {
        if (SICount > fastRecoveryThreshold && timerSCount > fastRecoveryThreshold)
        {
            /* hyperactive increase*/
            Ri = RHai * (toCount - fastRecoveryThreshold);
        }
        else /*active increase*/
        {
            Ri = Rai;
        }
    }
    else
        /*fast recovery*/
        Ri = 0;
    /* at the end of the first cycle of recovery*/
    if ((SICount == 1 || timerSCount == 1) && tRate > 10 * cRate)
    {
        tRate = tRate / 8; // reason for this --> preventing a case were tRate is WAY higher then cRate,
                           //which will later can create a significant increase in dataRate
    }
    else
    {
        tRate = tRate + Ri;
    }
    // increasing the rate
    cRate = (tRate + cRate) / 2.0;

    /* prevent increasing above the maximum possible value */
    if (cRate > MAX_DATA_RATE)
        cRate = MAX_DATA_RATE;

}
/*
 * Description: this function will launch after TIMER_PERIOD is over, this funciton increases the stage of the timer stage counter
 * 				it will also increase the rate of the host if state is true.
 */
void RP::timeExpired()
{
    int fastRecoveryThreshold = mySelf->getAncestorPar("FAST_RECOVERY_TH");
    double timerPeriod = mySelf->getAncestorPar("TIMER_PERIOD");
    timerPeriod = timerPeriod * pow(10, -3);
    if (state == true)
    {
        timerSCount++;
        selfIncrease();
        //reseting the timer
        // random margin 30%
        double temp = mySelf->getAncestorPar("expThr");
        double expirePeriod = 0;
        if (timerSCount < fastRecoveryThreshold)
        {
            expirePeriod = temp * timerPeriod;
        }
        else
        {
            expirePeriod = temp * (timerPeriod / 2.0);
        }
        // starting the timer again
        Host * hostTemp = (Host*) mySelf;
        simtime_t time = expirePeriod;
        hostTemp->scheduleAt(simTime() + time, selfTimer);
    }
}
