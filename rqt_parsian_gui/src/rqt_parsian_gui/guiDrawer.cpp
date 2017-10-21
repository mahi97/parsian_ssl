//
// Created by rebinnaf on 10/13/17.
//
#include "rqt_parsian_gui/guiDrawer.h"

CguiDrawer::CguiDrawer()
{

}

void CguiDrawer::drawRobot (Vector2D _pos, Vector2D _dir, QColor _color, int _ID, int _comID, QString _str, bool _newRobots)
{
    CGraphicalRobot newItem(_pos, _dir, _color, _ID, _comID, _str, _newRobots);
    robotBuffer.enqueue(newItem);
}

void CguiDrawer::clear(){
//    rectBuffer.clear();
//    arcBuffer.clear();
//    polygonBuffer.clear();
//    segBuffer.clear();
//    pointBuffer.clear();
//    textBuffer.clear();
    robotBuffer.clear();
}


