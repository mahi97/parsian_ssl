//
// Created by noOne on 11/30/2017.
//
#include <rqt_parsian_gui/robotstatus_widget.h>



namespace rqt_parsian_gui
{
    RobotStatusWidget::RobotStatusWidget(int team_color) :QWidget() {

        this->setFixedSize(400, 150);

        QFile *file=new QFile("resource/style_sheet/check_box_fault.ssh");
        bool bOpened = file->open(QFile::ReadOnly);
        assert (bOpened);
        QString styleSheet_fault = QLatin1String(file->readAll());
        file->close();
        file=new QFile("resource/style_sheet/check_box_status.ssh");
        bOpened = file->open(QFile::ReadOnly);
        assert (bOpened);
        QString styleSheet_status = QLatin1String(file->readAll());

        //########################################### layouts
        robot_vel_l = new QVBoxLayout;
        status_l = new QVBoxLayout;
        battery_l = new QVBoxLayout;
        data_loss_l = new QVBoxLayout;
        cap_charge_l = new QVBoxLayout;
        main_layout = new QHBoxLayout;
        faults_l = new QVBoxLayout;
        motors_l = new QHBoxLayout;
        encoders_l = new QHBoxLayout;

        status_l->setSizeConstraint(QLayout::SetMinimumSize);
        robot_vel_l->setSizeConstraint(QLayout::SetMinimumSize);
        faults_l->setSizeConstraint(QLayout::SetMinimumSize);
        //########################################### group boxs

        battery = new QGroupBox("Battery", this);
        data_loss = new QGroupBox("Data Loss", this);
        cap_charge = new QGroupBox("cap charge", this);

        robot_vel = new QGroupBox("Robot Vel", this);
        status = new QGroupBox("Status", this);
        faults = new QGroupBox("Faults", this);

        battery->setFixedHeight(25);
        data_loss->setFixedHeight(25);
        cap_charge ->setFixedHeight(25);

        robot_vel->setFixedSize(80, 120);
        status->setFixedSize(120, 120);
        faults->setFixedSize(120, 120);

        cap_charge->setFont(QFont("serif",7));
        battery->setFont(QFont("serif",7));
        data_loss->setFont(QFont("serif",7));

        cap_charge->setContentsMargins(1,5,0,0);
        battery->setContentsMargins(1,5,0,0);
        data_loss->setContentsMargins(1,5,0,0);

        //############################################################### vel
        vel_dir = new QLabel;
        vel = new QLabel("vel");
        vel_ang = new QLabel("vel ang");

        robot_vel_l->addWidget(vel_dir);
        robot_vel_l->addWidget(vel);
        robot_vel_l->addWidget(vel_ang);

        //########################################### load image

        color = (team_color == parsian_msgs::parsian_team_config::isYellow) ? 'y' : 'b';
        draw_id();
        //############################################################### progress bars

        battery_percentage = new QProgressBar;
        data_loss_percentage = new QProgressBar;
        cap_charge_percentage = new QProgressBar;

        battery_l->addWidget(battery_percentage);
        data_loss_l->addWidget(data_loss_percentage);
        cap_charge_l->addWidget(cap_charge_percentage);

        //############################################################### faults

        motors_f = new QCheckBox*[4];
        encoders_f = new QCheckBox*[4];
        for(int i=0;i<4;i++) {
            motors_f[i] = new QCheckBox("M"+QString::number(i+1));
            motors_f[i]->setStyleSheet(styleSheet_fault);
            motors_f[i]->setEnabled(false);
            motors_l->addWidget(motors_f[i]);
            //----------------
            encoders_f[i] = new QCheckBox("E"+QString::number(i+1));
            encoders_f[i]->setStyleSheet(styleSheet_fault);
            encoders_f[i]->setEnabled(false);
            encoders_l->addWidget(encoders_f[i]);
        }

        kick_f = new QCheckBox("Kick");
        chip_f = new QCheckBox("Chip");
        shoot_sens_f = new QCheckBox("Shoot Sensor");
        shoot_board_f = new QCheckBox("Shoot Board");

        kick_f->setEnabled(false);
        chip_f->setEnabled(false);
        shoot_sens_f->setEnabled(false);
        shoot_board_f->setEnabled(false);

        kick_f->setStyleSheet(styleSheet_fault);
        chip_f->setStyleSheet(styleSheet_fault);
        shoot_sens_f->setStyleSheet(styleSheet_fault);
        shoot_board_f->setStyleSheet(styleSheet_fault);

        faults_l->addLayout(motors_l);
        faults_l->addLayout(encoders_l);
        faults_l->addWidget(kick_f);
        faults_l->addWidget(chip_f);
        faults_l->addWidget(shoot_sens_f);
        faults_l->addWidget(shoot_board_f);

        //############################################################### status

        shoot_sens = new QCheckBox("Shoot Sensor");
        spin = new QCheckBox("Spin Catch ball");

        shoot_sens->setEnabled(false);
        spin->setEnabled(false);

        shoot_sens->setStyleSheet(styleSheet_status);
        spin->setStyleSheet(styleSheet_status);

        battery->setLayout(battery_l);
        data_loss->setLayout(data_loss_l);
        cap_charge->setLayout(cap_charge_l);

        status_l->addWidget(battery);
        status_l->addWidget(data_loss);
        status_l->addWidget(cap_charge);
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

    QString RobotStatusWidget::getFileName(){
        return QString("resource/images/" + color + robot_id + ".png");
    }
    void RobotStatusWidget::setMessage(parsian_msgs::parsian_robot_status msg){
        robot_id = QString::number(msg.id , 16);
        board_id = QString::number(msg.boardId , 16);
        battery_percentage->setValue(msg.battery);
        data_loss_percentage->setValue(msg.dataLoss);

        motors_f[0]->setChecked(msg.m1Fault);
        motors_f[1]->setChecked(msg.m2Fault);
        motors_f[2]->setChecked(msg.m3Fault);
        motors_f[3]->setChecked(msg.m4Fault);

        encoders_f[0]->setChecked(msg.En1Fault);
        encoders_f[1]->setChecked(msg.En2Fault);
        encoders_f[2]->setChecked(msg.En3Fault);
        encoders_f[3]->setChecked(msg.En4Fault);

        chip_f->setChecked(msg.chipFault);
        kick_f->setChecked(msg.kickFault);
        shoot_board_f->setChecked(msg.shootBoardFault);

        shoot_sens->setChecked(msg.shootSensor);
        spin->setChecked(msg.spinCatchBall);
    }

    void RobotStatusWidget::draw_id(){
        agent_i = new QPixmap(QPixmap::fromImage(QImage(getFileName())));
        agent_p = new QPainter(agent_i);
        agent_p->setFont(QFont("serif",10));
        agent_p->drawText(10,8,"robot id: " + robot_id);
        agent_p->drawText(10,15,"board id: " + board_id);
        vel_dir->setPixmap(*agent_i);
    }

}