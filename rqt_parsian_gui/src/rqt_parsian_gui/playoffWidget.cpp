//
// Created by parsian-ai on 12/31/17.
//

#include "rqt_parsian_gui/playoffWidget.h"

using namespace rqt_parsian_gui;

PlayOffWidget::PlayOffWidget(QWidget *parent) : QWidget(parent) {

    theplans = new parsian_msgs::parsian_update_plans();

    //plans.clear();
    //  m_plans.append(_loader->getPlans());

    choosen = NULL;

    mode = new QPushButton("Debug Mode", this);
    update = new QPushButton("Update (Don't Worry! it will work fine :)", this);
    active = new QPushButton("Active");
    deactive = new QPushButton("Deactive");
    master = new QPushButton("Master");

    columns = new QColumnView();

    active->setEnabled(false);
    update->setEnabled(false);
    deactive->setEnabled(false);
    master->setEnabled(false);

    //    selection = columns->selectionModel();

    model = new QStandardItemModel();
    selection = new QItemSelectionModel(model);

    updateModel();

    columns->setModel(model);
    columns->setFont(QFont("Monospace"));
    columns->setSelectionModel(selection);

    QList<int> widthList;
    widthList.append(300);
    widthList.append(200);
    widthList.append(100);
    columns->setColumnWidths(widthList);

    //*Details*//


    QFrame *line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);


    QHBoxLayout *buttons = new QHBoxLayout;
    QVBoxLayout *main = new QVBoxLayout(this);
    QVBoxLayout *detail = new QVBoxLayout;

    buttons->addWidget(deactive);
    buttons->addWidget(active);
    buttons->addWidget(master);

    for (int i = 0; i < 8; i++) {
        details[i] = new QLabel(this);
        details[i]->setFont(QFont("Monospace"));
        detail->addWidget(details[i]);
    }


    main->addWidget(mode);
    main->addWidget(update);
    main->addWidget(columns);
    main->addLayout(buttons);
    main->addWidget(line); //  <-- Just A Line
    main->addLayout(detail);

    connect(update, SIGNAL(clicked()), this, SLOT(slt_updatePlans()));
    connect(mode, SIGNAL(clicked()), this, SLOT(slt_changeMode()));
    connect(active, SIGNAL(clicked()), this, SLOT(slt_active()));
    connect(master, SIGNAL(clicked()), this, SLOT(slt_master()));
    connect(deactive, SIGNAL(clicked()), this, SLOT(slt_deactive()));
    connect(model, SIGNAL(itemChanged(QStandardItem * )), this, SLOT(slt_edit(QStandardItem * )));
    connect(selection, SIGNAL(selectionChanged(
                                      const QItemSelection &, const QItemSelection &)),
            this, SLOT(slt_selectionChanged(QItemSelection, QItemSelection)));
    //connect(m_loader, SIGNAL(plansUpdated()), this, SLOT(updateModel()));

    setLayout(main);
}

PlayOffWidget::~PlayOffWidget() {
}

void PlayOffWidget::updateModel() {

    model->clear();
    QStandardItem *pkg;
    QStandardItem *file;
    QStandardItem *plan;
    int pkgCounter = 0;
    int fileCounter = 0;
    int planCounter = 0;
    SGUI *lastGui = new SGUI;
    for (size_t i = 0; i < m_plans.size(); i++) {
        m_plans[i]->gui.index[0] = pkgCounter;
        m_plans[i]->gui.index[1] = fileCounter;
        m_plans[i]->gui.index[2] = planCounter;

        SGUI &guiPlan = m_plans.at(i)->gui;
        if (lastGui->package != guiPlan.package) {
            pkgCounter++;
            fileCounter++;
            qDebug() << "PKG";
            pkg = new QStandardItem(guiPlan.package);
            model->appendRow(pkg);
            qDebug() << "FILE";
            file = new QStandardItem(guiPlan.planFile);
            QString temp = guiPlan.package;
            file->setToolTip(
                    "<html><img src=" + temp.replace(".", "/") + "/" + guiPlan.planFile + ".png" + "/></html>");
            pkg->appendRow(file);
        } else if (lastGui->planFile != guiPlan.planFile) {
            fileCounter++;
            qDebug() << "PLAN";
            file = new QStandardItem(guiPlan.planFile);
            QString temp = guiPlan.package;
            file->setToolTip(
                    "<html><img src=" + temp.replace(".", "/") + "/" + guiPlan.planFile + ".png" + "/></html>");
            pkg->appendRow(file);
        }
        planCounter++;
        plan = new QStandardItem(QString("%1").arg(i));
        file->appendRow(plan);

        file->setEditable(false);
        plan->setEditable(false);
        pkg->setEditable(false);
        lastGui = &m_plans.at(i)->gui;

    }
}

