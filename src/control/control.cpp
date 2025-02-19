/**
 * @file control.cpp
 * @brief Control for the MCP3008 sensor calibration project
 * @date 01-29-2025
 * @par The timebase and control code
 */
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <signal.h>
#include <time.h>
#include <math.h>
#include <wiringPi.h>
#include <mcp3004.h>
#include "util.h"
#include "config.h"
#include "config-file.h"
#include "imain-window.h"
#include "gui.h"
#include "struct.h"
#include "cal.h"
#include "control.h"

extern Config_File *config_file; // Import from main.cpp
extern Param_t param; // Import from struct.c
extern Analog_Input_t analog_input[MAX_SENSORS]; // Import from struct.c
extern IMainWindow imain_window; // import from imain-window.cpp

unsigned int seconds_count = 0;
bool one_sec_toggle = false;
timer_t timebase_timerid;

enum Color_Test_State_e{
  TEXT_RED,
  TEXT_GREEN,
  TEXT_BLUE,
  TEXT_BLACK,
  HIGHLIGHT_RED,
  HIGHLIGHT_GREEN,
  HIGHLIGHT_BLUE,
  HIGHLIGHT_WHITE,
  BG_RED,
  BG_GREEN,
  BG_BLUE,
  BG_WHITE,
  COLOR_TEST_DONE
};

void color_test(void)
{
  static unsigned int color_test_state = TEXT_RED;

  switch(color_test_state){
    case TEXT_RED:
      for(unsigned int i = 0; i < param.num_sensors; i++) {
        set_data_display_label_sensor_raw_fg_color(i, 0xffff, 0, 0);
        set_data_display_label_sensor_pv_fg_color(i, 0xffff, 0, 0);
      }
      ++color_test_state;
      break;
    case TEXT_GREEN:
      for(unsigned int i = 0; i < param.num_sensors; i++) {
        set_data_display_label_sensor_raw_fg_color(i, 0, 0xffff, 0);
        set_data_display_label_sensor_pv_fg_color(i, 0, 0xffff, 0);
      }
      ++color_test_state;
      break;
    case TEXT_BLUE:
      for(unsigned int i = 0; i < param.num_sensors; i++) {
        set_data_display_label_sensor_raw_fg_color(i, 0, 0, 0xffff);
        set_data_display_label_sensor_pv_fg_color(i, 0, 0, 0xffff);
      }
      ++color_test_state;
      break;
    case TEXT_BLACK:
      for(unsigned int i = 0; i < param.num_sensors; i++) {
        set_data_display_label_sensor_raw_fg_color(i, 0, 0, 0);
        set_data_display_label_sensor_pv_fg_color(i, 0, 0, 0);
      }
      ++color_test_state;
      break;
    case HIGHLIGHT_RED:
      for(unsigned int i = 0; i < param.num_sensors; i++) {
        set_data_display_label_sensor_raw_highlight_color(i, 0xffff, 0, 0);
        set_data_display_label_sensor_pv_highlight_color(i, 0xffff, 0, 0);
      }
      ++color_test_state;
      break;
    case HIGHLIGHT_GREEN:
      for(unsigned int i = 0; i < param.num_sensors; i++) {
        set_data_display_label_sensor_raw_highlight_color(i, 0, 0xffff, 0);
        set_data_display_label_sensor_pv_highlight_color(i, 0, 0xffff, 0);
      }
      ++color_test_state;
      break;
    case HIGHLIGHT_BLUE:
      for(unsigned int i = 0; i < param.num_sensors; i++) {
        set_data_display_label_sensor_raw_highlight_color(i, 0, 0, 0xffff);
        set_data_display_label_sensor_pv_highlight_color(i, 0, 0, 0xffff);
      }
      ++color_test_state;
      break;
    case HIGHLIGHT_WHITE:
      for(unsigned int i = 0; i < param.num_sensors; i++) {
        set_data_display_label_sensor_raw_highlight_color(i, 0xffff, 0xffff, 0xffff);
        set_data_display_label_sensor_pv_highlight_color(i, 0xffff, 0xffff, 0xffff);
      }
      ++color_test_state;
      break;
    case BG_RED:
      for(unsigned int i = 0; i < param.num_sensors; i++) {
        set_data_display_label_sensor_raw_bg_color(i, 0xffff, 0, 0);
        set_data_display_label_sensor_pv_bg_color(i, 0xffff, 0, 0);
      }
      ++color_test_state;
      break;
    case BG_GREEN:
      for(unsigned int i = 0; i < param.num_sensors; i++) {
        set_data_display_label_sensor_raw_bg_color(i, 0, 0xffff, 0);
        set_data_display_label_sensor_pv_bg_color(i, 0, 0xffff, 0);
      }
      ++color_test_state;
      break;
    case BG_BLUE:
      for(unsigned int i = 0; i < param.num_sensors; i++) {
        set_data_display_label_sensor_raw_bg_color(i, 0, 0, 0xffff);
        set_data_display_label_sensor_pv_bg_color(i, 0, 0, 0xffff);
      }
      ++color_test_state;
      break;
    case BG_WHITE:
      for(unsigned int i = 0; i < param.num_sensors; i++) {
        set_data_display_label_sensor_raw_bg_color(i, 0xffff, 0xffff, 0xffff);
        set_data_display_label_sensor_pv_bg_color(i, 0xffff, 0xffff, 0xffff);
      }
      color_test_state = TEXT_RED;
      break;
    default:
      std::cout << "color_test default state error\n";
      break;
  }
}

