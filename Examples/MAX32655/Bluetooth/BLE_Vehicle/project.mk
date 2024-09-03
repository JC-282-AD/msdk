# This file can be used to set build configuration
# variables.  These variables are defined in a file called 
# "Makefile" that is located next to this one.

# For instructions on how to use this system, see
# https://analogdevicesinc.github.io/msdk/USERGUIDE/#build-system

# **********************************************************

# Enable Cordio library
LIB_CORDIO = 1

LIB_CMSIS_DSP = 1
# Cordio library options
INIT_PERIPHERAL = 1
INIT_CENTRAL = 0

# TRACE option
# Set to 0 to disable
# Set to 1 to enable serial port trace messages
# Set to 2 to enable verbose messages
TRACE = 1

# Add services and profiles folders to build
IPATH += services
VPATH += services
IPATH += profiles/bcv
VPATH += profiles/bcv

IPATH += vehicle
VPATH += vehicle
IPATH += adxl343
VPATH += adxl343
BOARD=FTHR_Apps_P1