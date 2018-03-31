#include <rqt_parsian_gui/refereeWidget.h>


namespace rqt_parsian_gui
{
    RefereeWidget::RefereeWidget(ros::NodeHandle & n) : QWidget() {

        mainLayout = new QHBoxLayout;


        auto *btnRefGroup = new QButtonGroup();
        strRefNames << "H" << "FS" << "S" << "NS" << "FK" << "IK" << "KO" << "PK" << "BP" << "FK" << "IK" << "KO" << "PK" << "BP";
        strRefCommands << "H" << "s" << "S" << " " << "F" << "I" << "K" << "P" << "B" << "f" << "i" << "k" << "p" << "b";
        for(int i=0 ; i<14 ; i++ )
        {
            btnRefs[i] = new QPushButton(strRefNames[i],this);
            QString strType = "n";
            if( i >=4 )
                strType = "b";
            if( i >=9 )
                strType = "y";
            btnRefs[i]->setProperty("refType" , QVariant::fromValue(strType));
            btnRefGroup->addButton(btnRefs[i] , i);
        }
        for (auto &btnRef : btnRefs) {
            mainLayout->addWidget(btnRef);
        }
        double widgetWidth = 300;
        QString strWidth = QString("%1px").arg((int) (widgetWidth / 12));
        //StyleSheet
        QString styleSheet =
                QString("\
                    QPushButton[refType=\"n\"] {background-color:gray; color:black; padding: 0; border: 1px solid black; border-radius: 5px; width: %1} \
            QPushButton[refType=\"b\"] {background-color:blue; color:white; padding: 0; border: 1px solid black; border-radius: 5px; width: %2} \
            QPushButton[refType=\"y\"] {background-color:yellow; color:black; padding: 0; border: 1px solid black; border-radius: 5px; width: %3} \
            QPushButton[refType=\"n\"]:hover {border: 2px solid red} \
            QPushButton[refType=\"b\"]:hover {border: 2px solid red} \
            QPushButton[refType=\"y\"]:hover {border: 2px solid red} \
            ").arg(strWidth).arg(strWidth).arg(strWidth);

        this->setLayout(mainLayout);
        this->setStyleSheet(styleSheet);
        connect(btnRefGroup , SIGNAL(buttonClicked(int)) , this , SLOT(SetManualGS(int)));






        timer = n.createTimer(ros::Duration(0.016), &RefereeWidget::timerCb, this);
    }



    void RefereeWidget::timerCb(const ros::TimerEvent& _timer){

    }

    RefereeWidget::~RefereeWidget() = default;


}




