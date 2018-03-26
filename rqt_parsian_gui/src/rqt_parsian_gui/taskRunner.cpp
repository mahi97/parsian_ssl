#include <rqt_parsian_gui/taskRunner.h>



namespace rqt_parsian_gui
{

    TaskRunner::TaskRunner() : rqt_gui_cpp::Plugin()
    {
        setObjectName("noOne");
    }

    void TaskRunner::initPlugin(qt_gui_cpp::PluginContext& context)
    {

        n = getNodeHandle();
        n_private = getPrivateNodeHandle();
        // create QWidget

        taskRunnerWidget = new TaskRunnerWidget(n);
        context.addWidget(taskRunnerWidget);

    }

    void TaskRunner::shutdownPlugin() {
        ROS_INFO("Sddf");
        n.shutdown();
        n_private.shutdown();

    }

}

PLUGINLIB_EXPORT_CLASS(rqt_parsian_gui::TaskRunner, rqt_gui_cpp::Plugin)
