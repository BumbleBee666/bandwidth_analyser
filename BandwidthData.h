/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   BandwidthData.h
 * Author: mark
 *
 * Created on 2 June 2021, 19:09
 */

#ifndef BANDWIDTHDATA_H
#define BANDWIDTHDATA_H

#include <map>
#include <set>
#include <vector>
#include <string>
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <memory>

#include "JSONBase.h"
#include "BandwidthStatistics.h"
#include "BandwidthDay.h"
#include "BandwidthDataListener.h"

class BandwidthData : public JSONBase
{
public:
    BandwidthData(const std::string& filepath);
    BandwidthData(const BandwidthData& orig);
    virtual ~BandwidthData();
    
    // These are returning references to object attributes.
    const BandwidthDay& GetDay(const std::string& day) const;
    const std::map<std::string, const std::unique_ptr<const BandwidthDay>>& GetDays() const;
    int GetNoOfDays() const;
    
    // These are returning objects and giving ownership of those objects to the caller.
    std::unique_ptr<std::set<std::string>> GetMonths() const;
    std::unique_ptr<std::map<std::string, std::unique_ptr<BandwidthStatistics>>> GetStatistics() const;
    std::unique_ptr<std::map<std::string, std::unique_ptr<BandwidthStatistics>>> GetStatistics(const std::string& day) const;
    std::unique_ptr<std::map<std::string, std::unique_ptr<BandwidthStatistics>>> GetStatistics(const std::string& startDay, const std::string& endDay) const;
      
    // Registered listeners will receive a callback when the bandwidth data is updated.
    void RegisterListener(std::shared_ptr<BandwidthDataListener> listener);

    virtual bool Deserialize(const rapidjson::Value& obj);
    virtual bool Serialize(rapidjson::Writer<rapidjson::StringBuffer>* writer) const;
    
private:
    std::unique_ptr<std::set<std::string>> GetFileNames() const;

    std::unique_ptr<std::map<std::string, std::unique_ptr<BandwidthStatistics>>> GetStatistics(std::map<std::string, std::vector<double>>& bandwidthsByTime) const;
        
    std::vector<std::shared_ptr<BandwidthDataListener>> m_listeners;

    std::string m_filepath;
    std::set<std::string> m_filenames;
    std::map<std::string, std::unique_ptr<BandwidthDay>> m_days;

    std::atomic<bool> m_finishThread;
    void UpdateThread();
    std::unique_ptr<std::thread> m_updateThread;
    std::mutex m_threadLock;

    template<class Duration>
    bool wait_for(Duration duration);

    std::condition_variable c_;
    std::mutex m_;
    bool stop_;
};

#endif /* BANDWIDTHDATA_H */

