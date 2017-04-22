//****************************************************************************
//*  Copyright: National ICT Australia,  2007 - 2010                         *
//*  Developed at the ATP lab, Networked Systems research theme              *
//*  Author(s): Athanassios Boulis, Yuriy Tselishchev                        *
//*  This file is distributed under the terms in the attached LICENSE file.  *
//*  If you do not find this file, copies can be found by writing to:        *
//*                                                                          *
//*      NICTA, Locked Bag 9013, Alexandria, NSW 1435, Australia             *
//*      Attention:  License Inquiry.                                        *
//*                                                                          *  
//****************************************************************************/

package castalia_3_2.src.node.communication.mac.tMac;

simple TMAC like castalia_3_2.src.node.communication.mac.iMac
{
    parameters:
        //debug parameters
        bool collectTraceInfo = default(true);
        bool printStateTransitions = default(true);
        //mac layer packet sizes, these parameters are described in TMacFrame.msg file
        int ackPacketSize = default(11);
        int beaconPacketSize = default(11);
        int gackPacketSize = default(11);
        int oackPacketSize = default(11);
        int sackPacketSize = default(11);
        int dataPacketSize = default(255);
        //mac layer parameters
        int macMaxPacketSize = default(0);		//no limit on frame size
        int macPacketOverhead = default(11);	//DATA frame overhead is described in TMacFrame.msg
        int macBufferSize = default(32);		//buffer of 32 packets by default

        //parameters dependent on physical layer
        double phyDelayForValidCS = default(0.128);
        double phyDataRate = default(250);
        int phyFrameOverhead = default(6);
        
       bool printDebugInfo= default(false);

	int maxRetries= default(2);
    int maxGacks= default(10);
    int maxOacks= default(10);
    int maxBeacons= default(5);

    int nextAckRetries= default(1);
    int enableSlottedCSMA= default(0);
    int macMinBE= default(2);
    int phyBitsPerSymbol= default(2);
    int macMaxBE= default(8);
    int macMaxCSMABackoffs= default(5);
    int macMaxAckRetries= default(5);
    double unitBackoffPeriod= default(0.3);
    double contentionPeriod=default(10);

    int numTx= default(0);

    int revBeacon= default(0);
    int revAck= default(0);
    int revGack= default(0);
    int revOack= default(0);
    int revSack= default(0);
    int revData= default(0);

    bool batteryLifeExtention= default(false);
    bool txAllPacketsInFreeChannel= default(false);

    double dutyCycle= default (0.1);
    double listenInterval= default (500);
    double sleepInterval= default (2100);

	bool isSink= default (false);

		//current communication peer (can be BROADCAST)
	int txRetries= default (0);		//number of transmission attempts to txAddr (when reaches 0 - packet is dropped)


    int numTxTries= default (0);
    int backoffTimes= default (0);
    int numTxRetries= default (0);
    int sentGacks= default (0);
    int sentOacks= default (0);
    int sentBeacons= default (0);
    int sentAcks= default (0);
    
    
    double waitTimeout= default (2);//发完ack后，等gack;收到gack到收到oack
    double symbolLen= default (15);
    double currentFrameStart= default (0);
    double phyDelayRx2Tx= default (2);
    

    gates:
        output toNetworkModule;
        output toRadioModule;
        input fromNetworkModule;
        input fromRadioModule;
        input fromCommModuleResourceMgr;
}

