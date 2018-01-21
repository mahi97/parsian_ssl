#ifndef OURBALLPLACEMENT_H
#define OURBALLPLACEMENT_H

#include "masterplay.h"

class COurBallPlacement : public CMasterPlay{
public:
        COurBallPlacement();
        ~COurBallPlacement();
        void execute_x();
        void init(const QList <Agent*>& _agents);
private:
        void reset();
};

#endif // OURBALLPLACEMENT_H
