#ifndef __JSONCLIENT_H___
#define __JSONCLIENT_H__

#include <QThread>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QList>
#include <QPair>

class QSslError;
class QAuthenticator;
class QNetworkReply;
class QString;

typedef QList<QPair<QString, QString> > CJsonMap;

Q_DECLARE_METATYPE(CJsonMap *);

class JsonClient : public QObject
{
    Q_OBJECT
    public:
        JsonClient(QObject *parent = 0);
        virtual ~JsonClient();

        bool getJson(QString &urlString, CJsonMap *result);

    signals: 
        void getFinish(CJsonMap *result);


    private:
        QNetworkAccessManager *qnam;

        void parseJson(QByteArray &byteArray, CJsonMap *result);
        void parseJsonDocument(QJsonDocument &jsonDocument, CJsonMap *result);
        void parseJsonObject(QJsonObject &jsonObject, CJsonMap *result);
        void parseJsonValue(QJsonValue &jsonValue, QString *value, CJsonMap *result);
        void parseJsonArray(QJsonArray &jsonArray, QString *value, CJsonMap *result);
        void parseCustomFormat(QByteArray &byteArray, CJsonMap *result);

        void waitForFinish(int seconds, QNetworkReply *reply);

        private slots:
            void httpFinished();
            void httpReadyRead();
            void updateDataReadProgress(qint64 bytesRead, qint64 totalBytes);
};


#endif

