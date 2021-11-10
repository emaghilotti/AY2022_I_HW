
#ifndef __I2C_COMMUNICATION_H__
    #define __I2C_COMMUNICATION_H__
    
    #include "project.h"
    
    
    #define SLAVE_BUFFER_SIZE 7
    #define SLAVE_BUFFER_RW  2
    uint8_t Slave_Buffer[SLAVE_BUFFER_SIZE];
       
    void SetBuffer(int32 average_temp, int32 average_ldr);
    void reset_of_the_timer();
       
#endif