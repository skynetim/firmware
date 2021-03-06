/* 
 *                SSSSS  kk                            tt    
 *               SS      kk  kk yy   yy nn nnn    eee  tt    
 *                SSSSS  kkkkk  yy   yy nnn  nn ee   e tttt  
 *                    SS kk kk   yyyyyy nn   nn eeeee  tt    
 *                SSSSS  kk  kk      yy nn   nn  eeeee  tttt 
 *                               yyyyy                         
 * 
 * SkynetClient for http://skynet.im, OPEN COMMUNICATIONS NETWORK & API FOR 
 * THE INTERNET OF THINGS.
 *
 * This sketch parses any messages it receives and echos them to the Serial.
 *
 * Should work with any cc3000 shield or breakout like Adafruit or Sparkfun:
 * https://www.sparkfun.com/products/12071
 *
 * However no matter which you choose, we have a requirement on the Sparkfun
 * library:
 * https://github.com/sparkfun/SFE_CC3000_Library/
 *
 * You will notice we're using F() in Serial.print which might be new to you
 * Its covered briefly on the arduino print page but it means we can store
 * our strings in flash, instead of in ram. 
 * 
 * You can turn on debugging within SkynetClient.h by uncommenting 
 * #define SKYNETCLIENT_DEBUG
 */

#include <EEPROM.h>
#include "SkynetClient.h"
#include "jsmn.h"
#include <SPI.h>
#include <SFE_CC3000.h>
#include <SFE_CC3000_Client.h>

// Pins
#define CC3000_INT      2   // Needs to be an interrupt pin (D2/D3) //Adafruit is 3
#define CC3000_EN       7   // Can be any digital pin //Adafruit is 5
#define CC3000_CS       10  // Preferred is pin 10 on Uno

// Global Variables
SFE_CC3000 wifi = SFE_CC3000(CC3000_INT, CC3000_EN, CC3000_CS);

SFE_CC3000_Client client = SFE_CC3000_Client(wifi);

// Constants
char ssid[] = "yournetworkname";     //  your network SSID (name)
char pass[] = "yourpassword";  // your WPA network password
unsigned int ap_security = WLAN_SEC_WPA2; // Security of network
unsigned int timeout = 30000;             // Milliseconds

SkynetClient skynetclient(client);

char hostname[] = "skynet.im";
int port = 80;

void setup()
{
  Serial.begin(9600);
  
  // Initialize CC3000 (configure SPI communications)
  if ( wifi.init() ) {
    Serial.println(F("CC3000 initialization complete"));
  } else {
    Serial.println(F("Something went wrong during CC3000 init!"));
    while(1);
  }
  
  skynetclient.setMessageDelegate(onMessage);
}

void onMessage(const char * const data) {
  Serial.print(F("Parse: "));
  Serial.println(data);
}

void loop() {

  while(!skynetclient.monitor())
  {
    // Connect to AP using DHCP
    Serial.print(F("Connecting to SSID: "));
    Serial.println(ssid);
    if(wifi.connect(ssid, ap_security, pass, timeout)) 
    {
      bool skynetStatus = false;
      do {
        skynetStatus = skynetclient.connect(hostname, port);
      } while (!skynetStatus);
      
      Serial.println(F("Connected!"));
      
      char uuid[UUIDSIZE];
    
      skynetclient.getUuid(uuid);
      Serial.print(F("uuid: "));
      Serial.println(uuid);
      
      skynetclient.getToken(uuid);
      Serial.print(F("token: "));
      Serial.println(uuid);
    }else
    {
      Serial.println(F("Error: Could not connect to AP"));
    }
  }
}