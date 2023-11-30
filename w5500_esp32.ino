/*
  Esp connections:
    extxa rst -> 26
*/

#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include "local_config.h"  // <--- Change settings for YOUR network here.

const int relayPin1 = 13;
const int relayPin2 = 4;
const int relayPin3 = 2;
const int relayPin4 = 15;



const char* password = "12345678";
bool passwordEntered = false;

const int NTP_PACKET_SIZE = 48;
byte packetBuffer[NTP_PACKET_SIZE];
EthernetUDP Udp;
char  ReplyBuffer[] = "acknowledged";       // a string to send back

void WizReset() {
  Serial.print("Resetting Wiz W5500 Ethernet Board...  ");
  pinMode(RESET_P, OUTPUT);
  digitalWrite(RESET_P, HIGH);
  delay(250);
  digitalWrite(RESET_P, LOW);
  delay(50);
  digitalWrite(RESET_P, HIGH);
  delay(350);
  Serial.println("Done.");
}

void prt_hwval(uint8_t refval) {
  switch (refval) {
    case 0:
      Serial.println("No hardware detected.");
      break;
    case 1:
      Serial.println("WizNet W5100 detected.");
      break;
    case 2:
      Serial.println("WizNet W5200 detected.");
      break;
    case 3:
      Serial.println("WizNet W5500 detected.");
      break;
    default:
      Serial.println("UNKNOWN - Update espnow_gw.ino to match Ethernet.h");
  }
}

void prt_ethval(uint8_t refval) {
  switch (refval) {
    case 0:
      Serial.println("Unknown status.");
      break;
    case 1:
      Serial.println("Link flagged as UP.");
      break;
    case 2:
      Serial.println("Link flagged as DOWN. Check cable connection.");
      break;
    default:
      Serial.println("UNKNOWN - Update espnow_gw.ino to match Ethernet.h");
  }
}


void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\n\tUDP NTP Client v3.0\r\n");
  pinMode(relayPin1, OUTPUT);
  pinMode(relayPin2, OUTPUT);
  pinMode(relayPin3, OUTPUT);
  pinMode(relayPin4, OUTPUT);

  Ethernet.init(5);  // GPIO5 on the ESP32.
  WizReset();

  Serial.println("Starting ETHERNET connection...");
  Ethernet.begin(eth_MAC, eth_IP, eth_DNS, eth_GW, eth_MASK);
  delay(200);

  Serial.print("Ethernet IP is: ");
  Serial.println(Ethernet.localIP());

  Serial.print("Checking connection.");
  bool rdy_flag = false;
  for (uint8_t i = 0; i <= 20; i++) {
    if ((Ethernet.hardwareStatus() == EthernetNoHardware) || (Ethernet.linkStatus() == LinkOFF)) {
      Serial.print(".");
      rdy_flag = false;
      delay(80);
    } else {
      rdy_flag = true;
      break;
    }
  }
  if (rdy_flag == false) {
    Serial.println("\n\r\tHardware fault, or cable problem... cannot continue.");
    Serial.print("Hardware Status: ");
    prt_hwval(Ethernet.hardwareStatus());
    Serial.print("   Cable Status: ");
    prt_ethval(Ethernet.linkStatus());
    while (true) {
      delay(10);
    }
  } else {
    Serial.println(" OK");
  }

  Udp.begin(localPort);
}

void loop() {
  
  if (!passwordEntered) {
    waitForPassword();
  } else {
    int packetSize = Udp.parsePacket();
    if (packetSize > 0) {
      char incomingPacket[255];
      Udp.read(incomingPacket, sizeof(incomingPacket));
      incomingPacket[packetSize] = '\0';
      handleCommand(incomingPacket);
    }
    feedback();
  }

  



  delay(100);
}

void waitForPassword() {
  Serial.println("Enter the password:");
  char receivedPassword[255];

  while (Serial.available() == 0) {
    delay(50);
  }

  int passwordLength = Serial.readBytesUntil('\n', receivedPassword, sizeof(receivedPassword));
  if (passwordLength > 0 && receivedPassword[passwordLength - 1] == '\r') {
    receivedPassword[passwordLength - 1] = '\0';
  } else {
    receivedPassword[passwordLength] = '\0';
  }

  Serial.print("Received password: ");
  Serial.println(receivedPassword);

  if (strcmp(receivedPassword, password) == 0) {
    passwordEntered = true;
    Serial.println("Password accepted. LED control enabled.");
  } else {
    Serial.println("Incorrect password. Try again.");
  }
}

void handleCommand(const char* command) {
  if (strcmp(command, "relay1on") == 0) {
    digitalWrite(relayPin1, HIGH);
    Serial.println("Relay1 ON");
  }
  else if (strcmp(command, "relay1off") == 0) {
    digitalWrite(relayPin1, LOW);
    Serial.println("Relay1 OFF");
  }
  else if (strcmp(command, "relay2on") == 0) {
    digitalWrite(relayPin2, HIGH);
    Serial.println("Relay2 ON");
  }
  else if (strcmp(command, "relay2off") == 0) {
    digitalWrite(relayPin2, LOW);
    Serial.println("Relay2 OFF");
  }
  else if (strcmp(command, "relay3on") == 0) {
    digitalWrite(relayPin3, HIGH);
    Serial.println("Relay3 ON");

  }
  else if (strcmp(command, "relay3off") == 0) {
    digitalWrite(relayPin3, LOW);
    Serial.println("Relay3 OFF");
  }
  else if (strcmp(command, "relay4on") == 0) {
    digitalWrite(relayPin4, HIGH);
    Serial.println("Relay4 ON");
  }
  else if (strcmp(command, "relay4off") == 0) {
    digitalWrite(relayPin4, LOW);
    Serial.println("Relay4 OFF");
  }
  else {
    Serial.println("Unknown command");
  }

}


}
