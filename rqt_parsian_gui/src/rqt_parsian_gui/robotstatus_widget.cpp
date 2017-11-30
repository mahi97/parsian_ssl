//
// Created by noOne on 11/30/2017.
//
#include <rqt_parsian_gui/robotstatus_widget.h>

namespace rqt_parsian_gui
{
    RobotStatusWidget::RobotStatusWidget(ros::NodeHandle & n) :QWidget() {
        list = glGenLists(0);
        glNewList(list, GL_COMPILE);
        this ->setFixedSize(550,300);

        //########################################### layouts
        robot_vel_l = new QVBoxLayout;
        sensors_l = new QVBoxLayout;
        battery_l = new QVBoxLayout;
        data_loss_l = new QVBoxLayout;
        main_layout = new QGridLayout;
        faults_l = new QGridLayout;
        main_layout = new QGridLayout;
        //########################################### group boxs
        robot_vel = new QGroupBox("Robot Vel",this);
        battery = new QGroupBox("Battery",this);
        data_loss = new QGroupBox("Data Loss",this);
        sensors = new QGroupBox("Sensors",this);
        faults = new QGroupBox("Faults",this);

        robot_vel->setFixedSize(150,300);
        battery->setFixedSize(150,50);
        data_loss->setFixedSize(150,50);
        sensors->setFixedSize(150,100);
        faults->setFixedSize(200,300);

        //############################################################### progress bars

        battery_percentage = new QProgressBar;
        data_loss_percentage = new QProgressBar;

        battery_l->addWidget(battery_percentage);
        data_loss_l->addWidget(data_loss_percentage);

        //############################################################### faults

        shoot_sens = new QCheckBox("Shoot Sensor");
        spin_sens = new QCheckBox("Spin Sensor");

        shoot_sens->setEnabled(false);
        spin_sens->setEnabled(false);

        sensors_l->addWidget(shoot_sens);
        sensors_l->addWidget(spin_sens);

        //############################################################### faults

        motors_f = new QCheckBox*[4];
        encoders_f = new QCheckBox*[4];
        for(int i=0;i<4;i++) {
            motors_f[i] = new QCheckBox("M"+QString::number(i+1));
            motors_f[i]->setFixedSize(45,30);
            motors_f[i]->setEnabled(false);
            faults_l->addWidget(motors_f[i],0,i);
            //----------------
            encoders_f[i] = new QCheckBox("E"+QString::number(i+1));
            encoders_f[i]->setFixedSize(45,30);
            encoders_f[i]->setEnabled(false);
            faults_l->addWidget(encoders_f[i],1,i);
        }

        kick_f = new QCheckBox("Kick");
        chip_f = new QCheckBox("Chip");
        shoot_sens_f = new QCheckBox("Shoot Sense");
        shoot_board_f = new QCheckBox("Shoot Board");

        kick_f->setEnabled(false);
        chip_f->setEnabled(false);
        shoot_sens_f->setEnabled(false);
        shoot_board_f->setEnabled(false);

        kick_f->setFixedSize(185,30);
        chip_f->setFixedSize(185,30);
        shoot_sens_f->setFixedSize(185,30);
        shoot_board_f->setFixedSize(185,30);

        faults_l->addWidget(kick_f,2,0);
        faults_l->addWidget(chip_f,3,0);
        faults_l->addWidget(shoot_sens_f,4,0);
        faults_l->addWidget(shoot_board_f,5,0);
        //############################################## add widgets and layouts

        robot_vel->setLayout(robot_vel_l);
        sensors->setLayout(sensors_l);
        battery->setLayout(battery_l);
        data_loss->setLayout(data_loss_l);
        faults->setLayout(faults_l);
        //---------------------------
        main_layout->addWidget(robot_vel,0,0);
        main_layout->addWidget(battery,0,1);
        main_layout->addWidget(data_loss,1,1);
        main_layout->addWidget(sensors,2,1);
        main_layout->addWidget(faults,0,2);
        //--------------------------
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