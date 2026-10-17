cmake_minimum_required(VERSION 3.16)
project(test_checkers)

# Create targets for testing
add_library(Common STATIC GW.c)
add_library(ContentApp STATIC main.c)
add_library(IoTTestApp STATIC IoTTestApp.c)
add_library(NBTestApp STATIC NBTestApp.c)
add_library(PGCS STATIC PGCS.c)
add_library(NRNCS STATIC NRNCS.c)