/*
	NovaGenesis
	
	Name:		CoreSCNAck01
	Object:		CoreSCNAck01
	File:		CoreSCNAck01.h
	Author:		Antonio M. Alberti
	Date:		11/2012
	Version:	0.1
*/

#ifndef _CORESCNACK01_H
#define _CORESCNACK01_H

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

#define ERROR 1
#define OK 0

class Block;

class CoreSCNAck01: public Action
{
	public:

		// Constructor
		CoreSCNAck01(string _LN, Block *_PB, MessageBuilder *_PMB);

		// Destructor
		virtual ~CoreSCNAck01();

		// Run the actions behind a received message
		virtual int Run(Message *_ReceivedMessage, CommandLine *_PCL, vector<Message*> &ScheduledMessages, Message *&InlineResponseMessage);
};

#endif






