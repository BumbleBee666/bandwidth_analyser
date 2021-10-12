/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   BandwidthData.cpp
 * Author: mark
 * 
 * Created on 2 June 2021, 19:09
 */

#include <stdlib.h>
#include <filesystem>
#include <dirent.h>
#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>

#include "BandwidthData.h"

const int updateIntervalInMins = 5;
const std::string cacheFile = "BandwidthDataCache.dat";

BandwidthData::BandwidthData(const std::string& filepath)
:m_filepath(filepath),
 m_finishThread(false),
 stop_(false)
{
    m_updateThread = std::unique_ptr<std::thread>(new std::thread(&BandwidthData::UpdateThread, this));
}

BandwidthData::BandwidthData(const BandwidthData& orig) 
:m_filepath(orig.m_filepath)
{
}

BandwidthData::~BandwidthData()
{
    m_finishThread = true;
    {
	std::lock_guard<std::mutex> l(m_);
	stop_ = true;
    }
    c_.notify_one();
    m_updateThread->join();
}

const BandwidthDay& BandwidthData::GetDay(const std::string& day) const 
{
    auto const& it = m_days.find(day);
    
    if (it == m_days.end())
    {
        throw std::invalid_argument( "Given day not found" );
    }
    
    return *(it->second);
}

const std::map<std::string, const std::unique_ptr<const BandwidthDay>>& BandwidthData::GetDays() const
{
    return (const std::map<std::string, const std::unique_ptr<const BandwidthDay>>&)m_days;
}

int BandwidthData::GetNoOfDays() const
{
    return m_days.size();
}

std::unique_ptr<std::set<std::string>> BandwidthData::GetMonths() const
{
    std::unique_ptr<std::set<std::string>> months(new std::set<std::string>());
    
    for(auto const& it : m_days)
    {
        months->insert(it.first.substr(0,6));
    }
    
    return months;
}

std::unique_ptr<std::set<std::string>> BandwidthData::GetFileNames() const
{
    std::unique_ptr<std::set<std::string>> filenames(new std::set<std::string>());
    
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (m_filepath.c_str())) != NULL)
    {
        while ((ent = readdir (dir)) != NULL) 
        {
            const std::string filename(ent->d_name);
            int pos = filename.find("results_", 0);
            
            if (pos >= 0)
            {
                filenames->insert(filename);
            }            
        }
        closedir (dir);
    }
    
    return filenames;
}

void BandwidthData::RegisterListener(std::shared_ptr<BandwidthDataListener> listener)
{
    m_listeners.push_back(listener);
}

bool BandwidthData::Serialize(rapidjson::Writer<rapidjson::StringBuffer>* writer) const
{
    writer->StartObject();
    writer->Key("FileNames");
    writer->StartArray();
    for (auto const& filename : m_filenames)
        writer->String(filename.c_str());
    writer->EndArray();
    writer->Key("Days");
    writer->StartArray();
    for (auto const& day : m_days)
        day.second->Serialize(writer);
    writer->EndArray();
    writer->EndObject();

    return true;
}

bool BandwidthData::Deserialize(const rapidjson::Value& obj)
{
    rapidjson::Value::ConstArray filenames = obj["FileNames"].GetArray();
    for (rapidjson::Value::ConstValueIterator it = filenames.Begin(); it != filenames.End(); ++it)
    {
        m_filenames.insert(it->GetString());
    }    
    rapidjson::Value::ConstArray days = obj["Days"].GetArray();
    for (rapidjson::Value::ConstValueIterator it = days.Begin(); it != days.End(); ++it)
    {
        auto bandwidthDay = std::unique_ptr<BandwidthDay>(new BandwidthDay(it->GetObject()));
        m_days[bandwidthDay->Date()] = std::move(bandwidthDay);
    }    

    return true;
}

void BandwidthData::UpdateThread()
{
    // Load the cache into memory.
    JSONBase::DeserializeFromFile(cacheFile);
    
    // Keep going until told to exit.
    while (!m_finishThread)
    {
        // Get the set of file dates and files.
        auto filenames = GetFileNames();
        
        // Has the set changed?
        if (filenames->size() != m_filenames.size())
        {
            // The set of files has changed.
            std::set<std::string> newFiles;
            std::set_symmetric_difference(filenames->begin(), filenames->end(), m_filenames.begin(), m_filenames.end(), std::inserter(newFiles, newFiles.end()));
            
            for(auto const& it : newFiles)
            {
                // We don't have this data, so we'll load it.
                auto day = it.substr(8, 8);
                {
                    if (m_days.find(day) == m_days.end())
                    {
                        m_days[day] = std::unique_ptr<BandwidthDay>(new BandwidthDay(day));
                    }
                    m_days.find(day)->second->LoadDataPoint(m_filepath, it);
                }
                m_filenames.insert(it);
            }
            
            // Write the new data set to our cache file.
            JSONBase::SerializeToFile(cacheFile);
        }
    
//        std::cout << JSONBase::Serialize() << std::endl;
    
        // Send update to listeners.
        for (auto it : m_listeners)
        {
            it->BandwidthUpdated();
        }

	while (wait_for(std::chrono::minutes(updateIntervalInMins)));
    }
}