/**
 * @brief Read and print raw values from all 8 channels of the MCP3008
 * @param none
 * @return none
 */
void mcp3008_read_all_print(void)
{
  for (unsigned int channel = 0; channel < param.num_sensors; ++channel) {
    int value = analogRead(MCP3008_0_PINBASE + channel);
    std::cout << "Ch " << channel << ": " << value << '\n';
  }
}

/**
 * @brief Read MCP3008 ADC values and convert to millivolts
 * @param none
 * @return none
 */
void mcp3008_read_raw_mv_all(void)
{
  for (unsigned int channel = 0; channel < param.num_sensors; ++channel) {
    int value = analogRead(MCP3008_0_PINBASE + channel);
    param.sensor_raw[channel] = VREF * (double)value / ADC_MAX;
  }
}

/**
 * @brief Calculate millivolts from ADC value
 * @param adc_val - ADC output value
 * @return ADC output value converted to millivolts
 */
double calc_mv(int adc_val)
{
  return VREF * (double)adc_val / ADC_MAX;
}

/**
 * @brief Calculate filtered millivolts
 * @param unfiltered_mv - Current unfiltered millivolts value
 * @param last_filtered_mv - Last filtered millivolts value
 * @return Filtered millivolt value
 */
double calc_filtered_mv(double unfiltered_mv, double last_filtered_mv)
{
  double jump;
  double filtera;
  jump = fabs(last_filtered_mv - unfiltered_mv); // calculate difference between filter in and out

  if (jump > BIG_JUMP)  // set filter based on jump - for big jumps the filter is relaxed 
    filtera = (double) param.ewma_filter / 8.0;

  else if (jump > MID_JUMP)  // for middle jumps the filter is less relaxed than for big jumps
    filtera = (double) param.ewma_filter / 4.0;

  else if (jump > SMALL_JUMP) // for small jumps the filter is less relaxed than for middle jumps
    filtera = (double) param.ewma_filter / 1.5;

  else
    filtera = (double) param.ewma_filter;

  if (filtera < 1.0)    // dFilterA CANNOT BE LESS THAN 1 TO AVOID GETTING A NEGATIVE FILTER VALUE
    filtera = 1.0;

  filtera = (filtera - 1.0) / filtera; // get filter ready for use

  return filtera * last_filtered_mv  + (1.0-filtera) * unfiltered_mv; // apply the EWMA filter
}

/**
 * @brief Calculate PV
 * @param sensor_index - Index of sensor to calculate PV for
 * @return PV for sensor at sensor_index
 */
double calc_pv(unsigned int sensor_index)
{
  return (param.sensor_raw[sensor_index] - param.sensor_offset[sensor_index]) / param.sensor_slope[sensor_index];
}

/**
 * @brief Update the main window sensor raw signal display boxes
 * @param none
 * @return none
 */
void update_sensor_raw_display(void)
{
  for(unsigned int channel = 0; channel < param.num_sensors; ++channel) {
    set_data_display_label_sensor_raw(channel, dtos(param.sensor_raw[channel], param.raw_disp_precision).c_str());
  }
}

