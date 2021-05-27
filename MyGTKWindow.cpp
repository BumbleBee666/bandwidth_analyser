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
#include <deque>

#include "MyGTKCalendarWindow.h"
#include "BandwidthDay.h"

#include "MyGTKWindow.h"

int MyGTKWindow::width = 1460;
int MyGTKWindow::height = 700;

MyGTKWindow::MyGTKWindow(GtkApplication* app, const std::string filepath) :
app(app),
calendar(NULL),
surface(NULL),
selected_day("")
{
    std::set<std::string> months;
    BandwidthMonth::GetFileMonths(filepath, months);
    
    for (std::set<std::string>::const_iterator it = months.begin() ; it != months.end() ; ++it)
    {
        BandwidthMonth *bandwidthMonth = new BandwidthMonth();
        bandwidthMonth->LoadData(filepath, *it);
        m_bandwidthMonths.push_back(bandwidthMonth);
    }
    
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

    GtkWidget *byMonth_item = gtk_menu_item_new_with_label( "By Month" );
    gtk_menu_shell_append( GTK_MENU_SHELL(subMenu), byMonth_item );
//    g_signal_connect (open_item, "select", G_CALLBACK (LoadDataFile), this);
    
    GtkWidget *byMonth_subMenu = gtk_menu_new ();
    gtk_menu_item_set_submenu( GTK_MENU_ITEM( byMonth_item ), byMonth_subMenu );
    
    for (std::set<std::string>::const_iterator it = months.begin() ; it != months.end() ; ++it)
    {
        GtkWidget *month_item = gtk_check_menu_item_new_with_label((*it).c_str());
        gtk_check_menu_item_set_active((GtkCheckMenuItem*)month_item, true);
        g_signal_connect (month_item, "toggled", G_CALLBACK (Toggled), this);
        gtk_menu_shell_append( GTK_MENU_SHELL(byMonth_subMenu), month_item );
        
        m_menuItems.push_back((GtkCheckMenuItem*)month_item);
    }

    GtkWidget *byDay_item = gtk_menu_item_new_with_label( "By Day");
    gtk_menu_shell_append( GTK_MENU_SHELL(subMenu), byDay_item );
    g_signal_connect (byDay_item, "activate", G_CALLBACK (SelectDay), this);

    GtkWidget *separator1 = gtk_separator_menu_item_new();
    gtk_menu_shell_append( GTK_MENU_SHELL(subMenu), separator1 );
    
    GtkWidget *exit_item = gtk_menu_item_new_with_label( "Exit");
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

void MyGTKWindow::Toggled (GtkCheckMenuItem *menuitem, gpointer data)
{    
    MyGTKWindow* myWindow = (MyGTKWindow*)data;
    
    myWindow->selected_day = "";
}

void MyGTKWindow::SelectDay (GtkMenuItem *menuitem, gpointer data)
{    
    MyGTKWindow* myWindow = (MyGTKWindow*)data;

    myWindow->dialog = gtk_dialog_new();
    
    gtk_window_set_position(GTK_WINDOW(myWindow->dialog), GTK_WIN_POS_CENTER_ON_PARENT);

    GtkWidget *calendar = gtk_calendar_new ();
    gtk_calendar_mark_day (GTK_CALENDAR (calendar), 19);	
    
    GtkWidget *content_area = gtk_dialog_get_content_area (GTK_DIALOG(myWindow->dialog));
    gtk_box_pack_start(GTK_BOX(content_area), calendar, TRUE, TRUE, 5);

    g_signal_connect (calendar, "day-selected-double-click", G_CALLBACK (DayDoubleClicked), data);
    
    gtk_widget_show_all(myWindow->dialog);
    
    gtk_dialog_run(GTK_DIALOG(myWindow->dialog));
    
    gtk_widget_destroy(myWindow->dialog);
    
    DrawSurface(myWindow);
}

void MyGTKWindow::DayDoubleClicked (GtkCalendar *calendar, gpointer data)
{
    MyGTKWindow* myWindow = (MyGTKWindow*)data;
    
    guint year;
    guint month;
    guint day;
    
    gtk_calendar_get_date (calendar, &year, &month, &day);
    
    char buffer[50];
    sprintf(buffer, "%04d%02d%02d", year, month+1, day);
    myWindow->selected_day = buffer;
    
    gtk_dialog_response(GTK_DIALOG(myWindow->dialog), NULL);
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
    
    int sample_rate_in_minutes = 5;
    
    if (!myWindow->selected_day.empty())
    {
        // We need to show the chart for a specific day.
        // Look to see if we have the appropriate month record.
        for (std::vector<BandwidthMonth*>::const_iterator month_it = myWindow->m_bandwidthMonths.begin() ; month_it != myWindow->m_bandwidthMonths.end() ; ++month_it)
        {
            const std::string& strMonth = (*month_it)->Month();
            if (strMonth.substr(0,6).compare(myWindow->selected_day.substr(0,6)) == 0)
            {
                // We found the month.
                std::map<std::string, BandwidthDay*>::const_iterator day_it = (*month_it)->Days().find(myWindow->selected_day);
                if (day_it != (*month_it)->Days().end())
                {
                    cairo_set_source_rgb (cr, 0, 0, 0);
                    int x = 0;
                    for (std::map<std::string, BandwidthDataPoint*>::const_iterator it_dp = day_it->second->DataPoints().begin() ; it_dp != day_it->second->DataPoints().end() ; ++it_dp)
                    {
                        int bandwidth = bandwidth_shift - it_dp->second->Bandwidth() * bandwidth_scale;
                        if (x == 0)
                        {
                            x = time_shift;
                            cairo_move_to (cr, x, bandwidth);
                        }
                        else
                        {
                            int hour = atoi(it_dp->first.substr(0,2).c_str());
                            int minute = atoi(it_dp->first.substr(2,2).c_str());
                            int elapsed_minutes = hour * 60 + minute;
                            x = time_shift + elapsed_minutes / sample_rate_in_minutes * time_scale;
                            cairo_line_to (cr, x, bandwidth);
                        }
                    }
                    cairo_stroke (cr);
                }
            }
        }
    }
    else
    {
        for (std::vector<BandwidthMonth*>::const_iterator month_it = myWindow->m_bandwidthMonths.begin() ; month_it != myWindow->m_bandwidthMonths.end() ; ++month_it)
        {
            std::string strMonth = (*month_it)->Month();

            // Should we draw this month?
            for (std::vector<GtkCheckMenuItem*>::const_iterator menu_it = myWindow->m_menuItems.begin() ; menu_it != myWindow->m_menuItems.end() ; ++menu_it)
            {
                if (strMonth.compare(gtk_menu_item_get_label((GtkMenuItem*)*menu_it)) == 0 && gtk_check_menu_item_get_active((GtkCheckMenuItem*)*menu_it))
                {
                    cairo_set_source_rgba (cr, 1, 0, 0, 0.5);
                    int x = 0;
                    std::deque<int> high_av;
                    std::deque<int> low_av;
                    int count = 0;
                    for (std::map<std::string, BandwidthStatistics*>::const_iterator it_stat = (*month_it)->Statistics().begin() ; it_stat != (*month_it)->Statistics().end() ; ++it_stat)
                    {
                        int bandwidth_high = bandwidth_shift - it_stat->second->High() * bandwidth_scale;
                        int bandwidth_low = bandwidth_shift - it_stat->second->Low() * bandwidth_scale;

                        if (count < 5)
                        {
                            count++;
                        }
                        else
                        {
                            high_av.pop_front();
                            low_av.pop_front();
                        }

                        high_av.push_back(bandwidth_high);
                        low_av.push_back(bandwidth_low);

                        bandwidth_high = 0;
                        for (std::deque<int>::const_iterator it_high = high_av.begin() ; it_high != high_av.end() ; ++it_high)
                            bandwidth_high += *it_high;
                        bandwidth_high = bandwidth_high / count;

                        bandwidth_low = 0;
                        for (std::deque<int>::const_iterator it_low = low_av.begin() ; it_low != low_av.end() ; ++it_low)
                            bandwidth_low += *it_low;
                        bandwidth_low = bandwidth_low / count;

                        int hour = atoi(it_stat->first.substr(0,2).c_str());
                        int minute = atoi(it_stat->first.substr(2,2).c_str());
                        int elapsed_minutes = hour * 60 + minute;
                        x = time_shift + elapsed_minutes / sample_rate_in_minutes * time_scale;
                        cairo_move_to (cr, x, bandwidth_high);
                        cairo_line_to (cr, x, bandwidth_low);
                        cairo_stroke (cr);
                    }

                    cairo_set_source_rgb (cr, 0, 0, 0);
                    x = 0;
                    for (std::map<std::string, BandwidthStatistics*>::const_iterator it_stat = (*month_it)->Statistics().begin() ; it_stat != (*month_it)->Statistics().end() ; ++it_stat)
                    {
                        int bandwidth = bandwidth_shift - it_stat->second->Average() * bandwidth_scale;
                        if (x == 0)
                        {
                            x = time_shift;
                            cairo_move_to (cr, x, bandwidth);
                        }
                        else
                        {
                            int hour = atoi(it_stat->first.substr(0,2).c_str());
                            int minute = atoi(it_stat->first.substr(2,2).c_str());
                            int elapsed_minutes = hour * 60 + minute;
                            x = time_shift + elapsed_minutes / sample_rate_in_minutes * time_scale;
                            cairo_line_to (cr, x, bandwidth);
                        }
                    }
                    cairo_stroke (cr);
                }
            }
        }
    }
    
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