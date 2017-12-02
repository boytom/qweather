#include "jsonclient.h"
#include <QString>
#include <QSslError>
#include <QAuthenticator>
#include <QNetworkReply>
#include <QDebug>
#include <QMessageBox>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QStringList>
#include <QEventLoop>

static const QNetworkRequest::Attribute jsonAttribute = QNetworkRequest::User;
/*
   JsonClient::JsonClient(QObject *parent)
   {
   url[0] = "http://www.weather.com.cn/data/cityinfo/%1.html";
   url[1] = "http://www.weather.com.cn/data/sk/%1.html";
   url[2] = "http://m.weather.com.cn/data/%1.html";
   }
   */

    JsonClient::JsonClient(QObject *parent)
: QObject(parent)
{
    qnam = new QNetworkAccessManager(this);
}


JsonClient::~JsonClient()
{
    delete qnam;
    qnam = 0;
}

bool JsonClient::getJson(QString &urlString, CJsonMap *result)
{
    /*
       if(weatherInfo == NULL)
       return false;
       */
    QNetworkReply *reply;
    QNetworkRequest request;

    request.setAttribute(jsonAttribute,
            QVariant::fromValue<CJsonMap *>(result));
    request.setUrl(QUrl(urlString));
    request.setRawHeader("User-Agent", "Autoupdater");

    reply = qnam->get(request);

    if(reply->error()) {
        qDebug() << reply->errorString();
        return false;
    }

    connect(reply, SIGNAL(finished()), this, SLOT(httpFinished()));
    connect(reply, SIGNAL(readyRead()), this, SLOT(httpReadyRead()));
    connect(reply, SIGNAL(downloadProgress(qint64, qint64)),
            this, SLOT(updateDataReadProgress(qint64, qint64)));
    waitForFinish(1, reply);
    return true;
}


void JsonClient::httpFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    CJsonMap *result = reply->request().attribute(jsonAttribute) .value<CJsonMap *>();
    QByteArray byteArray = reply->readAll();

    QVariant redirectionTarget = 
        reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if(reply->error())
        qDebug() << tr("Getting Weather Error. Get failed: %1.")
            .arg(reply->errorString());
    else if(!redirectionTarget.isNull()) {
        QUrl newUrl = reply->url().resolved(redirectionTarget.toUrl());
        qDebug() << tr("Redirection Not Support redirect to %1")
            .arg(newUrl.toString());
        return;
    }
    else {
        //qDebug() << tr("What should I do here?");
    }

    parseJson(byteArray, result);

    reply->deleteLater();
    reply->close();
    reply = 0;

    emit getFinish(result);
    //qDebug() << tr("Finished");
}

void JsonClient::httpReadyRead()
{

}

void JsonClient::updateDataReadProgress(qint64 bytesRead, qint64 totalBytes)
{
    //qDebug() << bytesRead << " " << totalBytes;
}

void JsonClient::parseJson(QByteArray &byteArray, CJsonMap *result)
{
    QJsonParseError jsonParseError;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(byteArray, &jsonParseError);

    if(jsonParseError.error != QJsonParseError::NoError) {
        //qDebug() << "Parse Error:" << jsonParseError.errorString() << byteArray;
        // 这里是取得了城市代码。城市代码不是用 JSON 返回的
        parseCustomFormat(byteArray, result);
        return;
    }

    parseJsonDocument(jsonDocument, result);

}

void JsonClient::parseJsonDocument(QJsonDocument &jsonDocument, CJsonMap *result)
{
    if(jsonDocument.isArray()) {
        QJsonArray jsonArray = jsonDocument.array();
        QString value;
        parseJsonArray(jsonArray, &value, result);
    }
    else if(jsonDocument.isObject()) {
        QJsonObject jsonObject = jsonDocument.object();
        parseJsonObject(jsonObject, result);
    }
}

