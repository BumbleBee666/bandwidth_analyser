/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   BandwithDataPoint.cpp
 * Author: mark
 * 
 * Created on 10 May 2021, 18:05
 */

#include "BandwidthDataPoint.h"

BandwidthDataPoint::BandwidthDataPoint(const std::string& date, const std::string& time, const double bandwidth) :
m_date(date), m_time(time), m_bandwidth(bandwidth)
{
}

BandwidthDataPoint::BandwidthDataPoint(const BandwidthDataPoint& orig) :
m_date(orig.Date()), m_time(orig.Time()), m_bandwidth(orig.Bandwidth())
{
}

BandwidthDataPoint::~BandwidthDataPoint() {
}

