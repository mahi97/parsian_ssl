//
// Created by parsian-ai on 9/28/17.
//

#include "parsian_util/tools/debuger.h"

void Debugger::debug(const QString& _text , unsigned long _type , const QColor& _color) {

    parsian_msgs::parsian_debug parsianDebug;
    parsianDebug.author = _type;
    parsianDebug.value  = _text.toStdString();
    parsianDebug.color  = toColorRGBA(_color);

    debugs.debugs.push_back(parsianDebug);
}