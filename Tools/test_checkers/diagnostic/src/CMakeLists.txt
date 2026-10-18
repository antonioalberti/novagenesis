cmake_minimum_required(VERSION 3.16)
project(test_checkers_diagnostic)

# Create targets for testing diagnostic profile
add_library(Common STATIC GW.c)
add_library(ng039_gw_q0_q1_d2 STATIC test.c)