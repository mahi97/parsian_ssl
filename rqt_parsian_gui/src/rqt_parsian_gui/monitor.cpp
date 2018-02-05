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
    Monitor::~Monitor(){

    }

    void Monitor::initPlugin(qt_gui_cpp::PluginContext& context)
    {

        n = getNodeHandle();
        n_private = getPrivateNodeHandle();

        wm_sub = n.subscribe("/world_model", 1000, &Monitor::wmCb, this);
        draw_sub = n.subscribe("/draws", 1000, &Monitor::drawCb, this);
        color_sub = n.subscribe("/team_config", 1000, &Monitor::colorCb, this);
        timer = n.createTimer(ros::Duration(0.080), &Monitor::timerCb, this);
        parsian_msgs::parsian_team_configPtr team_config{new parsian_msgs::parsian_team_config};
        // access standalone command line arguments
        QStringList argv = context.argv();
        // create QWidget
        widget_ = new QWidget();
        drawer=new CguiDrawer();
        lastdrawer=new CguiDrawer();

//        QPushButton *startProf;
//        startProf=new QPushButton("nnnn",widget_);
//        QGridLayout *l = new QGridLayout(widget_);
//        l->addWidget(startProf);
//        widget_->setLayout(l);

        widget_->setWindowTitle("nadia");
        ourCol = QColor("blue");
        oppCol = QColor("yellow");


//        wm=new CWorldModel();

        fieldWidget=new MonitorWidget();


        // extend the widget with all attributes and children from UI file

        // add widget to the user interface
//        context.addWidget(widget_);
        context.addWidget(fieldWidget);
    }

    void Monitor::colorCb(const parsian_msgs::parsian_team_configConstPtr& _color){
        
        mycolor=_color;
        if(mycolor->color==0){
            ourCol = QColor("blue");
            oppCol = QColor("yellow");
        }
        else{
            ourCol = QColor("yellow");
            oppCol = QColor("blue");
        }

    }

    void Monitor::wmCb(const parsian_msgs::parsian_world_modelConstPtr &_wm) {




        mywm =_wm;



        drawer=new CguiDrawer();
//        drawer->polygonBuffer=lastdrawer->polygonBuffer;
//        drawer->rectBuffer=lastdrawer->rectBuffer;
//        drawer->pointBuffer=lastdrawer->pointBuffer;
//        drawer->segBuffer=lastdrawer->segBuffer;





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
        
        for (parsian_msgs::parsian_draw_circle cir: _draw->circles) {
            lastdrawer->arcBuffer->append(cir);

        }
        for (parsian_msgs::parsian_draw_polygon polygon: _draw->polygons) {
            lastdrawer->polygonBuffer->append(polygon);

        }
        for (parsian_msgs::parsian_draw_rect rect: _draw->rects) {
            lastdrawer->rectBuffer->append(rect);

        }
        for (parsian_msgs::parsian_draw_segment seg: _draw->segments) {
            lastdrawer->segBuffer->append(seg);
        }
        for (parsian_msgs::parsian_draw_text txt: _draw->texts) {
            lastdrawer->textBuffer->append(txt);

        }
        for (parsian_msgs::parsian_draw_vector point: _draw->vectors) {
            lastdrawer->pointBuffer->append(point);

        }
//        fieldWidget->update();

    }

    void Monitor::timerCb(const ros::TimerEvent &_timer) {

//        fieldWidget->drawerBuffer->clear();
        drawer->arcBuffer=lastdrawer->arcBuffer;
        drawer->segBuffer=lastdrawer->segBuffer;
        drawer->pointBuffer=lastdrawer->pointBuffer;
        drawer->textBuffer=lastdrawer->textBuffer;
        drawer->rectBuffer=lastdrawer->rectBuffer;
        drawer->polygonBuffer=lastdrawer->polygonBuffer;
        lastdrawer=drawer;
        fieldWidget->drawerBuffer=lastdrawer;

//        fieldWidget->drawerBuffer->draw(Circle2D(ballpos, radius), 0, 360, QColor("orange"), true);

        fieldWidget->update();

//        fieldWidget->drawerBuffer->robotBuffer.clear();
    }


    void Monitor::shutdownPlugin()
    {
        // unregister all publishers here
        ROS_INFO("Monitor closed");
        wm_sub.shutdown();
        draw_sub.shutdown();
        color_sub.shutdown();
        timer.stop();
        n.shutdown();
        n_private.shutdown();

    }


}  // namespace rqt_example_cpp
PLUGINLIB_EXPORT_CLASS(rqt_parsian_gui::Monitor, rqt_gui_cpp::Plugin)
