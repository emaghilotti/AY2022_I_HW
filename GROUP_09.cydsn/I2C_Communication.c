
// Inlcude libraries
#include "project.h"
#include "I2C_Communication.h"


// function to set address from 0x03 to 0x06 of the I2C Slave Buffer Structure
void SetBuffer(int32 average_temp, int32 average_ldr){

        Slave_Buffer[3] = average_temp >> 8;           // MSB of the temperature sensor
        Slave_Buffer[4] = average_temp & 0xFF;         // LSB of the temperature sensor
        Slave_Buffer[5] = average_ldr >> 8;            // MSB of the LDR
        Slave_Buffer[6] = average_ldr & 0xFF;          // LSB of the LDR
        
}

void reset_of_the_timer(){
    Timer_WritePeriod(Slave_Buffer[1]);     // set new timer period
    Reset_timer_Write(1);                   // timer reset
    CyDelay(1);
    Reset_timer_Write(0);     
    //Timer_ReadStatusRegister();   
    Timer_Start();                          // timer start
}