/**
 * @brief Update the main window sensor PV display boxes
 * @param none
 * @return none
 */
void update_pv_display(void)
{
  for(unsigned int channel = 0; channel < param.num_sensors; ++channel) {
    set_data_display_label_sensor_pv(channel, dtos(param.sensor_pv[channel], param.pv_disp_precision).c_str());
  }
}

/**
 * @brief Set channel alarm display
 * @param ch - Channel to set alarm display for
 * @param alarm - Alarm_e alarm state to set display to
 * @return none 
 */
void set_ch_alarm_display(unsigned int ch, Alarm_e alarm)
{
  switch(alarm) {
  case ALARM_NONE:
    // text color black
    set_data_display_label_sensor_raw_fg_color(ch, DEFAULT_NO_ALARM_FG_COLOR16_R,
                                               DEFAULT_NO_ALARM_FG_COLOR16_G,
                                               DEFAULT_NO_ALARM_FG_COLOR16_B);
    set_data_display_label_sensor_pv_fg_color(ch, DEFAULT_NO_ALARM_FG_COLOR16_R,
                                              DEFAULT_NO_ALARM_FG_COLOR16_G,
                                              DEFAULT_NO_ALARM_FG_COLOR16_B);
    
    // highlight color white
    set_data_display_label_sensor_raw_highlight_color(ch, DEFAULT_NO_ALARM_BG_COLOR16_R,
                                                      DEFAULT_NO_ALARM_BG_COLOR16_G,
                                                      DEFAULT_NO_ALARM_BG_COLOR16_B);
    set_data_display_label_sensor_pv_highlight_color(ch, DEFAULT_NO_ALARM_BG_COLOR16_R,
                                                     DEFAULT_NO_ALARM_BG_COLOR16_G,
                                                     DEFAULT_NO_ALARM_BG_COLOR16_B);
    // bg color white
    set_data_display_label_sensor_raw_bg_color(ch, DEFAULT_NO_ALARM_BG_COLOR16_R,
                                               DEFAULT_NO_ALARM_BG_COLOR16_G,
                                               DEFAULT_NO_ALARM_BG_COLOR16_B);
    set_data_display_label_sensor_pv_bg_color(ch, DEFAULT_NO_ALARM_BG_COLOR16_R,
                                              DEFAULT_NO_ALARM_BG_COLOR16_G,
                                              DEFAULT_NO_ALARM_BG_COLOR16_B);
    // font weight normal
    set_data_display_label_sensor_raw_font_weight(ch, PANGO_WEIGHT_NORMAL);
    set_data_display_label_sensor_pv_font_weight(ch, PANGO_WEIGHT_NORMAL);
    break;
  case ALARM_LOW:
    // text color black
    set_data_display_label_sensor_raw_fg_color(ch, DEFAULT_LO_ALARM_FG_COLOR16_R,
                                               DEFAULT_LO_ALARM_FG_COLOR16_G,
                                               DEFAULT_LO_ALARM_FG_COLOR16_B);
    set_data_display_label_sensor_pv_fg_color(ch, DEFAULT_LO_ALARM_FG_COLOR16_R,
                                              DEFAULT_LO_ALARM_FG_COLOR16_G,
                                              DEFAULT_LO_ALARM_FG_COLOR16_B);
    // highlight color yel
    set_data_display_label_sensor_raw_highlight_color(ch, DEFAULT_LO_ALARM_BG_COLOR16_R,
                                                      DEFAULT_LO_ALARM_BG_COLOR16_G,
                                                      DEFAULT_LO_ALARM_BG_COLOR16_B);
    set_data_display_label_sensor_pv_highlight_color(ch, DEFAULT_LO_ALARM_BG_COLOR16_R,
                                                     DEFAULT_LO_ALARM_BG_COLOR16_G,
                                                     DEFAULT_LO_ALARM_BG_COLOR16_B);
    // bg color yel
    set_data_display_label_sensor_raw_bg_color(ch, DEFAULT_LO_ALARM_BG_COLOR16_R,
                                               DEFAULT_LO_ALARM_BG_COLOR16_G,
                                               DEFAULT_LO_ALARM_BG_COLOR16_B);
    set_data_display_label_sensor_pv_bg_color(ch, DEFAULT_LO_ALARM_BG_COLOR16_R,
                                              DEFAULT_LO_ALARM_BG_COLOR16_G,
                                              DEFAULT_LO_ALARM_BG_COLOR16_B);
    // font weight bold
    set_data_display_label_sensor_raw_font_weight(ch, PANGO_WEIGHT_BOLD);
    set_data_display_label_sensor_pv_font_weight(ch, PANGO_WEIGHT_BOLD);
    break;
  case ALARM_MID:
    // text color black
    set_data_display_label_sensor_raw_fg_color(ch, DEFAULT_MID_ALARM_FG_COLOR16_R,
                                               DEFAULT_MID_ALARM_FG_COLOR16_G,
                                               DEFAULT_MID_ALARM_FG_COLOR16_B);
    set_data_display_label_sensor_pv_fg_color(ch, DEFAULT_MID_ALARM_FG_COLOR16_R,
                                              DEFAULT_MID_ALARM_FG_COLOR16_G,
                                              DEFAULT_MID_ALARM_FG_COLOR16_B);
    // highlight color orange
    set_data_display_label_sensor_raw_highlight_color(ch, DEFAULT_MID_ALARM_BG_COLOR16_R,
                                                      DEFAULT_MID_ALARM_BG_COLOR16_G,
                                                      DEFAULT_MID_ALARM_BG_COLOR16_B);
    set_data_display_label_sensor_pv_highlight_color(ch, DEFAULT_MID_ALARM_BG_COLOR16_R,
                                                     DEFAULT_MID_ALARM_BG_COLOR16_G,
                                                     DEFAULT_MID_ALARM_BG_COLOR16_B);
    // bg color orange
    set_data_display_label_sensor_raw_bg_color(ch, DEFAULT_MID_ALARM_BG_COLOR16_R,
                                               DEFAULT_MID_ALARM_BG_COLOR16_G,
                                               DEFAULT_MID_ALARM_BG_COLOR16_B);
    set_data_display_label_sensor_pv_bg_color(ch, DEFAULT_MID_ALARM_BG_COLOR16_R,
                                              DEFAULT_MID_ALARM_BG_COLOR16_G,
                                              DEFAULT_MID_ALARM_BG_COLOR16_B);
    // font weight bold
    set_data_display_label_sensor_raw_font_weight(ch, PANGO_WEIGHT_BOLD);
    set_data_display_label_sensor_pv_font_weight(ch, PANGO_WEIGHT_BOLD);
    break;
  case ALARM_HI:
    // text color white
    set_data_display_label_sensor_raw_fg_color(ch, DEFAULT_HI_ALARM_FG_COLOR16_R,
                                               DEFAULT_HI_ALARM_FG_COLOR16_G,
                                               DEFAULT_HI_ALARM_FG_COLOR16_B);
    set_data_display_label_sensor_pv_fg_color(ch, DEFAULT_HI_ALARM_FG_COLOR16_R,
                                              DEFAULT_HI_ALARM_FG_COLOR16_G,
                                              DEFAULT_HI_ALARM_FG_COLOR16_B);
    // highlight color red
    set_data_display_label_sensor_raw_highlight_color(ch, DEFAULT_HI_ALARM_BG_COLOR16_R,
                                                      DEFAULT_HI_ALARM_BG_COLOR16_G,
                                                      DEFAULT_HI_ALARM_BG_COLOR16_B);
    set_data_display_label_sensor_pv_highlight_color(ch, DEFAULT_HI_ALARM_BG_COLOR16_R,
                                                     DEFAULT_HI_ALARM_BG_COLOR16_G,
                                                     DEFAULT_HI_ALARM_BG_COLOR16_B);
    // bg color red
    set_data_display_label_sensor_raw_bg_color(ch, DEFAULT_HI_ALARM_BG_COLOR16_R,
                                               DEFAULT_HI_ALARM_BG_COLOR16_G,
                                               DEFAULT_HI_ALARM_BG_COLOR16_B);
    set_data_display_label_sensor_pv_bg_color(ch, DEFAULT_HI_ALARM_BG_COLOR16_R,
                                              DEFAULT_HI_ALARM_BG_COLOR16_G,
                                              DEFAULT_HI_ALARM_BG_COLOR16_B);
    // font weight bold
    set_data_display_label_sensor_raw_font_weight(ch, PANGO_WEIGHT_BOLD);
    set_data_display_label_sensor_pv_font_weight(ch, PANGO_WEIGHT_BOLD);
    break;
  case ALARM_FAULT: // we haven't decided what fault conditions are yet..
    // text color white
    set_data_display_label_sensor_raw_fg_color(ch, DEFAULT_FLT_ALARM_FG_COLOR16_R,
                                               DEFAULT_FLT_ALARM_FG_COLOR16_G,
                                               DEFAULT_FLT_ALARM_FG_COLOR16_B);
    set_data_display_label_sensor_pv_fg_color(ch, DEFAULT_FLT_ALARM_FG_COLOR16_R,
                                              DEFAULT_FLT_ALARM_FG_COLOR16_G,
                                              DEFAULT_FLT_ALARM_FG_COLOR16_B);
    // highlight color red
    set_data_display_label_sensor_raw_highlight_color(ch, DEFAULT_FLT_ALARM_BG_COLOR16_R,
                                                      DEFAULT_FLT_ALARM_BG_COLOR16_G,
                                                      DEFAULT_FLT_ALARM_BG_COLOR16_B);
    set_data_display_label_sensor_pv_highlight_color(ch, DEFAULT_FLT_ALARM_BG_COLOR16_R,
                                                     DEFAULT_FLT_ALARM_BG_COLOR16_G,
                                                     DEFAULT_FLT_ALARM_BG_COLOR16_B);
    // bg color red
    set_data_display_label_sensor_raw_bg_color(ch, DEFAULT_FLT_ALARM_BG_COLOR16_R,
                                               DEFAULT_FLT_ALARM_BG_COLOR16_G,
                                               DEFAULT_FLT_ALARM_BG_COLOR16_B);
    set_data_display_label_sensor_pv_bg_color(ch, DEFAULT_FLT_ALARM_BG_COLOR16_R,
                                              DEFAULT_FLT_ALARM_BG_COLOR16_G,
                                              DEFAULT_FLT_ALARM_BG_COLOR16_B);
    // font weight bold
    set_data_display_label_sensor_raw_font_weight(ch, PANGO_WEIGHT_BOLD);
    set_data_display_label_sensor_pv_font_weight(ch, PANGO_WEIGHT_BOLD);
    break;
  default:
    std::cerr << "set_ch_alarm_display(): default case entered. alarm value: " << alarm << "\n";
    break;
  }
}

