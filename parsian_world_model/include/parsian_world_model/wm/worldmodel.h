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


class CWorldModel : public QObject {
public:
    CWorldModel();
    ~CWorldModel();

    void updateDetection(const parsian_msgs::ssl_vision_detectionConstPtr&);
    void updateGeom(const parsian_msgs::ssl_vision_geometryConstPtr&);
    void execute();
    void init();
    parsian_msgs::parsian_world_model getParsianWorldModel() const;

private:
    parsian_msgs::parsian_world_model rosWM;

    CVisionClient *vc;
    CHalfWorld* hw;
    CHalfWorld w;
    CHalfWorld mergedHalfWorld;

    CBall* ball;
    CRobot* us[_MAX_NUM_PLAYERS];
    CRobot* them[_MAX_NUM_PLAYERS];


    bool simulationMode;
    void run();
    void update(CHalfWorld*);
    void testFunc(const parsian_msgs::ssl_vision_detectionConstPtr & packet);
    void printRobotInfo(const parsian_msgs::ssl_vision_detection_robot &robot);

    parsian_msgs::ssl_vision_detectionConstPtr detection;

    double visionFPS;
    double visionLatency;
    double visionTimestep;
    double visionProcessTime;


};


#endif //PARSIAN_WORLD_MODEL_WORLDMODEL_H
