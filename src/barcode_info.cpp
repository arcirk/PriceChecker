#include "include/barcode_info.hpp"
#include <QDebug>
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>

BarcodeInfo::BarcodeInfo(QObject *parent)
: QObject{parent}
{
    barcode_inf = arcirk::client::barcode_info();
    barcode_inf.currency = "руб.";
    barcode_inf.unit = "шт.";
}

void BarcodeInfo::set_barcode_info_object(const std::string &source)
{

//    auto s = nlohmann::json::parse(source);
//    auto b_info = s.value("barcode_info", nlohmann::json{});

    //qDebug() << qPrintable(QString::fromStdString(b_info.dump()));

//    qDebug() << "start std parce" << QDateTime::currentDateTime();

   // auto b_info = nlohmann::json::parse(source);
    //barcode_inf = pre::json::from_json<arcirk::client::barcode_info>(b_info);
    barcode_inf = pre::json::from_json<arcirk::client::barcode_info>(source);

    if(!barcode_inf.image_base64.empty()){
         QByteArray img = QByteArray::fromBase64(barcode_inf.image_base64.data());
         if(!img.isNull()){
             QString image("data:image/png;base64,");
             image.append(QString::fromLatin1(img.toBase64().data()));
             m_imageSource = image;
         }else
             m_imageSource = "";
    }else
        m_imageSource = "";

//    qDebug() << "end std parce" << QDateTime::currentDateTime();

//    qDebug() << "start qt parce" << QDateTime::currentDateTime();

//    auto obj = QJsonDocument::fromJson(source.data()).object();

//    QByteArray img = QByteArray::fromBase64(obj.value("image_base64").toString().toUtf8());
//    if(!img.isNull()){
//        QString image("data:image/png;base64,");
//        image.append(QString::fromLatin1(img.toBase64().data()));
//        //_imageQr = image;
//    }//else
//        //_imageQr = "";

//    qDebug() << "end qt parce" << QDateTime::currentDateTime();

    emit barcodeInfoChanged();

}

void BarcodeInfo::set_image_from_base64(const std::string &response)
{
    auto info = nlohmann::json::parse(response);
    if(info.value("uuid", "") != barcode_inf.uuid)
        return;

    barcode_inf.image_base64 = info.value("image_base64", "");
    if(!barcode_inf.image_base64.empty()){
         QByteArray img = QByteArray::fromBase64(barcode_inf.image_base64.data());
         if(!img.isNull()){
             QString image("data:image/png;base64,");
             image.append(QString::fromLatin1(img.toBase64().data()));
             m_imageSource = image;
             emit imageSourceChanged();
         }else
             m_imageSource = "";
    }else
        m_imageSource = "";
}

void BarcodeInfo::set_image(const QByteArray &ba)
{
//    m_image.loadFromData(ba);
//    emit imageChanged(m_image);
    if(!ba.isNull()){
        QString image("data:image/png;base64,");
        image.append(QString::fromLatin1(ba.toBase64().data()));
        m_imageSource = image;
        emit imageSourceChanged();
    }else
        m_imageSource = "";
}

QString BarcodeInfo::barcode() const
{
    return QString::fromStdString(barcode_inf.barcode);
}

QString BarcodeInfo::synonym() const
{
    return QString::fromStdString(barcode_inf.synonym);
}

int BarcodeInfo::balance() const
{
    return barcode_inf.balance;
}

double BarcodeInfo::price() const
{
    return barcode_inf.price;
}

QString BarcodeInfo::image_base64() const
{
    return QString::fromStdString(barcode_inf.image_base64);
}

bool BarcodeInfo::is_qr() const
{
    return barcode_inf.is_qr;
}

QString BarcodeInfo::imageSource()
{
    return m_imageSource;
}

void BarcodeInfo::clear(const std::string& def)
{
    m_imageSource = "";
    barcode_inf = arcirk::client::barcode_info();
    barcode_inf.barcode = def;
    emit clearData();
}
