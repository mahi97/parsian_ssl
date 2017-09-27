//
// Created by parsian-ai on 9/21/17.
//

#ifndef PARSIAN_UTIL_TEAM_H
#define PARSIAN_UTIL_TEAM_H

#include "parsian_util/core/agent.h"
#include <QDebug>
#include <QQueue>

#define _MAX_NUM_PLAYERS 12

struct CTeamData
{
    CRobot* teamMembers[_MAX_NUM_PLAYERS];
    QQueue<int> activeAgents;
    QString color;
    QString side;
    int goalieID;
};

class CTeam
{
public:
    CTeamData *data;
    CTeam(bool isOurTeam);
    ~CTeam();
    CRobot* operator [](const int i);
    CRobot* active(const int i);
    int activeAgentsCount();
    int activeAgentID(int i);
    void update();

    void setColor(const QString& c);
    void updateGoaliID(int id);

};

#endif //PARSIAN_UTIL_TEAM_H
