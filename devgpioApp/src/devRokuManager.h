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
// devGpio is distributed in the hope that it will be useful,
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

#ifndef DEV_GPIO_MANAGER_H
#define DEV_GPIO_MANAGER_H

//_____ I N C L U D E S ________________________________________________________

// ANSI C/C++ includes
#include <map>
#include <string>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <curl/curl.h>

// EPICS includes
#include <epicsTypes.h>

// local includes

//_____ D E F I N I T I O N S __________________________________________________

class RokuManager {
 public:

  void sendCommand ( char httpAddress[100] );

  static RokuManager& instance() {
    static RokuManager rinstance;
    return rinstance;
  }

 private:
  struct GPIO {
    bool exported;
  };

  RokuManager();
  ~RokuManager();
  RokuManager( RokuManager const& rother ); // Not implemented
  RokuManager& operator=( RokuManager const& rother ); // Not implemented

  std::string _gpiobase;
};

#endif

