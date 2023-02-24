#include "include/synch_load_first_data.hpp"



SynchFirstData::SynchFirstData(arcirk::Settings *sett, QObject *parent):
    QObject(parent)
{
    sett_ = sett;
}

void SynchFirstData::synchronize()
{

}
