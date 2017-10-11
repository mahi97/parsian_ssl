// auto-generated don't edit !

#ifndef Onetouch_HEADER_
#define Onetouch_HEADER_


#include <parsian_util/actions/action.h>
#include <parsian_util/geom/geom.h>


class OnetouchAction : public Action {

public:
    Onetouch();
    ~Onetouch();

    void setMessage(const parsian_msgs:: _msg) {
            robot_id = _msg.robot_id;
            chip = _msg.chip;
            kickSpeed = _msg.kickSpeed;
            waitPos = _msg.waitPos;
            target = _msg.target;
            distToBallLine = _msg.distToBallLine;
            velocityToBallLine = _msg.velocityToBallLine;
            receiveChip = _msg.receiveChip;
            avoidPenaltyArea = _msg.avoidPenaltyArea;
            moveTowardTheBall = _msg.moveTowardTheBall;
            shotToEmptySpot = _msg.shotToEmptySpot;
    }

    parsian_msgs::Onetouch getMessage() {
        parsian_msgs::Onetouch _msg;
        _msg.robot_id = robot_id;
        _msg.chip = chip;
        _msg.kickSpeed = kickSpeed;
        _msg.waitPos = waitPos;
        _msg.target = target;
        _msg.distToBallLine = distToBallLine;
        _msg.velocityToBallLine = velocityToBallLine;
        _msg.receiveChip = receiveChip;
        _msg.avoidPenaltyArea = avoidPenaltyArea;
        _msg.moveTowardTheBall = moveTowardTheBall;
        _msg.shotToEmptySpot = shotToEmptySpot;

    }

    SkillProperty(Onetouch, quint8, Robot_Id, robot_id)
    SkillProperty(Onetouch, bool, Chip, chip)
    SkillProperty(Onetouch, qint32, Kickspeed, kickSpeed)
    SkillProperty(Onetouch, Vector2D, Waitpos, waitPos)
    SkillProperty(Onetouch, Vector2D, Target, target)
    SkillProperty(Onetouch, double, Disttoballline, distToBallLine)
    SkillProperty(Onetouch, double, Velocitytoballline, velocityToBallLine)
    SkillProperty(Onetouch, bool, Receivechip, receiveChip)
    SkillProperty(Onetouch, bool, Avoidpenaltyarea, avoidPenaltyArea)
    SkillProperty(Onetouch, bool, Movetowardtheball, moveTowardTheBall)
    SkillProperty(Onetouch, bool, Shottoemptyspot, shotToEmptySpot)
}

#endif // Onetouch_HEADER_