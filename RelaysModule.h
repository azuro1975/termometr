#ifndef RELAYS_MODULE_H
#include <memory>
#include <LiquidCrystal_I2C.h>
#include <ArduinoJson.h>

class CRelaysModule
{
    static constexpr int m_swiches = 4;
    static constexpr int m_timeSize =7;

  public:
    CRelaysModule();
    virtual ~CRelaysModule();
    
    void init();
    void yellowOnOff(bool bSet);
    void orangrOnOff(bool bSet);
    void greenOnOff(bool bSet);
    void blueOnOff(bool bSet);
    bool getYellow() { return m_yellowState;}
    bool getOrangr() { return m_orangrState;}
    bool getGreen() { return m_greenState;}
    bool getBlue() { return m_blueState;}
    void connectLCD(std::shared_ptr<LiquidCrystal_I2C> pLCD);
    void refrashLCD();
    void jsonToMem(JsonObject& root);
    void memToJson(JsonObject& root);
    void tickTime(time_t now);
    
  private:
    int m_yellowPIN;
    int m_orangrPIN;
    int m_greenPIN;
    int m_bluePIN;
    bool m_yellowState;
    bool m_orangrState;
    bool m_greenState;
    bool m_blueState;         
    std::shared_ptr<LiquidCrystal_I2C> m_LCD;
    time_t m_timeOn[m_swiches][m_timeSize];
    time_t m_timeOff[m_swiches][m_timeSize];
};
#endif //RELAYS_MODULE_H
