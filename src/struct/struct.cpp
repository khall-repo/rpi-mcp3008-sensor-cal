/**
 * @file struct.cpp
 * @brief Data structures for the MCP3008 sensor calibration project
 * @par (C) 2025 Keith Hall
 */
#include <iostream>
#include <cstring>
#include "config.h"
#include "struct.h"
#include "config-file.h"
#include "util.h"

extern Config_File *config_file; // Import from main.cpp

Param_t param;
Analog_Input_t analog_input[MAX_SENSORS];
Appearance_Config_t appearance_config;

/**
 * @brief Set raw and PV values with test values from the config file
 * @param none
 * @return none
 */
int set_param_from_config_file(void)
{
  std::string value = "";

  if(NULL == config_file) {
    std::cerr << "Error: Config file not initialized" << '\n';
    return -1;
  }

  value = config_file->get_config_value("param", "num_sensors");
  param.num_sensors = std::stoi(value);
  value = config_file->get_config_value("param", "sensor_min_scale");
  param.sensor_min_scale = std::stod(value);
  value = config_file->get_config_value("param", "sensor_full_scale");
  param.sensor_full_scale = std::stod(value);
  value = config_file->get_config_value("param", "cal1_pv");
  param.sensor_cal1_pv = std::stod(value);
  value = config_file->get_config_value("param", "cal2_pv");
  param.sensor_cal2_pv = std::stod(value);
  value = config_file->get_config_value("param", "hi_alarm");
  param.hi_alarm = std::stod(value);
  value = config_file->get_config_value("param", "mid_alarm");
  param.mid_alarm = std::stod(value);
  value = config_file->get_config_value("param", "lo_alarm");
  param.low_alarm = std::stod(value);
  value = config_file->get_config_value("param", "raw_disp_precision");
  param.raw_disp_precision = std::stoi(value);
  value = config_file->get_config_value("param", "pv_disp_precision");
  param.pv_disp_precision = std::stoi(value);
  value = config_file->get_config_value("param", "adc_num_samples");
  param.adc_num_samples = std::stoi(value);

  // Load calibration data
  for (unsigned int sensor_index = 0; sensor_index < param.num_sensors; ++sensor_index) {
    value = config_file->get_config_value("calibration",
                                          "sensor_offset_" + std::to_string(sensor_index));
    try{
      param.sensor_offset[sensor_index] = std::stod(value);
    } catch (std::exception &e) {
      std::cerr << "Error: " << e.what() << '\n';
      std::cerr << "Error reading sensor offset for sensor index: " << sensor_index << '\n';
      std::cerr << "Value: " << value << '\n';
      return -1;
    }
    
    value = config_file->get_config_value("calibration",
                                          "sensor_cal2_raw_" + std::to_string(sensor_index));
    try{
      param.sensor_cal2_raw[sensor_index] = std::stod(value);
    } catch (std::exception &e) {
      std::cerr << "Error: " << e.what() << '\n';
      std::cerr << "Error reading cal2_raw for sensor index: " << sensor_index << '\n';
      std::cerr << "Value: " << value << '\n';
      return -1;
    }
    
    value = config_file->get_config_value("calibration",
                                          "sensor_slope_" + std::to_string(sensor_index));
    try{
      param.sensor_slope[sensor_index] = std::stod(value);
    } catch (std::exception &e) {
      std::cerr << "Error: " << e.what() << '\n';
      std::cerr << "Error reading sensor slope for sensor index: " << sensor_index << '\n';
      std::cerr << "Value: " << value << '\n';
      return -1;
    }
  }

  // Load appearance options
  value = config_file->get_config_value("appearance", "data_display_font_family");
  if (!is_font_family_valid(value.c_str())) {
    std::cerr << "Error: Invalid font family: " << value << '\n';
    return -1;
  }
  // check value string length and copy to appearance_config.data_display_font_family
  // if length is less than MAX_FONT_FAMILY_LEN
  if (value.length() < MAX_FONT_FAMILY_LEN) {
    memset(&appearance_config.data_display_font_family[0], 0, sizeof(appearance_config.data_display_font_family));
    strncpy(appearance_config.data_display_font_family, value.c_str(), value.length());
  } else {
    std::cerr << "Error: Font family string too long: " << value << '\n';
    return -1;
  }

  value = config_file->get_config_value("appearance", "data_display_font_size");
  appearance_config.data_display_font_size = std::stoi(value);
  //if(MAX_FONT_SIZE < appearance_config.data_display_font_size) {
  //  appearance_config.data_display_font_size = DEFAULT_DATA_DISP_FONT_SIZE;
  //}
  return 0;
}

/**
 * @brief Initialize the param structure
 * @param none
 * @return none
 */
int init_param()
{
  // zero out the param structure 
  param.num_sensors = 0;
  param.sensor_min_scale = 0.0;
  param.sensor_full_scale = 0.0;
  param.sensor_cal1_pv = 0.0;
  param.sensor_cal2_pv = 0.0;
  param.hi_alarm = 0.0;
  param.mid_alarm = 0.0;
  param.low_alarm = 0.0;
  param.raw_disp_precision = 0;
  param.pv_disp_precision = 0;
  for(int i = 0; i < MAX_SENSORS; i++) {
    param.sensor_raw[i] = 0.0;
    param.sensor_pv[i] = 0.0;
    param.sensor_cal1_raw[i] = 0.0;
    param.sensor_cal2_raw[i] = 0.0;
    param.sensor_slope[i] = 0.0;
    param.sensor_offset[i] = 0.0;
  }

  // zero out analog input structure
  for(int i = 0; i < MAX_SENSORS; i++) {
    analog_input[i].adc_val = 0;
    analog_input[i].accum_adc = 0;
    analog_input[i].sample_num = 0;
    analog_input[i].unfiltered_mv = 0.0;
    analog_input[i].filtered_mv = 0.0;
  }

  // zero out appearance config structure
  memset(&appearance_config, 0, sizeof(Appearance_Config_t));
  //strncpy(appearance_config.data_display_font_family,
  //        DEFAULT_DATA_DISP_FONT_FAMILY,
  //        std::strlen(DEFAULT_DATA_DISP_FONT_FAMILY)+1);

  // zero out channel status
  for(int i = 0; i < MAX_SENSORS; i++) {
    param.channel_status[i].all_bits = 0;
  }

  if(0 != set_param_from_config_file()) {
    return -1;
  }
  return 0;
}
