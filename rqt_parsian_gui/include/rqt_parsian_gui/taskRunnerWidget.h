
#ifndef RQT_TASKRUNNERWIDGET_H
#define RQT_TASKRUNNERWIDGET_H

//
// Created by noOne on 10/19/17.
//

#include <ros/ros.h>
#include <QWidget>
#include <QString>
#include <QGridLayout>
#include <QPushButton>
#include <QAction>
namespace rqt_parsian_gui
{
    class TaskRunnerWidget:public QWidget {
    Q_OBJECT
    public:
        TaskRunnerWidget(ros::NodeHandle & n);
        virtual ~TaskRunnerWidget();
    public slots:


    protected:
    private:

        ros::Publisher team_config_pub;

    };
}

#endif //RQT_TASKRUNNERWIDGET_H
