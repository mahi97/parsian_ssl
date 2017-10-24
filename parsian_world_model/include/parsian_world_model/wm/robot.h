//
// Created by parsian-ai on 10/13/17.
//

#ifndef PARSIAN_WORLD_MODEL_ROBOT_H
#define PARSIAN_WORLD_MODEL_ROBOT_H

#include <parsian_world_model/wm/movingobject.h>
#include <parsian_world_model/util/robottracker.h>
#include <parsian_util/base.h>
#include <parsian_util/core/field.h>

struct kalmParam
{
    double vx;
    double vy;
    double vw;
};



class CRobot : public CMovingObject
{
private:
    double ANGULAR_DIRECTION;
    long double ADD_TO_ANGULAR_DIRECTION;

//    double distFunction(Vector2D pos1,Vector2D dir1,Vector2D pos2,Vector2D dir2);

protected:
    double blindness, delayTime;
    double lastFrameAngle, turns;
public:

    kalmParam kalman_velocs;
    RobotTracker* tracker;
    bool newRobot;
    QString role;
    static const double robot_radius_old;
    static const double robot_radius_new;
    static const double center_from_kicker_old;
    static const double center_from_kicker_new;
    static const double kicker_width_old;
    static const double kicker_width_new;
    static const double wheel_rad_old;
    static const double wheel_rad_new;
    void setNewRobot(bool _new); //new generation of our robots have different configurations
    bool isNewRobot();
    double robotRadius();
    double kickerWidth();
    double centerFromKicker();
    double wheelRadius();
    CRobot(int _id, bool isOurTeam, bool noKalman=false);
    ~CRobot();
    virtual void init();
    virtual void filter(int vanished);
    virtual void resetKalman();
    void setReplace(Vector2D newPos, float newDirection); //used in simulation
    int replacementPacket(char* buf);
    void resetVariables();
    int id;
    bool inOurTeam;
    bool kickSensor;
    QList<CRobot*> relaxed;
    bool elementNotInSight;
    bool isActive();
    Circle2D getCirle();
    Vector2D getKickerPos(double margin = 0);
//    bool isBallOwner(double verticaldist=0, double horizontaldist=-1);
//    double ballComingSpeed();
//    bool isBallComingTowardMe(double factor=20.0,double vBallMin=500.0);
//    bool isBallGoingFartherFrom(double factor=20.0,double vBallMin=500.0);
//    bool isBallBack();
//    bool isBallAside();
//    bool isBallBack(Vector2D goal);
//    bool collidesBall(Vector2D target);
    void recvData(char Data);
    Property(bool, Replaced, repl);
    Property(Vector2D, ReplPos, replPos);
    Property(float, ReplDir, replDir);

public:
    bool markedByDefense;
    bool markedByMark;
    bool shootSensor;
    double danger;
};


#endif //PARSIAN_WORLD_MODEL_ROBOT_H
