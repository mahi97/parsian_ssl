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


class CWorldModel : public QObject {
public:
    CWorldModel();
    ~CWorldModel();

    void updateDetection(const parsian_msgs::ssl_vision_detectionConstPtr&);
    void updateGeom(const parsian_msgs::ssl_vision_geometryConstPtr&);
    void execute();
    parsian_msgs::parsian_world_model getParsianWorldModel() const;

private:
    parsian_msgs::parsian_world_model rosWM;

    CVisionClient *vc;
    CHalfWorld* hw;
    bool simulationMode;
    void reconnect();
    void run();
    void testFunc(const parsian_msgs::ssl_vision_detectionConstPtr & packet);
    void printRobotInfo(const parsian_msgs::ssl_vision_detectionConstPtr & robot);


};


#endif //PARSIAN_WORLD_MODEL_WORLDMODEL_H
