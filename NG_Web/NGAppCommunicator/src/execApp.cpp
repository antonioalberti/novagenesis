//============================================================================
// Name        : Simple application
// Author      : Antonio Alberti
// Version     : v0.1
// Copyright   : Alberti, INATEL 2012
// Description : Executable for a simple application
//============================================================================

#include "App.h"

#ifndef _TIME_H
#include <time.h>
#endif

#ifndef _SYS_TIME_H
#include <sys/time.h>
#endif

#include "NGManager.h"

#include <pthread.h>

bool isDbusRunning;

void *thread_dbus( void *ptr )
{
	GMainLoop *loop;

	printf("Init D-Bus Manager\n");

	NGManager* ngc = NGManager::getInstance();

/*
	while(!ngc->isAdaptorRegistred()){
		printf("Waiting for register Adaptors\n");
		sleep(1);
	}
*/
	printf("Adaptors registered.\n");

	loop = g_main_loop_new (NULL, FALSE);
 	g_main_loop_run (loop);

}


int main(int argc, char *argv[])
{
	unsigned int 	R=0;
	double		 	Time=0;
	bool		 	Problem=false;
	pthread_t thread1;

	if (argc != 0)
		{
			if (argc == 3)
				{
					string Path=argv[1];

					cout << "(The I/O path is "<<Path<< ")"<<endl;

					string Temp=argv[2];

					if (Temp == "Client" || Temp == "Server")
						{
							int iret1 = pthread_create( &thread1, NULL, thread_dbus,NULL);
						    if(iret1)
						    {
						         fprintf(stderr,"Error - pthread_create() return code: %d\n",iret1);
						         exit(EXIT_FAILURE);
						    }
						    printf("pthread_create() for thread 1 returns: %d\n",iret1);

							cout<<"(This is a "<<Temp<<" process.)"<<endl;


							// Allocate IPC input keys vector
							vector<key_t> *IPCInputKeys=new vector<key_t>;

							// Allocate IPC output keys vector
							vector<key_t> *IPCOutputKeys=new vector<key_t>;

							struct timespec t;

							clock_gettime(CLOCK_MONOTONIC, &t);

							Time = ((t.tv_sec)+(double)(t.tv_nsec/1e9));

							// Initialize the random generator
							srand(Time);

							// Generates a random key
							R=1000+(rand()%10000);

							// Add input keys to the vector
							IPCInputKeys->push_back(R);

							// Add output keys to the vector
							IPCOutputKeys->push_back(R+1);

							// Set the shm key
							key_t Key = R;

							// Create a process instance
							AppBrowser execApp("App2",Temp, Key,Path);

							// Clear the vectors
							IPCInputKeys->clear();
							IPCOutputKeys->clear();

							// Delete them
							delete IPCInputKeys;
							delete IPCOutputKeys;
						}
					else
						{
							Problem=true;

							cout<<"(ERROR: Empty role)"<<endl;
						}
				}
			else
				{
					Problem=true;

					cout<< "(ERROR: Wrong number of main() arguments)"<<endl;
				}
		}
	else
		{
			Problem=true;

			cout<< "(ERROR: No argument supplied)"<<endl;
		}

	if(Problem == true)
		{
			cout<< "(Usage: ./App Path Role)"<< endl;
			cout<< "(Path example: /home/myprofile/newWorkspace/novagenesis/IO/Client1/)"<<endl;
			cout<< "(Role can be \"Client\" or \"Server\")"<<endl;
		}

	pthread_join( thread1, NULL);

	return 0;
}




