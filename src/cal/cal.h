/**
 * @file cal.cpp
 * @brief Calibration functions
 * @par (C) 2025 Keith Hall
 */
#ifndef CAL_H
#define CAL_H

int capture_zero_raw(int sensor_num);
int capture_span_raw(int sensor_num);
int force_zero(int sensor_num, bool recalculate_slope);
int force_span(int sensor_num);
int do_zero(int sensor_num);
int do_span(int sensor_num);

#endif