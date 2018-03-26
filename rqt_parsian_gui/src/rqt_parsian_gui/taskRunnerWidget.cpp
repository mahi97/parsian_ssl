#include <rqt_parsian_gui/taskRunnerWidget.h>


namespace rqt_parsian_gui
{
    TaskRunnerWidget::TaskRunnerWidget(ros::NodeHandle & n) : QWidget() {

        client = new parsian_msgs::grsim_ball_replacement();
        this->setFixedSize(200,50);
        gridLayout = new QGridLayout();

        toolButton = new QToolButton();
        toolButton->setText("Choose Task");

        agentId = new QToolButton;
        agentId->setText("agent id");

        tasks = new QAction* [TASK_NUM];

        for (int i = 0; i < TASK_NUM; ++i) {
            tasks[i] = new QAction(taskNames[i],this);
            connect(toolButton, SIGNAL(triggered(QAction * )), this, SLOT(setTask(QAction * )));
            toolButton->addAction(tasks[i]);
        }



        connect(toolButton,SIGNAL(),toolButton,SLOT());
        gridLayout->addWidget(toolButton);
        gridLayout->addWidget(agentId);
        this->setLayout(gridLayout);

        mousePosSub = n.subscribe("/mousePos",10, &TaskRunnerWidget::mousePosCallBack, this);
        ballReplacementClient = n.serviceClient<parsian_msgs::grsim_ball_replacement>("/GrsimBallReplacesrv",true);
        robTaskPub = new ros::Publisher[_MAX_NUM_PLAYERS];
        for (int i = 0; i < _MAX_NUM_PLAYERS; ++i) {
            std::string topic(QString("/agent_%1/task").arg(i).toStdString());
            robTaskPub[i] = n.advertise<parsian_msgs::parsian_robot_task>(topic, 1000);
        }
    }

    void TaskRunnerWidget::setTask(QAction* action) {
        toolButton->setText(action->text());
    }

    TaskRunnerWidget::~TaskRunnerWidget() {
    }

    void TaskRunnerWidget::mousePosCallBack(parsian_msgs::vector2DConstPtr pos) {
        if (QString::fromStdString(taskNames[0]) == toolButton->text()){  // BALL REPLACEMENT REQUEST
            client->request.vx = 0;
            client->request.vy = 0;
            client->request.x = pos->x;
            client->request.y = pos->y;
            if(ballReplacementClient.call(*client)){
                ROS_INFO_STREAM("YES");
            } else ROS_INFO_STREAM("CALL FAILED");


        } else{
        }
    }

}