void PlayOffWidget::slt_changeMode() {
    debugMode = !debugMode;
    mode->setText((debugMode) ? "Game Mode" : "Debug Mode");
    updateBtn(debugMode);
    qDebug() << "Mode Chaged to " << ((debugMode) ? "Debug" : "Game") << " Mode";
}

void PlayOffWidget::updateBtn(bool _debug) {
    if (_debug) {
        update->setEnabled(true);
        columns->setEnabled(true);
        active->setEnabled(true);
        master->setEnabled(true);
        deactive->setEnabled(true);
    } else {
        active->setEnabled(false);
        update->setEnabled(false);
        deactive->setEnabled(false);
        master->setEnabled(false);
    }
}

void PlayOffWidget::slt_updatePlans() {

    //m_loader->loadAll();
    m_plans.clear();
    //m_plans.append(m_loader->getPlans());
    updateModel();
}

void PlayOffWidget::slt_active() {
    QModelIndexList modelList = m_itemSelected.indexes();
    Q_FOREACH(QModelIndex model, modelList) {
            if (model.parent().row() == -1) {
                int i = -1;
                while (model.child(++i, 0).data().toString() != "") {
                    int j = -1;
                    while (model.child(i, 0).child(++j, 0).data().toString() != "") {
                        m_plans.at(model.child(i, 0).child(j, 0).data().toInt())->gui.active = true;
                        m_plans.at(model.child(i, 0).child(j, 0).data().toInt())->gui.master = true;

                    }
                }
            } else if (model.parent().parent().row() == -1) {
                details[0]->setText(QString("Type : File"));
                int i = -1;
                while (model.child(++i, 0).data().toString() != "") {
                    m_plans.at(model.child(i, 0).data().toInt())->gui.active = true;
                    m_plans.at(model.child(i, 0).data().toInt())->gui.master = false;

                }
            } else if (model.parent().parent().parent().row() == -1) {

                int planIndex = model.data().toInt();

                m_choosen = m_plans.at(planIndex);
                m_choosen->gui.active = true;
                m_choosen->gui.master = false;

            }

            active->setEnabled(false);
            deactive->setEnabled(true);
            master->setEnabled(true);

        }
}

void PlayOffWidget::slt_deactive() {
    QModelIndexList modelList = m_itemSelected.indexes();
    Q_FOREACH(QModelIndex model, modelList) {
            if (model.parent().row() == -1) {
                int i = -1;
                while (model.child(++i, 0).data().toString() != "") {
                    int j = -1;
                    while (model.child(i, 0).child(++j, 0).data().toString() != "") {
                        m_plans.at(model.child(i, 0).child(j, 0).data().toInt())->gui.master = false;
                        m_plans.at(model.child(i, 0).child(j, 0).data().toInt())->gui.active = false;
                    }
                }
            } else if (model.parent().parent().row() == -1) {
                details[0]->setText(QString("Type : File"));
                int i = -1;
                while (model.child(++i, 0).data().toString() != "") {
                    m_plans.at(model.child(i, 0).data().toInt())->gui.master = false;
                    m_plans.at(model.child(i, 0).data().toInt())->gui.active = false;
                }
            } else if (model.parent().parent().parent().row() == -1) {

                int planIndex = model.data().toInt();

                m_choosen = m_plans.at(planIndex);
                m_choosen->gui.active = false;
                m_choosen->gui.master = false;
            }

            active->setEnabled(true);
            deactive->setEnabled(false);
            master->setEnabled(true);

        }
}

