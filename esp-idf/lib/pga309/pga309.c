#include <time.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"
#include "esp_log.h"

#include "pga309.h"


#define txPin GPIO_NUM_23 

/*
Set the baud rate(speed) of the UART Transmission. 
Baud rates between 4.8kbits/sec to 38.4kbits/sec are acceptable.
*/
#define speed 9600

static bool testPinconfig = false; 
static bool txPinconfig = false;
static bool start_bit = 0; 
static bool stop_bit = 1;

unsigned long int delay = 0;


void set_testPin(uint8_t testPin, bool signal){
    if(!testPinconfig) {
    gpio_config_t io_config;
    //disable interrupt
    io_config.intr_type = GPIO_PIN_INTR_DISABLE;
    //set as output mode
    io_config.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set
    io_config.pin_bit_mask = (1ULL<<testPin);
    //disable pull down mode
    io_config.pull_down_en = 0;
    //Enable pull up mode
    io_config.pull_up_en = 0;
    //Configure gpio with given settings
    gpio_config(&io_config);

    //enable test pin
    testPinconfig = true;
    }
    gpio_set_level(testPin, signal);
    vTaskDelay(5.6/portTICK_PERIOD_MS);
}


static void reg_write(uint16_t data, reg_addr_t reg_addr){

    /*Configre the gpio pin as an output, disable interrupt and enable pull up */
    if(!txPinconfig){
    gpio_config_t io_config;
    //disable interrupt
    io_config.intr_type = GPIO_PIN_INTR_DISABLE;
    //set as output mode
    io_config.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set
    io_config.pin_bit_mask = (1ULL<<txPin);
    //disable pull down mode
    io_config.pull_down_en = 0;
    //Enable pull up mode
    io_config.pull_up_en = 1;
    //Configure gpio with given settings
    gpio_config(&io_config);
    vTaskDelay(5.6/portTICK_PERIOD_MS);

    //enabling tx pin config done 
    txPinconfig = true;
    //time needed for one bit
    delay = (1/speed)*1000000;
    }

    bool bit_value = 0;
    uint8_t data_arr[7] = {initialization_byte, reg_addr_cmd, reg_addr, initialization_byte, reg_write_cmd, (data & 0x00FF), ((data & 0xFF00)>>8)};
    for(int i = 0; i<=7; i++){
        //start bit sending 
        gpio_set_level(txPin, start_bit);
        ets_delay_us(delay);
        
        //Data bits sending
        for(int j = 0; j < 8; j++){
        bit_value = 0 != (data_arr[i] & (1 << j));
        gpio_set_level(txPin, bit_value);
        ets_delay_us(delay);
        }

        //Stop bit sending 
        gpio_set_level(txPin, stop_bit);
        ets_delay_us(delay);
    }
}


/*
    Function: pga_config
        set the pga309 Gain DAC and Zero DAC register
        values based on the parameters user will specify in config_t structure. 
*/
void pga_config(config_t *config_pga){
    //Calculating maximum bridge voltage
    float VB_max  = (config_pga->FSS)*(config_pga->Vb);
    //Calculating maximum desired gain
    float GT = ((config_pga->Vout_max)-(config_pga->Vout_min))/VB_max;
    //Calculating value for GAIN DAC register 
    float Gain_DAC = GT/((config_pga->FEG)*(config_pga->GO));
    //Convert GAIN DAC value into Decimal counts
    uint16_t Dec_gain = (Gain_DAC-0.333)*1.5*65536;
    //Calculating zero DAC in volts 
    float Vzero_DAC = (config_pga->Vout_min)/(Gain_DAC*(config_pga->GO));
    //Convert Vzero_DAC into Decimal counts
    uint16_t Dec_zero = Vzero_DAC/((config_pga->Vref)/65536);

    //writing to pga309 register # 2
    reg_write(Dec_gain, reg2_addr_ptr);
    vTaskDelay(5.6/portTICK_PERIOD_MS);

    //writing to pga309 register # 1
    reg_write(Dec_zero, reg1_addr_ptr);
    vTaskDelay(5.6/portTICK_PERIOD_MS);
}

/*
    Function: set_reg3
        Set register 3 of PGA309, (Linearization and excitation and 
        reference voltage control register)
*/
void set_reg3(reg3_t *reg_data){
    uint16_t data = 0x0;
    data = (reg_data->EXS << 11) | (reg_data->EXEN << 10) | (reg_data->RS << 9) | (reg_data->REN << 8);
    //calling register write function. This function writes commands into pga309.
    reg_write(data, reg3_addr_ptr);
    vTaskDelay(5.6/portTICK_PERIOD_MS);
}


//Front end gain calculation function
static uint16_t calc_FrontEndGain(float FronEndGain){
    if (FronEndGain == 4)
        {return 0x0000;}
    else if (FronEndGain == 8)
        {return 0x0001;}
    else if (FronEndGain == 16)
        {return 0x0002;}
    else if (FronEndGain == 23.27f)
        {return 0x0003;}
    else if (FronEndGain == 32)
        {return 0x0004;}
    else if (FronEndGain == 42.67)
        {return 0x0005;}
    else if (FronEndGain == 64)
        {return 0x0006;}
    else if (FronEndGain == 128)
        {return 0x0007;}
    else 
        {return 0x0000;}
}

//Output amplifier gain calculation function
static uint16_t calc_OutputGain(float outputGain){

    if (outputGain == 2)
        {return 0x0000;}
    else if (outputGain == 2.4f)
        {return 0x0001;}
    else if (outputGain == 3)
        {return 0x0002;}
    else if (outputGain == 3.6f)
        {return 0x0003;}
    else if (outputGain == 4.5f)
        {return 0x0004;}
    else if (outputGain == 6)
        {return 0x0005;}
    else if (outputGain == 9)
        {return 0x0006;}
    else if (outputGain == 0) //diisable internal feedback
        {return 0x0007;}
    else 
        {return 0x0000;}
}

// Coarse offset ajust calculation function
static uint16_t calc_CoarseOffset(float coarseOffset, float VREF){
uint16_t step = coarseOffset / (VREF * 0.00085);
	if(abs(step) > 0){
		if(abs(step) > 7){
			if(step > 0){
				return (step + 1);
			} else {
				return ((abs(step) + 1) | 0b10000);
			}
		} else {
			if(step > 0){
				return (step);
			} else {
				return (abs(step) | 0b10000);
			}
		}
	} else {
		return 0;
	}
}

/*
    Function: set_reg4
        Set register 4 of PGA309, (Front end gain, output amplifier gain and 
        coarse offset adjust register)
*/
void set_reg4(reg4_t *reg_data){
    uint16_t data = 0x0;
    data = (reg_data->OWD << 15) | (reg_data->IMUX << 11);

    //Converting gain and coarse offset values to binary equivalents
    uint16_t front_end_gain = calc_FrontEndGain(reg_data->FEG);
    uint16_t out_amp_gain = calc_OutputGain(reg_data->GO);
    uint16_t coarse_offset = calc_CoarseOffset(reg_data->Coarse_Offset, reg_data->Vref);

    data = data | (front_end_gain << 8) | (out_amp_gain << 12) | coarse_offset;
    //calling register write function. This function writes commands into pga309.
    reg_write(data, reg4_addr_ptr);
    vTaskDelay(5.6/portTICK_PERIOD_MS);
}
