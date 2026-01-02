# VFD WiFi Sync clock

A Wi-Fi–synchronized (NTP) clock built with an ESP32 and a VFD (Vacuum Fluorescent Display) salvaged from an old calculator.

Please take a look at the related Instractables, [https://www.instructables.com/A-Modern-Calculator-and-Clock-From-a-Broken-50-Yea/](https://www.instructables.com/A-Modern-Calculator-and-Clock-From-a-Broken-50-Yea/)

* In this project, a 6-digit VFD extracted from a broken CASIO MINI CM-602 (1973) was used.
* The remaining case, keypad, and PCB from this calculator have been reborn as an RPN calculator.
For details, please refer to this project: [RPN-calc-with-fractional-BCD](https://github.com/sh1ura/RPN-calc-with-fractional-BCD)

## Video

[![thumb](https://github.com/user-attachments/assets/9849487b-e39a-4aa6-ad1b-d3a3a4a657f1)](https://youtu.be/MaTxv_QstjQ)
Click the image above to play the video.

## Circuit

![circuit](https://github.com/user-attachments/assets/6cfd5ac1-3c4a-4d1f-ac30-27e8ac6ca0d2)

### For grids and anodes (segments)

VFDs generally require a relatively high voltage (around 25 V) to drive the grids and anode segments.
In this design, Mitsubishi Electric M54564P transistor array ICs (source-type drivers) are used to switch 25 V signals using the 3.3 V logic outputs from the ESP32.

* An M54564P can be purchased in Japan for about 50 JPY (≈ 0.3 USD) from　<a href="https://akizukidenshi.com/catalog/g/g116842/">Akizuki Denshi</a>.
* A single M54564P can drive up to 8 grids and/or anode segments.
* No external pull-up or pull-down resistors are required.

### For the cathode (heater filament)

The VFD heater filament typically requires a low voltage (around 1V).
Instead of preparing a dedicated low-voltage power supply, a series resistor is used to drive the filament from a 5V source.

1. Measure the resistance of the heater filament.
2. Start with a series resistor approximately 5× the filament resistance. For example, a 40 Ω resistor was used for an 8 Ω filament.
3. Adjust the resistance to achieve appropriate brightness. Avoid excessive current, as it may damage the filament.

### DC-DC converter

Low-cost boost DC–DC converters capable of generating 5–28V are widely available.
Search for modules based on the “MT3608” IC.

## Code

### Functions

* The project uses [https://github.com/tzapu/WiFiManager](WifiManager) to configure and store the SSID and password on the ESP32.
* The NTP server FQDN and local time zone are also configured via the WiFiManager web interface.

### Modification of the code

* List the GPIO pin numbers in the arrays named `segment[]` and `grid[]`.
* In the `segment[]` array, order the pin numbers from segment `a` to `g` of the 7-segment display, followed by the pin for the decimal point.
* In the `grid[]` array, order the pin numbers from left to right.

## Enclosure

* The VFD is intentionally left exposed to create a retrofuturistic, steampunk-inspired appearance.
