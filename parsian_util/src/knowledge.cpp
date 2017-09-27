//
// Created by parsian-ai on 9/21/17.
//

#include "parsian_util/knowledge.h"

CKnowledge::CKnowledge() {

}

CKnowledge::~CKnowledge() {

}

double CKnowledge::getEmptyAngle(Vector2D p , Vector2D p1 , Vector2D p2 , QList<Circle2D> obs , double &percent ,
                                double &mostOpenAngle , double &biggestAngle , bool oppGoal , bool _draw) {

    bool drawn = false;
//    QColor rect_color;
    if(oppGoal)
    {
        int r , g , b;
//        QColor("darkcyan").getRgb(&r , &g, &b);
//        rect_color =  QColor(r, g, b, 21);
    }
    else
    {
        int r , g , b;
//        QColor("magenta").getRgb(&r , &g, &b);
//        rect_color = QColor(r , g, b, 21);
    }
    Vector2D goal_pos;

    if(oppGoal)
        goal_pos = field.oppGoal();
    else
        goal_pos = field.ourGoal();

    double gx1 = p1.x;
    double gy1 = p1.y;
    double gx2 = p2.x;
    double gy2 = p2.y;
    double x = p.x;
    double y = p.y;
    //similar codes can be found in cl/evalpos.c
    double d, a1, a2, a, l, a0, q1, q2, al;
    double la, lb, lc;
    double ox, oy;
    la = gy2-gy1;
    lb = gx1-gx2;
    lc = -gx1*la-gy1*lb;
    bool inobs = false;
    struct range tmpr{};
    int count = 0;
    int i,j;
    d = 0;
    struct range r[20];
    bool flag[20];
    for (i = 0;i<20;i++)
        flag[i] = false;
    al = getangle(x, y, (gx1+gx2)*0.5, (gy1+gy2)*0.5);
    q1 = getangle(x,y,gx1,gy1)-al;
    q2 = getangle(x,y,gx2,gy2)-al;
    q1 = normalang(q1);
    q2 = normalang(q2);
    if (normalang(q1 - q2) > 0)
    {
        a = q1;
        q1 = q2;
        q2 = a;
    }
    float openangle = 0;
    for (i = 0; i < obs.count(); ++i) {
        ox = obs[i].center().x;
        oy = obs[i].center().y;
        float rad = obs[i].radius();
        l = len(x,y,ox,oy);
        if (l<rad) {inobs = true;break;}
        a1 = ox*la + oy*lb + lc;
        a2 = x*la + y*lb + lc;
        if (a1 > 0) a1 = 1;else a1 = -1;
        if (a2 > 0) a2 = 1;else a2 = -1;
        a1 = a1*a2;
        if (a1 > 0)
        {
            a = normalang(getangle(x,y,ox,oy)-al);
            a0 = asin(rad/l);
            a1 = a - a0;
            a2 = a + a0;
            if (a1 < -M_PI*0.95*0.5) a1 = -M_PI*0.95*0.5;
            if (a2 < -M_PI*0.95*0.5) a2 = -M_PI*0.95*0.5;
            if (a1 > +M_PI*0.95*0.5) a1 =  M_PI*0.95*0.5;
            if (a2 > +M_PI*0.95*0.5) a2 =  M_PI*0.95*0.5;
            if (normalang(a1-a2)>0)
            {
                a = a1;
                a1 = a2;
                a2 = a;
            }
            a1 = normalang(a1);
            a2 = normalang(a2);
            if (normalang(a1-q1)<=0) a1 = q1;
            if (normalang(a1-q2)>=0) a1 = q2;
            if (normalang(a2-q1)<=0) a2 = q1;
            if (normalang(a2-q2)>=0) a2 = q2;
            if (normalangabs(a1-a2)>=0.001)
            {
                r[count].a = a1;
                r[count].b = a2;
                count ++;
            }
        }
    }
    if (!inobs)
    {
        for (i=0;i<count;i++)
            for (j=0;j<count-1;j++)
            {
                if (normalang(r[j].a-r[j+1].a) > 0)
                {
                    tmpr = r[j];
                    r[j] = r[j+1];
                    r[j+1] = tmpr;
                }
            }
        for (i=0;i<count-1;i++)
        {
            if (normalang(r[i+1].a - r[i].b) < 0)
            {
                r[i+1].a = r[i].a;
                if (normalang(r[i+1].b - r[i].b) < 0)
                {
                    r[i+1].b = r[i].b;
                }
                flag[i] = true;
            }
        }
        bool changed = false;
        if (count > 0)
        {
            double lastBlockedDir = q1;
            int k = 0;
            mostOpenAngle = 0;
            biggestAngle = 0;
            for (i=0;i < count;i++)
            {
                if (! flag[i])
                {
                    if ((k == 0) && (normalang(q1 - r[i].a) >= 0))
                    {

                    }
                        /*                    else if ((i == count-1) && (normalang(r[count-1].b - q2) >= 0))
                        {

                        }*/
                    else {
                        float dist = normalangabs(r[i].a - lastBlockedDir);
                        float bisect = normalang(normalang(r[i].a - lastBlockedDir) / 2.0 + lastBlockedDir);
                        if (dist >= biggestAngle)
                        {
                            biggestAngle = dist;
                            mostOpenAngle = bisect;
                            changed = true;
                        }
                    }
                    if(_draw)
                    {
                        Line2D line1(p, p+Vector2D::unitVector((lastBlockedDir+al) * 180.0 / M_PI));
                        Line2D line2(p, p+Vector2D::unitVector(((r[i].a+al) * 180.0 / M_PI)));
                        Line2D goal_line(goal_pos, Vector2D(goal_pos.x, goal_pos.y+2.0));
                        Vector2D p3(line1.intersection(goal_line));
                        Vector2D p4(line2.intersection(goal_line));

                        Polygon2D polygon_draw;
                        polygon_draw.addVertex(p);
                        polygon_draw.addVertex(p3);
                        polygon_draw.addVertex(p4);
                        polygon_draw.addVertex(p);
//                        draw( polygon_draw, rect_color, true);
                        drawn = true;

                        //draw(Segment2D(p, p+Vector2D::unitVector((bisect+al) * 180.0 / M_PI)*5 ), "black");
                        //draw(Segment2D(p, p+Vector2D::unitVector((r[i].a+al) * 180.0 / M_PI)*5 ), "black");
                        //draw(Segment2D(p, p+Vector2D::unitVector((lastBlockedDir+al) * 180.0 / M_PI)*5 ), "purple");
                    }
                    lastBlockedDir = r[i].b;
                    k ++;
                }
            }
            if (normalang(r[count-1].b - q2) <= 0)
            {
                float dist = normalangabs(q2 - lastBlockedDir);
                float bisect = normalang(normalang(q2 - lastBlockedDir) / 2.0 + lastBlockedDir);
                if (dist >= biggestAngle)
                {
                    biggestAngle = dist;
                    mostOpenAngle = bisect;
                    changed = true;
                }
                if(_draw)
                {
                    Line2D line1(p, p+Vector2D::unitVector((lastBlockedDir+al) * 180.0 / M_PI));
                    Line2D line2(p, p+Vector2D::unitVector(((q2+al) * 180.0 / M_PI)));
                    Line2D goal_line(goal_pos, Vector2D(goal_pos.x, goal_pos.y+2.0));
                    Vector2D p1(line1.intersection(goal_line));
                    Vector2D p2(line2.intersection(goal_line));

                    Polygon2D polygon_draw;
                    polygon_draw.addVertex(p);
                    polygon_draw.addVertex(p1);
                    polygon_draw.addVertex(p2);
                    polygon_draw.addVertex(p);
//                    draw( polygon_draw, rect_color, true);
                    drawn = true;

                    //                        draw(Segment2D(p, p+Vector2D::unitVector((bisect+al) * 180.0 / M_PI)*5 ), "blue");
                    //                        draw(Segment2D(p, p+Vector2D::unitVector((q2+al) * 180.0 / M_PI)*5 ), "red");
                    //                        draw(Segment2D(p, p+Vector2D::unitVector((lastBlockedDir+al) * 180.0 / M_PI)*5 ), "orange");
                }
            }
        }
        for (i=0;i<count;i++)
        {
            if (! flag[i])
            {
                d += normalang(r[i].b - r[i].a);
                flag[i] = true;
            }
        }
        openangle = d;
        openangle = normalangabs(q2-q1) - openangle;
        d /= normalangabs(q2-q1);
        d = 1-d;
        if (!changed || (d<0.001)){
            if (count==0) biggestAngle = normalangabs(q2 - q1);
            else biggestAngle = 0;
            mostOpenAngle = normalang(normalang(q2 - q1) / 2.0 + q1);
        }
        mostOpenAngle = normalang(mostOpenAngle + al);
        biggestAngle *= 180.0 / M_PI;
        mostOpenAngle *= 180.0 / M_PI;
    }
    else {
        mostOpenAngle = normalang(0.5*(q1 + q2) + al);
        biggestAngle = 0;
        mostOpenAngle *= 180.0 / M_PI;
        //                         draw(Segment2D(p, p+Vector2D::unitVector((mostOpenAngle) )*5 ), "blue");

        d = 0.0;
    }
    percent = d;

    if(_draw && !drawn)
    {
        Vector2D p3, p4;
        if(oppGoal)
        {
            p3 = field.oppGoalL();
            p4 = field.oppGoalR();
        }
        else
        {
            p3 = field.ourGoalL();
            p4 = field.ourGoalR();
        }
        Polygon2D polygon_draw;
        polygon_draw.addVertex(p);
        polygon_draw.addVertex(p3);
        polygon_draw.addVertex(p4);
        polygon_draw.addVertex(p);

//        draw( polygon_draw, rect_color, true);
    }
    return openangle * 180.0 / M_PI;
}

Vector2D CKnowledge::getReflectPos(Vector2D goal, double dist, Vector2D _ballpos) {
    Vector2D res;
    Segment2D dummySeg(goal,goal+Vector2D(-5,0));
    Vector2D nearest(dummySeg.nearestPoint(_ballpos));
    Vector2D sol1,sol2;
    Rect2D oppField(0,CField::_FIELD_HEIGHT/2 -0.01,CField::_FIELD_WIDTH/2 + 0.01,CField::_FIELD_HEIGHT -0.01);
    Circle2D oppCircle(Vector2D(CField::_FIELD_WIDTH/2,0)- Vector2D(1,0),dist);



    res.x = nearest.x;
    res.y = nearest.y*2 - _ballpos.y;

    dummySeg.assign(CField::oppGoal(),(res - CField::oppGoal()).norm()*12);
    //    oppField.intersection(dummySeg,&sol1,&sol2);
    oppCircle.intersection(dummySeg,&sol1,&sol2);
    if(CField::isInField(sol1))
        res = sol1;
    else
        res = sol2;

    return res;
}