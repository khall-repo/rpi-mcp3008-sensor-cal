/**
 * @file util.h
 * @brief Utility functions for the MCP3008 sensor calibration project
 * @date 02-06-2025
 * @par The utility functions for the MCP3008 sensor calibration project
 */
#ifndef UTIL_H
#define UTIL_H

#ifdef __cplusplus
#include <string>
extern "C" {
#endif

#include "attrib.h"

#ifdef __cplusplus
std::string dtos(double val, unsigned int precision);
#endif

void clear_terminal_screen(void);
bool is_font_family_valid(const char *font_family);
int save_calibration_data(unsigned int sensor_num);
int print_cal_param(unsigned int sensor_index);
int print_stored_cal_param(unsigned int sensor_index);
int color16_to_rgbd(Color16_t color, double *red, double *green, double *blue);
int rgbd_to_color16(Color16_t *p_color, double red, double green, double blue);

#ifdef __cplusplus
}
#endif

#endif // UTIL_H
/**
 End of file
*/
