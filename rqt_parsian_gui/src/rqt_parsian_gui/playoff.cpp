
#include <rqt_parsian_gui/playoff.h>

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
        client = n.serviceClient<parsian_msgs::parsian_update_plans> ("update_plans");

        playOffWidget = new PlayOffWidget();

        // extend the widget with all attributes and children from UI file

        // add widget to the user interface

        context.addWidget(playOffWidget);
    }


    void PlayOff::shutdownPlugin(){

    }

}  // namespace rqt_parsian_gui
PLUGINLIB_EXPORT_CLASS(rqt_parsian_gui::PlayOff, rqt_gui_cpp::Plugin)
