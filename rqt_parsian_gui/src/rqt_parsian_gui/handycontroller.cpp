/*
  Copyright 2016 Lucas Walter
*/

#include <rqt_parsian_gui/handycontroller.h>


namespace rqt_parsian_gui
{

    HandyController::HandyController() : rqt_gui_cpp::Plugin(), widget_(0)
    {

        qApp->installEventFilter(this);
        // Constructor is called first before initPlugin function, needless to say.
        // give QObjects reasonable names
        setObjectName("handycontroller");
        //cmd = new parsian_msgs::parsian_robot_command;
    }

    void HandyController::initPlugin(qt_gui_cpp::PluginContext& context)
    {

        // access standalone command line arguments
        QStringList argv = context.argv();

        // create QWidget
        widget_ = new QWidget();
        widget_->setWindowTitle("handycontroller");
        // extend the widget with all attributes and children from UI file
        mUI.setupUi(widget_);

        //initial value
        agentID = mUI.Agent_combobox->currentText().toInt();        cmd.robot_id = agentID;
        speed = mUI.Speed_slide->sliderPosition();                  //no msg for it should effect in wheelspwwds
        kickspeed = mUI.Kickspeed_slide->sliderPosition();          cmd.kickSpeed = 0;//kickspeed;
        chipkickspeed = mUI.ChipKickSpeed_slide->sliderPosition();  cmd.kickspeedz = 0;//chipkickspeed;
        rollerspeed = mUI.RollerSpeed_slide->sliderPosition();      cmd.roller_speed = rollerspeed;
        cmd.wheelsspeed = true;

        //connect signal and slots
        connect(mUI.Speed_slide, SIGNAL(valueChanged(int)), mUI.Speed_show, SLOT(setNum(int)));
        connect(mUI.Kickspeed_slide, SIGNAL(valueChanged(int)), mUI.Kickspeed_show, SLOT(setNum(int)));
        connect(mUI.RollerSpeed_slide, SIGNAL(valueChanged(int)), mUI.RollerSpeed_show, SLOT(setNum(int)));
        connect(mUI.ChipKickSpeed_slide, SIGNAL(valueChanged(int)), mUI.ChipKickSpeed_show, SLOT(setNum(int)));
        connect(mUI.Left, SIGNAL(pressed()), this, SLOT(leftpressed()));
        connect(mUI.Up, SIGNAL(pressed()), this, SLOT(uppressed()));
        connect(mUI.Right, SIGNAL(pressed()), this, SLOT(rightpressed()));
        connect(mUI.Down, SIGNAL(pressed()), this, SLOT(downpressed()));
        connect(mUI.Angle1, SIGNAL(pressed()), this, SLOT(angle1pressed()));
        connect(mUI.Angle2, SIGNAL(pressed()), this, SLOT(angle2pressed()));
        connect(mUI.Kick, SIGNAL(pressed()), this, SLOT(kickpressed()));
        connect(mUI.Chip, SIGNAL(pressed()), this, SLOT(chippressed()));
        connect(mUI.Roller, SIGNAL(pressed()), this, SLOT(rollerpressed()));
        connect(mUI.Left, SIGNAL(released()), this, SLOT(released()));
        connect(mUI.Up, SIGNAL(released()), this, SLOT(released()));
        connect(mUI.Right, SIGNAL(released()), this, SLOT(released()));
        connect(mUI.Down, SIGNAL(released()), this, SLOT(released()));
        connect(mUI.Angle1, SIGNAL(released()), this, SLOT(released()));
        connect(mUI.Angle2, SIGNAL(released()), this, SLOT(released()));
        connect(mUI.Kick, SIGNAL(released()), this, SLOT(released()));
        connect(mUI.Chip, SIGNAL(released()), this, SLOT(released()));
        connect(mUI.Roller, SIGNAL(released()), this, SLOT(released()));
        connect(mUI.Agent_combobox, SIGNAL(activated(QString)),this, SLOT(agentidchanged(QString)));
        connect(mUI.Speed_slide, SIGNAL(valueChanged(int)), this, SLOT(speedchanged(int)));
        connect(mUI.Kickspeed_slide, SIGNAL(valueChanged(int)), this, SLOT(kickspeedchanged(int)));
        connect(mUI.ChipKickSpeed_slide, SIGNAL(valueChanged(int)), this, SLOT(chipkickspeedchanged(int)));
        connect(mUI.RollerSpeed_slide, SIGNAL(valueChanged(int)), this, SLOT(rollerspeedchanged(int)));

        //initial publishing
        ros::NodeHandle& nh = getNodeHandle();
        ros::NodeHandle& nh_private = getPrivateNodeHandle();
        grsim_pub  = nh.advertise<parsian_msgs::parsian_robot_command>("robot_command0", 1000);


        // add widget to the user interface
        context.addWidget(widget_);
        widget_->show();


    }

