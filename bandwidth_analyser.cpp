#include <stdio.h>
#include <gtk/gtk.h>
#include <string>

#include "MyGTKWindow.h"

#define PROJECT_NAME "bandwidth_analyser"

const MyGTKWindow *myWindow[10];

static std::string filepath = "";

static void activate(GtkApplication* app/*, gpointer user_data*/)
{
    for (auto i=0;i<1;i++)
        myWindow[i] = new MyGTKWindow(app, filepath);
}

static int
command_line (GApplication            *application,
              GApplicationCommandLine *cmdline)
{
  gchar **argv;
  gint argc;
  gint i;

  argv = g_application_command_line_get_arguments (cmdline, &argc);

  if (argc != 2)
  {
    g_application_command_line_print (cmdline, "Application requires path to data files\n");
  }
  else
  {
      filepath = argv[1];
  }

  g_strfreev (argv);

    g_application_activate(application);
    
    return 0;
}

int main(int argc, char **argv) 
{
  GtkApplication *app;

  try
  {
    app = gtk_application_new(NULL, G_APPLICATION_HANDLES_COMMAND_LINE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    g_signal_connect(app, "command-line", G_CALLBACK(command_line), NULL);
    
    gtk_main_iteration_do(FALSE);
    g_application_run(G_APPLICATION(app), argc, argv);
  }
  catch (...)
  {
      g_object_unref(app);
  }
  
  return 0;
}