void JsonClient::parseJsonObject(QJsonObject &jsonObject, CJsonMap *result)
{
    QJsonObject::const_iterator jsonObjectIter;

    QJsonValue jsonValue;
    QString value;

    for(jsonObjectIter = jsonObject.constBegin();
            jsonObjectIter != jsonObject.constEnd();
            jsonObjectIter++) {
        value.clear() ;
        jsonValue = jsonObjectIter.value();
        parseJsonValue(jsonValue, &value, result);
        result->append(qMakePair(jsonObjectIter.key(), value));
#ifdef _WEATHER_DEBUG_
        qDebug() << jsonObjectIter.key() << ":" <<  value;
#endif
    }
    /*
       QJsonObject::const_iterator weatherIter;
       for(weatherIter = weatherObject.constBegin();
       weatherIter != weatherObject.constEnd();
       weatherIter++) {
       qDebug() << (*weatherIter).toString();
       }
       */
}

void JsonClient::parseJsonValue(QJsonValue &jsonValue, QString *value, CJsonMap *result)
{
    QJsonObject jsonObject;
    QJsonArray jsonArray;

    switch(jsonValue.type()) {
        case QJsonValue::Array:
            //qDebug() << tr("Array");
            jsonArray = jsonValue.toArray();
            parseJsonArray(jsonArray, value, result);
            break;
        case QJsonValue::Bool:
            //qDebug() << tr("Bool");
            *value += QString(jsonValue.toBool());
            break;
        case QJsonValue::Double:
            //qDebug() << tr("Double");
            *value += QString("%1").arg(jsonValue.toDouble());
            break;
        case QJsonValue::Null:
            //qDebug() << tr("Null");
            *value += QString("NULL");
            break;
        case QJsonValue::Object:
            //qDebug() << tr("Object");
            jsonObject = jsonValue.toObject();
            parseJsonObject(jsonObject, result);
            break;
        case QJsonValue::String:
            *value += jsonValue.toString();
            break;
        case QJsonValue::Undefined:
            //qDebug() << tr("Undefined");
            *value += QString("Undefined");
            break;
    }

}

void JsonClient::parseJsonArray(QJsonArray &jsonArray, QString *value, CJsonMap *result)
{
    QJsonArray::const_iterator arrayIter;
    QJsonValue jsonValue;

    for(arrayIter = jsonArray.constBegin(); 
            arrayIter != jsonArray.constEnd();
            arrayIter++) {
        jsonValue = *arrayIter;
        parseJsonValue(jsonValue, value, result);
    }

}

void JsonClient::waitForFinish(int seconds, QNetworkReply *reply)
{
    QEventLoop eventLoop;

    if (seconds <= 0)
        return;
    connect(reply, SIGNAL(finished()), &eventLoop, SLOT(quit()));
    eventLoop.exec();
}

void JsonClient::parseCustomFormat(QByteArray &byteArray, CJsonMap *result)
{
    QByteArray keyValue;
    QString key, value;
    int p, q, keyPos, valuePos;

#ifdef _WEATHER_DEBUG_
    qDebug() << byteArray;
#endif

    p = -1;
    while((q = byteArray.indexOf(',', ++p)) > 0) {
        keyValue = byteArray.mid(p, q - p); 
        keyPos = 0;
        valuePos = keyValue.indexOf('|', 0);
        key = keyValue.mid(keyPos, valuePos - keyPos);
        value = keyValue.mid(valuePos + 1, keyValue.length() - valuePos - 1);
        result->append(qMakePair(key, value));
        p = q;
    }
    if(p < byteArray.length()) {
        keyValue = byteArray.mid(p, byteArray.length() - p);
        keyPos = 0;
        valuePos = keyValue.indexOf('|', 0);
        key = keyValue.mid(keyPos, valuePos - keyPos);
        value = keyValue.mid(valuePos + 1, keyValue.length() - valuePos - 1);
        result->append(qMakePair(key, value));
    }
#ifdef _WEATHER_DEBUG_
    CJsonMap::const_iterator citer;

    for(citer = result->constBegin();
            citer != result->constEnd();
            citer++)
        qDebug() << (*citer).first << ":" << (*citer).second;
#endif
}
