//
// Created by parsian-ai on 11/13/17.
//

#ifndef PARSIAN_SANDBOX_SANDBOX_MAHI_H
#define PARSIAN_SANDBOX_SANDBOX_MAHI_H

#include <parsian_sandbox/sandbox.h>
#include <parsian_util/action/autogenerate/gotopointavoidaction.h>
#include <parsian_agent/gotopointavoid.h>
class SandBoxMahi : public SandBox {
public:
    SandBoxMahi();
    ~SandBoxMahi();

    void init();
    void execute();

private:

    GotopointavoidAction* gpa;
    CSkillGotoPointAvoid* sgp;

protected:
};

#endif //PARSIAN_SANDBOX_SANDBOX_MAHI_H
