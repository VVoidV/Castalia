/*  Copyright: National ICT Australia,  2007 - 2010                         */

#include <cmath>
#include <vector>
#include <map>
#include <algorithm>
#include<iostream>

#include "TMAC.h"
using namespace std;
Define_Module(TMAC);

void TMAC::startup()
{
    dutyCycle = par("dutyCycle");
    startTime[10] = { 0.0 };
    listenInterval = ((double)par("listenInterval")) / 1000.0;  // convert msecs to secs
    sleepInterval = ((double)par("sleepInterval")) / 1000.0;
    printStateTransitions = par("printStateTransitions");
    txAllPacketsInFreeChannel = par("txAllPacketsInFreeChannel");

    ackPacketSize = par("ackPacketSize");
    beaconPacketSize = par("beaconPacketSize");
    gackPacketSize = par("gackPacketSize");
    oackPacketSize = par("oackPacketSize");
    sackPacketSize = par("sackPacketSize");
    dataPacketSize = par("dataPacketSize");
    waitTimeout = ((double)par("waitTimeout")) / 1000.0;//发完beacon到等待发送gack的时间，以及发完gack等ack的时间，如果发完gack在这段时间内
//没有收到ack，则回复oack；如果gack达到最大传输次数，则回复oack

    maxRetries = par("maxRetries");
    maxGacks = par("maxGacks");
    maxOacks = par("maxOacks");
    maxBeacons = par("maxBeacons");
    macMaxCSMABackoffs = par("macMaxCSMABackoffs");

    contentionPeriod = ((double)par("contentionPeriod")) / 1000.0;

    phyDelayRx2Tx = (double)par("phyDelayRx2Tx") / 1000.0;
    unitBackoffPeriod = (double)par("unitBackoffPeriod") / 1000.0;
    symbolLen = 1 / (phyDataRate * 1000 / phyBitsPerSymbol);
    currentFrameStart = 0;
    CW = 0;
    numTxRetries = 0;
    isAckReceived.resize(10);//bug
    fill(isAckReceived.begin(),isAckReceived.end(),false);
    //Initialise state descriptions used in debug output
    if (printStateTransitions) {
        stateDescr[100] = "MAC_STATE_SETUP";
        stateDescr[101] = "MAC_STATE_SLEEP";
        stateDescr[102] = "MAC_STATE_ACTIVE",
        stateDescr[103] = "MAC_STATE_ACTIVE_SILENT";
        stateDescr[104] = "MAC_STATE_IN_TX_BEACON";
        stateDescr[105] = "MAC_STATE_IN_TX_ACK";
        stateDescr[106] = "MAC_STATE_IN_TX_GACK";
        stateDescr[107] = "MAC_STATE_IN_TX_OACK";
        stateDescr[108] = "MAC_STATE_IN_TX_DATA";
        stateDescr[109] = "MAC_STATE_IN_TX_SACK";
        stateDescr[110] = "MAC_CARRIER_SENSE_FOR_TX_BEACON";
        stateDescr[111] = "MAC_CARRIER_SENSE_FOR_TX_GACK";
        stateDescr[112] = "MAC_CARRIER_SENSE_FOR_TX_OACK";
        stateDescr[113] = "MAC_CARRIER_SENSE_FOR_TX_SACK";
        stateDescr[114] = "MAC_CARRIER_SENSE_BEFORE_SLEEP";
        stateDescr[115] = "MAC_CARRIER_SENSE_FOR_TX_DATA";
        stateDescr[116] = "MAC_STATE_WAIT_FOR_DATA";
        stateDescr[117] = "MAC_STATE_WAIT_FOR_GACK";
        stateDescr[118] = "MAC_STATE_WAIT_FOR_OACK";
        stateDescr[119] = "MAC_STATE_WAIT_FOR_SACK";
        stateDescr[120] = "MAC_STATE_WAIT_FOR_BEACON";
        stateDescr[121] = "MAC_BACKOFF_BEFORE_ACK";
        stateDescr[122] = "MAC_STATE_WAIT_FOR_ACK_BEACON";
        stateDescr[123] = "MAC_STATE_WAIT_FOR_ACK_GACK";
    }

    phyDataRate = par("phyDataRate");
    phyDelayForValidCS = (double)par("phyDelayForValidCS") / 1000.0;    //parameter given in ms in the omnetpp.ini
    phyLayerOverhead = par("phyFrameOverhead");


    beaconPacket = NULL;
    ackPacket = NULL;
    gackPacket = NULL;
    oackPacket = NULL;
    sackPacket = NULL;
    declareOutput("Sent packets breakdown");
    numTxTries = 0;
    backoffTimes = 0;
    numTxRetries = 0;
    sentGacks = 0;
    sentOacks = 0;
    sentBeacons = 0;

    //isAckReceived[9] = { false };


    if (self == 0) {
        isSink = true;
        setTimer(START_LISTENING, 0.5);
        startTime[self] = getClock();

    }
    else
    {
        isSink = false;
        setTimer(START_LISTENING, 0.5);
        startTime[self] = getClock();
    }
    macState = MAC_STATE_SLEEP;
    WATCH(macState);
    WATCH(macBufferSize);
}

