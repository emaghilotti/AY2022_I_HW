/*
BRIDGE CONTROL PANEL
w 08 00 14 p   ;set # samples (5) + both stop
w 08 00 15 p   ;set # samples (5) + just temp
w 08 00 16 p   ;set # samples (5) + just ldr
w 08 00 17 p   ;set # samples (5) + both start

w 08 01 4 p   ;set the Timer used to generate the ISR (4 ms)
;r 08 x p
;---- REPEAT ----
w 08 03 p
r 08 @1TEMP p
w 08 04 p
r 08 @0TEMP p
w 08 05 p
r 08 @1LDR p
w 08 06 p
r 08 @0LDR p
;-----------------
*/

/* Assignment academic year 2021-2022 - I semester
   Group 9
   Emanuele Ghilotti
   Giuseppe Di Tursi
*/

// Include libraries
#include "project.h"
#include "InterruptRoutines.h"
#include "I2C_Communication.h"

// Initialization of global variables
volatile int flag_ISR = 0;          // flag used to track ISR
volatile int number_of_samples;     // number of samples to be averaged
int counter = 0;                    // counter used to track the desired number of samples
int32 average_temp = 0;             // initialization of the temperature sensor average
int32 average_ldr = 0;              // initialization of the LDR sensor average
uint8_t status;                     // sampling channel selection: 0b00 --> device stopped
                                    //                             0b01 --> sample of Ch0
                                    //                             0b10 --> sample of Ch1
                                    //                             0b11 --> sample of Ch0 and Ch1

#define WHO_AM_I          0xBC  
#define STOP_BOTH         0x00      // 0b00 --> device stopped
#define ONLY_TEMP         0x01      // 0b01 --> sample of Ch0
#define ONLY_LDR          0x02      // 0b10 --> sample of Ch1
#define START_BOTH        0x03      // 0b11 --> sample of Ch0 and Ch1


int main(void)
{
    
    CyGlobalIntEnable;      // global interrupt activation
    
    // Start the components 
    isr_Timer_StartEx(Custom_ISR_ADC);       
    Timer_Start();
    Analog_MUX_Start();
    EZI2C_Start();   
    
    // Initialization of the I2C Slave Buffer Structure
    Slave_Buffer[0] = 0x00;
    Slave_Buffer[1] = 0x00;
    Slave_Buffer[2] = WHO_AM_I;
    SetBuffer(0,0);     // see I2C_Communication.c
    
    // Sets up the data buffer to be exposed to the master on a primary slave address request
    EZI2C_SetBuffer1(SLAVE_BUFFER_SIZE, SLAVE_BUFFER_RW, Slave_Buffer);
      
    
    for(;;)
    {
        // if the all samples to be averaged are already collected or the timer period is zero:
        //  - number of samples are updated
        //  - variables are reset
        //  - timer is reset
        if(counter == 0 || Timer_ReadPeriod() == 0)
        {            
            number_of_samples = (Slave_Buffer[0] & 0b00111100) >> 2;    // updates the number of samples
            
            if(number_of_samples == 0){     // if the number of samples is set to zero, no sampling is performed
                status = STOP_BOTH;
            }
        
            average_temp = 0;       // variables are zeroed
            average_ldr = 0;    
            sum_temp = 0;
            sum_ldr = 0;
            reset_of_the_timer();   // timer is reset (see I2C_Communication.c)
        }
        
 
        status = (Slave_Buffer[0] & 0b00000011);        // status is updated 
        
        switch (status){   
            
            case STOP_BOTH:         // 0b00 --> device stopped                                  
                ADC_Stop();         // ADC stops
                Pin_LED_Write(0);   // LED is turned off  
                break;
                            
            case ONLY_TEMP:                         // 0b01 --> sample of Ch0                                    
                Pin_LED_Write(0);                   // LED is turned off                                                       
                if(counter < number_of_samples)     // until desired number of samples is reached        
                {   
                    while(flag_ISR==0){};   // only when the ISR occurs the following code is executed
                    sample_temp();          // temperature sampling (see InterruptRoutines.c)                                
                    flag_ISR = 0;           // flag reset                                   
                    counter++;              // counter updated                                       
                }
                else if(counter >= number_of_samples)                   // when desired number of samples is reached                           
                {   
                    average_temp = sum_temp / number_of_samples;        // temperature average computation                    
                    average_temp = ADC_CountsTo_mVolts(average_temp);   // temperature average mapped from 0 to 5000mV    
                    SetBuffer(average_temp, average_ldr);               // set from 0x03 to 0x06 of the I2C Slave Buffer Structure (see I2C_communication.c)              
                    counter=0;                                         
                }    
                break;
                                                      
            case ONLY_LDR:                          // 0b10 --> sample of Ch1  
                                                    // (previous algorithm applies to ONLY_LDR case)                                  
                Pin_LED_Write(0);                                                                   
                if(counter < number_of_samples)             
                {                              
                    while(flag_ISR==0){};                                                      
                    sample_ldr();                                                  
                    flag_ISR = 0;                                                 
                    counter++;                                                           
                }  
                else if(counter >= number_of_samples)
                {   
                    average_ldr = sum_ldr / number_of_samples;                 
                    average_ldr = ADC_CountsTo_mVolts(average_ldr);       
                    SetBuffer(average_temp, average_ldr);                 
                    counter=0;                                          
                }                 
                break;       
                                
            case START_BOTH:                        // 0b11 --> sample of both channels  
                                                    // (previous algorithm applies to START_BOTH case)                            
                Pin_LED_Write(1);                       
                if(counter < number_of_samples)
                {                                      
                    while(flag_ISR==0){};
                    sample_ldr();                   
                    sample_temp();                   
                    flag_ISR = 0;                                    
                    counter++;                                        
                }    
                else if(counter >= number_of_samples)                  
                {
                    average_ldr = sum_ldr / number_of_samples;                 
                    average_temp = sum_temp / number_of_samples;
                 
                    average_ldr = ADC_CountsTo_mVolts(average_ldr);      
                    average_temp = ADC_CountsTo_mVolts(average_temp);                        
                    
                    SetBuffer(average_temp, average_ldr);
                    
                    counter=0;                                       
                }
                break;
            
            
            default:        
                break;
                
        }     
    }
}



