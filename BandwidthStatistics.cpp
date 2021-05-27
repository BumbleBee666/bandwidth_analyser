/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   BandwidthStatistics.cpp
 * Author: mark
 * 
 * Created on 26 May 2021, 19:09
 */

#include "BandwidthStatistics.h"

BandwidthStatistics::BandwidthStatistics(const std::string& time, double average, double high, double low):
m_time(time), m_average(average), m_high(high), m_low(low)
{
}

BandwidthStatistics::BandwidthStatistics(const BandwidthStatistics& orig) {
}

BandwidthStatistics::~BandwidthStatistics() {
}

