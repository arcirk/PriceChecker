#ifndef BARCODE_INFO_HPP
#define BARCODE_INFO_HPP

#include <QObject>
#include "shared_struct.hpp"
#include <QImage>

BOOST_FUSION_DEFINE_STRUCT(
        (arcirk::client), barcode_info,
        (std::string, barcode) //Штрихкод
        (std::string, barcode_qr) //Qr код
        (std::string, synonym) //Наименование
        (int, balance) //Остаток
        (double, price) //Цена
        (std::string, image_base64) //картинка
        (bool, is_qr) // это Qr?
        (std::string, uuid) //идентфикатор номенклатуры
        (std::string, unit) // единица измерения
        (std::string, currency) //валюта
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
    Q_PROPERTY(QString imageSource READ imageSource NOTIFY imageSourceChanged)
    Q_PROPERTY(bool isLongImgLoad READ isLongImgLoad NOTIFY isLongImgLoadChanged)
    Q_PROPERTY(QString unit READ unit NOTIFY unitChanged)
    Q_PROPERTY(QString currency READ currency NOTIFY currencyChanged)
public:
    explicit BarcodeInfo(QObject *parent = nullptr);

    void set_barcode_info_object(const std::string& source);
    void set_barcode_info(const arcirk::client::barcode_info& info);
    void set_image_from_base64(const std::string& response);

    std::string uuid(){
        return barcode_inf.uuid;
    }

    void set_image(const QByteArray& ba);

    void clear(const std::string& def = "");

private:
    arcirk::client::barcode_info barcode_inf;
    QString barcode() const;
    QString synonym() const;
    int balance() const;
    double price() const;
    QString image_base64() const;
    bool is_qr() const;
    QString m_imageSource;
    QString imageSource();

    bool isLongImgLoad(){return true;};

    QString currency(){return QString::fromStdString(barcode_inf.currency);};
    QString unit(){return QString::fromStdString(barcode_inf.unit);};



signals:
    void barcodeInfoChanged();
    void barcodeChanged();
    void synonymChanged();
    void balanceChanged();
    void priceChanged();
    void image_base64Changed();
    void is_qrChanged();
    void imageSourceChanged();
    void isLongImgLoadChanged();
    void unitChanged();
    void currencyChanged();
    void clearData();
};


#endif // BARCODE_INFO_HPP
