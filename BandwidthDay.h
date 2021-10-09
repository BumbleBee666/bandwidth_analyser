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
#include <memory>

#include "JSONBase.h"
#include "BandwidthDataPoint.h"

class BandwidthDay : public JSONBase
{
public:
    BandwidthDay(const rapidjson::Value& obj);
    BandwidthDay(const std::string& date);
    BandwidthDay(const BandwidthDay& orig);
    virtual ~BandwidthDay();
    
    void LoadData(const std::string& directory);
    void LoadDataPoint(const std::string& directory, const std::string& filename);

    const std::map<std::string, const std::unique_ptr<const BandwidthDataPoint>>& DataPoints() const;
    
    const std::string& Date() const { return m_date; }
    
    virtual bool Deserialize(const rapidjson::Value& obj);
    virtual bool Serialize(rapidjson::Writer<rapidjson::StringBuffer>* writer) const;
    
private:
    std::string m_date;
    std::map<std::string, std::unique_ptr<BandwidthDataPoint>> m_bandwidthDataPoints;
};

#endif /* BANDWIDTHDAY_H */

