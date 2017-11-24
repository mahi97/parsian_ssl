
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

#define YELLOW 1
#define LEFT 1
#define SIMULATION 1
#define FILE_PATH "config/team_config.txt"
namespace rqt_parsian_gui
{


    class ModeChooserWidget:public QWidget {
        Q_OBJECT
    public:
        ModeChooserWidget(ros::NodeHandle & n);
        ~ModeChooserWidget();
    public slots:
        void toggleMode();
        void toggleColor();
        void toggleSide();
    protected:

    private:
        void sendTeamConfig();
        void saveTeamConfig();
        void loadTeamConfig();

        QAction * modeAct;
        QGridLayout *mainLayout;
        QString modeStr[2],colorStr[2],sideStr[2];
        QPushButton *modePB, *colorPB, *sidePB;
        bool mode, color, side;

        ros::Publisher team_config_pub;
        };
}

#endif //RQT_PARSIAN_GUI_MONITORWIDGET_H
