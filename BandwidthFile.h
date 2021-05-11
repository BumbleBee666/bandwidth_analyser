/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   BandwidthFile.h
 * Author: mark
 *
 * Created on 10 May 2021, 17:16
 */

#ifndef BANDWIDTHFILE_H
#define BANDWIDTHFILE_H

#include <vector>

#include "BandwidthDataPoint.h"

class BandwidthFile {
public:
    BandwidthFile();
    BandwidthFile(const BandwidthFile& orig);
    virtual ~BandwidthFile();
    
    static BandwidthDataPoint* LoadFile (const std::string& filename);
    
private:
    static void GetFields(const std::string& inLine, std::vector <std::string>& fields);
};

#endif /* BANDWIDTHFILE_H */

