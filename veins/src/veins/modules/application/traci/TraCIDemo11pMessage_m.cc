//
// Generated file, do not edit! Created by nedtool 5.5 from veins/modules/application/traci/TraCIDemo11pMessage.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wshadow"
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wunused-parameter"
#  pragma clang diagnostic ignored "-Wc++98-compat"
#  pragma clang diagnostic ignored "-Wunreachable-code-break"
#  pragma clang diagnostic ignored "-Wold-style-cast"
#elif defined(__GNUC__)
#  pragma GCC diagnostic ignored "-Wshadow"
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
#  pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

#include <iostream>
#include <sstream>
#include "TraCIDemo11pMessage_m.h"

namespace omnetpp {

// Template pack/unpack rules. They are declared *after* a1l type-specific pack functions for multiple reasons.
// They are in the omnetpp namespace, to allow them to be found by argument-dependent lookup via the cCommBuffer argument

// Packing/unpacking an std::vector
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::vector<T,A>& v)
{
    int n = v.size();
    doParsimPacking(buffer, n);
    for (int i = 0; i < n; i++)
        doParsimPacking(buffer, v[i]);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::vector<T,A>& v)
{
    int n;
    doParsimUnpacking(buffer, n);
    v.resize(n);
    for (int i = 0; i < n; i++)
        doParsimUnpacking(buffer, v[i]);
}

// Packing/unpacking an std::list
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::list<T,A>& l)
{
    doParsimPacking(buffer, (int)l.size());
    for (typename std::list<T,A>::const_iterator it = l.begin(); it != l.end(); ++it)
        doParsimPacking(buffer, (T&)*it);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::list<T,A>& l)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i=0; i<n; i++) {
        l.push_back(T());
        doParsimUnpacking(buffer, l.back());
    }
}

// Packing/unpacking an std::set
template<typename T, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::set<T,Tr,A>& s)
{
    doParsimPacking(buffer, (int)s.size());
    for (typename std::set<T,Tr,A>::const_iterator it = s.begin(); it != s.end(); ++it)
        doParsimPacking(buffer, *it);
}

template<typename T, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::set<T,Tr,A>& s)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i=0; i<n; i++) {
        T x;
        doParsimUnpacking(buffer, x);
        s.insert(x);
    }
}

// Packing/unpacking an std::map
template<typename K, typename V, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::map<K,V,Tr,A>& m)
{
    doParsimPacking(buffer, (int)m.size());
    for (typename std::map<K,V,Tr,A>::const_iterator it = m.begin(); it != m.end(); ++it) {
        doParsimPacking(buffer, it->first);
        doParsimPacking(buffer, it->second);
    }
}

template<typename K, typename V, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::map<K,V,Tr,A>& m)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i=0; i<n; i++) {
        K k; V v;
        doParsimUnpacking(buffer, k);
        doParsimUnpacking(buffer, v);
        m[k] = v;
    }
}

// Default pack/unpack function for arrays
template<typename T>
void doParsimArrayPacking(omnetpp::cCommBuffer *b, const T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimPacking(b, t[i]);
}

template<typename T>
void doParsimArrayUnpacking(omnetpp::cCommBuffer *b, T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimUnpacking(b, t[i]);
}

// Default rule to prevent compiler from choosing base class' doParsimPacking() function
template<typename T>
void doParsimPacking(omnetpp::cCommBuffer *, const T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimPacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

template<typename T>
void doParsimUnpacking(omnetpp::cCommBuffer *, T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimUnpacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

}  // namespace omnetpp

namespace veins {

// forward
template<typename T, typename A>
std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec);

// Template rule which fires if a struct or class doesn't have operator<<
template<typename T>
inline std::ostream& operator<<(std::ostream& out,const T&) {return out;}

// operator<< for std::vector<T>
template<typename T, typename A>
inline std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec)
{
    out.put('{');
    for(typename std::vector<T,A>::const_iterator it = vec.begin(); it != vec.end(); ++it)
    {
        if (it != vec.begin()) {
            out.put(','); out.put(' ');
        }
        out << *it;
    }
    out.put('}');
    
    char buf[32];
    sprintf(buf, " (size=%u)", (unsigned int)vec.size());
    out.write(buf, strlen(buf));
    return out;
}

Register_Class(TraCIDemo11pMessage)

