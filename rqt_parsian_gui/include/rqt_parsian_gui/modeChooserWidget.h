
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
#include <QAction>
#include <QKeySequence>
#include <parsian_msgs/parsian_team_config.h>

namespace rqt_parsian_gui
#define yellow 1
#define left 1
#define simulation 1
{
    class ModeChooserWidget:public QWidget {
        Q_OBJECT
    public:
        ModeChooserWidget(ros::NodeHandle & n);
    public slots:
        void toggleMode();
        void toggleColor();
        void toggleSide();
    protected:

    private:
        void sendTeamConfig();

        QAction * modeAct;
        QGridLayout *mainLayout;
        QString modeStr[2],colorStr[2],sideStr[2];
        QPushButton *modePB, *colorPB, *sidePB;
        bool mode, color, side;

        ros::Publisher team_config_pub;
        };
}

#endif //RQT_PARSIAN_GUI_MONITORWIDGET_H
