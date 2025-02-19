/**
 * @file imain-window.h
 * @brief Main Window interface header
 * @par (C) 2025 Keith Hall
 * Programmer interacts with the Main Window through this interface code
 */
#ifndef IMAIN_WINDOW_H
#define IMAIN_WINDOW_H

#include <stdbool.h>
#include <gtk/gtk.h> // needed for PangoWeight enum
#include "config.h"
#include "gui.h"
#include "attrib.h"

#define MAX_COL_HEADER_STR_SZ   32u
#define MAX_DATA_DISP_SZ 32u

typedef struct _Data_Display_Label_t
{
  char label_text[MAX_DATA_DISP_SZ] = {0};
  bool update_text = false;
  Color16_t foreground_color = {0,0,0};
  bool update_foreground_color = false;
  Color16_t background_color = {0,0,0};
  bool update_background_color = false;
  Color16_t text_highlight_color = {0,0,0};
  bool update_text_highlight_color = false;
  Font_Attrib_t font_attrib = {NULL,PANGO_WEIGHT_NORMAL,0};
}Data_Display_Label_t;

typedef struct _IMainWindow
{
  unsigned int num_sensors = 0;
  Data_Display_Label_t tab_label_page[MAX_NUM_PAGES]; // // TODO: rename this to tab_page_label
  Data_Display_Label_t col_header_sensor_raw[MAX_NUM_DATA_COL];
  Data_Display_Label_t col_header_sensor_pv[MAX_NUM_DATA_COL];
  Data_Display_Label_t channel_label[MAX_SENSORS];
  Data_Display_Label_t data_display_label_sensor_raw[MAX_SENSORS];
  Data_Display_Label_t data_display_label_sensor_pv[MAX_SENSORS];
  
  bool button0; // bottom button

  bool button_zero_pressed[MAX_SENSORS];
  bool button_span_pressed[MAX_SENSORS];

} IMainWindow;

const char *get_tab_page_label(unsigned int index);
const char *get_channel_label(unsigned int index);
const char *get_col_header_sensor_raw(unsigned int index);
const char *get_col_header_sensor_pv(unsigned int index);
const char *get_data_display_label_sensor_raw(unsigned int index);
const char *get_data_display_label_sensor_pv(unsigned int index);

void set_tab_page_label(unsigned int index, const char *text);
void set_channel_label(unsigned int index, const char *text);
void set_col_header_sensor_raw(unsigned int index, const char *text);
void set_col_header_sensor_pv(unsigned int index, const char *text);
void set_data_display_label_sensor_raw(unsigned int index, const char *text);
void set_data_display_label_sensor_pv(unsigned int index, const char *text);


void set_channel_label_fg_color(unsigned int index, guint16 red, guint16 green,
                                guint16 blue);
void set_channel_label_highlight_color(unsigned int index, guint16 red, 
                                       guint16 green, guint16 blue);
void set_data_display_label_sensor_raw_fg_color(unsigned int index, guint16 red,
                                                guint16 green, guint16 blue);
void set_data_display_label_sensor_raw_bg_color(unsigned int index, guint16 red,
                                                guint16 green, guint16 blue);
void set_data_display_label_sensor_raw_highlight_color(unsigned int index,
                                                       guint16 red, guint16 green,
                                                       guint16 blue);
void set_data_display_label_sensor_pv_fg_color(unsigned int index, guint16 red,
                                               guint16 green, guint16 blue);
void set_data_display_label_sensor_pv_bg_color(unsigned int index, guint16 red,
                                               guint16 green, guint16 blue);
void set_data_display_label_sensor_pv_highlight_color(unsigned int index,
                                                      guint16 red, guint16 green,
                                                      guint16 blue);

void set_channel_label_font_family(unsigned int index, const char *font_family);
void set_data_display_label_sensor_raw_font_family(unsigned int index,
                                                   const char *font_family);
void set_data_display_label_sensor_pv_font_family(unsigned int index,
                                                  const char *font_family);
void set_channel_label_font_size(unsigned int index, int size);
void set_data_display_label_sensor_raw_font_size(unsigned int index, gint size);
void set_data_display_label_sensor_pv_font_size(unsigned int index, gint size);
void set_tab_label_page_font_weight(unsigned int index, PangoWeight pango_weight);
void set_channel_label_font_weight(unsigned int index, PangoWeight pango_weight);
void set_data_display_label_sensor_raw_font_weight(unsigned int index,
                                                   PangoWeight pango_weight);
void set_data_display_label_sensor_pv_font_weight(unsigned int index,
                                                  PangoWeight pango_weight);

bool get_update_text_tab_label_page_ack(unsigned int index);
bool get_update_text_channel_label_ack(unsigned int index);
bool get_update_text_raw_ack(unsigned int index);
bool get_update_text_pv_ack(unsigned int index);
bool get_update_foreground_color_channel_label_ack(unsigned int index);
bool get_update_highlight_color_channel_label_ack(unsigned int index);
bool get_update_foreground_color_raw_ack(unsigned int index);
bool get_update_background_color_raw_ack(unsigned int index);
bool get_update_highlight_color_raw_ack(unsigned int index);
bool get_update_foreground_color_pv_ack(unsigned int index);
bool get_update_background_color_pv_ack(unsigned int index);
bool get_update_highlight_color_pv_ack(unsigned int index);

bool get_update_font_attrib_channel_label_ack(unsigned int index);
bool get_update_font_attrib_raw_ack(unsigned int index);
bool get_update_font_attrib_pv_ack(unsigned int index);

bool button_zero_pressed(unsigned int index);
bool button_span_pressed(unsigned int index);
bool button_zero_pressed_ack(unsigned int index);
bool button_span_pressed_ack(unsigned int index);

int init_imain_window(unsigned int num_sensor_ch);
void cleanup_imain_window(void);
#endif

/**
 * End of file
 */
