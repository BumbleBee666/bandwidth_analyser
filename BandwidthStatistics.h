/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   BandwidthStatistics.h
 * Author: mark
 *
 * Created on 26 May 2021, 19:09
 */

#ifndef BANDWIDTHSTATISTICS_H
#define BANDWIDTHSTATISTICS_H

#include <string>

class BandwidthStatistics {
public:
    BandwidthStatistics(const std::string& time, double average, double high, double low);
    BandwidthStatistics(const BandwidthStatistics& orig);
    virtual ~BandwidthStatistics();
    
    double Average() const { return m_average; }
    double High() const { return m_high; }
    double Low() const { return m_low; }
    const std::string& Time() const { return m_time; }

private:
    double m_average;
    double m_high;
    double m_low;
    std::string m_time;
};

#endif /* BANDWIDTHSTATISTICS_H */

