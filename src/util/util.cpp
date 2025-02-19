/**
 * @file util.cpp
 * @brief Utility functions for the MCP3008 sensor calibration project
 * @date 02-06-2025
 * @par The utility functions for the 555 soil moisture sensor calibration
 * project.
 */
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <cstring>
#include <vector>
#include "config.h"
#include "struct.h"
#include "config-file.h"
#include "util.h"

extern Config_File *config_file; // Import from main.cpp
extern Param_t param; // Import from struct.c

const std::vector<std::string> allowed_font_families = {
  "FreeMono",
  "FreeSans",
  "FreeSerif",
  "PibotoLT",
  "LiberationMono",
  "LiberationSans",
  "LiberationSerif",
  "NimbusMonoPS",
  "NimbusRoman"
};

// not used yet
const std::vector<std::string> allowed_font_weights = {
  "bold",
  "normal"
};

/**
 * @brief Convert double to string, with precision adjust
 * @param val - Double value to convert
 * @param precision - Number of decimal places to display
 * @return String representation of the double value
 */
std::string dtos(double val, unsigned int precision)
{
  std::ostringstream out;
  out << std::fixed << std::setprecision(precision) << val;
  return out.str();
}

/**
 * @brief Clear terminal screen
 * @param none
 * @return none
 */
void clear_terminal_screen(void)
{
    std::cout << "\033[2J\033[H";
}

/**
 * @brief Verify font family string is valid
 * @param font_family - Font family string to verify
 * @return true if font family is valid, false otherwise
 */
 bool is_font_family_valid(const char *font_family)
 {
  for (const auto &family : allowed_font_families) {
    if(0 == std::strcmp(family.c_str(), font_family)) {
      return true;
    }
  }
  return 0;
}

/**
 * @brief Save calibration data to config file
 * @param sensor_num - the sensor number to save calibration data for
 * @return 0 if success, -1 if error
 */
int save_calibration_data(unsigned int sensor_num)
{
  std::string section = "calibration";
  std::string key;
  std::string value;

  key = "sensor_offset_" + std::to_string(sensor_num);
  value = dtos(param.sensor_offset[sensor_num], param.raw_disp_precision);
  if(0 != config_file->set_config_value(section, key, value, false)) {
    std::cerr << "Error setting config value: " << key << '\n';
    return -1;
  }
  
  key = "sensor_cal2_raw_" + std::to_string(sensor_num);
  value = dtos(param.sensor_cal2_raw[sensor_num], param.raw_disp_precision);
  if(0 != config_file->set_config_value(section, key, value, false)) {
    std::cerr << "Error setting config value: " << key << '\n';
    return -1;
  }
  
  key = "sensor_slope_" + std::to_string(sensor_num);
  value = dtos(param.sensor_slope[sensor_num], param.raw_disp_precision);
  if(0 != config_file->set_config_value(section, key, value, false)) {
    std::cerr << "Error setting config value: " << key << '\n';
    return -1;
  }
  return 0;
}

/**
 * @brief Print a double value stored in config file for a sensor
 * @param section - Config file section
 * @param key - Config file key
 * @return 0 if success, -1 if error
 */
int print_stored_param_double(std::string section, std::string key)
{
  std::string value = "";
  value = config_file->get_config_value(section, key);
  if("" == value) {
    std::cerr << "print_stored_param_double(): config_file->get_config_value() failed\n";
    return -1;
  }
  std::cout << key << ": " << value << "\n";
  return 0;
}

/**
 * @brief Print calibration params stored in config file for a sensor
 * @param sensor_index - sensor to print calibration params for
 * @return 0 if success, -1 if error
 */
int print_cal_param(unsigned int sensor_index)
{
  if(MAX_SENSORS <= sensor_index) {
    std::cerr << "print_cal_param(): sensor_index out of range: " << sensor_index << '\n';
    return -1;
  }
  
  std::cout << "\nSensor #" << sensor_index << " calibration param:\n";
  std::cout << "sensor_cal1_pv: " << param.sensor_cal1_pv << "\n";
  std::cout << "sensor_cal2_pv: " << param.sensor_cal2_pv << "\n";
  std::cout << "sensor_cal1_raw: " << param.sensor_cal1_raw[sensor_index] << "\n";
  std::cout << "sensor_cal2_raw: " << param.sensor_cal2_raw[sensor_index] << "\n";
  std::cout << "sensor_offset: " << param.sensor_offset[sensor_index] << "\n";
  std::cout << "sensor_slope: " << param.sensor_slope[sensor_index] << "\n";
  
  return 0;
}

/**
 * @brief Print calibration params stored in config file for a sensor
 * @param sensor_index - sensor to print calibration params for
 * @return 0 if ok, -1 if error
 */
int print_stored_cal_param(unsigned int sensor_index)
{
  std::cout << "\nSensor #" << (sensor_index+1) << " stored calibration param:\n";
  std::string section = "calibration";
  std::string key = "";
  int status = 0;
  
  if(MAX_SENSORS <= sensor_index) {
    std::cerr << "print_stored_cal_param(): sensor_index out of range: " << sensor_index << '\n';
    return -1;
  }
  
  key = "sensor_offset_" + std::to_string(sensor_index);
  status = print_stored_param_double(section, key);
  
  if(0 == status) {
    key = "sensor_cal2_raw_" + std::to_string(sensor_index);
    status = print_stored_param_double(section, key);
  }

  if(0 == status) {
    key = "sensor_slope_" + std::to_string(sensor_index);
    status = print_stored_param_double(section, key);
  }

  if(status) {
    std::cerr << "print_stored_cal_param(): error reading " << "[" << section << "]: " << key << '\n';
  }
  
  return 0;
}

/**
 * @brief Print all sensor raw values
 * @param none
 * @return none
 */
void print_all_sensor_raw(void)
{
  for (unsigned int channel = 0; channel < param.num_sensors; ++channel) {
    std::cout << "Ch " << channel << ": " << param.sensor_raw[channel] << " mV" << '\n';
  }
}

// Pango wants color in 16-bit uint, whereas Cairo wants doubles.
/**
 * @brief Convert Color16_t RGB values to double (normalized) RGB values
 * @param color - Color16_t structure
 * @param red - pointer to red output
 * @param green - pointer to green output
 * @param blue - pointer to blue output
 * @return 0 if success, -1 if null color was passed
 */
int color16_to_rgbd(Color16_t color, double *p_red, double *p_green, double *p_blue)
{
  if((NULL == p_red) || (NULL == p_red) || (NULL == p_red)) {
    return -1;
  }
  *p_red = color.red / 65535.0;
  *p_green = color.green / 65535.0;
  *p_blue = color.blue / 65535.0;
  return 0;
}

/**
 * @brief Convert double RGB values to 16-bit RGB values
 * @param color - pointer to Color16_t structure output
 * @param red - pointer to red output
 * @param green - pointer to green output
 * @param blue - pointer to blue output
 * @return 0 if success, -1 if null Color_t was passed, -2 if invalid color
 * was pased
 */
int rgbd_to_color16(Color16_t *p_color, double red, double green, double blue)
{
  if(NULL == p_color) {
    return -1;
  }
  
  if((1.0 < red) || (1.0 < green) || (1.0 < blue) ||
     (0 > red) || (0 > green) || (0 > blue)) {
    return -2;
  }
  
  p_color->red = 65535.0 * red;
  p_color->green = 65535.0 * green;
  p_color->blue = 65535.0 * blue;
  return 0;
}

/**
 End of file
*/
