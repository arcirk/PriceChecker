#ifndef SYNCDATA_H
#define SYNCDATA_H
#include <QObject>
#include <QFloat16>
#include <nlohmann/json.hpp>

class SyncData : public QObject{
    Q_OBJECT
    Q_PROPERTY(bool running READ running WRITE setRunning NOTIFY runningChanged)
    Q_PROPERTY(QString message READ message WRITE setMessage NOTIFY messageChanged)


public:
    explicit SyncData(QObject *parent = 0);
    bool running() const;
    QString message() const;

    void setComparisonOfDocuments(const nlohmann::json& resp);

    void setDatabaseFileName(const QString& file);
private:
    bool m_running;
    QString m_message;
    int count;
    nlohmann::json srv_resp;
    QString db_path;

    bool synchronizeBaseDocuments(nlohmann::json& sendToSrvDocuments);

signals:
    void finished();    // Сигнал, по которому будем завершать поток, после завершения метода run
    void runningChanged(bool running);
    void messageChanged(QString message);
    void sendMessage(QString message);

    void endSynchronize(bool result, const nlohmann::json& send_srv);

public slots:
    void run(); // Метод с полезной нагрузкой, который может выполняться в цикле
    void setRunning(bool running);
    void setMessage(QString message);
};

#endif // SYNCDATA_H
