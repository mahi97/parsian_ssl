//
// Created by parsian-ai on 10/13/17.
//

#include <parsian_world_model/wm/halfworld.h>


CHalfWorld::CHalfWorld()
{
    c = new CVisionBelief();
    currentFrame = 0;
    playmakerID = -1;
}

bool dbg = false;

//updates p0 by p
void CHalfWorld::track(QList<CRawObject>&p0, QList<CRawObject>&p)
{
    for (int i=0;i<p0.count();i++)
    {
        p0[i].updated = false;
    }
    //p0 => old data
    //p  => new data
    QList<QList<int> > subs;
    QList<int> u;
    int flag = 0;
    if (p0.count() == 0)
    {
        for (int k=0;k<p.count();k++)
        {
            p0.append(p[k]);
            p0.back().updated = true;
        }
        return;
    }
    if (p.count() > p0.count()) //more objects this frame
    {
        for (int k=0;k<p.count();k++)
        {
            u.append(k);
        }
        subs = generateSubsets(u, p0.count());
        flag = 1;
    }
    else //less or equal objects this frame
    {
        for (int k=0;k<p0.count();k++)
        {
            u.append(k);
        }
        subs = generateSubsets(u, p.count());
        flag = 2;
    }
    double bestDist = 0.0;
    QList<int> bestComb;
    bool firsttime = true;
    for (int k=0;k<subs.count();k++)
    {
        QList<QList<int> > comb = generateCombinations(subs[k]);
        for (int i=0;i<comb.count();i++)
        {
            double d = 0;
            if (flag==1)
            {
                for (int j=0;j<p0.count();j++)
                    d += (p0[j].pos - p[comb[i][j]].pos).r2();
            }
            else {
                for (int j=0;j<p.count();j++)
                    d += (p[j].pos - p0[comb[i][j]].pos).r2();
            }
            if ((firsttime) || (d<bestDist))
            {
                bestDist = d;
                bestComb = comb[i];
                firsttime = false;
            }
        }
    }
    //debug(QString("subs cnt=%1;p0=%2;p=%3;dist=%4").arg(subs.count()).arg(p0.count()).arg(p.count()).arg(bestDist), D_ALI);
//    QString ss;
//    ss = QString("l=");

    if (!firsttime)//there is a combination
    {
        if (flag == 1)
        {
            auto *updated = new bool [p.count()];
            for (int k=0;k<p.count();k++) updated[k] = false;
            for (int k=0;k<bestComb.count();k++)
            {
                //if ((p[bestComb[k]].pos - p0[k].pos).length() < 0.5)
                {
                    p0[k] = p[bestComb[k]];
                    p0[k].updated = true;
                    updated[bestComb[k]] = true;
                }
            }
            for (int k=0;k<p.count();k++)
            {
                if (! updated[k])
                {
                    p0.append(p[k]);
                    p0.back().updated = true;
                }
            }
            delete [] updated;
        }
        else {
            for (int k=0;k<bestComb.count();k++)
            {
                p0[bestComb[k]] = p[k];
                p0[bestComb[k]].updated = true;
            }
        }
//        if (dbg)
//            qDebug() << "naridi";
    }
    else {
//        if (dbg)
//            qDebug() << "ridi: subs= " << subs.count() << "p0.count=" << p0.count() << "p.count=" << p.count() << " flag=" << flag;
    }
}
void CHalfWorld::vanishOutOfSights()
{
    if (ball.count()>0)
    {
        if (ball[0]->inSight<=0)
        {
            ball[0]->inSight = 0.5;
            if (ball.count()>1)
            {
                for (int i=1;i<ball.count();i++)
                {
                    if (ball[i]->inSight > 0.0)
                    {
                        ball.swap(0,i);
                        break;
                    }
                }
            }
        }
    }
    for (int i=1;i<ball.count();i++)
    {
        if (ball[i]->inSight<=0.0)
        {
            delete ball[i];
            ball.removeAt(i);
            i--;
        }
    }
}

