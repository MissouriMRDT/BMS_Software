/////////////////////////////////////////////////////
// Second pri task?
void setupPackSoftFuse() 
{
  adc_threshhold = mapFloats(PACK_SOFT_FUSE_AMPS_MAX_THRESHHOLD, AMPS_MIN, AMPS_MAX, ADC_MIN, ADC_MAX);
  
}//end fnctn

/////////////////////////////////////////////////////
// Second pri task?
void loopPackSoftFuse() 
{  
  // check for over current
  if( analogRead(PACK_AMPS_READ_P5_5) > adc_threshhold)
  {  
    //Task_sleep(ANALOG_DEBOUNCE_DELAY);
    delay(ANALOG_DEBOUNCE_DELAY);
    
    // double check to avoid transient triggering
    if( analogRead(PACK_AMPS_READ_P5_5) > adc_threshhold)
    {
      // turn pack off
      digitalWrite(ESTOP_WRITE_P6_0, LOW);
      
      //Task_sleep(REBOOT_DELAY);
      delay(REBOOT_DELAY);

      // try to turn pack on
      for(int reboot_tries = 0; reboot_tries < REBOOT_TRY_COUNT; reboot_tries++)
      {
        digitalWrite(ESTOP_WRITE_P6_0, HIGH);
     
        if( analogRead(PACK_AMPS_READ_P5_5) > adc_threshhold)
        {   
          digitalWrite(ESTOP_WRITE_P6_0, LOW);   
        }else{
          
          // successful reboot, resume looping
          break;
        }//end if        
      }//end for 
    }//end if
  }//end if
  
  delay(LOOP_DELAY); 
  
}//end loop
