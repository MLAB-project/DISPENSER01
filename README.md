# DISPENSER01 - Smart, open-source solder paste dispenser

![PXL\_20240507\_160355764](https://github.com/MLAB-project/DISPENSER01/assets/5196729/c065def6-bded-49b3-9272-79abaaee53b1)

## Introduction

DISPENSER01 is an open-source project for precise dispensing of solder paste, flux, adhesives, and other liquid materials.
The device is based on the [AstroMeters AMFOC01 hardware](https://www.astrometers.eu/products/AMFOC01/) and is designed to make the dispensing process faster, cleaner, and more repeatable.

## Development Story

For a long time, I was searching for suitable hardware for a liquid dispenser using standard cartridges—mainly for solder paste, flux, and adhesives. On the internet, you can typically find either expensive Chinese boxes or mechanical dispensers using gear motors. When I faced this problem myself, I found neither solution to be ideal.

The quickest and, as it turned out, most practical approach was to adapt the firmware of an open-source focuser (the [AMFOC01](https://www.astrometers.eu/products/AMFOC01/)), which already included a microcontroller, display, and motor driver—perfect for controlling a solenoid valve as well.

The entire system is designed so that two cartridges can be connected at once, allowing for simultaneous operation and switching between them using a single button (note: some hardware must be duplicated for dual operation). Despite its simplicity, the dispenser has proven to be highly functional and useful.

We use DISPENSER01 ourselves, for example, during the production of MLAB modules, where it helps us achieve clean and precise application of solder paste or flux.

## Features

* **Versatile dispensing:** Precise and repeatable dispensing of solder paste, flux, adhesives, and other materials.
* **User-friendly configuration:** All main parameters—such as extrusion time, delay between extrusions, or number of repetitions—are set using four intuitive buttons on the enclosure.
* **Easy operation:** Dosing is started with a foot pedal, allowing hands-free control.
* **Flexible output:** Select which extruder (1 or 2) is active directly on the device.
* **Internal configuration storage:** Easily transfer and modify settings via USB-C.
* **OLED display:** Easy and intuitive operation via the onboard display.
* **Dual cartridge support:** Connect two cartridges for simultaneous or alternating operation.

## Bill of Materials (BOM)

To build your own DISPENSER01, you will need:

* **AstroMeters AMFOC01**
* **12V power supply**
* **Solenoid pneumatic valve**
* **Compressor with pressure regulation** (We used a small compressor for brush-art, which is sufficient)
* **Pneumatic tubing and adapter for cartridges**
* **Mechanical foot pedal or external button** (Connects via RJ45 connector)

## Firmware & Documentation

The firmware for DISPENSER01 is included in this repository and is fully open-source.
It is programmed using the Arduino IDE, making it easy to modify or extend for your own needs.

The hardware schematic for the [AstroMeters AMFOC01](https://www.astrometers.eu/products/AMFOC01/) board is also open-source and available on [GitHub](https://github.com/AstroMeters/AMFOC01).

## Usage

DISPENSER01 is designed for maximum user comfort.
All key parameters—including extrusion time, gap between extrusions, and the number of repeats—can be set directly using the four buttons on the front panel.
Material dispensing is controlled by a mechanical foot pedal, enabling hands-free and precise operation.

The buttons also allow you to select whether you want to use extruder 1 or extruder 2.

## Availability

DISPENSER01 is not currently available in any e-shop.

However, if you are interested, you have two options:

* **Purchase the [AMFOC01 hardware](https://www.astrometers.eu/products/AMFOC01/)** and build the dispenser yourself using our open-source resources (see Bill of Materials above).
* Or **contact us**, and we can discuss providing you with a fully assembled DISPENSER01 unit.

## Questions?

Feel free to contact us, ideally through the [MLAB-project discussions](https://github.com/orgs/MLAB-project/discussions).