/**
 * @brief Read and average ADC output values and update the analog input 
 * structure
 * @param none
 * @return none 
 */
void update_alarm_display(void)
{
  for(unsigned int i = 0; i < param.num_sensors; i++) {
    if(param.channel_status[i].bits.fault) {
      set_ch_alarm_display(i, ALARM_FAULT);
    } else if(param.channel_status[i].bits.hi_alarm) {
      set_ch_alarm_display(i, ALARM_HI);
    } else if(param.channel_status[i].bits.mid_alarm) {
      set_ch_alarm_display(i, ALARM_MID);
    } else if(param.channel_status[i].bits.low_alarm) {
      set_ch_alarm_display(i, ALARM_LOW);
    } else {
      set_ch_alarm_display(i, ALARM_NONE);
    }
  }
}

/**
 * @brief Read and average ADC output values and update the analog input 
 * structure
 * @param none
 * @return none 
 */
void update_analog_input(void)
{
  for (unsigned int channel = 0; channel < param.num_sensors; ++channel) {
    analog_input[channel].adc_val = analogRead(MCP3008_0_PINBASE + channel);
    analog_input[channel].accum_adc += analog_input[channel].adc_val;
    if(param.adc_num_samples == ++analog_input[channel].sample_num) {
      analog_input[channel].unfiltered_mv = 
        calc_mv(analog_input[channel].accum_adc / param.adc_num_samples);
      analog_input[channel].filtered_mv = 
        calc_filtered_mv(analog_input[channel].unfiltered_mv, analog_input[channel].filtered_mv);
      analog_input[channel].accum_adc = 0.0;
      analog_input[channel].sample_num = 0;
    }
  }
}

