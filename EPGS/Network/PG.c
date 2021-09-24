/*
	NovaGenesis

	Name:		EPGS novagenesis proxy gateway
	Object:		PG
	File:		PG.c
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

#include "PG.h"
#include "../epgs_wrapper.h"
#include "../ng_util/ng_util.h"
#include "../Controller/epgs_controller.h"

/**
 * Fun��o para converter uma string MAC (11:22:33:44:55:66 com 17 caracteres) em
 * um vetor de bytes (6 bytes)
 */
void convertStrToMAC (char *macSTR, char **macBytes)
{

  char *destinationMAC = (char *)ng_malloc (sizeof (char) * 6);

  char *end_str;
  char *stringDestMAC = (char *)ng_calloc (sizeof (char) * 1, ng_strlen (macSTR) + 1);
  ng_strcpy (stringDestMAC, macSTR);

  char *token = strtok_r (stringDestMAC, ":", &end_str);
  int value = ng_strtoul (token, NULL, 16);
  destinationMAC[0] = value;

  token = strtok_r (NULL, ":", &end_str);
  value = ng_strtoul (token, NULL, 16);
  destinationMAC[1] = value;

  token = strtok_r (NULL, ":", &end_str);
  value = ng_strtoul (token, NULL, 16);
  destinationMAC[2] = value;

  token = strtok_r (NULL, ":", &end_str);
  value = ng_strtoul (token, NULL, 16);
  destinationMAC[3] = value;

  token = strtok_r (NULL, ":", &end_str);
  value = ng_strtoul (token, NULL, 16);
  destinationMAC[4] = value;

  token = strtok_r (NULL, ":", &end_str);
  value = ng_strtoul (token, NULL, 16);
  destinationMAC[5] = value;

  ng_free (stringDestMAC);
  //ng_free(token);
  //ng_free(end_str);
  *macBytes = destinationMAC;
}

/**
 * Fun��o de prepara��o e envio da mensagem NovaGenesis via Ethernet ou WiFi
 */
