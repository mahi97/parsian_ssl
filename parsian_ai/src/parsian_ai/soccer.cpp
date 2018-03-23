#include <parsian_ai/soccer.h>

CSoccer* soccer;

CSoccer::CSoccer() {
    agents = new Agent*[_MAX_NUM_PLAYERS];
    for (int i = 0; i < _MAX_NUM_PLAYERS; i++) {
        agents[i] = new Agent(i);
        agents[i]->action = nullptr;
    }
    coach = new CCoach(agents);
    experimental = new Exprimental(agents);
}

CSoccer::~CSoccer() {
    for (int i = _MAX_NUM_PLAYERS - 1; i >= 0; i--) {
        delete agents[i];
    }
    delete agents;
    delete coach;
    delete experimental;

}


void CSoccer::execute() {
    bool custom = false;
    customControl(custom);
    if (! custom) {
        coach->execute();

    }
}

void CSoccer::customControl(bool &custom) {
    custom = false;
    custom = true;
    experimental->execute();
}

CCoach* CSoccer::getCoach() {
    return coach;
}