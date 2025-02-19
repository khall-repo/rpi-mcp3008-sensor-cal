/**
 * @file main-window.c
 * @brief Main Window file for the MCP3008 sensor calibration project
 * @par (C) 2025 Keith Hall
 * Read the following page at url to understand some of what is going on..
 * The GLib Object System
 * https://docs.gtk.org/gobject/concepts.html
 * Also, the package "devhelp" is extremely helpful (apt install devhelp).
 * Devhelp will appear in your program menu under programming when installed.
 * It is a collection of manuals for Gtk, Glib, Pango, Cairo, etc..
 */
#include "config.h"
#include "imain-window.h"
#include "main-window.h"
#include "sub-window.h"
#include "attrib.h"
#include "util.h"

#define CH_LIMIT_PAGE 8u // max num channels per page

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

extern IMainWindow imain_window;

// This is the handle for the idle function that updates the main window 
static guint window_update_fn_source_id = 0;

typedef struct _MainWindow
{
  GtkApplicationWindow parent_instance;
  // Tab widgets
  GtkWidget *notebook;
  GtkWidget *tab_label_page[MAX_NUM_PAGES]; // TODO: rename this to tab_page_label

  // Column labels
  GtkWidget *col_header_sensor_raw[MAX_NUM_DATA_COL];
  GtkWidget *col_header_sensor_pv[MAX_NUM_DATA_COL];

  // Channel labels
  GtkWidget *channel_label[MAX_SENSORS];

  // Text boxes for display data
  GtkWidget *data_display_label_sensor_raw[MAX_SENSORS];
  GtkWidget *data_display_label_sensor_pv[MAX_SENSORS];

  // Drawing areas for display data
  GtkWidget *drawing_area_sensor_raw[MAX_SENSORS];
  GtkWidget *drawing_area_sensor_pv[MAX_SENSORS];
  
  //GtkWidget *button0; // bottom button
  //GtkWidget *button1; // bottom button
  
  GtkWidget *button_zero[MAX_SENSORS];
  GtkWidget *button_span[MAX_SENSORS];

} MainWindow;

typedef struct _MainWindowClass
{
  GtkApplicationWindowClass parent_class;
} MainWindowClass;



// Register the MainWindow type with the GObject type system.
// This macro will actually take the string "main_window" and converts it to
// "main_window_app_class_init" and "main_app_window_init", and stores those
// names internally.
G_DEFINE_TYPE(MainWindow, main_window, GTK_TYPE_APPLICATION_WINDOW)
// moved this down here to make it clearer that below line is related to the
// line above.
#define MAIN_WINDOW_TYPE (main_window_get_type())

#define MAIN_WINDOW(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), MAIN_WINDOW_TYPE, MainWindow))




// Custom log handler to detect "Gtk-CRITICAL" errors and shut down the program
// Because boy... I've caused many of those...
void custom_log_handler(const gchar *log_domain, GLogLevelFlags log_level,
                        const gchar *message, gpointer user_data)
{
  if (log_level & G_LOG_LEVEL_CRITICAL)
  {
    g_printerr("Critical error detected: %s\n", message);
    exit(EXIT_FAILURE);
  }
}

// Draw function for the GtkDrawing areas
static void draw_gtkdrawingarea_fill_color(GtkDrawingArea *area,
  cairo_t *cr,
  int             width,  // unused
  int             height, // unused
  gpointer        data)
{
  Color16_t *color = (Color16_t *)data;
  double red, green, blue;
  color16_to_rgbd(*color, &red, &green, &blue);

  width = gtk_widget_get_width(GTK_WIDGET (area));
  height = gtk_widget_get_height(GTK_WIDGET (area));

  // Set color (RGB values from 0 to 1)
  cairo_set_source_rgb(cr, red, green, blue);

  // Draw rectangle covering whole area
  cairo_rectangle(cr, 0, 0, width, height);

  cairo_fill (cr);
}




