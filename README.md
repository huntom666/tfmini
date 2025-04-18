# ESPHome Custom Component for Benewake TFmini Plus LiDAR

This guide explains how to set up a Benewake TFmini Plus LiDAR sensor with an ESP32 using ESPHome and the provided custom component files.

## Overview

This project uses a custom ESPHome component to read distance data from a TFmini Plus sensor via UART. The configuration allows for connection to multiple WiFi networks and integrates with Home Assistant via the native API.

## Hardware Requirements

* **ESP32 Development Board:** Any board supported by ESPHome (e.g., ESP32-DevKitC, NodeMCU-32S). The example YAML uses `esp32dev`.
* **Benewake TFmini Plus LiDAR Sensor:** Ensure it's the "Plus" model if using the exact configuration.
* **Connecting Wires:** Jumper wires (Dupont cables) or soldered connections.
* **Power Supply:** Adequate power for the ESP32 board (usually via USB). The TFmini Plus requires 5V.

## Wiring Instructions (UART)

Connect the TFmini Plus sensor to your ESP32 board as follows. **Important:** Double-check the pin numbers on your specific ESP32 board. This guide uses **GPIO16** and **GPIO17** as configured in the example `tfmini-lidar.yaml`.

| TFmini Plus Pin | ESP32 Pin       | Function        |
| :-------------- | :-------------- | :-------------- |
| 5V (Red wire)   | 5V / VIN        | Power (+)       |
| GND (Black wire)| GND             | Ground (-)      |
| RX (Green wire) | **GPIO17** (TX) | ESP32 Transmit  |
| TX (White wire) | **GPIO16** (RX) | ESP32 Receive   |

**Note:** The sensor's TX (Transmit) connects to the ESP32's RX (Receive) pin, and the sensor's RX connects to the ESP32's TX pin.

