#include <QOpenGLWidget>
#include <QFile>
#include <QWidget>
#include <QtOpenGL/qgl.h>
#include <QSlider>
#include <QLabel>
#include <QRect>
#include <QGroupBox>
#include <QCheckBox>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPixmap>
#include <QPainter>
#include <parsian_msgs/parsian_robots_status.h>
#include <ros/ros.h>
namespace rqt_parsian_gui {
    class RobotStatusWidget : public QWidget {
    Q_OBJECT
    public:
        explicit RobotStatusWidget(ros::NodeHandle &n);


    protected:
        GLuint list;

    private:
        void updateRobotVel(QRect);
        void updateBattery(QRect);
        void updateDataLoss(QRect);
        void updateSens(QRect);
        void updateFaults(QRect);


        //#########################################################################################################

        parsian_msgs::parsian_robot_statusConstPtr robotStatus;

        //#######################################################
        QGroupBox   *robot_vel,*battery,*data_loss,*status,*faults;
        QVBoxLayout *robot_vel_l,*battery_l,*data_loss_l,*status_l,*faults_l;
        QHBoxLayout *motors_l,*encoders_l,*main_layout;
        QProgressBar *battery_percentage, *data_loss_percentage;
        QCheckBox ** motors_f,**encoders_f,*kick_f,*chip_f,*shoot_sens_f,*shoot_board_f,*shoot_sens,*spin;
        QLabel *vel,*ang_vel,*agent;
        QPixmap agent_i;
        QPainter agent_p;
    };
}