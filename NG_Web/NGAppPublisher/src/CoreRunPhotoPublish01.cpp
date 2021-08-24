/*
	NovaGenesis

	Name:		CoreRunPhotoPublish01
	Object:		CoreRunPhotoPublish01
	File:		CoreRunPhotoPublish01.cpp
	Author:		Antonio M. Alberti
	Date:		01/2013
	Version:	0.1
*/

#ifndef _CORERUNPHOTOPUBLISH01_H
#include "CoreRunPhotoPublish01.h"
#endif

#ifndef _HT_H
#include "HT.h"
#endif

#ifndef _CORE_H
#include "Core.h"
#endif

CoreRunPhotoPublish01::CoreRunPhotoPublish01(string _LN, Block *_PB, MessageBuilder *_PMB):Action(_LN,_PB,_PMB)
{
}

CoreRunPhotoPublish01::~CoreRunPhotoPublish01()
{
}

// Run the actions behind a received command line
// ng -run --photopublish _Version
int CoreRunPhotoPublish01::Run(Message *_ReceivedMessage, CommandLine *_PCL, vector<Message*> &ScheduledMessages, Message *&InlineResponseMessage)
{
	int 				Status=ERROR;
	string				Offset = "                    ";
	Core				*PCore=0;
	CommandLine			*PCL=0;
	Message 			*RunPhotoPublish=0;
	vector<string>		Limiters;
	vector<string>		Sources;
	vector<string>		Destinations;
	struct dirent 		*dirpent;
	DIR 				*dirp;
	string::size_type	Pos=0;
	string 				FileName;
	string 				Extension;
	const char			*ThePath;
	string				PayloadHash;
	bool				PublicationStatus;
	int					Counter=0;

	PCore=(Core*)PB;

	PB->S << Offset <<  this->GetLegibleName() << endl;

	// ******************************************************
	// Looping over Path folder
	// ******************************************************

	PB->S << Offset <<  "(Looping over the files on path = "<<PB->GetPath()<<endl;

	ThePath=PB->GetPath().c_str();

	dirp=opendir(ThePath);

	if(dirp)
	   {
		   while((dirpent=readdir(dirp)) !=NULL)
		   	   {
			   	   PublicationStatus=true;

			   	   FileName=dirpent->d_name;

	           	   Pos=FileName.rfind('.');

	           	   if(Pos != string::npos)
	           	   	   {
	           		   	   Extension = FileName.substr(Pos+1);
	           	   	   }

	           	   if (Extension == "jpg" || Extension == "JPG")
	           	   	   {
	           		   	   PB->S << endl << Offset <<  "(Checking the photo number "<<Counter<<" with name "<< FileName<<")"<<endl;

	           		   	   if (PCore->PublishAllPhotos == true)
	           		   	   	   {
	           		   		   	   PB->S << Offset <<  "(The publish all photos flag is enabled. Going to publish all .jpg photos on the path)"<<endl;

	           		   		   	   PB->S << Offset <<  "(Generating the ng -pub --notify)"<<endl;

	           		   		   	   if(PCore->GetFileContentHash(FileName,PayloadHash) == OK)
	           		   		   	   	   {
	           		   		   		   	   PublishAPhotoToAllPeerApps02(FileName,ScheduledMessages);

	           		   		   		   	   PCore->Content.push_back(PayloadHash);

	           		   		   		   	   //PB->S << Offset <<  "(The payload hash is = "<<PayloadHash<<")"<<endl;
	           		   		   	   	   }
	           		   	   	   }
	           		   	   else
	           		   	   	   {
	           		   		   	   PB->S << Offset <<  "(The publish all photos flag is not enabled. Going to publish only the unpublished .jpg files)"<<endl;

	           		   		   	   PB->S << Offset <<  "(Already aware of "<<PCore->Content.size()<<" published photos)"<<endl;

									if (PCore->GetFileContentHash(FileName,PayloadHash) == OK)
										{
										   for (unsigned int i=0; i<PCore->Content.size(); i++)
											   {
													if (PayloadHash == PCore->Content[i])
														{
															PublicationStatus=false;

															//PB->S << Offset <<  "(The photo "<<FileName<<" was already published. Skipping)"<<endl;
														}
											   }
										}
									else
										{
											PB->S << Offset << "(ERROR: Unable to calculate the payload hash)" << endl;
										}

	           		   		   	   if (PublicationStatus==true)
	           		   		   	   	   {
	           		   		   		   	   PB->S << Offset <<  "(Going to publish the photo "<<FileName<<". Registering this publication on Content container)"<<endl;

	           		   		   		   	   PublishAPhotoToAllPeerApps02(FileName,ScheduledMessages);

	           		   		   		   	   PCore->Content.push_back(PayloadHash);

	           		   		   		   	   PB->S << Offset <<  "(The payload hash is = "<<PayloadHash<<")"<<endl;

	           		           		   	   if (Counter == 2)
	           								   {
	           									   break;
	           								   }
	           		   		   	   	   }
	           		   	   	   }

	           		   	   Counter++;
	           	   	   }
		   	   }
	   }

	closedir(dirp);

	PCore->PublishAllPhotos = false;

	PB->S << Offset <<  "(Finished)"<<endl;

	if( PCore->IsPublished ) return Status;

	// ******************************************************
	// Schedule a message to -run --photopublish again
	// ******************************************************

	// Setting up the process SCN as the space limiter
	Limiters.push_back(PB->PP->Intra_Process);

	// Setting up the CLI block SCN as the source SCN
	Sources.push_back(PB->GetSelfCertifyingName());

	// Setting up the HT block SCN as the destination SCN
	Destinations.push_back(PB->GetSelfCertifyingName());

	// Creating a new message
	PB->PP->NewMessage(GetTime()+PCore->DelayBeforeANewPhotoPublish,1,false,RunPhotoPublish);

	// Creating the ng -cl -msg command line
	PMB->NewConnectionLessCommandLine("0.1",&Limiters,&Sources,&Destinations,RunPhotoPublish,PCL);

	// Adding a ng -run --periodic command line
	RunPhotoPublish->NewCommandLine("-run","--photopublish","0.1",PCL);

	// Generate the SCN
	PB->GenerateSCNFromMessageBinaryPatterns16Bytes(RunPhotoPublish,SCN);

	// Creating the ng -scn --s command line
	PMB->NewSCNCommandLine("0.1",SCN,RunPhotoPublish,PCL);

	// ******************************************************
	// Finish
	// ******************************************************

	// Push the message to the GW input queue
	PCore->PGW->PushToInputQueue(RunPhotoPublish);

	//PB->S << Offset <<  "(Done)" << endl << endl << endl; 

	return Status;
}

