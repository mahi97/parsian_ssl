#include "parsian_util/core/team.h"
#include <ros/ros.h>

QMutex wmMutex;


CTeam::CTeam(const bool isYellow, const bool isLeft)
{
    data = new CTeamData;
    for (int i=0;i< _MAX_NUM_PLAYERS;i++)data->teamMembers[i]=new CRobot(i);
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
   for( int i = 0; i < _MAX_NUM_PLAYERS; i++ ){
       data->teamMembers[i]->setActive(false);
       update();
//        ROS_INFO_STREAM("aftr falsing "<<i);
//        for(int j=0;j< _MAX_NUM_PLAYERS; j++ )
//             ROS_INFO_STREAM(" active "<<j<<" __--->"<< this->data->teamMembers[j]->getActive());

   }

   for(auto& robot : _robots) {
      // ROS_INFO_STREAM(static_cast<int>(robot.id));
       data->teamMembers[static_cast<int>(robot.id)]->update(robot);
       data->teamMembers[static_cast<int>(robot.id)]->setActive(true);
   }
   //update();

}

int CTeam::activeAgentsCount()
{
    return data->activeAgents.size();
}

void CTeam::update()
{
    wmMutex.lock();
    data->activeAgents.clear();
    for( int i = 0; i < _MAX_NUM_PLAYERS; i++ )
    {
        if( data->teamMembers[i]->getActive() )
           // if (!data->activeAgents.contains(i))
            {
                data->activeAgents.push_back(i);
               // ROS_INFO_STREAM("sadasdasd    " << i <<"   "<<data->activeAgents.size());
            }
    }
    wmMutex.unlock();
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
CRobot* CTeam::operator [](const int i) const
{
    if (i>=0 && i<_MAX_NUM_PLAYERS) return data->teamMembers[i];
    PDEBUG("id out of range", i, D_ERROR);
    return nullptr;
}

CRobot* CTeam::active(const int i) const
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
    data->color.fromStdString((isYellow) ? "yellow" : "blue");
}

void CTeam::setSide(const bool& isLeft) {
    data->side.fromStdString((isLeft) ? "left" : "right");
}

void CTeam::updateGoaliID(int id)
{
    data->goalieID = id;
}
