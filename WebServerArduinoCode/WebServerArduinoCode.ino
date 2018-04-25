#include <SPI.h>
#include <Ethernet2.h>
#include <SD.h>

// MAC address from Ethernet shield sticker under board
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 69); // IP address, may need to change depending on network
EthernetServer server(80);  // create a server at port 80

String input; //input from the serial port for now 

File myFile;
const String filename = "index.txt";

// constants won't change. They're used here to set pin numbers:
const int buttonPin = 2;     // the number of the pushbutton pin
// variables will change:
int buttonState = 0;         // variable for reading the pushbutton status
const int ledPin =  4;      // the number of the LED pin

int openOrClosed = 0;

void setup()
{
    // initialize the pushbutton pin as an input:
    pinMode(buttonPin, INPUT);
    // initialize the LED pin as an output:
    pinMode(ledPin, OUTPUT);

    //Ethernet Shield Code
    Ethernet.begin(mac, ip);  // initialize Ethernet device
    server.begin();           // start to listen for clients
    Serial.begin(9600);       // for debugging
    
    // initialize SD card
    Serial.println("Initializing SD card...");
    if (!SD.begin(4)) {
        Serial.println("ERROR - SD card initialization failed!");
        return;    // init failed
    }
    Serial.println("SUCCESS - SD card initialized.");
    // check for index.html file
    /*
    if (!SD.exists("index.html")) {
        Serial.println("ERROR - Can't find index.htm file!");
        return;  // can't find index file
    }
    Serial.println("SUCCESS - Found index.html file.");
    */
}

void loop()
{
   //Wait for user Input (button Pressed)
  buttonstate();
  sendFile();
}

//Button for user input
void buttonstate(){
  // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);
  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonState==HIGH && openOrClosed==0) {
     buttonState= LOW;
    int buttonState = 0;         // variable for reading the pushbutton status
    // turn LED on:
    digitalWrite(ledPin, HIGH);
    Serial.println("door is currently closed");
    Serial.println("writing open to sd card");
    writeOpenToSD();
    //sendFile();
    openOrClosed=1;
  }if(buttonState==HIGH && openOrClosed==1){
    buttonState= LOW;         // variable for reading the pushbutton status
    // turn LED off:
    digitalWrite(ledPin, LOW);
    Serial.println("door is currently open");
    Serial.println("writing close to sd card");
    writeCloseToSD();
    //sendFile();
    openOrClosed=0;
  } 
}

void writeOpenToSD(){
  removeIndexFromSD();
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  //delay(5000);
  myFile = SD.open(filename, FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing open to file");
    myFile.println("*");
    // close the file:
    myFile.close();
    Serial.println("done writing open to file");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
  delay(5000);
}

void writeCloseToSD(){
  removeIndexFromSD();
   // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  myFile = SD.open(filename, FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing close to file");
    myFile.println("close");
    // close the file:
    myFile.close();
    Serial.println("done writing close to file");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
  delay(5000);
}

void removeIndexFromSD(){
  SD.remove(filename);
}

void sendFile(){
  EthernetClient client = server.available();  // try to get client
  if (client) {  // got client?
        boolean currentLineIsBlank = true;
        while (client.connected()) {
            if (client.available()) {   // client data available to read
                char c = client.read(); // read 1 byte (character) from client
                // last line of client request is blank and ends with \n
                // respond to client only after last line received
                if (c == '\n' && currentLineIsBlank) {
                    // send a standard http response header
                    client.println("HTTP/1.1 200 OK");
                    client.println("Content-Type: text/html");
                    client.println("Connection: close");
                    client.println();
                                      
                    // send web page
                    myFile = SD.open("index.txt");        // open web page file
                    if (myFile) {
                        while(myFile.available()) {
                            client.write(myFile.read()); // send web page to client
                        }
                        myFile.close();
                    }//
                    break;
                    
                }
                // every line of text received from the client ends with \r\n
                if (c == '\n') {
                    // last character on line of received text
                    // starting new line with next character read
                    currentLineIsBlank = true;
                } 
                else if (c != '\r') {
                    // a text character was received from client
                    currentLineIsBlank = false;
                }
            } // end if (client.available())
        } // end while (client.connected())
        delay(1);      // give the web browser time to receive the data
        client.stop(); // close the connection
    } // end if (client)
}
/*
void printClose(){
  EthernetClient client = server.available();  // try to get client
  if (client) {  // got client?
        boolean currentLineIsBlank = true;
        while (client.connected()) {
            if (client.available()) {   // client data available to read
                char c = client.read(); // read 1 byte (character) from client
                // last line of client request is blank and ends with \n
                // respond to client only after last line received
                if (c == '\n' && currentLineIsBlank) {
                    // send a standard http response header
                    client.println("HTTP/1.1 200 OK");
                    client.println("Content-Type: text/html");
                    client.println("Connection: close");
                    client.println();
                    client.write("c");
                    /* send web page
                    webFile = SD.open("close.html");        // open web page file
                    if (webFile) {
                        while(webFile.available()) {
                            client.write(webFile.read()); // send web page to client
                        }
                        webFile.close();
                    }
                    break;
                }
                // every line of text received from the client ends with \r\n
                if (c == '\n') {
                    // last character on line of received text
                    // starting new line with next character read
                    currentLineIsBlank = true;
                } 
                else if (c != '\r') {
                    // a text character was received from client
                    currentLineIsBlank = false;
                }
            } // end if (client.available())
        } // end while (client.connected())
        delay(1);      // give the web browser time to receive the data
        client.stop(); // close the connection
    } // end if (client)
}
*/

