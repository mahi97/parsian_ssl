/*
  Copyright 2016 Lucas Walter
*/
#include <rqt_parsian_gui/mode_chooser.h>



namespace rqt_parsian_gui
{

    ModeChooser::ModeChooser() : rqt_gui_cpp::Plugin(), widget_(0)
    {
        setObjectName("noOne");
    }

    void ModeChooser::initPlugin(qt_gui_cpp::PluginContext& context)
    {

        n = getNodeHandle();
        n_private = getPrivateNodeHandle();


        // create QWidget
        widget_ = new QWidget();
        widget_->setWindowTitle("noOne");
        fieldWidget=new ModeChooserWidget(n);
        context.addWidget(fieldWidget);
    }
}
PLUGINLIB_EXPORT_CLASS(rqt_parsian_gui::ModeChooser, rqt_gui_cpp::Plugin)
