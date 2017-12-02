#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QComboBox>
#include <QStatusBar>
#include <QTextEdit>
#include <QDebug>
#include <QLabel>
#include <QGroupBox>
#include <QTabBar>

#include "weatherwin.h"

  WeatherWin::WeatherWin(QWidget *parent)
: QWidget(parent)
{
  sheng = new QComboBox(this);
  diqushi = new QComboBox(this);
  xian = new QComboBox(this);
  weatherOption = new QComboBox(this);

  weatherOption->addItem("实时", QVariant::fromValue<int>(0));
  weatherOption->addItem("1天", QVariant::fromValue<int>(1));
  weatherOption->addItem("2天", QVariant::fromValue<int>(2));
  weatherOption->addItem("3天", QVariant::fromValue<int>(3));
  weatherOption->addItem("6天", QVariant::fromValue<int>(6));

  sheng->setObjectName("sheng");
  diqushi->setObjectName("diqushi");
  xian->setObjectName("xian");

  sheng->setEnabled(false);
  diqushi->setEnabled(false);
  xian->setEnabled(false);

  QHBoxLayout *hlayout = new QHBoxLayout;

  QGroupBox *groupBox = new QGroupBox("省／直辖市");
  QVBoxLayout *vlayout = new QVBoxLayout;
  vlayout->addWidget(sheng);
  groupBox->setLayout(vlayout);

  hlayout->addWidget(groupBox);

  vlayout = new QVBoxLayout;
  groupBox = new QGroupBox("地区／市");
  vlayout->addWidget(diqushi);
  groupBox->setLayout(vlayout);

  hlayout->addWidget(groupBox);

  vlayout = new QVBoxLayout;
  groupBox = new QGroupBox("市／县");
  vlayout->addWidget(xian);
  groupBox->setLayout(vlayout);

  hlayout->addWidget(groupBox);

  vlayout = new QVBoxLayout;
  groupBox = new QGroupBox("预报时间");
  vlayout->addWidget(weatherOption);
  groupBox->setLayout(vlayout);

  hlayout->addWidget(groupBox);
  hlayout->addStretch();

  statusBar = new QStatusBar(this);
  statusBar->setObjectName("statusBar");

  tempShow = new QTextEdit;

  vlayout = new QVBoxLayout;
  vlayout->addLayout(hlayout);
  vlayout->addWidget(tempShow);
  vlayout->addWidget(statusBar);
  vlayout->setAlignment(statusBar, Qt::AlignBottom);
  vlayout->setContentsMargins(0, 0, 0, 0);

  setLayout(vlayout);

  weatherInfo = new WeatherInfo(this);

#ifdef _WEATHER_DEBUG_
  const CJsonMap *result = weatherInfo->getCityCode(WeatherInfo::SHENG);
  QString cityCode("0101");
  result = weatherInfo->getCityCode(WeatherInfo::XIAN, &cityCode);
  CJsonMap::const_iterator citer;

  for(citer = result->constBegin();
      citer != result->constEnd();
      citer++)
    qDebug() << citer.key() << ":" << citer.value();
#endif
  addCityToCombo();
  updateDiqushi(0);
  updateXian(0);
  updateWeather(0);
  connect(sheng, SIGNAL(currentIndexChanged(int)), this, SLOT(updateDiqushi(int)));
  connect(diqushi, SIGNAL(currentIndexChanged(int)), this, SLOT(updateXian(int)));
  connect(xian, SIGNAL(currentIndexChanged(int)), this, SLOT(updateWeather(int)));
  connect(weatherOption, SIGNAL(currentIndexChanged(int)), 
      this, SLOT(updateWeather(int)));
  statusBar->showMessage(tr("就绪"));
  //setWindowFlags(Qt::Window| Qt::FramelessWindowHint);
  //setAttribute(Qt::WA_TranslucentBackground);
}

void WeatherWin::addCityToCombo(void)
{
  const CJsonMap *list;
  CJsonMap::const_iterator citer;

  list = weatherInfo->getCityCode(WeatherInfo::SHENG);
  sheng->clear();
  for(citer = list->constBegin(); citer != list->constEnd(); citer++)
    sheng->addItem((*citer).second, QVariant::fromValue<QString>((*citer).first));
  sheng->setEnabled(true);
}

