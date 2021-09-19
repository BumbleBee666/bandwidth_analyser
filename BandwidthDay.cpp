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

BandwidthDay::BandwidthDay(const std::string& date)
:m_date(date)
{
}

BandwidthDay::BandwidthDay(const BandwidthDay& orig)
{   
    this->m_date = orig.m_date;
    for (auto const& it : orig.m_bandwidthData)
    {
        m_bandwidthData[it.first] = std::make_unique<BandwidthDataPoint>(*it.second);
    }
}

BandwidthDay::~BandwidthDay() {
}

void BandwidthDay::LoadDataPoint(const std::string& directory, const std::string& filename)
{
    auto datapoint = BandwidthFile::LoadFile(directory + "//" + filename);
    if (datapoint != NULL)
    {
        std::string filetime = datapoint->Time().substr(0,2) + datapoint->Time().substr(3,2);
        m_bandwidthData[filetime] = std::move(datapoint);
    }
}

const std::map<std::string, const std::unique_ptr<const BandwidthDataPoint>>& BandwidthDay::DataPoints() const
{ 
    return (const std::map<std::string, const std::unique_ptr<const BandwidthDataPoint>>&)m_bandwidthData; 
}
   

const std::string BandwidthDay::to_json() const
{
    const std::string newline = "\n\r";
    const std::string continuation = ",";
    
    std::string json;

    json += "[" + newline;

    int i = 0;
    for (auto const& data : m_bandwidthData)
    {
        json += "{" + newline;
        json += "\"Time\" : \"" + data.first + "\"" + continuation + newline;
        json += "\"Bandwidth\" : " + std::to_string(data.second->Bandwidth()) + newline;
        json += "}" + (++i == m_bandwidthData.size() ? "" : continuation) + newline;
    }
    
    json += "]";
    
    return json;
}

void BandwidthDay::from_json(const std::string& json)
{
    std::cout << "Blah";
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
                    auto datapoint = BandwidthFile::LoadFile(directory + "//" + filename);
                    if (datapoint != NULL)
                    {
                        std::string filetime = datapoint->Time().substr(0,2) + datapoint->Time().substr(3,2);
                        m_bandwidthData[filetime] = std::move(datapoint);
                    }
                }
            }            
        }
        closedir (dir);
    }
}
