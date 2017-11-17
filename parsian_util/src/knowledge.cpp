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
    Rect2D oppField(0,field._FIELD_HEIGHT/2 -0.01,field._FIELD_WIDTH/2 + 0.01,field._FIELD_HEIGHT -0.01);
    Circle2D oppCircle(Vector2D(field._FIELD_WIDTH/2,0)- Vector2D(1,0),dist);



    res.x = nearest.x;
    res.y = nearest.y*2 - _ballpos.y;

    dummySeg.assign(field.oppGoal(),(res - field.oppGoal()).norm()*12);
    //    oppField.intersection(dummySeg,&sol1,&sol2);
    oppCircle.intersection(dummySeg,&sol1,&sol2);
    if(field.isInField(sol1))
        res = sol1;
    else
        res = sol2;

    return res;
}

int CKnowledge::getNearestRobotToPoint(CTeam _team, Vector2D _point) {
    double minDist = 1.0e13;
    int nearest = -1;
    for(int i = 0; i < _team.activeAgentsCount(); i++ )
    {
        if (_team.active(i)->inSight <=0)
            continue;
        double dist = (_team.active(i)->pos - _point).length();
        if( dist < minDist )
        {
            minDist = dist;
            nearest = _team.active(i)->id;
        }
    }
    return nearest;
}

NewFastestToBall CKnowledge::newFastestToBall(double timeStep, QList<int> ourList, QList<int> oppList){
    ////
    ////Code By Sepehr
    ////

    // reset everything
    NewFastestToBall result;
    if(!wm->field->fieldRect().contains(wm->ball->pos))
        return result;

    double t = 0;
    Vector2D ballPredict;

    bool ourCalced[_MAX_NUM_PLAYERS];
    bool oppCalced[_MAX_NUM_PLAYERS];
    for( int i=0 ; i<_MAX_NUM_PLAYERS ; i++ ){
        ourCalced[i] = false;
        oppCalced[i] = false;
    }

    // use the correct Robot acceleration and maximum Velocity below :
    double robotMaxVel;
    ros::param::get("agent_node/VelMax", robotMaxVel); 
    double robotMAxAcc;
    ros::param::get("agent_node/AccMaxForward", robotMAxAcc);

    while ( t < 20 && (result.ourF.size() < ourList.size() || result.oppF.size() < oppList.size()) )
    {
        ballPredict = wm->ball->predict(t);

        double tToVMax;
        Vector2D center;
        double radius;
        for ( int i = 0; i < ourList.count(); i++ )
        {
            if ( ourCalced[i] )
                continue;
            center = wm->our[ourList[i]]->pos + wm->our[ourList[i]]->vel * t;
            tToVMax = ( robotMaxVel - wm->our[ourList[i]]->vel.length()) / robotMAxAcc;
            radius = 0;
            if ( tToVMax > t)
                radius = 0.5*robotMAxAcc*t*t + wm->our[ourList[i]]->vel.length() * t;
            else if ( tToVMax > 0 && wm->our[ourList[i]]->vel.length() < robotMaxVel)
            {
                radius = 0.5*robotMAxAcc*tToVMax*tToVMax + wm->our[ourList[i]]->vel.length()*tToVMax;
                radius += ( t - tToVMax ) * robotMaxVel;
            }
            else
                radius = t * wm->our[ourList[i]]->vel.length();
            radius += CRobot::robot_radius_old;
            Circle2D cir = Circle2D( center, radius);
            Vector2D s0,s2;
            if( cir.contains(ballPredict) || cir.intersection(Segment2D( wm->ball->pos, ballPredict), &s0, &s2) )
            {
                result.ourF.append(pair<double,int>(t , ourList[i]));
                ourCalced[i] = true;
                if( result.catch_time > t ){
                    result.catch_time = t;
                    result.isFastestOurs = true;
                }
            }
            //            draw( cir, 0 , 360, "red");
        }
        for ( int i = 0; i < oppList.count(); i++ )
        {
            if ( oppCalced[i] )
                continue;
            center = wm->opp[oppList[i]]->pos + wm->opp[oppList[i]]->vel * t;
            tToVMax = ( robotMaxVel - wm->opp[oppList[i]]->vel.length()) / robotMAxAcc;
            radius = 0;
            if ( tToVMax > t)
                radius = 0.5*robotMAxAcc*t*t + wm->opp[oppList[i]]->vel.length() * t;
            else if ( tToVMax > 0 && wm->opp[oppList[i]]->vel.length() < robotMaxVel)
            {
                radius = 0.5*robotMAxAcc*tToVMax*tToVMax + wm->opp[oppList[i]]->vel.length()*tToVMax;
                radius += ( t - tToVMax ) * robotMaxVel;
            }
            else
                radius = t * wm->opp[oppList[i]]->vel.length();
            radius += CRobot::robot_radius_old;
            Circle2D cir = Circle2D( center, radius) ;
            Vector2D s0,s2;
            if( cir.contains(ballPredict) || cir.intersection(Segment2D( wm->ball->pos, ballPredict), &s0, &s2))
            {
                result.oppF.append(pair<double,int>(t , oppList[i]));
                oppCalced[i] = true;
                if( result.catch_time > t ){
                    result.catch_time = t;
                    result.isFastestOurs = false;
                }
            }
            //            draw( cir, 0 , 360, "blue");
        }
        t += timeStep;
    }
    qSort(result.ourF.begin() , result.ourF.end());
    qSort(result.oppF.begin() , result.oppF.end());
    if( result.catch_time > 10 )
        result.catch_time = 0;
    return result;
}

