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
#include <gtk-3.0/gtk/gtkwidget.h>
#include <gtk-3.0/gtk/gtkdrawingarea.h>
#include <deque>
#include <set>
#include <algorithm>

#include "MyGTKCalendarWindow.h"
#include "BandwidthData.h"
#include "BandwidthDay.h"
#include "BandwidthStatistics.h"

#include "MyGTKWindow.h"

const int initial_width = 1460;
const int initial_height = 700;

const std::string strFile = "File";
const std::string strExit= "Exit";

const std::string strSelectData = "Select Data";
const std::string strByMonth = "By Month";
const std::string strByDay= "By Day";

const int averaging_range = 10;

MyGTKWindow::MyGTKWindow(GtkApplication *app, std::shared_ptr<const BandwidthData> bandwidthData) : 
        m_app(app),
        m_bandwidthData(bandwidthData),
        m_width(initial_width),
        m_height(initial_height),
        m_slider(NULL),
        m_surface(NULL)
{
    m_window = gtk_application_window_new (m_app);

    gtk_window_set_title (GTK_WINDOW (m_window), "Welcome to GNOME");
    gtk_window_set_default_size (GTK_WINDOW (m_window), m_width, m_height);
    
    // Create a box, and add it to the window.
    m_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add (GTK_CONTAINER (m_window), m_box);

    // Create a menu bar, and add it to the box.
    m_menubar = gtk_menu_bar_new ();
    gtk_box_pack_start (GTK_BOX (m_box), m_menubar, FALSE, FALSE, 0);

    // Create the File menu.
    m_fileItem = gtk_menu_item_new_with_label (strFile.c_str());
    gtk_menu_shell_append (GTK_MENU_SHELL (m_menubar), m_fileItem);
    m_fileMenu = gtk_menu_new ();
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (m_fileItem), m_fileMenu);
    m_exitItem = gtk_menu_item_new_with_label(strExit.c_str());
    gtk_menu_shell_append (GTK_MENU_SHELL (m_fileMenu), m_exitItem );

    // Create the Select Data menu.
    m_selectDataItem = gtk_menu_item_new_with_label (strSelectData.c_str());
    gtk_menu_shell_append (GTK_MENU_SHELL (m_menubar), m_selectDataItem);
    m_selectDataMenu = gtk_menu_new ();
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (m_selectDataItem), m_selectDataMenu);
    m_selectDataByMonthItem = gtk_menu_item_new_with_label (strByMonth.c_str());
    gtk_menu_shell_append (GTK_MENU_SHELL (m_selectDataMenu), m_selectDataByMonthItem);
    m_selectDataByMonthMenu = gtk_menu_new ();
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (m_selectDataByMonthItem), m_selectDataByMonthMenu);
    CreateSelectDataByMonthMenu();
    m_selectDataByDayItem = gtk_menu_item_new_with_label (strByDay.c_str());
    gtk_menu_shell_append (GTK_MENU_SHELL (m_selectDataMenu), m_selectDataByDayItem);
    g_signal_connect (m_selectDataByDayItem, "activate", G_CALLBACK (SelectDay), this);

    // Create a frame, and add it to the box.
    m_frame = gtk_frame_new (NULL);
    gtk_frame_set_shadow_type (GTK_FRAME (m_frame), GTK_SHADOW_IN);
    gtk_container_add (GTK_CONTAINER (m_box), m_frame);

    // Create a drawing area, and add it to the frame.
    m_drawingArea = gtk_drawing_area_new ();
    gtk_widget_set_size_request (m_drawingArea, m_width, m_height);
    gtk_container_add (GTK_CONTAINER (m_frame), m_drawingArea);
    g_signal_connect (m_drawingArea, "configure-event", G_CALLBACK (Configure), this);
    g_signal_connect (m_drawingArea, "draw", G_CALLBACK (Draw), this);

    // Create a slider, and add it to the frame.
    double max = m_bandwidthData->GetNoOfDays() <= 1 ? 1.0 : m_bandwidthData->GetNoOfDays() - 1;
    m_slider = gtk_scale_new_with_range (GTK_ORIENTATION_HORIZONTAL, 0.0, max, 1.0);
    gtk_container_add (GTK_CONTAINER (m_box), m_slider);
    g_signal_connect (m_slider, "format-value", G_CALLBACK (FormatValue), this);
    g_signal_connect (m_slider, "value-changed", G_CALLBACK (ValueChanged), this);
    
    gtk_widget_show_all (m_window);
}