void CHalfWorld::update(QList<CBall *>& ball, CVisionBelief* v)
{

    if (ball.count() == 0)
    {
        for (int i=0;i<v->ball.count();i++)
        {
            ball.append(new CBall(false));
            ball.last()->update(v->ball[i]);
        }
    }
    else {
        QList<bool> flag;
        for (int i=0;i<ball.count();i++)
            flag.append(false);
        for (int i=0;i<v->ball.count();i++)
        {
            double min_d = 1e5;
            int k = -1;
            for (int j=0;j<ball.count();j++)
            {
                if (flag[j]) continue;
                double d = (ball[j]->pos + ball[j]->vel * getFramePeriod() - v->ball[i].pos).length();
                if (d < min_d)
                {
                    min_d = d;
                    k = j;
                }
            }
            if (k != -1)
            {
                if (min_d > ball[k]->vel.length() * getFramePeriod()*2.0 + 1.0)
                {
                    k = -1;
                }
            }
            if (k != -1)
            {
                flag[k] = true;
                ball[k]->update(v->ball[i]);
            }
            else {
                flag.append(true);
                ball.append(new CBall(false));
                ball.last()->update(v->ball[i]);
            }
        }
        for (int i=0;i<flag.count();i++)
        {
            if (!flag[i])
            {
                ball[i]->update(CRawObject(0, ball[i]->pos, 0.0, -1, 0.0, NULL, v->cam_id, v->time));
            }
        }
    }
    /////WatchDog
    if (ball.count() > 0)
    {
        double minVel = 0.6;
        if (ball[0]->vel.length() < minVel)
            for (int k=1;k<ball.count();k++)
            {
                if (ball[k]->vel.length() >= minVel)
                {
                    ball.swap(k, 0);
                    break;
                }
            }
    }
}

void CHalfWorld::update(QList<CRobot*>& robot, CVisionBelief* v, QList<CRawObject>& robot0, int id, bool our)
{
    if (robot.count() == 0)
    {
        for (int i=0;i<robot0.count();i++)
        {
            robot.append(new CRobot(id, our, false));
            robot.last()->update(robot0[i]);
        }
    }
    else {
        QList<bool> flag;
        for (int i=0;i<robot.count();i++)
            flag.append(false);
        for (int i=0;i<robot0.count();i++)
        {
            double min_d = 1e5;
            int k = -1;
            for (int j=0;j<robot.count();j++)
            {
                if (flag[j]) continue;
                double d = (robot[j]->pos + robot[j]->vel * getFramePeriod() - robot0[i].pos).length();
                if (d < min_d)
                {
                    min_d = d;
                    k = j;
                }
            }
            if (k != -1)
            {
                if (min_d > robot[k]->vel.length() * getFramePeriod()*2.0 + 1.0)
                {
                    k = -1;
                }
            }
            if (k != -1)
            {
                flag[k] = true;
                robot[k]->update(robot0[i]);
            }
            else {
                flag.append(true);
                robot.append(new CRobot(id, our, false));
                robot.last()->update(robot0[i]);
            }
        }
        for (int i=0;i<flag.count();i++)
        {
            if (!flag[i])
            {
                robot[i]->update(CRawObject(0, robot[i]->pos, robot[i]->dir.th().degree(), -1, 0.0, NULL, v->cam_id, v->time));
            }
        }
    }
}




