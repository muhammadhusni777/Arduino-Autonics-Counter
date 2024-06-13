#include <ModbusMaster.h>
#include <SoftwareSerial.h>

const byte rxPin = 5;
const byte txPin = 6;

// Set up a new SoftwareSerial object
SoftwareSerial mySerial(rxPin, txPin);

#define MAX485_DE 4
#define MAX485_RE_NEG 7

// DI => TX
// RO => RX

uint16_t index0;
uint16_t index1;

int32_t payout;

ModbusMaster node;

void preTransmission() {
  digitalWrite(MAX485_RE_NEG, HIGH);
  digitalWrite(MAX485_DE, HIGH);
}

void postTransmission() {
  digitalWrite(MAX485_RE_NEG, LOW);
  digitalWrite(MAX485_DE, LOW);
}

void setup() {
  mySerial.begin(9600);

  pinMode(MAX485_RE_NEG, OUTPUT);
  pinMode(MAX485_DE, OUTPUT);

  // Init in receive mode
  digitalWrite(MAX485_RE_NEG, LOW);
  digitalWrite(MAX485_DE, LOW);

  Serial.begin(9600);
  delay(10);
  Serial.println("Starting Arduino");
  Serial.println("Setting up Serial");
  Serial.println("Setting up RS485 port");

  // Slave id
  node.begin(1, mySerial);
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);
}

void loop() {
  int result;
  uint16_t data[10]; // Array to store the read data

  // Read input registers starting from address 1001, read 3 registers
  result = node.readInputRegisters(1003, 3);

  // Check if the read operation was successful
  if (result == node.ku8MBSuccess) {
    // Print each register value
    for (int i = 0; i < 2; i++) {
      data[i] = node.getResponseBuffer(i); // Get the value of each register

      if (i == 0) {
        index0 = static_cast<uint16_t>(data[i]);
      }

      if (i == 1) {
        index1 = static_cast<uint16_t>(data[i]);
      }
    }

    // Calculate payout based on index0 and index1
    // Convert to int32_t to allow negative values
    if(index1 == 65535){
    payout = static_cast<int32_t>(index0) - static_cast<int32_t>(index1) - 1;
    } else {
     payout = static_cast<int32_t>(index0) - static_cast<int32_t>(index1); 
    }
    // Print the results
    Serial.print(index1);
    Serial.print("  ");
    Serial.print(index0);
    Serial.print("  ");
    Serial.println(payout);
  } 

  delay(1000); // Wait for 1 second before next loop iteration
}
