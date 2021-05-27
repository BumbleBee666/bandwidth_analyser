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
#include <gtk/gtk.h>

#include "BandwidthMonth.h"
#include "MyGTKCalendarWindow.h"
#include "BandwidthStatistics.h"

class MyGTKWindow
{
public:
    MyGTKWindow(GtkApplication* app, const std::string filepath);
    MyGTKWindow(const MyGTKWindow& orig);
    virtual ~MyGTKWindow();

private:    
    static void DrawSurface (MyGTKWindow* myWindow);
    static gboolean Configure (GtkWidget *widget, GdkEventConfigure *event, gpointer data);
    static gboolean Draw (GtkWidget *widget, cairo_t *cr, gpointer data);
    static void SelectDay (GtkMenuItem *menuitem, gpointer data);
    static void DayDoubleClicked (GtkCalendar *calendar, gpointer data);
    static void Toggled (GtkCheckMenuItem *menuitem, gpointer data);

private:
    GtkApplication *app;
    GtkWidget *widget;
    GtkWidget *window;
    GtkWidget *label;
    GtkWidget *frame;
    GtkWidget *drawing_area;
    GtkWidget *opt, *menu, *item;
    GtkWidget *box1;
    GtkWidget *dialog;

    cairo_surface_t *surface;
            
    static int width;
    static int height;
    
    std::vector<GtkCheckMenuItem*> m_menuItems;
    std::vector<BandwidthMonth*> m_bandwidthMonths;
    MyGTKCalendarWindow* calendar;
    
    std::string selected_day;
};

#endif /* MYGTKWINDOW_H */

