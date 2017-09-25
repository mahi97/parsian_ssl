//
// Created by parsian-ai on 9/22/17.
//

#ifndef PARSIAN_AI_SOCCER_H
#define PARSIAN_AI_SOCCER_H

#include <parsian_util/base.h>
#include <parsian_util/knowledge.h>
#include <parsian_util/core/agent.h>
#include <parsian_util/core/worldmodel.h>
#include <parsian_ai/coach.h>

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

    ///////////////////////////////////////
    char map_stage(SSL_Referee::Stage stage);
    char map_command(SSL_Referee::Command command);
    char compute_command(SSL_Referee refSC);
    ///////////////////////////////////////

    CWorldModel* wm;
    CAgent **agents;
    CCoach *coach;
Property(ControlMode, ControlMode, controlMode);
PropertyGet(GameMode, Mode, mode);
PropertyGet(ETeamSideType, TeamSide, teamSide);
PropertyGet(ETeamColorType, TeamColor, teamColor);
    clock_t realTimeTime;
    struct timeval t1, t2;

private:

    /****lasts****/
    char cached_command_char;
    SSL_Referee::Stage last_stage;
    SSL_Referee::Command last_command;
    int last_yellow_ycards, last_blue_ycards;
    unsigned int last_yellow_rcards, last_blue_rcards;
    int cmdCnt, lastCmdCnt;
    GameStatePacket gsp;
    bool doClose;
    void primaryDraws();
    void resetRoles();
    void findSupporterRoles();
    void sendPacketToSimulator();
    void sendPacketToRealWorld();

    void refUpdate();
    //////////////////mhmmd thread
    void getRobotVel(int _id, double _x, double _y, double _w);
    ////////////////
    void runMonitorUpdate();



};


#endif //PARSIAN_AI_SOCCER_H
