#ifndef Block_H
#define Block_H

#include <parsian_ai/roles/role.h>

class CRoleBlockInfo : public CRoleInfo
{
public:
    CRoleBlockInfo(QString _roleName);
	void findPos( bool blockGoal );
    Vector2D blockPosition;
    void reset(){}
};

class CRoleBlock : public CRole
{
protected:
    GotopointavoidAction* gotopoint;
public:
	virtual void parse(QStringList params = QStringList());
    SkillProperty(CRoleBlock, bool, BlockGoal, blockGoal);
    SkillProperty(CRoleBlock, bool, Stop, stop);
};

#endif // Block_H
