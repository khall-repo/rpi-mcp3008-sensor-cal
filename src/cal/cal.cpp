/**
 * @file cal.cpp
 * @brief Calibration functions
 * @par (C) 2025 Keith Hall
 */
#include <iostream>
#include "config.h"
#include "struct.h"
#include "cal.h"

// Import from struct.cpp
extern Param_t param;

/**
 * @brief Capture a zero calibration raw value
 * @param sensor_num - the sensor number to capture the zero for
 * @return -1 if error, 0 if success
 */
int capture_zero_raw(int sensor_num)
{
  if(MAX_SENSORS < sensor_num){
    return -1;
  }

  param.sensor_cal1_raw[sensor_num] = param.sensor_raw[sensor_num];

  return 0;
}

/**
 * @brief Capture a span calibration raw value
 * @param sensor_num - the sensor number to capture the span raw for
 * @return -1 if error, 0 if success
 */
int capture_span_raw(int sensor_num)
{
  if(MAX_SENSORS < sensor_num){
    return -1;
  }

  param.sensor_cal2_raw[sensor_num] = param.sensor_raw[sensor_num];

  return 0;
}

/**
 * @brief Capture and force zero now, with option to recalculate slope
 * @param sensor_num - the sensor number to force the zero offset for
 * @param recalculate_slope - True causes recalculation of slope after zero
 * offset capture
 * @return -1 if error, 0 if success
 */
int force_zero(int sensor_num, bool recalculate_slope)
{
  if(MAX_SENSORS < sensor_num){
    return -1;
  }

  param.sensor_cal1_raw[sensor_num] = param.sensor_raw[sensor_num];
  param.sensor_offset[sensor_num] = param.sensor_cal1_raw[sensor_num];

  if(recalculate_slope){
    do_span(sensor_num);
  }

  return 0;
}

/**
 * @brief Capture span and calculate the calibration slope for a sensor now
 * @param sensor_num - the sensor number to capture span and calculate the cal
 * slope for
 * @return -1 if error, 0 if success
 */
int force_span(int sensor_num)
{
  if(MAX_SENSORS < sensor_num){
    return -1;
  }
  param.sensor_cal2_raw[sensor_num] = param.sensor_raw[sensor_num];
  param.sensor_slope[sensor_num] = (param.sensor_cal2_raw[sensor_num] - param.sensor_cal1_raw[sensor_num])/(param.sensor_cal2_pv - param.sensor_cal1_pv);

  return 0;
}

/**
 * @brief Capture the zero offset for a sensor
 * @param sensor_num - the sensor number to capture the zero offset for
 * @return -1 if error, 0 if success
 */
int do_zero(int sensor_num)
{
  if(MAX_SENSORS < sensor_num){
    return -1;
  }

  param.sensor_offset[sensor_num] = param.sensor_cal1_raw[sensor_num];

  return 0;
}

/**
 * @brief Calculate the calibration slope for a sensor
 * @param sensor_num - the sensor number to calculate the cal slope for
 * @return -1 if error, 0 if success
 */
int do_span(int sensor_num)
{
  if(MAX_SENSORS < sensor_num){
    return -1;
  }

  param.sensor_slope[sensor_num] = (param.sensor_cal2_raw[sensor_num] - param.sensor_cal1_raw[sensor_num])/(param.sensor_cal2_pv - param.sensor_cal1_pv);

  return 0;
}
