#include <parsian_ai/soccer.h>

CSoccer* soccer;

CSoccer::CSoccer()
{
//    wm = new CWorldModel;
    agents = new CAgent*[_MAX_NUM_PLAYERS];
    for(int i = 0; i < wm->our.activeAgentsCount(); i++ )
    {
        agents[i] = new CAgent(wm->our.activeAgentID(i));// todo noOne says: is myChange is correct;
        agents[i]->self = *wm->our.active(i);
    }

//    knowledge = new CKnowledge(agents);
//    coach = new CCoach(agents);

    mode = Simulation;
    controlMode = AI;
    teamColor = _COLOR_BLUE;
    teamSide = _SIDE_LEFT;
    lastCmdCnt = 0;
    cmdCnt = 0;
    //shared variables
    doClose = false;
}

CSoccer::~CSoccer()
{


    //	qDebug () << "soccer closed";
    //   delete simulator;
    //   delete robCom;
    //   delete coach;
    //   delete knowledge;
    //   delete wm;
    //   delete visionThread;
    //   delete positioning;
    //   for (int i=0;i<_MAX_NUM_PLAYERS;i++)
    //	   delete agents[i];
    //   delete agents;

    // MASOUD: MUST BE CHECKED!!
//    delete coach;
//    delete knowledge;
    for(int i = _MAX_NUM_PLAYERS-1; i >= 0; i-- ){
        delete agents[i];
    }
    delete agents;


}

void CSoccer::primaryDraws(){

//    ////////////////////////////////// Draw Section ///////////////////////////////////
//    static int ballDrawVel = 0;
//    ballDrawVel++;
//    if( ballDrawVel == 5 ){
//        ballDrawVel = 0;
//        double ballDrawVelocity = wm->ball->vel.length();
//        auto ballVelInt = static_cast<int>(ballDrawVelocity);
//        int ballVelFloat = static_cast<int>((ballDrawVelocity - ballVelInt) * 100.0);
//        draw(QString("BV: %1.%2").arg(ballVelInt).arg(ballVelFloat) , Vector2D(-0.5,-2.2) , "blue" , 15);
//    }
//
//    draw(QString("GS: ") + knowledge->stateToString(knowledge->getGameState()) , Vector2D(2.5, 2.55), QColor("red") , 15);
//    draw(QString("GM: ") + knowledge->stateToString(knowledge->getGameMode()) , Vector2D(2.5, 2.35), QColor("blue") , 15);
//
//    double regionWidth = 0;
//    QList<int> relaxIDS;
//    knowledge->getEmptyPosOnGoal(wm->ball->pos, regionWidth,true,relaxIDS,relaxIDS,1.0, true);
//    knowledge->getEmptyPosOnGoal(wm->ball->pos, regionWidth,false,relaxIDS,relaxIDS,1.0, true);
//    ///////////////////////////////////////////////////////////////////////////////////

}

void CSoccer::execute()
{
//    QTime timer;
//    timer.start();


//    primaryDraws();


    //////////////////set opponents roles more specificly and set priority for each of which////////////////////
//    coach->setOpponents();
    //////////////////////////////////////

//    findSupporterRoles();


//    timer.restart();
    if( mode != Spy && controlMode == AI)
    {
        for (int i=0;i<_NUM_PLAYERS;i++)
        {
//            agents[i]->waitHere(); TODO : Robot Command
        }
        bool custom = false;
//        customControl(custom);
        if (!custom)
        {
//            coach->execute();

        }
    }

    //  debug(QString("%1) MainLoop Time2: %2").arg(knowledge->frameCount).arg(timer.elapsed()) , D_MASOOD);
//    timer.restart();


//    ////////////////////////////////if simulation mode is running send packets to simulator/////////////////////////////////////
//    if( mode == Simulation ){
//        sendPacketToSimulator();
//    }
//    else//////if real mode is running send packets to agents by communication module
//    {
//        sendPacketToRealWorld();
//    }
//    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //  debug(QString("%1) MainLoop Time3: %2").arg(knowledge->frameCount).arg(timer.elapsed()) , D_MASOOD);
//    timer.restart();

//    updateTask();
}

void CSoccer::updateTask(){
    auto* kick = new KickAction;
    kick->setKickspeed(1023);
    kick->setTarget(wm->field->oppGoal());
    kick->setSlow(true);
    kick->setKkshotempyspot(true);

    auto* gtp = new GotopointavoidAction;
    Circle2D aroundBall = Circle2D(wm->ball->pos, 0.5);
    Vector2D vec1, vec2;
    aroundBall.intersection(Line2D(Vector2D(0,0), wm->ball->pos), &vec1, &vec2);
    gtp->setTargetpos(vec1.absX() < vec2.absX() ? vec1 : vec2);


    if(!gameState->canMove()) {    //HALT
        agents[0]->action = gtp;
    }
    else {
        agents[0]->action = kick;
    }
}
