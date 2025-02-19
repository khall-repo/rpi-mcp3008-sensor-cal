/**
 * @file struct.h
 * @brief Data structures for the MCP3008 sensor calibration project
 * @par (C) 2025 Keith Hall
 */
#ifndef STRUCT_H
#define STRUCT_H

#include "config.h"
//#include "gui.h"

#ifdef __cplusplus
extern "C" {
#endif

/// @brief Sensor parameter structure typedef
typedef struct _Param_t {
  unsigned int num_sensors;
  double sensor_min_scale;
  double sensor_full_scale;
  double sensor_cal1_pv;
  double sensor_cal2_pv;
  double hi_alarm;
  double mid_alarm;
  double low_alarm;
  unsigned int raw_disp_precision;
  unsigned int pv_disp_precision;
  unsigned int adc_num_samples; // number of samples to average
  unsigned int ewma_filter;     // exponential weighted moving average filter val
  double sensor_raw[MAX_SENSORS];
  double sensor_pv[MAX_SENSORS];
  double sensor_cal1_raw[MAX_SENSORS];
  double sensor_cal2_raw[MAX_SENSORS];
  double sensor_slope[MAX_SENSORS];
  double sensor_offset[MAX_SENSORS];
  union {
    struct {
      uint32_t low_alarm :1;
      uint32_t mid_alarm :1;
      uint32_t hi_alarm  :1;
      uint32_t fault     :1;
      uint32_t unused    :28;
    }bits;
    uint32_t all_bits;
  }channel_status[MAX_SENSORS];
}Param_t;

/// @brief Analog input structure typedef
typedef struct _Analog_Input_t {
  int adc_val;
  int accum_adc;
  unsigned int sample_num;
  double unfiltered_mv;
  double filtered_mv;
}Analog_Input_t;

typedef struct _Appearance_Config_t {
  char data_display_font_family[MAX_FONT_FAMILY_LEN];
  int data_display_font_size;
}Appearance_Config_t;

typedef enum _Alarm_e {
  ALARM_NONE,
  ALARM_LOW,
  ALARM_MID,
  ALARM_HI,
  ALARM_FAULT
}Alarm_e;

int init_param();

#ifdef __cplusplus
}
#endif

#endif
