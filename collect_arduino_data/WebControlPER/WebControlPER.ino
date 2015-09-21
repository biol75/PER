

// don't use pin 4 or 10-12 either...

// code derived from https://github.com/wadelab/flyCode/blob/master/collectData/fly_arduino/WebServer/WebServer.ino
//#define __wifisetup__


#define mega1

//_____________________________________________________

#ifdef mega1
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
 * Ethernet shield attached to pins 10, 11, 12, 13
 * Analog inputs attached to pins A0 through A5 (optional)

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
byte usedLED  = 0;
const byte fiberLED = 8 ;
const byte noContactLED = 2;

// define LED mapping here
const byte bluvioletLED = 8 ;
const byte amberled = 6;
const byte whiteled = 11;
const byte cyaled = 9;

#ifdef due4
const byte redled = 7;
const byte grnled = 3;
const byte bluLED = 5;
#else
#ifdef __SAM3X8E__
// fix the LED order in hardware....
const byte redled = 6;
const byte grnled = 5;
const byte bluLED = 7;
#else
const byte redled = 5;
const byte grnled = 6;
const byte bluLED = 8;
#endif
#endif

const byte analogPin = 0 ;
const byte connectedPin = A1;
byte iGainFactor = 1 ;

short intensity = 128 ;



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
  if (true) //1 != Ethernet.begin(mac))
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



  goColour(0, 0, 255, 0, false);

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


void send_GoBack_to_Stim_page ()
{
  client.println F("<A HREF=\"") ;
  if (MyReferString != String("131"))
  {

    //    client.println F(" <script>");
    //    client.println F("function goBack() ");
    //    client.println F("{ window.history.back() }");
    //    client.println F("</script>");

    client.println (MyReferString) ;
    client.println F("\"" );
  }
  //    Serial.print("My reference is :");
  //    Serial.println (MyReferString) ;
  else
  {
    client.print F("javascript:void(0)\" onclick=\"window.home(); ") ;
  }
  client.println F("\">the stimulus selection form</A>  <BR>");
}

void updateColour (const bool boolUpdatePage)
{
  if (boolUpdatePage)
  {
    sendHeader ("Lit up ?", "onload=\"goBack()\" ");
    client.println F("Click to reload");
    send_GoBack_to_Stim_page ();

    sendFooter();
  }
}

void goColour(const byte r, const byte g, const byte b, const byte a, const byte w, const byte l, const byte c,  const bool boolUpdatePage)
{
  analogWrite( redled, r );
  analogWrite( grnled, g );
  analogWrite( bluLED, b );
#ifdef due4
  analogWrite( amberled, a );
  analogWrite( whiteled, w );
  analogWrite( bluvioletLED, l );
  analogWrite( cyaled, c );
#endif
#ifdef due1
  analogWrite( fiberLED, a );
#endif
  updateColour( boolUpdatePage);
}

void goColour(const byte r, const bool boolUpdatePage)
{
  goColour (r, r, r, 0, r, 0, 0, boolUpdatePage);
}

void goColour(const byte r, const byte g, const byte b, const byte f, const bool boolUpdatePage)
{
  goColour (r, g, b, f, 0, 0, 0, boolUpdatePage);
}



