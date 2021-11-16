<p align="center">
  <img src='images/pga309-esp32.png'>
</p>

## Contents
  * [Introduction](#introduction)
  * [Connection layout](#connection-layout)
  * [Project Directory](#project-directory)
  * [Supported Functionality](#functionality)
  * [Resources](#resources)
  * [License](#licenses)

## Introduction <a name="introduction"></a>
The PGA309 is a smart programmable analog signal conditioner used with resistive bridge sensors. It is a complete signal conditioner with bridge excitation, initial span and offset adjustment, temperature adjustment of span and offset, internal/external temperature measurement capability, output over-scale and under-scale limiting, fault detection, and digital calibration.
<br>
<br>
The calibration is done via `one-wire` digital serial interface or through a `Two-wire` industry standard connection. For more information please refer to the [pga309 User Guide](https://www.ti.com/lit/ug/sbou024b/sbou024b.pdf?ts=1637018226340&ref_url=https%253A%252F%252Fwww.google.com%252F)

## Connection Layout: <a name="connection-layout"></a>
The library is written for esp32 so the following pin configuration needs to be taken into account. Any other microcontroller can also be interfaced with pga309, please refer to [supported functionality](#functionality) section. 

| ESP32 | PGA309 |
| --- | --- |
| GPIO23   | PRG |
| GPIO21 | SDA |
| GPIO22    | SCL |
| 3.3V    | VDD |
| GND     | GND |

## Project Directory: <a name="project-directory"></a>
The project is developed in Visual studio code under the PlatformIO extension. The code supports official docuemntation from [espressifs.](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/index.html) The structure of the files is shown below:

```
- esp-idf 
  -> .vscode
  -> include 
  -> lib
     -> pga309
        --> pga309.h
        --> pga309.c
  -> src
     --> strainGauge_main.c
  -> test
  -> ...
  -> platformio.ini
  
 - scripts
   --> pga309_register_calculation.m
```
The `strainGauge_main.c` file contains minimal application example code with resistive strain gauges. 

## Supported Functionality: <a name="functionality"></a>
pga309 is a complete signal conditioner with bridge excitation, initial span and offset adjustment, temperature adjustment of span and offset, internal/external temperature measurement capability, output over-scale and under-scale limiting, fault detection, and digital calibration. 
<br>
<br>
This library only supports some of the functionalities which includes `bridge excitation and linearization control`, `fine offset`, `fine gain adjustment` and `pga coarse offset adjustment` 

There are two digital interfaces on the pga309, `one wire` and `two wire` interfaces. The `PRG PIN` uses a one wire interface (UART Compatible) while external EEPROM uses the two wire interface for programming and reading. This library only supports `one wire` interface and communication with the pga309. 

### How to use library? 
The library is written in C language for esp32 but any other microcontroller can be used with just slight modification in the espressif's APIs. 

The `scripts` folder contains `pga309_register_calculation.m` This file can be used to calculate the internal register map values. The following things need to be configured according to your desired application and resistive bridge specifications.  
```
FSS = 1.5e-3; %Full scale sensitivity of the bridge sensor
Vos = 0; %offset voltage in mV
Vref = 3.3; %Reference voltage applied across pga309. 
Vb = 2.7; %bridge voltage across sensor
Rbrg = 120; %Bridge resistance in ohms
Vout_min = 0.5; %Desired minimum output voltage 
Vout_max = 2.5; %Desired maximum output voltage
```
The `strainGauge_main.c` contains a minimum application example for resistive strain gauges. The following `struct` needs to be changed according to your desired application,
```
static config_t config = {
    .FSS = 0.0015,
    .Vb = 2.7,
    .Vout_min = 0.5,
    .Vout_max = 2.5,
    .Vref = 3.3,
    .FEG = 128,
    .GO = 4.5,
};
```
For gain settings: please refer to the [PGA309 User Guide](https://www.ti.com/lit/ug/sbou024b/sbou024b.pdf?ts=1637018226340&ref_url=https%253A%252F%252Fwww.google.com%252F), the following structure `variables` has to be modified:
```
static reg4_t reg4_data = {
    .FEG = 16,                /* Front End Gain*/
    .GO = 2,                  /* Output Amplifier Gain*/
    .Coarse_Offset = -4.25,   /* Coarse Offset Adjustment*/
};
```

PGA309 consists of seven registers. 4 out of 7 internal registers can be modified with the help of this library. Additional functionalities can be adapted according to the application requirements. The library supports:
<br>
- register 1 (Fine Offset Adjust - Zero DAC) <br/>
- register 2 (Fine Gain Adjust - Gain Adjust) <br/>
- register 3 (Reference control and Linearization Register) <br/>
- register 4 (PGA Coarse Offset Adjust and Gain Control) <br/>

### Resources: <a name="resources"></a>
1. [PGA309 User Guide](https://www.ti.com/lit/ug/sbou024b/sbou024b.pdf?ts=1637018226340&ref_url=https%253A%252F%252Fwww.google.com%252F)
2. [ESP32 esp-idf official API Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/index.html)

### License: <a name="licenses"></a>
