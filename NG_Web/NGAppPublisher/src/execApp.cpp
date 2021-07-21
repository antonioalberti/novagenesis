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

int main(int argc, char *argv[])
{
	unsigned int 	R=0;
	double		 	Time=0;
	bool		 	Problem=false;

	if (argc != 0)
		{
			if (argc == 3)
				{
					string Path=argv[1];

					cout << "(The I/O path is "<<Path<< ")"<<endl;

					string Temp=argv[2];

					if (Temp == "Client" || Temp == "Server")
						{
							cout<<"(This is a "<<Temp<<" process.)"<<endl;

							struct timespec t;

							clock_gettime(CLOCK_MONOTONIC, &t);

							Time = ((t.tv_sec)+(double)(t.tv_nsec/1e9));

							// Initialize the random generator
							srand(Time);

							// Generates a random key
							R=1000+(rand()%10000);

							// Set the shm key
							key_t Key = R;

							// Create a process instance
							AppBrowser execApp("App",Temp,Key,Path);
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

	return 0;
}




