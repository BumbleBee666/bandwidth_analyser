/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   BandwidthDataListener.h
 * Author: mark
 *
 * Created on 20 June 2021, 13:42
 */

#ifndef BANDWIDTHDATALISTENER_H
#define BANDWIDTHDATALISTENER_H

class BandwidthDataListener 
{
public:
    virtual void BandwidthUpdated() = 0;
    virtual ~BandwidthDataListener() {}
};

#endif /* BANDWIDTHDATALISTENER_H */

