#ifndef __WEATHERINFO_H__
#define __WEATHERINFO_H__

#include "jsonclient.h"

class QString;

class WeatherInfo : public JsonClient
{
    Q_OBJECT
    public:
        WeatherInfo(QObject *parent = 0);
        ~WeatherInfo();

        typedef enum {SHENG, DIQUSHI, XIAN, CITYCODE} CityType;
        //enum CityType {SHENG,SHI, XIAN} ;
        const CJsonMap *getCityCode(CityType cityType, QString *upGradeCityCode = NULL);
        const CJsonMap *getWeather(QString *cityCode);
        const CJsonMap *getRealTimeWeather(QString *cityCode);
        const CJsonMap *getDays6Weather(QString *cityCode);
        void getCityCodeBySelfIp(QString *cityCode);

    private:
        CJsonMap shengList, diQuShiList, xianList, cityCode;
        CJsonMap weather, realtimeWeather, days6Weather;

    private slots:
        void getResult(CJsonMap *result);

};



#endif


