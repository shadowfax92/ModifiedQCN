#ifndef __CNGCONTROL_HOST_H_
#define __CNGCONTROL_HOST_H_

#include <omnetpp.h>
#include <string.h>
#include <stdio.h>
#include <vector>
#include "Eth_pck_m.h"
#define MAX_ID_NUM 30000
using namespace std;
typedef struct msgStore {
    Eth_pck* msg;
    cMessage* self;
} msgStore;

typedef enum types {
    general, request, reply
} types;

class RP {
public:
    /*
     * Description:	this function initializes the RL
     */
    RP(cDatarateChannel* channel, cModule* me);
    /*
     * Description: the destructor for deleting stuff if needed
     */
    virtual ~RP();
    /*
     * those are the variables of the QCN.
     */
    /*
     * Description: this function handles a feedback msg
     */
    virtual void FeedbackMsg(Eth_pck* msg);
    /*
     * Description: this function calculates the stage state of the RL and increases rate if necessary
     */
    virtual void afterTransmit(Eth_pck* msg);
    /*
     * Description: this function increases the rate according to information in fb
     */
    virtual void selfIncrease();
    /*
     * Description:	this function is called when the timer finish counting to 10ms
     */
    virtual void timeExpired();

    double getMaxDataRate() {
        return MAX_DATA_RATE;
    }
    bool state;
    double cRate;
    double tRate;
    double TXBCount; //Byte counter
    int SICount; //stage counter
    bool timer;
    int timerSCount;
    cModule* mySelf;
    cMessage* selfTimer;

    int host_id;
private:
    double MAX_DATA_RATE;
    double Q_EQ;
    double FB_MIN;
    double GD;
};

class Host: public cSimpleModule {
public:
    simsignal_t RTTSig;
    simsignal_t dataRateSig;
    int host_id;
protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void processMsgFromLowerLayer(Eth_pck *packet);
    virtual void processSelfTimer(cMessage *msg);
    unsigned char *myMac; // will hold my MAC address
    virtual Eth_pck* generateMessage(int type, unsigned char destination,
            unsigned int id);
    virtual void handleRegularMsg(Eth_pck* msg);
    virtual unsigned char decideSend();
private:
    // description on what those functions do on c file
    int *randArr; // used for randoming
    vector<Eth_pck*> msgQueue; // messages are stored here if channel is busy
    void sendMessage(Eth_pck* etherPacket, const char * gateName);
    unsigned int msgIdCnt;
    RP* RL;
    unsigned int decideCnt;
    unsigned char target;
    /* statistics counters*/
    unsigned long int requestMsgGenCnt;
    unsigned long int replyMsgGenCnt;
    unsigned long int generalMsgGenCnt;
    unsigned long int replyMsgRecCnt;
    /* statistics help*/
    double timeStamps[MAX_ID_NUM];

};

#endif