    void HandyController::shutdownPlugin()
    {
        // unregister all publishers here
    }

    void HandyController::saveSettings(qt_gui_cpp::Settings& plugin_settings,
                               qt_gui_cpp::Settings& instance_settings) const
    {
        // instance_settings.setValue(k, v)
    }

    void HandyController::restoreSettings(const qt_gui_cpp::Settings& plugin_settings,
                                  const qt_gui_cpp::Settings& instance_settings)
    {
        // v = instance_settings.value(k)
    }

    void HandyController::takeAction(EActionType action)
    {
        switch(action)
        {
        case EActionType::_STOP_NAVIGATION:         //TODO dont know what the hel is this
//            for (int i=0;i<_NUM_PLAYERS;i++)
//            {
//                char r = 0;
//                if (i==currentAgent) r = agents[i]->getRoller();
//                agents[i]->waitHere();
//                agents[i]->setBeep(true);
//                if (i==currentAgent) agents[i]->setRoller(r);
//            }

//            haltBtn->setIcon(QIcon("./icons/small/stop_on.png"));
//            upBtn->setIcon(QIcon("./icons/small/red_up.png"));
//            leftBtn->setIcon(QIcon("./icons/small/red_left.png"));
//            downBtn->setIcon(QIcon("./icons/small/red_down.png"));
//            rightBtn->setIcon(QIcon("./icons/small/red_right.png"));
//            ccwBtn->setIcon(QIcon("./icons/small/red_ccw.png"));
//            cwBtn->setIcon(QIcon("./icons/small/red_cw.png"));
            break;

        case EActionType::_STOP_OTHER:              //TODO dont know what the hel is this
//            agents[currentAgent]->setKick(0);
//            agents[currentAgent]->setForceKick(false);

//            kickBtn->setIcon(QIcon("./icons/small/kick_off.png"));
//            chipBtn->setIcon(QIcon("./icons/small/chip_off.png"));
            break;

        case EActionType::_FORWARD:
            //upBtn->setIcon(QIcon("../resource/icons/green_up.png"));
            setRobotVel(speed*4.2 / 127.0, 0.0, 0.0);
            break;

        case EActionType::_BEEP:                    //TODO dont know what the hell is this
            //agents[currentAgent]->beep = !agents[currentAgent]->beep;
            break;

        case EActionType::_BACKWARD:
            //downBtn->setIcon(QIcon("./icons/small/green_down.png"));
            setRobotVel(-speed*4.2 / 127.0, 0.0, 0.0);
            break;

        case EActionType::_LEFT:
            //leftBtn->setIcon(QIcon("./icons/small/green_left.png"));
            setRobotVel(0.0, speed*4.2 / 127.0, 0.0);

            break;

        case EActionType::_RIGHT:
            //rightBtn->setIcon(QIcon("./icons/small/green_right.png"));
            setRobotVel(0.0, -speed*4.2 / 127.0, 0.0);
            break;

        case EActionType::_TURN_CCW:
           // ccwBtn->setIcon(QIcon("./icons/small/green_ccw.png"));
            setRobotVel(0.0, 0.0, speed/20 );
            break;

        case EActionType::_TURN_CW:
           // cwBtn->setIcon(QIcon("./icons/small/green_cw.png"));
            setRobotVel(0.0, 0.0, -speed/20);
            break;

        case EActionType::_KICK:

            // kickBtn->setIcon(QIcon("./icons/small/kick_on.png"));
            cleanRobotChip();
            cmd.kickSpeed = kickspeed;
            break;

        case EActionType::_CHIP:
            // chipBtn->setIcon(QIcon("./icons/small/chip_on.png"));
            cleanRobotKick();
            cmd.chip = true;
            cmd.kickspeedz = chipkickspeed;
            break;

        case EActionType::_ROLLER:  //TODO i know what the hel is this but i dont know what the hel to do
//            if ( agents[currentAgent]->getRoller() )
//            {
//                //rollerBtn->setIcon(QIcon("./icons/small/roller_off.png"));
//                agents[currentAgent]->setRoller(0);
//            }
//            else
//            {
//                //rollerBtn->setIcon(QIcon("./icons/small/roller_on.png"));
//                agents[currentAgent]->setRoller(7);
//            }
            break;

        case EActionType::_STOP_ALL:
            break;

        }

    }