void MyGTKWindow::CreateSelectDataByMonthMenu()
{
    std::unique_ptr<std::set<std::string>> months = m_bandwidthData->GetMonths();

    std::set<std::string> newMonths;
    std::set_symmetric_difference(months->begin(), months->end(), m_months.begin(), m_months.end(), std::inserter(newMonths, newMonths.end()));
    
    for (auto const& month : newMonths)
    {
        GtkWidget *monthItem = gtk_check_menu_item_new_with_label (month.c_str());
        gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (monthItem), true);
        gtk_menu_shell_append (GTK_MENU_SHELL (m_selectDataByMonthMenu), monthItem);
        g_signal_connect (monthItem, "toggled", G_CALLBACK (Toggled), this);

        m_months.insert(month);
        m_selectDataByMonthMenuMonthItems.push_back (monthItem);
    }
}

void MyGTKWindow::ValueChanged (GtkRange *range, gpointer data)
{
    MyGTKWindow *myWindow = (MyGTKWindow*)data;

    // Get the current value.
    int value = gtk_range_get_value (range);

    int startValue = value - averaging_range;
    if (startValue < 0)
    {
        startValue = 0;
    }
   
    int endValue = value + averaging_range;
    if (endValue >= myWindow->m_bandwidthData->GetNoOfDays())
    {
        endValue = myWindow->m_bandwidthData->GetNoOfDays() - 1;
    }

    int i = 0;
    for (auto const& day : myWindow->m_bandwidthData->GetDays())
    {
        if (i == startValue)
        {
            myWindow->m_startDay = day.second->Date();
        }
        if (i == endValue)
        {
            myWindow->m_endDay = day.second->Date();
        }
        i++;
    }

    myWindow->m_selectedDay = "";

    gtk_widget_queue_draw (myWindow->m_drawingArea);
}

void MyGTKWindow::BandwidthUpdated()
{
    CreateSelectDataByMonthMenu();
    double max = m_bandwidthData->GetNoOfDays() == 0 ? 1.0 : m_bandwidthData->GetNoOfDays() - 1;
    gtk_range_set_range (GTK_RANGE(m_slider), 0.0, max);
    
    gtk_widget_queue_draw (m_drawingArea);

    gtk_widget_show_all (m_window);
}

gchar* MyGTKWindow::FormatValue(GtkScale *scale, gdouble value, gpointer data)
{
    MyGTKWindow* myWindow = (MyGTKWindow*)data;
    
    int iVal = value;
    
    int i = 0;
    int iDate = 0;
    for (auto const& day : myWindow->m_bandwidthData->GetDays())
    {
        if (i==iVal)
            iDate = atoi(day.second->Date().c_str());
        i++;
    }
    
    return g_strdup_printf ("%d", iDate);
}

MyGTKWindow::MyGTKWindow(const MyGTKWindow& orig)
{
}

MyGTKWindow::~MyGTKWindow()
{
}

void MyGTKWindow::Toggled (GtkCheckMenuItem *menuitem, gpointer data)
{    
    MyGTKWindow *myWindow = (MyGTKWindow*)data;
    
    myWindow->m_selectedDay = "";
    myWindow->m_startDay = "";

    gtk_widget_queue_draw (myWindow->m_drawingArea);
}

