/*
	NovaGenesis
	
	Name:		Auxiliary TCP IP message receiving class
	Object:		MessageReceiving
	File:		MessageReceiving.h
	Author:		Antonio Marcos Alberti
	Date:		05/2021
	Version:	0.1

 	Copyright (C) 2021  Antonio Marcos Alberti

    This work is available under the GNU Lesser General Public License (See COPYING.txt).

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef _MESSAGERECEIVING_H
#define _MESSAGERECEIVING_H

class MessageReceiving
{
	public:

		// The socket ID
		int					Number;

		// The message number
		unsigned int		MessageNumber;

		// The number of segments
		unsigned int		NoS;

		// The char message being received
		char 				*MessageBeingReceived;

		// The total size of a message being received
		long long int 		MessageSize;

		// The amount of bytes already received
		long long int 		ReceivedSoFar;

		// The amount of segments already received
		long long int 		SegmentsSoFar;

		// The receiving status
		bool 				ContinueReceiving;

		// Problem of extra 14 bytes at the end of the frames
		bool 				Problem;

  		// TODO: FIXP/Update - Added to timeout incomplete messages in PGCS PG Dispatcher Buffer
  		double 				Timestamp;
};

#endif






