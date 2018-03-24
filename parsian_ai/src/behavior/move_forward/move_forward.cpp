#include <behavior/move_forward/move_forward.h>

BehaviorMove_Forward::BehaviorMove_Forward()
: Behavior("Move_Forward", "describe me") {

}

BehaviorMove_Forward::~BehaviorMove_Forward() {

}

int BehaviorMove_Forward::execute() {
    return 0;
}

double BehaviorMove_Forward::eval() {
    //_behav = nullptr;
   Vector2D oppgoal{OPP_GOAL_X, OPP_GOAL_Y};
   double default_dist{oppgoal.dist(Vector2D(wm->ball->pos.x +  wm->ball->vel.x, wm->ball->pos.y +  wm->ball->vel.y)) / 2.0};
   //ROS_INFO_STREAM("kian: " << default_dist);
   QList<Segment2D> obstacles;
   drawer->draw(Circle2D(Vector2D(wm->ball->pos.x +  wm->ball->vel.x, wm->ball->pos.y +  wm->ball->vel.y), default_dist), QColor(100, 255, 50), false);

   for (int i{}; i < wm->opp.activeAgentsCount(); i++) {
       if (wm->opp.active(i)->pos.x + wm->opp.active(i)->vel.x > wm->ball->pos.x + wm->ball->vel.x) {
           if (Vector2D(wm->opp.active(i)->pos.x + wm->opp.active(i)->vel.x, wm->opp.active(i)->pos.y + wm->opp.active(i)->vel.y).dist(Vector2D(wm->ball->pos.x +  wm->ball->vel.x, wm->ball->pos.y +  wm->ball->vel.y)) < default_dist) {
               Segment2D temp{Vector2D{wm->opp.active(i)->pos.x + wm->opp.active(i)->vel.x, wm->opp.active(i)->pos.y + wm->opp.active(i)->vel.y}, Vector2D{wm->ball->pos.x + wm->ball->vel.x, wm->ball->pos.y + wm->ball->vel.y}};
               drawer->draw(temp, QColor(50, 55, 155));
               obstacles.push_back(temp);
           }
       }
   }
   Segment2D left{Vector2D{wm->ball->pos.x + wm->ball->vel.x, wm->ball->pos.y + wm->ball->vel.y + default_dist}, Vector2D{wm->ball->pos.x + wm->ball->vel.x, wm->ball->pos.y + wm->ball->vel.y}};
   Segment2D right{Vector2D{wm->ball->pos.x + wm->ball->vel.x, wm->ball->pos.y + wm->ball->vel.y - default_dist}, Vector2D{wm->ball->pos.x + wm->ball->vel.x, wm->ball->pos.y + wm->ball->vel.y}};
   obstacles.push_back(left);
   obstacles.push_back(right);
   drawer->draw(left, QColor(50, 55, 155));
   drawer->draw(right, QColor(50, 55, 155));
   sortobstacles(obstacles);
//   for(int i{}; i < obstacles.size(); i++)
//       ROS_INFO_STREAM("kian::: "<<obstacles[i].a().y);
   QList<double> angles;
    for(int i{}; i < obstacles.size() - 1; i++)
    {
        angles.push_back(angleOfTwoSegment(obstacles[i], obstacles[i + 1]));
    }
   //ROS_INFO_STREAM("kian: " << angleOfTwoSegment(Segment2D{Vector2D{1, 0}, Vector2D{0, 0}}, Segment2D{Vector2D{0,1}, Vector2D{0,0}}));
    QList<QPair<Vector2D, double>> result;
    double angsum{};
    for(int i{}; i<angles.size(); i++)
    {
        QPair<Vector2D, double> tmp;
        if(i != 0)
            angsum += angles[i - 1];
        double ang{angsum + angles[i]/2};
        tmp.first = Vector2D{10000, tan(3.14-ang)*10000};
        tmp.second = angles[i];
        result.push_back(tmp);
    }
    for(int i{}; i <result.size(); i++)
        drawer->draw(Segment2D{Vector2D{wm->ball->pos.x + wm->ball->vel.x, wm->ball->pos.y + wm->ball->vel.y}, result[i].first}, QColor(50, 10, 50));
   return 0.0;

}


void BehaviorMove_Forward::swap(Segment2D *xp, Segment2D *yp)
{
    Segment2D temp = *xp;
    *xp = *yp;
    *yp = temp;
}

void BehaviorMove_Forward::sortobstacles(QList<Segment2D> &obstacles)
{
   int j{}, n{obstacles.size()};
   for (int i{}; i < n-1; i++)
       for (int j{}; j < n-i-1; j++)
           if (obstacles[j].a().y > obstacles[j+1].a().y)
              swap(&obstacles[j], &obstacles[j+1]);
}


double BehaviorMove_Forward::angleOfTwoSegment(const Segment2D &xp, const Segment2D &yp)
{
    double theta1 = std::atan2(xp.a().y-xp.b().y,xp.a().x-xp.b().x);
    double theta2 = std::atan2(yp.a().y-yp.b().y,yp.a().x-yp.b().x);
    double diff = fabs(theta1-theta2);
    return min(diff,fabs(180-diff));
}

double BehaviorMove_Forward::findmax(const QList<double> &list)
{
    double max{-100000};
    for(int i{}; i < list.size(); i++)
    {
        if(list[i] > max)
            max = list[i];
    }
    return max;
}