// Publish a photo to all the peer applications - notifies just one peer
int CoreRunPhotoPublish01::PublishAPhotoToAllPeerApps01(string _FileName, vector<Message*> &ScheduledMessages)
{
	int 			Status=ERROR;
	Message			*Run=0;
	string			Offset = "                    ";
	Core			*PCore=0;
	CommandLine		*PCL=0;

	PCore=(Core*)PB;

	if (ScheduledMessages.size() > 0)
		{
			//PB->S << Offset <<  "(There is a scheduled message)" << endl;

			Run=ScheduledMessages.at(0);

			if (Run != 0)
				{
					// Publish to all peer servers
					for (unsigned int i=0; i<PCore->PeerServerAppTuples.size(); i++)
						{
							// Adding a ng -run --publish command line
							Run->NewCommandLine("-run","--publish","0.1",PCL);

							PCL->NewArgument(3);

							// Use the stored Subscription to fulfill the command line
							PCL->SetArgumentElement(0,0,"Server");

							PCL->SetArgumentElement(0,1,PB->IntToString(i));

							PCL->SetArgumentElement(0,2,_FileName);

							Status=OK;
						}

					// Publish to all peer clients
					for (unsigned int j=0; j<PCore->PeerClientAppTuples.size(); j++)
						{
							// Adding a ng -run --publish command line
							Run->NewCommandLine("-run","--publish","0.1",PCL);

							PCL->NewArgument(3);

							// Use the stored Subscription to fulfill the command line
							PCL->SetArgumentElement(0,0,"Client");

							PCL->SetArgumentElement(0,1,PB->IntToString(j));

							PCL->SetArgumentElement(0,2,_FileName);

							Status=OK;
						}
				}
		}

	return Status;
}

// Publish a photo to all the peer applications - notifies all peers
int CoreRunPhotoPublish01::PublishAPhotoToAllPeerApps02(string _FileName, vector<Message*> &ScheduledMessages)
{
	int 			Status=ERROR;
	Message			*Run=0;
	string			Offset = "                    ";
	CommandLine		*PCL=0;

	if (ScheduledMessages.size() > 0)
		{
			//PB->S << Offset <<  "(There is a scheduled message)" << endl;

			Run=ScheduledMessages.at(0);

			if (Run != 0)
				{
					// Adding a ng -run --publish command line
					Run->NewCommandLine("-run","--publish","0.2",PCL);

					PCL->NewArgument(1);

					PCL->SetArgumentElement(0,0,_FileName);

					Status=OK;
				}
		}

	return Status;
}
