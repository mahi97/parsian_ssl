
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

enum class Color {
    YELLOW = 0,
    BLUE = 1
};

enum class TeamSide {
    LEFT = 0,
    RIGHT = 1
};

enum class GameMode {
    SIMULATION = 0,
    REAL = 1
};

#define FILE_PATH "/config/team_config.txt"
namespace rqt_parsian_gui
{


    class ModeChooserWidget:public QWidget {
    Q_OBJECT
    public:
        ModeChooserWidget(ros::NodeHandle & n);
        virtual ~ModeChooserWidget();
    public slots:
        void toggleMode();
        void toggleColor();
        void toggleSide();
        void saveTeamConfig();

    protected:
    private:
        void sendTeamConfig();
        void loadTeamConfig();

        QAction * modeAct;
        QGridLayout *mainLayout;
        QString modeStr[2],colorStr[2],sideStr[2];
        QPushButton *modePB, *colorPB, *sidePB;
        Color color;
        TeamSide side;
        GameMode mode;
        boost::shared_ptr<parsian_msgs::parsian_team_config> team_config;

        ros::Publisher team_config_pub;
    };
}

#endif //RQT_PARSIAN_GUI_MONITORWIDGET_H