int forwardNGMessageToPeer (NgEPGS *ngEPGS, int peerID, NgMessage *message)
{

  if (!message || !ngEPGS || !ngEPGS->MyNetInfo || !ngEPGS->MyNetInfo->Identifier || !ngEPGS->PeersNetInfo)
	{
	  return NG_ERROR;
	}

  // Prepara o endere�o MAC de destino. Em caso de brodcast, somente setar o destino para FF:FF:FF:FF:FF:FF
  char *destinationMAC;

  if (ngEPGS->PeersNetInfo[peerID])
	{
	  if (ngEPGS->PeersNetInfo[peerID]->Identifier)
		{
		  convertStrToMAC (ngEPGS->PeersNetInfo[peerID]->Identifier, &destinationMAC);
		}
	  else
		{
		  return NG_ERROR;
		}
	}
  else
	{
	  return NG_ERROR;
	}

  ConvertMessageFromCommandLinesandPayloadCharArrayToCharArray (&message);

  // Prepara o MAC de origem (MAC da placa onde esta rodando o EPGS)
  char *sourceMAC;
  convertStrToMAC (ngEPGS->MyNetInfo->Identifier, &sourceMAC);

  // Aloca o SDU. SDU � composto pela mensagem NovaGenesis a ser enviada mais o
  // cabe�alho NovaGenesis de 8 bytes que indica o tamanho da mensagem NG a ser enviada.
  int payloadSize = HEADER_SIZE_FIELD_SIZE + message->MessageSize;
  char *sdu = (char *)ng_malloc (sizeof (char) * payloadSize);


  // O tamanho da mensagem NG a ser enviada � convertido para bytes e preenche o inicio do SDU,
  // que � o vetor de 8 bytes do cabe�alho NG referente ao tamanho.
  // Ex: Uma mensagem com tamanho 650, preencher� o cabe�alho da seguinte forma: '00 00 00 00 00 00 02 8A'
  int intSize = sizeof (int);
  int i = 0;

  if (intSize == 2)
	{
	  sdu[0] = 0;
	  sdu[1] = 0;
	  sdu[2] = 0;
	  sdu[3] = 0;
	  sdu[4] = 0;
	  sdu[5] = 0;
	  i = 6;
	}
  else if (intSize == 4)
	{
	  sdu[0] = 0;
	  sdu[1] = 0;
	  sdu[2] = 0;
	  sdu[3] = 0;
	  i = 4;
	}

  for (; i < HEADER_SIZE_FIELD_SIZE; i++)
	{
	  sdu[i] = (message->MessageSize >> (8 * (7 - i)));
	}

  // O restante do SDU � preenchido com a mensagem NG
  for (i = 0; i < message->MessageSize; i++)
	{
	  sdu[HEADER_SIZE_FIELD_SIZE + i] = message->Msg[i];
	}

  // Cada mensagem enviada tem o seu ID, que � um n�mero aleat�rio de 4 bytes
  unsigned MessageNumber = ng_rand ();
  MessageNumber = (MessageNumber << 16) | ng_rand ();

  int bytesSent = 0;

  // Dado o MTU, pode ser necess�rio fragmentar o SDU. Este método calcula quantos fragmentos ser�o necess�rios
  int numberOfMsgs = getNumberOfMessages (message->MessageSize);

  // O n�mero de sequencia indica qual � o fragmento da mensagem. Ser� 0 caso n�o seja necessaria a fragmenta��o
  unsigned int SequenceNumber = 0;

  // Este loop � executado para cada fragmento da mensagem. Ele � repons�vel por inserir o cabe�alho
  // Ethernet/Wifi (MAC de destino/origem e tipo) e tamb�m inserir o cabe�alho NG referente a fragmenta��o.
  int j = 0;
  for (j = 0; j < numberOfMsgs; j++)
	{

	  // MTU � alocado, com tamanho do Default MTU.
	  char *mtu = (char *)ng_calloc (sizeof (char) * 1, DEFAULT_MTU);

	  // Inserido no MTU parte do cabe�alho ethernet referente ao MAC de destino e origem
	  int index = 0;
	  for (i = 0; i < ETHERNET_MAC_ADDR_FIELD_SIZE; i++)
		{
		  mtu[index + i] = destinationMAC[i];
		  mtu[index + i + ETHERNET_MAC_ADDR_FIELD_SIZE] = sourceMAC[i];
		}
	  index += ETHERNET_MAC_ADDR_FIELD_SIZE + ETHERNET_MAC_ADDR_FIELD_SIZE; // Destination + Source

	  // Inserido no MTU parte do cabe�alho ethernet referenete o tipo. Por conve��o, o tipo NG � '1234'
	  mtu[index] = NG_TYPE_MSB;
	  mtu[index + 1] = NG_TYPE_LSB;
	  index += ETHERNET_TYPE_FIELD_SIZE;

	  // Os 8 bytes referentes ao cabe�alho NG de fragmenta��o s�o criados. Os 4 bytes mais significativos representam
	  // o id da mensagem (n�mero randomico gerado anteriormente). Os 4 bytes menos significativos representam o n�mero de sequencia do fragmento
	  // que ser� enviado. Caso n�o tenha fragmenta��o, este n�mero � 0.
	  unsigned char *HeaderSegmentationField = (unsigned char *)ng_malloc (sizeof (unsigned char) * 8);
	  HeaderSegmentationField[0] = (MessageNumber >> (8 * 3)) & 0xff;
	  HeaderSegmentationField[1] = (MessageNumber >> (8 * 2)) & 0xff;
	  HeaderSegmentationField[2] = (MessageNumber >> (8 * 1)) & 0xff;
	  HeaderSegmentationField[3] = (MessageNumber >> (8 * 0)) & 0xff;
	  HeaderSegmentationField[4] = (SequenceNumber >> (8 * 3)) & 0xff;
	  HeaderSegmentationField[5] = (SequenceNumber >> (8 * 2)) & 0xff;
	  HeaderSegmentationField[6] = (SequenceNumber >> (8 * 1)) & 0xff;
	  HeaderSegmentationField[7] = (SequenceNumber >> (8 * 0)) & 0xff;

	  // Cabe�alho NG de fragmenta��o � copiado para o MTU
	  for (i = 0; i < HEADER_SEGMENTATION_FIELD_SIZE; i++)
		{
		  mtu[index + i] = HeaderSegmentationField[i];
		}
	  ng_free (HeaderSegmentationField);

	  // N�mero de sequencia de fragmento � incrementado
	  SequenceNumber++;
	  index += HEADER_SEGMENTATION_FIELD_SIZE;

	  // A parte do SDU referente a este fragmento � copiado para o MTU.
	  // -index aponta para a pr�xima posi��o do MTU onde deve ser escrito o SDU.
	  // Os bytes do SDU s�o copiados para o MTU at� que o index atinga o tamanho MTU, finalizando o fragmento.
	  // -bytesSent reprensenta a quantidade de bytes do SDU que j� foi enviado e indica a partir de qual byte do SDU deve come�ar o proximo fragmento.
	  // No primeiro fragmento ele � zero. O ultimo fragmento pode n�o atingir o MTU, ent�o a c�pia deve parar quando o bytesSent atingir o tamanho do SDU (payloadSize).
	  for (; index < DEFAULT_MTU && bytesSent < payloadSize; index++)
		{
		  mtu[index] = sdu[bytesSent];
		  bytesSent++;
		}

	  // Wrapper da Fun��o da plataforma que faz o envio dos dados.
	  ng_EthernetSendData (mtu, index, ngEPGS->Interface);

	  ng_printf ("\nEthernet MSG forwarded:\n");
	  int k = 0;
	  for (k = 0; k < 30; k++)
		{
		  ng_printf ("%02X ", mtu[k] & 0xFF);
		}
	  ng_printf ("\n%s\n\n", message->Msg);

	  ng_free (mtu);
	}

  ng_free (sdu);
  ng_free (sourceMAC);
  ng_free (destinationMAC);

  return NG_OK;
}

