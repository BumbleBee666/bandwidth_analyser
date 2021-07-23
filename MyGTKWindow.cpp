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
#include <set>
#include <algorithm>

#include "MyGTKCalendarWindow.h"
#include "BandwidthData.h"
#include "BandwidthDay.h"
#include "BandwidthStatistics.h"

#include "MyGTKWindow.h"

int MyGTKWindow::width = 1460;
int MyGTKWindow::height = 700;

MyGTKWindow::MyGTKWindow(GtkApplication* app, std::shared_ptr<const BandwidthData> bandwidthData) :
app(app),
calendar(NULL),
surface(NULL),
selected_day(""),
m_bandwidthData(bandwidthData),
window(NULL),
box1(NULL)
{
    window = gtk_application_window_new (app);

    gtk_window_set_title(GTK_WINDOW (window), "Welcome to GNOME");
    gtk_window_set_default_size(GTK_WINDOW (window), width, height);
    
    DrawWindow();
}

void MyGTKWindow::DrawWindow()
{
    if (box1 != NULL)
    {
        gtk_widget_destroy(box1);
    }
    
    box1 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add (GTK_CONTAINER (window), box1);
    
    menu = gtk_menu_bar_new ();
    gtk_box_pack_start (GTK_BOX (box1), menu, FALSE, FALSE, 0);

    GtkWidget *item = gtk_menu_item_new_with_label ("Load Data File");
    gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);

    GtkWidget *subMenu = gtk_menu_new ();
    gtk_menu_item_set_submenu( GTK_MENU_ITEM( item ), subMenu );

    byMonth_item = gtk_menu_item_new_with_label( "By Month" );
    gtk_menu_shell_append( GTK_MENU_SHELL(subMenu), byMonth_item );
    
    byMonth_subMenu = gtk_menu_new ();
    gtk_menu_item_set_submenu( GTK_MENU_ITEM( byMonth_item ), byMonth_subMenu );
    
    m_months = m_bandwidthData->GetMonths();
    for (auto const& it : *m_months)
    {
        GtkWidget *month_item = gtk_check_menu_item_new_with_label(it.c_str());
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

    int max = m_bandwidthData->GetNoOfDays() == 0 ? 1 : m_bandwidthData->GetNoOfDays();
    slider = gtk_scale_new_with_range (GTK_ORIENTATION_HORIZONTAL, 0, max, 1.0);
    gtk_container_add (GTK_CONTAINER (box1), slider);
    g_signal_connect (slider, "format-value", G_CALLBACK (FormatValue), this);
    g_signal_connect (slider, "value-changed", G_CALLBACK (ValueChanged), this);
    
    gtk_widget_show_all(window);
}

void MyGTKWindow::ValueChanged (GtkRange *range, gpointer data)
{
    MyGTKWindow* myWindow = (MyGTKWindow*)data;
    
    int i = 0;
    int iVal = gtk_range_get_value(range);
    int iStartVal = iVal - 10;
    if (iStartVal < 0) iStartVal = 0;
    int iEndVal = iVal + 10;
    if (iEndVal >= myWindow->m_bandwidthData->GetNoOfDays()) iEndVal = myWindow->m_bandwidthData->GetNoOfDays() - 1;
    for (auto const& it_day : myWindow->m_bandwidthData->GetDays())
    {
        if (i==iStartVal)
        {
            myWindow->start_day = it_day.second->Date();
        }
        if (i==iEndVal)
        {
            myWindow->end_day = it_day.second->Date();
        }
        i++;
    }

    myWindow->selected_day = "";
    
    DrawSurface(myWindow);
    
    gtk_widget_queue_draw(myWindow->drawing_area);
}

void MyGTKWindow::BandwidthUpdated()
{
    DrawWindow();

    DrawSurface(this);
    
    gtk_widget_queue_draw(drawing_area);
}

gchar* MyGTKWindow::FormatValue(GtkScale *scale, gdouble value, gpointer data)
{
    MyGTKWindow* myWindow = (MyGTKWindow*)data;
    
    int iVal = value;
    
    int i = 0;
    int iDate = 0;
    for (auto const& it_day : myWindow->m_bandwidthData->GetDays())
    {
        if (i==iVal)
            iDate = atoi(it_day.second->Date().c_str());
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
    MyGTKWindow* myWindow = (MyGTKWindow*)data;
    
    myWindow->selected_day = "";
    myWindow->start_day = "";

    DrawSurface(myWindow);
    
    gtk_widget_queue_draw(myWindow->drawing_area);
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
    
    gtk_dialog_response(GTK_DIALOG(myWindow->dialog), 0);
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

    int bandwidth_shift = gtk_widget_get_allocated_height(myWindow->drawing_area) - 50;
    int bandwidth_scale = 8;
    
    int sample_rate_in_minutes = 5;
    
    if (!myWindow->selected_day.empty())
    {
        // We need to show the chart for a specific day.
        try
        {
            auto const& day = myWindow->m_bandwidthData->GetDay(myWindow->selected_day);
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
        catch (std::invalid_argument)
        {}
    }
    else if (!myWindow->start_day.empty())
    {
        auto months = myWindow->m_bandwidthData->GetMonths();
        for(auto const& month_it : *months)
        {
            // Should we draw this month?
            for (std::vector<GtkCheckMenuItem*>::const_iterator menu_it = myWindow->m_menuItems.begin() ; menu_it != myWindow->m_menuItems.end() ; ++menu_it)
            {
                if (month_it.compare(gtk_menu_item_get_label((GtkMenuItem*)*menu_it)) == 0 && gtk_check_menu_item_get_active((GtkCheckMenuItem*)*menu_it))
                {
                    // Yes, so we need to retrieve the statistics for this month.
                    auto statistics = myWindow->m_bandwidthData->GetStatistics(myWindow->start_day, myWindow->end_day);
                    
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
            for (std::vector<GtkCheckMenuItem*>::const_iterator menu_it = myWindow->m_menuItems.begin() ; menu_it != myWindow->m_menuItems.end() ; ++menu_it)
            {
                if (month_it.compare(gtk_menu_item_get_label((GtkMenuItem*)*menu_it)) == 0 && gtk_check_menu_item_get_active((GtkCheckMenuItem*)*menu_it))
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
    
    cairo_destroy (cr);
}

void MyGTKWindow::DrawStatisticalView(cairo_t *cr, const std::map<std::string, std::unique_ptr<BandwidthStatistics>>& statistics, int bandwidth_shift, int bandwidth_scale, int time_shift, int time_scale, int sample_rate_in_minutes)
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