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
    Vector2D getEmptyTarget(Vector2D _position, double _radius);
    void assignSkills();


private:
    void reset();
    bool isPenaltyShootOut;
    CRolePlayMake* playmakeRole;
    QList<GotopointavoidAction*> moveSkills;
    void generatePositions();
    QList<Vector2D> positions;
};

#endif // OURPENALTY_H
