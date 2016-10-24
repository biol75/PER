

// don't use pin 4 or 10-12 either...

// code derived from https://github.com/wadelab/flyCode/blob/master/collectData/fly_arduino/WebServer/WebServer.ino
//#define __wifisetup__


#define due5

//_____________________________________________________

#ifdef due6
#define MAC_OK 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
//biolpc2793 [in use in lab with Emily and Richard]
#endif

#ifdef due5
#define MAC_OK 0x90, 0xA2, 0xDA, 0x0F, 0x42, 0x02
//biolpc2804

#endif

#ifdef due1
#define MAC_OK 0x90, 0xA2, 0xDA, 0x0E, 0x09, 0xA2
//90-A2-DA-0E-09-A2 biolpc2886 [in use for Sultan, has fiber]
#endif

#ifdef due2
#define MAC_OK 0x90, 0xA2, 0xDA, 0x0F, 0x6F, 0x9E
//90-A2-DA-0E-09-A2 biolpc2898 [used in testing...]
#endif

#ifdef due3
#define MAC_OK 0x90, 0xA2, 0xDA, 0x0F, 0x75, 0x17
//90-A2-DA-0E-09-A2 biolpc2899
#endif

#ifdef due4
#define MAC_OK 0x90, 0xA2, 0xDA, 0x0E, 0x09, 0xA3
//90-A2-DA-0E-09-A3 biolpc2939 144.32.86.171
#endif

#ifdef __wifisetup__
#define MAC_OK
#endif

//#if defined(__AVR_ATmega2560__  __SAM3X8E__
/*

  Prototype : put the grey wire in ground, and purple wire in pin7

  Based on Web Server

  A simple web server that shows the value of the analog input pins.
  using an Arduino Wiznet Ethernet shield.

  Circuit:
   Ethernet shield attached to pins 10, 11, 12, 13
   Analog inputs attached to pins A0 through A5 (optional)

  created 18 Dec 2009
  by David A. Mellis
  modified 9 Apr 2012
  by Tom Igoe

*/
#define SS_SD_CARD   4
#define SS_ETHERNET 10
// is 10 on normal uno

#include <SPI.h>

#ifndef __wifisetup__


#include <Ethernet.h>

#else

#include <WiFi.h>

#endif

#include <SD.h>
//#include <FixFFT.h>

short iIndex = 0 ;


//
volatile byte usedLED  = 0;
const byte fiberLED = 8 ;
const byte noContactLED = 2;

// define LED mapping here
const byte bluvioletLED = 8 ;
const byte amberled = 6;
const byte whiteled = 11;
const byte cyaled = 9;
const byte extrawhitepin = 53;

#ifdef ARDUINO_LINUX
const byte redled = 3;
const byte grnled = 5;
const byte bluLED = 6;
#endif

#ifdef due3
const byte redled = 7;
const byte grnled = 3;
const byte bluLED = 5;
#endif

#ifdef due4
const byte redled = 7;
const byte grnled = 3;
const byte bluLED = 5;
#endif

#ifdef due5
const byte redled = 7;
const byte grnled = 3;
const byte bluLED = 5;
#endif

#ifdef due6
const byte redled = 6;
const byte grnled = 5;
const byte bluLED = 7;
#endif

//#ifdef ESP8266
//const byte redled = 4;
//const byte grnled = 0;
//const byte bluLED = 5;
//#endif

#ifdef ESP8266
const byte redled = 13; // Farnell 2080005
const byte grnled = 15; // 1855562
const byte bluLED = 2;  // 1045418
#endif
const byte analogPin = 0 ;
//const byte connectedPin = A1;
byte iGainFactor = 1 ;

int myIntensity = 128 ;
int myBackground = 20 ;
int myStep = 64 ;
int myTime = 3000 ;
int myCol = 1 ;


uint8_t second, myminute, hour, day, month;
uint16_t year ;

const short MaxInputStr = 130 ;
String MyInputString = String(MaxInputStr + 1);
String MyReferString = String(MaxInputStr + 1);
char cInput [130];


#ifndef MAC_OK
#error please define which arduino you are setting up
#endif

#ifndef __wifisetup__
//

byte mac[] = { MAC_OK } ;
IPAddress myIP, theirIP, dnsIP ;

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);
EthernetClient client ;
#include <Dns.h>

#else

