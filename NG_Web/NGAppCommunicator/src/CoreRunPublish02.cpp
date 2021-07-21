/*
	NovaGenesis

	Name:		CoreRunPublish02
	Object:		CoreRunPublish02
	File:		CoreRunPublish02.cpp
	Author:		Antonio M. Alberti
	Date:		02/2013
	Version:	0.2
*/

#ifndef _CORERUNPUBLISH02_H
#include "CoreRunPublish02.h"
#endif

#ifndef _CORE_H
#include "Core.h"
#endif

#ifndef _GW_H
#include "GW.h"
#endif

#ifndef _STRIP_TAGS
#include "Striptags.h"
#endif

CoreRunPublish02::CoreRunPublish02(string _LN, Block *_PB, MessageBuilder *_PMB):Action(_LN,_PB,_PMB)
{
}

CoreRunPublish02::~CoreRunPublish02()
{
}

// Run the actions behind a received command line
// ng -run --publish 0.2 [ < 1 string FileName > ]
int CoreRunPublish02::Run(Message *_ReceivedMessage, CommandLine *_PCL, vector<Message*> &ScheduledMessages, Message *&InlineResponseMessage)
{
	int 			Status=OK;
	string			Offset = "                    ";
	Core			*PCore=0;
	vector<string>	Limiters;
	vector<string>	Sources;
	vector<string>	Destinations;
	vector<string>	Values;
	string			PayloadHash;
	vector<Tuple*>	PubNotify;
	vector<Tuple*>	SubNotify;
	string 			PayloadString;
	unsigned int 	NA=0;
	vector<string>	FileName;
	Tuple			*PT=0;

	PCore=(Core*)PB;

	PB->S << Offset << this->GetLegibleName() << endl;

	// Load the number of arguments
	if (_PCL->GetNumberofArguments(NA) == OK)
		{
			// Check the number of arguments
			if (NA == 1)
				{
					// Get received command line arguments
					_PCL->GetArgument(0,FileName);

					if (FileName.size() == 1)
						{
							PB->S << Offset << "(File = " << FileName[0] << ")" << endl;
							//PB->S << Offset << "(Path = " << PB->GetPath() << ")" << endl;
#if 0
							for (unsigned int i=0; i<PCore->PeerServerAppTuples.size(); i++)
								{
									Tuple *ExtendedPeerAppTuple=new Tuple;

									// Set this extended tuple to have a publication notification
									ExtendedPeerAppTuple->Values.push_back("pub");

									// Copy the related Tuple pointer
									PT=PCore->PeerServerAppTuples[i];

									PB->S << Offset << "(Server " << i << ")" << endl;
									//PB->S << Offset << "(HID = " << PT->Values[0] << ")" << endl;
									//PB->S << Offset << "(OSID = " << PT->Values[1] << ")" << endl;
									//PB->S << Offset << "(PID = " << PT->Values[2] << ")" << endl;
									//PB->S << Offset << "(BID = " << PT->Values[3] << ")" << endl;

									// Set the remaining elements on the tuple
									for (unsigned int j=0; j<PT->Values.size(); j++)
										{
											ExtendedPeerAppTuple->Values.push_back(PT->Values[j]);
										}

									// Configure the tuple to be informed of this publication
									PubNotify.push_back(ExtendedPeerAppTuple);
								}

							for (unsigned int k=0; k<PCore->PeerClientAppTuples.size(); k++)
								{
									Tuple *ExtendedPeerAppTuple=new Tuple;

									// Set this extended tuple to have a publication notification
									ExtendedPeerAppTuple->Values.push_back("pub");

									// Copy the related Tuple pointer
									PT=PCore->PeerClientAppTuples[k];

									PB->S << Offset << "(Client " << k << ")" << endl;
									//PB->S << Offset << "(HID = " << PT->Values[0] << ")" << endl;
									//PB->S << Offset << "(OSID = " << PT->Values[1] << ")" << endl;
									//PB->S << Offset << "(PID = " << PT->Values[2] << ")" << endl;
									//PB->S << Offset << "(BID = " << PT->Values[3] << ")" << endl;

									// Set the remaining elements on the tuple
									for (unsigned int l=0; l<PT->Values.size(); l++)
										{
											ExtendedPeerAppTuple->Values.push_back(PT->Values[l]);
										}

									// Configure the tuple to be informed of this publication
									PubNotify.push_back(ExtendedPeerAppTuple);
								}
#endif
							CreatePublishMessage(FileName[0],PubNotify,SubNotify);
							PCore->IsPublished = true;
#if 0
							for (unsigned int m=0; m<PubNotify.size(); m++)
								{
									Tuple *PET=PubNotify[m];

									PB->S << Offset << "(Deleting the extended tuple number " << m << ")" << endl;

									delete PET;
								}
#endif
							if (ScheduledMessages.size() > 0)
								{
									Message *Temp=ScheduledMessages.at(0);

									Temp->Delete=true;

									// Make the scheduled messages vector empty
									ScheduledMessages.clear();
								}
						}
					else
						{
							PB->S << Offset <<  "(ERROR: One or more argument is empty)" << endl;
						}
				}
			else
				{
					PB->S << Offset <<  "(ERROR: Wrong number of arguments)" << endl;
				}
		}
	else
		{
			PB->S << Offset <<  "(ERROR: Unable to read the number of arguments)" << endl;
		}

	//PB->S << Offset <<  "(Done)" << endl << endl << endl;

	return Status;
}

