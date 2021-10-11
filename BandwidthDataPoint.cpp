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

#include "JSONBase.h"

#include "BandwidthDataPoint.h"

BandwidthDataPoint::BandwidthDataPoint(const rapidjson::Value& obj)
{
    Deserialize(obj);
}

BandwidthDataPoint::BandwidthDataPoint(const std::string& time, const double bandwidth) :
m_time(time), m_bandwidth(bandwidth)
{
}

BandwidthDataPoint::BandwidthDataPoint(const BandwidthDataPoint& orig) :
m_time(orig.Time()), m_bandwidth(orig.Bandwidth())
{
}

bool BandwidthDataPoint::Serialize(rapidjson::Writer<rapidjson::StringBuffer>* writer) const 
{
    writer->StartObject();
    writer->Key("Time");
    writer->String(m_time.c_str());
    writer->Key("Bandwidth");
    writer->Double(m_bandwidth);
    writer->EndObject();
    
    return true;
}

bool BandwidthDataPoint::Deserialize(const rapidjson::Value& obj) 
{
    m_time = obj["Time"].GetString();
    m_bandwidth = obj["Bandwidth"].GetDouble();
    
    return true;
}
