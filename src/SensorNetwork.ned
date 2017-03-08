//********************************************************************************
//*  Copyright: National ICT Australia,  2007 - 2010                             *
//*  Developed at the ATP lab, Networked Systems research theme                  *
//*  Author(s): Athanassios Boulis, Dimosthenis Pediaditakis, Yuriy Tselishchev  *
//*  This file is distributed under the terms in the attached LICENSE file.      *
//*  If you do not find this file, copies can be found by writing to:            *
//*                                                                              *
//*      NICTA, Locked Bag 9013, Alexandria, NSW 1435, Australia                 *
//*      Attention:  License Inquiry.                                            *
//*                                                                              *
//*******************************************************************************/

package castalia_3_2.src;

import castalia_3_2.src.wirelessChannel.*;
import castalia_3_2.src.physicalProcess.*;
import castalia_3_2.src.node.*;

network SN {
	
 parameters:
	int field_x = default (30);			// the length of the deployment field
	int field_y = default (30);			// the width of the deployment field
	int field_z = default (0);			// the height of the deployment field (2-D field by default)

	int numNodes;						// the number of nodes

	string deployment = default ("");

	int numPhysicalProcesses = default (1);
	string physicalProcessName = default ("CustomizablePhysicalProcess");
	string debugInfoFileName = default ("Castalia-Trace.txt");

 submodules:
	wirelessChannel:castalia_3_2.src.wirelessChannel.WirelessChannel {
	 gates:
		toNode[numNodes];
		fromNode[numNodes];
	} 

	physicalProcess[numPhysicalProcesses]: <physicalProcessName> 
				like castalia_3_2.src.physicalProcess.iPhysicalProcess {
	 gates:
		toNode[numNodes];
		fromNode[numNodes];
	}

	node[numNodes]:castalia_3_2.src.node.Node {
	 gates:
		toPhysicalProcess[numPhysicalProcesses];
		fromPhysicalProcess[numPhysicalProcesses];
	}

 connections:
	for i = 0..numNodes - 1 {
		node[i].toWirelessChannel --> wirelessChannel.fromNode[i];
		node[i].fromWirelessChannel <-- wirelessChannel.toNode[i];
	}

	for i = 0..numNodes - 1, for j = 0..numPhysicalProcesses - 1 {
		node[i].toPhysicalProcess[j] -->  physicalProcess[j].fromNode[i];
		node[i].fromPhysicalProcess[j] <-- physicalProcess[j].toNode[i];
	}
}
