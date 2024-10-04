#include <SD.h>
#include <SPI.h>

// SD card chip select pin (adjust based on your board)
const int chipSelect = 4;
const unsigned long baud_rate = 9600;

// Function to initialize the SD card
bool initializeSD() {
    if (!SD.begin(chipSelect)) {
        Serial.println("Error initializing SD card.");
        return false;
    }
    return true;
}

// Function to send binary data from CSV to Teensy via serial port
void send_binary_data_to_teensy(const char* csv_file_path) {
    File csv_file = SD.open(csv_file_path);
    if (!csv_file) {
        Serial.println("Error opening CSV file");
        return;
    }

    char line[256];
    while (csv_file.available()) {
        int len = csv_file.readBytesUntil('\n', line, sizeof(line) - 1);
        line[len] = '\0';  // Null-terminate the line

        // Parse CSV line into an array of integers (assume each line has 10 values)
        int16_t data[10];
        char* token = strtok(line, ",");
        int index = 0;
        while (token != NULL && index < 10) {
            data[index] = (int16_t)atoi(token); // Convert token to integer and store in array
            token = strtok(NULL, ",");
            index++;
        }

        // Ensure we have 10 valid data points before sending
        if (index == 10) {
            // Send the binary data to the Teensy
            Serial.write((uint8_t*)data, sizeof(data));  // Send all 10 values as 16-bit binary data
            Serial.print("Sent binary data to Teensy: ");
            for (int i = 0; i < 10; i++) {
                Serial.print(data[i]);
                Serial.print(" ");
            }
            Serial.println();

            // Wait briefly before sending the next row
            delay(1000); // 1 second delay (adjust as needed)
        }
        else {
            Serial.println("Invalid row in CSV, skipping...");
        }
    }

    // Close the CSV file
    csv_file.close();
}

void setup() {
    // Start serial communication
    Serial.begin(baud_rate);

    // Initialize SD card
    if (!initializeSD()) {
        while (1); // Halt if SD card initialization fails
    }
}

void loop() {
    // Send binary data from the CSV file to the Teensy continuously
    send_binary_data_to_teensy("data.csv");

    // Restart the transmission after finishing the CSV
    Serial.println("Finished sending CSV data, restarting...");
    delay(1000); // Optional delay before starting over
}

