//
// Created by rebinnaf on 10/19/17.
//
#include <parsian_msgs/parsian_draw_circle.h>
#include "rqt_parsian_gui/monitorWidget.h"

namespace rqt_parsian_gui
{

    MonitorWidget::MonitorWidget()
            :QOpenGLWidget()

    {
        // Constructor is called first before initPlugin function, needless to say.

        // give QObjects reasonable names
        list = glGenLists(0);
        glNewList(list, GL_COMPILE);
        field.setRect(-_FIELD_WIDTH/2.0, -_FIELD_HEIGHT/2.0, _FIELD_WIDTH, _FIELD_HEIGHT);
        fieldCenter.setRect(-_CENTER_CIRCLE_RAD, _CENTER_CIRCLE_RAD, 2*_CENTER_CIRCLE_RAD, 2*_CENTER_CIRCLE_RAD);
        leftPenalty.setRect(-_FIELD_WIDTH/2.0, - _PENALTY_WIDTH/2.0, _GOAL_RAD, _PENALTY_WIDTH);
        rightPenalty.setRect(_FIELD_WIDTH/2.0 - _GOAL_RAD, -_PENALTY_WIDTH/2.0, _GOAL_RAD, _PENALTY_WIDTH);
        this->setMouseTracking(true);
        //    this->setFixedSize((viewportSize.width()) , (viewportSize.height()));

        stadiumGreen = QColor::fromRgbF(0.34,0.78,0.18,1.0);//QColor::fromRgbF(0.50,0.25,0.0,1.0);//
        fieldGreen = QColor::fromRgbF(0.27,0.76,0.10,1.0);
        stadiumSize.setWidth((_STADIUM_WIDTH));
        stadiumSize.setHeight((_STADIUM_HEIGHT));
//        setViewportWidth(conf()->Common_Viewport_Width());
        drawerBuffer=new CguiDrawer();
    }
    void MonitorWidget::setViewportWidth(int width)
    {
        viewportWidth = width;
        WH_RATIO= (double)stadiumSize.width()/(double)stadiumSize.height();
        viewportSize.setWidth(viewportWidth);
        viewportSize.setHeight(double(viewportWidth / WH_RATIO));
        resizeGL(viewportWidth, ((double) viewportWidth)/WH_RATIO); //Dont care inputs
        setFixedSize(viewportSize.width(), viewportSize.height());
        //    setFixedSize(800, 600);

    }

    void MonitorWidget::initializeGL()
    {
        qglClearColor(stadiumGreen);
        glDisable(GL_DEPTH_TEST);
    }
    void MonitorWidget::paintGL() {


        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearDepth(1.0);
        glEnable(GL_BLEND);
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_COLOR);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glShadeModel(GL_SMOOTH);
        glLoadIdentity();
        glTranslated(0, 0, -10.0);
        drawField();


        CGraphicalRobot rob;
        while (!drawerBuffer->robotBuffer.isEmpty()) {
            rob = drawerBuffer->robotBuffer.dequeue();
            drawRobot(rob.pos.x,
                      rob.pos.y,
                      rob.dir.th().degree(),
                      rob.ID,
                      rob.comID,
                      rob.color,
                      rob.str,
                      rob.newRobots);

        }

        if (drawerBuffer->guiBall.inSight > 0)
        {
            drawArc(drawerBuffer->guiBall.pos.x,
                    drawerBuffer->guiBall.pos.y,
                    0.03,
                    0,
                    360,
                    QColor("orange"),
                    true);
        }

        parsian_msgs::parsian_draw_circle arc;
////        CGraphicalArc arc;
        while (!drawerBuffer->arcBuffer.isEmpty())
        {
            arc = drawerBuffer->arcBuffer.dequeue();
            QColor col=QColor(arc.color.r,arc.color.g,arc.color.b);

            drawArc(arc.circle.center.x,
                    arc.circle.center.y,
                    arc.circle.radius,
                    arc.startAng,
                    arc.endAng,
                    col,
                    arc.filled);
        }



        parsian_msgs::parsian_draw_polygon polygon;
        while (!drawerBuffer->polygonBuffer.isEmpty()) {
            polygon = drawerBuffer->polygonBuffer.dequeue();
            glColor4f(polygon.color.r, polygon.color.g, polygon.color.b, polygon.color.a);
            if (polygon.filled) {
                glBegin(GL_TRIANGLE_FAN);
            } else
                glBegin(GL_LINE_LOOP);


            for (unsigned int i = 0; i < polygon.points.size(); i++) {
                glVertex2f(polygon.points.at(i).x, -polygon.points.at(i).y);
            }
            glEnd();
        }