static void main_window_class_init(MainWindowClass *klass)
{
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);
  GError *error = NULL;
  char *contents;
  gsize length;

  // Load the template from the file and bind tags with MainWindow private data.
  if (g_file_get_contents(PATH_TO_MAIN_WIN_RESOURCE, &contents, &length, &error)) {
    GBytes *template_bytes = g_bytes_new_take(contents, length);
    gtk_widget_class_set_template(widget_class, template_bytes);
    g_bytes_unref(template_bytes);

    /*Binding MainWindow struct members to the corresponding tag in .ui file*/
    // bind tab stuff
    gtk_widget_class_bind_template_child(widget_class, MainWindow, notebook);

    for(unsigned int i = 0; i < MAX_NUM_PAGES; i++) {
      char widget_name[32];
      snprintf(widget_name, sizeof(widget_name), "tab_label_page%d", i);
      gtk_widget_class_bind_template_child_full(widget_class,
                                                widget_name,
                                                FALSE,
                                                offsetof(MainWindow, tab_label_page) + i * sizeof(GtkWidget *));
    }

    // bind col header labels
    for(unsigned int i = 0; i < MAX_NUM_DATA_COL; i++) {
      char widget_name[32];
      snprintf(widget_name, sizeof(widget_name), "col_header_sensor_raw%d", i);
      gtk_widget_class_bind_template_child_full(widget_class,
                                                widget_name,
                                                FALSE,
                                                offsetof(MainWindow, col_header_sensor_raw) + i * sizeof(GtkWidget *));
    }
    for(unsigned int i = 0; i < MAX_NUM_DATA_COL; i++) {
      char widget_name[32];
      snprintf(widget_name, sizeof(widget_name), "col_header_sensor_pv%d", i);
      gtk_widget_class_bind_template_child_full(widget_class,
                                                widget_name,
                                                FALSE,
                                                offsetof(MainWindow, col_header_sensor_pv) + i * sizeof(GtkWidget *));
    }
    
    
    // bind ch labels
    for(unsigned int i = 0; i < imain_window.num_sensors; i++) {
      char widget_name[32];
      snprintf(widget_name, sizeof(widget_name), "channel_label%d", i);
      gtk_widget_class_bind_template_child_full(widget_class,
                                                widget_name,
                                                FALSE,
                                                offsetof(MainWindow, channel_label) + i * sizeof(GtkWidget *));
    }
    
    // bind sensor raw data display labels
    for(unsigned int i = 0; i < imain_window.num_sensors; i++) {
      char widget_name[64];
      snprintf(widget_name, sizeof(widget_name), "data_display_label_sensor_raw%d", i);
      gtk_widget_class_bind_template_child_full(widget_class,
                                                widget_name,
                                                FALSE,
                                                offsetof(MainWindow, data_display_label_sensor_raw) + i * sizeof(GtkWidget *));
    }
    
    // bind sensor pv data display labels
    for(unsigned int i = 0; i < imain_window.num_sensors; i++) {
      char widget_name[64];
      snprintf(widget_name, sizeof(widget_name), "data_display_label_sensor_pv%d", i);
      gtk_widget_class_bind_template_child_full(widget_class,
                                                widget_name,
                                                FALSE,
                                                offsetof(MainWindow, data_display_label_sensor_pv) + i * sizeof(GtkWidget *));
    }

    // bind sensor raw data display drawing area
    for(unsigned int i = 0; i < imain_window.num_sensors; i++) {
      char widget_name[32];
      snprintf(widget_name, sizeof(widget_name), "drawing_area_sensor_raw%d", i);
      gtk_widget_class_bind_template_child_full(widget_class,
                                                widget_name,
                                                FALSE,
                                                offsetof(MainWindow, drawing_area_sensor_raw) + i * sizeof(GtkWidget *));
    }
    
    // bind sensor pv data display drawing area
    for(unsigned int i = 0; i < imain_window.num_sensors; i++) {
      char widget_name[32];
      snprintf(widget_name, sizeof(widget_name), "drawing_area_sensor_pv%d", i);
      gtk_widget_class_bind_template_child_full(widget_class, widget_name, FALSE, offsetof(MainWindow, drawing_area_sensor_pv) + i * sizeof(GtkWidget *));
    }

    // bind sensor zero buttons
    for(unsigned int i = 0; i < imain_window.num_sensors; i++) {
      char widget_name[16];
      snprintf(widget_name, sizeof(widget_name), "button_zero%d", i);
      gtk_widget_class_bind_template_child_full(widget_class,
                                                widget_name,
                                                FALSE,
                                                offsetof(MainWindow, button_zero) + i * sizeof(GtkWidget *));
    }
    // bind sensor span buttons
    for(unsigned int i = 0; i < imain_window.num_sensors; i++) {
      char widget_name[16];
      snprintf(widget_name, sizeof(widget_name), "button_span%d", i);
      gtk_widget_class_bind_template_child_full(widget_class,
                                                widget_name,
                                                FALSE,
                                                offsetof(MainWindow, button_span) + i * sizeof(GtkWidget *));
    }
    // bind the buttons with no purpose
    //gtk_widget_class_bind_template_child(widget_class, MainWindow, button0);
    //gtk_widget_class_bind_template_child(widget_class, MainWindow, button1);
  } else {
    g_error("Failed to load MainWindow template: %s\n", error->message);
    g_error_free(error);
    exit(EXIT_FAILURE);
  }
}

