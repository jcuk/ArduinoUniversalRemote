/* 
    remoteCore.h
    
    Arduino Universal IR Remote controller
    Copyright (C)  2015 Jason Crane
    jasoncraneuk (at) hotmail.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/
#ifndef RemoteCore_H
#define RemoteCore_H

#include "Arduino.h"
#include <IRremote.h>

class RemoteCore
{
  public:
    void begin();
    void sendCode(unsigned int decode_type,
      unsigned long value,
      unsigned long address,
      unsigned int numBits,
      int repeat,
      bool toggle);
    void readIRCode();
    void resetLastCode();
    decode_results lastDecodedCode();
      
  private:
    IRsend _irsend;
    decode_results _results;
    void resetCode(decode_results *results);
};

#endif
