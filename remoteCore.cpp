/*
    remoteCore.cpp
    
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
    
#include "Arduino.h"
#include "remoteCore.h"
#include <IRremote.h>
#include <IRremoteInt.h>

int TX_STATUS = 5;
int RX_STATUS = 6;

int RX_PIN = 9;
IRrecv irrecv(RX_PIN);

void RemoteCore::begin() {
    pinMode(TX_STATUS, OUTPUT);
    pinMode(RX_STATUS, OUTPUT);

    digitalWrite(RX_STATUS, LOW);
    digitalWrite(TX_STATUS, LOW);
  
    irrecv.enableIRIn();

}

/* Transmit the given IR code via the Tx LED */
void RemoteCore::sendCode(unsigned int decode_type,
    unsigned long value,
    unsigned long address,
    unsigned int numBits,
    int repeat,
    bool toggle) {
 
  digitalWrite(TX_STATUS, HIGH);  
  
  if (decode_type == NEC) {
    if (repeat) {
      _irsend.sendNEC(REPEAT, numBits);
      Serial.println("Sent NEC repeat");
    } 
    else {
      _irsend.sendNEC(value, numBits);
      Serial.print("Sent NEC");
      Serial.println(value, HEX);
    }
  } 
  else if (decode_type == SONY) {
    _irsend.sendSony(value, numBits);
    Serial.print("Sent Sony");
    Serial.println(value, HEX);
  } 
  else if (decode_type == RC5 || decode_type == RC6) {
    // Put the toggle bit into the code to send
    value = value & ~(1 << (numBits - 1));
    value = value | (toggle << (numBits - 1));
    if (decode_type == RC5) {
      Serial.print("Sent RC5");
      Serial.println(value, HEX);
      _irsend.sendRC5(value, numBits);
    } 
    else {
      _irsend.sendRC6(value, numBits);
      Serial.print("Sent RC6");
      Serial.println(value, HEX);
    }
  }
  else if (decode_type == PANASONIC) {
    _irsend.sendPanasonic(address, value);
    Serial.print("Sent Panasonic");
    Serial.println(address, HEX);
    Serial.println(value, HEX);
  } 
  else if (decode_type == JVC) {
    _irsend.sendJVC(value, numBits, repeat==1);
    Serial.print("Sent JVC");
    Serial.println(value, HEX);
  } 
  else if (decode_type == SAMSUNG) {
    _irsend.sendSAMSUNG(value, numBits);
    Serial.print("Sent Samsung");
    Serial.println(value, HEX);
  }
  else if (decode_type == WHYNTER) {
    _irsend.sendSAMSUNG(value, numBits);
    Serial.print("Sent Whynter");
    Serial.println(value, HEX);
  }
  else if (decode_type == AIWA_RC_T501) {
    _irsend.sendAiwaRCT501(value);
    Serial.print("Sent Aiwa");
    Serial.println(value, HEX);
  }
  else if (decode_type == LG) {
    _irsend.sendLG(value, numBits);
    Serial.print("Sent LG");
    Serial.println(value, HEX);
  }
  else if (decode_type == DISH) {
    _irsend.sendDISH(value, numBits);
    Serial.print("Sent Dish");
    Serial.println(value, HEX);
  }
  else if (decode_type == SHARP) {
    _irsend.sendSharp(value, numBits);
    Serial.print("Sent Sharp");
    Serial.println(value, HEX);
  }
  else if (decode_type == DENON) {
    _irsend.sendDenon(value, numBits);
    Serial.print("Sent Denon");
    Serial.println(value, HEX);
  }
  else if (decode_type == UNKNOWN /* i.e. raw */) {
    Serial.println("Unknown type");
    //TODO: suport raws
  } else {
    Serial.println("Incorrect decode type");
    //NB Sending of Sanyo and Mitsubishi is not implemented in the IR decoder library
    // and Pronto decoding is not implented
  }
  
  
  digitalWrite(TX_STATUS, LOW);  
}

