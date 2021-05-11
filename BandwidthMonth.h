/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   BandWidthMonth.h
 * Author: mark
 *
 * Created on 11 May 2021, 13:27
 */

#ifndef BANDWIDTHMONTH_H
#define BANDWIDTHMONTH_H

#include <string>
#include <vector>
#include <map>

#include "BandwidthDataPoint.h"
#include "BandwidthDay.h"

class BandwidthMonth {
public:
    BandwidthMonth();
    BandwidthMonth(const BandwidthMonth& orig);
    virtual ~BandwidthMonth();
    
    bool LoadData(const std::string& date);

    const std::map<int, double>& Data() { return m_bandwidthData; }
    
    const std::string& Date() { return m_date; }
    
private:
    std::string m_date;
    std::map<int, double> m_bandwidthData;
    std::map<int, BandwidthDay*> m_days;
};

#endif /* BANDWIDTHMONTH_H */

