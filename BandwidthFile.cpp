/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   BandwidthFile.cpp
 * Author: mark
 * 
 * Created on 10 May 2021, 17:16
 */

#include <fstream>
#include <vector>
#include <algorithm>
#include <string>

#include "BandwidthFile.h"
#include "BandwidthDataPoint.h"

BandwidthFile::BandwidthFile() {
}

BandwidthFile::BandwidthFile(const BandwidthFile& orig) {
}

BandwidthFile::~BandwidthFile() {
}

std::unique_ptr<std::vector<std::string>> BandwidthFile::GetFields(const std::string& inLine)
{
    auto fields = std::unique_ptr<std::vector<std::string>>(new std::vector<std::string>());
    
    bool bInField = false;
    std::string strField;
    for (auto const& it : inLine)
    {
        if (it == '"')
        {
            bInField = !bInField;
        }
        else if (it == ',' && !bInField)
        {
            fields->push_back(strField);
            strField = "";
        }
        else
        {
            strField += it;
        }
    }
    fields->push_back(strField);
    
    return fields;
}

std::unique_ptr<BandwidthDataPoint> BandwidthFile::LoadFile (const std::string& filename)
{
    auto datapoint = std::unique_ptr<BandwidthDataPoint>();
    
    try
    {
        std::fstream inFile(filename);
        std::string inLine;
        std::string::size_type sz;
        getline (inFile, inLine);
        if (inLine.length() > 0)
        {
            auto fields = GetFields(inLine);
            double bandwidth = std::stod((*fields)[5]) / 125000.0;
            
            int pos = filename.find("results_", 0);
            std::string filedate = filename.substr(pos+8, 8);
            std::string filetime = filename.substr(pos+17, 5);
            
            datapoint = std::unique_ptr<BandwidthDataPoint>(new BandwidthDataPoint(filedate, filetime, bandwidth));
        }
        inFile.close();
    }
    catch (...)
    {
    }
    
    return datapoint;
}
