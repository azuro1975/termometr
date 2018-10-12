#include "dbjason.h"
#include "FS.h"
#include <memory>
#include <TimeLib.h>

#define FILE_DB_NAME "Data.json"

DBjason::DBjason():
    m_pjBuffer(nullptr)
{
}

JsonObject &DBjason::loadFile(const char *fileName)
{
    Serial.printf("%s\n\r",__PRETTY_FUNCTION__);
    //DynamicJsonBuffer jsonBuffer;
    if (!SPIFFS.exists(fileName))
    {
        Serial.printf("ERROR %s not exist.\n\r", fileName);
        return JsonObject::invalid();
    }

    File dbFile = SPIFFS.open(fileName, "r");
    if (!dbFile)
    {
        Serial.printf("ERROR %s not opend.\n\r", fileName);
        return JsonObject::invalid();
    }

    if (dbFile.size()<=0)
    {
        Serial.printf("%s is empty.\n\r", fileName);
        dbFile.close();
        return JsonObject::invalid();
    }
    if (m_pjBuffer)
        m_pjBuffer->clear();
    else
        m_pjBuffer = new DynamicJsonBuffer(dbFile.size());
    JsonObject& root = m_pjBuffer->parseObject(dbFile);
    dbFile.close();
    root.prettyPrintTo(Serial);
    return root;
}

void DBjason::saveFile(const char *fileName, JsonObject& root )
{
    Serial.printf("%s\n\r",__PRETTY_FUNCTION__);
    File dbFile = SPIFFS.open(fileName, "w");
    if (!dbFile)
    {
        Serial.printf("ERROR %s not opend.\n\r", fileName);
        return;
    }
    root.prettyPrintTo(dbFile);
    dbFile.close();
}

JsonObject &DBjason::_addSwich(JsonArray& swiches)
{
    Serial.printf("%s\n\r",__PRETTY_FUNCTION__);
    JsonObject& jSwitch = swiches.createNestedObject();
    jSwitch["name"] = "Wł";
    jSwitch["state"] = false;
    jSwitch["OnOff"] = -1;
    JsonArray& aTimeOn = jSwitch.createNestedArray("TimeOn");
    JsonArray& aTimeOff = jSwitch.createNestedArray("TimeOff");
    setTime(8,0,0,0,0,0);
    aTimeOn.add(now());
    aTimeOn.add(now());
    aTimeOn.add(now());
    aTimeOn.add(now());
    aTimeOn.add(now());
    setTime(9,0,0,0,0,0);
    aTimeOn.add(now());
    aTimeOn.add(now());
    setTime(21,0,0,0,0,0);
    aTimeOff.add(now());
    aTimeOff.add(now());
    aTimeOff.add(now());
    aTimeOff.add(now());
    aTimeOff.add(now());
    aTimeOff.add(now());
    aTimeOff.add(now());

    return jSwitch;
}

JsonObject &DBjason::createEmptyDB()
{
    Serial.printf("%s\n\r",__PRETTY_FUNCTION__);

    DynamicJsonBuffer jsonBuffer;
    JsonObject & root = jsonBuffer.createObject();
    setTime(8,0,0,0,0,0);
    root["Time"] = now();
    root["Light"] = "0.00";
    root["Temperature"] = "0.00";
    JsonArray& swiches = root.createNestedArray("Switches");
    _addSwich(swiches);
    _addSwich(swiches);
    _addSwich(swiches);
    _addSwich(swiches);

    swiches[0]["name"]="Wł1";
    swiches[1]["name"]="Wł2";
    swiches[2]["name"]="Wł3";
    swiches[3]["name"]="Wł4";

    root.prettyPrintTo(Serial);

    return root;
}

const char *DBjason::getFileName()
{
    return FILE_DB_NAME;
}

void DBjason::freeBuffer()
{
    if (m_pjBuffer)
    {
        delete m_pjBuffer;
        m_pjBuffer = nullptr;
    }
}
