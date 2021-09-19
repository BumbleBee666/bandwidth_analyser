/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   MyGTKWindow.h
 * Author: mark
 *
 * Created on 20 April 2021, 11:22
 */

#ifndef MYGTKWINDOW_H
#define MYGTKWINDOW_H

#include <vector>
#include <map>
#include <set>
#include <memory>

#include <gtk/gtk.h>

#include "MyGTKCalendarWindow.h"
#include "BandwidthStatistics.h"
#include "BandwidthDataListener.h"
#include "BandwidthData.h"

class MyGTKWindow : public BandwidthDataListener
{
public:
    MyGTKWindow(GtkApplication *app, std::shared_ptr<const BandwidthData> bandwidthData);
    MyGTKWindow(const MyGTKWindow& orig);
    virtual ~MyGTKWindow();
    
    void DrawWindow();
    virtual void BandwidthUpdated();
    
private:    
    static void DrawSurface (MyGTKWindow *myWindow);
    static gboolean Configure (GtkWidget *widget, GdkEventConfigure *event, gpointer data);
    static gboolean Draw (GtkWidget *widget, cairo_t *cr, gpointer data);
    static void SelectDay (GtkMenuItem *menuitem, gpointer data);
    static void DayDoubleClicked (GtkCalendar *calendar, gpointer data);
    static void Toggled (GtkCheckMenuItem *menuitem, gpointer data);
    static gchar* FormatValue(GtkScale *scale, gdouble value, gpointer data);
    static void ValueChanged (GtkRange *range, gpointer data);
    static void DrawStatisticalView(cairo_t *cr, const std::map<std::string, std::unique_ptr<BandwidthStatistics>>& statistics, int bandwidth_shift, int bandwidth_scale, int time_shift, int time_scale, int sample_rate_in_minutes);
    
private:
    GtkApplication *m_app;
    GtkWidget *window;
    GtkWidget *label;
    GtkWidget *frame;
    GtkWidget *drawing_area;
    GtkWidget *opt, *menu, *item;
    GtkWidget *box1;
    GtkWidget *dialog;
    GtkWidget *slider;
    GtkWidget *byMonth_item;
    GtkWidget *itemSelectData;
    GtkWidget *subMenu;
    GtkWidget *byDay_item;
    GtkWidget *separator1;
    GtkWidget *exit_item;
    GtkWidget *byMonth_subMenu;
    std::vector<GtkWidget*> m_menuItems;

    cairo_surface_t *surface;
            
    static const int width;
    static const int height;
    
    std::unique_ptr<std::set<std::string>> m_months;
    std::unique_ptr<MyGTKCalendarWindow> calendar;
    
    std::shared_ptr<const BandwidthData> m_bandwidthData;
    
    std::string selected_day;
    std::string start_day, end_day;
};

#endif /* MYGTKWINDOW_H */
