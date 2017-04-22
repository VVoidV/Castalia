//
// Generated file, do not edit! Created by nedtool 5.0 from src/node/communication/mac/tMac/TMacPacket.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "TMacPacket_m.h"

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
    throw omnetpp::cRuntimeError("Parsim error: no doParsimPacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

template<typename T>
void doParsimUnpacking(omnetpp::cCommBuffer *, T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: no doParsimUnpacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

}  // namespace omnetpp


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

EXECUTE_ON_STARTUP(
    omnetpp::cEnum *e = omnetpp::cEnum::find("TmacPacket_type");
    if (!e) omnetpp::enums.getInstance()->add(e = new omnetpp::cEnum("TmacPacket_type"));
    e->insert(BEACON_TMAC_PACKET, "BEACON_TMAC_PACKET");
    e->insert(ACK_TMAC_PACKET, "ACK_TMAC_PACKET");
    e->insert(GACK_TMAC_PACKET, "GACK_TMAC_PACKET");
    e->insert(OACK_TMAC_PACKET, "OACK_TMAC_PACKET");
    e->insert(SACK_TMAC_PACKET, "SACK_TMAC_PACKET");
    e->insert(DATA_TMAC_PACKET, "DATA_TMAC_PACKET");
);

Register_Class(TMacPacket);

TMacPacket::TMacPacket(const char *name, int kind) : ::MacPacket(name,kind)
{
    this->type = 0;
    this->bufferSize = 0;
    ackedNode_arraysize = 0;
    this->ackedNode = 0;
    transferOrder_arraysize = 0;
    this->transferOrder = 0;
    O_ACK_bufferSize_arraysize = 0;
    this->O_ACK_bufferSize = 0;
}

TMacPacket::TMacPacket(const TMacPacket& other) : ::MacPacket(other)
{
    ackedNode_arraysize = 0;
    this->ackedNode = 0;
    transferOrder_arraysize = 0;
    this->transferOrder = 0;
    O_ACK_bufferSize_arraysize = 0;
    this->O_ACK_bufferSize = 0;
    copy(other);
}

TMacPacket::~TMacPacket()
{
    delete [] this->ackedNode;
    delete [] this->transferOrder;
    delete [] this->O_ACK_bufferSize;
}

TMacPacket& TMacPacket::operator=(const TMacPacket& other)
{
    if (this==&other) return *this;
    ::MacPacket::operator=(other);
    copy(other);
    return *this;
}

void TMacPacket::copy(const TMacPacket& other)
{
    this->type = other.type;
    this->bufferSize = other.bufferSize;
    delete [] this->ackedNode;
    this->ackedNode = (other.ackedNode_arraysize==0) ? nullptr : new bool[other.ackedNode_arraysize];
    ackedNode_arraysize = other.ackedNode_arraysize;
    for (unsigned int i=0; i<ackedNode_arraysize; i++)
        this->ackedNode[i] = other.ackedNode[i];
    delete [] this->transferOrder;
    this->transferOrder = (other.transferOrder_arraysize==0) ? nullptr : new int[other.transferOrder_arraysize];
    transferOrder_arraysize = other.transferOrder_arraysize;
    for (unsigned int i=0; i<transferOrder_arraysize; i++)
        this->transferOrder[i] = other.transferOrder[i];
    delete [] this->O_ACK_bufferSize;
    this->O_ACK_bufferSize = (other.O_ACK_bufferSize_arraysize==0) ? nullptr : new int[other.O_ACK_bufferSize_arraysize];
    O_ACK_bufferSize_arraysize = other.O_ACK_bufferSize_arraysize;
    for (unsigned int i=0; i<O_ACK_bufferSize_arraysize; i++)
        this->O_ACK_bufferSize[i] = other.O_ACK_bufferSize[i];
}

void TMacPacket::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::MacPacket::parsimPack(b);
    doParsimPacking(b,this->type);
    doParsimPacking(b,this->bufferSize);
    b->pack(ackedNode_arraysize);
    doParsimArrayPacking(b,this->ackedNode,ackedNode_arraysize);
    b->pack(transferOrder_arraysize);
    doParsimArrayPacking(b,this->transferOrder,transferOrder_arraysize);
    b->pack(O_ACK_bufferSize_arraysize);
    doParsimArrayPacking(b,this->O_ACK_bufferSize,O_ACK_bufferSize_arraysize);
}

