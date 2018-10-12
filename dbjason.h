#ifndef DBJASON_H
#define DBJASON_H
#include <ArduinoJson.h>

class DBjason
{
public:
    DBjason();
    JsonObject& loadFile(const char* fileName);
    void saveFile(const char* fileName, JsonObject& root);
    JsonObject& createEmptyDB();
    const char* getFileName();
    void freeBuffer();

private:
    DynamicJsonBuffer* m_pjBuffer;
    JsonObject& _addSwich(JsonArray& swiches);
};

#endif // DBJASON_H
