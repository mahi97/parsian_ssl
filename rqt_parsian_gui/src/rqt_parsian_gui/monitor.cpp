/*
  Copyright 2016 Lucas Walter
*/

#include <rqt_parsian_gui/monitor.h>
#include <rqt_parsian_gui/guiDrawer.h>


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
        draw_sub = n.subscribe("/draws", 1000, &Monitor::drawCb, this);
        timer = n.createTimer(ros::Duration(0.080), &Monitor::timerCb, this);
        // access standalone command line arguments
        QStringList argv = context.argv();
        // create QWidget
        widget_ = new QWidget();
        drawer=new CguiDrawer();

//        QPushButton *startProf;
//        startProf=new QPushButton("nnnn",widget_);
//        QGridLayout *l = new QGridLayout(widget_);
//        l->addWidget(startProf);
//        widget_->setLayout(l);

        widget_->setWindowTitle("nadia");


//        wm=new CWorldModel();

        fieldWidget=new MonitorWidget();


        // extend the widget with all attributes and children from UI file

        // add widget to the user interface
//        context.addWidget(widget_);
        context.addWidget(fieldWidget);
    }

    void Monitor::wmCb(const parsian_msgs::parsian_world_modelConstPtr &_wm) {



        mywm =_wm;



        drawer=new CguiDrawer();

        ROS_INFO("__%d__",drawer->robotBuffer.length());
        if(mywm->isYellow){
            ourCol = QColor("yellow");
            oppCol = QColor("blue");
        }
        else{
            ourCol = QColor("blue");
            oppCol = QColor("yellow");
        }


        drawer->guiBall.inSight=mywm->ball.inSight;
        drawer->guiBall.pos.x=mywm->ball.pos.x;
        drawer->guiBall.pos.y=mywm->ball.pos.y;
        drawer->guiBall.radius=mywm->ball.obstacleRadius;
        for( int i = 0; i < mywm->our.size(); i++ )
        {
            if (fabs(mywm->our[i].inSight-0.5)<0.01)
            {
                ourCol.setAlpha(150);
            }
            drawer->drawRobot(mywm->our[i].pos, mywm->our[i].dir,
                                                 ourCol, mywm->our[i].id, i, "" ,false);

            //        if (soccer->agents[wm->our.active(i)->id]->goalVisibility>0)
            //            draw(QString::number(soccer->agents[wm->our.active(i)->id]->goalVisibility,'f',2), wm->our.active(i)->pos + Vector2D(-0.3, -0.1), QColor("black"), 14);


        }



        for (const auto &i : mywm->opp) {


            if (fabs(i.inSight-0.5)<0.01)
            {
                oppCol.setAlpha(150);
            }


            drawer->drawRobot(i.pos, i.dir,
                                    oppCol, i.id, -1);

        }
//        fieldWidget->update();




    }

    void Monitor::drawCb(const parsian_msgs::parsian_drawConstPtr &_draw) {

//        fieldWidget->drawerBuffer->clear();
        for (parsian_msgs::parsian_draw_circle cir: _draw->circles) {
            drawer->arcBuffer.append(cir);

        }
        for (parsian_msgs::parsian_draw_polygon polygon: _draw->polygons) {
            drawer->polygonBuffer.append(polygon);

        }
        for (parsian_msgs::parsian_draw_rect rect: _draw->rects) {
            drawer->rectBuffer.append(rect);

        }
        for (parsian_msgs::parsian_draw_segment seg: _draw->segments) {
            drawer->segBuffer.append(seg);
        }
        for (parsian_msgs::parsian_draw_text txt: _draw->texts) {
            drawer->textBuffer.append(txt);

        }
        for (parsian_msgs::parsian_draw_vector point: _draw->vectors) {
            drawer->pointBuffer.append(point);

        }
//        fieldWidget->update();

    }

    void Monitor::timerCb(const ros::TimerEvent &_timer) {

        fieldWidget->drawerBuffer->clear();
        fieldWidget->drawerBuffer=drawer;

//        fieldWidget->drawerBuffer->draw(Circle2D(ballpos, radius), 0, 360, QColor("orange"), true);

        fieldWidget->update();
//        fieldWidget->drawerBuffer->robotBuffer.clear();
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
