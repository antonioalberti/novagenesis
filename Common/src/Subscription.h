/*
	NovaGenesis
	
	Name:		Auxiliary subscription object
	Object:		Subscription
	File:		Subscription.h
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

#ifndef _SUBSCRIPTION_H
#define _SUBSCRIPTION_H

#ifndef _STRING_H
#include <string>
#endif

#ifndef _TUPLE_H
#include "Tuple.h"
#endif

class Subscription
{
	public:

		// The publisher LN
		string				LN;

		// The publisher ULN
		string				ULN;

		// The category pf the bindings being subscribed
		unsigned int		Category;

		// The keys being subscribed
		string				Key;

		// The publisher of the bindings being subscribed
		Tuple				Publisher;

		// Auxiliary flag for associated -info --payload
		bool				HasContent;

		// Associated payload file
		string				FileName;

		// The subscription status
		string 				Status;

		// Timestamp for subscription performance
		double				Timestamp;
};

#endif