void CHalfWorld::update(CVisionBelief *v)
{

    belief = *v;
    QList<CRawObject> p0;
    update(ball, v);

    for (int j=0;j<_MAX_NUM_PLAYERS;j++)
    {
        if (ourTeam[j].count() == 0)
        {
            ourTeam[j].append(new CRobot(j,true,false));
        }
        else {
            if (v->ourTeam[j].count()>0)
                ourTeam[j][0]->update(v->ourTeam[j][0]);
            else
            if (ourTeam[j][0]->inSight > 0)
            {
                ourTeam[j][0]->update(CRawObject(0, ourTeam[j][0]->pos, ourTeam[j][0]->dir.th().degree(), -1, 0.0, NULL, v->cam_id, v->time));
            }
        }
    }
    for (int j=0;j<_MAX_NUM_PLAYERS;j++)
    {
        if (oppTeam[j].count() == 0)
        {
            oppTeam[j].append(new CRobot(j,true,false));
        }
        else {
            if (v->oppTeam[j].count()>0)
                oppTeam[j][0]->update(v->oppTeam[j][0]);
            else
            if (oppTeam[j][0]->inSight > 0)
            {
                oppTeam[j][0]->update(CRawObject(0, oppTeam[j][0]->pos, oppTeam[j][0]->dir.th().degree(), -1, 0.0, NULL, v->cam_id, v->time));
            }
        }
    }


/*    for (int j=0;j<_MAX_NUM_PLAYERS;j++)
    {
        p0.clear();
        for (int i=0;i<min(ourTeam[j].count(), MAX_OBJECTS);i++)
        {
            p0.append(CRawObject(0, ourTeam[j][i]->pos, ourTeam[j][i]->dir.th().degree(), j, ourTeam[j][i]->inSight));
        }
        track(p0, v->ourTeam[j]);
        for (int i=0;i<p0.count();i++)
        {
            if (p0[i].updated)
            {
                if (i>=ourTeam[j].count())
                {
                    ourTeam[j].append(new CRobot(j,true,false));
                }
                p0[i].time = v->time;
                //if (ourTeam[j][i]->cam_id==v->cam_id)
                    ourTeam[j][i]->update(p0[i]);
                ourTeam[j][i]->lastFrameUpdated = currentFrame;
                ourTeam[j][i]->cam_id = v->cam_id;
            }
            else {
//                if (ourTeam[j][i]->cam_id==v->cam_id)
                    ourTeam[j][i]->update(CRawObject(0, ourTeam[j][i]->pos, ourTeam[j][i]->dir.th().degree(), j, 0.0, NULL, v->cam_id, v->time));
            }
        }
    }
    for (int j=0;j<_MAX_NUM_PLAYERS;j++)
    {
        p0.clear();
        for (int i=0;i<min(oppTeam[j].count(), MAX_OBJECTS);i++)
        {
            p0.append(CRawObject(0, oppTeam[j][i]->pos, oppTeam[j][i]->dir.th().degree(), j, oppTeam[j][i]->inSight));
        }
        track(p0, v->oppTeam[j]);
        for (int i=0;i<p0.count();i++)
        {
            if (p0[i].updated)
            {
                if (i>=oppTeam[j].count())
                {
                    oppTeam[j].append(new CRobot(j,false,false));
                }
                p0[i].time = v->time;
              //  if (oppTeam[j][i]->cam_id==v->cam_id)
                    oppTeam[j][i]->update(p0[i]);
                oppTeam[j][i]->lastFrameUpdated = currentFrame;
                oppTeam[j][i]->cam_id = v->cam_id;
            }
            else
            {
//                if (oppTeam[j][i]->cam_id==v->cam_id)
                    oppTeam[j][i]->update(CRawObject(0, oppTeam[j][i]->pos, oppTeam[j][i]->dir.th().degree(), j, 0.0, NULL, v->cam_id, v->time));
            }
        }
    }*/

}

void CHalfWorld::update(CHalfWorld *w)
{
    for (int k=0;k<ball.count();k++) delete ball[k];
    ball.clear();
    for (int k=0;k<w->ball.count();k++)
    {
        ball.append(new CBall(true));
        ball.back()->update(w->ball[k]);
    }
    for (int j=0;j<_MAX_NUM_PLAYERS;j++)
    {
        for (int k=0;k<ourTeam[j].count();k++) delete ourTeam[j][k];
        ourTeam[j].clear();
        for (int k=0;k<w->ourTeam[j].count();k++)
        {
            ourTeam[j].append(new CRobot(j, true, true));
            ourTeam[j].back()->update(w->ourTeam[j][k]);
        }
    }
    for (int j=0;j<_MAX_NUM_PLAYERS;j++)
    {
        for (int k=0;k<oppTeam[j].count();k++) delete oppTeam[j][k];
        oppTeam[j].clear();
        for (int k=0;k<w->oppTeam[j].count();k++)
        {
            oppTeam[j].append(new CRobot(j, false, true));
            oppTeam[j].back()->update(w->oppTeam[j][k]);
        }
    }
}

void CHalfWorld::selectBall(Vector2D pos)
{
    double minDist = 0.0;
    int bestBall = -1;
    for (int i=0;i<ball.count();i++)
    {
        double d = (ball[i]->pos-pos).length();
        if (d<minDist || bestBall==-1)
        {
            bestBall = i;
            minDist = d;
        }
    }
    if (bestBall != -1 && ball.count()>0)
    {
        ball.swap(0, bestBall);
    }
}

