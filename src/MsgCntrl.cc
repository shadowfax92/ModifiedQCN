#include "MsgCntrl.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#define FEEDBACK 1600
#define DELAY_BOUND 0.000020 //In simulation time
Define_Module(MsgCntrl);

void MsgCntrl::initialize()
{
    //Getting index of parent module for MAC address
    for (int i = 0; i < MAC_SIZE; i++)
    {
        if (i == 4)
            myMac[i] = getParentModule()->getIndex();
        else
            myMac[i] = 0xFF;
    }
    string temp = getParentModule()->par("tableFile");
    char* fileName = new char[10];
    sprintf(fileName, "%s%d", temp.c_str(), myMac[4]);
    cGate *g = gate("in", 0);
    hostsNum = g->getVectorSize();
    switchTbl = new tblEntry[hostsNum];
    makeTable(fileName);

}

/*
 * Description: This Function makes switch table from files
 */
void MsgCntrl::makeTable(const char* fileName)
{
    FILE* fStr = fopen(fileName, "r");
    int tPort;
    char* tHost = new char[4]; //max 255 host for network
    for (int i = 0; i < hostsNum; i++)
    {
        fscanf(fStr, "%d", &tPort);
        switchTbl[i].port = tPort;
        fscanf(fStr, "%s", tHost);
        while (strcmp(tHost, "$"))
        {
            switchTbl[i].hostNum.push_back(atoi(tHost));
            fscanf(fStr, "%s", tHost);
        }
    }
    for (int j = 0; j < 4; j++)
    {
        EV << "host: " << switchTbl[4].hostNum[j] << endl;
    }
    fclose(fStr);
    delete tHost;

}
/*
 *	Description: this function handles self messages
 */
void MsgCntrl::processSelfMsg(cMessage *msg)
{
    //TODO optional for later use

}
/*
 * Description: This function forwards the general not FB message according to matching
 * between MAC address and switch table entry. If destination MAC address will not be found in
 * the table the packet will be dropped
 */
void MsgCntrl::processMsg(Eth_pck *msg)
{
    int i, j;
    bool portFound = false;
    int mac = msg->getMacDest(5);
    //Searching destination MAC address in switch table
    for (i = 0; i < hostsNum && !portFound; i++)
    {
        for (j = 0; j < (int) switchTbl[i].hostNum.size() && !portFound; j++)
        {
            if (mac == switchTbl[i].hostNum[j])
            {
                int port = switchTbl[i].port;
                portFound = true;
                send(msg, "mc$o", port);
            }
        }
    }
    if (!portFound)
    {
        EV << "MsgCntr: Destenation mac address not in the table the message will be droped" << endl;
        delete msg;
    }
}
/*
 * Description: message that was received from the CP
 */
void MsgCntrl::processFbMsg(Eth_pck *msg)
{
    //debug print message array
    char print_msg[50];

    string destMacAddressString;

    for (unsigned int i = 0; i < msg->getMacSrcArraySize(); i++)
    {
        msg->setMacSrc(i, myMac[i]);
    }
    msg->setLength(FEEDBACK);
    //processMsg(msg);

    /*
     * dynamic tables solution
     */
    //getting MAC address for the dynamic table
    unsigned char *destMacAddress = new unsigned char[6];
    for (int i = 0; i < msg->getMacDestArraySize(); ++i)
    {
        destMacAddress[i] = msg->getMacDest(i);

    }

    //converting MAC unsigned char to char
    int bufferSize = 6;
    char converted[13];
    int i;
    for (i = 0; i < bufferSize; i++)
    {
        sprintf(&converted[i * 2], "%02X", destMacAddress[i]);
    }
    destMacAddressString = converted;

    //debug messages
    sprintf(print_msg, "destMac=%s\n", converted);
    //bubble(print_msg);
    EV << print_msg << "\n";

    double previous_sim_time;
    double current_sim_time = simTime().dbl();
    if (dyanimicFbSentMap.find(destMacAddressString) == dyanimicFbSentMap.end())
    {
        dyanimicFbSentMap.insert(std::make_pair(destMacAddressString, current_sim_time));
        processMsg(msg);

        //debug messages
        sprintf(print_msg, "FIRST destMac=%s\n", converted);
        bubble(print_msg);
        sprintf(print_msg, "FIRST destMac=%s current_time=%lf\n", converted, current_sim_time);
        EV << "\nMsgCntrl.cc::ProcessFbMsg:" << print_msg;
    }
    else
    {
        iteratorTemp = dyanimicFbSentMap.find(converted);
        previous_sim_time = iteratorTemp->second;
        double delay_double = current_sim_time - previous_sim_time;

        //debug messages
        sprintf(print_msg, "SECOND destMac=%s\n", converted);
        bubble(print_msg);
        sprintf(print_msg, "FIRST destMac=%s\n current_time=%lf delay=%lf", converted, current_sim_time, delay_double);
        EV << "\nMsgCntrl.cc::ProcessFbMsg:" << print_msg;

        if (delay_double > DELAY_BOUND)
        {
            dyanimicFbSentMap.insert(std::make_pair(destMacAddressString, current_sim_time));
            processMsg(msg);

            //debug messages
            sprintf(print_msg, "UPDATE destMac=%s\n", converted);
            bubble(print_msg);
            sprintf(print_msg, "UPDATE destMac=%s current_time=%lf\n", converted, current_sim_time);
            EV << "\nMsgCntrl.cc::ProcessFbMsg:" << print_msg;

        }
        else
        {
            //drop it
        }
    }
    //print the entire map
    for (iteratorTemp = dyanimicFbSentMap.begin(); iteratorTemp != dyanimicFbSentMap.end(); ++iteratorTemp)
    {
        string address = iteratorTemp->first;
        double time = iteratorTemp->second;
        sprintf(print_msg, "destaddress - %s time - %lf", address.c_str(), time);
        EV << "\nDynamic table values:" << print_msg;
    }
}
/*
 * Description:	this function, handles a messages from
 */
void MsgCntrl::handleMessage(cMessage *msg)
{
    char print_msg[50];

    if (msg->isSelfMessage())
        processSelfMsg(msg);
    else // message arrived from Message Control
    {
        Eth_pck *message = check_and_cast<Eth_pck *>(msg);

        sprintf(print_msg, "MsgCntrl: handle message length=%d", message->getLength());
        //bubble(print_msg);
        /*
         * Nikhil:changed the below line
         */
        //if (message->getLength()==1601)//no MAC source address
        if (message->getLength() == FEEDBACK) //no MAC source address
            processFbMsg(message); //need to fill source and dest MAC addresses
        else
            processMsg(message); //process general message
    }
}
