//
// Created by parsian-ai on 10/13/17.
//

#ifndef PARSIAN_WORLD_MODEL_HALFWORLD_H
#define PARSIAN_WORLD_MODEL_HALFWORLD_H

#include <QList>
#include <QString>
#include <QMap>

#include <parsian_util/geom/geom.h>
#include <parsian_world_model/wm/visionbelief.h>
#include <parsian_world_model/wm/robot.h>
#include <parsian_world_model/wm/ball.h>


class CHalfWorld
{
public:
    Vector2D positioningPoints[_MAX_NUM_PLAYERS];
    int positioningPointsCount;

    quint8 game_state;
    quint8 game_mode;

    CVisionBelief belief;
    QList<CRobot*> ourTeam[_MAX_NUM_PLAYERS];
    QList<CRobot*> oppTeam[_MAX_NUM_PLAYERS];
    QList<CBall*> ball;
    QString ourRole[_MAX_NUM_PLAYERS];
    QString oppRole[_MAX_NUM_PLAYERS];
    CVisionBelief* c;
    int currentFrame;
    int playmakerID;
    bool closing;
    CHalfWorld();
    void track(QList<CRawObject>& p0, QList<CRawObject>& p);
    void update(QList<CRobot*>& robot, CVisionBelief* v, QList<CRawObject>& robot0, int id, bool our);
    void update(QList<CBall*>& ball, CVisionBelief* v);
    void update(CVisionBelief* v);
    void update(CHalfWorld* w);
    void vanishOutOfSights();
    void selectBall(Vector2D pos);
    QMap<QString, QString> knowledgeVars;
};


#endif //PARSIAN_WORLD_MODEL_HALFWORLD_H
