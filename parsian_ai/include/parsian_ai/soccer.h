//
// Created by parsian-ai on 9/22/17.
//

#ifndef PARSIAN_AI_SOCCER_H
#define PARSIAN_AI_SOCCER_H

#include <parsian_util/base.h>
#include <parsian_util/knowledge.h>
#include <parsian_util/core/agent.h>
#include <parsian_util/core/worldmodel.h>
//#include <parsian_ai/coach.h>

#include <QtCore/QtCore>

class CSoccer {

public:
    enum GameMode {
        Simulation = 1,
        Real = 2,
        Spy = 3
    };
    enum ControlMode
    {
        HandyControl = 1,
        AI   = 2,
        Stop = 3
    };
    CSoccer();
    ~CSoccer();
    void execute();

    void setMode(GameMode _mode);
    void setTeamColor(ETeamColorType _color);
    void setTeamSide(ETeamSideType _side);
    void selectBall(Vector2D pos);
    void selectRobot(Vector2D pos);
//    virtual void customControl(bool& custom);

    CWorldModel* wm;

    CAgent **agents;
//    CCoach *coach;
Property(ControlMode, ControlMode, controlMode);
PropertyGet(GameMode, Mode, mode);
PropertyGet(ETeamSideType, TeamSide, teamSide);
PropertyGet(ETeamColorType, TeamColor, teamColor);
    clock_t realTimeTime;
    struct timeval t1, t2;

private:

    /****lasts****/
    char cached_command_char;
    int last_yellow_ycards, last_blue_ycards;
    unsigned int last_yellow_rcards, last_blue_rcards;
    int cmdCnt, lastCmdCnt;
    GameStatePacket gsp;
    bool doClose;
    void primaryDraws();
//    void findSupporterRoles();

    void refUpdate();



};


#endif //PARSIAN_AI_SOCCER_H