template<class Duration>
bool BandwidthData::wait_for(Duration duration)
{
	std::unique_lock<std::mutex> l(m_);
	return !c_.wait_for(l, duration, [this]() { return stop_; });
}

std::unique_ptr<std::map<std::string, std::unique_ptr<BandwidthStatistics>>> BandwidthData::GetStatistics(std::map<std::string, std::vector<double>>& bandwidthsByTime) const
{
    auto statistics = std::make_unique<std::map<std::string, std::unique_ptr<BandwidthStatistics>>>();
            
    for (auto const& bandwidths : bandwidthsByTime)
    {
        double high = 0.0;
        double low = 80.0;
        double average = 0.0;

        for (auto const& bandwidth : bandwidths.second)
        {
            if (bandwidth > high) high = bandwidth;
            if (bandwidth < low) low = bandwidth;
            average += bandwidth;
        }
        
        average = average / bandwidths.second.size();
        
        (*statistics)[bandwidths.first] = std::make_unique<BandwidthStatistics>(average, high, low);
    }
    
    return statistics;
}
    
std::unique_ptr<std::map<std::string, std::unique_ptr<BandwidthStatistics>>> BandwidthData::GetStatistics() const
{
    // Construct our map of time -> bandwidths.
    // Include all data.
    std::map<std::string, std::vector<double>> bandwidthsByTime;
    for (auto const& it_day : m_days)
    {
        for (auto const& it_dp : it_day.second->DataPoints())
        {
            std::map<std::string, std::vector<double>>::const_iterator it_time = bandwidthsByTime.find(it_dp.first);
            if (it_time == bandwidthsByTime.end())
            {
                bandwidthsByTime[it_dp.first] = std::vector<double>();
            }
            bandwidthsByTime[it_dp.first].push_back(it_dp.second->Bandwidth());
        }
    }
    
    // Calculate the statistics.
    return GetStatistics(bandwidthsByTime);
}

std::unique_ptr<std::map<std::string, std::unique_ptr<BandwidthStatistics>>> BandwidthData::GetStatistics(const std::string& filter) const
{
    // Construct our map of time -> bandwidths.
    // Filter by date YYYYMMDD.
    std::map<std::string, std::vector<double>> bandwidthsByTime;
    for (auto const& it_day : m_days)
    {
        if (filter.compare(it_day.first.substr(0, filter.length())) == 0)
        {
            for (auto const& it_dp : it_day.second->DataPoints())
            {
                std::map<std::string, std::vector<double>>::const_iterator it_time = bandwidthsByTime.find(it_dp.first);
                if (it_time == bandwidthsByTime.end())
                {
                    bandwidthsByTime[it_dp.first] = std::vector<double>();
                }
                bandwidthsByTime[it_dp.first].push_back(it_dp.second->Bandwidth());
            }
        }
    }
    
    // Calculate the statistics.
    return GetStatistics(bandwidthsByTime);
}

std::unique_ptr<std::map<std::string, std::unique_ptr<BandwidthStatistics>>> BandwidthData::GetStatistics(const std::string& start_filter, const std::string& end_filter) const
{
    // Construct our map of time -> bandwidths.
    // Filter by date from YYYYMMDD to YYYYMMDD.
    std::map<std::string, std::vector<double>> bandwidthsByTime;
    bool include = false;
    for (auto const& it_day : m_days)
    {
        if (start_filter.compare(it_day.first.substr(0, start_filter.length())) == 0)
        {
            include = true;
        }

        if (include)
        {
            for (auto const& it_dp : it_day.second->DataPoints())
            {
                auto const& it_time = bandwidthsByTime.find(it_dp.first);
                if (it_time == bandwidthsByTime.end())
                {
                    bandwidthsByTime[it_dp.first] = std::vector<double>();
                }
                bandwidthsByTime[it_dp.first].push_back(it_dp.second->Bandwidth());
            }
        }

        if (end_filter.compare(it_day.first.substr(0, end_filter.length())) == 0)
        {
            include = false;
        }
    }
    
    // Calculate the statistics.
    return GetStatistics(bandwidthsByTime);
}
