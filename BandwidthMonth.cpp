/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   BandWidthMonth.cpp
 * Author: mark
 * 
 * Created on 11 May 2021, 13:27
 */

#include <set>
#include <filesystem>
#include <dirent.h>
#include <vector>

#include "BandwidthMonth.h"
#include "BandwidthDataPoint.h"
#include "BandwidthDay.h"

//const std::string BandwidthMonth::m_directory = "//home//mark//Documents//bandwidth";
    
BandwidthMonth::BandwidthMonth() {
}

BandwidthMonth::BandwidthMonth(const BandwidthMonth& orig) {
}

BandwidthMonth::~BandwidthMonth() {
}

void BandwidthMonth::GetFileDates(const std::string& filepath, const std::string& month, std::set<std::string>& filedates) const
{
    filedates.clear();
    
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (filepath.c_str())) != NULL)
    {
        while ((ent = readdir (dir)) != NULL) 
        {
            const std::string filename(ent->d_name);
            int pos = filename.find("results_", 0);
            
            if (pos >= 0)
            {
                const std::string filedate = filename.substr(pos+8, 8);
                if (filedate.substr(0, 6).compare(month) == 0)
                {
                    filedates.insert(filedate);
                }
            }            
        }
        closedir (dir);
    }
}

void BandwidthMonth::GetFileMonths(const std::string& filepath, std::set<std::string>& filemonths)
{
    filemonths.clear();
    
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (filepath.c_str())) != NULL)
    {
        while ((ent = readdir (dir)) != NULL) 
        {
            const std::string filename(ent->d_name);
            int pos = filename.find("results_", 0);
            
            if (pos >= 0)
            {
                const std::string filedate = filename.substr(pos+8, 8);
                filemonths.insert(filename.substr(pos+8, 6));
            }            
        }
        closedir (dir);
    }
}

bool BandwidthMonth::LoadData(const std::string& filepath, const std::string& month)
{
    bool bSuccess = false;
    
    m_month = month;
    
    // Get the file dates for the specified month.
    std::set<std::string> filedates;
    GetFileDates(filepath, m_month, filedates);
    
    for (std::set<std::string>::const_iterator it = filedates.begin(); it != filedates.end(); ++it)
    {
        BandwidthDay* day = new BandwidthDay();
        day->LoadData(*it);
        m_days[*it] = day;
    }
    
    // Build our daily averages.
    std::map<std::string, std::vector<double>> averages;
    
    for (std::map<std::string, BandwidthDay*>::const_iterator it_day = m_days.begin() ; it_day != m_days.end() ; ++it_day)
    {
        for (std::map<std::string, BandwidthDataPoint*>::const_iterator it_dp = it_day->second->DataPoints().begin() ; it_dp != it_day->second->DataPoints().end() ; ++it_dp)
        {
            std::map<std::string, std::vector<double>>::const_iterator it_time = averages.find(it_dp->first);
            if (it_time == averages.end())
            {
                averages[it_dp->first] = std::vector<double>();
            }
            averages[it_dp->first].push_back(it_dp->second->Bandwidth());
        }
    }
    
    for (std::map<std::string, std::vector<double>>::const_iterator it_average = averages.begin() ; it_average != averages.end() ; ++it_average)
    {
        double high = 0.0;
        double low = 80.0;
        double average = 0.0;
        for (std::vector<double>::const_iterator it_vec = it_average->second.begin() ; it_vec != it_average->second.end() ; ++it_vec)
        {
            if (*it_vec > high) high = *it_vec;
            if (*it_vec < low) low = *it_vec;
            average += *it_vec;
        }
        average = average / it_average->second.size();
        
        m_stats[it_average->first] = new BandwidthStatistics(it_average->first, average, high, low);
    }
    
    return bSuccess;
}