//void run_graph()
//{
//  // turn off any LEDs, always do flash with blue
//  goColour(0, 0, 0, 0, false);
//
//  // read the value of  analog input pin and turn light on if in mid-stimulus...
//  short sensorReading = analogRead(connectedPin);
//  //  Serial.print(" sweep is : ");
//  //  Serial.println(sensorReading);
//
//  if (sensorReading < 2 || sensorReading > 4090)
//  {
//    //probably no contact
//    digitalWrite (noContactLED, HIGH);
//    //    Serial.print("on");
//  }
//  else
//  {
//    digitalWrite (noContactLED, LOW);
//  }
//
//  sensorReading = analogRead(analogPin);
//  myGraphData[iIndex] = sensorReading / iGainFactor ;
//  iIndex ++ ;
//  if (iIndex > max_graph_data / 10 && iIndex < max_graph_data / 2)
//  {
//    analogWrite(bluLED, 255);
//  }
//  else
//  {
//    analogWrite(bluLED, 0);
//  }
//
//  sendHeader ("Graph of last sweep") ;
//  client.println F("<script>");
//
//  // script to reload ...
//  client.println F("var myVar = setInterval(function(){myTimer()}, 1000);"); //mu sec
//  client.println F("function myTimer() {");
//  client.println F("location.reload(true);");
//  client.println F("};");
//
//  client.println F("function myStopFunction() {");
//  client.println F("clearInterval(myVar); }");
//  client.println F("");
//  client.println F("</script>");
//  // now do the graph...
//  client.println F("<canvas id=\"myCanvas\" width=\"640\" height=\"520\" style=\"border:1px solid #d3d3d3;\">");
//  client.println F("Your browser does not support the HTML5 canvas tag.</canvas>");
//
//  client.println F("<script>");
//  client.println F("var c = document.getElementById(\"myCanvas\");");
//  client.println F("var ctx = c.getContext(\"2d\");");
//
//  if (iIndex >= max_graph_data) iIndex = 0;
//  for (int i = 0; i < max_graph_data - 2; i++)
//  {
//    if (i < iIndex - 1 || i > iIndex + 1)
//    {
//      client.print F("ctx.moveTo(");
//      client.print (i * 20);
//      client.print F(",");
//      client.print (myGraphData[i] / 2);
//      client.println F(");");
//      client.print F("ctx.lineTo(");
//      client.print ((i + 1) * 20);
//      client.print F(",");
//      client.print(myGraphData[i + 1] / 2);
//      client.println F(");");
//      client.println F("ctx.strokeStyle=\"blue\";");
//      client.println F("ctx.stroke();");
//    }
//  }
//  //draw stimulus...
//  client.print F("ctx.moveTo(");
//  client.print ((max_graph_data / 10) * 20);
//  client.print F(",30);");
//
//  client.print F("ctx.lineTo(");
//  client.print (max_graph_data / 2 * 20);
//  client.print F(",30);");
//
//  client.println F("ctx.strokeStyle=\"blue\";");
//  //              client.println("ctx.lineWidth=5;");
//  client.println F("ctx.stroke();");
//
//  client.println F("</script>");
//  client.println F("<BR><BR><button onclick=\"myStopFunction()\">Stop display</button>");
//
//  client.println F("To run a test please stop and then load ") ;
//
//  send_GoBack_to_Stim_page ();
//
//  sendFooter();
//
//}


void printTwoDigits(char * p, uint8_t v)
{

  *p   = '0' + v / 10;
  *(p + 1) = '0' + v % 10;
  *(p + 2) = 0;

}



void webTime ()
{
#ifdef __wifisetup__
  WiFiClient timeclient;
#else
  EthernetClient timeclient;
#endif
  // default values ...
  year = 2015;
  second = myminute = hour = day = month = 1;

  // Just choose any reasonably busy web server, the load is really low
  if (timeclient.connect ("www.york.ac.uk", 80))
  {
    // Make an HTTP 1.1 request which is missing a Host: header
    // compliant servers are required to answer with an error that includes
    // a Date: header.
    timeclient.print(F("GET / HTTP/1.1 \r\n\r\n"));

    char buf[5];			// temporary buffer for characters
    timeclient.setTimeout(5000);
    if (timeclient.find((char *)"\r\nDate: ") // look for Date: header
        && timeclient.readBytes(buf, 5) == 5) // discard
    {
      day = timeclient.parseInt();	   // day
      timeclient.readBytes(buf, 1);	   // discard
      timeclient.readBytes(buf, 3);	   // month
      year = timeclient.parseInt();	   // year
      hour = timeclient.parseInt();   // hour
      myminute = timeclient.parseInt(); // minute
      second = timeclient.parseInt(); // second


      switch (buf[0])
      {
        case 'F': month = 2 ; break; // Feb
        case 'S': month = 9; break; // Sep
        case 'O': month = 10; break; // Oct
        case 'N': month = 11; break; // Nov
        case 'D': month = 12; break; // Dec
        default:
          if (buf[0] == 'J' && buf[1] == 'a')
            month = 1;		// Jan
          else if (buf[0] == 'A' && buf[1] == 'p')
            month = 4;		// Apr
          else switch (buf[2])
            {
              case 'r': month =  3; break; // Mar
              case 'y': month = 5; break; // May
              case 'n': month = 6; break; // Jun
              case 'l': month = 7; break; // Jul
              default: // add a default label here to avoid compiler warning
              case 'g': month = 8; break; // Aug
            }
      } // months sorted
      //month -- ; // zero based, I guess

    }
  }
  delay(10);
  timeclient.flush();
  timeclient.stop();

  return ;
}



