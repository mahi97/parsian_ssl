#include <parsian_ai/roles/role.h>
#include <parsian_ai/soccer.h>

#include <utility>

CRole::CRole() = default;

CRole::CRole(Agent* _agent)
{
    assign(_agent);
}


void CRole::assign(Agent* _agent)
{
    agent = _agent;
}

CRoleInfo::CRoleInfo(QString _roleName)
{
    roleName = std::move(_roleName);
}

Agent* CRoleInfo::robot(int i)
{
    int k=0;
    for (int j=0;j < _MAX_NUM_PLAYERS;j++) {
        if (soccer->agents[j]->roleName == roleName && (wm->our[j]->inSight > 0)) {
            if (k == i) return soccer->agents[j];
            k++;
        }
    }
    return nullptr;

}

int CRoleInfo::count() {
    return agents.size();
}

int CRoleInfo::index(CRole *me)
{
    return agents.indexOf(me->agent);
}

void CRoleInfo::addAgent(Agent* agent) {
    if (!agents.contains(agent)) {
        agents.append(agent);
    }
}

QString CRoleInfo::getRoleName() {
    return roleName;
}
