
#ifndef RQT_REFEREEWIDGET_H
#define RQT_REFEREEWIDGET_H

//
// Created by noOne on 10/19/17.
//

#define _MAX_NUM_PLAYERS 12

#include <ros/ros.h>
#include <QWidget>
#include <QString>
#include <QStringList>
#include <QButtonGroup>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVariant>
namespace rqt_parsian_gui
{

    class RefereeWidget : public QWidget {
    Q_OBJECT
    public:
        explicit RefereeWidget(ros::NodeHandle & n);
        virtual ~RefereeWidget();

    public slots:
//        void setTask(QAction*);
//        void setID(QAction * );
    protected:

    private:
        QHBoxLayout *mainLayout;
        QPushButton *btnRefs[14];
        QStringList strRefNames , strRefCommands;
        ros::Timer timer;
        ros::Publisher refereePub;
        void timerCb(const ros::TimerEvent& _timer);

    };
}

#endif //RQT_REFEREEWIDGET_H
