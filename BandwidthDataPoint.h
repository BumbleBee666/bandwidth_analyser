/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   BandwithDataPoint.h
 * Author: mark
 *
 * Created on 10 May 2021, 18:05
 */

#ifndef BANDWITHDATAPOINT_H
#define BANDWITHDATAPOINT_H

#include <string>

class BandwidthDataPoint {
public:
    BandwidthDataPoint(const std::string& date, const std::string& time, double bandwidth);
    BandwidthDataPoint(const BandwidthDataPoint& orig);
    virtual ~BandwidthDataPoint();
    
    const std::string& Date() const { return m_date; }
    const std::string& Time() const { return m_time; }
    double Bandwidth() const { return m_bandwidth; }
    
private:
    const std::string m_date;
    const std::string m_time;
    const double m_bandwidth;
};

#endif /* BANDWITHDATAPOINT_H */

