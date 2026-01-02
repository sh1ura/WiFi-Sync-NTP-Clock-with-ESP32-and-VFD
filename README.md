# VFD WiFi Sync clock

**A WiFi Sync (NTP) clock with ESP32 and VFD (Vacuum Fluorescent Display) extracted from old calculator. **

[![thumb](https://github.com/user-attachments/assets/9849487b-e39a-4aa6-ad1b-d3a3a4a657f1)](https://youtu.be/MaTxv_QstjQ)


## Circuit

![circuit](https://github.com/user-attachments/assets/6cfd5ac1-3c4a-4d1f-ac30-27e8ac6ca0d2)

### for grids and anodes (segment)

Generally, VFD requires relatively high voltage (around 25V) to turn on the segments.
I use Mitsubishi M54564P transistor array (source-type driver) IC to switch 25V by 3.3V logic level output from ESP32.
In this case, any pullup / pulldown resistors are required.

### for the cathode (heater filament)

Usually low voltage (around 1V) is applied to the heater. Instead of preparing 1V power source, I used 5V and a registor.
1. Measure the resistance of the heater filament.
2. Start from a 5x resistor of the heater filament. For example, I used 40ohm for 8ohm heater filament.
3. Tune the resistance to get appropriate brightness of the segments. Do not feed too high current or it damages the filament.

## Code

* It uses [https://github.com/tzapu/WiFiManager](WifiManager) to resister the SSID and password to ESP32.
* FQDN of the NTP server and your time zone are also registered from the WifiManager interface.

## Enclosure

* I exposed VFD in the air to give retrofuturistic cyberpunk taste.
