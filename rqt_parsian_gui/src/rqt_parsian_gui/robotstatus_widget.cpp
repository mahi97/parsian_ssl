//
// Created by noOne on 11/30/2017.
//
#include <rqt_parsian_gui/robotstatus_widget.h>


namespace rqt_parsian_gui
{
    RobotStatusWidget::RobotStatusWidget(ros::NodeHandle & n) :QWidget() {
        list = glGenLists(0);
        glNewList(list, GL_COMPILE);
        this ->setFixedSize(400,150);

        QFile file("resource/style_sheet/check_box.ssh");
        bool bOpened = file.open(QFile::ReadOnly);
        assert (bOpened);
        QString styleSheet = QLatin1String(file.readAll());

        //########################################### layouts
        robot_vel_l = new QVBoxLayout;
        status_l = new QVBoxLayout;
        battery_l = new QVBoxLayout;
        data_loss_l = new QVBoxLayout;
        main_layout = new QHBoxLayout;
        faults_l = new QVBoxLayout;
        motors_l = new QHBoxLayout;
        encoders_l = new QHBoxLayout;
        //########################################### group boxs
        robot_vel = new QGroupBox("Robot Vel",this);
        battery = new QGroupBox("Battery",this);
        data_loss = new QGroupBox("Data Loss",this);
        status = new QGroupBox("Status",this);
        faults = new QGroupBox("Faults",this);

        robot_vel->setFixedSize(80,120);
        battery->setFixedSize(120,40);
        data_loss->setFixedSize(120,40);
        status->setFixedSize(120,120);
        faults->setFixedSize(120,120);

        //############################################################### progress bars

        battery_percentage = new QProgressBar;
        data_loss_percentage = new QProgressBar;

        battery_l->addWidget(battery_percentage);
        data_loss_l->addWidget(data_loss_percentage);


        //############################################################### faults

        motors_f = new QCheckBox*[4];
        encoders_f = new QCheckBox*[4];
        for(int i=0;i<4;i++) {
            motors_f[i] = new QCheckBox("M"+QString::number(i+1));
            motors_f[i]->setStyleSheet(styleSheet);
            motors_f[i]->setEnabled(false);
            motors_l->addWidget(motors_f[i]);
            //----------------
            encoders_f[i] = new QCheckBox("E"+QString::number(i+1));
            encoders_f[i]->setStyleSheet(styleSheet);
            encoders_f[i]->setEnabled(false);
            encoders_l->addWidget(encoders_f[i]);
        }

        kick_f = new QCheckBox("Kick");
        chip_f = new QCheckBox("Chip");
        shoot_sens_f = new QCheckBox("Shoot Sense");
        shoot_board_f = new QCheckBox("Shoot Board");

        kick_f->setEnabled(false);
        chip_f->setEnabled(false);
        shoot_sens_f->setEnabled(false);
        shoot_board_f->setEnabled(false);

        kick_f->setStyleSheet(styleSheet);
        chip_f->setStyleSheet(styleSheet);
        shoot_sens_f->setStyleSheet(styleSheet);
        shoot_board_f->setStyleSheet(styleSheet);

        faults_l->addLayout(motors_l);
        faults_l->addLayout(encoders_l);
        faults_l->addWidget(kick_f);
        faults_l->addWidget(chip_f);
        faults_l->addWidget(shoot_sens_f);
        faults_l->addWidget(shoot_board_f);

        //############################################################### status

        shoot_sens = new QCheckBox("Shoot Sensor");
        spin = new QCheckBox("Spin");

        shoot_sens->setEnabled(false);
        spin->setEnabled(false);

        shoot_sens->setStyleSheet(styleSheet);
        spin->setStyleSheet(styleSheet);

        battery->setLayout(battery_l);
        data_loss->setLayout(data_loss_l);

        status_l->addWidget(battery);
        status_l->addWidget(data_loss);
        status_l->addWidget(shoot_sens);
        status_l->addWidget(spin);
        //############################################## add widgets and layouts
        robot_vel->setLayout(robot_vel_l);
        status->setLayout(status_l);
        faults->setLayout(faults_l);
        //---------------------------
        main_layout->addWidget(robot_vel);
        main_layout->addWidget(status);
        main_layout->addWidget(faults);
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