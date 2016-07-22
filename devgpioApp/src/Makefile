TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#==================================================
# build a support library

LIBRARY_IOC += devroku

# xxxRecord.h will be created from xxxRecord.dbd
#DBDINC += xxxRecord
# install devgpio.dbd into <top>/dbd
DBD += devroku.dbd

# specify all source files to be compiled and added to the library
devroku_SRCS += devRokuBo.c devRoku.cpp devRokuManager.cpp

devroku_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