WiFiServer server (80);
WiFiClient client (80);

#endif


void setup() {

  // ...
  pinMode(SS_SD_CARD, OUTPUT);
  pinMode(SS_ETHERNET, OUTPUT);

  pinMode(noContactLED, OUTPUT);

  digitalWrite(SS_SD_CARD, HIGH);  // HIGH means SD Card not active
  digitalWrite(SS_ETHERNET, HIGH); // HIGH means Ethernet not active


  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

#ifdef __wifisetup__
  //char ssid[] = "SSID";     //  your network SSID (name)
  //char pass[] = "PASSWD";  // your network password
#include "./secret.h"

  int status = WL_IDLE_STATUS;
  while ( status != WL_CONNECTED)
  {
    Serial.print F("Attempting to connect to Network named: ");
    Serial.println(ssid);                   // print the network name (SSID);

    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(2000); // 2 s seems enough
  }
  server.begin();                           // start the web server on port 80
  printWifiStatus();                        // you're connected now, so print out the status

#else
  digitalWrite(SS_ETHERNET, LOW); // HIGH means Ethernet not active
  Serial.println F("Setting up the Ethernet card...\n");
  // start the Ethernet connection and the server:
  if (1 != Ethernet.begin(mac))
  {
    // Setup for eg an ethernet cable from Macbook to Arduino Ethernet shield
    // other macbooks or mac airs may assign differnt local networks
    //
    Serial.println F("DHCP failed, trying 172, 16, 1, 10");
    Serial.println F("Please set your mac ethernet to Manually and '172.16.1.1'");
    byte ip[] = { 172, 16, 1, 10 };
    Ethernet.begin(mac, ip);
  };
  server.begin();
  Serial.println F("server is at ");
  myIP = Ethernet.localIP() ;
  dnsIP = Ethernet.dnsServerIP();
  Serial.print(myIP);
  Serial.print(" using dns server ");
  Serial.println(dnsIP);

#endif

  goColour(0, 0, myBackground);
}

#ifndef __wifisetup__

//bool readMAC()
//{
//  // read a byte from the current address of the EEPROM
//  // start at 0
//  int address = 0;
//  byte value;
//  char cMac [4];
//  cMac[3] = '\0' ;
//  while (address < 20)
//  {
//    value = EEPROM.read(address);
//    char * c = " ";
//    *c = value ;
//    if (address < 3)
//    {
//      cMac [address] = (char)value  ;
//    }
//    else
//    {
//      if (address < 9)
//      {
//        mac [address - 3] = value ;
//      }
//    }
//
//    //
//    //    Serial.print(address);
//    //    Serial.print("\t");
//    //    Serial.print (c);
//    //    Serial.print("\t");
//    //    Serial.print(value, DEC);
//    //    Serial.print("\t");
//    //    Serial.print(value, HEX);
//    //    Serial.println();
//
//    // advance to the next address of the EEPROM
//    address = address + 1;
//  }
//  int iComp = strncmp (cMac, "MAC", 3);
//
//  //  Serial.print ("Comparing :") ;
//  //  Serial.print (cMac);
//  //  Serial.print (" with MAC gives ");
//  //  Serial.println (iComp);
//
//  return ( 0 == iComp) ;
//
//}

