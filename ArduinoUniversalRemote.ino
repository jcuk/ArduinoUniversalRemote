/*
    ArduinoUniversalRemote.ino
    
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
        
 Circuit:
 * Ethernet shield pins 10, 11, 12, 13
 * IR led out pin 3
 * TSOP4838 IR reciever in pin 9
 * Rx Status LED Pin 5
 * Tx Status LEDs pin 6
 */
 
 #include <IRremote.h>
#include <IRremoteInt.h>
#include "remoteCore.h"
#include <stdio.h>
#include <string.h>

#include <SPI.h>
#include <Ethernet.h>

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 1, 80);

String version="1.0.0";

//Wrapper for IRRemote services
RemoteCore remoteCore;

// Initialize the Ethernet server library
EthernetServer server(80);

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect.
  }
  
  // Start the HTTP and IR services
  Ethernet.begin(mac, ip);
  remoteCore.begin();
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
  
  Serial.print("Version :");
  Serial.println(version);

}

#define BUFSIZ 300
void loop() {
  // listen for incoming connection
  char clientline[BUFSIZ];
  int index = 0;
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        
      if (c != '\n' && c != '\r') {
          clientline[index] = c;
          index++;
          
          if (index >= BUFSIZ) {
              index = BUFSIZ -1;
              
             Serial.print("buffer overflow :");
             Serial.println(c);
          }

          // read more data
          continue;
      }

      clientline[index] = 0;
      index=0;
      
      if (strstr(clientline, "GET /") != 0) {
        char *filename;
        char *param;
          
          filename = clientline + 5; // ignore HTTP/1.1 string
          (strstr(clientline, " HTTP"))[0] = 0;
          
          //Ignore parameters
          (strstr(filename, "?"))[0] = 0;
          
          param = strstr(filename, "/")+1;
          
          Serial.print("Request : ");
          Serial.println(filename);
          //Serial.println(param);
          
          if (strstr(filename,"remote")!=0) {
            
            if (strstr(filename,"/read")!=0) {
              //read IR
              decode_results results = remoteCore.lastDecodedCode();
                  responseHeaders(client, 200);
                  client.println("{ ");
                  client.print(" \"decodeType\": " );
                  client.print(results.decode_type);
                  client.println(",");
                  client.print(" \"value\": \"" );
                  client.print(results.value, HEX);
                  client.println("\",");
                  client.print(" \"address\": \"" );
                  client.print(results.address, HEX);
                  client.println("\",");
                  client.print(" \"bits\": " );
                  client.println(results.bits);
                  client.println("}");
            } else if (strstr(filename,"/send")!=0) {
              char *pch = strtok(param, "/"); 
              pch = strtok(0, "/"); 
              unsigned long value = strtol(pch, 0, 16);
              pch = strtok(0, "/");
              unsigned long address = strtol(pch, 0, 16);
              pch = strtok(0, "/");
              unsigned int decodeType = atoi(pch);
              pch = strtok(0, "/");
              unsigned int numBits = atoi(pch);
              pch = strtok(0, "/");
              unsigned int repeat = atoi(pch);
              pch = strtok(0, "/");
              boolean toggle = atoi(pch);
              
              Serial.println("-----");
              Serial.println(value, HEX);
              Serial.println(address, HEX);
              Serial.println(decodeType);
              Serial.println(numBits);
              Serial.println(repeat);
              Serial.println(toggle);
              
              remoteCore.sendCode(
                decodeType,
                value,
                address,
                numBits,
                repeat,
                toggle);

              response(client, "sent");
            } else if (strstr(filename,"/ping")!=0) {
              response(client, "OK");
            } else if (strstr(filename,"/record")!=0) {

              remoteCore.resetLastCode();              
              
              response(client, "OK");
            } else {
              pageNotFound(client);
            }
            
          } else {
            pageNotFound(client);
            break;
          }
          
      }
      break;
        
      }
    }
    // give the REST client time to receive data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
  
  //Check for incoming IR traffic
  remoteCore.readIRCode();

}

void pageNotFound(EthernetClient client) {
  responseHeaders(client, 404);
  client.println("{ \"error\":\"File not found\"}");
}

void response(EthernetClient client, char *response) {
  //send a standard http response header
  responseHeaders(client, 200);
  client.println("{");
  client.print(" \"response\": \"" );
  client.print(response);
  client.println("\",");
  client.print(" \"version\": \"" );
  client.print(version);
  client.println("\"");
  client.println("}");
}

void responseHeaders(EthernetClient client, int code) {
  client.print("HTTP/1.1 ");
  client.print(code);
  if (code == 404) {
    client.println(" Not Found");
  } else {
    client.println(" OK");
  }
  client.println("Content-Type: application/json;charset=utf-8");
  client.println("Access-Control-Allow-Origin: *");
  client.println("Server: Arduino");
  client.println("Connection: close");
  client.println();
}

