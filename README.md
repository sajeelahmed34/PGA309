**One Wire Programming interface for Programmable Gain Amplifier PGA309**
<br/>
<br/>
This project consists of two files, 
- PGA309_Sript.m (Matlab code which could be directly used to calculate PGA309 register values)
- esp-idf ('C' code for PGA309 written in Visual Studio Code under extension PlatformIO and esp-idf developement kit)
<br/>
Matlab script is very simple which could be opened in Matlab and run the script. The required specifications has to be mentioned beforehand in the Script. For example, bridge resistance, reference voltage, output voltage maximum and minimum range etc. 
<br/>
<br/>
While, esp-idf is a complete project which contains libraries (pga309.c, pga309.h) and example code (strainGauge_main.c). 
<br/>
<br/>
In this project, we are using one-wire programming interface to update the internal registers of pga309. Bit-banging scheme has been used by keeping the same concept as used in UART communication. It is because, PGA309 accepts UART protocol. 
<br/>
<br/>
PGA309 consists of seven different registers which needs to be updated according to the project. In this code only few registers are updated. The list of the registers which can be updated using this code is listed below. 
<br/>
- register 1 (Fine Offset Adjust - Zero DAC) <br/>
- register 2 (Fine Gain Adjust - Gain Adjust) <br/>
- register 3 (Reference control and Linearization Register) <br/>
- register 4 (PGA Coarse Offset Adjust and Gain Control) <br/>
<br/>
<br/>
Other register updates will be added shortly. For more imformation regarding code and which register does which thing, please refer to pga309 uuser guide.
