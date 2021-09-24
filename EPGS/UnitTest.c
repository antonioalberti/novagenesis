/*
	NovaGenesis

	Name:		EPGS unit tester
	Object:		UnitTest
	File:		UnitTest.c
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

#include "epgs_wrapper.h"
#include "epgs.h"
#include <stdio.h>
#include <stdlib.h>

//int main(int argc, char *argv[]) {
//
//	char msgStr[316] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xAB, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x12, 0x34, 0x05, 0x45, 0x07, 0x02, 0x00, 0x00, 0x00, 0x00, 0x0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x1D};
//	ng_sprintf(msgStr + 30, "%s", "ng -m --cl 0.1 [ < 1 s 15B239D1 > < 4 s 0BD95286 ED12F3ED CE362E2E 4479BCFE > < 4 s empty empty empty empty > ]\nng -hello --ihc 0.2 [ < 6 s 51A6873B E4980E41 E86E7B38 Ethernet em1 ac:22:0b:c9:df:3b > < 4 s 0BD95286 ED12F3ED DECC2634 82C5E549 > ]\nng -scn --seq 0.1 [ < 1 s 4B00DED6 > ]\n");
//
//	char msgStrHello[261] = {0xA1, 0xB2, 0xC3, 0xD4, 0xE5, 0xF6, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x12, 0x34, 0x15, 0x25, 0x47, 0xF2, 0x00, 0x00, 0x00, 0x00, 0x0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE6};
//	ng_sprintf(msgStrHello + 30, "%s", "ng -m --cl 0.1 [ < 1 s 15B239D1 > < 4 s ABCD5286 ED12F3ED CE362E2E NULL > < 4 s empty empty empty empty > ]\nng -hello --ihc 0.1 [ < 6 s NULL NULL NULL Ethernet eth0 a1:b2:c3:d4:e5:f6 > ]\nng -scn --seq 0.1 [ < 1 s 4B00DED6 > ]\n");
//
//	char msgStr2[322] = {0xAC, 0x22, 0x0B, 0xC9, 0xDF, 0x3B, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x12, 0x34, 0x05, 0x66, 0x03, 0x02, 0x00, 0x00, 0x00, 0x00, 0x0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x23};
//	ng_sprintf(msgStr2 + 30, "%s", "ng -m --cl 0.1 [ < 1 s 15B239D1 > < 4 s 0BD95286 ED12F3ED CE362E2E 4479BCFE > < 4 s 7E3FFFA6 027552A1 7CE42568 NULL > ]\nng -notify --s 0.1 [ < 1 s _CATEGORY > < 1 s _KEY_VJM > < 4 s _PUBLISHER_HID _PUBLISHER_OSID _PUBLISHER_PID _PUBLISHER_BID > ]\nng -scn --seq 0.1 [ < 1 s _HASH_MESSAGE > ]\n");
//
//	char msgStr2Peer[322] = {0xAE, 0x22, 0x0B, 0xC9, 0xDF, 0x3B, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x12, 0x34, 0x65, 0x86, 0x43, 0x72, 0x00, 0x00, 0x00, 0x00, 0x0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x23};
//	ng_sprintf(msgStr2Peer + 30, "%s", "ng -m --cl 0.1 [ < 1 s 15B239D1 > < 4 s 0BD95286 ED12F3ED CE362E2E 4479BCFE > < 4 s 0BD95286 ED12F3ED DECC2634 82C5E549 > ]\nng -notify --s 0.1 [ < 1 s _CATEGORY > < 1 s _KEY_VJM > < 4 s _PUBLISHER_HID _PUBLISHER_OSID _PUBLISHER_PID _PUBLISHER_BID > ]\nng -scn --seq 0.1 [ < 1 s _HASH_MESSAGE > ]\n");
//
//	char msgStr3Peer[322] = {0xAF, 0x22, 0x0B, 0xC9, 0xDF, 0x3B, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x12, 0x34, 0x65, 0x86, 0x43, 0x72, 0x00, 0x00, 0x00, 0x00, 0x0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x23};
//	ng_sprintf(msgStr3Peer + 30, "%s", "ng -m --cl 0.1 [ < 1 s 15B239D1 > < 4 s FBD95286 ED12F3ED CE362E2E 4479BCFE > < 4 s BBCD5286 ED12F3ED CE362E2E NULL > ]\nng -notify --s 0.1 [ < 1 s _CATEGORY > < 1 s _KEY_VJM > < 4 s _PUBLISHER_HID _PUBLISHER_OSID _PUBLISHER_PID _PUBLISHER_BID > ]\nng -scn --seq 0.1 [ < 1 s _HASH_MESSAGE > ]\n");
//
//	char msgStr3[1201] =  {0x12, 0x34, 0x56, 0x78, 0x9A, 0xAB, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x12, 0x34, 0x05, 0x04, 0x03, 0x07, 0x00, 0x00, 0x00, 0x00, 0x0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x64};
//	ng_sprintf(msgStr3 + 30, "%s", "ng -m --cl 0.1 [ < 1 s 15B239D1 > < 4 s 0BD95286 ED12F3ED CE362E2E 4479BCFE > < 4 s 7E3FFFA6 027552A1 7CE42568 NULL > ]\nng -d --b 0.1 [ < 1 s 18 > < 1 s _KEY_VJM > < 1 s Service_Acceptance.txt > ]\nng -info --payload 0.1 [ < 1 s Service_Acceptance.txt > ]\nng -d --b 0.1 [ < 1 s 2 > < 1 s _KEY > < 3 s PUB_OSID PUB__PID PUB__BID > ]\nng -d --b 0.1 [ < 1 s 9 > < 1 s _KEY > < 1 s PUB__PID > ]\nng -message --type 0.1 [ < 1 s 1 > ]\nng -scn --ack 0.1 [ < 2 s EADFC501 1D293F1B > ]\nng -scn --ack 0.1 [ < 2 s D2F5B7E1 7865E5D4 > ]\n\nAPP_HID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP");
//
//	char msgStr3b[233] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xAB, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x12, 0x34, 0x05, 0x04, 0x03, 0x07, 0x00, 0x00, 0x00, 0x01};
//	ng_sprintf(msgStr3b + 22, "%s", "_PID APP_BID APP_HID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID");
//
//	/*char *dataP = (char*)ng_calloc(sizeof(char), 1727);
//	dataP = "Burgess nota, com a indulg�ncia a que as suas pr�prias contradi��es o obrigam, a ironia de um socialista chamar socialismo ao regime mais monstruoso que consegue imaginar; mas n�o precisa de explicar, e n�o explica, as raz�es �bvias desta op��o. N�s, habitantes do S�culo XXI, habituados pela propaganda vigente a equacionar \"esquerda\" com \"estatismo\", tamb�m podemos ver ironia na escolha deste nome. As raz�es de Burgess para notar esta ironia s�o, contudo, um pouco diferentes das nossas. Burgess n�o era um anti-estatista doutrin�rio, mas sim um conservador na tradi��o burkeana, a quem a ideologia anarco-capitalista e revolucion�ria representada por Margaret Thatcher e Ronald Reagan repugnaria tanto como a qualquer militante da esquerda dita radical. N�o acredita que o Estado seja a emana��o do Mal, mas exige dele essa coisa fora de moda que � a responsabilidade moral. No cap�tulo \"Clockwork oranges\" de \"1985\", declara os seus pressupostos �tico-pol�ticos: A chemical substance injected into [Alex's] blood induces nausea while he is watching the films, but the nausea is also associated with the music. It was not the intention of his State manipulators to introduce this bonus or malus: it is purely an accident that, from now on, he will automatically react to Mozart or Beethoven as he will to rape or murder. The State has succedeed in its primary aim: to deny Alex ng_free moral choice, which, to the State, means choice of evil. But it has added an unforeseen punishment: the gates of heaven are closed to the boy, since music is a figure of celestial bliss. The State has commited a double sin: it has destroyed a human being, since humanity is defined by moral choice; it has also destroyed an angel.";
//	int dataPSize = (int)ng_strlen(dataP);*/
//
//	/*char msgStr[316] = {0x05, 0x45, 0x07, 0x02, 0x00, 0x00, 0x00, 0x00, 0x0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x1D};
//	ng_sprintf(msgStr + 16, "%s", "ng -m --cl 0.1 [ < 1 s 15B239D1 > < 4 s 0BD95286 ED12F3ED CE362E2E 4479BCFE > < 4 s empty empty empty empty > ]\nng -hello --ihc 0.2 [ < 6 s 51A6873B E4980E41 E86E7B38 Bluetooth em1 ac:22:0b:c9:df:3b > < 4 s 0BD95286 ED12F3ED DECC2634 82C5E549 > ]\nng -scn --seq 0.1 [ < 1 s 4B00DED6 > ]\n");
//
//	char msgStr2[318] = {0x05, 0x66, 0x03, 0x02, 0x00, 0x00, 0x00, 0x00, 0x0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x1F};
//	ng_sprintf(msgStr2 + 16, "%s", "ng -m --cl 0.1 [ < 1 s 15B239D1 > < 4 s PSS_HID PSS_OSID PSS_PID PS_BID > < 4 s PSS_HID PSS_OSID PSS_PID PS_BID > ]\nng -notify --s 0.1 [ < 1 s _CATEGORY > < 1 s _KEY_VJM > < 4 s _PUBLISHER_HID _PUBLISHER_OSID _PUBLISHER_PID _PUBLISHER_BID > ]\nng -scn --seq 0.1 [ < 1 s _HASH_MESSAGE > ]\n");
//
//	char msgStr3[1025] =  {0x05, 0x04, 0x03, 0x07, 0x00, 0x00, 0x00, 0x00, 0x0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x76};
//	ng_sprintf(msgStr3 + 16, "%s", "ng -m --cl 0.1 [ < 1 s 15B239D1 > < 4 s HTS_HID HTS_OSID HTS_PID HT_BID > < 4 s EPGS_HID EPGS_OSID EPGS_PID NULL > ]\nng -d --b 0.1 [ < 1 s 18 > < 1 s _KEY_VJM > < 1 s Service_Acceptance.txt > ]\nng -info --payload 0.1 [ < 1 s Service_Acceptance.txt > ]\nng -d --b 0.1 [ < 1 s 2 > < 1 s _KEY > < 3 s PUBLISHER_OSID PUBLISHER_PID PUBLISHER_BID > ]\nng -d --b 0.1 [ < 1 s 9 > < 1 s _KEY > < 1 s PUBLISHER_PID > ]\nng -message --type 0.1 [ < 1 s 1 > ]\nng -scn --ack 0.1 [ < 2 s EADFC501 1D293F1B > ]\nng -scn --ack 0.1 [ < 2 s D2F5B7E1 7865E5D4 > ]\n\nAPP_HID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID APP_H");
//
//	char msgStr3b[399] = {0x05, 0x04, 0x03, 0x07, 0x00, 0x00, 0x00, 0x01};
//	ng_sprintf(msgStr3b + 8, "%s", "ID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID");
//
//*/
//
//
//	NgEPGS *ngInfo = NULL;
//
//	initEPGS(&ngInfo);
//
//	setHwConfigurations(&ngInfo, "123452", "54321", "1222", "Wifi", "eth0", "00:12:34:AB:CD:EF");
//
//	addKeyWords(&ngInfo, "Term�metro");
//
//	addHwSensorFeature(&ngInfo, "sensorType", "Temperature");
//	addHwSensorFeature(&ngInfo, "sensorRangeMin", "-20");
//	addHwSensorFeature(&ngInfo, "sensorRangeMax", "100");
//	addHwSensorFeature(&ngInfo, "sensorResolution", "0.1");
//	addHwSensorFeature(&ngInfo, "sensorAccuracy", "0.2");
//
//	enablePeriodicHello(&ngInfo, true);
//
//	int dataPSize;
//	FILE *pFile;
//	pFile=fopen("D:\\Mestrado\\novagenesis\\NG_IoT\\EPGS\\EPGS\\Debug\\img.jpg", "rb");
//
//	// obtain file size:
//	fseek (pFile , 0 , SEEK_END);
//	dataPSize = ftell (pFile);
//	rewind (pFile);
//
//	char *dataP  = (char*) malloc (sizeof(char)*dataPSize);
//	fread (dataP,1,dataPSize,pFile);
//
//
//	while(true) {
//
//		ng_printf("NG STATE: %d\n\n", ngInfo->ngState);
//
//		processLoop(&ngInfo);
//
//		enablePeriodicHello(&ngInfo, false);
//
//		if(ngInfo->ngState == HELLO) {
//			newEthernetReceivedMessage(&ngInfo, msgStr, 315);
//		}
//
//		if(ngInfo->ngState == WAIT_SERVICE_ACCEPTANCE_NOTIFY) {
//			newEthernetReceivedMessage(&ngInfo, msgStr2,  321);
//		}
//
//		if(ngInfo->ngState == WAIT_SERVICE_ACCEPTANCE_DELIVERY) {
//			newEthernetReceivedMessage(&ngInfo, msgStr3,  1200);
//			newEthernetReceivedMessage(&ngInfo, msgStr3b, 232);
//		}
//
//
////		if(ngInfo->ngState == WAIT_HELLO_PGCS) {
////			newBLEReceivedMessage(&ngInfo, msgStr, 301);
////		}
////
////		if(ngInfo->ngState == WAIT_SERVICE_ACCEPTANCE_NOTIFY) {
////			newBLEReceivedMessage(&ngInfo, msgStr2,  303);
////		}
////
////		if(ngInfo->ngState == WAIT_SERVICE_ACCEPTANCE_DELIVERY) {
////			newBLEReceivedMessage(&ngInfo, msgStr3,  1024);
////			newBLEReceivedMessage(&ngInfo, msgStr3b, 398);
////		}
//
//		if(ngInfo->ngState == PUB_DATA) {
//			setDataToPub(&ngInfo, "img.png", dataP, dataPSize);
//
//			//newEthernetReceivedMessage(&ngInfo, msgStrHello, 260);
//
//			newEthernetReceivedMessage(&ngInfo, msgStr2Peer,  321);
//
//			//newEthernetReceivedMessage(&ngInfo, msgStr3Peer,  321);
//		}
//		Sleep(3000);
//
//	}
//	ng_free(dataP);
//	destroy_NgEPGS(&ngInfo);
//	return 1;
//}

