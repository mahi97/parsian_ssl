#ifndef CSTOPPLAY_H
#define CSTOPPLAY_H

#include "parsian_ai/plays/masterplay.h"

class CStopPlay : public CMasterPlay
{
public:
    CStopPlay();
    ~CStopPlay();

    void execute_0();
    void execute_1();
    void execute_2();
    void execute_3();
    void execute_4();
    void execute_5();
    void execute_6();
    void init(const QList <CAgent*>& _agents);
private:
    void stopPosition();
    void reset();

    GotopointavoidAction *gpa[6];
    Vector2D rolePosition[6];

};

#endif // CSTOPPLAY_H
