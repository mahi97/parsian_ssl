//
// Created by parsian-ai on 9/28/17.
//

#ifndef PARSIAN_UTIL_DEBUGER_H
#define PARSIAN_UTIL_DEBUGER_H

#include "parsian_msgs/parsian_debugs.h"
#include <QtGui/QColor>

class Debugger {
public:

    void debug(const QString& text, unsigned long type, const QColor& color = QColor(0,0,0));

    parsian_msgs::parsian_debugs debugs;

private:
    std_msgs::ColorRGBA toColorRGBA(const QColor& _color);

};

Debugger debugger;

#endif //PARSIAN_UTIL_DEBUGER_H
