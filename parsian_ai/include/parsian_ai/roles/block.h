#ifndef Block_H
#define Block_H

#include <parsian_ai/roles/role.h>
#include <parsian_util/core/agent.h>

class CRoleBlockInfo : public CRoleInfo
{
public:

    CRoleBlockInfo::CRoleBlockInfo(QString _roleName) : CRoleInfo(_roleName)
    void CRoleBlockInfo::findPos(bool blockGoal)

    CRoleBlockInfo(QString _roleName);
	void findPos( bool blockGoal );
    Vector2D blockPosition;
    void reset(){}
};

class CRoleBlock : public CRole
{
protected:
    GotopointavoidAction* gotopoint;
    static CRoleBlockInfo* roleInfo;
public:
    CRoleBlock(CAgent *_agent);
    ~CRoleBlock();
    void execute();
    double progress();
	virtual void parse(QStringList params = QStringList());
    CRoleBlockInfo* info();
    SkillProperty(CRoleBlock, bool, BlockGoal, blockGoal);
    SkillProperty(CRoleBlock, bool, Stop, stop);
};

#endif // Block_H

