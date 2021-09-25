Thank you for your interest in NovaGenesis. 

# 1. What is NovaGenesis (NG)?

NG is a convergent information processing, storage and exchanging architecture developed at INATEL - Instituto Nacional de Telecomunicações, Santa Rita do Sapucaí, MG, Brazil. Nowadays, NovaGenesis is kept by Information and Communications Technologies (ICT) Lab.

Its design started in 2008, when Prof. Antônio Marcos Alberti selected project requirements, design principles, guidelines and main foundations. Alberti also did its first specification in 2011 and coded this prototype in C/C++ in 2012-2013. The key idea of NovaGenesis can be summarized in the following statement: services (including protocol implementations) that organize themselves based on names, name bindings, and contracts to meet semantically rich goals and policies. Every component of the architecture is offered to others by publishing several name-bindings. NovaGenesis is also an event-driven simulator. NG implements a novel Internet stack which runs over raw sockets in Linux. There is no support for other operating systems. A security layer is also missing in this prototype. Please read our open source license in order to use it. All the source code is offered as it is. There is no guarantee of any kind. NG is a registered software.

More details about NG design can be found in the following paper: https://www.sciencedirect.com/science/article/abs/pii/S0167739X16302643. 

All the papers regarding NG are accessible here: https://www.researchgate.net/profile/Antonio-Alberti-2. 

# 2. NovaGenesis folder structure

c-make-building - It is where the executable program appear after compilation with cmake or clion. We prepared a project in clion for compiling all the source code using a CMakeLists.txt file in NG root folder. 

Common - It is the source code of NG main and unique library. NG employs a fractal coding approach. This library employs a GNU LESSER GENERAL PUBLIC LICENSE. Please read the Copying.txt file in this folder for more information. This library implements base objects for all the NG services, including messaging, events, queues, structural blocks, etc. The compilation of the source code in this folder generates the libCommon.a library, which is linked to other NG services.

ContentApp - It is an important NG content distribution application that has two types of instances: sources and repositories. Files (e.g. .jpg photos) are published and subscribed by this service. It requires a NG core services to be running before it. This application employs NG publish/subscribe API to distribute contents among instance. The compilation of this folder results in a NG application called ContentApp. Its previous name was just App. Please, configure the App.ini file in the folder Scripts/Docker/Includes-ContentApp before running to customize default parameters. 

Docker - This folder contains the folders and subsequent files required to prepare NG Docker container images. A CMakeLists.txt file is provided for each image, as well as a Dockerfile, a running script (Run.sh, which is the entrypoint for Docker a container) and a supervisord.conf, which contains Supervisord instructions to start NG services inside a container. Supervisord is required since more than one process runs in a container. This folder stores include files when running make-all-docker-images.sh.

EPGS - It is a lightweight version of PGCS aimed to be embedded into IoT devices. EPGS has its own structure and compilation procedures that are not required to run NG in Linux OS. There is a way to run an emulated EPGS in Linux, but it is under development. More details are given here: https://ieeexplore.ieee.org/document/7970111

GIRS - It is a generic indirection resolution service (GIRS), which is nowadays integrated into NRNCS. GIRS selects which hash table service (HTS) will finally stores the name bindings and/or contents. GIRS selects the HTS service that is serving a certain name (or key) binary range, i.e. a subset of all possible binary numbers according to the number of bits of the key of a content or a name binding. Typically, a local domain can have several PSS, GIRS, and HTS instances. This approach has been replaced in 2021 by NRNCS. However, GIRS is an independent service that could be used together with PSS and HTS to replace the NRNCS in a distributed way. In other words, NRNCS integrates PSS, GIRS and HTS in a unique service. 

HTS - HTS provides a mechanism to retrieve already published bindings and deliver them directly to an authorized subscriber. Every HTS instance contains a hash table data structure where
name bindings are stored in a key/value(s) format. GIRS determines to where a certain ID range will be stored by a division remainder.

IO - This folder is employed to store input files required to run NG services as well as to store services outputs. All the programs have a .ini file, which contains customizable features of these services. 

IoTTestApp - This application was developed to work as a client of IoT or I4.0 devices. It generates demands for sensors or actuators that embedd EPGS. All the operation is contract-based from embedded EPGS, PGCS and IoTTestApp. Therefore, it is useless to run IoTtestApp without an EPGS instance and a PGCS that represents it in NG core (like a digital twin). The PGCS must be configured in -pc mode for this IoT/I4.0 scenario. For more detail in this application, check https://ieeexplore.ieee.org/document/7970111.

Make - This folder contains some alternative C/C++ compilation scripts. It can used, but it is better to use cmake or clion with the CMakeLists.txt. To directly compile NG service use Compile.sh. Be sure to have GCC/G++ installed.

	makedirs create the folders to store compiled code inside service source files' folders. 
	
	Therefore, run it first. 

