/*
	NovaGenesis

	Name:		EPGS novagenesis data structures
	Object:		epgs_structures
	File:		epgs_structures.h
	Author:		Vâner José Magalhães
	Date:		05/2021
	Version:	0.1

  	Copyright (C) 2021  Vâner José Magalhães

    This work is available under the GNU General Public License (See COPYING.txt).

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef DATASTRUCTURES_EPGS_STRUCTURES_H_
#define DATASTRUCTURES_EPGS_STRUCTURES_H_

#include "../epgs_defines.h"

enum ng_states {
  HELLO = 1,
  WAIT_HELLO_PGCS,
  EXPOSITION,
  SERVICE_OFFER,
  WAIT_SERVICE_ACCEPTANCE_NOTIFY,
  SUBSCRIBE_SERVICE_ACCEPTANCE,
  WAIT_SERVICE_ACCEPTANCE_DELIVERY,
  PUB_DATA
};
typedef enum ng_states States;

struct _ng_net_info {

  // LIMITER
  char LIMITER[SVN_SIZE + 1];

  // HID
  char HID[SVN_SIZE + 1];
  // SOID
  char OSID[SVN_SIZE + 1];
  // PID
  char PID[SVN_SIZE + 1];
  // BID
  char BID[SVN_SIZE + 1];

  // GW_SCN
  char GW_SCN[SVN_SIZE + 1];
  // HT_SCN
  char HT_SCN[SVN_SIZE + 1];
  // CORE_BID_SCN
  char CORE_BID_SCN[SVN_SIZE + 1];
  // Stack
  char *Stack;                // "Ethernet"
  // Interface
  char *Interface;            // "eth0"
  // Identifier
  char *Identifier;            // "00:12:34:AB:CD:EF"

};

struct _ng_hw_descriptor {
  int keyWordsCounter;
  char **keyWords;

  int featureCounter;
  char **sensorFeatureName;    // "Accuracy; Resolution"
  char **sensorFeatureValue;    // "0.1; 0.1"

};

struct _ng_pgcs_info {

  // GW_SCN
  char GW_SCN[SVN_SIZE + 1];
  // HT_SCN
  char HT_SCN[SVN_SIZE + 1];
  // CORE_BID_SCN
  char CORE_BID_SCN[SVN_SIZE + 1];
  // Stack
  char *Stack;                // "wi-fi"
  // Interface
  char *Interface;            // "eth0"
  // Identifier
  char *Identifier;            // "00:12:34:AB:CD:EF"

};

struct _ng_scn_id_info {

  // HID
  char HID[SVN_SIZE + 1];
  // OSID
  char OSID[SVN_SIZE + 1];
  // PID
  char PID[SVN_SIZE + 1];
  // BID
  char BID[SVN_SIZE + 1];

};

struct _ng_received_msg {

  unsigned msg_id;

  int mgs_size;

  int number_of_frames;

  int frames_read;

  char *buffer;
};

struct _ng_data_to_pub {
  char *pubDataFileName;
  char *pubData;
  int pubDataSize;
};

struct _ng_epgs {
  // Descriptors and identifiers
  struct _ng_net_info *MyNetInfo;
  struct _ng_net_info *PGCSInfo;
  struct _ng_scn_id_info *PSSScnIDInfo;
  struct _ng_scn_id_info *APPScnIDInfo;
  struct _ng_hw_descriptor *HwDescriptor;

  // Message Relay - Peer's information
  int PeersNetInfoCount;
  struct _ng_net_info **PeersNetInfo;

  // Information about received messages
  struct _ng_received_msg *ReceivedMsg;

  // State of the EPGS
  States ngState;

  // Enable periodic Hello
  bool enableHello;

  // Counter of messages
  int MessageCounter;

  // Key of the contract
  char *key;

  // Information about the data to publish
  struct _ng_data_to_pub *DataToPub;

  char *pubDataFileName;
  char *pubData;
  int pubDataSize;

  // This Interface was added in June 2018 to avoid a static declaration in EPGS_Emulator
  // It avoids the usage of NG_INTERFACE_NAME[]
  char *Interface;
};

typedef struct _ng_net_info NgNetInfo;
typedef struct _ng_hw_descriptor NgHwDescriptor;
typedef struct _ng_pgcs_info NgPGCSInfo;
typedef struct _ng_scn_id_info NgScnIDInfo;
typedef struct _ng_received_msg NgReceivedMsg;
typedef struct _ng_epgs NgEPGS;

void destroy_NgNetInfo (struct _ng_net_info **ngHWInfo);
void destroy_NgHwDescriptor (struct _ng_hw_descriptor **ngHwDescriptor);
void destroy_NgPGCSInfo (struct _ng_pgcs_info **ngPeerInfo);
void destroy_NgScnIDInfo (struct _ng_scn_id_info **ngScnIDInfo);
void destroy_NgReceivedMsg (struct _ng_received_msg **ngReceivedMsg);
void destroy_NgEPGS (struct _ng_epgs **ngEPGS);

#endif /* DATASTRUCTURES_EPGS_STRUCTURES_H_ */
