#include "weatherwin.h"
#include <QApplication>
#include "weatherinfo.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    WeatherWin *weatherWin = new WeatherWin;

    weatherWin->show();
    return app.exec();
    /*
    WeatherThread *thread = new WeatherThread;
    thread->getWeather("101281601");
    thread->wait();
    return 0;
    */
}
