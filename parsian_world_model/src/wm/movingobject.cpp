#include <parsian_world_model/wm/movingobject.h>

double observeTimeStep = 0.02;
double lastObserveTimeStep = 0.01;


CMovingObject::CMovingObject(bool resetToZero)
        : vanishedCounter() , lastInsight() , delayTime() , kalmanEnabled() , stoppedFrames() , lastSpeeds() , pos() ,
          dir(), angularVel() , obstacleRadius() , modelObjStopPos() , modelDir() , modelCurDir() ,
          modelFrameCnt() , modelC2Sum() , modelC2Ave() , modelC2Count() , modelDirC0() , modelDirC1() , ballStopPos() {
    frameCounter = 0;
    lastFrameKalmanReset = 0;
    lastFrameUpdated = 0;
    cam_id = -1;
    inSight = 0;
    vel.assign(0,0);
    acc.assign(0,0);
    modelObjStopped = true;
    modelObjStopPos.invalidate();
    modelC0 = modelC1 = modelC2 = 0.0;
    modelCurDir.invalidate();
    modelSampleTime = 0.02;
    if (!resetToZero)
    {
        pos.invalidate();
    }
    else pos.assign(0,0);

    observation = new CRawObject();

}

void CMovingObject::update(CMovingObject* obj)
{
    if (obj == nullptr) return;
    cam_id = obj->cam_id;
    pos = obj->pos;
    vel = obj->vel;
    acc = obj->acc;
    dir = obj->dir;
    angularVel = obj->angularVel;
    inSight = obj->inSight;
    hist.clear();
    hist.append(obj->hist);
    modelObjStopped = obj->modelObjStopped;
    modelObjStopPos = obj->modelObjStopPos;
    modelC0 = obj->modelC0;
    modelC1 = obj->modelC1;
    modelC2 = obj->modelC2;
    modelCurDir = obj->modelCurDir;
    modelSampleTime = obj->modelSampleTime;
}

void CMovingObject::update(CRawObject raw)
{
    cam_id = raw.cam_id;
    *observation = raw;


    //pos = observation.pos;
    //vanishedCounter = 0;
    //inSight = 1.0;
    kalmanFilter();
//    findModel(dt);
}

void CMovingObject::updateDelayTime(double newDelayTime)
{
    delayTime = newDelayTime;
}

void CMovingObject::init()
{

}

void CMovingObject::findModel(double dt)
{
    if(hist.count() > 100)
        delete hist.dequeue();
    hist.append(new CRawObject(0, pos, dir.th().degree(), -1, 1.0));

    double aa = 1 ; // TODO : => FIX THIS BallFriction() * Gravity; // acc.length();
    double vv = vel.length()*vel.length();
    if (aa<0.3) aa = 0.3;
    if (acc*vel.norm()>0.3) ballStopPos.invalidate();
    else ballStopPos = pos + vel.norm()*(vv/(2.0*aa));
    modelFrameCnt ++;
    modelSampleTime = dt;
}

void CMovingObject::resetKalman()
{
//overloaded by CBall & CRobot
}

void CMovingObject::filter(int vanished)
{
    pos = observation->pos;
    dir = observation->dir;
    vel.assign(0,0);
    angularVel = 0.0;
    acc.assign(0,0);
}

void CMovingObject::kalmanFilter()
{
    //Kalman Watchdog
    if (vel.length() < 0.01)
    {
        if (frameCounter - lastFrameKalmanReset > 30)
        {
            lastFrameKalmanReset = frameCounter;
//            resetKalman();
        }
    }
    if (lastSpeeds.length() > 50)
        lastSpeeds.pop_back();

    if (lastAngularSpeeds.length() > 50)
        lastAngularSpeeds.pop_back();

    lastSpeeds.push_front(vel);
    lastAngularSpeeds.push_front(angularVel);

    if (lastSpeeds.count() >= 10)
    {
        Vector2D mr(0,0);
        for (int i=0;i<lastSpeeds.count();i++)
        {
            mr.x += lastSpeeds[i].x;
            mr.y += lastSpeeds[i].y;
        }
        mr /= lastSpeeds.count();
        bool ok = false;
        for (int i=0;i<lastSpeeds.count();i++)
        {
            if ((lastSpeeds[i] - mr).length() > 0.01)
            {
                ok = true;
                break;
            }
        }
        double ma = 0;
        for (int i=0;i<lastAngularSpeeds.count();i++)
        {
            ma += lastAngularSpeeds[i];
        }
        ma /= lastAngularSpeeds.count();
        for (int i=0;i<lastAngularSpeeds.count();i++)
        {
            if (fabs(lastAngularSpeeds[i] - ma) > 1)
            {
                ok = true;
                break;
            }
        }
        if (!ok)
        {
            lastSpeeds.clear();
            lastAngularSpeeds.clear();
            resetKalman();
        }
    }

//	debug(QString("ass hole=%1").arg(lastSpeeds.count()), D_ERROR);

    //
    frameCounter ++;
    try {
        inSight = observation->confidence;
        if (observation->confidence<=0)
        {
            vanishedCounter ++;
            filter(vanishedCounter);
        }
        else {
            vanishedCounter = 0;
            inSight = observation->confidence;
            filter(0);
        }
        lastInsight = inSight;
    }
    catch (...)
    {
        qDebug()<<"Warning: Kalman library throwed an exception.";
    }
}


Vector2D CMovingObject::predict(double time)
{
    //must be checked if it works precisely or not
    if (time < 0.001)
        return pos;

    if (acc.length() < 0.01 || vel.length() < 0.1)
    {
        return pos;
    }

    if (acc.valid() && (acc*vel<0))
    {
        double vf = (vel.length() - acc.length()*time);
        if (vf < 0) return pos + vel*vel.length() / (2.0*acc.length());
        return pos - 0.5*acc.length()*vel.norm()*time*time + vel*time;
    }
    return pos + vel*time;
}


Vector2D CMovingObject::predictV(double time)
{
    if (acc.valid() && (acc*vel<0))
    {
        double vf = vel.length() - (acc.length()*time);
        if (vf < 0.0) vf = 0.0;
        return vel.norm() * vf;
    }
    return vel;
}

Vector2D CMovingObject::whereIsAtVel(Vector2D V)
{
    double dx=(V.x*V.x - vel.x*vel.x)/(2*acc.x);
    double dy=(V.y*V.y - vel.y*vel.y)/(2*acc.y);
    return Vector2D{pos.x + dx, pos.y + dy};
}

double CMovingObject::whenIsAtVel(double L)
{
    double A=acc.x*acc.x + acc.y*acc.y;
    double B=2*(vel.x*acc.x + vel.y*acc.y);
    double C=vel.x*vel.x+vel.y*vel.y-L*L;

    double delta=B*B - 4*A*C;
    if(delta<0)
        return -1;
    double T1 = (-B  + sqrt(delta))/(2*A);
    double T2 = (-B  - sqrt(delta))/(2*A);

    if(T1>0)
    {
        if(T2>0)
        {
            return min(T1,T2);
        }

        return T1;

    }

    if(T2>0) {
        return T2;
    }

    return -2;
}

