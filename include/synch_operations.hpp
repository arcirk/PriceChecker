#ifndef SYNCH_OPERATIONS_H
#define SYNCH_OPERATIONS_H

#include <QRunnable>

template<class SynchClass>
class SynhOperations : public QRunnable{

public:
    explicit SynhOperations(SynchClass * synhClass)
    {
        synchClass_ = synhClass;
    }

    void run() override{
        synchronize();
    }

    void synchronize(){
        synchClass_->synchronize();
    }


private:
    SynchClass * synchClass_;

};

#endif // SYNCH_OPERATIONS_H
