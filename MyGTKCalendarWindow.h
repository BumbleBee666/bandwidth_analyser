/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   MyGTKCalendarWindow.h
 * Author: mark
 *
 * Created on 22 May 2021, 12:03
 */

#ifndef MYGTKCALENDARWINDOW_H
#define MYGTKCALENDARWINDOW_H

#include <gtk/gtk.h>

class MyGTKCalendarWindow {
public:
    MyGTKCalendarWindow(GtkApplication* app);
    MyGTKCalendarWindow(const MyGTKCalendarWindow& orig);
    virtual ~MyGTKCalendarWindow();
    GtkWidget *window;
private:
    GtkWidget *frame;
    GtkWidget *box;
    GtkWidget *calendar;
};

#endif /* MYGTKCALENDARWINDOW_H */

