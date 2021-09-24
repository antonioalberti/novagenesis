/*
	NovaGenesis

	Name:		Embedded Proxy/Gateway Service
	Object:		epgs
	File:		epgs.h
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

#ifndef NG_EPGS_H_
#define NG_EPGS_H_

#include "DataStructures/epgs_structures.h"

/**
 *
 * Inits the EPGS data structure, flags and counters.
 *
 * @param ngEPGS Reference to the EPGS data structure pointer.
 *
 * @return OK if success, ERROR otherwise (@see epgs_defines.h)
 */
int initEPGS (NgEPGS **ngEPGS);

/**
 *
 * Destroy the EPGS data structure, flags and counters.
 *
 * @param ngEPGS Reference to the EPGS data structure pointer.
 *
 * @return OK if success, ERROR otherwise (@see epgs_defines.h)
 */
int destroyEPGS (NgEPGS **ngEPGS);

/**
 * Sets the Hardware configuration
 *
 * @param ngEPGS 		Reference to the EPGS data structure pointer.
 * @param hid 			The Hardware Id - Unique number that describes the hardware
 * @param soid			System Operation Id
 * @param pid			Process Id
 * @param Stack			Type of network stack used (eg: wi-fi, ethernet, bluetooth, ip-v6...)
 * @param Interface		Interface of the network (eg: eth0, em0...)
 * @param Identifier	The identifier of the network (eg: 00:12:34:56:78:9A)
 *
 * @return OK if success, ERROR otherwise (@see epgs_defines.h)
 */
int
setHwConfigurations (NgEPGS **ngEPGS, const char *hid, const char *soid, const char *pid, const char *Stack, const char *Interface, const char *Identifier);

/**
 * Adds keywords that specifies the Node
 *
 * @param ngEPGS 		Reference to the EPGS data structure pointer.
 * @param name			The keyword (eg: "Temperature").
 *
 * @return OK if success, ERROR otherwise (@see epgs_defines.h)
 */
int addKeyWords (NgEPGS **ngEPGS, const char *name);

/**
 * Adds sensor features in name/value way.
 *
 * @param ngEPGS 		Reference to the EPGS data structure pointer.
 * @param name			The name of the feature (eg: Temperature_Max).
 * @param value			The value of the feature (eg: 100).
 *
 * @return OK if success, ERROR otherwise (@see epgs_defines.h)
 */
int addHwSensorFeature (NgEPGS **ngEPGS, const char *name, const char *value);

/**
 * Analysis the EPGS state and takes actions. This function must be called in a loop.
 *
 * @param ngEPGS 		Reference to the EPGS data structure pointer.
 *
 * @return State Machine ID (@see epgs_structures.h)
 */
int processLoop (NgEPGS **ngEPGS);

/**
 * Parses the received message and takes actions.
 *
 * @param ngEPGS 		Reference to the EPGS data structure pointer.
 * @param message		The received message.
 * @param msgSize		The size of the received message.
 *
 * @return OK if success, ERROR otherwise (@see epgs_defines.h)
 */
int newEthernetReceivedMessage (NgEPGS **ngEPGS, const char *message, int msgSize);

/**
 * Parses the bluetooth received message and takes actions.
 *
 * @param ngEPGS 		Reference to the EPGS data structure pointer.
 * @param message		The received message.
 * @param msgSize		The size of the received message.
 *
 * @return OK if success, ERROR otherwise (@see epgs_defines.h)
 */
int newBLEReceivedMessage (NgEPGS **ngEPGS, const char *message, int msgSize);

/**
 * Sets the data to be publish in the next cycle.
 *
 * @param ngEPGS 		Reference to the EPGS data structure pointer.
 * @param fileName		The name of file to be published.
 * @param data			Data content of the file to be published.
 * @param dataLengh		Size of the data to be published.
 *
 * @return OK if success, ERROR otherwise (@see epgs_defines.h)
 */
int setDataToPub (NgEPGS **ngEPGS, const char *fileName, const char *data, int dataLength);

/**
 * Enables the periodic hello
 *
 * @param ngEPGS 		Reference to the EPGS data structure pointer.
 * @param enable		true: EPGS will send periodic hello message.
 * 						false: Disable periodic hello message.
 */
int enablePeriodicHello (NgEPGS **ngEPGS, bool enable);

#endif /* NG_EPGS_H_ */
