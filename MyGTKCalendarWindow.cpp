/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   MyGTKCalendarWindow.cpp
 * Author: mark
 * 
 * Created on 22 May 2021, 12:03
 */

#include "MyGTKCalendarWindow.h"

#define DEF_PAD 10

MyGTKCalendarWindow::MyGTKCalendarWindow(GtkApplication* app)
{
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    
    gtk_window_set_title(GTK_WINDOW (window), "Select Day");
    gtk_window_set_resizable (GTK_WINDOW (window), FALSE);
    
    box = gtk_vbox_new (FALSE, DEF_PAD);
    gtk_container_add (GTK_CONTAINER (window), box);
    
    frame = gtk_frame_new ("Calendar");
    gtk_box_pack_start(GTK_BOX (box), frame, FALSE, TRUE, DEF_PAD);
    calendar = gtk_calendar_new ();
    gtk_calendar_mark_day (GTK_CALENDAR (calendar), 19);	
    gtk_container_add (GTK_CONTAINER (frame), calendar);
    
    gtk_widget_show_all(window);
    gtk_window_set_modal(GTK_WINDOW (window), true);
}

MyGTKCalendarWindow::MyGTKCalendarWindow(const MyGTKCalendarWindow& orig) {
}

MyGTKCalendarWindow::~MyGTKCalendarWindow() {
}

