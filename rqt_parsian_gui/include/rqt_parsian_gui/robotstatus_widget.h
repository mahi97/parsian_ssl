#include <QWidget>
#include <QSlider>
#include <QLabel>
#include <ros/ros.h>
namespace rqt_parsian_gui {
    class RobotStatusWidget : public QWidget {
    Q_OBJECT
    public:
        explicit RobotStatusWidget(ros::NodeHandle &n);


    signals:

    public slots:
    private:


    };
}