#else
//ifdef __wifisetup__

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print F("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  myIP = WiFi.localIP();
  Serial.print F("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print F("signal strength (RSSI):");
  Serial.print (rssi);
  Serial.println F(" dBm");
  // print where to go in a browser:
  Serial.print F("To see this page in action, open a browser to http://");
  Serial.println (ip);
}

#endif





void sendHeader (const String & sTitle, const String & sINBody = "", bool isHTML = true, char * pDate = NULL)
{
  // send a standard http response header
  client.println F("HTTP/1.1 200 OK");
  if (isHTML)
  {
    client.println F("Content-Type: text/html");
  }
  else
  {
    client.println F("Content-Type: text/plain");
  }
  if (pDate)
  {
    client.print F("Last-Modified: ");
    client.println (pDate);
  }
  client.println F("Connection: close");  // the connection will be closed after completion of the response
  client.println();
  if (isHTML)
  {
    client.println F("<!DOCTYPE HTML><html><title>");
    client.println (sTitle);
    client.println F("</title><body ");
    client.println (sINBody);
    client.println F(">");
  }
}

void sendFooter()
{
  client.println F("</body></html>");
}



void goColour(const byte r, const byte g, const byte b)
{
  analogWrite( redled, r );
  analogWrite( grnled, g );
  analogWrite( bluLED, b );

}

void goColour(const byte r)
{
  goColour (r, r, r);
}





void flashLED (int time_on, int time_off, int iTimes) // ms
{
  int r = 0;
  int g = 0;
  int b = myBackground;
  switch ( myCol )
  {
    case 1:
      r = myIntensity ;
      break ;
    case 2:
      g = myIntensity ;
      break ;
    case 3:
      b = myIntensity ;
      break ;
  }

  for (int i = 0; i < iTimes; i++)
  {
    delayMicroseconds(long(time_on) * 100L);
    goColour(r, g, b ) ;

    delayMicroseconds(long(time_off) * 100L);
    goColour(0, 0, myBackground);
  }
}

int ParseInput(String sSearch)
{
  int fPOS = MyInputString.indexOf (sSearch);
  fPOS = fPOS + sSearch.length(); ; // length of myStep=
  int fEnd = MyInputString.indexOf ("&", fPOS);
  if (fEnd < 0) fEnd = MyInputString.indexOf (" ", fPOS);
  String sValue = MyInputString.substring(fPOS, fEnd);
  Serial.print (sSearch + " seems to be :" ) ;
  int myVal = sValue.toInt();
  Serial.println (myVal);
  return myVal ;
}

void sendReply ()
{
  int exp_size = MaxInputStr + 2 ;
  Serial.println(MyInputString);

  //GET /?time=3&background=20&intensity=128&myStep=64&col=blue HTTP/1.1
  int fPOS = MyInputString.indexOf F("intensity=");
  // asking for new sample
  if (fPOS > 0)
  {
    Serial.println (MyInputString);

    //int myIntensity = 128 ;
    //int myBackground = 20 ;
    //int myStep = 64 ;
    //int myTime = 3000 ;
    //int myCol = 1 ; //

    myStep = ParseInput F("myStep=");
    myTime = ParseInput F("time=");
    myBackground = ParseInput F("background=");
    myIntensity = ParseInput F("intensity=");
    myCol = ParseInput F("col=");

    writehomepage ();

    flashLED (1, 9, myTime);
    return ;
  }


  //light up
  fPOS = MyInputString.indexOf F("white/");
  if (fPOS > 0)
  {
    goColour(255) ;
    return ;
  }



  fPOS = MyInputString.indexOf F("red/");
  if (fPOS > 0)
  {
    goColour(255, 0, 0) ;
    return ;
  }
  fPOS = MyInputString.indexOf F("blue/");
  if (fPOS > 0)
  {
    goColour(0, 0, 255) ;
    return ;
  }
  fPOS = MyInputString.indexOf F("green/");
  if (fPOS > 0)
  {
    goColour(0, 255, 0) ;
    return ;
  }
  fPOS = MyInputString.indexOf F("black/");
  if (fPOS > 0)
  {
    goColour(0);
    return ;
  }

  // default - any other url
  goColour(0, 0, myBackground);
  writehomepage();
  MyInputString = "";
}

void loop()
{

  String sTmp = "";
  MyInputString = "";
  //getData ();
  boolean currentLineIsBlank = true;
  // listen for incoming clients

  client = server.available();
  if (client) {
    Serial.println F("new client");
    MyInputString = "";
    // an http request ends with a blank line
    while (client.connected()) {
      if (client.available()) {            // if there's bytes to read from the client,
        char c = client.read();

        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank)
        {
          sendReply ()  ;
          break ;
        }

        if (c == '\n')
        {
          //Serial.print("Input string now " );
          //Serial.println (sTmp);

          // you're starting a new line
          // see if we need to save the old one
          if (sTmp.indexOf F("GET") >= 0)
          {
            MyInputString = sTmp;
          }
          int iTmp = sTmp.indexOf F("Referer:") ;
          if (iTmp >= 0)
          {
            String sHost = sTmp.substring(16);
            //Serial.println (sHost) ;
            int iSlash = sHost.indexOf ("/");
            sHost = sHost.substring(0, iSlash);
            //Serial.println (sHost) ;
            DNSClient dc;
            dc.begin(dnsIP);
            dc.getHostByName(sHost.c_str(), theirIP);
            //S//erial.print F("Their IP is ");
            //Serial.println (theirIP) ;
            if (myIP != theirIP)
            {
              //Serial.println F("this does not appear to be my ip");
              MyReferString = sTmp.substring(iTmp + 9);
              //Serial.print F("Ref string now :" );
              //Serial.println (MyReferString);
            }
            //            else
            //            {
            //              Serial.println F("this appears to be my ip");
            //              Serial.print F("Ref string unchanged at :" );
            //              Serial.println (MyReferString);
            //            }


          }
          sTmp = "";

          currentLineIsBlank = true;
        }
        else if (c != '\r')
        {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
          if (sTmp.length() < MaxInputStr)
          {
            sTmp.concat(c);
          }
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    //Serial.println("client disonnected: Input now:" + MyInputString + "::::");
  }
}

void writehomepage ()
{
  client.print F("<!DOCTYPE html> <html> <head> <base href=\"http://");
  client.print  (myIP);
  client.println("\"> </head> <body> ");
  client.println("<form action=\"/\">");
  client.println("Stimulus duration (s) <BR>");
  client.println("<input type=\"radio\" name=\"time\"  id=\"one\" value=\"1000\">1<br>");
  client.println("<input type=\"radio\" name=\"time\"  id=\"three\" value=\"3000\">3<br>");
  client.println("<input type=\"radio\" name=\"time\"  id=\"five\" value=\"5000\">5<br>");
  client.println("");
  client.println("<p style=\"color:red\"><HR>Blue Background");
  client.println("");
  client.print("<input type=\"number\" id=\"myNumber\" name=\"background\" value=\"");
  client.print(myBackground);
  client.println("\">");
  client.println("<input type=\"hidden\" id=\"myStep\" name=\"mystep\" value=\"64\">");
  client.println("");
  client.println("");
  client.println("<BR><BR><HR><p style=\"color:Black\">");
  client.println("");
  client.println("<table style=\"text-align: left; width: 50%;\" border=\"1\" cellpadding=\"2\" cellspacing=\"2\">");
  client.println("<tbody><tr><td>Intensity</td><td>Colour</td></tr>");
  client.println("");
  client.println("<tr><td>");
  client.print("<input type=\"number\" id=\"myIntensity\" name=\"intensity\" value=\"");
  client.print(myIntensity);
  client.println("\">");
  client.println("<input type=\"submit\" value=\"Submit\">");
  client.println("</td><td>");
  client.println("<input type=\"radio\" name=\"col\" id=\"red\" value=\"1\">red<br>");
  client.println("<input type=\"radio\" name=\"col\" id=\"green\" value=\"2\">green<br>");
  client.println("<input type=\"radio\" name=\"col\" id=\"blue\" value=\"3\">blue<br><BR>");
  client.println("</td></tr></table> </form><BR>Intensity Presets: <BR>");

  client.println("<button onclick = 'document.getElementById( \"myIntensity\" ).value = 0 '> 0 </button> ");
  client.println("<button onclick = 'document.getElementById( \"myIntensity\" ).value = 64 '> 64 </button> ");
  client.println("<button onclick = 'document.getElementById( \"myIntensity\" ).value = 128 '> 128 </button> ");
  client.println("<button onclick = 'document.getElementById( \"myIntensity\" ).value = 255 '> 255 </button><BR> ");
  client.println("<button onclick = 'document.getElementById( \"myIntensity\" ).value = document.getElementById( \"myNumber\" ).value '> Background </button><BR> " );
  client.println("");
  switch (myCol)
  {
    case 1:
      client.println("<script>var stimColor = \"red\";");
      break;
    case 2:
      client.println("<script>var stimColor = \"green\";");
      break;
    case 3:
    default:
      client.println("<script>var stimColor = \"blue\";");
      break;
  }
  client.println("document.getElementById(stimColor).checked = true; </script>");

  client.println("");
  switch (myTime)
  {
    case 1000:
      client.println("<script>var stimTime = \"one\";");
      break;
    case 3000:
    default:
      client.println("<script>var stimTime = \"three\";");
      break;
    case 5000:
      client.println("<script>var stimTime = \"five\";");
      break;
  }
  client.println("document.getElementById(stimTime).checked = true; </script>");
  client.println("");
  client.println("</body></html>");
}

