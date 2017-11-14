
#ifndef RQT_PARSIAN_GUI_MODECHOOSERWIDGET_H
#define RQT_PARSIAN_GUI_MODECHOOSERWIDGET_H

//
// Created by noOne on 10/19/17.
//

#include <ros/ros.h>
#include <QWidget>
#include <QString>
#include <QGridLayout>
#include <QPushButton>

namespace rqt_parsian_gui
#define yellow 1
#define left 1
#define simulation 1
{
    class ModeChooserWidget:public QWidget {
        Q_OBJECT
    public:
        ModeChooserWidget();
    public slots:
        void toggleMode();
        void toggleColor();
        void toggleSide();
    protected:

    private:
        QGridLayout *mainLayout;
        QString modeStr[2],colorStr[2],sideStr[2];
        QPushButton *modePB, *colorPB, *sidePB;
        bool mode, color, side;
        };
}

#endif //RQT_PARSIAN_GUI_MONITORWIDGET_H
