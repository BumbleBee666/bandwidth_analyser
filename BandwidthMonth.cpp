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

#include "BandwidthMonth.h"
#include "BandwidthDataPoint.h"
#include "BandwidthDay.h"

BandwidthMonth::BandwidthMonth() {
}

BandwidthMonth::BandwidthMonth(const BandwidthMonth& orig) {
}

BandwidthMonth::~BandwidthMonth() {
}

bool BandwidthMonth::LoadData(const std::string& date)
{
    bool bSuccess = false;
    m_date = date;
    
    const std::string directory = "//home//mark//Documents//bandwidth";
    
    std::set<std::string> filedates;
    
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (directory.c_str())) != NULL)
    {
        while ((ent = readdir (dir)) != NULL) 
        {
            const std::string filename(ent->d_name);
            int pos = filename.find("results_", 0);
            
            if (pos >= 0)
            {
                const std::string filedate = filename.substr(pos+8, 8);
                if (filedate.substr(0, 6).compare(date) == 0)
                {
                    filedates.insert(filedate);
                }
            }            
        }
        closedir (dir);
    }
    
    for (std::set<std::string>::iterator it = filedates.begin(); it != filedates.end(); ++it)
    {
        BandwidthDay* day = new BandwidthDay();
        day->LoadData(*it);
        m_days.insert(std::pair<int, BandwidthDay*>(std::atoi((*it).c_str()), day));
    }
    
    // Build our daily averages.
    std::map<int, std::vector<double>> averages;
    
    for (std::map<int, BandwidthDay*>::iterator it = m_days.begin() ; it != m_days.end() ; ++it)
    {
        for (std::map<int, BandwidthDataPoint*>::const_iterator it_dps = (*it).second->Data().begin() ; it_dps != (*it).second->Data().end() ; ++it_dps)
        {
            std::map<int, std::vector<double>>::iterator it_time = averages.find((*it_dps).first);
            if (it_time == averages.end())
            {
                averages[(*it_dps).first] = std::vector<double>();
            }
            averages[(*it_dps).first].push_back((*it_dps).second->Bandwidth());
        }
    }
    
    for (std::map<int, std::vector<double>>::iterator it = averages.begin() ; it != averages.end() ; ++it)
    {
        double result = 0.0;
        for (std::vector<double>::iterator it_vec = (*it).second.begin() ; it_vec != (*it).second.end() ; ++it_vec)
        {
            result += *it_vec;
        }
        result = result / (*it).second.size();
        
        m_bandwidthData[(*it).first] = result;
    }
    
    return bSuccess;
}