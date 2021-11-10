
#ifndef __INTERRUPT_ROUTINES_H__
    
    #define __INTERRUPT_ROUTINES_H__
    #include "project.h"
    
    #define TEMP_MUX 0x01
    #define LDR_MUX 0x00
    
    int32 temp;
    int32 ldr;
    int32 sum_temp;
    int32 sum_ldr;
        
    CY_ISR_PROTO(Custom_ISR_ADC);
  
    void sample_temp();    
    void sample_ldr();
      
#endif