void CoreRunPublish02::CreatePublishMessage(string _FileName, vector<Tuple*> &_PubNotify, vector<Tuple*> &_SubNotify)
{
	string			Offset = "                    ";
	Core			*PCore=0;
	vector<string>	Limiters;
	vector<string>	Sources;
	vector<string>	Destinations;
	Message 		*Publish=0;
	CommandLine		*PCL=0;
	vector<string>	Values;
	string			PayloadHash;
	char 			*Payload=0;
	long long int 	PayloadSize=0;
	string 			PayloadString;
	vector<string>	FileName;

	PCore=(Core*)PB;

	// Setting up the OSID as the space limiter
	Limiters.push_back(PB->PP->GetDomainSelfCertifyingName());

	// Setting up the this OS as the 1st source SCN
	Sources.push_back(PB->PP->GetHostSelfCertifyingName());

	// Setting up the this OS as the 2nd source SCN
	Sources.push_back(PB->PP->GetOperatingSystemSelfCertifyingName());

	// Setting up the this process as the 3rd source SCN
	Sources.push_back(PB->PP->GetSelfCertifyingName());

	// Setting up the PS block SCN as the 4th source SCN
	Sources.push_back(PB->GetSelfCertifyingName());

	// Setting up the destination 1st source
	Destinations.push_back(PCore->PSSTuples[0]->Values[0]);

	// Setting up the destination 2nd source
	Destinations.push_back(PCore->PSSTuples[0]->Values[1]);

	// Setting up the destination 3rd source
	Destinations.push_back(PCore->PSSTuples[0]->Values[2]);

	// Setting up the destination 4th source
	Destinations.push_back(PCore->PSSTuples[0]->Values[3]);

	// Creating a new message
	PB->PP->NewMessage(GetTime(),1,true,"Not_Necessary",_FileName,"Not_Necessary",PB->GetPath(),Publish);

	Publish->ConvertPayloadFromFileToCharArray();

	// Creating the ng -cl -msg command line
	PMB->NewConnectionLessCommandLine("0.1",&Limiters,&Sources,&Destinations,Publish,PCL);

	// ***************************************************
	// Generate the bindings to be published
	// ***************************************************

	// Get the payload to be published
	Publish->GetPayloadFromCharArray(Payload);

	// Get the payload size
	Publish->GetPayloadSize(PayloadSize);

	if (PayloadSize > 0)
		{
			//PB->S << Offset <<"(The payload is:" << endl << Offset << PayloadString << endl << Offset << ")"<< endl;

			// Allocates a new subscription
			Publication	*PP;

			PCore->NewPublication(PP);

			PP->Timestamp=GetTime();

			// Calculates the offer file payload hash
			PCore->GetFileContentHash(_FileName,PayloadHash);

			PCore->Debug.OpenOutputFile();

			PCore->Debug << "Publishing the file "<< _FileName << " with hash code equal to " << PayloadHash << endl;

			PCore->Debug.CloseFile();

			PB->S << endl << Offset <<"(The payload hash is " << PayloadHash << ")"<< endl;

			// Put the file name on the binding value
			Values.push_back(_FileName);
#if 0
			// Set the ng -pub --notify 0.1
			PMB->NewPublicationWithNotificationCommandLine("0.1",18,PayloadHash,&Values,&_PubNotify,&_SubNotify,Publish,PCL);
#endif
			// ***************************************************
			// Generate the -info --payload command line
			// ***************************************************

			// Adding a ng -info --payload 01 [< 1 string Service_Offer.txt > ]
			PMB->NewInfoPayloadCommandLine("0.1",&Values,Publish,PCL);

			// Publish the provenance information
			Values.clear();

			Striptags st;
			st.makeStriptags(PCore->Debug.Path+_FileName);
// for each ( keywords )

			PB->S << endl << Offset <<"(Trying open the file is " << PCore->Debug.Path+_FileName << ")"<< endl;

			// Binding < Hash(Payload), App::Core BID>
			Values.push_back(PayloadHash);

			for(unsigned int i=0;i<st.getSize();i++){
				// for keyword - teste 1
				//Values.push_back(st.getWord(i));

				//PMB->NewCommonCommandLine("-pub","--bind","0.1",1,PayloadHash,&Values,Publish,PCL);
				PMB->NewCommonCommandLine("-pub","--bind","0.1",0,st.getWord(i),&Values,Publish,PCL);

			}
			// Clear Hash(Payload)
			Values.clear();
// end foreach

			Values.push_back(PB->GetSelfCertifyingName());

			// Binding < Hash(Payload), App::Core BID>
			PMB->NewCommonCommandLine("-pub","--bind","0.1",2,PayloadHash,&Values,Publish,PCL);

			Values.clear();

			Values.push_back(PB->PP->GetSelfCertifyingName());

			// Binding < Hash(Payload), App PID>
			PMB->NewCommonCommandLine("-pub","--bind","0.1",2,PayloadHash,&Values,Publish,PCL);

			Values.clear();

			Values.push_back(PB->PP->GetOperatingSystemSelfCertifyingName());

			// Binding < Hash(Payload), App OSID>
			PMB->NewCommonCommandLine("-pub","--bind","0.1",2,PayloadHash,&Values,Publish,PCL);

			Values.clear();

			Values.push_back(PB->PP->GetHostSelfCertifyingName());

			// Binding < Hash(Payload), App PID>
			PMB->NewCommonCommandLine("-pub","--bind","0.1",9,PayloadHash,&Values,Publish,PCL);

			// ***************************************************
			// Generate the ng -message --type [ < 1 string 1 > ]
			// ***************************************************

			Publish->NewCommandLine("-message","--type","0.1",PCL);

			PCL->NewArgument(1);

			PCL->SetArgumentElement(0,0,PB->IntToString(Publish->GetType()));

			// ***************************************************
			// Generate the ng -message --seq [ < 1 string 1 > ]
			// ***************************************************

			Publish->NewCommandLine("-message","--seq","0.1",PCL);

			PCL->NewArgument(1);

			PCL->SetArgumentElement(0,0,PB->IntToString(PCore->GetSequenceNumber()));

			// ******************************************************
			// Generate the SCN
			// ******************************************************

			// Generate the SCN
			PB->GenerateSCNFromMessageBinaryPatterns16Bytes(Publish,SCN);

			// Creating the ng -scn --s command line
			PMB->NewSCNCommandLine("0.1",SCN,Publish,PCL);

			// Stores the SCN on the Publication object for future reference
			PP->Key=SCN;

			//PB->S << Offset <<"(The following message was published to the peer)"<< endl;

			//PB->S << "(" << endl << *Publish << ")"<< endl;

			// ******************************************************
			// Finish
			// ******************************************************

			// Push the message to the GW input queue
			PCore->PGW->PushToInputQueue(Publish);
		}
	else
		{
			PB->S << Offset <<"(ERROR: Invalid size)"<< endl;
		}
}