void MyGTKWindow::SelectDay (GtkMenuItem *menuitem, gpointer data)
{    
    MyGTKWindow *myWindow = (MyGTKWindow*)data;

    myWindow->m_dialog = gtk_dialog_new();
    
    gtk_window_set_position (GTK_WINDOW(myWindow->m_dialog), GTK_WIN_POS_CENTER_ON_PARENT);

    GtkWidget *calendar = gtk_calendar_new ();
    gtk_calendar_mark_day (GTK_CALENDAR (calendar), 19);	
    
    GtkWidget *content_area = gtk_dialog_get_content_area (GTK_DIALOG(myWindow->m_dialog));
    gtk_box_pack_start (GTK_BOX(content_area), calendar, TRUE, TRUE, 5);

    g_signal_connect (calendar, "day-selected-double-click", G_CALLBACK (DayDoubleClicked), data);
    
    gtk_widget_show_all (GTK_WIDGET(myWindow->m_dialog));
    
    gtk_dialog_run (GTK_DIALOG(myWindow->m_dialog));
    
    gtk_widget_destroy (GTK_WIDGET(myWindow->m_dialog));

    gtk_widget_queue_draw (myWindow->m_drawingArea);
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
    myWindow->m_selectedDay = buffer;
    
    gtk_dialog_response(GTK_DIALOG(myWindow->m_dialog), 0);

    gtk_widget_queue_draw (myWindow->m_drawingArea);
}

