# GeophoneReading

This code is designed to efficiently capture analog signals from a Geophone sensor using the ADS1256
ADC with the Adafruit ESP32 Feather microcontroller.

To run this code seamlessly, ensure you have the PlatformIO extension installed on either Visual 
Studio Code or CLion.

### About the Code

The `geophoneData` struct encapsulates essential data points:
- `Node_ID` (a numerical identifier for the Geophone station)
- `checksum` (for error detection and correction)
- `status_code` (indicating the status of ADS1256)
- `data` (comprising Geophone x, y, and z-axis readings)

This struct is transmitted via the MQTT protocol. Initially, it's serialized before being 
transmitted. On the receiver end, the data is parsed using `plot.py`. This script not only 
parses the received struct but also facilitates live data plotting through the Matplotlib library.