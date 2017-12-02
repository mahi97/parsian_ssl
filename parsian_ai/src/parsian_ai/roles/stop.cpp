#include "parsian_ai/roles/stop.h"


const double StopRadius = 0.55;//0.70;//1.02;//0.78;
CRoleStopInfo::CRoleStopInfo(QString _roleName)
	: CRoleInfo(_roleName)
{
	//	inCorner = -1;
}

void CRoleStopInfo::findPositions()
{
	//	if (inCorner == -1)
	//	{
	//        if ((wm->ball->pos.x < -_FIELD_WIDTH/2.0 + 0.7) && (fabs(wm->ball->pos.y) > _FIELD_HEIGHT/2.0 - 0.5))
	//		{
	//			inCorner = 1;
	//		}
	//		else inCorner = -1;
	//	}
	//	else {
	//		if ((wm->ball->pos.x < -_FIELD_WIDTH/2.0 + 0.6) && (fabs(wm->ball->pos.y) > _FIELD_HEIGHT/2.0 - 0.6))
	//		{
	//			inCorner = 1;
	//		}
	//		else if ((wm->ball->pos.x > -_FIELD_WIDTH/2.0 + 0.8) && (fabs(wm->ball->pos.y) > _FIELD_HEIGHT/2.0 - 0.4))
	//		{
	//			inCorner = 0;
	//		}
	//	}

	//	if (inCorner == 1)
	//		draw("Trapped in corner, now what?", Vector2D(-1.2, 0.0), "red");

	double sRadius = StopRadius;
	Vector2D c = wm->ball->pos;
  const double radius = 1.8 + 2.0*CRobot::robot_radius_new;
  const double MARGIN = 0.01;

	TA = wm->field->ourGoal();
	if((wm->ball->pos - wm->field->ourGoal()).length() < radius )
		TA = wm->field->oppGoal();
	else
		TA = wm->field->ourGoal();

	Vector2D baseDirVec;
	baseDirVec = (TA - c).norm();


	Ps.clear();
	int coef = 1;
	if( count() > 0 ){
		Ps.append( c + baseDirVec * (sRadius+CRobot::robot_radius_new));
	}
	for( int i=1 ; i<count() ; i++ ){
		Ps.append( c + baseDirVec.rotatedVector((coef*((i+1)/2))*_RAD2DEG*0.2/0.6) * (sRadius+CRobot::robot_radius_new));
		coef *= -1;
	}

	thR.clear();
	thP.clear();
	robotId.clear();
	for (int i=0; i<count(); i++)
	{
		thP.append(Vector2D::angleBetween(Ps[i] - c,baseDirVec).degree());
		robotId.append(robot(i)->id());
		thR.append(Vector2D::angleBetween(robot(i)->pos() - c,baseDirVec).degree());
	}

	for( int i = 0; i < thP.size(); i++)
	{
		for( int j = i + 1; j < thP.size(); j++)
		{
			if( thP[i] < thP[j])
			{
				thP.swap(i, j);
				Ps.swap(i, j);
			}
		}
	}
	for( int i = 0; i < thR.size(); i++)
	{
		for( int j = i + 1; j < thR.size(); j++)
		{
			if( thR[i] < thR[j])
			{
				thR.swap(i, j);
				robotId.swap(i, j);
			}
		}
	}

}

CRoleStop::CRoleStop(CAgent *_agent) : CRole(_agent)
{
	gotopoint = new GotopointavoidAction();
}

CRoleStop::~CRoleStop()
{
	delete gotopoint;
}

void CRoleStop::execute()
{

	Vector2D target;
	info()->findPositions();
	int kkk;
	for (int k=0;k<info()->count();k++)
	{
		if (agent->id()==info()->robotId[k])
		{
			target = info()->Ps[k];
			kkk = k;
		}
	}


	gotopoint->setSlowmode(true);
	gotopoint->setRobot_Id(static_cast<quint8>(agent->id()));//gotopoint->setAgent(agent);
	gotopoint->setMaxvelocity(2.0);
	//gotopoint->setMaxVelocityNormal(1.0); todo: should be deleted?
	if (wm->ball->inSight<=0 || !wm->ball->pos.valid() || !wm->field->isInField(wm->ball->pos)) {agent->waitHere();return;}
	info()->findPositions();
	gotopoint->setTargetpos(target);
	gotopoint->setTargetdir(Vector2D(1.0, 0.0));
	gotopoint->setLookat(wm->ball->pos);
	// gotopoint->setKeepLooking(true); todo: should be deleted?
	gotopoint->setAvoidpenaltyarea(true);

    gotopoint->setBallobstacleradius(0.50);
    // gotopoint->setAvoidBall(true); todo: should be deleted?
	drawer->draw(Circle2D(target , 0.03) , "magenta" , true);
    drawer->draw(QString("%1").arg(kkk) , target);
    agent->action = gotopoint;
}

void CRoleStop::parse(QStringList params)
{
	switchAgent = false;
	for (int i=0;i<params.length();i++)
	{
		if (params[i].toLower()=="switch") switchAgent = true;
	}
}

double CRoleStop::progress()
{
	return 0.0;
}

void CRoleStop::generateFromConfig(CAgent *a)
{
	agent = a;
}

CRoleStopInfo* CRoleStop::info()
{
//	return (CRoleStopInfo*) CSkills::getInfo("stop");
}


//--------------------------------Halt--------------------------------//

CRoleHaltInfo::CRoleHaltInfo(QString _roleName) : CRoleInfo(_roleName)
{

}

CRoleHalt::CRoleHalt(CAgent *_agent) : CRole(_agent)
{

}

CRoleHalt::~CRoleHalt()
{
}

void CRoleHalt::execute()
{
  agent->waitHere();
}

double CRoleHalt::progress()
{
  return 1.0;
}

void CRoleHalt::generateFromConfig(CAgent *a)
{
  agent = a;
}