/**
 * Fun��o de prepara��o e envio da mensagem NovaGenesis via Ethernet ou WiFi
 */
int sendNGMessage (NgEPGS *ngEPGS, NgMessage *message, bool isBroadcast)
{

  if (!message || !ngEPGS || !ngEPGS->MyNetInfo || !ngEPGS->MyNetInfo->Identifier)
	{
	  return NG_ERROR;
	}

  // Prepara o endere�o MAC de destino. Em caso de brodcast, somente setar o destino para FF:FF:FF:FF:FF:FF
  char *destinationMAC;
  if (!isBroadcast)
	{
	  if (ngEPGS->PGCSInfo)
		{
		  if (ngEPGS->PGCSInfo->Identifier)
			{
			  convertStrToMAC (ngEPGS->PGCSInfo->Identifier, &destinationMAC);
			}
		  else
			{
			  return NG_ERROR;
			}
		}
	  else
		{
		  return NG_ERROR;
		}

	}
  else
	{
	  destinationMAC = (char *)ng_malloc (sizeof (char) * 6);
	  destinationMAC[0] = 0xFF;
	  destinationMAC[1] = 0xFF;
	  destinationMAC[2] = 0xFF;
	  destinationMAC[3] = 0xFF;
	  destinationMAC[4] = 0xFF;
	  destinationMAC[5] = 0xFF;
	}

  // Prepara o MAC de origem (MAC da placa onde esta rodando o EPGS)
  char *sourceMAC;
  convertStrToMAC (ngEPGS->MyNetInfo->Identifier, &sourceMAC);

  // Aloca o SDU. SDU � composto pela mensagem NovaGenesis a ser enviada mais o
  // cabe�alho NovaGenesis de 8 bytes que indica o tamanho da mensagem NG a ser enviada.
  int payloadSize = HEADER_SIZE_FIELD_SIZE + message->MessageSize;
  char *sdu = (char *)ng_malloc (sizeof (char) * payloadSize);


  // O tamanho da mensagem NG a ser enviada � convertido para bytes e preenche o inicio do SDU,
  // que � o vetor de 8 bytes do cabe�alho NG referente ao tamanho.
  // Ex: Uma mensagem com tamanho 650, preencher� o cabe�alho da seguinte forma: '00 00 00 00 00 00 02 8A'
  int intSize = sizeof (int);
  int i = 0;

  if (intSize == 2)
	{
	  sdu[0] = 0;
	  sdu[1] = 0;
	  sdu[2] = 0;
	  sdu[3] = 0;
	  sdu[4] = 0;
	  sdu[5] = 0;
	  i = 6;
	}
  else if (intSize == 4)
	{
	  sdu[0] = 0;
	  sdu[1] = 0;
	  sdu[2] = 0;
	  sdu[3] = 0;
	  i = 4;
	}
  for (; i < HEADER_SIZE_FIELD_SIZE; i++)
	{
	  sdu[i] = (message->MessageSize >> (8 * (7 - i))) & 0xff;
	}

  // O restante do SDU � preenchido com a mensagem NG
  for (i = 0; i < message->MessageSize; i++)
	{
	  sdu[HEADER_SIZE_FIELD_SIZE + i] = message->Msg[i];
	}

  // Cada mensagem enviada tem o seu ID, que � um n�mero aleat�rio de 4 bytes
  unsigned MessageNumber = ng_rand ();
  MessageNumber = (MessageNumber << 16) | ng_rand ();

  int bytesSent = 0;

  // Dado o MTU, pode ser necess�rio fragmentar o SDU. Este método calcula quantos fragmentos ser�o necess�rios
  int numberOfMsgs = getNumberOfMessages (message->MessageSize);

  // O n�mero de sequencia indica qual � o fragmento da mensagem. Ser� 0 caso n�o seja necessaria a fragmenta��o
  unsigned int SequenceNumber = 0;

  // Este loop � executado para cada fragmento da mensagem. Ele � repons�vel por inserir o cabe�alho
  // Ethernet/Wifi (MAC de destino/origem e tipo) e tamb�m inserir o cabe�alho NG referente a fragmenta��o.
  int j = 0;
  for (j = 0; j < numberOfMsgs; j++)
	{

	  // MTU � alocado, com tamanho do Default MTU.
	  char *mtu = (char *)ng_calloc (sizeof (char) * 1, DEFAULT_MTU);

	  // Inserido no MTU parte do cabe�alho ethernet referente ao MAC de destino e origem
	  int index = 0;
	  for (i = 0; i < ETHERNET_MAC_ADDR_FIELD_SIZE; i++)
		{
		  mtu[index + i] = destinationMAC[i];
		  mtu[index + i + ETHERNET_MAC_ADDR_FIELD_SIZE] = sourceMAC[i];
		}
	  index += ETHERNET_MAC_ADDR_FIELD_SIZE + ETHERNET_MAC_ADDR_FIELD_SIZE; // Destination + Source

	  // Inserido no MTU parte do cabe�alho ethernet referenete o tipo. Por conve��o, o tipo NG � '1234'
	  mtu[index] = NG_TYPE_MSB;
	  mtu[index + 1] = NG_TYPE_LSB;
	  index += ETHERNET_TYPE_FIELD_SIZE;

	  // Os 8 bytes referentes ao cabe�alho NG de fragmenta��o s�o criados. Os 4 bytes mais significativos representam
	  // o id da mensagem (n�mero randomico gerado anteriormente). Os 4 bytes menos significativos representam o n�mero de sequencia do fragmento
	  // que ser� enviado. Caso n�o tenha fragmenta��o, este n�mero � 0.
	  unsigned char *HeaderSegmentationField = (unsigned char *)ng_malloc (sizeof (unsigned char) * 8);
	  HeaderSegmentationField[0] = (MessageNumber >> (8 * 3)) & 0xff;
	  HeaderSegmentationField[1] = (MessageNumber >> (8 * 2)) & 0xff;
	  HeaderSegmentationField[2] = (MessageNumber >> (8 * 1)) & 0xff;
	  HeaderSegmentationField[3] = (MessageNumber >> (8 * 0)) & 0xff;
	  HeaderSegmentationField[4] = (SequenceNumber >> (8 * 3)) & 0xff;
	  HeaderSegmentationField[5] = (SequenceNumber >> (8 * 2)) & 0xff;
	  HeaderSegmentationField[6] = (SequenceNumber >> (8 * 1)) & 0xff;
	  HeaderSegmentationField[7] = (SequenceNumber >> (8 * 0)) & 0xff;

	  // Cabe�alho NG de fragmenta��o � copiado para o MTU
	  for (i = 0; i < HEADER_SEGMENTATION_FIELD_SIZE; i++)
		{
		  mtu[index + i] = HeaderSegmentationField[i];
		}
	  ng_free (HeaderSegmentationField);

	  // N�mero de sequencia de fragmento � incrementado
	  SequenceNumber++;
	  index += HEADER_SEGMENTATION_FIELD_SIZE;

	  // A parte do SDU referente a este fragmento � copiado para o MTU.
	  // -index aponta para a pr�xima posi��o do MTU onde deve ser escrito o SDU.
	  // Os bytes do SDU s�o copiados para o MTU at� que o index atinga o tamanho MTU, finalizando o fragmento.
	  // -bytesSent reprensenta a quantidade de bytes do SDU que j� foi enviado e indica a partir de qual byte do SDU deve come�ar o proximo fragmento.
	  // No primeiro fragmento ele � zero. O ultimo fragmento pode n�o atingir o MTU, ent�o a c�pia deve parar quando o bytesSent atingir o tamanho do SDU (payloadSize).
	  for (; index < DEFAULT_MTU && bytesSent < payloadSize; index++)
		{
		  mtu[index] = sdu[bytesSent];
		  bytesSent++;
		}

	  // Wrapper da Fun��o da plataforma que faz o envio dos dados.
	  ng_EthernetSendData (mtu, index, ngEPGS->Interface);

	  ng_printf ("\nEthernet MSG sent:\n");
	  int k = 0;
	  for (k = 0; k < 30; k++)
		{
		  ng_printf ("%02X ", mtu[k] & 0xFF);
		}
	  ng_printf ("\n%s\n\n", message->Msg);

	  ng_free (mtu);
	}

  ng_free (sdu);
  ng_free (sourceMAC);
  ng_free (destinationMAC);

  return NG_OK;
}

