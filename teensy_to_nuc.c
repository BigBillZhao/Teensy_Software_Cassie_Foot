#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <stdint.h>

// Function to configure the serial port
int configure_serial_port(const char* serial_port_name, speed_t baud_rate) {
    int serial_port = open(serial_port_name, O_RDWR);
    if (serial_port < 0) {
        printf("Error opening serial port %s\n", serial_port_name);
        return -1;
    }

    struct termios tty;
    memset(&tty, 0, sizeof(tty));

    // Get current serial port settings
    if (tcgetattr(serial_port, &tty) != 0) {
        printf("Error reading serial port attributes\n");
        close(serial_port);
        return -1;
    }

    // Set baud rate for input and output
    cfsetispeed(&tty, baud_rate);
    cfsetospeed(&tty, baud_rate);

    // Set 8N1 mode (8 data bits, no parity, 1 stop bit)
    tty.c_cflag &= ~PARENB; // No parity bit
    tty.c_cflag &= ~CSTOPB; // Only 1 stop bit
    tty.c_c_cflag |= CS8;   // 8 data bits
    tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control
    tty.c_cflag |= CREAD | CLOCAL; // Turn on the receiver

    // Set non-canonical mode (raw input)
    tty.c_lflag &= ~ICANON;
    tty.c_lflag &= ~(ECHO | ECHOE | ISIG); // Disable echo
    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off software flow control
    tty.c_oflag &= ~OPOST; // Raw output

    // Set port attributes
    if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
        printf("Error setting serial port attributes\n");
        close(serial_port);
        return -1;
    }

    return serial_port;
}

// Function to send binary data from CSV to Teensy via serial port
void send_binary_data_to_teensy(const char* csv_file_path, const char* serial_port_name, speed_t baud_rate) {
    FILE* csv_file = fopen(csv_file_path, "r");
    if (!csv_file) {
        printf("Error opening CSV file: %s\n", csv_file_path);
        return;
    }

    // Configure the serial port
    int serial_port = configure_serial_port(serial_port_name, baud_rate);
    if (serial_port < 0) {
        fclose(csv_file);
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), csv_file)) {
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
            write(serial_port, data, sizeof(data));  // Send all 10 values as 16-bit binary data
            printf("Sent binary data to Teensy: ");
            for (int i = 0; i < 10; i++) {
                printf("%d ", data[i]);  // Print the data being sent
            }
            printf("\n");

            // Wait briefly before sending the next row
            usleep(1000000); // 1 second delay (adjust as needed)
        }
        else {
            printf("Invalid row in CSV, skipping...\n");
        }
    }

    // Close the CSV file and serial port
    fclose(csv_file);
    close(serial_port);
}

// Main function: Accepts command-line arguments for CSV file and serial port
int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: %s <csv_file_path> <serial_port_name>\n", argv[0]);
        return 1;
    }

    const char* csv_file_path = argv[1];   // First argument: CSV file path
    const char* serial_port_name = argv[2]; // Second argument: Serial port (e.g., /dev/ttyUSB0 or COM3)
    speed_t baud_rate = B9600; // Set baud rate (must match the Teensy settings)

    // Call function to send binary data
    send_binary_data_to_teensy(csv_file_path, serial_port_name, baud_rate);

    return 0;
}
