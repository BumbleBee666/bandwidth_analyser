#include <stdio.h>
#include <gtk/gtk.h>

#include "MyGTKWindow.h"

#define PROJECT_NAME "bandwidth_analyser"

const MyGTKWindow *myWindow[10];

static void activate(GtkApplication* app/*, gpointer user_data*/)
{
    for (auto i=0;i<1;i++)
        myWindow[i] = new MyGTKWindow(app);
}

int main(int argc, char **argv) 
{
  GtkApplication *app;

  try
  {
    app = gtk_application_new(NULL, G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    
    gtk_main_iteration_do( FALSE);
    g_application_run(G_APPLICATION(app), argc, argv);
  }
  catch (...)
  {
      g_object_unref(app);
  }
  
  return 0;
}
