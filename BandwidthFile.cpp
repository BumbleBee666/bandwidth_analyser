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

void BandwidthFile::GetFields(const std::string& inLine, std::vector <std::string>& fields)
{
    bool bInField = false;
    std::string strField;
    for (auto it=inLine.begin();it!=inLine.end();++it)
    {
        if (*it == '"')
        {
            bInField = !bInField;
        }
        else if (*it == ',' && !bInField)
        {
            fields.push_back(strField);
            strField = "";
        }
        else
        {
            strField += *it;
        }
    }
    fields.push_back(strField);
}

BandwidthDataPoint* BandwidthFile::LoadFile (const std::string& filename)
{
    BandwidthDataPoint* datapoint = NULL;
    
    try
    {
        std::fstream inFile(filename);
        std::string inLine;
        std::string::size_type sz;
        getline (inFile, inLine);
        if (inLine.length() > 0)
        {
            std::vector <std::string> fields;
            GetFields(inLine, fields);
            double bandwidth = std::stod(fields[5]) / 125000.0;
            
            int pos = filename.find("results_", 0);
            std::string filedate = filename.substr(pos+8, 8);
            std::string filetime = filename.substr(pos+17, 5);
            
            datapoint = new BandwidthDataPoint(filedate, filetime, bandwidth);
        }
        inFile.close();
    }
    catch (...)
    {
        if (datapoint) delete datapoint;
        datapoint = NULL;
    }
    
    return datapoint;
}
