#include "config.h"
#include "sub-window.h"

typedef struct _SubWindow
{
  GtkApplicationWindow parent_instance;

  GtkWidget *sw_col_header_label0;
  GtkWidget *sw_data_display_label0;
  GtkWidget *sw_button0;

} SubWindow;

typedef struct _SubWindowClass
{
  GtkApplicationWindowClass parent_class;
} SubWindowClass;

#define SUB_WINDOW_TYPE (sub_window_get_type())
#define SUB_WINDOW(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), SUB_WINDOW_TYPE, SubWindow))

// Register the SubWindow type with the GObject type system
G_DEFINE_TYPE(SubWindow, sub_window, GTK_TYPE_APPLICATION_WINDOW)

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

static void sub_window_class_init(SubWindowClass *klass)
{
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);
  GError *error = NULL;
  char *contents;
  gsize length;

  // Load the SubWindow UI template from the file.
  if (g_file_get_contents(PATH_TO_SUB_WIN_RESOURCE, &contents, &length, &error)) {
    GBytes *template_bytes = g_bytes_new_take(contents, length);
    gtk_widget_class_set_template(widget_class, template_bytes);
    g_bytes_unref(template_bytes);

    gtk_widget_class_bind_template_child(widget_class, SubWindow, sw_col_header_label0);
    gtk_widget_class_bind_template_child(widget_class, SubWindow, sw_data_display_label0);
    gtk_widget_class_bind_template_child(widget_class, SubWindow, sw_button0);
  } else {
    g_error("Failed to load SubWindow template: %s\n", error->message);
    g_error_free(error);
  }
}

static void sub_window_init(SubWindow *self)
{
  // Initialize the template
  gtk_widget_init_template(GTK_WIDGET(self));
}

static void sub_window_destroy_cb(GtkWidget *widget, gpointer data)
{
  g_print("SubWindow has been closed.\n");
  
  // Clean up stuff here if you need to
  // See main-window.c for example of something you'd clean up.
}

static void sw_button0_clicked_cb(GtkButton *button, SubWindow *self)
{
  gtk_label_set_text(GTK_LABEL(self->sw_data_display_label0), "sw_button0 was clicked!");
}

GtkWidget* sub_window_new(void)
{
  SubWindow *window = (SubWindow*)g_object_new(SUB_WINDOW_TYPE, NULL);

  // Store the SubWindow instance, maybe you want to use it later.
  g_object_set_data(G_OBJECT(window), "sub-window-instance", window);

  // Connect the sw_button0 signal its the callback function
  g_signal_connect(window->sw_button0, "clicked", G_CALLBACK(sw_button0_clicked_cb), window);
  // Connect the destroy signal to detect when the window is closed
  g_signal_connect(window, "destroy", G_CALLBACK(sub_window_destroy_cb), NULL);

  return (GtkWidget*)window;
}
#pragma GCC diagnostic pop // End disable warning for unused parameter