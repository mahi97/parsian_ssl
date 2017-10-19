/*
  Copyright 2016 Lucas Walter
*/

#include <rqt_parsian_gui/monitor.h>



namespace rqt_parsian_gui
{

    Monitor::Monitor() : rqt_gui_cpp::Plugin(), widget_(0)
//            ,               n(getNodeHandle()), n_private(getPrivateNodeHandle())
    {
        // Constructor is called first before initPlugin function, needless to say.
        // give QObjects reasonable names
        setObjectName("Nadia");
    }

    void Monitor::initPlugin(qt_gui_cpp::PluginContext& context)
    {

        n = getNodeHandle();
        n_private = getPrivateNodeHandle();

        wm_sub = n.subscribe("/world_model", 1000, &Monitor::wmCb, this);
        timer = n.createTimer(ros::Duration(0.006), &Monitor::timerCb, this);
        // access standalone command line arguments
        QStringList argv = context.argv();
        // create QWidget
        widget_ = new QWidget();

//        QPushButton *startProf;
//        startProf=new QPushButton("nnnn",widget_);
//        QGridLayout *l = new QGridLayout(widget_);
//        l->addWidget(startProf);
//        widget_->setLayout(l);

        widget_->setWindowTitle("nadia");


        wm=new CWorldModel();

        fieldWidget=new MonitorWidget();
//        fieldWidget->paintGL();
//        fieldWidget->drawerBuffer->clear();


        // extend the widget with all attributes and children from UI file

        // add widget to the user interface
//        context.addWidget(widget_);
        context.addWidget(fieldWidget);
    }

    void Monitor::wmCb(const parsian_msgs::parsian_world_modelConstPtr &_wm) {
        wm->update(*_wm);



        for( int i = 0; i < wm->our.activeAgentsCount(); i++ )
        {
            QColor col = QColor("yellow");
            if (fabs(wm->our.active(i)->inSight-0.5)<0.01)
            {
                col.setAlpha(150);
            }
            fieldWidget->drawerBuffer->drawRobot(wm->our.active(i)->pos, wm->our.active(i)->dir,
                                    col, wm->our.active(i)->id, i, "" ,false);

            //        if (soccer->agents[wm->our.active(i)->id]->goalVisibility>0)
            //            draw(QString::number(soccer->agents[wm->our.active(i)->id]->goalVisibility,'f',2), wm->our.active(i)->pos + Vector2D(-0.3, -0.1), QColor("black"), 14);


        }


        for( int i = 0; i < wm->opp.activeAgentsCount(); i++ )
        {
            QColor col = QColor("blue");
            if (fabs(wm->opp.active(i)->inSight-0.5)<0.01)
            {
                col.setAlpha(150);
            }


            fieldWidget->drawerBuffer->drawRobot(wm->opp.active(i)->pos, wm->opp.active(i)->dir,
                                    col, wm->opp.active(i)->id, -1);

        }



    }

    void Monitor::timerCb(const ros::TimerEvent &_timer) {
        ballpos.x=1;
        ballpos.y=1;
        fieldWidget->drawerBuffer->draw(Circle2D(ballpos, radius), 0, 360, QColor("orange"), true);
        fieldWidget->update();
    }


    void Monitor::shutdownPlugin()
    {
        // unregister all publishers here
    }

    void Monitor::saveSettings(qt_gui_cpp::Settings& plugin_settings,
                               qt_gui_cpp::Settings& instance_settings) const
    {
        // instance_settings.setValue(k, v)
    }

    void Monitor::restoreSettings(const qt_gui_cpp::Settings& plugin_settings,
                                  const qt_gui_cpp::Settings& instance_settings)
    {
        // v = instance_settings.value(k)
    }

/*bool hasConfiguration() const
{
  return true;
}

void triggerConfiguration()
{
  // Usually used to open a dialog to offer the user a set of configuration
}*/

}  // namespace rqt_example_cpp
PLUGINLIB_EXPORT_CLASS(rqt_parsian_gui::Monitor, rqt_gui_cpp::Plugin)
