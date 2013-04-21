//
// Generated file, do not edit! Created by opp_msgc 4.2 from feedBack.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "feedBack_m.h"

// Template rule which fires if a struct or class doesn't have operator<<
template<typename T>
std::ostream& operator<<(std::ostream& out,const T&) {return out;}

// Another default rule (prevents compiler from choosing base class' doPacking())
template<typename T>
void doPacking(cCommBuffer *, T& t) {
    throw cRuntimeError("Parsim error: no doPacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}

template<typename T>
void doUnpacking(cCommBuffer *, T& t) {
    throw cRuntimeError("Parsim error: no doUnpacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}




Register_Class(FeedBack);

FeedBack::FeedBack(const char *name, int kind) : cPacket(name,kind)
{
    this->fb_var = 0;
    this->qOff_var = 0;
    this->qDelta_var = 0;
}

FeedBack::FeedBack(const FeedBack& other) : cPacket(other)
{
    copy(other);
}

FeedBack::~FeedBack()
{
}

FeedBack& FeedBack::operator=(const FeedBack& other)
{
    if (this==&other) return *this;
    cPacket::operator=(other);
    copy(other);
    return *this;
}

void FeedBack::copy(const FeedBack& other)
{
    this->fb_var = other.fb_var;
    this->qOff_var = other.qOff_var;
    this->qDelta_var = other.qDelta_var;
}

void FeedBack::parsimPack(cCommBuffer *b)
{
    cPacket::parsimPack(b);
    doPacking(b,this->fb_var);
    doPacking(b,this->qOff_var);
    doPacking(b,this->qDelta_var);
}

void FeedBack::parsimUnpack(cCommBuffer *b)
{
    cPacket::parsimUnpack(b);
    doUnpacking(b,this->fb_var);
    doUnpacking(b,this->qOff_var);
    doUnpacking(b,this->qDelta_var);
}

unsigned int FeedBack::getFb() const
{
    return fb_var;
}

void FeedBack::setFb(unsigned int fb)
{
    this->fb_var = fb;
}

double FeedBack::getQOff() const
{
    return qOff_var;
}

void FeedBack::setQOff(double qOff)
{
    this->qOff_var = qOff;
}

double FeedBack::getQDelta() const
{
    return qDelta_var;
}

void FeedBack::setQDelta(double qDelta)
{
    this->qDelta_var = qDelta;
}

class FeedBackDescriptor : public cClassDescriptor
{
  public:
    FeedBackDescriptor();
    virtual ~FeedBackDescriptor();

    virtual bool doesSupport(cObject *obj) const;
    virtual const char *getProperty(const char *propertyname) const;
    virtual int getFieldCount(void *object) const;
    virtual const char *getFieldName(void *object, int field) const;
    virtual int findField(void *object, const char *fieldName) const;
    virtual unsigned int getFieldTypeFlags(void *object, int field) const;
    virtual const char *getFieldTypeString(void *object, int field) const;
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const;
    virtual int getArraySize(void *object, int field) const;

    virtual std::string getFieldAsString(void *object, int field, int i) const;
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const;

    virtual const char *getFieldStructName(void *object, int field) const;
    virtual void *getFieldStructPointer(void *object, int field, int i) const;
};

Register_ClassDescriptor(FeedBackDescriptor);

FeedBackDescriptor::FeedBackDescriptor() : cClassDescriptor("FeedBack", "cPacket")
{
}

FeedBackDescriptor::~FeedBackDescriptor()
{
}

bool FeedBackDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<FeedBack *>(obj)!=NULL;
}

const char *FeedBackDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int FeedBackDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 3+basedesc->getFieldCount(object) : 3;
}

unsigned int FeedBackDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<3) ? fieldTypeFlags[field] : 0;
}

const char *FeedBackDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "fb",
        "qOff",
        "qDelta",
    };
    return (field>=0 && field<3) ? fieldNames[field] : NULL;
}

int FeedBackDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='f' && strcmp(fieldName, "fb")==0) return base+0;
    if (fieldName[0]=='q' && strcmp(fieldName, "qOff")==0) return base+1;
    if (fieldName[0]=='q' && strcmp(fieldName, "qDelta")==0) return base+2;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *FeedBackDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "unsigned int",
        "double",
        "double",
    };
    return (field>=0 && field<3) ? fieldTypeStrings[field] : NULL;
}

const char *FeedBackDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    }
}

int FeedBackDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    FeedBack *pp = (FeedBack *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string FeedBackDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    FeedBack *pp = (FeedBack *)object; (void)pp;
    switch (field) {
        case 0: return ulong2string(pp->getFb());
        case 1: return double2string(pp->getQOff());
        case 2: return double2string(pp->getQDelta());
        default: return "";
    }
}

bool FeedBackDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    FeedBack *pp = (FeedBack *)object; (void)pp;
    switch (field) {
        case 0: pp->setFb(string2ulong(value)); return true;
        case 1: pp->setQOff(string2double(value)); return true;
        case 2: pp->setQDelta(string2double(value)); return true;
        default: return false;
    }
}

const char *FeedBackDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldStructNames[] = {
        NULL,
        NULL,
        NULL,
    };
    return (field>=0 && field<3) ? fieldStructNames[field] : NULL;
}

void *FeedBackDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    FeedBack *pp = (FeedBack *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}


