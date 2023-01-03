#ifndef WEBSOCKETS_HPP
#define WEBSOCKETS_HPP

#include <QObject>
#include <QtWebSockets/QWebSocket>
#include <QUuid>
#include "wsSettings.hpp"
#include "shared_struct.hpp"

class WebSocketClient : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUrl url READ getUrl WRITE setUrl NOTIFY urlChanged)
    Q_PROPERTY(QString typePriceRef READ typePriceRef WRITE setTypePriceRef NOTIFY typePriceRefChanged)
    Q_PROPERTY(QString stockRef READ stockRef WRITE setStockRef NOTIFY stockRefChanged)
    Q_PROPERTY(QString typePrice READ typePrice WRITE setTypePrice NOTIFY typePriceChanged)
    Q_PROPERTY(QString stock READ stock WRITE setStock NOTIFY stockChanged)

public:
    explicit WebSocketClient(const QUrl& url, QObject *parent = nullptr);
    explicit WebSocketClient(QObject *parent = nullptr);
    ~WebSocketClient();

    Q_INVOKABLE static QString generateHash(const QString& usr, const QString& pwd);
    Q_INVOKABLE bool isStarted();

    Q_INVOKABLE void open(arcirk::Settings * sett);
    Q_INVOKABLE void close();

    QString typePriceRef() const;
    QString stockRef() const;

    void setTypePriceRef(const QString& value);
    void setStockRef(const QString& value);

    QString typePrice() const;
    QString stock() const;

    void setTypePrice(const QString& value);
    void setStock(const QString& value);

    Q_INVOKABLE void setUrl(const QUrl& url);
    Q_INVOKABLE QUrl getUrl() const;

    static QString get_hash(const QString& first, const QString& second);

private:
    QWebSocket* m_client;
    arcirk::Settings * wsSettings;
    arcirk::client::checker_conf  chk_conf;

    QUuid m_currentSession;
    QUuid m_currentUserUuid;

    QUrl m_url;
    bool m_started;

    static QString get_sha1(const QByteArray& p_arg);

    void parse_response(const QString& resp);

private slots:

    void onConnected();
    void onDisconnected();
    void onTextMessageReceived(const QString &message);
    void onError(QAbstractSocket::SocketError error);

signals:
    void typePriceRefChanged();
    void stockRefChanged();
    void typePriceChanged();
    void stockChanged();
    void urlChanged();

    void displayError(const QString& what, const QString& err);
    void connectionSuccess(); //при успешной авторизации
    void connectionChanged(bool state);
};

#endif // WEBSOCKETS_HPP
