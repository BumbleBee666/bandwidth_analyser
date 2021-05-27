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

#include "BandwidthDay.h"
#include "BandwidthFile.h"

BandwidthDay::BandwidthDay()
{
}

BandwidthDay::BandwidthDay(const BandwidthDay& orig)
{
}

BandwidthDay::~BandwidthDay() {
}

bool BandwidthDay::LoadData(const std::string& date)
{
    bool bSuccess = false;
    m_date = date;
    
    const std::string directory = "//home//mark//Documents//bandwidth";
    
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
                if (filedate.compare(date) == 0)
                {
                    BandwidthDataPoint* datapoint = BandwidthFile::LoadFile(directory + "//" + filename);
                    if (datapoint != NULL)
                    {
                        std::string filetime = datapoint->Time().substr(0,2) + datapoint->Time().substr(3,2);
                        m_bandwidthData.insert(std::pair<std::string, BandwidthDataPoint*>(filetime.c_str(), datapoint));
                    }
                }
            }            
        }
        closedir (dir);
    }
    
    return bSuccess;
}