// find day of week http://stackoverflow.com/questions/6054016/c-program-to-find-day-of-week-given-date
int DayOfWeek (int d, int m, int y)
{
  return (d += m < 3 ? y-- : y - 2, 23 * m / 9 + d + 4 + y / 4 - y / 100 + y / 400) % 7   ;
}

void gmdate ( const dir_t & pFile)
{
  // Last-Modified: Tue, 15 Nov 1994 12:45:26 GMT
  const char * cDays PROGMEM = "Sun,Mon,Tue,Wed,Thu,Fri,Sat,Sun";
  const char * cMonths PROGMEM = "Jan,Feb,Mar,Apr,May,Jun,Jul,Aug,Sep,Oct,Nov,Dec,";
  char c [55] ;
  c [0] = 0;
  int iTmp ;
  int d = FAT_DAY(pFile.lastWriteDate) ;
  int m = FAT_MONTH(pFile.lastWriteDate) ;
  int y = FAT_YEAR(pFile.lastWriteDate) ;

  iTmp = DayOfWeek (d, m, y) ;
  if (iTmp > 6) iTmp = 0;
  strncpy(c, cDays + iTmp * 4, 3); // tue
  c[3] = 0;
  strcat_P(c, PSTR(", "));
  //Serial.println (c);

  printTwoDigits(c + strlen(c) , FAT_DAY(pFile.lastWriteDate));
  strcat_P (c, PSTR(" "));

  int iLen = strlen(c);
  iTmp = m - 1;
  if (iTmp > 11) iTmp = 0;
  strncpy(c + iLen, cMonths + iTmp * 4, 3); //nov
  c[iLen + 3] = 0;
  //Serial.println (c);

  strcat_P (c, PSTR(" "));
  itoa( y, c + strlen(c), 10);
  strcat_P (c, PSTR(" "));
  printTwoDigits(c + strlen(c) , FAT_HOUR(pFile.lastWriteTime));
  strcat_P (c, PSTR(":"));
  printTwoDigits(c + strlen(c) , FAT_MINUTE(pFile.lastWriteTime));
  strcat_P (c, PSTR(":"));
  printTwoDigits(c + strlen(c) , FAT_SECOND(pFile.lastWriteTime));
  strcat_P (c, PSTR(" GMT"));

  //Serial.println( c );
}