void WeatherWin::updateDiqushi(int index)
{
  const CJsonMap *list;
  CJsonMap::const_iterator citer;

  if(index < 0)
    return;

  QString currentCityCode = sheng->itemData(index).toString();

  list = weatherInfo->getCityCode(WeatherInfo::DIQUSHI, &currentCityCode);
  diqushi->clear();
  for(citer = list->constBegin(); citer != list->constEnd(); citer++)
    diqushi->addItem((*citer).second, 
        QVariant::fromValue<QString>((*citer).first));
  diqushi->setEnabled(true);
}

void WeatherWin::updateXian(int index)
{
  const CJsonMap *list;
  CJsonMap::const_iterator citer;

  if(index < 0)
    return;

  QString currentCityCode = diqushi->itemData(index).toString();

  list = weatherInfo->getCityCode(WeatherInfo::XIAN, &currentCityCode);
  xian->clear();
  for(citer = list->constBegin(); citer != list->constEnd(); citer++)
    xian->addItem((*citer).second, QVariant::fromValue<QString>((*citer).first));
  xian->setEnabled(true);
}

void WeatherWin::updateWeather(int index)
{
  const CJsonMap *weatherList, *cityCodeList;

  if(index < 0)
    return;

  QString currentCityCode = xian->itemData(xian->currentIndex()).toString();
  cityCodeList = weatherInfo->getCityCode(WeatherInfo::CITYCODE, &currentCityCode);

  currentCityCode = (*cityCodeList->constBegin()).second;

  switch(weatherOption->currentIndex()) {
    case 0:
      weatherList = weatherInfo->getRealTimeWeather(&currentCityCode);
      break;
    case 1: case 2: case 3: case 4: default:
      weatherList = weatherInfo->getDays6Weather(&currentCityCode);
  }

  tempShow->clear();
  updateWeatherShow(weatherList);
}

void WeatherWin::updateWeatherShow(const CJsonMap *weatherList)
{
  CJsonMap::const_iterator citer;
  QString weatherText;
  int days;
  const char *weatherItem1[] = {"fl", "temp", "weather", "wind", "tempF", "st"}; 
  const char *weatherItem2[] = {"city", "index", "index48", "index48_d", 
    "index48_uv", "index_ag", "index_cl", "index_co", "index_d", "index_ls", 
    "index_tr", "index_uv", "index_xc", "cityid", "date", "week"};

  days = weatherOption->itemData(weatherOption->currentIndex()).toInt();
  if(days == 0) {
    for(citer = weatherList->constBegin();
        citer != weatherList->constEnd();
        citer++) {
      weatherText += citer->first;
      weatherText += "=";
      weatherText += (*citer).second;
      weatherText += "\n";
    }
    tempShow->append(weatherText);
    return;
  }
  for(citer = weatherList->constBegin(); 
      citer != weatherList->constEnd();
      citer++) {
    int j;
    for(j = 0; j < sizeof(weatherItem2) / sizeof(weatherItem2[0]); j++)
      if(citer->first == weatherItem2[j]) {
        weatherText += citer->first;
        weatherText += "=";
        weatherText += (*citer).second;
        weatherText += "\n";
        //weatherList->removeOne(*citer);
      } 
  }
  for(int i = 1; i <= days; i++) {
    for(citer = weatherList->constBegin(); citer != weatherList->constEnd();
        citer++) {
      int j;
      for(j = 0; j < sizeof(weatherItem1) / sizeof(weatherItem1[0]); j++) {
        QString stmp;

        stmp.append(weatherItem1[j]).append(i + '0');

        if(citer->first == stmp) {
          weatherText += citer->first;
          weatherText += "=";
          weatherText += (*citer).second;
          weatherText += "\n";
        }
      }
    }
  }
  tempShow->append(weatherText);
  for(citer = weatherList->constBegin(); citer != weatherList->constEnd();
      citer++)
    qDebug() << citer->first << " : " << citer->second;
}

