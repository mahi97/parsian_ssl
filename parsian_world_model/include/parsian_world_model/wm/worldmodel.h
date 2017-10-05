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

class CWorldModel : public QObject {
public:
    CWorldModel();
    ~CWorldModel();

    void updateDetection(const parsian_msgs::ssl_vision_detection&);
    void updateGeom(const parsian_msgs::ssl_vision_geometry&);
    void execute(parsian_msgs::parsian_world_model& rosWM);

private:




};


#endif //PARSIAN_WORLD_MODEL_WORLDMODEL_H