NBTestApp - This application performs a NRNCS (unique process version) or PSS/GIRS/HTS (distributed version) testing by publishing thousands of name bindings and latter subscribing some of them randomly. This application was previously used in the paper: https://www.sciencedirect.com/science/article/abs/pii/S0167739X16302643. Its previous name was NBSimpleTestApp. Please, configure the App.ini file in the folder Scripts/Docker/Includes-NBTestApp before running to customize default parameters. 

NRNCS - It is 2021 service that integrates PSS, GIRS and HTS in a unique service. NRNCS offers a publish/subscribe API for NG services. It also implements a Hash Table data structure to store domain level name bindings and employs Linux file system to store associated content files (e.g. .jpg photos). Every NG domain must have at least one NRNCS instance, or alternatively, one PSS, GIRS and HTS.

Plots - Contais a script for plotting NG output files in GNU Plot. Some NG services generate statistics of their operations that can be plotted by a script like this. 

PGCS - It is aimed at message forwarding and routing over link layer. It provides: message  encapsulation  over  already  established  networking technologies,  such  as Ethernet, Wi-Fi, LoRa  or  Bluetooth;  a  proxy  service  to represent other NG services inside an OS; and bootstrapping functionalities  to  initialize  a  domain.  PGCS  enables  PSS,GIRS  and  HTS  discovery  during  initialization.  Since  an  address is a name that denotes the position to where an existence can inhabit or be attached, PGCS relays on HTS to store name-bindings among already established address formats (e.g. a physical world  or  an  emulated  MAC  Ethernet)  and/or  NG  addresses. PGCS also has an internal hash table in which it copies/locally stores  discovered  NBs.  Independently  of  the  address  formatused to connect PGCSes, inside NG all the communication is ID-oriented. Additionally to the gateway functionality, PGCS publishes  NBs  about  NG  services  inside  an  OS  to  other PGCSes in the same domain. PGCS can also represent physical things, acting as a digital twin. See more here: (PDF) Advancing NovaGenesis Architecture Towards Future Internet of Things. Available from: https://www.researchgate.net/publication/318279437_Advancing_NovaGenesis_Architecture_Towards_Future_Internet_of_Things [accessed Sep 25 2021].

PSS - By using PSS, other NG services or applications can expose its NBs to other services or discover how published names are related one another. Eventually, services can successively subscribe NBs to identify and locate other existences, storing these NBs in their local hash tables (every NG service also has a hash table), and routing/forwarding information based on them. The NB/content subscription model also employs transfer of digitally signed hash table keys. In 2021, PSS was integrated into NRNCS process.

Scripts - This folder stores some new and old bash scripts to run NG. It has:

	Docker - This folder is the newest way to run NG developed in 2021. A description o how to use such scripts to run NG is further provided in this document.
	
	Old releases - This folder contains scripts from older releases. These scripts will be probably useless for the majorituy of users.
	
	Python - Contains some scripts used in applications scenarios.
	
	Simple - This folder has some local host running scripts without Docker. These scripts could be usefull and will also be discussed bellow.
	
.git - Contains git realted files.

.idea - Contains clion related files.

CMakeLists - Contains instructions for clion or cmake. To compile NG without clion locally (i.e. you are not going to use Docker images to run) just open a terminal and go to the folder novagenesis. Then, run:

	sudo cmake --build cmake-build-debug --target all           
	
For this approach of running, go to How to run NG in the host OS (Section 4) for further instructions. Or alternativally, create Docker images and run NG inside Docker containers. In this case, see How to run NG with Docker containers (Section 3). 

Dockerfile - This Dockerfile is employed to create a ng-generic:latest container image. It includes and compiles all the NG services. Therefore, it is a generic approach without an specific running objective. This image has a larger overhead. If you don't need it for developing purposes, just comment the docker -build line in the make-all-images.sh script bellow. Otherwise, this image will be generated by default.

make-all-docker-images.sh - Prepare all the NG docker container images automatically. At the end of the process, chech the images created with: $sudo docker images

make-generic-docker-image.sh - This script builds the ng-generic Docker image only. 

remove-all-docker-images.sh - Remove all NG Docker images created.

.dockerignore - A file to avoid some problems when building Docker images. 

.gitignore - A file to avoid git to keep some type of files and folders.



# 3. How to run NG with Docker containers



First of all, you need to generate the NG images. For this purpose, just run:

	sudo sh make-all-docker-images.sh

Observe that you do not need to compile NG in the host OS to compile NovaGenesis inside the containers. 

These are separeted things. If you want to run NovaGenesis in your local host, without containers, go to Section 4.

After running this scripts, you can check the created images with:

	sudo docker images

