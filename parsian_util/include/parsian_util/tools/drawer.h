//
// Created by parsian-ai on 9/28/17.
//

#ifndef PARSIAN_UTIL_DRAWER_H_H
#define PARSIAN_UTIL_DRAWER_H_H

#include "parsian_util/geom/geom.h"
#include "parsian_msgs/parsian_draw.h"

#include <QtCore/QString>
#include <QtGui/QColor>

void draw (Rect2D _rect = Rect2D(Vector2D(0, 0), Vector2D(0, 0)), QColor _color = QColor(255, 255, 255), bool _filled = false);
void draw (QString _text = "", Vector2D _pos = Vector2D(0,0), QColor _color = QColor("black"), int _size = 12);
void draw (Circle2D _circle = Circle2D(Vector2D(0,0), 0 ), int _startAng = 0, int _endAng = 360, QColor _color=QColor(255, 255, 255), bool _filled = false);
void draw (Polygon2D _polygon = Polygon2D(), QColor _color = QColor(255, 255, 255), bool _filled = false);
void draw (Segment2D _seg = Segment2D(Vector2D(0,0), Vector2D(0,0)), QColor _color = QColor(255, 255, 255));
void draw (Vector2D _point = Vector2D(0,0), int _type = 0, QColor _color = QColor(255, 255, 255));

#endif //PARSIAN_UTIL_DRAWER_H_H
