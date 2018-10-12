#include "RelaysModule.h"
#include <Arduino.h>
#include <time.h>
#include <NtpClientLib.h>

CRelaysModule::CRelaysModule():
  m_yellowPIN(14),
  m_orangrPIN(12),
  m_greenPIN(13),
  m_bluePIN(15),
  m_yellowState(false),
  m_orangrState(false),
  m_greenState(false),
  m_blueState(false),
  m_LCD(nullptr)
{
}

CRelaysModule::~CRelaysModule()
{
}

void CRelaysModule::init()
{
  pinMode(m_yellowPIN, OUTPUT);
  pinMode(m_orangrPIN, OUTPUT);
  pinMode(m_greenPIN, OUTPUT);
  pinMode(m_bluePIN, OUTPUT);
  digitalWrite(m_yellowPIN, HIGH);
  digitalWrite(m_orangrPIN, HIGH);
  digitalWrite(m_greenPIN, HIGH);
  digitalWrite(m_bluePIN, HIGH);
}

void CRelaysModule::yellowOnOff(bool bSet)
{
    m_yellowState = bSet;
    digitalWrite(m_yellowPIN, bSet?LOW:HIGH);
    refrashLCD();
}

void CRelaysModule::orangrOnOff(bool bSet)
{
    m_orangrState = bSet;
    digitalWrite(m_orangrPIN, bSet?LOW:HIGH);
    refrashLCD();
}

void CRelaysModule::greenOnOff(bool bSet)
{
    m_greenState = bSet;
    digitalWrite(m_greenPIN, bSet?LOW:HIGH);
    refrashLCD();
}

void CRelaysModule::blueOnOff(bool bSet)
{
    m_blueState = bSet;
    digitalWrite(m_bluePIN, bSet?LOW:HIGH);
    refrashLCD();
}

void CRelaysModule::connectLCD(std::shared_ptr<LiquidCrystal_I2C> pLCD)
{
    m_LCD = pLCD;
}

void CRelaysModule::refrashLCD()
{
    if (!m_LCD)
        return;

    m_LCD->setCursor(12,1);
    m_LCD->print(m_yellowState?'o':'x');
    m_LCD->setCursor(13,1);
    m_LCD->print(m_orangrState?'o':'x');
    m_LCD->setCursor(14,1);
    m_LCD->print(m_greenState?'o':'x');
    m_LCD->setCursor(15,1);
    m_LCD->print(m_blueState?'o':'x');
}

void CRelaysModule::jsonToMem(JsonObject &root)
{
    Serial.print(__PRETTY_FUNCTION__);
    for (int i = 0; i<m_swiches; ++i)
        for (int j = 0; j<m_timeSize;++j)
        {
            Serial.printf("root[%d][%d] On:%s Off:%s\n\r",i,j, NTP.getTimeStr(m_timeOn[i][j]).c_str(), NTP.getTimeStr(m_timeOff[i][j]).c_str());
            m_timeOn[i][j] = root["Switches"][i]["TimeOn"][j];
            m_timeOff[i][j] = root["Switches"][i]["TimeOff"][j];
        }
}

void CRelaysModule::memToJson(JsonObject &root)
{
    for (int i = 0; i<m_swiches; ++i)
        for (int j = 0; j<m_timeSize;++j)
        {
            root["Switches"][i]["TimeOn"][j] = m_timeOn[i][j] ;
            root["Switches"][i]["TimeOff"][j] = m_timeOff[i][j];
        }
}

void CRelaysModule::tickTime(time_t now)
{
    int idx = weekday(now);
    Serial.printf("Today is %d day of week\n\r",idx);
    for (int i = 0; i < m_swiches; ++i)
    {
        switch (i) {
        case 0:
            if (getYellow())
            {
                Serial.printf("YellowOff[%d][%d]:%s\n\r",i,idx,NTP.getTimeStr(m_timeOff[i][idx]).c_str());
                if (now >= m_timeOff[i][idx])
                    yellowOnOff(false);
            }
            else {
                Serial.printf("YellowOn[%d][%d]:%s\n\r",i,idx,NTP.getTimeStr(m_timeOn[i][idx]).c_str());
                if (now >= m_timeOn[i][idx])
                    yellowOnOff(true);
            }
            break;
        case 1:
            if (getOrangr())
            {
                if (now >= m_timeOff[i][idx])
                    orangrOnOff(false);
            }
            else {
                if (now >= m_timeOn[i][idx])
                    orangrOnOff(true);
            }
            break;
        case 2:
            if (getGreen())
            {
                if (now >= m_timeOff[i][idx])
                    greenOnOff(false);
            }
            else {
                if (now >= m_timeOn[i][idx])
                    greenOnOff(true);
            }
            break;
        case 3:
            if (getBlue())
            {
                if (now >= m_timeOff[i][idx])
                    blueOnOff(false);
            }
            else {
                if (now >= m_timeOn[i][idx])
                    blueOnOff(true);
            }
            break;
        default:
            Serial.printf("Error no switch %d number\n\r",i);
            break;
        }
    }
}