/**
 * @brief Update param structure live data
 * @param none
 * @return none 
 */
void update_param(void)
{
  for (unsigned int channel = 0; channel < param.num_sensors; ++channel) {
    param.sensor_raw[channel] = analog_input[channel].filtered_mv;
    param.sensor_pv[channel] = calc_pv(channel);
  }
}

/**
 * @brief Print sensor calibration values
 * @param sensor_num - the sensor number to print calibration data for
 * @return none
 */
void print_sensor_cal_values(unsigned int sensor_num)
{
  std::cout << "Sensor " << sensor_num << " offset: " << param.sensor_offset[sensor_num] << '\n';
  std::cout << "Sensor " << sensor_num << " slope: " << param.sensor_slope[sensor_num] << '\n';
}

/**
 * @brief PV alarm handler
 * @param none
 * @return none
 */
 void alarm_handler(void)
 {
   for(unsigned int i = 0; i < param.num_sensors; i++) {
     if(param.low_alarm <= param.sensor_pv[i]) {
       param.channel_status[i].bits.low_alarm = 1;
     } else {
       param.channel_status[i].bits.low_alarm = 0;
     }
     if(param.mid_alarm <= param.sensor_pv[i]) {
       param.channel_status[i].bits.mid_alarm = 1;
     } else {
       param.channel_status[i].bits.mid_alarm = 0;
     }
     if(param.hi_alarm <= param.sensor_pv[i]) {
       param.channel_status[i].bits.hi_alarm = 1;
     } else {
       param.channel_status[i].bits.hi_alarm = 0;
     }
   }
 }

