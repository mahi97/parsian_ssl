#ifndef STOP_H
#define STOP_H

#include <parsian_ai/roles/role.h>
#include <parsian_util/core/worldmodel.h>
class CRoleStopInfo : public CRoleInfo
{
public:
    CRoleStopInfo(QString _roleName);
    void findPositions();
	int inCorner;
    Vector2D TA;
    QList <Vector2D> Ps;
    QList <double> thR;
    QList <double> thP;
    QList <int> robotId;
    void reset(){}
};

class CRoleStop : public CRole
{
protected:
    GotopointavoidAction* gotopoint;
	bool switchAgent;
public:
    virtual void generateFromConfig(CAgent *a);
	virtual void parse(QStringList params);
	CRoleStop(CAgent *_agent);
	~CRoleStop();
	void execute();

    CRoleStopInfo* info();
    double progress();
};



class CRoleHaltInfo : public CRoleInfo
{
public:
    CRoleHaltInfo(QString _roleName);
};

class CRoleHalt : public CRole
{
public:
    virtual void generateFromConfig(CAgent *a);
    CRoleHalt(CAgent *_agent);
    ~CRoleHalt();
    void execute();
    double progress();
};

#endif // STOP_H