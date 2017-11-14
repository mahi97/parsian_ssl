//
// Created by parsian-ai on 11/13/17.
//

#include <parsian_sandbox/sandbox_mahi.h>

SandBoxMahi::SandBoxMahi() {

}

SandBoxMahi::~SandBoxMahi() {

}

void SandBoxMahi::init() {
    gpa = new GotopointavoidAction;
    sgp = new CSkillGotoPointAvoid(nullptr);
}

void SandBoxMahi::execute() {
    Agent *a{new Agent(0)};
    sgp->setAgent(a);
    a->skill = sgp;
    a->skill->execute();

    gpa->setTargetpos(Vector2D(0, 0));
    gpa->setTargetdir(Vector2D(0, 0));

}