static void main_window_init(MainWindow *self)
{
  gtk_widget_init_template(GTK_WIDGET(self));

  // Set the tab label text
  //GtkWidget *label_page0 = gtk_label_new(imain_window.tab_label_page[TAB_PAGE_0].label_text);
  self->tab_label_page[TAB_PAGE_0] = gtk_label_new(imain_window.tab_label_page[TAB_PAGE_0].label_text);
  

  gtk_notebook_set_tab_label(GTK_NOTEBOOK(self->notebook),
                             gtk_notebook_get_nth_page(GTK_NOTEBOOK(self->notebook), 0),
                             self->tab_label_page[TAB_PAGE_0]);

  if(CH_LIMIT_PAGE < imain_window.num_sensors) {
    self->tab_label_page[TAB_PAGE_1] = gtk_label_new(imain_window.tab_label_page[TAB_PAGE_1].label_text);
    gtk_notebook_set_tab_label(GTK_NOTEBOOK(self->notebook),
                               gtk_notebook_get_nth_page(GTK_NOTEBOOK(self->notebook), 1),
                               self->tab_label_page[TAB_PAGE_1]);
  } else {
    gtk_notebook_remove_page(GTK_NOTEBOOK(self->notebook), -1); // -1 removes
  }                                                             // last page
  
}

static void main_window_destroy_cb(GtkWidget *widget, gpointer data)
{
  g_print("MainWindow has been closed.\n");
  // Remove the idle function from the main loop
  if (window_update_fn_source_id != 0) {
    g_source_remove(window_update_fn_source_id);
    window_update_fn_source_id = 0;
  }
}




static void set_label_font_attribs(GtkLabel *label, Font_Attrib_t *attrib)
{
  PangoContext *p_pcontext = gtk_widget_get_pango_context(GTK_WIDGET(label));
  PangoFontDescription *p_font_desc = pango_font_description_new();
  
  if(p_pcontext == NULL) {
    g_printerr("set_label_font_attribs(): Failed to create PangoContext\n");
    return;
  }
  if(p_font_desc == NULL) {
    g_printerr("set_label_font_attribs(): Failed to create PangoFontDescription\n");
    return;
  }
  if(NULL == attrib) {
    g_printerr("set_label_font_attribs(): NULL Font_Attrib_t\n");
    return;
  }

  pango_font_description_set_family(p_font_desc, attrib->font_family);
  pango_font_description_set_size(p_font_desc, attrib->size * PANGO_SCALE);
  pango_font_description_set_weight(p_font_desc, attrib->weight);
  pango_context_set_font_description(p_pcontext, p_font_desc);
  
  pango_font_description_free(p_font_desc);
}

static void set_label_color(GtkLabel *label, Color16_t foreground, Color16_t background)
{
  PangoAttrList *attr_list = pango_attr_list_new();
  PangoAttribute *attr = pango_attr_foreground_new(foreground.red, foreground.green, foreground.blue);
  pango_attr_list_insert(attr_list, attr);
  attr = pango_attr_background_new(background.red, background.green, background.blue);
  pango_attr_list_insert(attr_list, attr);
  gtk_label_set_attributes(label, attr_list);
  pango_attr_list_unref(attr_list);
}