FastestToBall CKnowledge::findFastestToBall(QList<int> ourList, QList<int> oppList)
{
    /////Extracted from DefensePlan
    /////By Pooria
    /////
    FastestToBall f;
    double time = 0.f;
    f.catch_time = 1000;
    while (true) {
        Vector2D ballPos = wm->ball->predict(min(time, f.catch_time));
        //            draw(ballPos, 1, "red");
        if (wm->ball->vel.length() < 0.05)
            ballPos = wm->ball->pos;
        double rad = time * 0.6 + 0.2;
        if (f.ourFastest == -1)
            for (int i = 0; i < ourList.count(); i++) {
                Vector2D playerPos = wm->our[ourList[i]]->pos;
                if (playerPos.dist(ballPos) < rad) {
                    f.ourFastest = ourList[i];
                    f.ourFastestTime = time;
                    if (time < f.catch_time)
                        f.catch_time = time;
                    break;
                }
            }

        if (f.oppFastest == -1)
            for (int i = 0; i < oppList.count(); i++) {
                Vector2D playerPos = wm->opp[oppList[i]]->pos;
                if (playerPos.dist(ballPos) < rad) {
                    f.oppFastest = oppList[i];
                    f.oppFastestTime = time;
                    if (time < f.catch_time)
                        f.catch_time = time;
                    break;
                }
            }

        if ((f.ourFastest > -1 || ourList.count() == 0) && (f.oppFastest > -1 || oppList.count() == 0)) {
            //				LOG("FFFF: ", f.ourFastest);
            break;
        }
        time += 0.1;
        if (time > 20) {
            if (wm->ball->vel.length() > 0.2) {
                Line2D line(wm->ball->pos, wm->ball->pos + wm->ball->vel.norm());
                if (f.ourFastest == -1 and ourList.count() > 0) {
                    float min = 99999;
                    for (int i = 0; i < ourList.count(); i++) {
                        Vector2D playerPos = wm->our[ourList[i]]->pos;
                        float dist = line.dist(playerPos);
                        if (dist < min) {
                            f.ourFastest = ourList[i];
                            min = dist;
                        }
                    }
                    f.ourFastestTime = time;
                    if (time < f.catch_time)
                        f.catch_time = time;
                }
                if (f.oppFastest == -1 and oppList.count() > 0) {
                    float min = 99999;
                    for (int i = 0; i < oppList.count(); i++) {
                        Vector2D playerPos = wm->opp[oppList[i]]->pos;
                        float dist = line.dist(playerPos);
                        if (dist < min) {
                            f.oppFastest = oppList[i];
                            min = dist;
                        }
                    }
                    f.oppFastestTime = time;
                    if (time < f.catch_time)
                        f.catch_time = time;
                }
            }
            break;
        }
    }
    f.catch_time = min(time, f.catch_time);
    return f;
}