    void HandyController::setRobotVel(double _vtan , double _vnorm , double _w )
    {
        cmd.vel_F = _vtan;     cmd.vel_N = _vnorm;        cmd.vel_w = _w;
        double wheel1, wheel2, wheel3, wheel4;
        jacobian(_vtan, _vnorm, _w, wheel1, wheel2, wheel3, wheel4);
        cmd.wheel1 = static_cast<float>(wheel1);   cmd.wheel2 = static_cast<float>(wheel2);   cmd.wheel3 = static_cast<float>(wheel3);   cmd.wheel4 = static_cast<float>(wheel4);
    }

    void HandyController::cleanRobotVel()
    {
         cmd.vel_F = 0;     cmd.vel_N = 0;        cmd.vel_w = 0;
         cmd.wheel1 = 0;   cmd.wheel2 = 0;   cmd.wheel3 = 0;   cmd.wheel4 = 0;
    }

    void HandyController::cleanRobotChip()
    {
        cmd.chip = false;
        cmd.kickspeedz = 0;
    }

    void HandyController::cleanRobotKick()
    {
        cmd.kickSpeed = 0;
    }

    bool HandyController::eventFilter(QObject*, QEvent* event)
    {
        if (event->type() == QEvent::KeyPress)
        {
            QKeyEvent* e = static_cast<QKeyEvent*>(event);
            if(e->key() == Qt::Key_A)
                emit mUI.Left->pressed();
            if(e->key() == Qt::Key_W)
                emit mUI.Up->pressed();
            if(e->key() == Qt::Key_D)
                emit mUI.Right->pressed();
            if(e->key() == Qt::Key_S)
                emit mUI.Down->pressed();
            if(e->key() == Qt::Key_Q)
                emit mUI.Angle1->pressed();
            if(e->key() == Qt::Key_E)
                emit mUI.Angle2->pressed();
            if(e->key() == Qt::Key_K)
                emit mUI.Kick->pressed();
            if(e->key() == Qt::Key_C)
                emit mUI.Chip->pressed();
            if(e->key() == Qt::Key_R)
                emit mUI.Roller->pressed();
        }
        if (event->type() == QEvent::KeyRelease)
        {
            QKeyEvent* e = static_cast<QKeyEvent*>(event);
            if(e->key() == Qt::Key_A)
                emit mUI.Left->released();
            if(e->key() == Qt::Key_W)
                emit mUI.Up->released();
            if(e->key() == Qt::Key_D)
                emit mUI.Right->released();
            if(e->key() == Qt::Key_S)
                emit mUI.Down->released();
            if(e->key() == Qt::Key_Q)
                emit mUI.Angle1->released();
            if(e->key() == Qt::Key_E)
                emit mUI.Angle2->released();
            if(e->key() == Qt::Key_K)
                emit mUI.Kick->released();
            if(e->key() == Qt::Key_C)
                emit mUI.Chip->released();
            if(e->key() == Qt::Key_R)
                emit mUI.Roller->released();
        }
        return false;
    }