TraCIDemo11pMessage::TraCIDemo11pMessage(const char *name, short kind) : ::veins::BaseFrame1609_4(name,kind)
{
    this->senderAddress = -1;
    this->serial = 0;
    this->senderSpeedRLDCO = 0;
    this->hopCountRLDCO = 0;
    this->SenderType = 0;
    this->AvailableResource = 0;
    this->AvailableStorage = 0;
    this->testData = 0;
    this->TargetAddress = 0;
    this->SenderVelocity = 0;
    this->AverageSpeed = 0;
    this->MessageTime = 0;
    this->TimeToReach = 0;
    this->DwellTime = 0;
    this->DwellDistance = 0;
    this->MessageInitTime = 0;
    this->RequestDeadline = 0;
    this->ServiceTime = 0;
    this->Requester = false;
    this->InRange = false;
}

TraCIDemo11pMessage::TraCIDemo11pMessage(const TraCIDemo11pMessage& other) : ::veins::BaseFrame1609_4(other)
{
    copy(other);
}

TraCIDemo11pMessage::~TraCIDemo11pMessage()
{
}

TraCIDemo11pMessage& TraCIDemo11pMessage::operator=(const TraCIDemo11pMessage& other)
{
    if (this==&other) return *this;
    ::veins::BaseFrame1609_4::operator=(other);
    copy(other);
    return *this;
}

void TraCIDemo11pMessage::copy(const TraCIDemo11pMessage& other)
{
    this->senderAddress = other.senderAddress;
    this->serial = other.serial;
    this->senderSpeedRLDCO = other.senderSpeedRLDCO;
    this->senderPositionRLDCO = other.senderPositionRLDCO;
    this->hopCountRLDCO = other.hopCountRLDCO;
    this->SenderType = other.SenderType;
    this->AvailableResource = other.AvailableResource;
    this->AvailableStorage = other.AvailableStorage;
    this->testData = other.testData;
    this->TargetAddress = other.TargetAddress;
    this->TargetCoord = other.TargetCoord;
    this->EntryCoord = other.EntryCoord;
    this->ExitCoord = other.ExitCoord;
    this->SenderVelocity = other.SenderVelocity;
    this->AverageSpeed = other.AverageSpeed;
    this->MessageTime = other.MessageTime;
    this->TimeToReach = other.TimeToReach;
    this->DwellTime = other.DwellTime;
    this->DwellDistance = other.DwellDistance;
    this->NeighborChain = other.NeighborChain;
    this->MessageInitTime = other.MessageInitTime;
    this->RequestDeadline = other.RequestDeadline;
    this->ServiceTime = other.ServiceTime;
    this->Requester = other.Requester;
    this->InRange = other.InRange;
}

void TraCIDemo11pMessage::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::veins::BaseFrame1609_4::parsimPack(b);
    doParsimPacking(b,this->senderAddress);
    doParsimPacking(b,this->serial);
    doParsimPacking(b,this->senderSpeedRLDCO);
    doParsimPacking(b,this->senderPositionRLDCO);
    doParsimPacking(b,this->hopCountRLDCO);
    doParsimPacking(b,this->SenderType);
    doParsimPacking(b,this->AvailableResource);
    doParsimPacking(b,this->AvailableStorage);
    doParsimPacking(b,this->testData);
    doParsimPacking(b,this->TargetAddress);
    doParsimPacking(b,this->TargetCoord);
    doParsimPacking(b,this->EntryCoord);
    doParsimPacking(b,this->ExitCoord);
    doParsimPacking(b,this->SenderVelocity);
    doParsimPacking(b,this->AverageSpeed);
    doParsimPacking(b,this->MessageTime);
    doParsimPacking(b,this->TimeToReach);
    doParsimPacking(b,this->DwellTime);
    doParsimPacking(b,this->DwellDistance);
    doParsimPacking(b,this->NeighborChain);
    doParsimPacking(b,this->MessageInitTime);
    doParsimPacking(b,this->RequestDeadline);
    doParsimPacking(b,this->ServiceTime);
    doParsimPacking(b,this->Requester);
    doParsimPacking(b,this->InRange);
}

