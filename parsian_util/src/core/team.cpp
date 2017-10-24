#include "parsian_util/core/team.h"

CTeam::CTeam(const bool isYellow, const bool isLeft)
{
    data = new CTeamData;
    for (auto& member : data->teamMembers) member = new CRobot();
    setColor(isYellow);
    setSide(isLeft);

}

CTeam::CTeam(const  bool isYellow, const bool isLeft, const std::vector<parsian_msgs::parsian_robot> & _robots) :
        CTeam(isYellow, isLeft) {
    updateRobot(_robots);
}

CTeam::~CTeam()
{
    for (auto &teamMember : data->teamMembers)
        delete teamMember;
    delete data;
}

void CTeam::updateRobot(const std::vector<parsian_msgs::parsian_robot> &_robots) {
    for (auto& member : data->teamMembers) member->setActive(false);

    for(auto& robot : _robots) {
        this->data->teamMembers[robot.id]->update(robot);
        this->data->teamMembers[robot.id]->setActive(true);
    }
    update();

}

int CTeam::activeAgentsCount()
{
    return data->activeAgents.size();
}

void CTeam::update()
{
    data->activeAgents.clear();
    for( int i = 0; i < _MAX_NUM_PLAYERS; i++ )
    {
        if( data->teamMembers[i]->isActive() )
            data->activeAgents.push_back(i);
    }
}

int CTeam::activeAgentID(int i)
{
    if((i<data->activeAgents.size())&&(i>=0))
    {
        return data->activeAgents[i];
    }
    debugger->debug(QString("request for id %1 that does not exist in team").arg(i), D_ERROR);
    debugger->debug("Active Agents:", D_DEBUG);
    for (int activeAgent : data->activeAgents)
        DEBUG(activeAgent , D_DEBUG);
    return -1;

}
CRobot* CTeam::operator [](const int i)
{
    if (i>=0 && i<_MAX_NUM_PLAYERS) return data->teamMembers[i];
    PDEBUG("id out of range", i, D_ERROR);
    return nullptr;
}

CRobot* CTeam::active(const int i)
{
    if((i<data->activeAgents.size())&&(i>=0))
    {
        return data->teamMembers[data->activeAgents[i]];
    }
    debugger->debug(QString("request for id %1 that does not exist in team").arg(i), D_ERROR);
    debugger->debug("Active Agents:", D_DEBUG);
    for (int activeAgent : data->activeAgents)
        DEBUG(activeAgent , D_DEBUG);
    return nullptr;

}


void CTeam::setColor(const bool& isYellow) {
    data->color = (isYellow) ? "yellow" : "blue";
}

void CTeam::setSide(const bool& isLeft) {
    data->side = (isLeft) ? "left" : "right";
}

void CTeam::updateGoaliID(int id)
{
    data->goalieID = id;
}
