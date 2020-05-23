/* 
    pga309.h --> Library for communication with PGA309 
    Created by Sajeel Ahmed, 20.05.2020

Description:

*/

#pragma once

#include <inttypes.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif


// Register addresses 
typedef enum {
    initialization_byte = 0x55, //initialization byte for transmission
    reg_addr_cmd = 0x01,   //register address command or point to this address
    reg_write_cmd = 0x04, //pga309 register write command

    //Register address pointers points to specific registers.Refer to page 104 of pga309 user guide.
    reg1_addr_ptr = 0x01,
    reg2_addr_ptr = 0x02, 
    reg3_addr_ptr = 0x03, 
    reg4_addr_ptr = 0x04
} reg_addr_t;


/*
PGA configuration and gain adjust setting.
*/
typedef struct {
    float FSS;  // Full scale sensitivity of the bridge sensor 
    float Vb; // Voltage across bridge sensor 
    float Vout_min; // Desired minimum output voltage 
    float Vout_max; // Desired maximum output voltage 
    float Vref; // Reference voltage 
    float GO; // Desired Output amplifier gain 
    float FEG; // Desired front end gain 
} config_t;


/*
Reference Control and Linearization Register
- EXEN = Excitation voltage enable bit: 1 -> enable VEXC, 0 -> disable VEXC 
- EXS = Gain select bit: 1 -> Gain = 0.83*Vref/Vexc, 0 -> 0.52*Vref/Vexc
- RS = Internal reference Vref select: 1 -> 4.096V, 0 -> 2.5V
- REN = Enable/Disable internal reference: 1 -> enable internal reference, 0 -> disbale internal reference 
*/
typedef struct {
    bool EXEN;
    bool RS;
    bool EXS;
    bool REN;
} reg3_t;


/*
PGA Coarse offset adjust and Gain select/output amplifier gain.
- OWD = one wire disbale: 1 -> disable, 0 -> enable
- IMUX = input multiplexer bit: 1 -> Vin1 = Vinn, Vin2 = vinp
                                0 -> Vin1 = Vinp, Vin2 = vinn
*/
typedef struct {
    bool OWD;
    bool IMUX;
    float GO;
    float FEG;
    float Coarse_Offset;
    float Vref;
} reg4_t;



//Set TEST pin of the PGA309.
void set_testPin(uint8_t testPin, bool signal);

//PGA configuration and calculation function
void pga_config(config_t *config_pga);

//Reference Control and Linearization Register 
void set_reg3(reg3_t *reg_data);

//PGA Coarse offset adjust and Gain select/output amplifier gain. 
void set_reg4(reg4_t *reg_data);

#ifdef __cplusplus
}
#endif