void TraCIDemo11pMessage::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::veins::BaseFrame1609_4::parsimUnpack(b);
    doParsimUnpacking(b,this->senderAddress);
    doParsimUnpacking(b,this->serial);
    doParsimUnpacking(b,this->senderSpeedRLDCO);
    doParsimUnpacking(b,this->senderPositionRLDCO);
    doParsimUnpacking(b,this->hopCountRLDCO);
    doParsimUnpacking(b,this->SenderType);
    doParsimUnpacking(b,this->AvailableResource);
    doParsimUnpacking(b,this->AvailableStorage);
    doParsimUnpacking(b,this->testData);
    doParsimUnpacking(b,this->TargetAddress);
    doParsimUnpacking(b,this->TargetCoord);
    doParsimUnpacking(b,this->EntryCoord);
    doParsimUnpacking(b,this->ExitCoord);
    doParsimUnpacking(b,this->SenderVelocity);
    doParsimUnpacking(b,this->AverageSpeed);
    doParsimUnpacking(b,this->MessageTime);
    doParsimUnpacking(b,this->TimeToReach);
    doParsimUnpacking(b,this->DwellTime);
    doParsimUnpacking(b,this->DwellDistance);
    doParsimUnpacking(b,this->NeighborChain);
    doParsimUnpacking(b,this->MessageInitTime);
    doParsimUnpacking(b,this->RequestDeadline);
    doParsimUnpacking(b,this->ServiceTime);
    doParsimUnpacking(b,this->Requester);
    doParsimUnpacking(b,this->InRange);
}

LAddress::L2Type& TraCIDemo11pMessage::getSenderAddress()
{
    return this->senderAddress;
}

void TraCIDemo11pMessage::setSenderAddress(const LAddress::L2Type& senderAddress)
{
    this->senderAddress = senderAddress;
}

int TraCIDemo11pMessage::getSerial() const
{
    return this->serial;
}

void TraCIDemo11pMessage::setSerial(int serial)
{
    this->serial = serial;
}

double TraCIDemo11pMessage::getSenderSpeedRLDCO() const
{
    return this->senderSpeedRLDCO;
}

void TraCIDemo11pMessage::setSenderSpeedRLDCO(double senderSpeedRLDCO)
{
    this->senderSpeedRLDCO = senderSpeedRLDCO;
}

Coord& TraCIDemo11pMessage::getSenderPositionRLDCO()
{
    return this->senderPositionRLDCO;
}

void TraCIDemo11pMessage::setSenderPositionRLDCO(const Coord& senderPositionRLDCO)
{
    this->senderPositionRLDCO = senderPositionRLDCO;
}

int TraCIDemo11pMessage::getHopCountRLDCO() const
{
    return this->hopCountRLDCO;
}

void TraCIDemo11pMessage::setHopCountRLDCO(int hopCountRLDCO)
{
    this->hopCountRLDCO = hopCountRLDCO;
}

int TraCIDemo11pMessage::getSenderType() const
{
    return this->SenderType;
}

void TraCIDemo11pMessage::setSenderType(int SenderType)
{
    this->SenderType = SenderType;
}

int TraCIDemo11pMessage::getAvailableResource() const
{
    return this->AvailableResource;
}

void TraCIDemo11pMessage::setAvailableResource(int AvailableResource)
{
    this->AvailableResource = AvailableResource;
}

int TraCIDemo11pMessage::getAvailableStorage() const
{
    return this->AvailableStorage;
}

void TraCIDemo11pMessage::setAvailableStorage(int AvailableStorage)
{
    this->AvailableStorage = AvailableStorage;
}

int TraCIDemo11pMessage::getTestData() const
{
    return this->testData;
}

void TraCIDemo11pMessage::setTestData(int testData)
{
    this->testData = testData;
}

int TraCIDemo11pMessage::getTargetAddress() const
{
    return this->TargetAddress;
}

void TraCIDemo11pMessage::setTargetAddress(int TargetAddress)
{
    this->TargetAddress = TargetAddress;
}

Coord& TraCIDemo11pMessage::getTargetCoord()
{
    return this->TargetCoord;
}

void TraCIDemo11pMessage::setTargetCoord(const Coord& TargetCoord)
{
    this->TargetCoord = TargetCoord;
}

Coord& TraCIDemo11pMessage::getEntryCoord()
{
    return this->EntryCoord;
}

void TraCIDemo11pMessage::setEntryCoord(const Coord& EntryCoord)
{
    this->EntryCoord = EntryCoord;
}

Coord& TraCIDemo11pMessage::getExitCoord()
{
    return this->ExitCoord;
}

