#ifndef HTTPSERVICE_HPP
#define HTTPSERVICE_HPP

#include <QNetworkAccessManager>
#include <QObject>
#include <QNetworkReply>

class HttpService : public QNetworkAccessManager
{
    Q_OBJECT
public:
    explicit HttpService(QObject *parent = nullptr, const QString& host = "", const QString& usr = "", const QString& pwd = "");

    void setHttpHost(const QString& value);
    void setHttpUser(const QString& value);
    void setHttpPassword(const QString& value);

    QString httpHost();
    QString httpUser();
    QString httpPassword();

    void request(const QString& os);

private:
    QString _httpHost;
    QString _httpUser;
    QString _httpPassword;
};

#endif // HTTPSERVICE_H
