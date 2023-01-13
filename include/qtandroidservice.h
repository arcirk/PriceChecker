#ifndef QTANDROIDSERVICE_H
#define QTANDROIDSERVICE_H

#ifdef IS_OS_ANDROID
#include <QtCore/private/qandroidextras_p.h>
#include <QObject>

class QtAndroidService : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString deviceId READ androidId NOTIFY deviceIdChanged)
public:
    QtAndroidService(QObject *parent = nullptr);

    static QtAndroidService *instance() { return m_instance; }
    Q_INVOKABLE void sendToService(const QString &name);


signals:
    void messageFromService(const QString &message);
    void deviceIdChanged();

private:
    void registerNatives();
    void registerBroadcastReceiver();
    QString androidId();

    static QtAndroidService *m_instance;



};
#endif
#endif // QTANDROIDSERVICE_H