The following images will be available:

	ng-generic:latest - This is a complete image if all services, but not prepared to run any demostration.

	ng-nrncs:latest - This is a NovaGenesis core image. It includes two services: PGCS and a NRNCS. Since NRNCS is important to store evey name binding and content available in a Domain, you should run this image in all NG demonstration/example scenarios.

	ng-contentapp:latest - This image has the named-content distribution application for NG demo.

	ng-nbtestapp:latest - This image has the name bindings testing applicatio for NG demo.

If you want to remove the NovaGenesis images, trun:

	sudo sh remove-all-docker-images.sh

Be sure to have stopped all running containers before doing this action.

After you have generated the images, you could now run the demonstrations/examples bellow.

In this NG running option, NG programs run inside containers and use containers shared memory for Interprocess communication. Docker bridge is employed for PGCSes communication using raw Ethernet socket. No HTTP/TCP/UDP/IP is employed at all. Only NovaGenesis and Ethernet.



# 3.1. Content Distribution

Go to the running folder:

	cd /Scripts/Docker

Run the script:

	sudo sh run-multiple-content-distribution-applications.sh $1 $2 $3 $4 $5

$1 is the number of content repository apps
$2 is the number of content source apps
$3 paramenter is the amount of photos to be created in a Source
$4 paramenter is the width of the photos to be created in a Source
$5 paramenter is the height of the photos to be created in a Source

For instance, type:

	sudo sh run-multiple-content-distribution-applications.sh 2 2 100 50 50

This call creates two sources that will publish 100 .jpg photos with size 50 x 50 pixels to the two photo repositories. 

This demonstrates NG content distributed approach with NRNCS.

If you want to collect what happened inside the containers just type:

	sudo sh  log-multiple-content-distribution-applications.sh $1 $2

$1 is the number of content repository apps
$2 is the number of content source apps

A banch of files will be created in this folder with results.

Check photo publishing (pub) and subscription (sub) to/from NRNCS, respectivelly.

It is also possible to check the logs of the programs in each container, the amount of content received, the network cache (NRNCS), delays, etc. 

To stop the containers, type:

	sudo sh stop-multiple-content-distribution-applications.sh $1 $2

You can also use Wireshark tool to check NovaGenesis packets using the docker0 (Docker Bridge) interface. Select eth.type == 0x1234 filter to reduce the log exclusively to NG packets. After contract establishment, you can se the photos frames passing in the Wireshark.

If you want to test NG transferring other files, for instance, video data chunks, just copy them to the container as indicated bellow: 

	#Copy files to a source container instance named Source$1
	docker cp/Downloads/. Source$1:/home/ng/workspace/novagenesis/IO/Source$1/

The Source$1 will detect the new files in its folder and start transferring to the existent Repository container(s).
 

# 3.2. Name Binding Resolution

In this demonstration a large amount of name bindings is published by NBTestApp to the NRNCS. Name bindings are published in thousands per message. After some time, the cache is full of name bindings. Then, the application starts to subscribe one of them randomly. The demo finishes after 1440 subscriptions, closing the NBTestApp. 

Go to the running folder:

	cd /Scripts/Docker

Run the script:

	sudo sh run-name-binding-resolution-application.sh

To access the results in the demonastration, type:

	sudo sh log-name-binding-resolution-application.sh

You can use this latter script every time you want to check what is going one in the containers. Novel results and log files versions are copied to this folder.

To stop the containers, type:

	sudo sh stop-name-binding-application.sh

You can also use Wireshark tool to check NovaGenesis packets using the docker0 (Docker Bridge) interface. Select eth.type == 0x1234 filter to reduce the log exclusively to NG packets. You can see NBs publishing in the beginning of the demo. After the publication phase finishes, subscription starts. 




# 4. How to run NG in the host OS

In this option, NG programs run locally using host OS shared memory for Interprocess communication. No socker is required. 

Go to the running folder:

	/Scripts/Simple

# 4.1. Content Distribution

Just run the script:

	sudo sh clean.sh

It is required to clean the POSIX semaphores from previous NG runnings.

Now, run the NG services locally:

	sudo sh Intra_OS_Content_Test_NRNCS.sh

Be sure to have compiled the NG services before running this latter script. If you haven't compiled, just go to /novagenesis folder and type:

	sudo cmake --build cmake-build-debug --target all

In this case, results will be placed locally in the /IO folder for each program.

Another important thing in this running option, is that you will need to copy some .jpg photos (smaller than 33 Mbytes) to the /IO/Source1 folder to be transferred for the Repository1.

This test never ends, so you need to close the programs when photos/files finished transferring from Source1 to Repository1.



# 4.2. Name Binding Resolution

Just run the script:

	sudo sh clean.sh

It is required to clean the POSIX semaphores from previous NG runnings.

Now, run the NG services locally:

	sudo sh Intra_OS_NBTest.sh

In this case, results will be placed locally in the /IO/NBTestApp folder.

When the test finishes, the NBTestApp stops runnig by itself.