void TMAC::timerFiredCallback(int timer)
{
    if (isSink)
    {//Sink
        switch (timer)
        {
        case START_SLEEPING:
            toRadioLayer(createRadioCommand(SET_STATE, SLEEP));
            setTimer(START_LISTENING, sleepInterval);
            break;
        case START_LISTENING:
            setMacState(MAC_CARRIER_SENSE_FOR_TX_BEACON, "sink wakes up");
            toRadioLayer(createRadioCommand(SET_STATE, TX));
            setTimer(PERFORM_CCA, phyDelayForValidCS);
            break;
        case WAIT_ACK_TIMEOUT:
            if (ackList.empty())
            {
                setMacState(MAC_CARRIER_SENSE_FOR_TX_BEACON, "sink sends beacon again or goes to sleep");
                setTimer(PERFORM_CCA, phyDelayForValidCS);
            }
           else
           {
                setMacState(MAC_CARRIER_SENSE_FOR_TX_GACK, "sink prepares to send a gack");
                setTimer(PERFORM_CCA, phyDelayForValidCS);
            }
            break;
        case WAIT_ACK_TIMEOUT_AGAIN:
            setMacState(MAC_CARRIER_SENSE_FOR_TX_OACK, "sink prepares to send an oack");
            setTimer(PERFORM_CCA, phyDelayForValidCS);
            break;
        case WAIT_DATA_TIMEOUT:
            setMacState(MAC_CARRIER_SENSE_FOR_TX_OACK, "sink sends oack again or goes to sleep");
            setTimer(PERFORM_CCA, phyDelayForValidCS);
            break;
        case WAIT_DATA_TIMEOUT_AFTER_SACK:
            //no more data income
            ackList.clear();
            id_buffer.clear();
            fill(isAckReceived.begin(),isAckReceived.end(),false);
            setMacState(MAC_CARRIER_SENSE_FOR_TX_BEACON, "finish receiving data,resends a beacon");
            toRadioLayer(createRadioCommand(SET_STATE, TX));
            setTimer(PERFORM_CCA, phyDelayForValidCS);
            break;
        case PERFORM_CCA:
#pragma region PERFORM_CCA
            if (macState == MAC_STATE_SLEEP)
                break;
            if (macState != MAC_CARRIER_SENSE_FOR_TX_BEACON
                && macState != MAC_CARRIER_SENSE_FOR_TX_GACK
                && macState != MAC_CARRIER_SENSE_FOR_TX_OACK
                && macState != MAC_CARRIER_SENSE_FOR_TX_DATA
                && macState != MAC_CARRIER_SENSE_BEFORE_SLEEP) {
                trace() << "macState:" << macState;
                trace() << "WARNING";
                break;
            }
            if (radioModule->isChannelClear()== CS_NOT_VALID_YET)
            {
                setTimer(PERFORM_CCA, phyDelayForValidCS);
                break;
            }
            else if (radioModule->isChannelClear() == CS_NOT_VALID)
            {
                toRadioLayer(createRadioCommand(SET_STATE, RX));
                setTimer(PERFORM_CCA, phyDelayForValidCS);
                break;
            }

            switch (macState)
            {
            case MAC_CARRIER_SENSE_FOR_TX_BEACON:
                if (radioModule->isChannelClear() == CLEAR)
                {
                    setMacState(MAC_STATE_IN_TX_BEACON, "sink send beacon");
                    trace() << "Channel Clear, MAC_STATE_TX_BEACON, sending beacon";
                    sendBeacon();
                }
                else if (radioModule->isChannelClear() == BUSY)
                {
                    setMacState(MAC_CARRIER_SENSE_FOR_TX_BEACON);
                    setTimer(PERFORM_CCA, phyDelayForValidCS);
                }
                break;
            case MAC_CARRIER_SENSE_FOR_TX_GACK:
                if (radioModule->isChannelClear() == CLEAR)
                {
                    setMacState(MAC_STATE_IN_TX_GACK, "sink sends gack");
                    trace() << "Channel Clear, MAC_STATE_TX_GACK, sending ";
                    sendGack();
                }
                else if (radioModule->isChannelClear() == BUSY)
                {
                    setMacState(MAC_CARRIER_SENSE_FOR_TX_GACK);
                    setTimer(PERFORM_CCA, phyDelayForValidCS);
                }
                break;
            case MAC_CARRIER_SENSE_FOR_TX_OACK:
                if (radioModule->isChannelClear() == CLEAR)
                {
                    setMacState(MAC_STATE_IN_TX_OACK, "sink sends an oack");
                    trace() << "Channel Clear, MAC_STATE_TX_OACK, sending ";
                    sendOack();
                }
                else if (radioModule->isChannelClear() == BUSY)
                {
                    setMacState(MAC_CARRIER_SENSE_FOR_TX_OACK);
                    setTimer(PERFORM_CCA, phyDelayForValidCS);
                }
                break;
            case MAC_CARRIER_SENSE_FOR_TX_DATA:
                //目前sink 没数据要传
                break;
            case MAC_CARRIER_SENSE_BEFORE_SLEEP:
                if (radioModule->isChannelClear() == CLEAR)
                {
                    setMacState(MAC_STATE_SLEEP, "sleep");
                    //这里无线电还没关
                    setTimer(START_LISTENING, sleepInterval);
                }
                else if (radioModule->isChannelClear() == BUSY)
                {
                    setMacState(MAC_CARRIER_SENSE_BEFORE_SLEEP);
                    setTimer(PERFORM_CCA, phyDelayForValidCS);
                }
                break;
            default:
                trace() << "WARNING:unexpected case occured! in PERFORM_CCA Switch MAC_STATE";
            }
#pragma endregion PERFORM_CCA
            break;
            case TRANSMISSION_TIMEOUT:
                //Sink finished sending SAck
                if(macState==MAC_STATE_IN_TX_SACK)
                {
                    setTimer(WAIT_DATA_TIMEOUT_AFTER_SACK, TX_TIME(dataPacketSize + ackPacketSize ));

                }
                break;
        default:
            trace() << "WARNING:unexpected case occured in switch (timer) SINK";

        }
    }
    else
    {//other Node
        switch (timer)
        {
        case START_SLEEPING:
            toRadioLayer(createRadioCommand(SET_STATE, SLEEP));
            setTimer(START_LISTENING, sleepInterval);
            break;
        case START_LISTENING:
            if (!TXBuffer.empty())
            {
                toRadioLayer(createRadioCommand(SET_STATE, RX));
                setMacState(MAC_STATE_WAIT_FOR_BEACON, "sensor wakes up");
                //setTimer(WAIT_BEACON_TIMEOUT, listenInterval);
            }
            else
            {
                toRadioLayer(createRadioCommand(SET_STATE, SLEEP));
                setTimer(START_LISTENING, sleepInterval);
            }
            break;
        case WAIT_BEACON_TIMEOUT:
            setMacState(MAC_STATE_SLEEP, "sensor receives no beacon");
            toRadioLayer(createRadioCommand(SET_STATE, SLEEP));
            setTimer(START_LISTENING, sleepInterval);
            break;
        case BACKOFF_BEFORE_ACK:
            if (radioModule->isChannelClear() == CLEAR)
            {
                sendAck();
            }
            else if (radioModule->isChannelClear() == BUSY)
            {
                int x = 2 + rand() % 7;
                setMacState(MAC_BACKOFF_BEFORE_ACK, "sensor receives beacon");
                setTimer(BACKOFF_BEFORE_ACK, x * unitBackoffPeriod);
            }
            break;
        case WAIT_GACK_TIMEOUT:
            setMacState(MAC_STATE_SLEEP, "sensor waits for gack timeout");
            toRadioLayer(createRadioCommand(SET_STATE, SLEEP));
            setTimer(START_LISTENING, sleepInterval);
            break;
        case WAIT_OACK_TIMEOUT:
            setMacState(MAC_STATE_SLEEP, "sensor waits for oack timeout");
            toRadioLayer(createRadioCommand(SET_STATE, SLEEP));
            setTimer(START_LISTENING, sleepInterval);
            break;
        case WAIT_SACK_TIMEOUT:
            //packet lose
            setMacState(MAC_CARRIER_SENSE_FOR_TX_DATA,"packet lost retransfer");
            setTimer(PERFORM_CCA, phyDelayForValidCS);
            break;
        case PERFORM_CCA:
            if (macState == MAC_STATE_SLEEP)
                break;
            if (radioModule->isChannelClear() == CLEAR)
            {
                switch (macState)
                {
                case MAC_CARRIER_SENSE_FOR_TX_DATA:
                    setMacState(MAC_STATE_IN_TX_DATA, "sensor sends data");
                    trace() << "Channel Clear, MAC_STATE_TX_DATA, sending ";
                    if (maxRetries) {
                        sendData();
                    }
                    else
                    {//drop current packet
                        popTxBuffer();
                    }
                    break;
                case MAC_CARRIER_SENSE_BEFORE_SLEEP:
                    setMacState(MAC_STATE_SLEEP, "sleep");
                    setTimer(START_LISTENING, sleepInterval);
                    break;
                default:
                    trace() << "Waring: unexpected case " << macState << " occured in if (radioModule->isChannelClear() == CLEAR) node";
                    break;
                }
            }//if (radioModule->isChannelClear() == CLEAR)
            else if (radioModule->isChannelClear() == BUSY)
            {
                switch (macState)
                {
                case MAC_CARRIER_SENSE_FOR_TX_DATA:
                    setMacState(MAC_CARRIER_SENSE_FOR_TX_DATA);
                    setTimer(PERFORM_CCA, phyDelayForValidCS);
                    break;
                case MAC_CARRIER_SENSE_BEFORE_SLEEP:
                    setMacState(MAC_CARRIER_SENSE_BEFORE_SLEEP);
                    setTimer(PERFORM_CCA, phyDelayForValidCS);
                default:
                    trace() << "Waring: unexpected case " << macState << " occured in if (radioModule->isChannelClear() == CLEAR) node";
                    break;
                }
            }//else if (radioModule->isChannelClear() == BUSY)
            else if (radioModule->isChannelClear() == CS_NOT_VALID_YET)
            {
                setTimer(PERFORM_CCA, phyDelayForValidCS);
            }
            else if (radioModule->isChannelClear() == CS_NOT_VALID)
            {
                toRadioLayer(createRadioCommand(SET_STATE, RX));
                setTimer(PERFORM_CCA, phyDelayForValidCS);
            }
            break;
        case TRANSMISSION_TIMEOUT:
            trace() << "transmission timeout ";
            break;
        default:
            trace() << "WARNING: unknown timer callback " << timer;
        }//switch
    }
}


