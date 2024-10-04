import pandas as pd
import serial
import time

# Function to read commands from CSV file and return as a list
def read_commands_from_csv(file_path):
    try:
        # Read the CSV file
        data = pd.read_csv(file_path, header=None)
        # Extract the first column which contains the commands
        commands = data.iloc[:, 0].tolist()
        return commands
    except Exception as e:
        print("Error reading the CSV file:")
        print(e)
        return []

# Function to send commands via USB to the Teensy
def send_commands_to_teensy(serial_port, baud_rate, commands):
    try:
        # Open serial connection to the Teensy device
        teensy = serial.Serial(serial_port, baud_rate, timeout=5)

        # Small delay to allow the Teensy to initialize properly
        time.sleep(2)

        # Loop through each command and send it to the Teensy
        for command in commands:
            print(f"Sending command: {command}")

            # Send the command to the Teensy
            teensy.write((command + '\n').encode())

            # Optionally read back a response from Teensy
            if teensy.in_waiting > 0:
                response = teensy.readline().decode().strip()
                print(f"Teensy response: {response}")

            # Pause briefly before sending the next command
            time.sleep(0.001)

        # Close the serial connection
        teensy.close()
    except Exception as e:
        print("Error communicating with the Teensy:")
        print(e)

# Main script
csv_file_path = 'path_to_your_csv.csv'  # Modify this with the correct CSV file path
serial_port = 'COM3'  # Replace with your actual COM port (Windows) or '/dev/ttyUSB0' (Linux)
baud_rate = 9600  # Set the appropriate baud rate for Teensy communication

# Continuous loop to repeatedly send CSV commands
while True:
    # Step 1: Read the commands from the CSV file
    commands = read_commands_from_csv(csv_file_path)

    # Step 2: Send the commands to the Teensy device
    if commands:
        send_commands_to_teensy(serial_port, baud_rate, commands)
    else:
        print("No commands to send.")

    # Pause between iterations to avoid overwhelming the system
    time.sleep(2)  # Adjust the pause duration as needed








