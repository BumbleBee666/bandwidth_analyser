/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   BandwidthDay.cpp
 * Author: mark
 * 
 * Created on 10 May 2021, 18:03
 */

#include <filesystem>
#include <dirent.h>
#include <fstream>
#include <iostream>

#include "BandwidthDay.h"

#include "BandwidthFile.h"

BandwidthDay::BandwidthDay(const rapidjson::Value& obj)
{
    Deserialize(obj);
}

BandwidthDay::BandwidthDay(const std::string& date)
:m_date(date)
{
}

BandwidthDay::BandwidthDay(const BandwidthDay& orig)
{   
    this->m_date = orig.m_date;
    for (auto const& it : orig.m_bandwidthDataPoints)
    {
        m_bandwidthDataPoints[it.first] = std::make_unique<BandwidthDataPoint>(*it.second);
    }
}

void BandwidthDay::LoadDataPoint(const std::string& directory, const std::string& filename)
{
    auto datapoint = BandwidthFile::LoadFile(directory + "//" + filename);
    if (datapoint != NULL)
    {
        std::string filetime = datapoint->Time().substr(0,2) + datapoint->Time().substr(3,2);
        m_bandwidthDataPoints[filetime] = std::move(datapoint);
    }
}

const std::map<std::string, const std::unique_ptr<const BandwidthDataPoint>>& BandwidthDay::DataPoints() const
{ 
    return (const std::map<std::string, const std::unique_ptr<const BandwidthDataPoint>>&)m_bandwidthDataPoints; 
}
   
bool BandwidthDay::Serialize(rapidjson::Writer<rapidjson::StringBuffer>* writer) const
{
    writer->StartObject();
    writer->Key("Date");
    writer->String(m_date.c_str());
    writer->Key("DataPoints");
    writer->StartArray();
    for (auto const& datapoint : m_bandwidthDataPoints)
        datapoint.second->Serialize(writer);
    writer->EndArray();
    writer->EndObject();
    
    return true;
}

bool BandwidthDay::Deserialize(const rapidjson::Value& obj)
{
    m_date = obj["Date"].GetString();
    rapidjson::Value::ConstArray datapoints = obj["DataPoints"].GetArray();
    for (rapidjson::Value::ConstValueIterator it = datapoints.Begin(); it != datapoints.End(); ++it)
    {
        auto bandwidthDataPoint = std::unique_ptr<BandwidthDataPoint>(new BandwidthDataPoint(it->GetObject()));
        std::string time = bandwidthDataPoint->Time().substr(0,2) + bandwidthDataPoint->Time().substr(3,2);
        m_bandwidthDataPoints[time] = std::move(bandwidthDataPoint);
    }

    return true;
}

void BandwidthDay::LoadData(const std::string& directory)
{
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (directory.c_str())) != NULL)
    {
        /* print all the files and directories within directory */
        while ((ent = readdir (dir)) != NULL) 
        {
            const std::string filename(ent->d_name);
            int pos = filename.find("results_", 0);
            
            if (pos >= 0)
            {
                const std::string filedate = filename.substr(pos+8, 8);
                if (filedate.compare(m_date) == 0)
                {
                    auto bandwidthDataPoint = BandwidthFile::LoadFile(directory + "//" + filename);
                    if (bandwidthDataPoint != NULL)
                    {
                        std::string time = bandwidthDataPoint->Time().substr(0,2) + bandwidthDataPoint->Time().substr(3,2);
                        m_bandwidthDataPoints[time] = std::move(bandwidthDataPoint);
                    }
                }
            }            
        }
        closedir (dir);
    }
}