//
//void AppendFlashReport()
//{
//  client.print F("Acquired ") ;
//  client.print ( nRepeats );
//  client.print F(" of ");
//  client.print (maxRepeats);
//  client.println F(" data blocks so far " );
//  client.println F("<button onclick=\"myStopFunction()\">Stop Data Acquisition</button><BR>");
//  client.println (cInput);
//  client.println F( "<BR> ");
//
//
//  if (nRepeats > 0)
//  {
//    client.println F("<canvas id=\"myCanvas\" width=\"640\" height=\"520\" style=\"border:1px solid #d3d3d3;\">");
//    client.println F("Your browser does not support the HTML5 canvas tag.</canvas>");
//
//    client.println F("<script>");
//    client.println F("var c = document.getElementById(\"myCanvas\");");
//    client.println F("var ctx = c.getContext(\"2d\");");
//
//    for (int i = 0; i < max_data - max_data / 6; i = i + 15)
//    {
//      client.print F("ctx.moveTo(");
//      client.print((8 * i) / 10 );
//      client.print F(",");
//      client.print(350 - myGraphData[i]/4);
//      client.println F(");");
//      client.print F("ctx.lineTo(");
//      client.print((8 * (i + 15)) / 10 );
//      client.print F(",");
//      client.print(350 - myGraphData[i + 15]/4);
//      client.println F(");");
//      client.println F("ctx.stroke();");
//
//      client.print F("ctx.moveTo(");
//      client.print((8 * i) / 10 );
//      client.print F(",");
//      client.print(10 + fERG_Now(time_stamp[i] - time_stamp[0]) );
//      client.println F(");");
//      client.print F("ctx.lineTo(");
//      client.print((8 * (i + 13)) / 10 );
//      client.print F(",");
//      client.print(10 + fERG_Now(time_stamp[i + 13] - time_stamp[0]));
//      client.println F(");");
//      client.println F("ctx.stroke();");
//    }
//    client.println F("</script>");
//  }
//}

void flashLED (int time_on, int time_off, int iTimes) // ms
{
  for (int i = 0; i < iTimes; i++)
  {
    delayMicroseconds(long(time_on) * 1000L);
    goColour(intensity, 0, 0, 255, 0, 0, 0, false) ;

    delayMicroseconds(long(time_off) * 1000L);
    goColour(0, 0, 0, 255, 0, 0, 0, false) ;
  }
}

void sendReply ()
{
  int exp_size = MaxInputStr + 2 ;
  Serial.println(MyInputString);

  int fPOS = MyInputString.indexOf F("run=");
  // asking for new sample
  if (fPOS > 0)
  { 
    Serial.println (MyInputString);
    fPOS = MyInputString.indexOf F("intensity=");
    flashLED (1, 99, 1000);
    return ;
  }


  //light up
  fPOS = MyInputString.indexOf F("white/");
  if (fPOS > 0)
  {
    goColour(255, true) ;
    return ;
  }

  fPOS = MyInputString.indexOf F("amber/");
  if (fPOS > 0)
  {
    //void go4Colour(const byte r, const byte g, const byte b, const byte a, const byte w, const byte l, const byte c,  const bool boolUpdatePage)
    goColour(0, 0, 0, 255, 0, 0, 0, true) ;
    return ;
  }
  fPOS = MyInputString.indexOf F("cyan/");
  if (fPOS > 0)
  {
    //void go4Colour(const byte r, const byte g, const byte b, const byte a, const byte w, const byte l, const byte c,  const bool boolUpdatePage)
    goColour(0, 0, 0, 0, 0, 0, 255, true) ;
    return ;
  }
  fPOS = MyInputString.indexOf F("blueviolet/");
  if (fPOS > 0)
  {
    //void go4Colour(const byte r, const byte g, const byte b, const byte a, const byte w, const byte l, const byte c,  const bool boolUpdatePage)
    goColour(0, 0, 0, 0, 0, 255, 0, true) ;
    return ;
  }

  fPOS = MyInputString.indexOf F("red/");
  if (fPOS > 0)
  {
    goColour(255, 0, 0, 0, true) ;
    return ;
  }
  fPOS = MyInputString.indexOf F("blue/");
  if (fPOS > 0)
  {
    goColour(0, 0, 255, 0, true) ;
    return ;
  }
  fPOS = MyInputString.indexOf F("green/");
  if (fPOS > 0)
  {
    goColour(0, 255, 0, 0, true) ;
    return ;
  }
  fPOS = MyInputString.indexOf F("black/");
  if (fPOS > 0)
  {
    goColour(0, 0, 0, 0, true) ;
    return ;
  }
  fPOS = MyInputString.indexOf F("fiber/");
  if (fPOS > 0)
  {
    goColour(0, 0, 0, 255, true) ;
    return ;
  }


  

  // default - any other url

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

