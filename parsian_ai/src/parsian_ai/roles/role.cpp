#include <parsian_ai/roles/role.h>
#include <parsian_ai/soccer.h>

CRole::CRole() = default;

CRole::CRole(Agent* _agent)
{
}

CRole* CRole::fromString(QString roleName)
{
}


void CRole::assign(Agent* _agent)
{

    if (_agent!=NULL)
    {
        agent = _agent;
        // TODO  : ADD THIS STUFF TO AGENT
//        agent->role = this;
//        agent->skillName = getName();
//        agent->localName = localAgentName;
    }
}

CRoleInfo* CRole::generateInfoClass()
{
    return nullptr;
}

CRoleInfo::CRoleInfo(QString _roleName)
{
    InitVal(roleName);
}

Agent* CRoleInfo::robot(int i)
{
    int k=0;
    for (int j=0;j < _MAX_NUM_PLAYERS;j++)
		if (soccer->agents[j]->roleName == roleName // TODO : add RoleName attr to Agent
				&& (wm->our[j]->inSight > 0)
				)
        {
            if (k==i)
                return soccer->agents[j];
            k++;
        }
    return nullptr;

}

int CRoleInfo::count()
{
    int k = 0;
    for (int j=0;j< _MAX_NUM_PLAYERS ;j++)
//		if ((soccer->agents->skillName == roleName) // TODO : add roleName attr to Agent
//			&& (wm->our[j]->inSight > 0))
//        {
//            k++;
//        }
    return k;
}

int CRoleInfo::index(CRole *me)
{    
    int k = 0;
    for (int j=0;j < _MAX_NUM_PLAYERS;j++)
//        if (soccer->agents[j]->roleName == roleName) // TODO: add roleName to Agent
//        {
//            if (soccer->agents[j]->role == me) // TODO : add role to Agent
//            {
//                return k;
//            }
//            k++;
//        }
    return -1;
}
