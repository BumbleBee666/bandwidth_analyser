#ifndef JSONBASE_H
#define JSONBASE_H

#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"

class JSONBase
{
public:	
    virtual ~JSONBase() {};
    bool DeserializeFromFile(const std::string& filePath);
    bool SerializeToFile(const std::string& filePath); 	

    std::string Serialize() const;
    bool Deserialize(const std::string& s);
    virtual bool Deserialize(const rapidjson::Value& obj) = 0;
    virtual bool Serialize(rapidjson::Writer<rapidjson::StringBuffer>* writer) const = 0;
protected:	
    bool InitDocument(const std::string & s, rapidjson::Document &doc);
};

#endif /* JSONBASE_H */

