/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   MyGTKWindow.cpp
 * Author: mark
 * 
 * Created on 20 April 2021, 11:22
 */

#include <gtk/gtk.h>
#include <math.h>
#include <ctime>
#include <iostream>
#include <string>
#include <gtk-2.0/gtk/gtkwidget.h>
#include <gtk-2.0/gtk/gtkdrawingarea.h>

#include "MyGTKWindow.h"
#include "BandwidthDay.h"

int MyGTKWindow::width = 1460;
int MyGTKWindow::height = 700;

MyGTKWindow::MyGTKWindow(GtkApplication* app) :
surface(NULL)
{
    m_bandwidth.LoadData("202105");
    
    window = gtk_application_window_new (app);
    
    gtk_window_set_title(GTK_WINDOW (window), "Welcome to GNOME");
    gtk_window_set_default_size(GTK_WINDOW (window), width, height);
    
    box1 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add (GTK_CONTAINER (window), box1);
    
    menu = gtk_menu_bar_new ();
    gtk_box_pack_start (GTK_BOX (box1), menu, FALSE, FALSE, 0);

    GtkWidget *item = gtk_menu_item_new_with_label ("Load Data File");
    gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);

    GtkWidget *subMenu = gtk_menu_new ();
    gtk_menu_item_set_submenu( GTK_MENU_ITEM( item ), subMenu );

    GtkWidget *open_item = gtk_menu_item_new_with_label( "Open");
    GtkWidget *close_item = gtk_menu_item_new_with_label( "Close");
    GtkWidget *separator1 = gtk_separator_menu_item_new();
    GtkWidget *exit_item = gtk_menu_item_new_with_label( "Exit");
    gtk_menu_shell_append( GTK_MENU_SHELL(subMenu), open_item );
//    g_signal_connect (open_item, "select", G_CALLBACK (LoadDataFile), this);
    gtk_menu_shell_append( GTK_MENU_SHELL(subMenu), close_item );
    gtk_menu_shell_append( GTK_MENU_SHELL(subMenu), separator1 );
    gtk_menu_shell_append( GTK_MENU_SHELL(subMenu), exit_item );    

    frame = gtk_frame_new (NULL);
    gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_IN);
    gtk_container_add (GTK_CONTAINER (box1), frame);

    drawing_area = gtk_drawing_area_new ();
    
    /* set a minimum size */
    gtk_widget_set_size_request (drawing_area, width, height);

    gtk_container_add (GTK_CONTAINER (frame), drawing_area);

    /* Signals used to handle the backing surface */
    g_signal_connect (drawing_area,"configure-event", G_CALLBACK (Configure), this);
    g_signal_connect (drawing_area, "draw", G_CALLBACK (Draw), this);
    
    gtk_widget_show_all(window);
}

MyGTKWindow::MyGTKWindow(const MyGTKWindow& orig)
{
}

MyGTKWindow::~MyGTKWindow()
{
}

void MyGTKWindow::Redraw()
{
    gtk_widget_queue_draw(drawing_area);
}

void MyGTKWindow::DrawSurface (MyGTKWindow* myWindow)
{
    cairo_t *cr;

    cr = cairo_create (myWindow->surface);

    // Clear background to white.
    cairo_set_source_rgb (cr, 1, 1, 1);
    cairo_paint (cr);

    cairo_set_line_width(cr, 1);
    cairo_set_source_rgb (cr, 0, 0, 0);
    
    int time_shift = 10;
    int time_scale = 5;
    
    int bandwidth_shift = 650;
    int bandwidth_scale = 8;
    
    int x = 10;
    for (std::map<int, double>::const_iterator it = myWindow->m_bandwidth.Data().begin() ; it != myWindow->m_bandwidth.Data().end() ; ++it)
    {
        int bandwidth = bandwidth_shift - (*it).second * bandwidth_scale;
        if ( it == myWindow->m_bandwidth.Data().begin())
        {
            cairo_move_to (cr, x, bandwidth);
        }
        else
        {
            cairo_line_to (cr, x, bandwidth);
        }
        x += time_scale;
    }

    cairo_stroke (cr);
    
    // Draw horizontal gridlines.
    for (int i = 0 ; i < 9 ; ++i)
    {
        cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
        cairo_move_to (cr, time_shift, bandwidth_shift - i * 10 * bandwidth_scale + 10);
        char buffer[50];
        sprintf(buffer, "%d", i*10);
        std::string strBandwidth(buffer);
        cairo_show_text (cr, strBandwidth.c_str());
        
        cairo_set_source_rgb (cr, 0.8, 0.8, 0.8);
        cairo_move_to (cr, time_shift,                        bandwidth_shift - i * 10 * bandwidth_scale);
        cairo_line_to (cr, time_shift + 12 * 24 * time_scale, bandwidth_shift - i * 10 * bandwidth_scale);
        cairo_stroke (cr);
    }
    
    // Draw vertical gridlines.
    for (int i = 0 ; i < 25 ; ++i)
    {
        cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
        cairo_move_to (cr, i * time_scale * 12, bandwidth_shift + 10);
        char buffer[50];
        sprintf(buffer, "%d:00", i);
        std::string strBandwidth(buffer);
        cairo_show_text (cr, strBandwidth.c_str());
        
        cairo_set_source_rgb (cr, 0.8, 0.8, 0.8);
        cairo_move_to (cr, time_shift + i * time_scale * 12, bandwidth_shift);
        cairo_line_to (cr, time_shift + i * time_scale * 12, bandwidth_shift - 10 * 8 * bandwidth_scale);
        cairo_stroke (cr);
    }
    
    cairo_destroy (cr);
}

gboolean MyGTKWindow::Configure (GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
    MyGTKWindow* myWindow = (MyGTKWindow*)data;

    myWindow->widget = widget;
    
    if (myWindow->surface)
        cairo_surface_destroy (myWindow->surface);

    myWindow->surface = gdk_window_create_similar_surface (
            gtk_widget_get_window (widget),
            CAIRO_CONTENT_COLOR,
            gtk_widget_get_allocated_width (widget),
            gtk_widget_get_allocated_height (widget));

    DrawSurface (myWindow);

    /* We've handled the configure event, no need for further processing. */
    return TRUE;
}

gboolean MyGTKWindow::Draw (GtkWidget *widget, cairo_t *cr, gpointer data)
{
    MyGTKWindow* myWindow = (MyGTKWindow*)data;

    // Paint our window using the source surface.
    cairo_set_source_surface (cr, myWindow->surface, 0, 0);
    cairo_paint (cr);

    return FALSE;
}