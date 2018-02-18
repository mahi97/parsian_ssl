#ifndef OURBALLPLACEMENT_H
#define OURBALLPLACEMENT_H

#include "masterplay.h"

enum class BallPlacement {
    NoState = 0,
    GO_FOR_BALL = 1,
    PASS  =2,
    RECIVE_AND_POS = 3,
    FINAL_POS = 4,
    DONE = 5
};

class COurBallPlacement : public CMasterPlay{
public:
        COurBallPlacement();
        ~COurBallPlacement();
        void execute_x();
        void init(const QList <Agent*>& _agents);

private:
        void reset();
        BallPlacement state;
        bool flag = false;
        void execute_x(int pos);
};

#endif // OURBALLPLACEMENT_H