            parsian_msgs::parsian_draw_rect rec;
            while(!drawerBuffer->rectBuffer.isEmpty())
            {
                rec = drawerBuffer->rectBuffer.dequeue();

                QColor col=QColor(rec.color.r,rec.color.g,rec.color.b);

                drawRect(rec.rect.left_x,
                         rec.rect.top_y,
                         rec.rect.left_x+rec.rect.width,
                         rec.rect.top_y+rec.rect.length,
                         col,
                         rec.filled);
            }

            parsian_msgs::parsian_draw_segment seg;
            while(!drawerBuffer->segBuffer.isEmpty())
            {
                seg = drawerBuffer->segBuffer.dequeue();

                QColor col=QColor(seg.color.r,seg.color.g,seg.color.b);

                drawLine(seg.start.x, seg.start.y,
                         seg.end.x, seg.end.y,
                         col);
            }


            parsian_msgs::parsian_draw_vector pnt;
            //int sds=drawerBuffer->pointBuffer.size();
            while(!drawerBuffer->pointBuffer.isEmpty())
                //while(sds>0)
            {
                pnt = drawerBuffer->pointBuffer.dequeue();

                QColor col=QColor(pnt.color.r,pnt.color.g,pnt.color.b);
                // pnt = drawerBuffer->pointBuffer[sds-1];
                drawLine(pnt.vector.x-0.050, pnt.vector.y+0.050,
                         pnt.vector.x+0.050, pnt.vector.y-0.050,
                         col);
                drawLine(pnt.vector.x+0.050, pnt.vector.y+0.050,
                         pnt.vector.x-0.050, pnt.vector.y-0.050,
                         col);
                //sds--;
            }


