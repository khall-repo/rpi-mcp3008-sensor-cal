/**
 * @file config.cpp
 * @brief Code to read an ini style configuration file, without using someone
 * else's library
 * @par (C) 2025 Keith Hall
 */
#ifndef CONFIG_FILE_H
#define CONFIG_FILE_H

#include <fstream>
#include <map>

class Config_Section {
  friend class Config_File;
  public:
    Config_Section();
    ~Config_Section();
    int read_config_section(void);
    std::string get_config_section_value(std::string key);
  private:
    
    int num_elements; // I think I only need this for debug
    std::ifstream config_section;
    std::map<std::string, std::string> config_value_map;
};

class Config_File {
  public:
    Config_File();
    ~Config_File();

    int read_config_file(void);
    std::string get_config_value(std::string section, std::string key);
    int set_config_value(std::string section, std::string key, std::string value, bool add_if_not_found);
  private:
    std::ifstream config_file;
    int num_sections; // I think I only need this for debug
    std::map<std::string, std::map<std::string, std::string>> config_section_map;
};

#endif