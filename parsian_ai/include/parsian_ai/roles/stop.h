#ifndef STOP_H
#define STOP_H

#include <parsian_ai/roles/role.h>

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
	NoAction*			  noAction;
	bool switchAgent;
public:
	virtual void parse(QStringList params);
	CRoleStop(Agent *_agent);
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
    CRoleHalt(Agent *_agent);
    ~CRoleHalt();
    void execute();
    double progress();

private:
	NoAction* noAction;
};

#endif // STOP_H