void TMAC::sendData()
{
    if (TXBuffer.empty()) {
        trace() << "WARNING: Invalid MAC_CARRIER_SENSE_FOR_TX_DATA while TX buffer is empty";
        return;
    }
    // create a copy of first message in the TX buffer and send it to the radio
    toRadioLayer(TXBuffer.front()->dup());
    collectOutput("Sent packets breakdown", "DATA");

    //update MAC state based on transmission time and destination address
    double txTime = TX_TIME(TXBuffer.front()->getByteLength());

    if (txAddr == BROADCAST_MAC_ADDRESS) {
        // This packet is broadcast, so no reply will be received
        // The packet can be cleared from transmission buffer
        // and MAC timeout is only to allow RADIO to finish the transmission
        popTxBuffer();
        //        setMacState(MAC_STATE_IN_TX_DATA, "sent DATA packet to BROADCAST address");
        //        setTimer(TRANSMISSION_TIMEOUT, txTime);

    }
    else
    {
        txRetries--;
        setMacState(MAC_STATE_WAIT_FOR_SACK, "sent DATA packet to UNICAST address");
        setTimer(WAIT_SACK_TIMEOUT, txTime + TX_TIME(11) + phyDelayForValidCS);
    }
    toRadioLayer(createRadioCommand(SET_STATE, TX));
}

