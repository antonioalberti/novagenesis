/*
	NovaGenesis
	
	Name:		Core Self-Certifying Name Sequence Version 01
	Object:		CoreSCNSeq01
	File:		CoreSCNSeq01.h
	Author:		Antonio M. Alberti
	Date:		11/2012
	Version:	0.1
*/

#ifndef _CORESCNSEQ01_H
#define _CORESCNSEQ01_H

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

class CoreSCNSeq01: public Action
{
	public:

		// Constructor
		CoreSCNSeq01(string _LN, Block *_PB, MessageBuilder *_PMB);

		// Destructor
		virtual ~CoreSCNSeq01();

		// Run the actions behind a received message
		virtual int Run(Message *_ReceivedMessage, CommandLine *_PCL, vector<Message*> &ScheduledMessages, Message *&InlineResponseMessage);
};

#endif






