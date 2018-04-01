#ifndef OURPENALTY_H
#define OURPENALTY_H

#include <parsian_ai/plays/masterplay.h>
#include <parsian_ai/roles/playmake.h>
#include <parsian_ai/gamestate.h>

class COurPenalty : public CMasterPlay {
public:
    COurPenalty();
    ~COurPenalty();
    void execute_x();
    void init(const QList <Agent*>& _agents);
    void setPlaymake(Agent* _playmakeAgent);
    void executeShootoutPositioning();
    void executeNormalPositioning();

private:
    void reset();
    bool isPenaltyShootOut;
    CRolePlayMake* playmakeRole;
};

#endif // OURPENALTY_H
