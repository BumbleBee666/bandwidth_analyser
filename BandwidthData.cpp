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

std::unique_ptr<std::set<std::string>> BandwidthData::GetFileDates() const
{
    std::unique_ptr<std::set<std::string>> filedates( new std::set<std::string>());
    
    for(auto const& it : *GetFileNames())
    {
        int pos = it.find("results_", 0);
        filedates->insert(it.substr(pos+8, 8));
    }   
    
    return filedates;
}

std::unique_ptr<std::set<std::string>> BandwidthData::GetFileMonths() const
{
    std::unique_ptr<std::set<std::string>> filemonths(new std::set<std::string>());

    for(auto const& it : *GetFileNames())
    {
        int pos = it.find("results_", 0);
        filemonths->insert(it.substr(pos+8, 6));
    }
    
    return filemonths;
}

void BandwidthData::RegisterListener(std::shared_ptr<BandwidthDataListener> listener)
{
    m_listeners.push_back(listener);
}

const std::string BandwidthData::to_json() const
{
    const std::string newline = "\n\r";
    const std::string continuation = ",";
    
    std::string json;

    json += "{" + newline;

    json += "\"Filenames\" : [" + newline;
    int i = 0;
    for (auto const& filename : m_filenames)
        json += "\"" + filename + "\"" + (++i == m_filenames.size() ? "" : continuation) + newline;
    json += "]" + continuation + newline;
    
    json += "\"Filedates\" : [" + newline;
    i = 0;
    for (auto const& filedate : m_filedates)
        json += "\"" + filedate + "\"" + (++i == m_filedates.size() ? "" : continuation) + newline;
    json += "]" + continuation + newline;
    
    json += "\"Days\" : [" + newline;
    i = 0;
    for (auto const& day : m_days)
    {
        json += "{" + newline;
        json += "\"Date\" : \"" + day.first + "\"" + continuation + newline;
        json += "\"Datapoints\" : " + day.second.get()->to_json() + newline;
        json += "}" + (++i == m_days.size() ? "" : continuation) + newline;
    }
    json += "]" + newline;
    
    json += "}";
    
    return json;
}

void BandwidthData::from_json(const std::string& json)
{
    std::cout << "Blah";
}

void BandwidthData::UpdateThread()
{
    // Load the cache into memory.
//    std::ifstream inFile;
//    inFile.open(cacheFile, std::ios::in);
//    from_json(inFile);
//    inFile.close();
    
    // Keep going until told to exit.
    while (!m_finishThread)
    {
        // Get the set of file dates and files.
        auto filenames = GetFileNames();
        
        // Has the set changed?
        if (filenames->size() != m_filenames.size())
        {
            std::cout << "New file(s) detected ..\n\r";
            
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
            
            // Send update to listeners.
            for (auto it : m_listeners)
            {
                it->BandwidthUpdated();
            }
        }
        to_json();
	while (wait_for(std::chrono::minutes(updateIntervalInMins)));
    }
}

template<class Duration>
bool BandwidthData::wait_for(Duration duration)
{
	std::unique_lock<std::mutex> l(m_);
	return !c_.wait_for(l, duration, [this]() { return stop_; });
}

std::unique_ptr<std::map<std::string, std::unique_ptr<BandwidthStatistics>>> BandwidthData::GetStatistics() const
{
    auto statistics = std::unique_ptr<std::map<std::string, std::unique_ptr<BandwidthStatistics>>>(new std::map<std::string, std::unique_ptr<BandwidthStatistics>>());

    // Build our daily averages.
    std::map<std::string, std::vector<double>> averages;
    
    for (auto const& it_day : m_days)
    {
        for (auto const& it_dp : it_day.second->DataPoints())
        {
            std::map<std::string, std::vector<double>>::const_iterator it_time = averages.find(it_dp.first);
            if (it_time == averages.end())
            {
                averages[it_dp.first] = std::vector<double>();
            }
            averages[it_dp.first].push_back(it_dp.second->Bandwidth());
        }
    }
    
    for (auto const& it_average : averages)
    {
        double high = 0.0;
        double low = 80.0;
        double average = 0.0;

        for (auto const& it_vec : it_average.second)
        {
            if (it_vec > high) high = it_vec;
            if (it_vec < low) low = it_vec;
            average += it_vec;
        }
        
        average = average / it_average.second.size();
        
        (*statistics)[it_average.first] = std::unique_ptr<BandwidthStatistics>(new BandwidthStatistics(it_average.first, average, high, low));
    }
    
    return statistics;
}

std::unique_ptr<std::map<std::string, std::unique_ptr<BandwidthStatistics>>> BandwidthData::GetStatistics(const std::string& filter) const
{
    auto statistics = std::unique_ptr<std::map<std::string, std::unique_ptr<BandwidthStatistics>>>(new std::map<std::string, std::unique_ptr<BandwidthStatistics>>());
    
    // Build our daily averages.
    std::map<std::string, std::vector<double>> averages;

    for (auto const& it_day : m_days)
    {
        if (filter.compare(it_day.first.substr(0, filter.length())) == 0)
        {
            for (auto const& it_dp : it_day.second->DataPoints())
            {
                std::map<std::string, std::vector<double>>::const_iterator it_time = averages.find(it_dp.first);
                if (it_time == averages.end())
                {
                    averages[it_dp.first] = std::vector<double>();
                }
                averages[it_dp.first].push_back(it_dp.second->Bandwidth());
            }
        }
    }
    
    for (auto const& it_average : averages)
    {
        double high = 0.0;
        double low = 80.0;
        double average = 0.0;
        for (std::vector<double>::const_iterator it_vec = it_average.second.begin() ; it_vec != it_average.second.end() ; ++it_vec)
        {
            if (*it_vec > high) high = *it_vec;
            if (*it_vec < low) low = *it_vec;
            average += *it_vec;
        }
        average = average / it_average.second.size();
        
        (*statistics)[it_average.first] = std::unique_ptr<BandwidthStatistics>(new BandwidthStatistics(it_average.first, average, high, low));
    }
    
    return statistics;
}

std::unique_ptr<std::map<std::string, std::unique_ptr<BandwidthStatistics>>> BandwidthData::GetStatistics(const std::string& start_filter, const std::string& end_filter) const
{
    auto statistics = std::unique_ptr<std::map<std::string, std::unique_ptr<BandwidthStatistics>>>(new std::map<std::string, std::unique_ptr<BandwidthStatistics>>());

    // Build our daily averages.
    std::map<std::string, std::vector<double>> averages;
    
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
                auto const& it_time = averages.find(it_dp.first);
                if (it_time == averages.end())
                {
                    averages[it_dp.first] = std::vector<double>();
                }
                averages[it_dp.first].push_back(it_dp.second->Bandwidth());
            }
        }

        if (end_filter.compare(it_day.first.substr(0, end_filter.length())) == 0)
        {
            include = false;
        }
    }
    
    for (auto const& it_average : averages)
    {
        double high = 0.0;
        double low = 80.0;
        double average = 0.0;
        
        for (auto const& it_vec : it_average.second)
        {
            if (it_vec > high) high = it_vec;
            if (it_vec < low) low = it_vec;
            average += it_vec;
        }
        
        average = average / it_average.second.size();
        
        (*statistics)[it_average.first] = std::unique_ptr<BandwidthStatistics>(new BandwidthStatistics(it_average.first, average, high, low));
    }
    
    return statistics;
}
