// auto-generated don't edit !

#ifndef Kick_HEADER_
#define Kick_HEADER_


#include <parsian_util/actions/action.h>
#include <parsian_util/geom/geom.h>


class KickAction : public Action {

public:
    Kick();
    ~Kick();

    void setMessage(const parsian_msgs:: _msg) {
            robot_id = _msg.robot_id;
            tolerance = _msg.tolerance;
            chip = _msg.chip;
            kickSpeed = _msg.kickSpeed;
            spin = _msg.spin;
            slow = _msg.slow;
            turn = _msg.turn;
            autoChipSpeed = _msg.autoChipSpeed;
            throughMode = _msg.throughMode;
            parallelMode = _msg.parallelMode;
            clear = _msg.clear;
            waitFrames = _msg.waitFrames;
            avoidPenaltyArea = _msg.avoidPenaltyArea;
            avoidOppPenaltyArea = _msg.avoidOppPenaltyArea;
            interceptMode = _msg.interceptMode;
            dontKick = _msg.dontKick;
            sagMode = _msg.sagMode;
            dontRecvPass = _msg.dontRecvPass;
            recvChip = _msg.recvChip;
            penaltyKick = _msg.penaltyKick;
            turnLastMomentForPenalty = _msg.turnLastMomentForPenalty;
            target = _msg.target;
            isGotoPointAvoid = _msg.isGotoPointAvoid;
            kkShotEmpySpot = _msg.kkShotEmpySpot;
            passProfiler = _msg.passProfiler;
            veryFine = _msg.veryFine;
            goalieMode = _msg.goalieMode;
            alternateMode = _msg.alternateMode;
            kickAngTol = _msg.kickAngTol;
            kickWithCenterOfDribbler = _msg.kickWithCenterOfDribbler;
            playMakeMode = _msg.playMakeMode;
            fastIntercept = _msg.fastIntercept;
    }

    parsian_msgs::Kick getMessage() {
        parsian_msgs::Kick _msg;
        _msg.robot_id = robot_id;
        _msg.tolerance = tolerance;
        _msg.chip = chip;
        _msg.kickSpeed = kickSpeed;
        _msg.spin = spin;
        _msg.slow = slow;
        _msg.turn = turn;
        _msg.autoChipSpeed = autoChipSpeed;
        _msg.throughMode = throughMode;
        _msg.parallelMode = parallelMode;
        _msg.clear = clear;
        _msg.waitFrames = waitFrames;
        _msg.avoidPenaltyArea = avoidPenaltyArea;
        _msg.avoidOppPenaltyArea = avoidOppPenaltyArea;
        _msg.interceptMode = interceptMode;
        _msg.dontKick = dontKick;
        _msg.sagMode = sagMode;
        _msg.dontRecvPass = dontRecvPass;
        _msg.recvChip = recvChip;
        _msg.penaltyKick = penaltyKick;
        _msg.turnLastMomentForPenalty = turnLastMomentForPenalty;
        _msg.target = target;
        _msg.isGotoPointAvoid = isGotoPointAvoid;
        _msg.kkShotEmpySpot = kkShotEmpySpot;
        _msg.passProfiler = passProfiler;
        _msg.veryFine = veryFine;
        _msg.goalieMode = goalieMode;
        _msg.alternateMode = alternateMode;
        _msg.kickAngTol = kickAngTol;
        _msg.kickWithCenterOfDribbler = kickWithCenterOfDribbler;
        _msg.playMakeMode = playMakeMode;
        _msg.fastIntercept = fastIntercept;

    }

    SkillProperty(Kick, quint8, Robot_Id, robot_id)
    SkillProperty(Kick, double, Tolerance, tolerance)
    SkillProperty(Kick, bool, Chip, chip)
    SkillProperty(Kick, qint32, Kickspeed, kickSpeed)
    SkillProperty(Kick, qint32, Spin, spin)
    SkillProperty(Kick, bool, Slow, slow)
    SkillProperty(Kick, bool, Turn, turn)
    SkillProperty(Kick, bool, Autochipspeed, autoChipSpeed)
    SkillProperty(Kick, bool, Throughmode, throughMode)
    SkillProperty(Kick, bool, Parallelmode, parallelMode)
    SkillProperty(Kick, bool, Clear, clear)
    SkillProperty(Kick, qint32, Waitframes, waitFrames)
    SkillProperty(Kick, bool, Avoidpenaltyarea, avoidPenaltyArea)
    SkillProperty(Kick, bool, Avoidopppenaltyarea, avoidOppPenaltyArea)
    SkillProperty(Kick, bool, Interceptmode, interceptMode)
    SkillProperty(Kick, bool, Dontkick, dontKick)
    SkillProperty(Kick, bool, Sagmode, sagMode)
    SkillProperty(Kick, bool, Dontrecvpass, dontRecvPass)
    SkillProperty(Kick, bool, Recvchip, recvChip)
    SkillProperty(Kick, bool, Penaltykick, penaltyKick)
    SkillProperty(Kick, bool, Turnlastmomentforpenalty, turnLastMomentForPenalty)
    SkillProperty(Kick, Vector2D, Target, target)
    SkillProperty(Kick, bool, Isgotopointavoid, isGotoPointAvoid)
    SkillProperty(Kick, bool, Kkshotempyspot, kkShotEmpySpot)
    SkillProperty(Kick, bool, Passprofiler, passProfiler)
    SkillProperty(Kick, bool, Veryfine, veryFine)
    SkillProperty(Kick, bool, Goaliemode, goalieMode)
    SkillProperty(Kick, bool, Alternatemode, alternateMode)
    SkillProperty(Kick, double, Kickangtol, kickAngTol)
    SkillProperty(Kick, bool, Kickwithcenterofdribbler, kickWithCenterOfDribbler)
    SkillProperty(Kick, bool, Playmakemode, playMakeMode)
    SkillProperty(Kick, bool, Fastintercept, fastIntercept)
}

#endif // Kick_HEADER_