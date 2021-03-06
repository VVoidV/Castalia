//
// Generated file, do not edit! Created by nedtool 5.0 from src/node/resourceManager/ResourceManagerMessage.msg.
//

#ifndef __RESOURCEMANAGERMESSAGE_M_H
#define __RESOURCEMANAGERMESSAGE_M_H

#include <omnetpp.h>

// nedtool version check
#define MSGC_VERSION 0x0500
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of nedtool: 'make clean' should help.
#endif



/**
 * Class generated from <tt>src/node/resourceManager/ResourceManagerMessage.msg:13</tt> by nedtool.
 * <pre>
 * message ResourceManagerMessage
 * {
 *     double powerConsumed;
 * }
 * </pre>
 */
class ResourceManagerMessage : public ::omnetpp::cMessage
{
  protected:
    double powerConsumed;

  private:
    void copy(const ResourceManagerMessage& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const ResourceManagerMessage&);

  public:
    ResourceManagerMessage(const char *name=nullptr, int kind=0);
    ResourceManagerMessage(const ResourceManagerMessage& other);
    virtual ~ResourceManagerMessage();
    ResourceManagerMessage& operator=(const ResourceManagerMessage& other);
    virtual ResourceManagerMessage *dup() const {return new ResourceManagerMessage(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b);

    // field getter/setter methods
    virtual double getPowerConsumed() const;
    virtual void setPowerConsumed(double powerConsumed);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const ResourceManagerMessage& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, ResourceManagerMessage& obj) {obj.parsimUnpack(b);}


#endif // ifndef __RESOURCEMANAGERMESSAGE_M_H

