//
// Created by parsian-ai on 9/28/17.
//

#include "parsian_util/tools/drawer.h"
#include "parsian_msgs/rect2D.h"


void draw (Rect2D _rect, QColor _color, bool _filled)
{
    parsian_msgs::parsian_draw_rect rect;
    parsian_msgs::rect2D d;
    d.
    rect.rect;
        drawer->draw(_rect,_color,_filled);

}

void draw (QString _text, Vector2D _pos, QColor _color, int _size)
{
        drawer->draw(_text, _pos, _color, _size);
}

void draw (Circle2D _circle, int _startAng, int _endAng, QColor _color, bool _filled)
{
        drawer->draw(_circle, _startAng, _endAng, _color, _filled);
}

void draw (Circle2D _circle, QColor _color, bool _filled)
{
        drawer->draw(_circle, 0, 360, _color, _filled);
}

void draw (Polygon2D _polygon, QColor _color, bool _filled)
{
        drawer->draw(_polygon, _color, _filled);
}

void draw (Segment2D _seg, QColor _color)
{
        drawer->draw(_seg, _color);
}

void draw (Vector2D _point, int _type, QColor _color)
{
        drawer->draw(_point, _type, _color);
}
