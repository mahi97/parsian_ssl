//
// Created by parsian-ai on 9/19/17.
//

#include "parsian_world_model/wm/worldmodel.h"


CWorldModel::CWorldModel(int c) {
    vc = new CVisionClient();
//    vc->ourSide = _SIDE_LEFT;
    simulationMode = true;
    visionFPS = 61.0;
    ball = new CBall(false);
    packs = 0;

}

CWorldModel::~CWorldModel() {
    delete vc;
    vc = nullptr;
}

void CWorldModel::updateDetection(const parsian_msgs::ssl_vision_detectionConstPtr& _detection) {
    detection = _detection;
}

void CWorldModel::execute() {
    run();
}

parsian_msgs::parsian_world_model CWorldModel::getParsianWorldModel() {
//    if (this->ball == nullptr) return rosWM;
    rosWM.ball.pos = this->ball->pos.toParsianVector2D();
    rosWM.ball.camera_id = this->ball->cam_id;
    rosWM.ball.obstacleRadius = this->ball->obstacleRadius;
    qDebug() << "Ball : " << rosWM.ball.pos.x;
    return rosWM;
}

void CWorldModel::printRobotInfo(const parsian_msgs::ssl_vision_detection_robot &robot)
{
    printf("CONF=%4.2f ", robot.confidence);
    printf("ID=%3d ",robot.robot_id);
    printf(" HEIGHT=%6.2f POS=<%9.2f,%9.2f> ",robot.height, robot.pos.x, robot.pos.y);
    printf("ANGLE=%6.3f ",robot.orientation);

    printf("RAW=<%8.2f,%8.2f>\n",robot.pixel_pos.x, robot.pixel_pos.y);
}

void CWorldModel::testFunc(const parsian_msgs::ssl_vision_detectionConstPtr &detection)
{
    printf("-----Received Wrapper Packet---------------------------------------------\n");
    //see if the packet contains a robot detection frame:

    //Display the contents of the robot detection results:
    double t_now = 0 /*= GetTimeSec()*/;

    printf("-[Detection Data]-------\n");
    //Frame info:
    printf("Camera ID=%d FRAME=%d T_CAPTURE=%.4f\n",detection->camera_id, detection->frame_number,detection->t_capture);

    printf("SSL-Vision Processing Latency                   %7.3fms\n",(detection->t_sent-detection->t_capture)*1000.0);
    printf("Network Latency (assuming synched system clock) %7.3fms\n",(t_now-detection->t_sent)*1000.0);
    printf("Total Latency   (assuming synched system clock) %7.3fms\n",(t_now-detection->t_capture)*1000.0);
    auto balls_n = static_cast<int>(detection->balls.size());
    auto robots_blue_n = static_cast<int>(detection->us.size());
    auto robots_yellow_n = static_cast<int>(detection->them.size());

    //Ball info:
    for (int i = 0; i < balls_n; i++) {
        parsian_msgs::ssl_vision_detection_ball ball = detection->balls[i];
        printf("-Ball (%2d/%2d): CONF=%4.2f POS=<%9.2f,%9.2f> ", i+1, balls_n, ball.confidence, ball.pos.x, ball.pos.y);
        printf("Z=%7.2f ",ball.pos.z);
        printf("RAW=<%8.2f,%8.2f>\n",ball.pixel_pos.x, ball.pixel_pos.y);
    }

    //Blue robot info:
    for (int i = 0; i < robots_blue_n; i++) {
        parsian_msgs::ssl_vision_detection_robot robot = detection->us[i];
        printf("-Robot(US) (%2d/%2d): ",i+1, robots_blue_n);
        printRobotInfo(robot);
    }

    //Yellow robot info:
    for (int i = 0; i < robots_yellow_n; i++) {
        parsian_msgs::ssl_vision_detection_robot robot = detection->them[i];
        printf("-Robot(THEM) (%2d/%2d): ",i+1, robots_yellow_n);
        printRobotInfo(robot);
    }
}


// This Function Run in a Loop
void CWorldModel::run()
{
    double lastSecond = 0.0, t=0.0;
    int frame=0;
    int lastSecondFrames=0;
    int packmax;
    double procTime = -1;

    usleep(1000);
    packmax = 4;// TODO : Config conf()->BallTracker_activeCamNum();
    if (vc == nullptr) return;
    vc->parse(detection);
    frame ++;
    packs ++;
//    testFunc(detection);

    if ( packs >= packmax ) {
        packs = 0;
        if (vc->lastCamera < CAMERA_NUM && vc->lastCamera >= 0)
        {
            vc->merge(packmax);
            mergedHalfWorld.currentFrame = frame;
            mergedHalfWorld.update(&(vc->res));
            mergedHalfWorld.vanishOutOfSights();
        }
        if (t - lastSecond > 1.0)
        {
            if (lastSecond > 0.0)
            {
                visionFPS = frame - lastSecondFrames;
            }
            lastSecond = t;
            lastSecondFrames = frame;
        }
        visionLatency  = vc->res.visionLatency;
        visionTimestep = vc->res.timeStep;
        if (procTime > 0) visionProcessTime = procTime;


        // UPDATE WM
        this->update(&mergedHalfWorld);
    }



    // UPDATE OLD KNOWLEDGE
//    mergedHalfWorld.game_state = knowledge->getGameState();
//    mergedHalfWorld.game_mode = knowledge->getGameMode();
//    mergedHalfWorld.closing = doClose;
//    for (int i=0; i< knowledge->agentCount();i++)
//    {
//        mergedHalfWorld.ourRole[i] = knowledge->getAgent(i)->skillName;
//    }
//    for (int i=0; i< _MAX_NUM_PLAYERS;i++)
//    {
//        mergedHalfWorld.oppRole[i] = wm->opp[i]->role;
//    }
//    mergedHalfWorld.gsp = gsp;
//    mergedHalfWorld.knowledgeVars = knowledge->variables;
//    for (int i=0;i<_MAX_NUM_PLAYERS;i++)
//        knowledge->positioningPoints[i] = mergedHalfWorld.positioningPoints[i];
//    knowledge->positioningPointsCount = mergedHalfWorld.positioningPointsCount;
//    if (knowledge->getPlayMaker() == NULL)
//    {
//        mergedHalfWorld.playmakerID = -1;
//    }
//    else mergedHalfWorld.playmakerID = knowledge->getPlayMaker()->id();
    //////////////////

}

void CWorldModel::update(CHalfWorld* w0) {
    w.update(w0);
    if (w.ball.count()>0) {
        ball->update(w.ball[0]);
    } else {
        ball->inSight = 0.0;
    }
    for (int i = 0; i < _MAX_NUM_PLAYERS; i++)
    {
        if (!w.ourTeam[i].isEmpty()) {
            us[i]->update(w.ourTeam[i][0]);
        } else {
            us[i]->inSight = 0.0;
        }
//        if (!w.oppTeam[i].isEmpty()) {
//            them[i]->update(w.oppTeam[i][0]);
//        } else {
//            them[i]->inSight = 0.0;
//        }
    }

}