void TMacPacket::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::MacPacket::parsimUnpack(b);
    doParsimUnpacking(b,this->type);
    doParsimUnpacking(b,this->bufferSize);
    delete [] this->ackedNode;
    b->unpack(ackedNode_arraysize);
    if (ackedNode_arraysize==0) {
        this->ackedNode = 0;
    } else {
        this->ackedNode = new bool[ackedNode_arraysize];
        doParsimArrayUnpacking(b,this->ackedNode,ackedNode_arraysize);
    }
    delete [] this->transferOrder;
    b->unpack(transferOrder_arraysize);
    if (transferOrder_arraysize==0) {
        this->transferOrder = 0;
    } else {
        this->transferOrder = new int[transferOrder_arraysize];
        doParsimArrayUnpacking(b,this->transferOrder,transferOrder_arraysize);
    }
    delete [] this->O_ACK_bufferSize;
    b->unpack(O_ACK_bufferSize_arraysize);
    if (O_ACK_bufferSize_arraysize==0) {
        this->O_ACK_bufferSize = 0;
    } else {
        this->O_ACK_bufferSize = new int[O_ACK_bufferSize_arraysize];
        doParsimArrayUnpacking(b,this->O_ACK_bufferSize,O_ACK_bufferSize_arraysize);
    }
}

int TMacPacket::getType() const
{
    return this->type;
}

void TMacPacket::setType(int type)
{
    this->type = type;
}

int TMacPacket::getBufferSize() const
{
    return this->bufferSize;
}

void TMacPacket::setBufferSize(int bufferSize)
{
    this->bufferSize = bufferSize;
}

void TMacPacket::setAckedNodeArraySize(unsigned int size)
{
    bool *ackedNode2 = (size==0) ? nullptr : new bool[size];
    unsigned int sz = ackedNode_arraysize < size ? ackedNode_arraysize : size;
    for (unsigned int i=0; i<sz; i++)
        ackedNode2[i] = this->ackedNode[i];
    for (unsigned int i=sz; i<size; i++)
        ackedNode2[i] = 0;
    ackedNode_arraysize = size;
    delete [] this->ackedNode;
    this->ackedNode = ackedNode2;
}

unsigned int TMacPacket::getAckedNodeArraySize() const
{
    return ackedNode_arraysize;
}

bool TMacPacket::getAckedNode(unsigned int k) const
{
    if (k>=ackedNode_arraysize) throw omnetpp::cRuntimeError("Array of size %d indexed by %d", ackedNode_arraysize, k);
    return this->ackedNode[k];
}

void TMacPacket::setAckedNode(unsigned int k, bool ackedNode)
{
    if (k>=ackedNode_arraysize) throw omnetpp::cRuntimeError("Array of size %d indexed by %d", ackedNode_arraysize, k);
    this->ackedNode[k] = ackedNode;
}

void TMacPacket::setTransferOrderArraySize(unsigned int size)
{
    int *transferOrder2 = (size==0) ? nullptr : new int[size];
    unsigned int sz = transferOrder_arraysize < size ? transferOrder_arraysize : size;
    for (unsigned int i=0; i<sz; i++)
        transferOrder2[i] = this->transferOrder[i];
    for (unsigned int i=sz; i<size; i++)
        transferOrder2[i] = 0;
    transferOrder_arraysize = size;
    delete [] this->transferOrder;
    this->transferOrder = transferOrder2;
}

unsigned int TMacPacket::getTransferOrderArraySize() const
{
    return transferOrder_arraysize;
}

int TMacPacket::getTransferOrder(unsigned int k) const
{
    if (k>=transferOrder_arraysize) throw omnetpp::cRuntimeError("Array of size %d indexed by %d", transferOrder_arraysize, k);
    return this->transferOrder[k];
}

void TMacPacket::setTransferOrder(unsigned int k, int transferOrder)
{
    if (k>=transferOrder_arraysize) throw omnetpp::cRuntimeError("Array of size %d indexed by %d", transferOrder_arraysize, k);
    this->transferOrder[k] = transferOrder;
}

void TMacPacket::setO_ACK_bufferSizeArraySize(unsigned int size)
{
    int *O_ACK_bufferSize2 = (size==0) ? nullptr : new int[size];
    unsigned int sz = O_ACK_bufferSize_arraysize < size ? O_ACK_bufferSize_arraysize : size;
    for (unsigned int i=0; i<sz; i++)
        O_ACK_bufferSize2[i] = this->O_ACK_bufferSize[i];
    for (unsigned int i=sz; i<size; i++)
        O_ACK_bufferSize2[i] = 0;
    O_ACK_bufferSize_arraysize = size;
    delete [] this->O_ACK_bufferSize;
    this->O_ACK_bufferSize = O_ACK_bufferSize2;
}

unsigned int TMacPacket::getO_ACK_bufferSizeArraySize() const
{
    return O_ACK_bufferSize_arraysize;
}

