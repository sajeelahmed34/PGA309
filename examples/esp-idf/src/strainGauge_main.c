#include "driver/gpio.h"
#include "pga309.h"


static config_t config = {
    .FSS = 0.0015,
    .Vb = 2.7,
    .Vout_min = 0.5,
    .Vout_max = 2.5,
    .Vref = 3.3,
    .FEG = 128,
    .GO = 4.5,
};


static reg3_t reg3_data = {
    .EXEN = 0,
    .EXS = 1,
    .REN = 0,
    .RS = 1,
};


static reg4_t reg4_data = {
    .OWD = 0,
    .IMUX = 0,
    .FEG = 16,
    .GO = 2,
    .Coarse_Offset = -4.25,
};


void app_main(void){

    /*
    Function: set_testPin
        Setting the test pin high enables direct writing of internal registers 
        and stops transactions with External EEPROM.
    */
    set_testPin(GPIO_NUM_4, true);


    /*
    Function: pga_config
        set the pga309 Gain DAC and Zero DAC register
        values based on the parameters user will specify in config_t structure. 
    */
    pga_config(&config);


    /*
    Function: set_reg3
        Set register 3 of PGA309, (Linearization and excitation and 
        reference voltage control register)
    */
    set_reg3(&reg3_data);


    /*
    Function: set_reg4
        Set register 4 of PGA309, (Front end gain, output amplifier gain and 
        coarse offset adjust register)
    */
    set_reg4(&reg4_data);


    /*
    Funcion: set_testPin
    For normal operation the test pin brought back to the LOW state
    */
    set_testPin(GPIO_NUM_4, false);
}