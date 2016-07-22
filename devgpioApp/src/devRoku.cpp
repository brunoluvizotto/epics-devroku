//******************************************************************************
// Copyright (C) 2015 Florian Feldbauer <feldbaue@kph.uni-mainz.de>
//                    - Helmholtz-Institut Mainz
//
// This file is part of devGpio
//
// devGpio is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// devRoku is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
// version 1.0.0; Aug 13, 2015
//
//******************************************************************************

//! @file devRokuManager.cpp
//! @author F.Feldbauer
//! @date 13 Aug 2015
//! @brief Implementation of GPIO Manager class

//! TODO: I/O interrupt handling hard coded on both flanks...Option to set by user?

//_____ I N C L U D E S ________________________________________________________

// ANSI C/C++ includes
#include <cstring>
#include <exception>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <unistd.h>

// EPICS includes
#include <alarm.h>
#include <biRecord.h>
#include <boRecord.h>
#include <dbAccess.h>
#include <errlog.h>
#include <epicsExport.h>
#include <iocsh.h>
#include <recGbl.h>

// local includes
#include "devRoku.h"
#include "devRokuManager.h"
#include "devGpioErrors.h"

//_____ D E F I N I T I O N S __________________________________________________

//_____ G L O B A L S __________________________________________________________

//_____ L O C A L S ____________________________________________________________

//_____ F U N C T I O N S ______________________________________________________

//------------------------------------------------------------------------------
//! @brief   Case insensitve comparison between two strings
//!
//! Similar function is available within BOOST. But BOOST is not available on
//! Raspberry Pi. Therefore using BOOST would make this code non-portable...
//------------------------------------------------------------------------------
static bool iequals( std::string const & a, std::string const& b) {
  if( b.size() != a.size() ) return false;
  for( unsigned i = 0; i < a.size(); ++i )
    if( tolower( a[i] ) != tolower( b[i] ) ) return false;
  return true;
}

//------------------------------------------------------------------------------
//! @brief   Initialization of device support
//!
//! @param   [in]  after  flag telling if function is called after or before
//!                       record initialization
//!
//! @return  In case of error return -1, otherwise return 0
//------------------------------------------------------------------------------
long devRokuInit( int after ) {

  if ( 0 == after ) { // before records have been initialized
    static bool firstRunBefore = true;
    if ( !firstRunBefore ) return OK;
    firstRunBefore = false;


  } else {

    static bool firstRunAfter = true;
    if ( !firstRunAfter ) return OK;
    firstRunAfter = false;

  }

  return OK;

}

//-------------------------------------------------------------------------------
//! @brief   Common initialization of the record
//!
//! @param   [in]  prec       Address of the record calling this function
//! @param   [in]  pconf      Address of record configuration
//!
//! @return  In case of error return -1, otherwise return 0
//------------------------------------------------------------------------------
long devRokuInitRecord( dbCommon *prec, devRoku_rec_t* pconf ){

  char httpMessage[100] = "";
  std::vector< std::string > options;

  if( INST_IO != pconf->ioLink->type ) {
    std::cerr << prec->name << ": Invalid link type for INP/OUT field: "
              << pamaplinkType[ pconf->ioLink->type ].strvalue
              << std::endl;
    return ERROR;
  }

  std::istringstream ss( pconf->ioLink->value.instio.string );
  std::string option;
  while( std::getline( ss, option, ' ' ) ) options.push_back( option );

  if( options.size() > 1 || options.empty() ) { // Roku outputs for now have only 1 argument
    std::cerr << prec->name << ": Invalid INP/OUT field: " << ss.str() << "\n"
              << "    Syntax is \"<httpMessage>\" - For now Roku outputs have only 1 argument" << std::endl;
    return ERROR;
  }

  strcpy(httpMessage, options.at(0).c_str());
  //httpMessage = options.at(0);

  try{
    // Leds are always output:
    if( !pconf->output ) {
      //RokuManager::instance().setDirection( gpioID, RokuManager::OUTPUT );
        std::cerr << prec->name << ": " << "Leds are always output, not input";
        return ERROR;
    }

  } catch( GpioManagerWarning &e ) {
    std::cerr << prec->name << ": " << e.what() << std::endl;
  } catch( GpioManagerError &e ) {
    std::cerr << prec->name << ": " << e.what() << std::endl;
    return ERROR;
  }

  devRoku_info_t *pinfo = new devRoku_info_t;
  pinfo->gpio = 0;

  //pinfo->httpMessage = httpMessage;
  strcpy(pinfo->httpMessage, httpMessage);
  pinfo->prec = prec;
  pinfo->pcallback = NULL;  // just to be sure

  // I/O Intr handling
  scanIoInit( &pinfo->ioscanpvt ); // TALVEZ SEJA UM PROBLEMA! :D

  prec->dpvt = pinfo;

  return OK;
}

//------------------------------------------------------------------------------
//! @brief   Callback for asynchronous handling of set parameters
//!
//! This callback processes the the record defined in callback user.
//!
//! @param   [in]  pcallback   Address of EPICS CALLBACK structure
//------------------------------------------------------------------------------
void devRokuCallback( CALLBACK *pcallback ) {
  void *puser;
  callbackGetUser( puser, pcallback );

  devRoku_info_t *pinfo = (devRoku_info_t *)puser;

  dbScanLock( pinfo->prec );
  dbProcess( pinfo->prec );
  dbScanUnlock( pinfo->prec );
}

//------------------------------------------------------------------------------
//! @brief   Wrapper to write GPIO value
//!
//! @param   [in]  pinfo  Address of private data from record
//!
//! @return  ERROR in case of an error, otherwise OK
//------------------------------------------------------------------------------
long devRokuWrite( devRoku_info_t *pinfo ){
  try{
    RokuManager::instance().sendCommand( pinfo->httpMessage );
  } catch( GpioManagerError &e ) {
    strncpy( pinfo->errmsg, e.what(), 255 );
    return ERROR;
  }
  return OK;
}

extern "C" {
  //----------------------------------------------------------------------------
  //! @brief   EPICS iocsh callable function to call constructor
  //!          for the GpioConst class
  //!
  //! @param   [in]  board  type of ARM-Board
  //----------------------------------------------------------------------------
  int devRokuConstConfigure( const char *board ) {
  }
  static const iocshArg initArg0 = { "board", iocshArgString };
  static const iocshArg * const initArgs[] = { &initArg0 };
  static const iocshFuncDef initFuncDef = { "GpioConstConfigure", 1, initArgs };
  static void initCallFunc( const iocshArgBuf *args ) {
    devRokuConstConfigure( args[0].sval );
  }

  //----------------------------------------------------------------------------
  //! @brief   Register functions to EPICS
  //----------------------------------------------------------------------------
  void devRokuConstRegister( void ) {
    static int firstTime = 1;
    if ( firstTime ) {
      iocshRegister( &initFuncDef, initCallFunc );
      firstTime = 0;
    }
  }
  epicsExportRegistrar( devRokuConstRegister );
}
