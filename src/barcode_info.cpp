#include "include/barcode_info.hpp"
#include <QDebug>

BarcodeInfo::BarcodeInfo(QObject *parent)
: QObject{parent}
{

}

void BarcodeInfo::set_barcode_info_object(const std::string &source)
{

    auto s = nlohmann::json::parse(source);
    auto b_info = s.value("barcode_info", nlohmann::json{});

    //qDebug() << qPrintable(QString::fromStdString(b_info.dump()));

    barcode_inf = pre::json::from_json<arcirk::client::barcode_info>(b_info);

    emit barcodeInfoChanged();

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