void TMAC::fromNetworkLayer(cPacket * netPkt, int destination)
{
    // Create a new MAC frame from the received packet and buffer it (if possible)
    TMacPacket *macPkt = new TMacPacket("TMAC data packet", MAC_LAYER_PACKET);
    encapsulatePacket(macPkt, netPkt);
    macPkt->setType(DATA_TMAC_PACKET);
    macPkt->setSource(SELF_MAC_ADDRESS);
    macPkt->setDestination(destination);
    if (bufferPacket(macPkt)) { // this is causing problems
        if (TXBuffer.size() == 1)
            checkTxBuffer();
        if (macState == MAC_STATE_SLEEP)
        {
            cancelTimer(START_LISTENING);
            setTimer(START_LISTENING, 0);
        }
    }
    else {
         //cancelAndDelete(macPkt);
         //We could send a control message to upper layer to inform of full buffer
        trace() << "WARNING  MAC buffer overflow";
    }
}


/* Helper function to change internal MAC state and print a debug statement if neccesary */
void TMAC::setMacState(int newState, const char *descr)
{
    if (macState == newState)
        return;
    if (printStateTransitions) {
        if (descr)
            trace() << "state changed from " << stateDescr[macState] <<
            " to " << stateDescr[newState] << ", reason: " << descr;
        else
            trace() << "state changed from " << stateDescr[macState] <<
            " to " << stateDescr[newState];
    }
    macState = newState;
}