int newMessageReceived (struct _ng_epgs **ngEPGS, const char *message, int rcvdMsgSize)
{

  ng_printf ("\n\nnewMessageReceived - size: %d\n", rcvdMsgSize);

  int k = 0;
  for (k = 0; k < 30; k++)
	{
	  ng_printf ("%02X ", message[k] & 0xFF);
	}

  int ethAddrHeaderSize = ETHERNET_MAC_ADDR_FIELD_SIZE + ETHERNET_MAC_ADDR_FIELD_SIZE;

  if (rcvdMsgSize < ethAddrHeaderSize + ETHERNET_TYPE_FIELD_SIZE + HEADER_SEGMENTATION_FIELD_SIZE)
	{
	  return NG_ERROR;
	}

  // Checking eth type. NG is type 0x1234
  if (!(message[ethAddrHeaderSize] == NG_TYPE_MSB) || !(message[ethAddrHeaderSize + 1] == NG_TYPE_LSB))
	{
	  ng_printf ("Not a NG type");
	  return NG_ERROR;
	}

  int index = ethAddrHeaderSize + ETHERNET_TYPE_FIELD_SIZE;
  int i = 0;

  int header_SeqmentationID_Size = HEADER_SEGMENTATION_FIELD_SIZE / 2;
  int header_SeqmentationCounter_Size = HEADER_SEGMENTATION_FIELD_SIZE / 2;

  unsigned msgSeq = 0;
  for (i = 0; i < header_SeqmentationID_Size; i++)
	{
	  msgSeq |= (message[index + i] & 0x000000FF) << (8 * (header_SeqmentationID_Size - 1 - i));
	}
  index += header_SeqmentationID_Size;

  unsigned msgNumber = 0;
  for (i = 0; i < header_SeqmentationCounter_Size; i++)
	{
	  msgNumber |= (message[index + i] & 0x000000FF) << (8 * (header_SeqmentationCounter_Size - 1 - i));
	}
  index += header_SeqmentationCounter_Size;

  bool b = false;

  if ((*ngEPGS)->ReceivedMsg == NULL && msgNumber == 0)
	{
	  (*ngEPGS)->ReceivedMsg = (NgReceivedMsg *)ng_malloc (sizeof (NgReceivedMsg) * 1);
	  (*ngEPGS)->ReceivedMsg->msg_id = msgSeq;
	  (*ngEPGS)->ReceivedMsg->frames_read = 0;

	  int msgSize = 0;
	  for (i = 0; i < HEADER_SIZE_FIELD_SIZE; i++)
		{
		  msgSize |= (message[index + i] & 0x000000FF) << (8 * (HEADER_SIZE_FIELD_SIZE - 1 - i));
		}
	  index += HEADER_SIZE_FIELD_SIZE;

	  (*ngEPGS)->ReceivedMsg->mgs_size = msgSize;
	  (*ngEPGS)->ReceivedMsg->number_of_frames = getNumberOfMessages (msgSize + HEADER_SIZE_FIELD_SIZE);
	  (*ngEPGS)->ReceivedMsg->buffer = (void *)ng_malloc (sizeof (void) * msgSize);
	}
  else if ((*ngEPGS)->ReceivedMsg->msg_id != msgSeq)
	{
	  destroy_NgReceivedMsg (&(*ngEPGS)->ReceivedMsg);
	  return NG_ERROR;
	}
  else
	{
	  b = false;
	}

  int bufferIndex = 0;
  if (msgNumber == 0)
	{
	  bufferIndex = 0;
	}
  else
	{
	  bufferIndex = DEFAULT_MTU
					- (HEADER_SEGMENTATION_FIELD_SIZE + ETHERNET_MAC_ADDR_FIELD_SIZE + ETHERNET_MAC_ADDR_FIELD_SIZE
					   + ETHERNET_TYPE_FIELD_SIZE + HEADER_SIZE_FIELD_SIZE) +
					(DEFAULT_MTU
					 - (HEADER_SEGMENTATION_FIELD_SIZE + ETHERNET_MAC_ADDR_FIELD_SIZE + ETHERNET_MAC_ADDR_FIELD_SIZE
						+ ETHERNET_TYPE_FIELD_SIZE)) * (msgNumber - 1);
	}

  for (i = 0; index < rcvdMsgSize; i++, index++)
	{
	  (*ngEPGS)->ReceivedMsg->buffer[bufferIndex + i] = message[index];
	}

  (*ngEPGS)->ReceivedMsg->frames_read++;

  if ((*ngEPGS)->ReceivedMsg->frames_read == (*ngEPGS)->ReceivedMsg->number_of_frames && !b)
	{
	  ParseReceivedMessage2 (ngEPGS);

	  return NG_OK;
	}

  return NG_PROCESSING;
}

