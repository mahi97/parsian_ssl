#ifndef WORLD_MODEL_H
#define WORLD_MODEL_H

#include "parsian_util/geom/geom.h"
#include "parsian_msgs/parsian_world_model.h"
#include "parsian_util/core/field.h"
#include "parsian_util/core/ball.h"
#include "parsian_util/core/agent.h"
#include "parsian_util/core/team.h"


class CWorldModel {
public:
    CWorldModel();
    CField* field;
    CBall* ball;
    CTeam *our, *opp;

private:
};
#endif // WORLD_MODEL_H
