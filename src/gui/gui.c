/**
 * @file gui.cpp
 * @brief GUI application functions
 * @date 01-29-2025
 */
#include <gtk/gtk.h>
//#include "config.h"
#include "main-window.h"
#include "sub-window.h"
//#include "struct.h" // for param.num_sensors
#include "gui.h"

//xtern Param_t param; // Import from struct.c, for param.num_sensors

int run_gui_application(int argc, char *argv[])
{
  GtkApplication *main_app;
  int status;

  // Disable accessibility so we don't get a bunch of warnings about it.
  g_setenv("GTK_A11Y", "none", TRUE);

  // On the RPi OS, it complains that G_APPLICATION_FLAGS_NONE is deprecated,
  // and that we should use instead use G_APPLICATION_DEFAULT_FLAGS.
  // But on Linux mint AMD64, that doesn't even exist.
  main_app = gtk_application_new("com.makeithappen.sensorcal", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect(main_app, "activate", G_CALLBACK(activate_main_window_cb), NULL);
  status = g_application_run(G_APPLICATION(main_app), argc, argv);

  g_object_unref(main_app);
  return status;
}
