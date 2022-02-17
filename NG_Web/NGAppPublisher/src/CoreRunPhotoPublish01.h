/*
	NovaGenesis
	
	Name:		CoreRunPhotoPublish01
	Object:		CoreRunPhotoPublish01
	File:		CoreRunPhotoPublish01.h
	Author:		Antonio M. Alberti
	Date:		01/2013
	Version:	0.1
*/

#ifndef _CORERUNPHOTOPUBLISH01_H
#define _CORERUNPHOTOPUBLISH01_H

#ifndef _STRING_H
#include <string>
#endif

#ifndef _ACTION_H
#include "Action.h"
#endif

#ifndef _MESSAGE_H
#include "Message.h"
#endif

#ifndef _MESSAGEBUILDER_H
#include "MessageBuilder.h"
#endif

#ifndef _STDIO_H
#include <stdio.h>
#endif

#ifndef _SYS_TYPES_H
#include <sys/types.h>
#endif

#ifndef _UNISTD_H
#include <unistd.h>
#endif

#ifndef _IOSTREAM_H
#include <iostream>
#endif

#ifndef _STDLIB_H
#include <stdlib.h>
#endif

#ifndef _DIRENT_H
#include <dirent.h>
#endif

#define ERROR 1
#define OK 0

class Block;

class CoreRunPhotoPublish01: public Action
{
	public:

		// Constructor
		CoreRunPhotoPublish01(string _LN, Block *_PB, MessageBuilder *_PMB);

		// Destructor
		virtual ~CoreRunPhotoPublish01();

		// Run the actions behind a received message
		virtual int Run(Message *_ReceivedMessage, CommandLine *_PCL, vector<Message*> &ScheduledMessages, Message *&InlineResponseMessage);

		// Publish a photo to all the peer applications - notifies just one peer
		int PublishAPhotoToAllPeerApps01(string _FileName, vector<Message*> &ScheduledMessages);

		// Publish a photo to all the peer applications - notifies all peers
		int PublishAPhotoToAllPeerApps02(string _FileName, vector<Message*> &ScheduledMessages);
};

#endif






