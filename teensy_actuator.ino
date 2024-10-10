
#define start_byte1 0x55
#define start_byte2 0xAA
#define response_byte1 0xAA
#define response_byte2 0x55
#define actuator_ID 0x01
#define inst_type 0x32

#define ACTUATOR Serial5


//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////


uint8_t calculateChecksum(uint8_t *data, int length) {
    uint16_t sum = 0;  // Use a larger type to avoid overflow
    for (int i = 2; i < length; i++) {
        sum += data[i];
      }  
    return static_cast<uint8_t>(sum & 0xFF);  // Return only the least significant byte
}

void sendCommand(uint8_t *cmnd) {

    uint8_t cmndLength = sizeof(cmnd)+1 / sizeof(cmnd[0]);
    
    uint8_t package[4+cmndLength] = {start_byte1,
                                     start_byte2,
                                     cmndLength,
                                     actuator_ID,
                                     inst_type};

    for (int i = 0; i < cmndLength; i++) {
      package[5+i] = cmnd[i];
    }                 

    int packageLength = sizeof(package) / sizeof(package[0]);
    Serial.println(packageLength);
    uint8_t checksum = calculateChecksum(package, packageLength);
    Serial.println(checksum,HEX);
    ACTUATOR.write(package, packageLength);
    ACTUATOR.write(checksum); // Send checksum as the last byte
    // readResponse();
}


// Function to read incoming data from the actuator
void readResponse() {

    uint8_t response[20];
    size_t bytesRead = 0;
    int length = 20;
    while (ACTUATOR.available() > 0) {
      uint8_t incomingByte = ACTUATOR.read();  // Read one byte from serial

      // Add the byte to the array if there's space
      if (bytesRead < length - 1) {
        response[bytesRead] = incomingByte;  // Store the byte in the array
        bytesRead++;                          // Increment the index
    } 
  }

  // Checks if the first and second byte of the response frame are valid
  handleResponse(response, length);
}

void handleResponse(byte* response, int length) {
      // Validate and process the response frame here
      // if (response[0] != response_byte1 || response[1] != response_byte2) {
      //     Serial.println("Invalid response frame");
      //     return;
      // }

      // Process the response frame
      uint16_t pos = (response[8] << 8) | response[9];

      // Further processing...
      Serial.print("Received position: ");
      Serial.println(pos, DEC); // Print the response ID in hexadecimal
  }


//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////


void setup() {
  
  Serial.begin(9600);
  while (!Serial);

  // Initializes serial communication with linear actuator
  ACTUATOR.begin(921600);
  
  // Fault clearance instruction frame
  // Header (2B) + Data length (1B) + ID (1B) + Instruction Type (1B) + register address (2B) + data (NB)
  uint8_t faultclr[] = {0x55, 0xAA, 0x05, 0x01, 
                        0x32, 0x18, 0x00, 0x01,
                        0x00, 0x51};

  //  Fault clearance for linear actuator
  // if (ACTUATOR.availableForWrite() > 0) {

     Serial.println("Fault clearance");
     ACTUATOR.write(faultclr,10);
    
     delay(100);
  // }

  // Reads fault clearance response frame
  readResponse();

  // Control mode to positioning mode instruction frame
  // Header (2B) + Data length (1B) + ID (1B) + Instruction Type (1B) + register address (2B) + data (NB)
  uint8_t ctrlmode[] = {0x55, 0xAA, 0x05, 0x01, 
                        0x32, 0x25, 0x00, 0x00,
                        0x00, 0x5D};

  // sets the control mode as the positioning mode
  if (ACTUATOR.availableForWrite() > 0) {

    Serial.println("Positioning mode begun...");
    ACTUATOR.write(ctrlmode,10);
     
    delay(100);
  }
  delay(1000);
  // Reads control mode change response frame
  readResponse();
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////


void loop() {
  
  // Sets position to 0 steps 
  uint8_t retract[] = {0x29, 0x00, 0x00, 0x00};

  // sets the target position and modifies the register 0x29 to 1000 steps (halfway extended)
    
  sendCommand(retract);
  //  delay(10);
  
  delay(2000);

  readResponse();
  delay(500);

  // // Sets position to 1000 steps 
  uint8_t halfway[] = {0x29, 0x00, 0xE8, 0x03};

  // // sets the target position and modifies the register 0x29 to 1000 steps (halfway extended)
  sendCommand(halfway);
  delay(2000);
  // 

  readResponse();
  delay(500);

  // Sets position to 2000 steps 
  uint8_t fullextend[] = {0x29, 0x00, 0xD0, 0x07};

  // sets the target position and modifies the register 0x29 to 1000 steps (halfway extended)
  sendCommand(fullextend);

  delay(2000);

  readResponse();
  delay(500);

  uint8_t halfway2[] = {0x29, 0x00, 0xE8, 0x03};

  // // sets the target position and modifies the register 0x29 to 1000 steps (halfway extended)
  sendCommand(halfway2);
  
  delay(2000);

  readResponse();
  delay(500);
}

// if (ACTUATOR.availableForWrite() > 0) {
  

//   }

    // ACTUATOR.write(0x55);
    // ACTUATOR.write(0xAA);
    // ACTUATOR.write(0x05);
    // ACTUATOR.write(0x01);
    // ACTUATOR.write(0x32);
    // ACTUATOR.write(0x29);
    // ACTUATOR.write(0x00);
    // ACTUATOR.write(0xD0);
    // ACTUATOR.write(0x07);
    // ACTUATOR.write(0x38);


