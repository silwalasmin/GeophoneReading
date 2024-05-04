import struct
import serial
import time
import matplotlib.pyplot as plt

# Define the data structure format
data_format = 'iiifff'
# Open the serial port
ser = serial.Serial('/dev/ttyUSB0', 9600)  # Replace /dev/ttyUSB0 with the serial port where your ESP32 is connected.
starttime = time.perf_counter_ns()



def update_plot(ax, lines, x_data, y_data):
    # Update plot
    for i, line in enumerate(lines):
        line.set_xdata(x_data)  # Update x-data with the current length of x_data
        line.set_ydata(y_data[i])  # Update y-data

    # Adjust x-axis limits to move with the data
    ax.set_xlim(0, max(x_data) + 1)  # Set x-axis limit to the maximum length of x_data plus 1 for padding
    ax.relim()
    ax.autoscale_view()
    plt.draw()
    plt.pause(0.01)  # Pause for a short duration to allow the plot to update

# Initialize plot
plt.ion()  # Turn on interactive mode
fig, ax = plt.subplots()
lines = [ax.plot([], [], '-', label=f'Data {i}')[0] for i in range(3)]  # Continuous format
ax.set_xlim(0, 10)  # Adjust according to your data
ax.set_ylim(0, 1)   # Assuming the range for your data
ax.set_xlabel('Time')
ax.set_ylabel('Value')
ax.set_title('3 Axis Live Plot')
ax.legend()

# Start updating plot
x_data = []  # Initialize empty list for x_data
y_data = [[] for _ in range(3)]



while True:
    starttime = time.perf_counter_ns()
    # Read the serialized data from Arduino
    sof = ser.read(1)
    if sof != b'\x7E':
        continue
    packet_length = ord(ser.read(1))
    if packet_length != 26:
        continue
    serialized_data = ser.read(packet_length)

    serialized_data = ser.read(struct.calcsize(data_format))

    # Unpack the data
    unpacked_data = struct.unpack(data_format, serialized_data)
    # Display the unpacked data
    print("Node ID:", unpacked_data[0])
    print("Checksum", unpacked_data[1])
    print("Status Code:", unpacked_data[2])
    print("ADC values:", unpacked_data[3:])

    # Get new data points (simulated here)
    new_data = [unpacked_data[i] for i in range(3, 6)]  # Assuming unpacked_data[3], [4], [5] represent the serial data

    # Update x_data with time steps
    x_data.append(len(x_data))  # Append the length of x_data

    # Update y_data with new data
    for i in range(3):
        y_data[i].append(new_data[i])

    # Update plot
    update_plot(ax, lines, x_data, y_data)

    # Control the update rate
    time.sleep(0.01)  # Adjust the sleep duration as needed