int main_x (int argc, char *argv[])
{

  char msgStr[316] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xAC, 0x22, 0x0B, 0xc9, 0xDF, 0x3B, 0x12, 0x34, 0x05, 0x45,
					  0x07, 0x02, 0x00, 0x00, 0x00, 0x00, 0x0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x1D};
  ng_sprintf (msgStr
			  + 30, "%s", "ng -m --cl 0.1 [ < 1 s 15B239D1 > < 4 s 0BD95286 ED12F3ED CE362E2E 4479BCFE > < 4 s empty empty empty empty > ]\nng -hello --ihc 0.2 [ < 6 s 51A6873B E4980E41 E86E7B38 Ethernet em1 ac:22:0b:c9:df:3b > < 4 s 0BD95286 ED12F3ED DECC2634 82C5E549 > ]\nng -scn --seq 0.1 [ < 1 s 4B00DED6 > ]\n");

  char msgStrHello[257] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xA1, 0xB2, 0xC3, 0xD4, 0xE5, 0xF6, 0x12, 0x34, 0x15,
						   0x25, 0x47, 0xF2, 0x00, 0x00, 0x00, 0x00, 0x0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE2};
  ng_sprintf (msgStrHello
			  + 30, "%s", "ng -m --cl 0.1 [ < 1 s 15B239D1 > < 4 s ABCD5286 ED12F3ED CE362E2E NULL > < 4 s empty empty empty empty > ]\nng -hello --ihc 0.1 [ < 6 s NULL NULL NULL Ethernet eth0 a1:b2:c3:d4:e5:f6 > ]\nng -scn --seq 0.1 [ < 1 s 4B00DED6 > ]\n");

  char msgStr2[285] = {0x00, 0x12, 0x34, 0x56, 0x78, 0x9A, 0xAC, 0x22, 0x0B, 0xC9, 0xDF, 0x3B, 0x12, 0x34, 0x05, 0x66,
					   0x03, 0x02, 0x00, 0x00, 0x00, 0x00, 0x0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE};
  ng_sprintf (msgStr2
			  + 30, "%s", "ng -m --cl 0.1 [ < 1 s 15B239D1 > < 4 s 0BD95286 ED12F3ED CE362E2E 4479BCFE > < 4 s 7E3FFFA6 027552A1 7CE42568 NULL > ]\nng -notify --s 0.1 [ < 1 s 18 > < 1 s 12BC34CD > < 4 s 0BD95286 ED12F3ED A1C31B34 BEAC1234 > ]\nng -scn --seq 0.1 [ < 1 s 5B64CDA1 > ]\n");

  char msgStr2PeerData[479] = {0x00, 0x12, 0x34, 0x56, 0x78, 0x9A, 0xA1, 0xB2, 0xC3, 0xD4, 0xE5, 0xF6, 0x12, 0x34, 0x14,
							   0x36, 0x44, 0xB2, 0x00, 0x00, 0x00, 0x00, 0x0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xC0};
  ng_sprintf (msgStr2PeerData
			  + 30, "%s", "ng -m --cl 0.1 [ < 1 s 15B239D1 > < 4 s 7E3FFFA6 027552A1 7CE42568 NULL > < 4 s 0BD95286 ED12F3ED DECC2634 82C5E549 > ]\nng -p --notify 0.1 [ < 1 s 18 > < 1 s 876BC23A > < 1 s Room_1_Access.json > < 5 s pub APP_HID APP_OSID APP_PID APP_BID > ]\nng -info --payload 0.1 [ < 1 s Room_1_Access.json > ]\nng -message --type 0.1 [ < 1 s 1 > ]\nng -message --seq 0.1 [ < 1 s 15 > ]\nng -scn --seq 0.1 [ < 1 s 8B616B6D > ]\n\n{ Male:1220, Female:456, Total:1676 }");

  char msgStr3[597] = {0x00, 0x12, 0x34, 0x56, 0x78, 0x9A, 0xAC, 0x22, 0x0B, 0xc9, 0xDF, 0x3B, 0x12, 0x34, 0x05, 0x04,
					   0x03, 0x07, 0x00, 0x00, 0x00, 0x00, 0x0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x36};
  ng_sprintf (msgStr3
			  + 30, "%s", "ng -m --cl 0.1 [ < 1 s 15B239D1 > < 4 s 0BD95286 ED12F3ED CE362E2E 4479BCFE > < 4 s 7E3FFFA6 027552A1 7CE42568 NULL > ]\nng -d --b 0.1 [ < 1 s 18 > < 1 s 12BC34CD > < 1 s Service_Acceptance.txt > ]\nng -info --payload 0.1 [ < 1 s Service_Acceptance.txt > ]\nng -d --b 0.1 [ < 1 s 2 > < 1 s 12BC34CD > < 3 s ED12F3ED A1C31B34 BEAC1234 > ]\nng -d --b 0.1 [ < 1 s 9 > < 1 s 12BC34CD > < 1 s 0BD95286 > ]\nng -message --type 0.1 [ < 1 s 1 > ]\nng -scn --ack 0.1 [ < 2 s EADFC501 1D293F1B > ]\nng -scn --ack 0.1 [ < 2 s D2F5B7E1 7865E5D4 > ]\n\n0BD95286 ED12F3ED A1C31B34 BEAC1234");

  char msgStrExpo[1201] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xAC, 0x22, 0x0B, 0xc9, 0xDF, 0x3B, 0x12, 0x34, 0x05,
						   0x45, 0x07, 0x02, 0x00, 0x00, 0x00, 0x00, 0x0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x5B};
  ng_sprintf (msgStrExpo
			  + 30, "ng -m --cl 0.1 [ < 1 s 15B239D1 > < 4 s 0BD95286 ED12F3ED CE362E2E 4479BCFE > < 4 s 7E3FFFA6 027552A1 7CE42568 NULL > ]\nng -p --b 0.1 [ < 1 s 2 > < 1 s BEE09CC1 > < 1 s CE362E2E > ]\nng -p --b 0.1 [ < 1 s 1 > < 1 s BEE09CC1 > < 1 s PGCS > ]\nng -p --b 0.1 [ < 1 s 2 > < 1 s D051B60B > < 1 s CE362E2E > ]\nng -p --b 0.1 [ < 1 s 1 > < 1 s D051B60B > < 1 s Core > ]\nng -p --b 0.1 [ < 1 s 2 > < 1 s 2AA3A2EF > < 1 s CE362E2E > ]\nng -p --b 0.1 [ < 1 s 1 > < 1 s 2AA3A2EF > < 1 s Gateway > ]\nng -p --b 0.1 [ < 1 s 2 > < 1 s D5546D53 > < 1 s CE362E2E > ]\nng -p --b 0.1 [ < 1 s 1 > < 1 s D5546D53 > < 1 s Proxy > ]\nng -p --b 0.1 [ < 1 s 2 > < 1 s 423FCDDC > < 1 s CE362E2E > ]\nng -p --b 0.1 [ < 1 s 1 > < 1 s 423FCDDC > < 1 s Controller > ]\nng -p --b 0.1 [ < 1 s 2 > < 1 s 58FA80A5 > < 1 s CE362E2E > ]\nng -p --b 0.1 [ < 1 s 1 > < 1 s 58FA80A5 > < 1 s IoT > ]\nng -p --b 0.1 [ < 1 s 2 > < 1 s 19656CF3 > < 1 s CE362E2E > ]\nng -p --b 0.1 [ < 1 s 1 > < 1 s 19656CF3 > < 1 s Wi-Fi > ]\nng -p --b 0.1 [ < 1 s 2 > < 1 s D051B60B > < 1 s CE362E2E > ]\nng -p --b 0.1 [ < 1 s 1 > < 1 s D051B60B > < 1 s Core > ]\nng -p --b 0.1 [ < 1 s 2 > < 1 s BEE09CC1 > < 1 s CE362E2E > ]\nng -p --b 0.1 [ < ");

  char msgStrExpo2[480] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xAC, 0x22, 0x0B, 0xc9, 0xDF, 0x3B, 0x12, 0x34, 0x05,
						   0x45, 0x07, 0x02, 0x00, 0x00, 0x00, 0x01, 0x0};
  ng_sprintf (msgStrExpo2
			  + 22, "1 s 2 > < 1 s D051B60B > < 1 s 4479BCFE > ]\nng -p --b 0.1 [ < 1 s 5 > < 1 s CE362E2E > < 1 s 4479BCFE > ]\nng -p --b 0.1 [ < 1 s 5 > < 1 s ED12F3ED > < 1 s CE362E2E > ]\nng -p --b 0.1 [ < 1 s 6 > < 1 s 0BD95286 > < 1 s ED12F3ED > ]\nng -p --b 0.1 [ < 1 s 9 > < 1 s Host > < 1 s 0BD95286 > ]\nng -p --b 0.1 [ < 1 s 2 > < 1 s OS > < 1 s ED12F3ED > ]\nng -message --type 0.1 [ < 1 s 1 > ]\nng -message --seq 0.1 [ < 1 s 35 > ]\nng -scn --seq 0.1 [ < 1 s EBF2BD6B > ]\n");

  /*char *dataP = (char*)ng_calloc(sizeof(char), 1727);
  dataP = "Burgess nota, com a indulg�ncia a que as suas pr�prias contradi��es o obrigam, a ironia de um socialista chamar socialismo ao regime mais monstruoso que consegue imaginar; mas n�o precisa de explicar, e n�o explica, as raz�es �bvias desta op��o. N�s, habitantes do S�culo XXI, habituados pela propaganda vigente a equacionar \"esquerda\" com \"estatismo\", tamb�m podemos ver ironia na escolha deste nome. As raz�es de Burgess para notar esta ironia s�o, contudo, um pouco diferentes das nossas. Burgess n�o era um anti-estatista doutrin�rio, mas sim um conservador na tradi��o burkeana, a quem a ideologia anarco-capitalista e revolucion�ria representada por Margaret Thatcher e Ronald Reagan repugnaria tanto como a qualquer militante da esquerda dita radical. N�o acredita que o Estado seja a emana��o do Mal, mas exige dele essa coisa fora de moda que � a responsabilidade moral. No cap�tulo \"Clockwork oranges\" de \"1985\", declara os seus pressupostos �tico-pol�ticos: A chemical substance injected into [Alex's] blood induces nausea while he is watching the films, but the nausea is also associated with the music. It was not the intention of his State manipulators to introduce this bonus or malus: it is purely an accident that, from now on, he will automatically react to Mozart or Beethoven as he will to rape or murder. The State has succedeed in its primary aim: to deny Alex ng_free moral choice, which, to the State, means choice of evil. But it has added an unforeseen punishment: the gates of heaven are closed to the boy, since music is a figure of celestial bliss. The State has commited a double sin: it has destroyed a human being, since humanity is defined by moral choice; it has also destroyed an angel.";
  int dataPSize = (int)ng_strlen(dataP);*/

  /*char msgStr[316] = {0x05, 0x45, 0x07, 0x02, 0x00, 0x00, 0x00, 0x00, 0x0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x1D};
  ng_sprintf(msgStr + 16, "%s", "ng -m --cl 0.1 [ < 1 s 15B239D1 > < 4 s 0BD95286 ED12F3ED CE362E2E 4479BCFE > < 4 s empty empty empty empty > ]\nng -hello --ihc 0.2 [ < 6 s 51A6873B E4980E41 E86E7B38 Bluetooth em1 ac:22:0b:c9:df:3b > < 4 s 0BD95286 ED12F3ED DECC2634 82C5E549 > ]\nng -scn --seq 0.1 [ < 1 s 4B00DED6 > ]\n");

  char msgStr2[318] = {0x05, 0x66, 0x03, 0x02, 0x00, 0x00, 0x00, 0x00, 0x0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x1F};
  ng_sprintf(msgStr2 + 16, "%s", "ng -m --cl 0.1 [ < 1 s 15B239D1 > < 4 s PSS_HID PSS_OSID PSS_PID PS_BID > < 4 s PSS_HID PSS_OSID PSS_PID PS_BID > ]\nng -notify --s 0.1 [ < 1 s _CATEGORY > < 1 s _KEY_VJM > < 4 s _PUBLISHER_HID _PUBLISHER_OSID _PUBLISHER_PID _PUBLISHER_BID > ]\nng -scn --seq 0.1 [ < 1 s _HASH_MESSAGE > ]\n");

  char msgStr3[1025] =  {0x05, 0x04, 0x03, 0x07, 0x00, 0x00, 0x00, 0x00, 0x0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x76};
  ng_sprintf(msgStr3 + 16, "%s", "ng -m --cl 0.1 [ < 1 s 15B239D1 > < 4 s HTS_HID HTS_OSID HTS_PID HT_BID > < 4 s EPGS_HID EPGS_OSID EPGS_PID NULL > ]\nng -d --b 0.1 [ < 1 s 18 > < 1 s _KEY_VJM > < 1 s Service_Acceptance.txt > ]\nng -info --payload 0.1 [ < 1 s Service_Acceptance.txt > ]\nng -d --b 0.1 [ < 1 s 2 > < 1 s _KEY > < 3 s PUBLISHER_OSID PUBLISHER_PID PUBLISHER_BID > ]\nng -d --b 0.1 [ < 1 s 9 > < 1 s _KEY > < 1 s PUBLISHER_PID > ]\nng -message --type 0.1 [ < 1 s 1 > ]\nng -scn --ack 0.1 [ < 2 s EADFC501 1D293F1B > ]\nng -scn --ack 0.1 [ < 2 s D2F5B7E1 7865E5D4 > ]\n\nAPP_HID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID APP_H");

  char msgStr3b[399] = {0x05, 0x04, 0x03, 0x07, 0x00, 0x00, 0x00, 0x01};
  ng_sprintf(msgStr3b + 8, "%s", "ID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID APP_HID APP_OSID APP_PID APP_BID");

*/
  NgEPGS *ngInfo = NULL;

  initEPGS (&ngInfo);

  setHwConfigurations (&ngInfo, "123452", "54321", "1222", "Wifi", "eth0", "00:12:34:AB:CD:EF");

  addKeyWords (&ngInfo, "Term�metro");

  addHwSensorFeature (&ngInfo, "sensorType", "Temperature");
  addHwSensorFeature (&ngInfo, "sensorRangeMin", "-20");
  addHwSensorFeature (&ngInfo, "sensorRangeMax", "100");
  addHwSensorFeature (&ngInfo, "sensorResolution", "0.1");
  addHwSensorFeature (&ngInfo, "sensorAccuracy", "0.2");

  enablePeriodicHello (&ngInfo, true);

  int dataPSize = 19;
  char *dataP = (char *)malloc (sizeof (char) * dataPSize);
  strcpy (dataP, "{ Temperature:32 }");

  int counter = 0;

