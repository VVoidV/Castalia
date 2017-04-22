 /*  Copyright: National ICT Australia,  2007 - 2010                         */

#ifndef _TMAC_H_
#define _TMAC_H_

#include "VirtualMac.h"
#include "TMacPacket_m.h"
#include "ThroughputTest.h"

#define TX_TIME(x)      (phyLayerOverhead + x)*1/(1000*phyDataRate/8.0) //x are in BYTES

using namespace std;


enum MacStates {
    MAC_STATE_SETUP=100,
    MAC_STATE_SLEEP=101,
    MAC_STATE_ACTIVE=102,
    MAC_STATE_ACTIVE_SILENT=103,
    MAC_STATE_IN_TX_BEACON=104,
    MAC_STATE_IN_TX_ACK=105,
    MAC_STATE_IN_TX_GACK=106,
    MAC_STATE_IN_TX_OACK=107,
    MAC_STATE_IN_TX_DATA=108,
    MAC_STATE_IN_TX_SACK=109,
    MAC_CARRIER_SENSE_FOR_TX_BEACON=110,
    MAC_CARRIER_SENSE_FOR_TX_GACK=111,
    MAC_CARRIER_SENSE_FOR_TX_OACK=112,
    MAC_CARRIER_SENSE_FOR_TX_SACK=113,
    MAC_CARRIER_SENSE_BEFORE_SLEEP=114,
    MAC_CARRIER_SENSE_FOR_TX_DATA=115,
    MAC_STATE_WAIT_FOR_DATA=116,
    MAC_STATE_WAIT_FOR_GACK=117,
    MAC_STATE_WAIT_FOR_OACK=118,
    MAC_STATE_WAIT_FOR_SACK=119,
    MAC_STATE_WAIT_FOR_BEACON=120,
    MAC_BACKOFF_BEFORE_ACK=121,
    MAC_STATE_WAIT_FOR_ACK_BEACON=122,
    MAC_STATE_WAIT_FOR_ACK_GACK=123,
};

enum TMacTimers {
    START_SLEEPING = 1,
    START_LISTENING = 2,
    BEACON_TIMEOUT = 3,
    WAIT_ACK_TIMEOUT = 4,
    WAIT_GACK_TIMEOUT = 5,
    WAIT_OACK_TIMEOUT = 6,
    WAIT_DATA_TIMEOUT = 7,
    WAIT_SACK_TIMEOUT=8,
    WAIT_BEACON_TIMEOUT=9,
    PERFORM_CCA= 10,
    ATTEMPT_TX_DATA = 12,
    SEND_DATA=12,
    TRANSMISSION_TIMEOUT=13,// this timer has to be last as TMAC will
                        // schedule multiple timers for silent wakeup
                        // if more than one secondary schedule is present
    BACKOFF_BEFORE_ACK=14,
    WAIT_ACK_TIMEOUT_AGAIN=15,
};

class ThroughputTest;
class TMAC: public VirtualMac {
public:
    bool isAckReceived[10];
    std::vector<int> ackList;
    std::vector<pair<int,int>> id_buffer;
    double receive_beacon;
   // std::vector<int> buffer_size;
   // int bufferSize[10];
 private:
    /*--- A map from int value of state to its description (used in debug) ---*/
    map<int,string> stateDescr;

    /*--- The .ned file's parameters ---*/
    bool printDebugInfo;
    bool printStateTransitions;

    int maxRetries;
    int maxGacks;
    int maxOacks;
    int maxBeacons;

    int beaconPacketSize ;
    int gackPacketSize ;
    int oackPacketSize;
    int ackPacketSize;
    int sackPacketSize;
    int dataPacketSize;

    int nextAckRetries;
    int macMinBE;
    int NB,BE,CW;
    int phyBitsPerSymbol;
    int macMaxBE;
    int macMaxCSMABackoffs;
    int macMaxAckRetries;
    double unitBackoffPeriod;
    int priority;

    int numTx;

    int revBeacon;
    int revAck;
    int revGack;
    int revOack;
    int revSack;
    int revData;

 //   string nextPacketState;
 //   bool batteryLifeExtention;
    bool txAllPacketsInFreeChannel;

    double dutyCycle;
    double listenInterval;
    double sleepInterval;
    double contentionPeriod;

 //   simtime_t lastbeacon;
    simtime_t waitTimeout;//发完ack后，等gack;收到gack到收到oack
 //   simtime_t timeout;//发完beacon后，这么长时间没有收到ack
    simtime_t symbolLen;
    simtime_t currentFrameStart;
    simtime_t phyDelayRx2Tx;

    /*--- General MAC variable ---*/
    bool isSink;
    int phyLayerOverhead;
    simtime_t phyDelayForValidCS;   // delay for valid CS
    double phyDataRate;

    /*--- TMAC state variables  ---*/
    int macState;
    int txAddr;         //current communication peer (can be BROADCAST)
    int txRetries;      //number of transmission attempts to txAddr (when reaches 0 - packet is dropped)


    int numTxTries;
    int backoffTimes;
    int numTxRetries;
    int sentGacks;
    int sentOacks;
    int sentBeacons;
    int sentAcks;

    simtime_t startTime[10];


    /*--- TMAC packet pointers (sometimes packet is created not immediately before sending) ---*/
    TMacPacket *beaconPacket;
    TMacPacket *ackPacket;
    TMacPacket *gackPacket;
    TMacPacket *oackPacket;
    TMacPacket *sackPacket;


 protected:
    void startup();
    void timerFiredCallback(int);
    void fromNetworkLayer(cPacket *, int);
    void fromRadioLayer(cPacket *, double, double);
    void attemptTx();
    void setMacState(int newState, const char *descr = NULL);
    void sendData();
//    void sendBeacon();
    void sendAck();
//    void sendGack();
 //   void sendOack();
 //   void sendSack();
    void checkTxBuffer();
    void popTxBuffer();
    void initiateCSMACA();
    void continueCSMACA();
};

#endif              //TMACMODULE