void TraCIDemo11pMessage::setExitCoord(const Coord& ExitCoord)
{
    this->ExitCoord = ExitCoord;
}

double TraCIDemo11pMessage::getSenderVelocity() const
{
    return this->SenderVelocity;
}

void TraCIDemo11pMessage::setSenderVelocity(double SenderVelocity)
{
    this->SenderVelocity = SenderVelocity;
}

double TraCIDemo11pMessage::getAverageSpeed() const
{
    return this->AverageSpeed;
}

void TraCIDemo11pMessage::setAverageSpeed(double AverageSpeed)
{
    this->AverageSpeed = AverageSpeed;
}

int TraCIDemo11pMessage::getMessageTime() const
{
    return this->MessageTime;
}

void TraCIDemo11pMessage::setMessageTime(int MessageTime)
{
    this->MessageTime = MessageTime;
}

int TraCIDemo11pMessage::getTimeToReach() const
{
    return this->TimeToReach;
}

void TraCIDemo11pMessage::setTimeToReach(int TimeToReach)
{
    this->TimeToReach = TimeToReach;
}

int TraCIDemo11pMessage::getDwellTime() const
{
    return this->DwellTime;
}

void TraCIDemo11pMessage::setDwellTime(int DwellTime)
{
    this->DwellTime = DwellTime;
}

double TraCIDemo11pMessage::getDwellDistance() const
{
    return this->DwellDistance;
}

void TraCIDemo11pMessage::setDwellDistance(double DwellDistance)
{
    this->DwellDistance = DwellDistance;
}

const char * TraCIDemo11pMessage::getNeighborChain() const
{
    return this->NeighborChain.c_str();
}

void TraCIDemo11pMessage::setNeighborChain(const char * NeighborChain)
{
    this->NeighborChain = NeighborChain;
}

double TraCIDemo11pMessage::getMessageInitTime() const
{
    return this->MessageInitTime;
}

void TraCIDemo11pMessage::setMessageInitTime(double MessageInitTime)
{
    this->MessageInitTime = MessageInitTime;
}

int TraCIDemo11pMessage::getRequestDeadline() const
{
    return this->RequestDeadline;
}

void TraCIDemo11pMessage::setRequestDeadline(int RequestDeadline)
{
    this->RequestDeadline = RequestDeadline;
}

int TraCIDemo11pMessage::getServiceTime() const
{
    return this->ServiceTime;
}

void TraCIDemo11pMessage::setServiceTime(int ServiceTime)
{
    this->ServiceTime = ServiceTime;
}

bool TraCIDemo11pMessage::getRequester() const
{
    return this->Requester;
}

void TraCIDemo11pMessage::setRequester(bool Requester)
{
    this->Requester = Requester;
}

bool TraCIDemo11pMessage::getInRange() const
{
    return this->InRange;
}

void TraCIDemo11pMessage::setInRange(bool InRange)
{
    this->InRange = InRange;
}

class TraCIDemo11pMessageDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    TraCIDemo11pMessageDescriptor();
    virtual ~TraCIDemo11pMessageDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyname) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyname) const override;
    virtual int getFieldArraySize(void *object, int field) const override;

    virtual const char *getFieldDynamicTypeString(void *object, int field, int i) const override;
    virtual std::string getFieldValueAsString(void *object, int field, int i) const override;
    virtual bool setFieldValueAsString(void *object, int field, int i, const char *value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual void *getFieldStructValuePointer(void *object, int field, int i) const override;
};

Register_ClassDescriptor(TraCIDemo11pMessageDescriptor)

TraCIDemo11pMessageDescriptor::TraCIDemo11pMessageDescriptor() : omnetpp::cClassDescriptor("veins::TraCIDemo11pMessage", "veins::BaseFrame1609_4")
{
    propertynames = nullptr;
}

TraCIDemo11pMessageDescriptor::~TraCIDemo11pMessageDescriptor()
{
    delete[] propertynames;
}

bool TraCIDemo11pMessageDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<TraCIDemo11pMessage *>(obj)!=nullptr;
}

const char **TraCIDemo11pMessageDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *TraCIDemo11pMessageDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int TraCIDemo11pMessageDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 25+basedesc->getFieldCount() : 25;
}

unsigned int TraCIDemo11pMessageDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISCOMPOUND,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISCOMPOUND,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISCOMPOUND,
        FD_ISCOMPOUND,
        FD_ISCOMPOUND,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<25) ? fieldTypeFlags[field] : 0;
}

