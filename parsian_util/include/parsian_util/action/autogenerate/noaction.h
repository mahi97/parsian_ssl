// auto-generated don't edit !

#ifndef NoAction_HEADER_
#define NoAction_HEADER_



#include <parsian_util/action/action.h>
#include <parsian_util/geom/geom.h>
#include <parsian_msgs/parsian_skill_no.h>

class NoAction : public Action {

public:

    void setMessage(const void* _msg);

    void* getMessage();

    static QString getActionName();


    SkillProperty(NoAction, quint8, Robot_Id, robot_id);
    SkillProperty(NoAction, bool, Waithere, waithere);

};

#endif // NoAction_HEADER_
