#ifndef SYNCH_LOAD_FIRST_DATA_H
#define SYNCH_LOAD_FIRST_DATA_H
#include <QObject>
#include "wsSettings.hpp"


class SynchFirstData : public QObject{
    Q_OBJECT
public:
    explicit SynchFirstData(arcirk::Settings* sett, QObject *parent = nullptr);

    void synchronize();

private:
    arcirk::Settings* sett_;
};

#endif // SYNCH_LOAD_FIRST_DATA_H