void PlayOffWidget::slt_master() {
    QModelIndexList modelList = m_itemSelected.indexes();
    Q_FOREACH(QModelIndex model, modelList) {
            if (model.parent().row() == -1) {
                int i = -1;
                while (model.child(++i, 0).data().toString() != "") {
                    int j = -1;
                    while (model.child(i, 0).child(++j, 0).data().toString() != "") {
                        m_plans.at(model.child(i, 0).child(j, 0).data().toInt())->gui.master = true;
                        m_plans.at(model.child(i, 0).child(j, 0).data().toInt())->gui.active = true;
                    }
                }
            } else if (model.parent().parent().row() == -1) {
                details[0]->setText(QString("Type : File"));
                int i = -1;
                while (model.child(++i, 0).data().toString() != "") {
                    m_plans.at(model.child(i, 0).data().toInt())->gui.master = true;
                    m_plans.at(model.child(i, 0).data().toInt())->gui.active = true;
                }
            } else if (model.parent().parent().parent().row() == -1) {

                int planIndex = model.data().toInt();

                m_choosen = m_plans.at(planIndex);
                m_choosen->gui.active = true;
                m_choosen->gui.master = true;
            }

            active->setEnabled(false);
            deactive->setEnabled(true);
            master->setEnabled(false);

        }
}

void PlayOffWidget::slt_edit(QStandardItem *_item) {
    // TODO : Make it possible to edit plans via gui.
}

void PlayOffWidget::slt_selectionChanged(const QItemSelection &selected, const QItemSelection &deselected) {

    for (int i = 0; i < 8; i++) details[i]->setText("");

    m_choosen = NULL;
    m_itemSelected = selected;
    QModelIndexList modelList = selected.indexes();
    Q_FOREACH(QModelIndex model, modelList) {

            if (model.parent().row() == -1) {
                details[0]->setText(QString("Type  : Package"));

                active->setEnabled(true);
                deactive->setEnabled(true);
                master->setEnabled(true);

            } else if (model.parent().parent().row() == -1) {
                details[0]->setText(QString("Type : File"));

                active->setEnabled(true);
                deactive->setEnabled(true);
                master->setEnabled(true);

            } else if (model.parent().parent().parent().row() == -1) {

                int planIndex = model.data().toInt();

                m_choosen = m_plans.at(planIndex);

                details[0]->setText(QString("Type       : Plan"));
                details[1]->setText(QString("Agent Size : %1").arg(m_choosen->common.agentSize));
//                details[2]->setText(QString("Plan Mode  : %1").arg(m_loader->getModeStr(m_choosen->common.planMode)));
                details[3]->setText(QString("Chance     : %1").arg(m_choosen->common.chance));
                details[4]->setText(QString("Last Dist  : %1").arg(m_choosen->common.lastDist));
                details[5]->setText(QString("Tags       : %1").arg(m_choosen->common.tags.join(" - ")));
                details[6]->setText(QString("ShotPos    : (%1, %2)").arg(m_choosen->matching.shotPos.x).arg(
                        m_choosen->matching.shotPos.y));
//                details[7]->setText(QString("ShotZone   : %1").arg(CCoach::getShotSpot(m_choosen->matching.initPos.ball, m_choosen->matching.shotPos)));

                active->setEnabled(!m_choosen->gui.active);
                deactive->setEnabled(m_choosen->gui.active);
                master->setEnabled(!m_choosen->gui.master);

            } else {
                details[0]->setText(QString("Type : SubPlan !!"));

            }
        }

}