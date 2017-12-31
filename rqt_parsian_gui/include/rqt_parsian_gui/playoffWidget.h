//
// Created by parsian-ai on 12/31/17.
//

#ifndef RQT_PARSIAN_GUI_PLAYOFF_H
#define RQT_PARSIAN_GUI_PLAYOFF_H

#include <QDockWidget>
#include <QLabel>
#include <QGridLayout>
#include <QTableWidget>
#include <QItemDelegate>
#include <QMetaType>
#include <QPointF>
#include <QVector>
#include <QWidget>
#include <QFocusEvent>
#include <QPainter>
#include <QToolButton>
#include <QPushButton>
#include <QLCDNumber>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QFrame>
#include <QSize>
#include <QPoint>
#include <QRect>
#include <QString>
#include <QLineEdit>
#include <QCheckBox>
#include <QTabWidget>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QColor>
#include <QTextDocument>
#include <QTime>
#include <QComboBox>
#include <QStackedWidget>
#include <QCheckBox>
#include <QLineEdit>
#include <QLabel>
#include <QButtonGroup>
#include <QProgressBar>
#include <QDockWidget>
#include <QFile>
#include <QTextStream>
#include <QComboBox>
#include <QCheckBox>
#include <QTime>
#include <QColorDialog>
#include <QPaintEvent>
#include <QLabel>
#include <QSize>
#include <QPoint>
#include <QRect>
#include <QString>
#include <QSpacerItem>
#include <QSlider>
#include <QTreeView>
#include <QScrollBar>
#include <QMessageBox>
#include <QFrame>
#include <QMenuBar>
#include <QColumnView>
#include <QStandardItemModel>
class PlayOffWidget : public QWidget {

    Q_OBJECT

public:
    PlayOffWidget(CLoadPlayOffJson* _loader, QWidget *parent = 0);
    ~PlayOffWidget();

protected:

    QPushButton *mode;
    QPushButton *active;
    QPushButton *update;
    QPushButton *master;
    QPushButton *deactive;

    QColumnView *columns;

    bool debugMode = false;
    QList< QList<bool> > activeGUI;
    QList< QList<bool> > masterGUI;

    QStandardItemModel *model;
    QItemSelectionModel *selection;

    QList<QStandardItem> *fileList;
    QList<QStandardItem> *planList;

    QList<NGameOff::SPlan*> m_plans;
    QList< QList< SPlayOffPlan*> >plans;
    CLoadPlayOffJson* m_loader;

    QLabel *details[8];

    NGameOff::SPlan* m_choosen;
    QItemSelection m_itemSelected;
private slots:

    void updateModel();
    void updateBtn(bool _debug);

public slots:
    void slt_changeMode();
    void slt_updatePlans();
    void slt_active();
    void slt_deactive();
    void slt_edit(QStandardItem*);
    void slt_master();
    void slt_selectionChanged(const QItemSelection &, const QItemSelection &);

};

#endif //RQT_PARSIAN_GUI_PLAYOFF_H
