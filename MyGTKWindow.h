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
    
    // This is our callback from BandwidthData to tell us that the data has been updated.
    virtual void BandwidthUpdated();
    
private:    
    // This event is triggered when the size, position, or stacking of the window changes.
    static gboolean Configure (GtkWidget *widget, GdkEventConfigure *event, gpointer data);
    
    // This event is called when we need to redraw our window.
    static gboolean Draw (GtkWidget *widget, cairo_t *cr, gpointer data);
    
    
    static void DrawSurface (MyGTKWindow *myWindow);
    static void SelectDay (GtkMenuItem *menuitem, gpointer data);
    static void DayDoubleClicked (GtkCalendar *calendar, gpointer data);
    static void Toggled (GtkCheckMenuItem *menuitem, gpointer data);
    static gchar* FormatValue(GtkScale *scale, gdouble value, gpointer data);
    static void ValueChanged (GtkRange *range, gpointer data);
    static void DrawStatisticalView(cairo_t *cr, const std::map<std::string, std::unique_ptr<BandwidthStatistics>>& statistics, int bandwidth_shift, int bandwidth_scale, int time_shift, int time_scale, int sample_rate_in_minutes);
    
    void CreateSelectDataByMonthMenu();
    
    GtkApplication *m_app;
    GtkWidget *m_window;
    GtkWidget *m_box;
    GtkWidget *m_frame;
    GtkWidget *m_drawingArea;
    
    GtkWidget *m_menubar;
    
    GtkWidget *m_fileItem;
    GtkWidget *m_fileMenu;
    GtkWidget *m_exitItem;
    
    GtkWidget *m_selectDataItem;
    GtkWidget *m_selectDataMenu;
    GtkWidget *m_selectDataByMonthItem;
    GtkWidget *m_selectDataByMonthMenu;
    GtkWidget *m_selectDataByDayItem;
    
    GtkWidget *m_dialog;
    GtkWidget *m_slider;
    
    std::set<std::string> m_months;
    std::vector<GtkWidget*> m_selectDataByMonthMenuMonthItems;

    cairo_surface_t *m_surface;
            
    int m_width;
    int m_height;
    
    std::unique_ptr<MyGTKCalendarWindow> m_calendar;
    
    std::shared_ptr<const BandwidthData> m_bandwidthData;
    
    std::string m_selectedDay;
    std::string m_startDay, m_endDay;
};

#endif /* MYGTKWINDOW_H */