/**
 * @brief Command handler for UI button presses
 * @param none
 * @return none
 */
void cmd_handler(void)
{
  bool save_cal = false;
  unsigned int cal_sensor_index = 0;

  for(unsigned int channel = 0; channel < param.num_sensors; ++channel) {

    if(button_zero_pressed_ack(channel)) {
      cal_sensor_index = channel;
      force_zero(cal_sensor_index, true);
      save_cal = true;
      break;
    } else if(button_span_pressed_ack(channel)) {
      cal_sensor_index = channel;
      force_span(cal_sensor_index);
      save_cal = true;
      break;
    }
  }

  if(save_cal){
    if(0 != save_calibration_data(cal_sensor_index)) {
      std::cerr << "Error saving calibration data" << '\n';
    }
  }
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
/**
 * @brief Program timebase handler
 * @param signum - Signal number
 * @param *info - Signal information
 * @param *context - Signal context
 * @return none
 * @note This function is called every 100 milliseconds
 */
void timebase_handler(int signum, siginfo_t *info, void *context)
{
  static int count = 0;

  // .1 second tasks
  
  update_analog_input();
  cmd_handler();

  // 1 second tasks
  if (10 == count) {
    update_param();

    // Main window update
    alarm_handler();
    update_alarm_display();
    update_sensor_raw_display();
    update_pv_display();

    ++seconds_count;
    one_sec_toggle ^= 1;
    count = 0;
  }
  ++count;
}

/**
 * @brief Start program timebase timer
 * @param val - none
 * @return 0 if success, -1 if error
 * @note To delete timer:
 *   if (timer_delete(timebase_timerid) == -1) {
 *    perror("timer_delete error");
 *    exit(EXIT_FAILURE);
 *   }
 */
int timebase_start(void)
{
  struct sigaction sa;
  struct sigevent sev;
  struct itimerspec its;

  // Install timebase_handler as the signal handler for SIGALRM
  sa.sa_flags = SA_SIGINFO;
  sa.sa_sigaction = timebase_handler;
  sigemptyset(&sa.sa_mask);
  if (sigaction(SIGALRM, &sa, NULL) == -1) {
    std::cerr << "sigaction error" << '\n';
    return -1;
  }

  // Create the timer
  sev.sigev_notify = SIGEV_SIGNAL;
  sev.sigev_signo = SIGALRM;
  sev.sigev_value.sival_ptr = &timebase_timerid;
  if (timer_create(CLOCK_REALTIME, &sev, &timebase_timerid) == -1) {
    std::cerr << "timer_create error" << '\n';
    return -1;
  }

  // Start the timer with a period of 100 milliseconds
  its.it_value.tv_sec = 0;
  its.it_value.tv_nsec = 100000000; // 100 milliseconds
  its.it_interval.tv_sec = 0;
  its.it_interval.tv_nsec = 100000000; // 100 milliseconds
  if (timer_settime(timebase_timerid, 0, &its, NULL) == -1) {
    std::cerr << "timer_settime error" << '\n';
    return -1;
  }
  return 0;
}
#pragma GCC diagnostic pop // End disable warning for unused parameter
/**
 End of file
 */
