/*
	NovaGenesis
	
	Name:		CoreRunPeriodic01
	Object:		CoreRunPeriodic01
	File:		CoreRunPeriodic01.h
	Author:		Antonio M. Alberti
	Date:		12/2012
	Version:	0.1
*/

#ifndef _CORERUNPERIODIC01_H
#define _CORERUNPERIODIC01_H

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

class CoreRunPeriodic01: public Action
{
	public:

		// Constructor
		CoreRunPeriodic01(string _LN, Block *_PB, MessageBuilder *_PMB);

		// Destructor
		virtual ~CoreRunPeriodic01();

		// Run the actions behind a received message
		virtual int Run(Message *_ReceivedMessage, CommandLine *_PCL, vector<Message*> &ScheduledMessages, Message *&InlineResponseMessage);
};

#endif






