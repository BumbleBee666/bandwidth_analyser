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
#include <set>
#include <map>

#include "BandwidthDataPoint.h"
#include "BandwidthDay.h"
#include "BandwidthStatistics.h"

class BandwidthMonth {
public:
    BandwidthMonth();
    BandwidthMonth(const BandwidthMonth& orig);
    virtual ~BandwidthMonth();
    
    bool LoadData(const std::string& filepath, const std::string& date);

    const std::map<std::string, BandwidthStatistics*>& Statistics() const { return m_stats; }
    
    const std::string& Month() const { return m_month; }
    const std::map<std::string, BandwidthDay*>& Days() const { return m_days; }
    static void GetFileMonths(const std::string& filepath, std::set<std::string>& filemonths);
    
private:
    void GetFileDates(const std::string& filepath, const std::string& month, std::set<std::string>& filedates) const;

    std::string m_month;
    std::map<std::string, BandwidthStatistics*> m_stats;
    std::map<std::string, BandwidthDay*> m_days;
    static const std::string m_directory;
};

#endif /* BANDWIDTHMONTH_H */

