#include "include/httpservice.hpp"

HttpService::HttpService(QObject *parent, const QString& host, const QString& usr, const QString& pwd)
    : QNetworkAccessManager{parent}
{

    if(!host.isEmpty())
        setHttpHost(host);

    if(!usr.isEmpty())
        setHttpUser(usr);

    if(!pwd.isEmpty())
        setHttpPassword(pwd);
}

void HttpService::setHttpHost(const QString &value)
{
    _httpHost = value;
}

void HttpService::setHttpUser(const QString &value)
{
    _httpUser = value;
}

void HttpService::setHttpPassword(const QString &value)
{
    _httpPassword = value;
}

QString HttpService::httpHost()
{
    return _httpHost;
}

QString HttpService::httpUser()
{
    return _httpUser;
}

QString HttpService::httpPassword()
{
    return _httpPassword;
}

void HttpService::request(const QString& cmd)
{
    QString concatenated = QString("%1:%2").arg(httpUser(), httpPassword());
    QByteArray data = concatenated.toLocal8Bit().toBase64();
    QString headerData = "Basic " + data;
    QNetworkRequest request=QNetworkRequest(QUrl(httpHost() + "/" + cmd));
    request.setRawHeader("Authorization", headerData.toLocal8Bit());
    this->get(request);
}
