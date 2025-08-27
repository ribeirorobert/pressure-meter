 /**
 * @file MPX5010DP.cpp
 * @author Robert R. Gomes
 * @brief Library to communicate with pressure sensor
 * @version 1.0
 * @date 03-06-2025
 * 
 * @copyright Copyright (c) 2025
 * 
*/

#include "MPX5010DP.h"


MPX5010_Typedef mpx;

/**
 * @brief read raw pressure
 * 
 * @param void
 * @return nothing
*/
void MPX5010_update_pressure(void) {
  mpx.rawValue = analogRead(MPX5010_PIN);
  mpx.pressure = (float)(((mpx.rawValue - (float)MPX5010_PRESSURE_OFFSET) 
                          * (float)MXP5010_FACTOR_PRESSURE) 
                          + mpx.offset);
}


/**
 * @brief get offset
 * 
 * @param void
 * @return offset
*/
int16_t MPX5010_get_offset(void) {
  return mpx.offset;
}


/**
 * @brief set offset
 * 
 * @param void
 * @return void
*/
void MPX5010_set_offset(float value) {
  mpx.offset += (-1) * value;
}


/**
 * @brief filtered pressure 
 * 
 * @param void
 * @return pressure
*/
float MPX5010_filtered_pressure(void) {
  mpx.pressSum -= mpx.pressBuffer[mpx.pressIndex];
  mpx.pressBuffer[mpx.pressIndex] = mpx.pressure;
  mpx.pressSum += mpx.pressBuffer[mpx.pressIndex];

  mpx.pressIndex ++;
  if (mpx.pressIndex >= MPX5010_PRESSURE_INDEX) {
    mpx.pressIndex = 0;
    mpx.filter = true;
  }

  if (mpx.filter) {
    mpx.filteredPressure = (float)(mpx.pressSum / (float)MPX5010_PRESSURE_INDEX);
    return mpx.filteredPressure;
  } else {
    return (float)mpx.pressure;
  }
}