// This is where the data display labels in the main window are updated,
// based on the data in the IMainWindow struct.
static gboolean update_main_window(MainWindow *self)
{
  // update tab page label font attrib, text, color
  for (unsigned int i = 0; i < MAX_NUM_PAGES; i++) {
    if(get_update_text_tab_label_page_ack(i)) {
      //gtk_label_set_text(GTK_LABEL(self->tab_label_page[i]), imain_window.tab_label_page[i].label_text);
      gtk_notebook_set_tab_label(GTK_NOTEBOOK(self->notebook),
                                 gtk_notebook_get_nth_page(GTK_NOTEBOOK(self->notebook), i),
                                 self->tab_label_page[i]);
    }
  }
  
  // update channel labels
  for (unsigned int i = 0; i < imain_window.num_sensors; i++) {
    if(get_update_font_attrib_channel_label_ack(i)) {
      set_label_font_attribs(GTK_LABEL(self->channel_label[i]), &imain_window.channel_label[i].font_attrib);
    }
    if(get_update_text_channel_label_ack(i)) {
      gtk_label_set_text(GTK_LABEL(self->channel_label[i]), imain_window.channel_label[i].label_text);
    }
    if(get_update_foreground_color_channel_label_ack(i) || get_update_highlight_color_channel_label_ack(i)) {
      set_label_color(GTK_LABEL(self->channel_label[i]), 
                                imain_window.channel_label[i].foreground_color,
                                imain_window.channel_label[i].text_highlight_color);
    }
  }
  
  // update sensor raw font attrib
  for (unsigned int i = 0; i < imain_window.num_sensors; i++) {
    if(get_update_font_attrib_raw_ack(i)) {
      set_label_font_attribs(GTK_LABEL(self->data_display_label_sensor_raw[i]), &imain_window.data_display_label_sensor_raw[i].font_attrib);
    }
    if(get_update_text_raw_ack(i)) {
      gtk_label_set_text(GTK_LABEL(self->data_display_label_sensor_raw[i]), imain_window.data_display_label_sensor_raw[i].label_text);
    }
    if(get_update_foreground_color_raw_ack(i) || get_update_highlight_color_raw_ack(i)) {
      set_label_color(GTK_LABEL(self->data_display_label_sensor_raw[i]), 
                                imain_window.data_display_label_sensor_raw[i].foreground_color,
                                imain_window.data_display_label_sensor_raw[i].text_highlight_color);
    }
    if(get_update_background_color_raw_ack(i)) {
      gtk_widget_queue_draw(GTK_WIDGET(self->drawing_area_sensor_raw[i]));
    }
  }

  // update sensor pv font attrib
  for (unsigned int i = 0; i < imain_window.num_sensors; i++) {
    if(get_update_font_attrib_pv_ack(i)) {
      set_label_font_attribs(GTK_LABEL(self->data_display_label_sensor_pv[i]), &imain_window.data_display_label_sensor_pv[i].font_attrib);
    }
    if(get_update_text_pv_ack(i)) {
      gtk_label_set_text(GTK_LABEL(self->data_display_label_sensor_pv[i]), imain_window.data_display_label_sensor_pv[i].label_text);
    }
    if(get_update_foreground_color_pv_ack(i) || get_update_highlight_color_pv_ack(i)) {
      set_label_color(GTK_LABEL(self->data_display_label_sensor_pv[i]), 
                                imain_window.data_display_label_sensor_pv[i].foreground_color,
                                imain_window.data_display_label_sensor_pv[i].text_highlight_color);
    }
    if(get_update_background_color_pv_ack(i)) {
      gtk_widget_queue_draw(GTK_WIDGET(self->drawing_area_sensor_pv[i]));
    }
  }

  // Return TRUE to keep the idle function running
  return TRUE;
  // Or return FALSE to stop the idle function
  //return FALSE;
}




/*static void button0_clicked_cb(GtkButton *button, MainWindow *self)
{
  GtkWidget *sub_window;

  // Create a new test SubWindow
  sub_window = sub_window_new();
  if (sub_window == NULL) {
      g_printerr("Failed to create SubWindow\n");
      return;
  }

  // Show all widgets in the SubWindow
  gtk_widget_show(sub_window);

  gtk_label_set_text(GTK_LABEL(self->data_display_label_sensor_raw[0]), "Button0 was clicked!");
}

static void button1_clicked_cb(GtkButton *button, MainWindow *self)
{
  // useless button at the moment
}*/

static void button_zero_clicked_cb(GtkButton *button, MainWindow *self)
{
  for(unsigned int i = 0; i < imain_window.num_sensors; i++) {
    if(self->button_zero[i] == GTK_WIDGET(button)) {
      imain_window.button_zero_pressed[i] = true; // notify the window interface
      return;
    }
  }
  g_printerr("button_zero_clicked_cb(): Failed to get button index\n");
}

static void button_span_clicked_cb(GtkButton *button, MainWindow *self)
{
  for(unsigned int i = 0; i < imain_window.num_sensors; i++) {
    if(self->button_span[i] == GTK_WIDGET(button)) {
      imain_window.button_span_pressed[i] = true; // notify the window interface
      return;
    }
  }
  g_printerr("button_span_clicked_cb(): Failed to get button index\n");
}




