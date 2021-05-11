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

#include "BandwidthMonth.h"

class MyGTKWindow
{
public:
    MyGTKWindow(GtkApplication* app);
    MyGTKWindow(const MyGTKWindow& orig);
    virtual ~MyGTKWindow();

private:    
    static void DrawSurface (MyGTKWindow* myWindow);
    static gboolean Configure (GtkWidget *widget, GdkEventConfigure *event, gpointer data);
    static gboolean Draw (GtkWidget *widget, cairo_t *cr, gpointer data);
    void Redraw ();

private:
    GtkWidget *widget;
    GtkWidget *window;
    GtkWidget *label;
    GtkWidget *frame;
    GtkWidget *drawing_area;
    GtkWidget *opt, *menu, *item;
    GtkWidget *box1;

    cairo_surface_t *surface;
            
    static int width;
    static int height;
    
    BandwidthMonth m_bandwidth;
};

#endif /* MYGTKWINDOW_H */

