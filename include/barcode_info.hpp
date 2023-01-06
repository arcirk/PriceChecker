#ifndef BARCODE_INFO_HPP
#define BARCODE_INFO_HPP

#include <QObject>
#include "shared_struct.hpp"

BOOST_FUSION_DEFINE_STRUCT(
        (arcirk::client), barcode_info,
        (std::string, barcode)
        (std::string, synonym)
        (int, balance)
        (double, price)
        (std::string, image_base64)
        (bool, is_qr)
);

class BarcodeInfo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString barcode READ barcode NOTIFY barcodeChanged)
    Q_PROPERTY(QString synonym READ synonym NOTIFY synonymChanged)
    Q_PROPERTY(int balance READ balance NOTIFY balanceChanged)
    Q_PROPERTY(double price READ price NOTIFY priceChanged)
    Q_PROPERTY(QString image_base64 READ image_base64 NOTIFY image_base64Changed)
    Q_PROPERTY(bool is_qr READ is_qr NOTIFY is_qrChanged)

public:
    explicit BarcodeInfo(QObject *parent = nullptr);

    void set_barcode_info_object(const std::string& source);

private:
    arcirk::client::barcode_info barcode_inf;
    QString barcode() const;
    QString synonym() const;
    int balance() const;
    double price() const;
    QString image_base64() const;
    bool is_qr() const;

signals:
    void barcodeInfoChanged();
    void barcodeChanged();
    void synonymChanged();
    void balanceChanged();
    void priceChanged();
    void image_base64Changed();
    void is_qrChanged();
};


#endif // BARCODE_INFO_HPP