int sendNGMessageThroughBLE (NgEPGS *ngEPGS, NgMessage *message)
{

  if (!message || !ngEPGS || !ngEPGS->MyNetInfo || !ngEPGS->MyNetInfo->Identifier)
	{
	  return NG_ERROR;
	}

  int payloadSize = HEADER_SIZE_FIELD_SIZE + message->MessageSize;

  char *sdu = (char *)ng_malloc (sizeof (char) * payloadSize);

  int i = 0;
  for (i = 0; i < HEADER_SIZE_FIELD_SIZE; i++)
	{
	  sdu[i] = (message->MessageSize >> (8 * (7 - i))) & 0xff;
	}

  for (i = 0; i < message->MessageSize; i++)
	{
	  sdu[HEADER_SIZE_FIELD_SIZE + i] = message->Msg[i];
	}

  int numberOfMsgs = getNumberOfMessages (message->MessageSize);
  unsigned MessageNumber = ng_rand ();
  MessageNumber = (MessageNumber << 16) | ng_rand ();

  unsigned int SequenceNumber = 0;

  int bytesSent = 0;

  int j = 0;
  for (j = 0; j < numberOfMsgs; j++)
	{
	  char *mtu = (char *)ng_calloc (sizeof (char) * 1, DEFAULT_MTU);

	  int index = 0;

	  unsigned char *HeaderSegmentationField = (unsigned char *)ng_malloc (sizeof (unsigned char) * 8);
	  HeaderSegmentationField[0] = (MessageNumber >> (8 * 3)) & 0xff;
	  HeaderSegmentationField[1] = (MessageNumber >> (8 * 2)) & 0xff;
	  HeaderSegmentationField[2] = (MessageNumber >> (8 * 1)) & 0xff;
	  HeaderSegmentationField[3] = (MessageNumber >> (8 * 0)) & 0xff;
	  HeaderSegmentationField[4] = (SequenceNumber >> (8 * 3)) & 0xff;
	  HeaderSegmentationField[5] = (SequenceNumber >> (8 * 2)) & 0xff;
	  HeaderSegmentationField[6] = (SequenceNumber >> (8 * 1)) & 0xff;
	  HeaderSegmentationField[7] = (SequenceNumber >> (8 * 0)) & 0xff;

	  for (i = 0; i < HEADER_SEGMENTATION_FIELD_SIZE; i++)
		{
		  mtu[index + i] = HeaderSegmentationField[i];
		}
	  ng_free (HeaderSegmentationField);
	  SequenceNumber++;
	  index += HEADER_SEGMENTATION_FIELD_SIZE;

	  for (; index < DEFAULT_MTU && bytesSent < payloadSize; index++)
		{
		  mtu[index] = sdu[bytesSent];
		  bytesSent++;
		}

	  //ng_BLESendData(mtu, index);

	  ng_printf ("\n\nSend Bluetooth MSG seq #%d: ", SequenceNumber);
	  /*int k = 0;
	  for(k = 0; k < index; k++) {
		  ng_printf("%02X ", mtu[k]  & 0xFF );
	  }
*/
	  ng_free (mtu);
	}

  ng_free (sdu);

  return NG_OK;
}