![image](https://github.com/user-attachments/assets/b3cb743a-bdb5-4649-88eb-404298a7da35)


## Software Setup

### 1. ESPHome Installation

If you haven't already, install ESPHome. Follow the official instructions: [Getting Started with ESPHome](https://esphome.io/guides/getting_started.html)

### 2. Configuration Directory Structure

You'll need a directory to hold your ESPHome configuration files. Inside this directory, create the following structure:

![image](https://github.com/user-attachments/assets/3e979e57-51b5-491c-93dd-dac68a47dd94)

### 3. Place Custom Component Files

Download or copy the custom component files (`__init__.py`, `sensor.py`, `tfmini.h`, `tfmini.cpp`) into the `my_components/tfmini/` directory you created above. (These files should be available in the GitHub repository where this README resides).

### 4. Create `tfmini-lidar.yaml`

Create the main configuration file named `tfmini-lidar.yaml` (or your preferred name) in your main configuration directory. Copy the following content into it:

```yaml
# tfmini-lidar.yaml

# ==========================================================
# ESPHome Configuration for Benewake TFmini Plus LiDAR
# Using Local Custom Component
# ==========================================================

esphome:
  name: tfmini-lidar      # Choose a name for your device
  friendly_name: tfmini_lidar # Optional: A user-friendly name
  board: esp32dev       # IMPORTANT: Replace with your specific board type

# Define the ESP32 board details (Top Level)
esp32:
  board: esp32dev
  framework:
    type: arduino

# Load the Local Custom Component (Top Level)
# Points to the 'my_components' directory relative to this YAML file
external_components:
  - source:
      type: local
      path: my_components
    # components: [ tfmini ] # Optional: Explicitly list components

# Network Configuration (Top Level)
# Supports multiple WiFi networks
wifi:
  networks: # List networks under this key
    # Network 1 (e.g., your primary location)
    - ssid: !secret wifi_ssid
      password: !secret wifi_password
      # Optional: Give this network higher priority
      # priority: 10.0

    # Network 2 (e.g., the sensor's installation location)
    - ssid: !secret wifi_ssid_location2 # Use a new secret name for the second SSID
      password: !secret wifi_password_location2 # Use a new secret name for the second password
      # Optional: Give this network lower priority if desired
      # priority: 5.0

  # Optional: Fallback Access Point (if neither network connects)
  ap:
    ssid: "Tfmini-Lidar Fallback Hotspot"
    password: "YOUR_FALLBACK_PASSWORD" # Use a secure password or !secret

# Enable Home Assistant API (Top Level)
api:
  encryption:
    key: !secret api_encryption_key # Use secrets for keys
  # password: !secret api_password # Optional: Set API password via secrets

# Enable Over-The-Air updates (Top Level)
ota:
  platform: esphome
  password: !secret ota_password # Use secrets for passwords

# Logger Configuration (Top Level)
logger:
  # Set level to DEBUG initially to see detailed sensor messages
  baud_rate: 0 # Keep serial logging disabled to avoid UART conflict
  level: DEBUG # Change to INFO once confirmed working

# Web Server for Logs/Status over WiFi (Top Level)
web_server:
  port: 80 # Default web server port

# UART Interface Configuration (Top Level)
# Using pins GPIO16 (RX) and GPIO17 (TX) based on common examples
uart:
  id: tfmini_uart           # ID used to link the sensor below
  tx_pin: GPIO17            # ESP32 TX pin (Connects to Sensor RX)
  rx_pin: GPIO16            # ESP32 RX pin (Connects to Sensor TX)
  baud_rate: 115200         # Default for TFmini Plus

# Sensor Configuration (Top Level)
sensor:
  - platform: tfmini          # Use the platform name defined by your custom component
    uart_id: tfmini_uart      # Link to the UART interface defined above
    name: "TFmini Plus Distance" # Name in Home Assistant
    update_interval: 1m       # How often to request data

    # Optional settings
    # unit_of_measurement: m
    # accuracy_decimals: 2
    # device_class: distance
    # state_class: measurement

    # Optional Filters
    # filters:
    #   - filter_out: nan
    #   - sliding_window_moving_average:
    #       window_size: 5
    #       send_every: 1
Important: Modify the board: type under esphome: and esp32: if you are not using a generic esp32dev board.5. Create secrets.yamlCreate a file named secrets.yaml in the same directory as tfmini-lidar.yaml. Do NOT commit secrets.yaml to GitHub. Add your sensitive information here:# secrets.yaml

# WiFi Network 1 Credentials
wifi_ssid: 'Your_Primary_SSID'
wifi_password: 'Your_Primary_Password'

# WiFi Network 2 Credentials
wifi_ssid_location2: 'SSID_at_Sensor_Location'
wifi_password_location2: 'Password_at_Sensor_Location'

# API Encryption Key (Generate a secure key, e.g., using openssl rand -base64 32)
api_encryption_key: 'YOUR_SECURE_API_ENCRYPTION_KEY_HERE'

# OTA Update Password
ota_password: 'YOUR_SECURE_OTA_PASSWORD_HERE'

# Optional API Password (if uncommented in main YAML)
# api_password: 'YOUR_SECURE_API_PASSWORD_HERE'

# Optional Fallback AP Password (if using !secret in main YAML)
# fallback_ap_password: 'YOUR_SECURE_FALLBACK_PASSWORD'
Replace the placeholder values with your actual network details and secure passwords/keys.Compilation and UploadUsing ESPHome Dashboard (Recommended): Add your device configuration via the dashboard. It will prompt you to install.Using ESPHome CLI: Navigate to your configuration directory in the terminal and run:esphome run tfmini-lidar.yaml
Follow the prompts to compile and upload wirelessly (OTA) or via USB.VerificationCheck Logs: After uploading, view the device logs (via ESPHome Dashboard "Logs" button, the Web UI http://<device_ip>, or esphome logs tfmini-lidar.yaml CLI command).

With level: DEBUG, you should see messages like [D][tfmini.sensor:...] TFmini Distance: ... appearing at the specified update_interval.Check Home Assistant:

If using the API integration, the sensor.tfmini_plus_distance entity should appear in Home Assistant and show distance readings.Check Web UI:

Navigate to the device's IP address in a browser to see the sensor state.Once you confirm it's working reliably, you can change the logger level back to INFO in tfmini-lidar.yaml to reduce log spam.CustomizationPins:

Change tx_pin and rx_pin under uart: if you use different GPIOs.Update Interval: Modify update_interval under sensor: to change polling frequency (e.g., 10s, 500ms).Board Type: Update board: under esphome: and esp32: to match your specific
