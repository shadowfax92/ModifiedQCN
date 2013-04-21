//
// Generated file, do not edit! Created by opp_msgc 4.2 from feedBack.msg.
//

#ifndef _FEEDBACK_M_H_
#define _FEEDBACK_M_H_

#include <omnetpp.h>

// opp_msgc version check
#define MSGC_VERSION 0x0402
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of opp_msgc: 'make clean' should help.
#endif



/**
 * Class generated from <tt>feedBack.msg</tt> by opp_msgc.
 * <pre>
 * packet FeedBack {
 *     unsigned int fb;
 *     double qOff;
 *     double qDelta;
 * }
 * </pre>
 */
class FeedBack : public ::cPacket
{
  protected:
    unsigned int fb_var;
    double qOff_var;
    double qDelta_var;

  private:
    void copy(const FeedBack& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const FeedBack&);

  public:
    FeedBack(const char *name=NULL, int kind=0);
    FeedBack(const FeedBack& other);
    virtual ~FeedBack();
    FeedBack& operator=(const FeedBack& other);
    virtual FeedBack *dup() const {return new FeedBack(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual unsigned int getFb() const;
    virtual void setFb(unsigned int fb);
    virtual double getQOff() const;
    virtual void setQOff(double qOff);
    virtual double getQDelta() const;
    virtual void setQDelta(double qDelta);
};

inline void doPacking(cCommBuffer *b, FeedBack& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, FeedBack& obj) {obj.parsimUnpack(b);}


#endif // _FEEDBACK_M_H_