    void HandyController::leftpressed()
    {
        ROS_INFO("leftclicked");
        takeAction(EActionType::_LEFT);
        grsim_pub.publish(cmd);
        cleanRobotVel();
    }
    void HandyController::uppressed()
    {
        ROS_INFO("upclicked");
        takeAction(EActionType::_FORWARD);
        grsim_pub.publish(cmd);
        cleanRobotVel();
    }
    void HandyController::rightpressed()
    {
        ROS_INFO("rightclicked");
        takeAction(EActionType::_RIGHT);
        grsim_pub.publish(cmd);
        cleanRobotVel();
    }
    void HandyController::downpressed()
    {
        ROS_INFO("downclicked");
        takeAction(EActionType::_BACKWARD);
        grsim_pub.publish(cmd);
        cleanRobotVel();
    }
    void HandyController::angle1pressed()
    {
        ROS_INFO("angle1clicked");
        takeAction(EActionType::_TURN_CCW);
        grsim_pub.publish(cmd);
        cleanRobotVel();
    }
    void HandyController::angle2pressed()
    {
        ROS_INFO("angle2clicked");
        takeAction(EActionType::_TURN_CW);
        grsim_pub.publish(cmd);
        cleanRobotVel();
    }
    void HandyController::kickpressed()
    {
        ROS_INFO("kickclicked");
        takeAction(EActionType::_KICK);
        grsim_pub.publish(cmd);
        cleanRobotKick();
    }
    void HandyController::chippressed()
    {
        ROS_INFO("chipclicked");
        takeAction(EActionType::_CHIP);
        grsim_pub.publish(cmd);
        cleanRobotChip();
    }
    void HandyController::rollerpressed()       //TODO not complete
    {
        ROS_INFO("Rollerclicked");
        takeAction(EActionType::_ROLLER);
    }

    void HandyController::released()
    {
        grsim_pub.publish(cmd);
    }

    void HandyController::agentidchanged(QString id)
    {
        agentID = id.toInt();   cmd.robot_id = agentID;
    }
    void HandyController::speedchanged(int speed_)
    {
        speed = speed_;
    }
    void HandyController::kickspeedchanged(int speed_)
    {
        kickspeed = speed_;     cmd.kickSpeed =kickspeed;
    }
    void HandyController::chipkickspeedchanged(int speed_)
    {
        chipkickspeed = speed_; cmd.kickspeedz = chipkickspeed;
    }
    void HandyController::rollerspeedchanged(int speed_)
    {
        rollerspeed = speed_;   cmd.roller_speed = rollerspeed;
    }


/*bool hasConfiguration() const
{
  return true;
}

void triggerConfiguration()
{
  // Usually used to open a dialog to offer the user a set of configuration
}*/

    void HandyController::jacobian(double vx, double vy, double w, double &v1, double &v2, double &v3, double &v4)
    {
        float robotRadius = 0.0795;
        float wheelRadius = 0.0275;

        // Calculate Motor Speeds
        double motorAlpha[4] = {60.0 * _DEG2RAD, 135.0 * _DEG2RAD, 225.0 * _DEG2RAD, 300 * _DEG2RAD};

        double dw1 =  (1.0 / wheelRadius) * (( (robotRadius * w) - (vx * sin(motorAlpha[0])) + (vy * cos(motorAlpha[0]))) );
        double dw2 =  (1.0 / wheelRadius) * (( (robotRadius * w) - (vx * sin(motorAlpha[1])) + (vy * cos(motorAlpha[1]))) );
        double dw3 =  (1.0 / wheelRadius) * (( (robotRadius * w) - (vx * sin(motorAlpha[2])) + (vy * cos(motorAlpha[2]))) );
        double dw4 =  (1.0 / wheelRadius) * (( (robotRadius * w) - (vx * sin(motorAlpha[3])) + (vy * cos(motorAlpha[3]))) );

        float motorMaxRadPerSec = 1000*2*M_PI/60.0f;

        dw1 = (dw1 / (motorMaxRadPerSec)) * _BIT_RESOLUTION;
        dw2 = (dw2 / (motorMaxRadPerSec)) * _BIT_RESOLUTION;
        dw3 = (dw3 / (motorMaxRadPerSec)) * _BIT_RESOLUTION;
        dw4 = (dw4 / (motorMaxRadPerSec)) * _BIT_RESOLUTION;

        v1 = (char) (round(dw1));
        v2 = (char) (round(dw2));
        v3 = (char) (round(dw3));
        v4 = (char) (round(dw4));
    }
}  // namespace rqt_example_cpp
PLUGINLIB_EXPORT_CLASS(rqt_parsian_gui::HandyController, rqt_gui_cpp::Plugin)
