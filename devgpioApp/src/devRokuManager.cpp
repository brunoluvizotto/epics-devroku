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

//! @file devRokuManager.cpp
//! @author F.Feldbauer
//! @date 13 Aug 2015
//! @brief Implementation of GPIO Manager class

//_____ I N C L U D E S ________________________________________________________

// ANSI C/C++ includes
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unistd.h>

// EPICS includes

// local includes
#include "devRokuManager.h"
#include "devGpioErrors.h"

//_____ D E F I N I T I O N S __________________________________________________

//_____ G L O B A L S __________________________________________________________

//_____ L O C A L S ____________________________________________________________
static const char ERR_BEGIN[] = "\033[31;1m";
static const char ERR_END[] = "\033[0m\n";

//_____ F U N C T I O N S ______________________________________________________

//------------------------------------------------------------------------------
//! @brief   Standard Constructor
//------------------------------------------------------------------------------
RokuManager::RokuManager() {
  _gpiobase = "/sys/class/leds/beaglebone:green:usr"; // "/sys/class/gpio/gpio";
}

//------------------------------------------------------------------------------
//! @brief   Standard Destructor
//------------------------------------------------------------------------------
RokuManager::~RokuManager() {
}

//------------------------------------------------------------------------------
//! @brief   Set Value of GPIO
//------------------------------------------------------------------------------
void RokuManager::sendCommand( char httpAddress[100] ) {

    CURL *curl;
    CURLcode res;

    char *url;

    url = httpAddress; //argv[1];

    /* In windows, this will init the winsock stuff */
    curl_global_init(CURL_GLOBAL_ALL);

    /* get a curl handle */
    curl = curl_easy_init();
    if(curl) {
      /* we want to use our own read function */
      curl_easy_setopt(curl, CURLOPT_URL, url);

      /* enable uploading */
      curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "");

      /* Now run off and do what you've been told! */
      res = curl_easy_perform(curl);
      /* Check for errors */
      if(res != CURLE_OK)
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));

      /* always cleanup */
      curl_easy_cleanup(curl);
    }

    curl_global_cleanup();

}