/* Check for an incoming IR code. If one is found, decode and store it */
void RemoteCore::readIRCode() {
  decode_results new_results;
  this->resetCode(&new_results);
  
    if (irrecv.decode(&new_results)) {
      digitalWrite(RX_STATUS, HIGH);
      
      //Recieved valid code, store for later
        unsigned int rawCodes[100];
        int codeType = new_results.decode_type;
        int count = new_results.rawlen;
        if (codeType == UNKNOWN) {
          Serial.println("Received unknown code, saving as raw");
//          int codeLen = new_results.rawlen - 1;
//          // To store raw codes:
//          // Drop first value (gap)
//          // Convert from ticks to microseconds
//          // Tweak marks shorter, and spaces longer to cancel out IR receiver distortion
//          for (int i = 1; i <= codeLen; i++) {
//            if (i % 2) {
//              // Mark
//              rawCodes[i - 1] = new_results.rawbuf[i]*USECPERTICK - MARK_EXCESS;
//              Serial.print(" m");
//            } 
//            else {
//              // Space
//              rawCodes[i - 1] = new_results.rawbuf[i]*USECPERTICK + MARK_EXCESS;
//              Serial.print(" s");
//            }
//            Serial.print(rawCodes[i - 1], DEC);
//          }
//          Serial.println("");
        }
        else {
      
          if (codeType == NEC) {
            Serial.print("Received NEC: ");
            if (new_results.value == REPEAT) {
              // Don't record a NEC repeat value as that's useless.
              Serial.println("repeat; ignoring.");
            } 
          } 
          
      //    else if (codeType == SONY) {
      //      Serial.print("Received SONY: ");
      //    } 
      //    else if (codeType == RC5) {
      //      Serial.print("Received RC5: ");
      //    } 
      //    else if (codeType == RC6) {
      //      Serial.print("Received RC6: ");
      //    } 
      //    else if (_results.decode_type == SAMSUNG) {
      //      Serial.print("Decoded SAMSUNG: ");
      //    }
      //    else if (_results.decode_type == JVC) {
      //      Serial.print("Decoded JVC: ");
      //    }
      //    else if (_results.decode_type == PANASONIC) {
      //      Serial.print("Decoded Panasonic: ");
      //    }
      //    else if (_results.decode_type == WHYNTER) {
      //      Serial.print("Decoded Whynter: ");
      //    }
      //    else if (_results.decode_type == AIWA_RC_T501) {
      //      Serial.print("Decoded Aiwa: ");
      //    }
      //    else if (_results.decode_type == LG) {
      //      Serial.print("Decoded LG: ");
      //    }
      //    else if (_results.decode_type == SANYO) {
      //      Serial.print("Decoded Sanyo: ");
      //    }
      //    else if (_results.decode_type == MITSUBISHI) {
      //      Serial.print("Decoded Mitsubishi: ");
      //    }
      //    else if (_results.decode_type == DISH) {
      //      Serial.print("Decoded Dish: ");
      //    }
      //    else if (_results.decode_type == SHARP) {
      //      Serial.print("Decoded Sharp: ");
      //    }
      //    else if (_results.decode_type == DENON) {
      //      Serial.print("Decoded Denon: ");
      //    }
      //    else if (_results.decode_type == PRONTO) {
      //      Serial.print("Decoded Pronto: ");
      //    }
      //    else {
      //      Serial.print("Unexpected codeType ");
      //      Serial.print(codeType, DEC);
      //      Serial.println("");
      //    }
          Serial.println(new_results.value, HEX);
      
          if (new_results.value != REPEAT) {
      
              //Store results for later
              _results.value                 = new_results.value;
              _results.address           = new_results.address;
              _results.decode_type   = new_results.decode_type;
              _results.bits                    = new_results.bits;
              _results.rawlen              = new_results.rawlen;
              
              Serial.println(_results.value, HEX);
              Serial.println(_results.address, HEX);
              Serial.println(_results.decode_type);
              Serial.println(_results.bits);
              //Serial.println(_results.repeat);
              //Serial.println(_results.toggle);
          }
        }

      irrecv.resume(); // resume receiver
      delay(100);
      digitalWrite(RX_STATUS, LOW);
      Serial.println("IR recieved");
  }
}

void RemoteCore::resetLastCode() {
  this->resetCode(&_results);
}

void RemoteCore::resetCode(decode_results *results) {
    results->decode_type = UNKNOWN;
    results->value = 0;
    results->address = 0;
    results->bits = 0;
    results->rawlen = 0;
}

decode_results RemoteCore::lastDecodedCode() {
  return _results;
}