int forwardNGMessageToPeerThroughBLE (NgEPGS *ngEPGS, int peerID, NgMessage *message)
{

  if (!message || !ngEPGS || !ngEPGS->MyNetInfo || !ngEPGS->MyNetInfo->Identifier || !ngEPGS->PeersNetInfo)
	{
	  return NG_ERROR;
	}

  // Prepara o endere�o MAC de destino. Em caso de brodcast, somente setar o destino para FF:FF:FF:FF:FF:FF
  char *destinationMAC;

  if (ngEPGS->PeersNetInfo[peerID])
	{
	  if (ngEPGS->PeersNetInfo[peerID]->Identifier)
		{
		  convertStrToMAC (ngEPGS->PeersNetInfo[peerID]->Identifier, &destinationMAC);
		}
	  else
		{
		  return NG_ERROR;
		}
	}
  else
	{
	  return NG_ERROR;
	}

  ConvertMessageFromCommandLinesandPayloadCharArrayToCharArray (&message);

  int payloadSize = HEADER_SIZE_FIELD_SIZE + message->MessageSize;

  char *sdu = (char *)ng_malloc (sizeof (char) * payloadSize);

  int i = 0;
  for (i = 0; i < HEADER_SIZE_FIELD_SIZE; i++)
	{
	  sdu[i] = (message->MessageSize >> (8 * (7 - i))) & 0xff;
	}

  for (i = 0; i < message->MessageSize; i++)
	{
	  sdu[HEADER_SIZE_FIELD_SIZE + i] = message->Msg[i];
	}

  int numberOfMsgs = getNumberOfMessages (message->MessageSize);
  unsigned MessageNumber = ng_rand ();
  MessageNumber = (MessageNumber << 16) | ng_rand ();

  unsigned int SequenceNumber = 0;

  int bytesSent = 0;

  int j = 0;
  for (j = 0; j < numberOfMsgs; j++)
	{
	  char *mtu = (char *)ng_calloc (sizeof (char) * 1, DEFAULT_MTU);

	  int index = 0;

	  unsigned char *HeaderSegmentationField = (unsigned char *)ng_malloc (sizeof (unsigned char) * 8);
	  HeaderSegmentationField[0] = (MessageNumber >> (8 * 3)) & 0xff;
	  HeaderSegmentationField[1] = (MessageNumber >> (8 * 2)) & 0xff;
	  HeaderSegmentationField[2] = (MessageNumber >> (8 * 1)) & 0xff;
	  HeaderSegmentationField[3] = (MessageNumber >> (8 * 0)) & 0xff;
	  HeaderSegmentationField[4] = (SequenceNumber >> (8 * 3)) & 0xff;
	  HeaderSegmentationField[5] = (SequenceNumber >> (8 * 2)) & 0xff;
	  HeaderSegmentationField[6] = (SequenceNumber >> (8 * 1)) & 0xff;
	  HeaderSegmentationField[7] = (SequenceNumber >> (8 * 0)) & 0xff;

	  for (i = 0; i < HEADER_SEGMENTATION_FIELD_SIZE; i++)
		{
		  mtu[index + i] = HeaderSegmentationField[i];
		}
	  ng_free (HeaderSegmentationField);
	  SequenceNumber++;
	  index += HEADER_SEGMENTATION_FIELD_SIZE;

	  for (; index < DEFAULT_MTU && bytesSent < payloadSize; index++)
		{
		  mtu[index] = sdu[bytesSent];
		  bytesSent++;
		}

	  //ng_BLESendData(mtu, index);

	  ng_printf ("\n\nForward Bluetooth MSG seq #%d: ", SequenceNumber);
	  /*int k = 0;
	  for(k = 0; k < index; k++) {
		  ng_printf("%02X ", mtu[k]  & 0xFF );
	  }
*/
	  ng_free (mtu);
	}

  ng_free (sdu);

  return NG_OK;
}

