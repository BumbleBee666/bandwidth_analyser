#include <stdio.h>
#include <gtk/gtk.h>
#include <string>
#include <memory>
#include <iostream>


#include "MyGTKWindow.h"
#include "BandwidthData.h"

#define PROJECT_NAME "bandwidth_analyser"

std::shared_ptr<BandwidthData> bandwidthData;
std::shared_ptr<MyGTKWindow> myWindow[10];
std::string filepath;

static void activate(GtkApplication *app/*, gpointer user_data*/)
{
    bandwidthData = std::shared_ptr<BandwidthData>(new BandwidthData(filepath));
    
    for (auto i=0;i<1;i++)
    {
        myWindow[i] = std::shared_ptr<MyGTKWindow>(new MyGTKWindow(app, std::shared_ptr<const BandwidthData>(bandwidthData)));
        bandwidthData->RegisterListener(myWindow[i]);
    }
}

static int command_line (GApplication* app, GApplicationCommandLine *cmdline)
{
    gchar **argv;
    gint argc;

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

    g_application_activate(app);
    
    return 0;
}

int main(int argc, char **argv) 
{
    GtkApplication *app;
    int status;

    app = gtk_application_new (NULL, G_APPLICATION_HANDLES_COMMAND_LINE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    g_signal_connect(app, "command-line", G_CALLBACK(command_line), NULL);

    status = g_application_run (G_APPLICATION(app), argc, argv);

    g_object_unref (app);
  
    return status;
}
