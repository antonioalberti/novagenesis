cmake_minimum_required(VERSION 3.5.1)
project(novagenesis)

set(CMAKE_CXX_STANDARD 20)

include_directories(EPGS/Actions)
include_directories(EPGS/Common)
include_directories(EPGS/Controller)
include_directories(EPGS/DataStructures)
include_directories(EPGS/Network)
include_directories(EPGS/ng_util)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g3 -p -pg -Wall -fmessage-length=0 -pthread -Wno-deprecated -lpthread -lrt")

add_executable(EPGS
        EPGS/Actions/epgs_exposition.c
        EPGS/Actions/epgs_exposition.h
        EPGS/Actions/epgs_hello.c
        EPGS/Actions/epgs_hello.h
        EPGS/Actions/epgs_pubNotify.c
        EPGS/Actions/epgs_pubNotify.h
        EPGS/Actions/epgs_subServiceAcceptance.c
        EPGS/Actions/epgs_subServiceAcceptance.h
        EPGS/Common/MurmurHash3.c
        EPGS/Common/MurmurHash3.h
        EPGS/Common/ng_command.c
        EPGS/Common/ng_command.h
        EPGS/Common/ng_epgs_hash.c
        EPGS/Common/ng_epgs_hash.h
        EPGS/Common/ng_hash_table.c
        EPGS/Common/ng_hash_table.h
        EPGS/Common/ng_message.c
        EPGS/Common/ng_message.h
        EPGS/Common/tinythread.cpp
        EPGS/Common/tinythread.h
        EPGS/Controller/epgs_controller.c
        EPGS/Controller/epgs_controller.h
        EPGS/DataStructures/epgs_structures.c
        EPGS/DataStructures/epgs_structures.h
        EPGS/epgs.c
        EPGS/epgs_defines.h
        EPGS/epgs_ethernet.c
        EPGS/epgs_ethernet.h
        EPGS/epgs.h
        EPGS/EPGS_Simulation.c
        EPGS/epgs_wrapper.c
        EPGS/epgs_wrapper.h
        EPGS/Network/PG.c
        EPGS/Network/PG.h
        EPGS/ng_util/ng_json.c
        EPGS/ng_util/ng_json.h
        EPGS/ng_util/ng_util.c
        EPGS/ng_util/ng_util.h
        EPGS/UnitTest.c)
