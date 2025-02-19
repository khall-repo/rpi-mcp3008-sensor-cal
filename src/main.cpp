/**
 * @file main.cpp
 * @brief Main program file for the MCP3008 sensor calibration
 * @par (C) 2025 Keith Hall
 * Output PV (Present Value) scale is 0 to 100 of arbitrary units we made up,
 * we've decided on calling them ST units (IYKYK).
 */
#include <iostream>
#include <wiringPi.h>
#include <mcp3004.h>
#include "config.h"
#include "config-file.h"
#include "struct.h"
#include "cal.h"
#include "gui.h"
#include "control.h"

extern Param_t param; // Import from struct.cpp

Config_File *config_file = NULL; // TODO: move this to config-file.cpp

/**
 * @brief Main program entry point
 * @return 0 if success, -1 if error
 */
int main(int argc, char *argv[])
{
  (void)argc; // not used
  (void)argv; // not used
  int status;

  // Read the config file
  config_file = new Config_File();
  if(0 != config_file->read_config_file()) {
    std::cerr << "Error reading config file" << '\n';
    return -1;
  }
  if(0 != init_param()) {
    std::cerr << "Failed to initialize param structure" << '\n';
    return -1; 
  }
  std::cerr << "Num ch: " << param.num_sensors << '\n';

  if(-1 == wiringPiSetup()) {
    std::cerr << "Failed to initialize WiringPi" << '\n';
    return -1;
  }

  if(-1 == mcp3004Setup(MCP3008_0_PINBASE, MCP3008_0_SPI_CHAN)) {
    std::cerr << "Failed to initialize MCP3008_0" << '\n';
    return -1;
  }
  if(MCP3008_NUM_CH < param.num_sensors) {
    if(-1 == mcp3004Setup(MCP3008_1_PINBASE, MCP3008_1_SPI_CHAN)) {
      std::cerr << "Failed to initialize MCP3008_1" << '\n';
      return -1;
    }
  }

  if(0 != timebase_start()) {
    std::cerr << "Failed to timebase" << '\n';
    return -1;
  }

  if(0 != init_imain_window(param.num_sensors)) {
    std::cerr << "Failed to initialize imain-window" << '\n';
    return -1;
  }

  status = run_gui_application(argc, argv);
  cleanup_imain_window();
  return status;
}
