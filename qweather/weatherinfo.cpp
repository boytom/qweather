#include "weatherinfo.h"
#include <QDebug>


    WeatherInfo::WeatherInfo(QObject *parent)
:JsonClient(parent)
{

}

WeatherInfo::~WeatherInfo()
{

}

const CJsonMap *WeatherInfo::getCityCode(CityType cityType, QString *upGradeCityCode)
{
    CJsonMap *result;
    QString url, urlOld;
    CJsonMap shengCodeOld, shiCodeOld, xianCodeOld;

    result = NULL;
    if (cityType == SHENG) {
        url = "http://m.weather.com.cn/data5/city.xml";
        urlOld = "http://www.weather.com.cn/data/list3/city.xml?level=1";
        result = &shengList;
    }
    else {
        if(upGradeCityCode != NULL && !upGradeCityCode->isEmpty()) {
            url = "http://m.weather.com.cn/data5/city%1.xml";
            url = url.arg(*upGradeCityCode);
            urlOld = "http://www.weather.com.cn/data/list3/city%1.xml?level=2";
            urlOld = urlOld.arg(*upGradeCityCode);
            switch(cityType) {
                case DIQUSHI:
                    result = &diQuShiList;
                    break;
                case XIAN:
                    result = &xianList;
                    break;
                case CITYCODE:
                    result = &cityCode;
                    break;
                default:
                    result = &shengList;
                    break;
            }
        }
    }
    if(result) {
        result->clear();
        getJson(url, result);
    }
    return (const CJsonMap *)result;
}

const CJsonMap *WeatherInfo::getWeather(QString *cityCode)
{
    QString url;
    url = "http://www.weather.com.cn/data/cityinfo/%1.html";
    url = url.arg(*cityCode);

    weather.clear();
    getJson(url, &weather);
    return (const CJsonMap *)&weather;
}

const CJsonMap *WeatherInfo::getRealTimeWeather(QString *cityCode)
{
    QString url;
    url = "http://www.weather.com.cn/adat/sk/%1.html";
    url = url.arg(*cityCode);

    realtimeWeather.clear();
    getJson(url, &realtimeWeather);
    return (const CJsonMap *)&realtimeWeather;
}

const CJsonMap *WeatherInfo::getDays6Weather(QString *cityCode)
{
    QString url;
    url = "http://m.weather.com.cn/data/%1.html";
    url = url.arg(*cityCode);

    days6Weather.clear();
    getJson(url, &days6Weather);
    return (const CJsonMap *)&days6Weather;
}

void WeatherInfo::getResult(CJsonMap *result)
{

}

void WeatherInfo::getCityCodeBySelfIp(QString *cityCode)
{
    QString url = "http://61.4.185.48:81/g/";
}
