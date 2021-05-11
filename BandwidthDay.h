/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   BandwidthDay.h
 * Author: mark
 *
 * Created on 10 May 2021, 18:03
 */

#ifndef BANDWIDTHDAY_H
#define BANDWIDTHDAY_H

#include <string>
#include <map>

#include "BandwidthDataPoint.h"

class BandwidthDay {
public:
    BandwidthDay();
    BandwidthDay(const BandwidthDay& orig);
    virtual ~BandwidthDay();
    
    bool LoadData(const std::string& date);

    const std::map<int, BandwidthDataPoint*>& Data() { return m_bandwidthData; }
    
    const std::string& Date() { return m_date; }
    
private:
    std::string m_date;
    std::map<int, BandwidthDataPoint*> m_bandwidthData;
};

#endif /* BANDWIDTHDAY_H */

