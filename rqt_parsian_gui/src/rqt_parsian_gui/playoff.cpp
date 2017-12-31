//
// Created by parsian-ai on 12/31/17.
//

#include "rqt_parsian_gui/playoff.h"
class parsian_robot_status;

namespace rqt_parsian_gui {

    PlayOff::PlayOff()
            : rqt_gui_cpp::Plugin() {
        // Constructor is called first before initPlugin function, needless to say.
        // give QObjects reasonable names
        setObjectName("PlayOff");
    }

    void PlayOff::initPlugin(qt_gui_cpp::PluginContext &context) {
        n = getNodeHandle();
        n_private = getPrivateNodeHandle();



        // extend the widget with all attributes and children from UI file

        // add widget to the user interface

        //context.addWidget(scrollArea);
    }


    void PlayOff::shutdownPlugin(){

    }

}  // namespace rqt_parsian_gui
PLUGINLIB_EXPORT_CLASS(rqt_parsian_gui::PlayOff, rqt_gui_cpp::Plugin)