        parsian_msgs::parsian_draw_text txt;
        while(!drawerBuffer->textBuffer.isEmpty())
        {
            txt = drawerBuffer->textBuffer.dequeue();
            QString str=QString::fromStdString(txt.value);
            QColor col=QColor(txt.color.r,txt.color.g,txt.color.b);
            drawText(txt.position.x, txt.position.y, str , col, txt.size);
        }
    }
    void MonitorWidget::resizeGL(int width, int height)
    {
        viewportSize.setWidth(width);
        viewportSize.setHeight(height);
        glViewport(0, 0, width, height);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(double(-1.0*stadiumSize.width()/2.0), double(stadiumSize.width()/2.0), double(stadiumSize.height()/2.0), double(-1*stadiumSize.height()/2.0), 4.0, 15.0);
        glMatrixMode(GL_MODELVIEW);
    }


    void MonitorWidget::qglClearColor(QColor clearColor) {
        glClearColor(clearColor.redF(), clearColor.greenF(), clearColor.blueF(), clearColor.alphaF());
    }




    GLuint MonitorWidget::drawArc(double centerX, double centerY, double radius, int start, int end, QColor color, bool fill, bool fullFill)
    {
        glColor4f(color.redF(),color.greenF(),color.blueF(),color.alphaF());
        centerY = -1*centerY;
        if(fill && fullFill)
        {
            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(centerX+sin(start * _DEG2RAD)*radius, centerY + cos(start * _DEG2RAD)*radius);
        }
        else if(fill)
        {
            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(centerX, centerY);
        }
        else
            glBegin(GL_LINE_STRIP);

        for( int angle=start; angle<=end; angle+=10 )
            glVertex2f(centerX+sin(angle * _DEG2RAD)*radius, centerY + cos(angle * _DEG2RAD)*radius);
        glEnd();

        glEndList();
        return list;
    }

    GLuint MonitorWidget::drawRect(double topLeftX, double topLeftY, double buttomRightX, double buttomRightY, QColor color, bool fill)
    {
        //glColor3f(color.redF(),color.greenF(),color.blueF());
        glColor4f(color.redF(),color.greenF(),color.blueF(),color.alphaF());
        topLeftY = -1 * topLeftY;
        buttomRightY= -1 * buttomRightY;
        if(fill)
        {
            glBegin(GL_TRIANGLE_FAN);
        }
        else
            glBegin(GL_LINE_LOOP);

        glVertex2f(topLeftX, topLeftY);
        glVertex2f(buttomRightX, topLeftY);
        glVertex2f(buttomRightX, buttomRightY);
        glVertex2f(topLeftX, buttomRightY);

        glEnd();

        glEndList();
        return list;

    }

    GLuint MonitorWidget::drawLine(double x1, double y1, double x2, double y2, QColor color)
    {
        glColor3f(color.redF(),color.greenF(),color.blueF());
        glBegin(GL_LINES);
        glVertex2f(x1, -1*y1);
        glVertex2f(x2, -1*y2);
        glEnd();

        glEndList();
        return list;
    }

    GLuint MonitorWidget::drawPoint(double x, double y, QColor color)
    {
        glColor3f(color.redF(),color.greenF(),color.blueF());
        glBegin(GL_POINTS);
        glVertex2f(x, -1*y);
        glEnd( );

        glEndList();
        return list;
    }

    void MonitorWidget::drawRobot(double x, double y, double ang, int ID, int comID, QColor color, QString str, bool newRobots)
    {

        double rad = newRobots ? robot_radius_new : robot_radius_old;
        if ( newRobots)
        {
            glCallList(drawArc(x, y, rad ,ang+140, ang+400, color, true, true));
            glCallList(drawLine(x, y, x + 0.05*cos(ang*_DEG2RAD) , y + 0.05*sin(ang*_DEG2RAD) , QColor("darkcyan")));
        }
        else
        {
            color.setGreen( (color.green() + 45) < 255 ? (color.green() + 45) : 255);
            color.setBlue( (color.blue() + 45) < 255 ? (color.blue() + 45) : 255);
            color.setRed( (color.red() + 45) < 255 ? (color.red() + 45) : 255);
            glCallList(drawArc(x, y, rad ,0, 360, color, true));
            glCallList(drawArc(x, y, rad ,0, 360, QColor(0, 0, 0), false));
            glCallList(drawLine(x, y, x + rad*cos(ang*_DEG2RAD), y + rad*sin(ang*_DEG2RAD), QColor(0, 0, 0)));
        }
        drawText(x,y+rad,QString("%1 %2").arg(ID).arg(str),QColor(0,0,0),10);
        if(comID!=-1){
            drawText(x,y-rad-0.100,QString::number(comID),QColor(255,0,0),10);
        }
    }

    void MonitorWidget::drawText(double x, double y, QString text, QColor color, int size)
    {

        glColor3f(color.redF(),color.greenF(),color.blueF());
        QFont font("Times", size);

        QFontMetrics fm(font);
        double pixelsWide = fm.width(text);


        painter.begin(this);

        painter.setPen(color);
        painter.setFont(font);
        painter.drawText(((x + stadiumSize.width() / 2.0)* (double(viewportSize.width()) / double(stadiumSize.width()))),
                         ((-1.0*y + stadiumSize.height() / 2.0) * (double(viewportSize.height()) / double(stadiumSize.height()))),
                         text);
        painter.end();
    }

    void MonitorWidget::drawField()
    {
        glCallList(drawLine(0, field.top(), 0, field.bottom()));
        glCallList(drawArc(0, 0, fieldCenter.width()/2, 0, 360));

        glCallList(drawLine(field.left(), field.top(), field.right(), field.top()));
        glCallList(drawLine(field.left(), field.bottom(), field.right(), field.bottom()));
        glCallList(drawLine(field.left(), field.top(), field.left(), field.bottom()));
        glCallList(drawLine(field.right(), field.top(), field.right(), field.bottom()));

        glCallList(drawArc(leftPenalty.left(), -1*(leftPenalty.bottom()-leftPenalty.width()), leftPenalty.width(), 0, 90));
        glCallList(drawArc(leftPenalty.left(), -1*(leftPenalty.top()+leftPenalty.width()), leftPenalty.width(), 90, 180));
        glCallList(drawLine(leftPenalty.left()+leftPenalty.width(), leftPenalty.bottom()-leftPenalty.width(), field.left()+leftPenalty.width(), leftPenalty.top()+leftPenalty.width() ));

        glCallList(drawArc(rightPenalty.right(), -1*(rightPenalty.bottom()-rightPenalty.width()), rightPenalty.width(), 270, 360));
        glCallList(drawArc(rightPenalty.right(), -1*(rightPenalty.top()+rightPenalty.width()), rightPenalty.width(), 180, 270));
        glCallList(drawLine(rightPenalty.right()-rightPenalty.width(), rightPenalty.bottom()-rightPenalty.width(), rightPenalty.right()-rightPenalty.width(), rightPenalty.top()+rightPenalty.width() ));

        glCallList(drawLine(field.left(), - _GOAL_WIDTH / 2.0, field.left() , _GOAL_WIDTH / 2.0, QColor("black")));
        glCallList(drawLine(field.right(), - _GOAL_WIDTH / 2.0, field.right() , _GOAL_WIDTH / 2.0, QColor("black")));
    }

    void MonitorWidget::mousePressEvent(QMouseEvent *event)
    {
//        QPoint tempMousePressPos = event->pos();
//        mousePressPos.x = ( ((double(tempMousePressPos.x()) / double(viewportSize.width())) * double(stadiumSize.width())) - (double(stadiumSize.width())/2.0));
//        mousePressPos.y = (-1.0*(((double(tempMousePressPos.y()) / double(viewportSize.height())) * double(stadiumSize.height())) - (double(stadiumSize.height())/2.0)));
//        ROS_INFO("__%f__%f__",mousePressPos.x ,mousePressPos.y);
    }

}  // namespace rqt_example_cpp