#ifdef _DEBUG
  printf("...DEBUG...\n\n");
#endif

  while (true)
	{

	  processLoop (&ngInfo);

	  enablePeriodicHello (&ngInfo, false);

	  if (ngInfo->ngState == HELLO)
		{
		  newEthernetReceivedMessage (&ngInfo, msgStr, 315);

		  newEthernetReceivedMessage (&ngInfo, msgStrExpo, 1200);
		  newEthernetReceivedMessage (&ngInfo, msgStrExpo2, 480);
		}

	  if (ngInfo->ngState == WAIT_SERVICE_ACCEPTANCE_NOTIFY)
		{
		  newEthernetReceivedMessage (&ngInfo, msgStr2, 284);
		}

	  if (ngInfo->ngState == WAIT_SERVICE_ACCEPTANCE_DELIVERY)
		{
		  newEthernetReceivedMessage (&ngInfo, msgStr3, 596);
		}

	  if (ngInfo->ngState == PUB_DATA)
		{
		  setDataToPub (&ngInfo, "Temperature.json", dataP, dataPSize);
		}

	  /*if(counter == 7) {
		  newEthernetReceivedMessage(&ngInfo, msgStrHello, 256);
	  }

	  if(counter >= 10) {
		  newEthernetReceivedMessage(&ngInfo, msgStr2PeerData,  478);
	  }*/

	  sleep (3);
	  counter++;

	}
  ng_free (dataP);
  destroy_NgEPGS (&ngInfo);
  return 1;
}