int TMacPacket::getO_ACK_bufferSize(unsigned int k) const
{
    if (k>=O_ACK_bufferSize_arraysize) throw omnetpp::cRuntimeError("Array of size %d indexed by %d", O_ACK_bufferSize_arraysize, k);
    return this->O_ACK_bufferSize[k];
}

void TMacPacket::setO_ACK_bufferSize(unsigned int k, int O_ACK_bufferSize)
{
    if (k>=O_ACK_bufferSize_arraysize) throw omnetpp::cRuntimeError("Array of size %d indexed by %d", O_ACK_bufferSize_arraysize, k);
    this->O_ACK_bufferSize[k] = O_ACK_bufferSize;
}

class TMacPacketDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    TMacPacketDescriptor();
    virtual ~TMacPacketDescriptor();

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

    virtual std::string getFieldValueAsString(void *object, int field, int i) const override;
    virtual bool setFieldValueAsString(void *object, int field, int i, const char *value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual void *getFieldStructValuePointer(void *object, int field, int i) const override;
};

Register_ClassDescriptor(TMacPacketDescriptor);

TMacPacketDescriptor::TMacPacketDescriptor() : omnetpp::cClassDescriptor("TMacPacket", "MacPacket")
{
    propertynames = nullptr;
}

TMacPacketDescriptor::~TMacPacketDescriptor()
{
    delete[] propertynames;
}

bool TMacPacketDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<TMacPacket *>(obj)!=nullptr;
}

const char **TMacPacketDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *TMacPacketDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int TMacPacketDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 5+basedesc->getFieldCount() : 5;
}

unsigned int TMacPacketDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISARRAY | FD_ISEDITABLE,
        FD_ISARRAY | FD_ISEDITABLE,
        FD_ISARRAY | FD_ISEDITABLE,
    };
    return (field>=0 && field<5) ? fieldTypeFlags[field] : 0;
}

const char *TMacPacketDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "type",
        "bufferSize",
        "ackedNode",
        "transferOrder",
        "O_ACK_bufferSize",
    };
    return (field>=0 && field<5) ? fieldNames[field] : nullptr;
}

int TMacPacketDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='t' && strcmp(fieldName, "type")==0) return base+0;
    if (fieldName[0]=='b' && strcmp(fieldName, "bufferSize")==0) return base+1;
    if (fieldName[0]=='a' && strcmp(fieldName, "ackedNode")==0) return base+2;
    if (fieldName[0]=='t' && strcmp(fieldName, "transferOrder")==0) return base+3;
    if (fieldName[0]=='O' && strcmp(fieldName, "O_ACK_bufferSize")==0) return base+4;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *TMacPacketDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",
        "int",
        "bool",
        "int",
        "int",
    };
    return (field>=0 && field<5) ? fieldTypeStrings[field] : nullptr;
}

const char **TMacPacketDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldPropertyNames(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        case 0: {
            static const char *names[] = { "enum",  nullptr };
            return names;
        }
        default: return nullptr;
    }
}

const char *TMacPacketDescriptor::getFieldProperty(int field, const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldProperty(field, propertyname);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        case 0:
            if (!strcmp(propertyname,"enum")) return "TmacPacket_type";
            return nullptr;
        default: return nullptr;
    }
}

int TMacPacketDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    TMacPacket *pp = (TMacPacket *)object; (void)pp;
    switch (field) {
        case 2: return pp->getAckedNodeArraySize();
        case 3: return pp->getTransferOrderArraySize();
        case 4: return pp->getO_ACK_bufferSizeArraySize();
        default: return 0;
    }
}

std::string TMacPacketDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    TMacPacket *pp = (TMacPacket *)object; (void)pp;
    switch (field) {
        case 0: return enum2string(pp->getType(), "TmacPacket_type");
        case 1: return long2string(pp->getBufferSize());
        case 2: return bool2string(pp->getAckedNode(i));
        case 3: return long2string(pp->getTransferOrder(i));
        case 4: return long2string(pp->getO_ACK_bufferSize(i));
        default: return "";
    }
}

bool TMacPacketDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    TMacPacket *pp = (TMacPacket *)object; (void)pp;
    switch (field) {
        case 0: pp->setType((TmacPacket_type)string2enum(value, "TmacPacket_type")); return true;
        case 1: pp->setBufferSize(string2long(value)); return true;
        case 2: pp->setAckedNode(i,string2bool(value)); return true;
        case 3: pp->setTransferOrder(i,string2long(value)); return true;
        case 4: pp->setO_ACK_bufferSize(i,string2long(value)); return true;
        default: return false;
    }
}

const char *TMacPacketDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

void *TMacPacketDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    TMacPacket *pp = (TMacPacket *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}