/* This function will handle a MAC frame received from the lower layer (physical or radio)
 * We try to see if the received packet is TMacPacket, otherwise we discard it
 * TMAC ignores values of RSSI and LQI
 */
void TMAC::fromRadioLayer(cPacket * pkt, double RSSI, double LQI)
{
    TMacPacket *macPkt = dynamic_cast <TMacPacket *>(pkt);
    if (macPkt == NULL)
        return;

    int source = macPkt->getSource();
    int destination = macPkt->getDestination();

    // first of all, check if the packet is to this node or not
    if (destination != SELF_MAC_ADDRESS && destination != BROADCAST_MAC_ADDRESS) {
        //setMacState(MAC_STATE_ACTIVE_SILENT, "overheard a packet");
        return;
    }

    switch (macPkt->getType()) {

        /* received a RTS frame */
    case BEACON_TMAC_PACKET: {
        cancelTimer(WAIT_BEACON_TIMEOUT);
        if (macState == MAC_STATE_WAIT_FOR_BEACON) {
            revBeacon++;
            int x = 2 + rand() % 7;
            setMacState(MAC_BACKOFF_BEFORE_ACK, "sensor receives beacon");
            setTimer(BACKOFF_BEFORE_ACK, x * unitBackoffPeriod);
            trace() << self << " receives beacon from sink at " << receive_beacon;
            //receive_beacon[self]=getClock();
        }
        break;
    }
    case ACK_TMAC_PACKET: {
        for (int i = 0; i < ackList.size(); i++)
        {
            cout << "ackList: " << ackList[i] << endl;
        }
        if (macState == MAC_STATE_WAIT_FOR_ACK_BEACON) {
            revAck = 0;
            for (int i = 0; i < ackList.size(); i++)
            {
                if (ackList[i] == source)
                {
                    isAckReceived[source] = true;
                    break;
                }
            }
            if (isAckReceived[source] == false)
            {
                ackList.push_back(source);//不存在
                int size = macPkt->getBufferSize();
                id_buffer.push_back(make_pair(source, size));
                //id_buffer[source]=macPkt->getBufferSize();//sink 收到source的ack包，把source的buffer大小存到buffersize中。
                isAckReceived[source] = true;
            }
        }
        if (macState == MAC_STATE_WAIT_FOR_ACK_GACK)
        {
            cancelTimer(WAIT_ACK_TIMEOUT_AGAIN);
            for (int i = 0; i < ackList.size(); i++)
            {
                if (ackList[i] == source)
                {
                    isAckReceived[source] = true;
                    break;
                }
            }
            if (isAckReceived[source] == false)
            {
                ackList.push_back(source);//不存在
                int size = macPkt->getBufferSize();
                id_buffer.push_back(make_pair(source, size));
                //id_buffer[source]=macPkt->getBufferSize();//sink 收到source的ack包，把source的buffer大小存到buffersize中。
                isAckReceived[source] = true;
            }
            setMacState(MAC_CARRIER_SENSE_FOR_TX_GACK, "send next GACK packet");
            //toRadioLayer(createRadioCommand(SET_STATE, RX));
            setTimer(PERFORM_CCA, phyDelayForValidCS);
        }
        break;
    }

                          // received a GACK frame
    case GACK_TMAC_PACKET: {
        std::cout << self << "receives gack at" << getClock() << endl;
        if (macState == MAC_STATE_WAIT_FOR_GACK) {
            revGack++;
            cancelTimer(WAIT_GACK_TIMEOUT);
            if (macPkt->getAckedNode(self) == true)
            {
                setMacState(MAC_STATE_WAIT_FOR_OACK, "wait OACK packet");
                toRadioLayer(createRadioCommand(SET_STATE, RX));
                setTimer(WAIT_OACK_TIMEOUT, waitTimeout*maxGacks + TX_TIME(11) * 2);
            }
            else
            {
                sentAcks++;
                int x = 2 + rand() % 7;
                setMacState(MAC_BACKOFF_BEFORE_ACK, "sensor's ack is not received");
                setTimer(BACKOFF_BEFORE_ACK, x * unitBackoffPeriod);
            }

        }
        break;
    }

    case OACK_TMAC_PACKET: {
        if (macState == MAC_STATE_WAIT_FOR_OACK)
        {
            revOack++;
            cancelTimer(WAIT_OACK_TIMEOUT);
            std::vector<pair<int, int>> temp;
            for(int i = 0; i < macPkt->getTransferOrderArraySize(); i++)
            {
                temp.push_back(make_pair( macPkt->getTransferOrder(i), macPkt -> getO_ACK_bufferSize(i) ));
            }
            //std::vector<pair<int, int>> ::iterator iter1 = temp.begin(); //访问vector
            std::vector<pair<int, int>> ::iterator key;
            double sum_size = 0.0;
            for (auto iter=temp.begin(); iter != temp.end(); ++iter)
            {

                if (iter->first == self)
                {
                    break;
                }
                cout << "id:" << iter->first << "buffer_size:" << iter->second << endl;
                sum_size += iter->second;
            }

            setMacState(MAC_CARRIER_SENSE_FOR_TX_DATA, "send data");
            trace() << self << " receives an ock at:" << getClock();
            setTimer(PERFORM_CCA, (sum_size)*TX_TIME(dataPacketSize + ackPacketSize));

        }
        break;
    }

                           // received DATA frame
    case DATA_TMAC_PACKET: {
        if (self == 0)
        {
            revData++;
            trace() << "receive data from " << source;
            if (isNotDuplicatePacket(macPkt))
                toNetworkLayer(macPkt->decapsulate());
            // If the frame was sent to broadcast address, nothing else needs to be done
            // If MAC was expecting this frame, clear the timeout
            if (macState == MAC_STATE_WAIT_FOR_DATA)
                cancelTimer(WAIT_DATA_TIMEOUT);
            cancelTimer(WAIT_DATA_TIMEOUT_AFTER_SACK);

            // Create and send an ACK frame (since this node is the destination for DATA frame)
            if (sackPacket)
                cancelAndDelete(sackPacket);

            sackPacket = new TMacPacket("TMAC SACK packet", MAC_LAYER_PACKET);
            sackPacket->setSource(SELF_MAC_ADDRESS);
            sackPacket->setDestination(source);
            sackPacket->setType(SACK_TMAC_PACKET);
            sackPacket->setByteLength(sackPacketSize);

            // Send ACK packet to the radio
            toRadioLayer(sackPacket);
            toRadioLayer(createRadioCommand(SET_STATE, TX));

            collectOutput("Sent packets breakdown", "SACK");
            //delete sackPacket;

            // update MAC state
            setMacState(MAC_STATE_IN_TX_SACK, "transmitting SACK packet");

            // create a timeout for this transmission - nothing is expected in reply
            // so MAC is only waiting for the RADIO to finish the packet transmission
            setTimer(TRANSMISSION_TIMEOUT, TX_TIME(sackPacketSize) + phyDelayForValidCS);
        }
        // Forward the frame to upper layer first

        break;
    }

                           // received ACK frame
    case SACK_TMAC_PACKET: {
        if (macState == MAC_STATE_WAIT_FOR_SACK && self == destination) {
            trace() << "Transmission successful to " << txAddr;
            cancelTimer(WAIT_SACK_TIMEOUT);
            popTxBuffer();
            if (TXBuffer.empty())
            {
                setMacState(MAC_STATE_SLEEP, "DATA send over");
                //setTimer(START_SLEEPING, sleepInterval);//bug
                toRadioLayer(createRadioCommand(SET_STATE, SLEEP));
                break;
            }
            else
            {
                //double txTime = TX_TIME(TXBuffer.front()->getByteLength());
                setMacState(MAC_CARRIER_SENSE_FOR_TX_DATA, "send next data");
                setTimer(PERFORM_CCA, 0);
            }
        }
        break;
    }

    default: {
        trace() << "Packet with unknown type (" << macPkt->getType() <<
            ") received: [" << macPkt->getName() << "]";
    }
    }
}


