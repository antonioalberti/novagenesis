/*
	NovaGenesis
	
	Name:		Application's core action to prepare a ng -pub --notify
	Object:		CoreRunPublish02
	File:		CoreRunPublish02.h
	Author:		Antonio M. Alberti
	Date:		02/2013
	Version:	0.2
*/

#ifndef _CORERUNPUBLISH02_H
#define _CORERUNPUBLISH02_H

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

#define ERROR 1
#define OK 0

class Block;

class CoreRunPublish02: public Action
{
	public:

		// Constructor
		CoreRunPublish02(string _LN, Block *_PB, MessageBuilder *_PMB);

		// Destructor
		virtual ~CoreRunPublish02();

		// Run the actions behind a received message
		virtual int Run(Message *_ReceivedMessage, CommandLine *_PCL, vector<Message*> &ScheduledMessages, Message *&InlineResponseMessage);

		// Auxiliar function
		void CreatePublishMessage(string _FileName, vector<Tuple*> &_PubNotify, vector<Tuple*> &_SubNotify);
};

#endif






