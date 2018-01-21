//
// Created by parsian-ai on 12/31/17.
//

#ifndef RQT_PARSIAN_GUI_PLAYOFF_WIDGET_H
#define RQT_PARSIAN_GUI_PLAYOFF_WIDGET_H

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QColumnView>
#include <QList>
#include <QStandardItemModel>
#include <parsian_msgs/parsian_update_plans.h>
#include <parsian_util/tools/debuger.h>
#include <QWidget>
#include <ros/ros.h>
namespace rqt_parsian_gui {
    class PlayOffWidget : public QWidget {

    Q_OBJECT

    public:
        PlayOffWidget(ros::NodeHandle & n,QWidget *parent = 0);
        ~PlayOffWidget();

    protected:

        QPushButton *mode;
        QPushButton *active;
        QPushButton *update;
        QPushButton *master;
        QPushButton *deactive;

        QColumnView *columns;

        bool debugMode = false;
        QList<QList<bool> > activeGUI;
        QList<QList<bool> > masterGUI;

        QStandardItemModel *model;
        QItemSelectionModel *selection;

        QList<QStandardItem> *fileList;
        QList<QStandardItem> *planList;

        parsian_msgs::parsian_update_plans *theplans;
        ros::ServiceClient client ;

        QLabel *details[8];

        parsian_msgs::parsian_plan *lastPlan;
        std::string *chosen;
        QItemSelection itemSelected;

    private slots:
        void updateModel();
        void updateBtn(bool _debug);

    public slots:
        void slt_changeMode();
        void slt_updatePlans();
        void slt_active();
        void slt_deactive();
        void slt_edit(QStandardItem *);
        void slt_master();
        void slt_selectionChanged(const QItemSelection &, const QItemSelection &);

    };

}
#endif //RQT_PARSIAN_GUI_PLAYOFF_H
