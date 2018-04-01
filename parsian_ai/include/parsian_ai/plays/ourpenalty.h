#ifndef OURPENALTY_H
#define OURPENALTY_H

#include <parsian_ai/plays/masterplay.h>
#include <parsian_ai/roles/playmake.h>
#include <parsian_ai/gamestate.h>

enum PenaltyState{
    Positioning,
    Kicking
};

class COurPenalty : public CMasterPlay {
public:
    COurPenalty();
    ~COurPenalty();
    void execute_x();
    void init(const QList <Agent*>& _agents);
    void setPlaymake(Agent* _playmakeAgent);
    void setState(PenaltyState _state){state = _state;};
    void executeShootoutPositioning();
    void executeNormalPositioning();
    Vector2D getEmptyTarget(Vector2D _position, double _radius);
    void assignSkills();
    void playmakePositioning();



private:
    void reset();
    bool isPenaltyShootOut;
    CRolePlayMake* playmakeRole;
    QList<GotopointavoidAction*> moveSkills;
    GotopointavoidAction* PMgotopoint;
    void generatePositions();
    QList<Vector2D> positions;
    PenaltyState state;
};

#endif // OURPENALTY_H