void MyGTKWindow::DrawSurface (MyGTKWindow *myWindow)
{
    // Create our context.
    cairo_t *cr = cairo_create (myWindow->m_surface);

    // Clear background to white.
    cairo_set_source_rgb (cr, 1, 1, 1);
    cairo_paint (cr);

    cairo_set_line_width(cr, 1);
    cairo_set_source_rgb (cr, 0, 0, 0);
    
    int time_shift = 10;
    int time_scale = 5;

    int bandwidth_shift = gtk_widget_get_allocated_height (GTK_WIDGET(myWindow->m_drawingArea)) - 50;
    int bandwidth_scale = 8;
    
    int sample_rate_in_minutes = 5;
    
    if (!myWindow->m_selectedDay.empty())
    {
        // We need to show the chart for a specific day.
        auto const& day = myWindow->m_bandwidthData->GetDay(myWindow->m_selectedDay);
        cairo_set_source_rgb (cr, 0, 0, 0);
        int x = 0;
        for (auto const& datapoint : day.DataPoints())
        {
            int bandwidth = bandwidth_shift - datapoint.second->Bandwidth() * bandwidth_scale;
            if (x == 0)
            {
                x = time_shift;
                cairo_move_to (cr, x, bandwidth);
            }
            else
            {
                int hour = atoi(datapoint.first.substr(0,2).c_str());
                int minute = atoi(datapoint.first.substr(2,2).c_str());
                int elapsed_minutes = hour * 60 + minute;
                x = time_shift + elapsed_minutes / sample_rate_in_minutes * time_scale;
                cairo_line_to (cr, x, bandwidth);
            }
        }
        cairo_stroke (cr);
    }
    else if (!myWindow->m_startDay.empty())
    {
        auto const& months = myWindow->m_bandwidthData->GetMonths();
        for(auto const& month_it : *months)
        {
            // Should we draw this month?
            for (auto const& menu_it : myWindow->m_selectDataByMonthMenuMonthItems)
            {
                if (month_it.compare(gtk_menu_item_get_label(GTK_MENU_ITEM(menu_it))) == 0 && gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(menu_it)))
                {
                    // Yes, so we need to retrieve the statistics for this month.
                    auto statistics = myWindow->m_bandwidthData->GetStatistics(myWindow->m_startDay, myWindow->m_endDay);
                    
                    DrawStatisticalView(cr, *statistics, bandwidth_shift, bandwidth_scale, time_shift, time_scale, sample_rate_in_minutes);
                }
            }
        }
    }
    else
    {
        auto months = myWindow->m_bandwidthData->GetMonths();
        for(auto const& month_it : *months)
        {
            // Should we draw this month?
            for (auto const& menu_it : myWindow->m_selectDataByMonthMenuMonthItems)
            {
                if (month_it.compare(gtk_menu_item_get_label(GTK_MENU_ITEM(menu_it))) == 0 && gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(menu_it)))
                {
                    // Yes, so we need to retrieve the statistics for this month.
                    auto statistics = myWindow->m_bandwidthData->GetStatistics(month_it);
                    
                    DrawStatisticalView(cr, *statistics, bandwidth_shift, bandwidth_scale, time_shift, time_scale, sample_rate_in_minutes);
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
    
    // Destroy our context.
    cairo_destroy (cr);
}

void MyGTKWindow::DrawStatisticalView (cairo_t *cr, const std::map<std::string, std::unique_ptr<BandwidthStatistics>>& statistics, int bandwidth_shift, int bandwidth_scale, int time_shift, int time_scale, int sample_rate_in_minutes)
{
    try
    {
    cairo_set_source_rgba (cr, 1, 0, 0, 0.5);
    int x = 0;
    std::deque<int> high_av;
    std::deque<int> low_av;
    int count = 0;
    for (auto const& it_stat : statistics)
    {
        int bandwidth_high = bandwidth_shift - it_stat.second.get()->High() * bandwidth_scale;
        int bandwidth_low = bandwidth_shift - it_stat.second.get()->Low() * bandwidth_scale;

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

        int hour = atoi(it_stat.first.substr(0,2).c_str());
        int minute = atoi(it_stat.first.substr(2,2).c_str());
        int elapsed_minutes = hour * 60 + minute;
        x = time_shift + elapsed_minutes / sample_rate_in_minutes * time_scale;
        cairo_move_to (cr, x, bandwidth_high);
        cairo_line_to (cr, x, bandwidth_low);
        cairo_stroke (cr);
    }

    cairo_set_source_rgb (cr, 0, 0, 0);
    x = 0;
    for (auto const& it_stat : statistics)
    {
        int bandwidth = bandwidth_shift - it_stat.second.get()->Average() * bandwidth_scale;
        if (x == 0)
        {
            x = time_shift;
            cairo_move_to (cr, x, bandwidth);
        }
        else
        {
            int hour = atoi(it_stat.first.substr(0,2).c_str());
            int minute = atoi(it_stat.first.substr(2,2).c_str());
            int elapsed_minutes = hour * 60 + minute;
            x = time_shift + elapsed_minutes / sample_rate_in_minutes * time_scale;
            cairo_line_to (cr, x, bandwidth);
        }
    }
    cairo_stroke (cr);
    }
    catch (...)
    {
        std::cout << "Caught exception in DrawStatisticalView" << std::endl;
    }
}

gboolean MyGTKWindow::Configure (GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
    MyGTKWindow* myWindow = (MyGTKWindow*)data;
    
    bool recreateSurface = false;
    if (myWindow->m_surface == NULL || event->height != myWindow->m_height || event->width != myWindow->m_width)
    {
        MyGTKWindow *myWindow = (MyGTKWindow*)data;

        if (myWindow->m_surface != NULL)
        {
            // We have an existing surface, so we'll destroy it first.
            cairo_surface_destroy (myWindow->m_surface);
        }

        myWindow->m_surface = gdk_window_create_similar_surface (event->window, CAIRO_CONTENT_COLOR, event->width, event->height);

        // Make sure we know the size of our surface.
        myWindow->m_height = event->height;
        myWindow->m_width = event->width;

        gtk_widget_queue_draw (myWindow->m_drawingArea);
    }

    /* We've handled the configure event, no need for further processing. */
    return TRUE;
}

gboolean MyGTKWindow::Draw (GtkWidget *widget, cairo_t *cr, gpointer data)
{
    MyGTKWindow* myWindow = (MyGTKWindow*)data;

    if (myWindow->m_surface != NULL)
    {
        // Draw onto our surface.
        DrawSurface (myWindow);
        
        // Paint our window using the source surface.
        cairo_set_source_surface (cr, myWindow->m_surface, 0, 0);
        cairo_paint (cr);
    }
    else
    {
        std::cout << "An attempt was made to draw the window, but the drawing surface is NULL" << std::endl;
    }

    return FALSE;
}