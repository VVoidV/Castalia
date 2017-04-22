//****************************************************************************
//*  Copyright: National ICT Australia,  2007 - 2011                         *
//*  Developed at the ATP lab, Networked Systems research theme              *
//*  Author(s): Athanassios Boulis, Yuriy Tselishchev                        *
//*  This file is distributed under the terms in the attached LICENSE file.  *
//*  If you do not find this file, copies can be found by writing to:        *
//*                                                                          *
//*      NICTA, Locked Bag 9013, Alexandria, NSW 1435, Australia             *
//*      Attention:  License Inquiry.                                        *
//*                                                                          *  
//****************************************************************************/

cplusplus {{
#include "MacPacket_m.h"
#include <vector>
}}

class MacPacket;

enum TmacPacket_type {
	BEACON_TMAC_PACKET = 1;
	ACK_TMAC_PACKET = 2;
	GACK_TMAC_PACKET = 3;
	OACK_TMAC_PACKET = 4;
	SACK_TMAC_PACKET = 5;
	DATA_TMAC_PACKET = 6;
}
;
packet TMacPacket extends MacPacket {
	// This a basic field, essential for any packet
	// size including source and destination field (found 
	// in the generic MacPAcket) is 9 bytes in total
	int type enum (TmacPacket_type);	// 1 byte
	//ACK
	int bufferSize;
	//G-ACK which node's ack had been receieved
	bool ackedNode[];
	//O-ACK 
	int transferOrder[];
	int O_ACK_bufferSize[];
}

