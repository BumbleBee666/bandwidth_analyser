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

#include "JSONBase.h"

class BandwidthDataPoint : public JSONBase
{
public:
    BandwidthDataPoint(const rapidjson::Value& obj);
    BandwidthDataPoint(const std::string& time, double bandwidth);
    BandwidthDataPoint(const BandwidthDataPoint& orig);
    virtual ~BandwidthDataPoint();
    
    const std::string& Time() const { return m_time; }
    double Bandwidth() const { return m_bandwidth; }

    virtual bool Deserialize(const rapidjson::Value& obj);
    virtual bool Serialize(rapidjson::Writer<rapidjson::StringBuffer>* writer) const;
    
private:
    std::string m_time;
    double m_bandwidth;
};

#endif /* BANDWITHDATAPOINT_H */