// TODO: See if this can be re-static'd
// was named activate_cb and was static
void activate_main_window_cb(GtkApplication *app, gpointer user_data)
{
  MainWindow *window = (MainWindow*)g_object_new(main_window_get_type(), "application", app, NULL);
  
  // Store the MainWindow instance in the GtkApplicationWindow
  // Then you can use it for something later on. Look at the commented out
  // code at the bottom of this file.
  g_object_set_data(G_OBJECT(window), "main-window-instance", window);

  // Connect the buttons with no real purpose signal to their callbacks
  //g_signal_connect(window->button0, "clicked", G_CALLBACK(button0_clicked_cb), window);
  //g_signal_connect(window->button1, "clicked", G_CALLBACK(button1_clicked_cb), window);

  // Connect the zero and span buttons to their callbacks
  for(unsigned int i = 0; i < imain_window.num_sensors; i++) {
    g_signal_connect(window->button_zero[i], "clicked", G_CALLBACK(button_zero_clicked_cb), window);
  }
  for(unsigned int i = 0; i < imain_window.num_sensors; i++) {
    g_signal_connect(window->button_span[i], "clicked", G_CALLBACK(button_span_clicked_cb), window);
  }

  // Connect the destroy signal to detect when the window is closed
  g_signal_connect(window, "destroy", G_CALLBACK(main_window_destroy_cb), NULL);

  gtk_window_present(GTK_WINDOW(window));

  // Add idle function to the main loop
  window_update_fn_source_id = g_idle_add((GSourceFunc)update_main_window, window);
  // TODO: Try this to reduce cpu utilization:
  // Alternatively, you could use g_timeout_add() to update the window at a
  // regular interval. The function would be called every 1000 milliseconds.
  //window_update_fn_source_id = g_timeout_add(1000, 
  //                                           (GSourceFunc)update_main_window,
  //                                           window);

  // Set the drawing area function for the data display label cells for filling
  // the box with color
  for(unsigned int i = 0; i < imain_window.num_sensors; i++) {
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(window->drawing_area_sensor_raw[i]), // which widget
                                                   draw_gtkdrawingarea_fill_color, // the drawing fn to use
                                                   (gpointer)&imain_window.data_display_label_sensor_raw[i].background_color, // where the color is stored
                                                   NULL); // GDestroyNotify
  }
  for(unsigned int i = 0; i < imain_window.num_sensors; i++) {
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(window->drawing_area_sensor_pv[i]),
                                                   draw_gtkdrawingarea_fill_color,
                                                   (gpointer)&imain_window.data_display_label_sensor_pv[i].background_color,
                                                   NULL);
  }
  
  // Set the custom log handler for the GTK log domain
  // This isn't necessary but it helps with debugging.
  g_log_set_handler("Gtk", G_LOG_LEVEL_CRITICAL, custom_log_handler, NULL);
}

#pragma GCC diagnostic pop // End disable warning for unused parameter

// Here is some scrap code.. It works so I'm leaving it here for my own
// reference.
/*MainWindow *_get_main_window_instance(void)
{
  GtkApplicationWindow *window = GTK_APPLICATION_WINDOW(gtk_application_get_active_window(main_app));
  return MAIN_WINDOW(g_object_get_data(G_OBJECT(window), "main-window-instance"));
}

// Function to retrieve the text from data_display_label_sensor_raw0
const char *_get_data_display_label0_text(MainWindow *window)
{
  return gtk_label_get_text(GTK_LABEL(window->data_display_label_sensor_raw0));
}

// Example usage of get_data_display_label0_text
const char *get_data_display_label0_text(void)
{
  MainWindow *window = _get_main_window_instance();
  if (window) {
    const char *text = _get_data_display_label0_text(window);
    return text;
  } else {
    g_print("Failed to retrieve MainWindow instance\n");
  }
  return "\0";
}

int set_data_display_label0_text(const char *text)
{
  MainWindow *window = _get_main_window_instance();
  if (window) {
    gtk_label_set_text(GTK_LABEL(window->data_display_label_sensor_raw0), text);
    return 0;
  } else {
    g_print("Failed to retrieve MainWindow instance\n");
  }
  return -1;
}

// Function to retrieve the text from a GtkLabel
const char *_get_main_window_gtklabel_text(GtkWidget *label)
{
  return gtk_label_get_text(GTK_LABEL(label));
}
*/
/*static void bind_template_child(GtkWidgetClass *widget_class, const char *widget_name, size_t offset)
{
  GParamSpec *pspec = g_object_class_find_property(G_OBJECT_CLASS(widget_class), widget_name);
  if (pspec) {
    gtk_widget_class_bind_template_child_full(widget_class, widget_name, FALSE, offset);
  } else {
    g_warning("Property '%s' not found in class '%s'", widget_name, G_OBJECT_CLASS_NAME(widget_class));
  }
}*/
