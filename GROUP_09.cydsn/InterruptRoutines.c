
// Include libraries
#include "project.h"
#include "InterruptRoutines.h"

// Initialization flag_ISR
extern volatile int flag_ISR;

// every time ISR occurs, flag_ISR is set to 1
CY_ISR(Custom_ISR_ADC)
{
    Timer_ReadStatusRegister();     // returns the current state of the status register 
    
    flag_ISR = 1;                   // flag is set to 1
}


void sample_temp()                      // function to sample temperature sensor
{   
    Analog_MUX_FastSelect(TEMP_MUX);    // channel selection
    ADC_Start();                        // ADC starts
    ADC_StartConvert();                 // ADC starts conversion
    temp = ADC_Read32();                // ADC acquires sample
    if(temp > 65535)    temp = 65535;   // to limit the sample within 2 bytes size   
    if(temp < 0)        temp = 0;       // to prevent negative values
    sum_temp = sum_temp + temp;         // updates the sum
}


void sample_ldr()                       // function to sample LDR sensor
{   
    Analog_MUX_FastSelect(LDR_MUX);     // channel selection             
    ADC_Start();                        // ADC starts
    ADC_StartConvert();                 // ADC starts conversion
    ldr = ADC_Read32();                 // ADC acquires sample
    if(ldr > 65535)    ldr = 65535;     // to limit the sample within 2 bytes size
    if(ldr < 0)       ldr = 0;          // to prevent negative values
    sum_ldr = sum_ldr + ldr;            // updates the sum
}