const char *TraCIDemo11pMessageDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "senderAddress",
        "serial",
        "senderSpeedRLDCO",
        "senderPositionRLDCO",
        "hopCountRLDCO",
        "SenderType",
        "AvailableResource",
        "AvailableStorage",
        "testData",
        "TargetAddress",
        "TargetCoord",
        "EntryCoord",
        "ExitCoord",
        "SenderVelocity",
        "AverageSpeed",
        "MessageTime",
        "TimeToReach",
        "DwellTime",
        "DwellDistance",
        "NeighborChain",
        "MessageInitTime",
        "RequestDeadline",
        "ServiceTime",
        "Requester",
        "InRange",
    };
    return (field>=0 && field<25) ? fieldNames[field] : nullptr;
}

int TraCIDemo11pMessageDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='s' && strcmp(fieldName, "senderAddress")==0) return base+0;
    if (fieldName[0]=='s' && strcmp(fieldName, "serial")==0) return base+1;
    if (fieldName[0]=='s' && strcmp(fieldName, "senderSpeedRLDCO")==0) return base+2;
    if (fieldName[0]=='s' && strcmp(fieldName, "senderPositionRLDCO")==0) return base+3;
    if (fieldName[0]=='h' && strcmp(fieldName, "hopCountRLDCO")==0) return base+4;
    if (fieldName[0]=='S' && strcmp(fieldName, "SenderType")==0) return base+5;
    if (fieldName[0]=='A' && strcmp(fieldName, "AvailableResource")==0) return base+6;
    if (fieldName[0]=='A' && strcmp(fieldName, "AvailableStorage")==0) return base+7;
    if (fieldName[0]=='t' && strcmp(fieldName, "testData")==0) return base+8;
    if (fieldName[0]=='T' && strcmp(fieldName, "TargetAddress")==0) return base+9;
    if (fieldName[0]=='T' && strcmp(fieldName, "TargetCoord")==0) return base+10;
    if (fieldName[0]=='E' && strcmp(fieldName, "EntryCoord")==0) return base+11;
    if (fieldName[0]=='E' && strcmp(fieldName, "ExitCoord")==0) return base+12;
    if (fieldName[0]=='S' && strcmp(fieldName, "SenderVelocity")==0) return base+13;
    if (fieldName[0]=='A' && strcmp(fieldName, "AverageSpeed")==0) return base+14;
    if (fieldName[0]=='M' && strcmp(fieldName, "MessageTime")==0) return base+15;
    if (fieldName[0]=='T' && strcmp(fieldName, "TimeToReach")==0) return base+16;
    if (fieldName[0]=='D' && strcmp(fieldName, "DwellTime")==0) return base+17;
    if (fieldName[0]=='D' && strcmp(fieldName, "DwellDistance")==0) return base+18;
    if (fieldName[0]=='N' && strcmp(fieldName, "NeighborChain")==0) return base+19;
    if (fieldName[0]=='M' && strcmp(fieldName, "MessageInitTime")==0) return base+20;
    if (fieldName[0]=='R' && strcmp(fieldName, "RequestDeadline")==0) return base+21;
    if (fieldName[0]=='S' && strcmp(fieldName, "ServiceTime")==0) return base+22;
    if (fieldName[0]=='R' && strcmp(fieldName, "Requester")==0) return base+23;
    if (fieldName[0]=='I' && strcmp(fieldName, "InRange")==0) return base+24;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *TraCIDemo11pMessageDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "LAddress::L2Type",
        "int",
        "double",
        "Coord",
        "int",
        "int",
        "int",
        "int",
        "int",
        "int",
        "Coord",
        "Coord",
        "Coord",
        "double",
        "double",
        "int",
        "int",
        "int",
        "double",
        "string",
        "double",
        "int",
        "int",
        "bool",
        "bool",
    };
    return (field>=0 && field<25) ? fieldTypeStrings[field] : nullptr;
}