/* This function will set a timer to perform carrier sense.
 * MAC state is important when performing CS, so setMacState is always called here.
 * delay allows to perform a carrier sense after a choosen delay (useful for
 * randomisation of transmissions)
 设置mac状态就会调用这里：设置一个timer去载波检测。延迟delay时间后，执行载波检测
 */


 /* This function will check the transmission buffer, and if it is not empty, it will update
  * current communication parameters: txAddr and txRetries
  检测传输buffer，如果不为空，则更新当前通信参数：txAddr and txRetries
  */
void TMAC::checkTxBuffer()
{
    if (TXBuffer.empty())
        return;
    TMacPacket *macPkt = check_and_cast <TMacPacket *>(TXBuffer.front());
    txAddr = macPkt->getDestination();
    txRetries = maxRetries;
}

/* This function will remove the first packet from MAC transmission buffer
 * checkTxBuffer is called in case there are still packets left in the buffer to transmit
 移除传输队列中的第一个包，并检测buffer是否为空
 */
void TMAC::popTxBuffer()
{
    cancelAndDelete(TXBuffer.front());
    TXBuffer.pop();
    checkTxBuffer();
}
//////////////////////////////////////////////////////////////////////////
void TMAC::sendBeacon()
{
    if (sentBeacons < maxBeacons)
    {
        sentBeacons++;
        TMacPacket *beaconPacket = new TMacPacket("BEACON message", MAC_LAYER_PACKET);
        beaconPacket->setSource(SELF_MAC_ADDRESS);
        beaconPacket->setDestination(BROADCAST_MAC_ADDRESS);
        beaconPacket->setType(BEACON_TMAC_PACKET);
        beaconPacket->setByteLength(beaconPacketSize);
        toRadioLayer(beaconPacket);
        toRadioLayer(createRadioCommand(SET_STATE, TX));
        trace() << "sink sends beacon successfully";

        setMacState(MAC_STATE_WAIT_FOR_ACK_BEACON, "after sending beacon packet");
        setTimer(WAIT_ACK_TIMEOUT, waitTimeout);//这么长时间后没有收到ack
        beaconPacket = NULL;
        // lastbeacon=getClock();
    }
    else
    {
        setMacState(MAC_STATE_SLEEP);
        sentBeacons = 0;
        toRadioLayer(createRadioCommand(SET_STATE, SLEEP));
        setTimer(START_LISTENING, sleepInterval);
    }
}
void TMAC::sendGack()
{
    if (sentGacks < maxGacks)
    {
        sentGacks++;
        TMacPacket *gackPacket = new TMacPacket("GACK message", MAC_LAYER_PACKET);
        gackPacket->setSource(SELF_MAC_ADDRESS);
        gackPacket->setDestination(BROADCAST_MAC_ADDRESS);
        gackPacket->setType(GACK_TMAC_PACKET);
        gackPacket->setByteLength(gackPacketSize);
        ///////
        gackPacket->setAckedNodeArraySize(10);
        for (int i = 0; i < 10; i++)
        {//need fix

            gackPacket->setAckedNode(i, isAckReceived[i]);

        }

        // gackPacket->setAckList(ackList);

        toRadioLayer(gackPacket);
        toRadioLayer(createRadioCommand(SET_STATE, TX));

        setMacState(MAC_STATE_WAIT_FOR_ACK_GACK, "after sending a gack packet");
        setTimer(WAIT_ACK_TIMEOUT_AGAIN, TX_TIME(11) + waitTimeout);
        gackPacket = NULL;
    }
    else
    {
        setMacState(MAC_CARRIER_SENSE_FOR_TX_OACK, "after sending a gack packet");
        setTimer(PERFORM_CCA, phyDelayForValidCS);
    }
}
void TMAC::sendOack()
{
    if (sentOacks < maxOacks)
    {
        sentOacks++;
        TMacPacket *oackPacket = new TMacPacket("OACK message", MAC_LAYER_PACKET);
        oackPacket->setSource(SELF_MAC_ADDRESS);
        oackPacket->setDestination(BROADCAST_MAC_ADDRESS);
        oackPacket->setType(OACK_TMAC_PACKET);
        oackPacket->setByteLength(oackPacketSize);
        //////////////////////////////////////////////
        oackPacket->setTransferOrderArraySize(ackList.size());
        oackPacket->setO_ACK_bufferSizeArraySize(ackList.size());
        for (int i = 0; i < ackList.size(); i++)
        {
            oackPacket->setTransferOrder(i, ackList[i]);
            oackPacket->setO_ACK_bufferSize(i, id_buffer[i].second);
        }
        //oackPacket->setIdBuffer(id_buffer);

        toRadioLayer(oackPacket);
        toRadioLayer(createRadioCommand(SET_STATE, TX));
        oackPacket = NULL;

        setMacState(MAC_STATE_WAIT_FOR_DATA, "sent oack packet");
        setTimer(WAIT_DATA_TIMEOUT, TX_TIME(dataPacketSize) + waitTimeout);//发完gack，等ack的时间,没等到则重发gack
                                                                           //if no reply is received after this time interval,then transmission attemp is considered failed and attempt counter is decremented.
    }
    else
    {
        toRadioLayer(createRadioCommand(SET_STATE, SLEEP));
        setMacState(MAC_STATE_SLEEP);
        setTimer(START_LISTENING, sleepInterval);
    }
}
void TMAC::sendAck()
{
    if ((sentAcks <= macMaxCSMABackoffs))
    {
        sentAcks++;
        TMacPacket *ackPacket = new TMacPacket("ACK message", MAC_LAYER_PACKET);
        ackPacket->setSource(SELF_MAC_ADDRESS);
        ackPacket->setDestination(BROADCAST_MAC_ADDRESS);
        ackPacket->setType(ACK_TMAC_PACKET);
        ackPacket->setByteLength(ackPacketSize);
        ackPacket->setBufferSize(TXBuffer.size());

        toRadioLayer(ackPacket);
        toRadioLayer(createRadioCommand(SET_STATE, TX));
        ackPacket = NULL;
        trace() << self << " sends an ack at:" << getClock();
        setMacState(MAC_STATE_WAIT_FOR_GACK, "sensor sends ack");
        setTimer(WAIT_GACK_TIMEOUT, waitTimeout);
    }
    else
    {
        setMacState(MAC_STATE_SLEEP, "sensor's ack arrives maxmum value");
        toRadioLayer(createRadioCommand(SET_STATE, SLEEP));
        setTimer(START_LISTENING, sleepInterval);
    }
}
