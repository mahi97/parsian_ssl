//
// Created by parsian-ai on 9/19/17.
//

#ifndef PARSIAN_WORLD_MODEL_WORLDMODEL_H
#define PARSIAN_WORLD_MODEL_WORLDMODEL_H

#include "QObject"
#include "QDebug"


#include "parsian_msgs/parsian_world_model.h"
#include "parsian_msgs/ssl_vision_detection.h"
#include "parsian_msgs/ssl_vision_geometry.h"
#include <parsian_world_model/wm/visionclient.h>
#include <parsian_world_model/wm/halfworld.h>
#include <parsian_world_model/wm/ball.h>
#include <parsian_world_model/wm/robot.h>

#include "parsian_world_model/world_modelConfig.h"



class CWorldModel : public QObject {
public:
    CWorldModel(int c);
    ~CWorldModel();

    void updateDetection(const parsian_msgs::ssl_vision_detection&);
    void updateGeom(const parsian_msgs::ssl_vision_geometryConstPtr&);
    void execute(world_model_config::world_modelConfig & config);
    void init();
    parsian_msgs::parsian_world_modelPtr getParsianWorldModel(bool colour_yellow, bool side_left);

private:
    parsian_msgs::parsian_robot rosRobots[_MAX_NUM_PLAYERS*2];
    parsian_msgs::parsian_robot rosBall;

    CVisionClient *vc;
    CHalfWorld* hw;
    CHalfWorld w;
    CHalfWorld mergedHalfWorld;

    CBall* ball;
    CRobot* us[_MAX_NUM_PLAYERS];
    CRobot* them[_MAX_NUM_PLAYERS];


    bool simulationMode;
    void run(world_model_config::world_modelConfig & config);
    void update(CHalfWorld*);
    void testFunc(const parsian_msgs::ssl_vision_detectionConstPtr & packet);
    void printRobotInfo(const parsian_msgs::ssl_vision_detection_robot &robot);

    void toParsianMessage(const CRobot* _robot, int id);
    void toParsianMessage(const CBall* _ball);


    parsian_msgs::ssl_vision_detection detection;

    double visionFPS;
    double visionLatency;
    double visionTimestep;
    double visionProcessTime;

    int packs;


};


#endif //PARSIAN_WORLD_MODEL_WORLDMODEL_H