const char **TraCIDemo11pMessageDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldPropertyNames(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *TraCIDemo11pMessageDescriptor::getFieldProperty(int field, const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldProperty(field, propertyname);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int TraCIDemo11pMessageDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    TraCIDemo11pMessage *pp = (TraCIDemo11pMessage *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

const char *TraCIDemo11pMessageDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    TraCIDemo11pMessage *pp = (TraCIDemo11pMessage *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string TraCIDemo11pMessageDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    TraCIDemo11pMessage *pp = (TraCIDemo11pMessage *)object; (void)pp;
    switch (field) {
        case 0: {std::stringstream out; out << pp->getSenderAddress(); return out.str();}
        case 1: return long2string(pp->getSerial());
        case 2: return double2string(pp->getSenderSpeedRLDCO());
        case 3: {std::stringstream out; out << pp->getSenderPositionRLDCO(); return out.str();}
        case 4: return long2string(pp->getHopCountRLDCO());
        case 5: return long2string(pp->getSenderType());
        case 6: return long2string(pp->getAvailableResource());
        case 7: return long2string(pp->getAvailableStorage());
        case 8: return long2string(pp->getTestData());
        case 9: return long2string(pp->getTargetAddress());
        case 10: {std::stringstream out; out << pp->getTargetCoord(); return out.str();}
        case 11: {std::stringstream out; out << pp->getEntryCoord(); return out.str();}
        case 12: {std::stringstream out; out << pp->getExitCoord(); return out.str();}
        case 13: return double2string(pp->getSenderVelocity());
        case 14: return double2string(pp->getAverageSpeed());
        case 15: return long2string(pp->getMessageTime());
        case 16: return long2string(pp->getTimeToReach());
        case 17: return long2string(pp->getDwellTime());
        case 18: return double2string(pp->getDwellDistance());
        case 19: return oppstring2string(pp->getNeighborChain());
        case 20: return double2string(pp->getMessageInitTime());
        case 21: return long2string(pp->getRequestDeadline());
        case 22: return long2string(pp->getServiceTime());
        case 23: return bool2string(pp->getRequester());
        case 24: return bool2string(pp->getInRange());
        default: return "";
    }
}

bool TraCIDemo11pMessageDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    TraCIDemo11pMessage *pp = (TraCIDemo11pMessage *)object; (void)pp;
    switch (field) {
        case 1: pp->setSerial(string2long(value)); return true;
        case 2: pp->setSenderSpeedRLDCO(string2double(value)); return true;
        case 4: pp->setHopCountRLDCO(string2long(value)); return true;
        case 5: pp->setSenderType(string2long(value)); return true;
        case 6: pp->setAvailableResource(string2long(value)); return true;
        case 7: pp->setAvailableStorage(string2long(value)); return true;
        case 8: pp->setTestData(string2long(value)); return true;
        case 9: pp->setTargetAddress(string2long(value)); return true;
        case 13: pp->setSenderVelocity(string2double(value)); return true;
        case 14: pp->setAverageSpeed(string2double(value)); return true;
        case 15: pp->setMessageTime(string2long(value)); return true;
        case 16: pp->setTimeToReach(string2long(value)); return true;
        case 17: pp->setDwellTime(string2long(value)); return true;
        case 18: pp->setDwellDistance(string2double(value)); return true;
        case 19: pp->setNeighborChain((value)); return true;
        case 20: pp->setMessageInitTime(string2double(value)); return true;
        case 21: pp->setRequestDeadline(string2long(value)); return true;
        case 22: pp->setServiceTime(string2long(value)); return true;
        case 23: pp->setRequester(string2bool(value)); return true;
        case 24: pp->setInRange(string2bool(value)); return true;
        default: return false;
    }
}

const char *TraCIDemo11pMessageDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        case 0: return omnetpp::opp_typename(typeid(LAddress::L2Type));
        case 3: return omnetpp::opp_typename(typeid(Coord));
        case 10: return omnetpp::opp_typename(typeid(Coord));
        case 11: return omnetpp::opp_typename(typeid(Coord));
        case 12: return omnetpp::opp_typename(typeid(Coord));
        default: return nullptr;
    };
}

void *TraCIDemo11pMessageDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    TraCIDemo11pMessage *pp = (TraCIDemo11pMessage *)object; (void)pp;
    switch (field) {
        case 0: return (void *)(&pp->getSenderAddress()); break;
        case 3: return (void *)(&pp->getSenderPositionRLDCO()); break;
        case 10: return (void *)(&pp->getTargetCoord()); break;
        case 11: return (void *)(&pp->getEntryCoord()); break;
        case 12: return (void *)(&pp->getExitCoord()); break;
        default: return nullptr;
    }
}

} // namespace veins