int newMessageReceivedThroughBLE (struct _ng_epgs **ngEPGS, const char *message, int rcvdMsgSize)
{

  if (rcvdMsgSize < HEADER_SEGMENTATION_FIELD_SIZE)
	{
	  return NG_ERROR;
	}

  int index = 0;
  int i = 0;

  int header_SeqmentationID_Size = HEADER_SEGMENTATION_FIELD_SIZE / 2;
  int header_SeqmentationCounter_Size = HEADER_SEGMENTATION_FIELD_SIZE / 2;

  unsigned msgSeq = 0;
  for (i = 0; i < header_SeqmentationID_Size; i++)
	{
	  msgSeq |= message[index + i] << (8 * (header_SeqmentationID_Size - 1 - i));
	}
  index += header_SeqmentationID_Size;

  unsigned msgNumber = 0;
  for (i = 0; i < header_SeqmentationCounter_Size; i++)
	{
	  msgNumber |= message[index + i] << (8 * (header_SeqmentationCounter_Size - 1 - i));
	}
  index += header_SeqmentationCounter_Size;

  bool b = false;

  if ((*ngEPGS)->ReceivedMsg == NULL && msgNumber == 0)
	{
	  (*ngEPGS)->ReceivedMsg = (NgReceivedMsg *)ng_malloc (sizeof (NgReceivedMsg) * 1);
	  (*ngEPGS)->ReceivedMsg->msg_id = msgSeq;
	  (*ngEPGS)->ReceivedMsg->frames_read = 0;

	  int msgSize = 0;
	  for (i = 0; i < HEADER_SIZE_FIELD_SIZE; i++)
		{
		  msgSize |= message[index + i] << (8 * (HEADER_SIZE_FIELD_SIZE - 1 - i));
		}
	  index += HEADER_SIZE_FIELD_SIZE;

	  (*ngEPGS)->ReceivedMsg->mgs_size = msgSize;
	  (*ngEPGS)->ReceivedMsg->number_of_frames = getNumberOfMessages (msgSize + HEADER_SIZE_FIELD_SIZE);
	  (*ngEPGS)->ReceivedMsg->buffer = (void *)ng_malloc (sizeof (void) * msgSize);
	}
  else if ((*ngEPGS)->ReceivedMsg->msg_id != msgSeq)
	{
	  destroy_NgReceivedMsg (&(*ngEPGS)->ReceivedMsg);
	  return NG_ERROR;
	}
  else
	{
	  b = false;
	}

  int bufferIndex = 0;
  if (msgNumber == 0)
	{
	  bufferIndex = 0;
	}
  else
	{
	  bufferIndex = DEFAULT_MTU
					- (HEADER_SEGMENTATION_FIELD_SIZE + ETHERNET_MAC_ADDR_FIELD_SIZE + ETHERNET_MAC_ADDR_FIELD_SIZE
					   + ETHERNET_TYPE_FIELD_SIZE + HEADER_SIZE_FIELD_SIZE) +
					(DEFAULT_MTU
					 - (HEADER_SEGMENTATION_FIELD_SIZE + ETHERNET_MAC_ADDR_FIELD_SIZE + ETHERNET_MAC_ADDR_FIELD_SIZE
						+ ETHERNET_TYPE_FIELD_SIZE)) * (msgNumber - 1);
	}

  for (i = 0; index < rcvdMsgSize; i++, index++)
	{
	  (*ngEPGS)->ReceivedMsg->buffer[bufferIndex + i] = message[index];
	}

  (*ngEPGS)->ReceivedMsg->frames_read++;

  if ((*ngEPGS)->ReceivedMsg->frames_read == (*ngEPGS)->ReceivedMsg->number_of_frames && !b)
	{
	  ParseReceivedMessage2 (ngEPGS);

	  return NG_OK;
	}

  return NG_PROCESSING;
}

int getNumberOfMessages (int msgSize)
{
  int headerSize = HEADER_SEGMENTATION_FIELD_SIZE + ETHERNET_MAC_ADDR_FIELD_SIZE + ETHERNET_MAC_ADDR_FIELD_SIZE
				   + ETHERNET_TYPE_FIELD_SIZE;
  int payloadMTU = DEFAULT_MTU - headerSize;

  int payloadSize = HEADER_SIZE_FIELD_SIZE + msgSize;

  return ((int)(payloadSize / payloadMTU)) + 1;
}

