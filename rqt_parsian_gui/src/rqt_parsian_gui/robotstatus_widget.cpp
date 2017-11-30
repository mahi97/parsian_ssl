//
// Created by noOne on 11/30/2017.
//
#include <rqt_parsian_gui/robotstatus_widget.h>

namespace rqt_parsian_gui
{
    RobotStatusWidget::RobotStatusWidget(ros::NodeHandle & n) :QWidget() {
        list = glGenLists(0);
        glNewList(list, GL_COMPILE);
        this ->setFixedSize(600,600);

        main_layout = new QGridLayout;
        //########################################### group boxs
        robot_vel = new QGroupBox("Robot Vel",this);
        battery = new QGroupBox("Battery",this);
        data_loss = new QGroupBox("Data Loss",this);
        sensors = new QGroupBox("Sensors",this);
        faults = new QGroupBox("Faults",this);

        robot_vel->setFixedSize(200,300);
        battery->setFixedSize(200,50);
        data_loss->setFixedSize(200,50);
        sensors->setFixedSize(200,100);
        faults->setFixedSize(300,500);

        main_layout->addWidget(robot_vel,0,0);
        main_layout->addWidget(battery,1,0);
        main_layout->addWidget(data_loss,2,0);
        main_layout->addWidget(sensors,3,0);
        main_layout->addWidget(faults,0,1);

        this->setLayout(main_layout);

    }



    void RobotStatusWidget::updateRobotVel(QRect rect){

    }

    void RobotStatusWidget::updateBattery(QRect rcet){

    }

    void RobotStatusWidget::updateDataLoss(QRect rect){

    }

    void RobotStatusWidget::updateSens(QRect rect){

    }

    void RobotStatusWidget::updateFaults(QRect rect){

    }

}