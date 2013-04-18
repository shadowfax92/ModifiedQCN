#ifndef __CNGCONTROL_MSGCNTRL_H_
#define __CNGCONTROL_MSGCNTRL_H_
#define MAC_SIZE 6

#include <omnetpp.h>
#include <Eth_pck_m.h>
#include <vector>
#include <string.h>
#include <string>
using namespace std;
struct tblEntry
{
        unsigned int port;
        vector<unsigned short> hostNum;
};

/*comparision function for the map
 *
 */
struct cmp_dynamic_fb_map
{
        bool operator()(unsigned char *srcMacAddress1, unsigned char *srcMacAddress2)
        {
            if (srcMacAddress1[5] > srcMacAddress2[5])
                return true;
            else
                return false;
        }
};

/**
 * TODO - Generated class
 */
/*
 * 6 character array class for MAC address
 */
struct SixCharArray
{
        SixCharArray(char in[6])
        {
            memcpy(data, in, 6);
        }
        char& operator[](unsigned int idx)
        {
            return data[idx];
        }
        char data[6];
};
struct cmp_sixchar_array_dynamic_fb_map
{
        bool operator()(unsigned char *srcMacAddress1, unsigned char *srcMacAddress2)
        {
            if (srcMacAddress1[5] > srcMacAddress2[5])
                return true;
            else
                return false;
        }
};

class MsgCntrl : public cSimpleModule
{

    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
        virtual void processSelfMsg(cMessage *msg);
        virtual void processMsg(Eth_pck *msg);
        virtual void processFbMsg(Eth_pck *msg);
        int hostsNum; //Number of Hosts connected to switch
    private:
        virtual void makeTable(const char* fileName);
        unsigned short myMac[MAC_SIZE];
        tblEntry* switchTbl;
    public:
//        std::map<char *, double> dyanimicFbSentMap;
//        map<char *, double>::iterator iteratorTemp;
        std::map<string, double> dyanimicFbSentMap;
        map<string, double>::iterator iteratorTemp;
//        std::map<char[13], double> dyanimicFbSentMap;
//        map<char[13], double>::iterator iteratorTemp;

};

#endif
