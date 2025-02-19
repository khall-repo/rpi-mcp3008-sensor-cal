/**
 * @file config.h
 * @brief Configuration header for the MCP3008 sensor calibration program
 * @par (C) 2025 Keith Hall
 */
#ifndef CONFIG_H
#define CONFIG_H

#include <glib.h>

// Path to window resource files. It's relative to the current working
// directory. The bin should be ran from the project root directory.
#define PATH_TO_MAIN_WIN_RESOURCE "./src/gui/resource/main-window.ui"
#define PATH_TO_SUB_WIN_RESOURCE "./src/gui/resource/sub-window.ui"

#define MAX_CONFIG_SECTIONS 64u
#define CONFIG_INI_FILE_PATH "config.ini"

#define MAX_SENSORS 16
#define TEST_CAL_RAW_VALUES_FILE_PATH     "test-cal-raw-values.txt"
#define TEST_READING_RAW_VALUES_FILE_PATH "test-reading-raw-values.txt"

#define MCP3008_NUM_CH      8
#define MCP3008_0_PINBASE   100
#define MCP3008_1_PINBASE   108
#define MCP3008_0_SPI_CHAN  0
#define MCP3008_1_SPI_CHAN  1

#define VREF    (double)3300.0
#define ADC_MAX (double)1024.0

#define FILTER            (double) 15.0 // normal filter
#define BIG_JUMP          (double) 50.0 // if the raw value jumps by this amount
                                        // then use this filter number (i.e.
                                        // relax the filtering for big jumps)
#define MID_JUMP          (double) 25.0
#define SMALL_JUMP        (double) 10.0
#define MAX_FILTER  (unsigned int) 25
#define MIN_FILTER  (unsigned int) 1

#define MAX_FONT_FAMILY_LEN 64u // should be more than enough


#endif
