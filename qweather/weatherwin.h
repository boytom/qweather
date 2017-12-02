#ifndef __WEATHERWIN_H__
#define __WEATHERWIN_H__

#include <QWidget>
#include "weatherinfo.h"

class QComboBox;
class QStatusBar;
class QTextEdit;
class WeatherInfo;
class QTabBar;

#if defined(_MSC_VER)
#pragma execution_character_set("utf-8")
#endif

class WeatherWin : public QWidget
{
    Q_OBJECT

    public:
        WeatherWin(QWidget *parent = 0);
    private:
        QComboBox *sheng, *diqushi, *xian, *weatherOption;
        QStatusBar *statusBar;
        QTextEdit *tempShow;

        WeatherInfo *weatherInfo;

        void addCityToCombo(void);

    private slots:
        void updateDiqushi(int index);
        void updateXian(int index);
        void updateWeather(int index);
        void updateWeatherShow(const CJsonMap *weatherList);
};


#endif

