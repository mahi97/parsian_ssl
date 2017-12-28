#include <planner/planner.h>
///////////////////////////////
////////IMPORTANT TODOs////////
///////////////////////////////


//global functions
void quicksort( state *a[] , int lo , int hi , int k)
{
    int i = lo , j = hi;
    double x = a[(lo + hi)/2]->pos[k];
    do
    {
        while ( a[i]->pos[k] < x ) i++;
        while ( a[j]->pos[k] > x ) j--;
        if ( i <= j )
        {
            swap( a[i] , a[j] );
            i++; j--;
        }
    }while ( i < j );


    if ( lo < j ) quicksort(a , lo , j , k);
    if ( i < hi ) quicksort(a , i , hi , k);
}

//====================================================================//
//    C2DTree class implementation
//====================================================================//

C2DTree::C2DTree(){
    head = first = nullptr;
    cnt = 0;
    mod = 5;
}

C2DTree::~C2DTree(){
    removeAll();
}

int C2DTree::size(){
    return cnt;
}

void C2DTree::removeAll(){
    cnt = 0;
    mod = 5;
    if( head == nullptr )
        return;
    removeBranch(head);
    head = first = nullptr;
}

void C2DTree::removeBranch(state *node){
    if( node ){
        removeBranch(node->left);
        removeBranch(node->right);
        delete node;
    }
}

void C2DTree::add(state * newState){
    allNodes[cnt++] = newState;

    //	if( head == nullptr )
    //		head = newState;
    //	return;

    if( cnt % 33 == 0 )
        mod++;

    if( cnt % mod ){
        addNode(newState , head , 0);
    }
    else{
        int hi = cnt-1;
        int lo = 0;
        head = makeBalanced(allNodes , 0 , lo , hi);
    }
}


void C2DTree::addNode( state * const &newState , state *&node , int depth){
    if( node == nullptr){
        node = newState;
    }
    else{
        k = depth % 2;
        if( newState->pos[k] < node->pos[k] )
            addNode(newState , node->left , depth+1);
        else
            addNode(newState , node->right , depth+1);
    }
}

state* C2DTree::makeBalanced(state *all[] , int depth , int lo , int hi){
    if( lo > hi )
        return nullptr;
    k = depth%2;
    if( lo < hi )
        quicksort(all , lo , hi , k);
    int mid = (lo+hi)/2;

    all[mid]->left = makeBalanced(all , depth+1 , lo , mid-1);
    all[mid]->right = makeBalanced(all , depth+1 , mid+1 , hi);
    return all[mid];
}


state *C2DTree::findNearest(Vector2D point){
    //	double mn = 10000;
    //	state *node;
    //	for( int i=0 ; i<cnt ; i++)
    //		if( allNodes[i]->pos.dist(point) < mn ){
    //			mn = allNodes[i]->pos.dist(point);
    //			node = allNodes[i];
    //		}
    //	return node;

    if( head ){
        return findNearestNode(point , head , 0);
    }
    else
        return nullptr;
}


state *C2DTree::findNearestNode(Vector2D &point , state * const &node , int depth){
    k = depth % 2;
    state *best , *otherSideBest;
    if( point[k] < node->pos[k] ){
        if( node->left ){
            best = findNearestNode(point , node->left , depth+1);
            if( node->pos.dist(point) < best->pos.dist(point) )
                best = node;
            if( node->right && fabs(node->pos[k] - point[k]) < point.dist(best->pos) ){
                otherSideBest = findNearestNode(point , node->right , depth+1);
                if( point.dist(otherSideBest->pos) < point.dist(best->pos) )
                    best = otherSideBest;
            }
        }
        else{
            best = node;
            if( node->right && fabs(node->pos[k] - point[k]) < point.dist(best->pos)  ){
                otherSideBest = findNearestNode(point , node->right , depth+1);
                if( point.dist(otherSideBest->pos) < point.dist(best->pos) )
                    best = otherSideBest;
            }
        }
    }
    else{
        if( node->right ){
            best = findNearestNode(point , node->right , depth+1);
            if( node->pos.dist(point) < best->pos.dist(point) )
                best = node;
            if( node->left && fabs(node->pos[k] - point[k]) < point.dist(best->pos)  ){
                otherSideBest = findNearestNode(point , node->left , depth+1);
                if( point.dist(otherSideBest->pos) < point.dist(best->pos) )
                    best = otherSideBest;
            }
        }
        else{
            best = node;
            if( node->left && fabs(node->pos[k] - point[k]) < point.dist(best->pos)  ){
                otherSideBest = findNearestNode(point , node->left , depth+1);
                if( point.dist(otherSideBest->pos) < point.dist(best->pos) )
                    best = otherSideBest;
            }
        }
    }
    return best;
}

void C2DTree::drawBranch(state *first , state* second , QColor color){
    if(second == nullptr)
        return;
    drawer->draw(Segment2D(first->pos , second->pos) , color);
    drawBranch(second , second->parent , color);
}



//====================================================================//
//    CPlanner class implementation
//====================================================================//

CPlanner::CPlanner(int _ID)
{
    obst.clear();
    threshold = conf->Target_Distance_Threshold;
    wayPointProb = conf->Waypoint_Catch_Probablity;
    goalProb = conf->Goal_Probablity;
    stepSize = conf->Extend_Step;
    goal.invalidate();
    counter=0;
    flag = false;
    pointStep = 0.04;
    this->ID = _ID;
    qRegisterMetaType< vector<Vector2D> >("vector<Vector2D>");
    qRegisterMetaType< Vector2D >("Vector2D");
    qRegisterMetaType< QList<int> >("QList<int>");
}

CPlanner::~CPlanner() = default;

Vector2D CPlanner::chooseTarget( Vector2D &GOAL ){
    double p = drand48();
    unsigned long idx;
    if( p<=goalProb )
        return GOAL;
    else if(! result.empty() && goalProb < p && p < wayPointProb+goalProb ){
        idx = rand() % result.size();
        return result[idx];
    }
    else{
        // random state is in a big field (1 meter larger from each side)
        Vector2D a;
        a.assign((2.0*drand48()-1.0)*(field._FIELD_WIDTH+2)/2.0 , (2.0*drand48()-1.0)*(field._FIELD_HEIGHT+2)/2.0);
        return a;
    }
}

state *CPlanner::extendTree( state *&NEAREST , Vector2D &GOAL , Vector2D &TARGET , vector<Vector2D> &res){
    Vector2D temp;
    if( NEAREST->pos.dist(GOAL) < stepSize )
        temp = GOAL;
    else{
        temp = NEAREST->pos + (TARGET - NEAREST->pos).norm()*stepSize;
    }

    if( obst.check(temp , NEAREST->pos) ){
        //		if( NEAREST->parent == nullptr || fabs(Vector2D::angleBetween(NEAREST->pos - NEAREST->parent->pos , temp - NEAREST->pos ).degree()) < 20 )
        return new state(temp , NEAREST , res);
    }
    else if( NEAREST->depth < 4 &&  obst.check(temp)){
        //		if( NEAREST->parent == nullptr || fabs(Vector2D::angleBetween(NEAREST->pos - NEAREST->parent->pos , temp - NEAREST->pos ).degree()) < 20 )
        return new state(temp , NEAREST , res);
    }
    return nullptr;
}

bool CPlanner::validState(state *&node){
    return node && node->pos.x >= (-(field._FIELD_WIDTH/2 + 1)) && node->pos.x <= ((field._FIELD_WIDTH/2 + 1)) \
            && node->pos.y >= (-(field._FIELD_HEIGHT/2 + 1)) && node->pos.y <= ((field._FIELD_HEIGHT/2 + 1));
}


void CPlanner::checkAgent(){
    //// check whether start-point is inside an obstacle or not!
    QList<int> obsid;
    if( !obst.check(Rgoal , obsid))
    {
        for( int i = 0; i < obsid.count(); i++)
        {
            double lastRad = obst.obs[obsid.at(i)].rad.x;
            double newRad = obst.obs[obsid.at(i)].pos.dist(Rgoal) - 0.03;
            obst.obs[obsid.at(i)].rad.x = lastRad<newRad?lastRad:newRad;
        }
    }

    //// check whether target-point is inside an obstacle or not!
    obsid.clear();
    if( !obst.check(goal , obsid))
    {
        for( int i = 0; i < obsid.count(); i++)
        {
            //// except penalty area (((( DOUBLE DEFENDE ))))
            if( obst.obs[obsid.at(i)].pos.dist(Vector2D(-3.20 , 0)) < 0.1 && fabs(obst.obs[obsid.at(i)].rad.x-1.1)<0.2  )
                continue;

            //// except center circle
            if( obst.obs[obsid.at(i)].pos.dist(Vector2D(0 , 0)) < 0.1 && fabs(obst.obs[obsid.at(i)].rad.x-0.5)<0.1 )
                continue;

            double lastRad = obst.obs[obsid.at(i)].rad.x;
            double newRad = obst.obs[obsid.at(i)].pos.dist(goal) - 0.07;
            obst.obs[obsid.at(i)].rad.x = lastRad<newRad?lastRad:newRad;
        }
    }
}


void CPlanner::runPlanner(){
    checkAgent();

    if( nodes.head == nullptr ){
        result.clear();
        Rresult.clear();
        averageDir.assign(0,0);
        return;
    }

    state *nearestToGoal=nullptr , *nearest=nullptr;
    Vector2D target;
    state *RnearestToGoal=nullptr , *Rnearest=nullptr;
    Vector2D Rtarget;
    state *middleNode=nullptr;
    double RnearestDist , nearestDist;
    int nnn = 0 , Rnnn = 0;
    bool marginFlag = true , RmarginFlag = true;


    obst.obsMargin = Robot::robot_radius_new+0.03;

    nearestToGoal = nodes.findNearest(goal);
    nearestDist = nearestToGoal->pos.dist(goal);
    while( nearestDist + EPSILON > threshold
           && nodes.size() < _PLANNER_EXTEND_POINT_LIMIT
           && nnn<_PLANNER_EXTEND_MAX_ATTEMPT){

        target = chooseTarget(goal);

        nearest = nodes.findNearest(target);

        state *newNode = extendTree(nearest , goal , target , result);

        if( validState(newNode) && nodes.findNearest(newNode->pos)->pos.dist(newNode->pos) > threshold ){
            marginFlag = true;
            obst.obsMargin = Robot::robot_radius_new+0.03;
            nnn = 0;
            nearest->next = newNode;
            nodes.add(newNode);

            if( nearestDist + EPSILON > newNode->pos.dist(goal) ){
                nearestToGoal = newNode;
                nearestDist = nearestToGoal->pos.dist(goal);
            }
        }
        else{
            delete newNode;
            nnn++;
        }

        if( nnn == _PLANNER_EXTEND_MAX_ATTEMPT && marginFlag ){
            marginFlag = false;
            obst.obsMargin = 0.05;
            nnn = 0;
        }
    }

    obst.obsMargin = Robot::robot_radius_new+0.03;

    if( Rnodes.head && nearestDist + EPSILON > threshold ){
        RnearestToGoal = Rnodes.findNearest(Rgoal);
        RnearestDist = RnearestToGoal->pos.dist(Rgoal);
        while( RnearestDist + EPSILON > threshold && Rnodes.size() < _PLANNER_EXTEND_POINT_LIMIT && Rnnn<_PLANNER_EXTEND_MAX_ATTEMPT){

            Rtarget = chooseTarget(Rgoal);

            Rnearest = Rnodes.findNearest(Rtarget);

            state *newNode = extendTree(Rnearest , Rgoal , Rtarget , Rresult);

            if( validState(newNode) && Rnodes.findNearest(newNode->pos)->pos.dist(newNode->pos) > threshold){
                RmarginFlag = true;
                obst.obsMargin = Robot::robot_radius_new+0.03;
                Rnnn = 0;
                Rnearest->next = newNode;
                Rnodes.add(newNode);

                if( RnearestDist + EPSILON > newNode->pos.dist(Rgoal) ){
                    RnearestToGoal = newNode;
                    RnearestDist = RnearestToGoal->pos.dist(Rgoal);
                }
            }
            else{
                delete newNode;
                Rnnn++;
            }
            if( Rnnn == _PLANNER_EXTEND_MAX_ATTEMPT && RmarginFlag ){
                RmarginFlag = false;
                obst.obsMargin = 0.05;
                Rnnn = 0;
            }
        }
    }

    obst.obsMargin = Robot::robot_radius_new+0.03;

    vector <Vector2D> temp;
    state *lll = nullptr;

    if( nearestToGoal->pos.dist(goal) + EPSILON < threshold ){
        temp.clear();
        while( nearestToGoal ){
            temp.push_back(nearestToGoal->pos);
            nearestToGoal = nearestToGoal->parent;
        }
    }
    else if( RnearestToGoal && RnearestToGoal->pos.dist(Rgoal) + EPSILON < threshold ){
        lll = nullptr;
        while( RnearestToGoal ){
            nearestToGoal = new state(RnearestToGoal->pos , lll , result);
            Rnodes.add(nearestToGoal);
            lll = nearestToGoal;
            RnearestToGoal = RnearestToGoal->parent;
        }

        temp.clear();
        while( nearestToGoal ){
            temp.push_back(nearestToGoal->pos);
            nearestToGoal = nearestToGoal->parent;
        }
        /*if( CProfiler::getTime()*1000.0 - drawTimer*1000.0 > 100.0 )
   draw("Target" , Vector2D(0.3,-2.2) , "red");*/
    }
    else{
        bool found = false;
        state *copyNearest = nearestToGoal;
        double mn = 1000;
        state *one = nullptr, *two = nullptr;
        for( int i=0 ; i<nodes.size() ; i++ ){
            for( int j=0 ; j<Rnodes.size() ; j++ ){
                middleNode = Rnodes.allNodes[j];
                nearestToGoal = nodes.allNodes[i];
                if( obst.check(middleNode->pos , nearestToGoal->pos) && \
                    ((result.size() && middleNode->dist + nearestToGoal->dist < mn) || (result.size() == 0 && middleNode->dist + nearestToGoal->dist + middleNode->pos.dist(nearestToGoal->pos) < mn)) ){
                    if( result.size() )
                        mn = (middleNode->dist + nearestToGoal->dist);
                    else
                        mn =  middleNode->dist + nearestToGoal->dist + middleNode->pos.dist(nearestToGoal->pos);
                    one = middleNode;
                    two = nearestToGoal;

                    found = true;
                }
            }
        }
        if( std::fabs(mn - 1000.0) > EPSILON ){
            auto num = static_cast<int>((one->pos.dist(two->pos)) / stepSize);
            for( int i=1 ; i<num ; i++ ){
                two = new state(two->pos + (one->pos-two->pos).norm()*stepSize , two , result);
                nodes.add(two);
            }
            while( one ){
                two = new state(one->pos , two , result);
                nodes.add(two);
                one = one->parent;
            }
            nearestToGoal = two;
        }

        if(! found){
            nearestToGoal = copyNearest;
            flag = true;
        }

        temp.clear();
        while( nearestToGoal ){
            temp.push_back(nearestToGoal->pos);
            nearestToGoal = nearestToGoal->parent;
        }
    }

    reverse(temp.begin() , temp.end());
    result.assign(temp.begin() , temp.end());
    Rresult.assign(result.begin() , result.end());
    reverse(Rresult.begin() , Rresult.end());


    resultModified.clear();
    RresultModified.clear();

    for(int i = static_cast<int>(result.size() - 1); i > 0 ; i --)
    {
        //draw(Segment2D(result[i],result[i-1]),QColor(Qt::red));
    }
    //////////////////////////////////////////////////path smoothing
    if( temp.size() >1)
        resultModified.push_back(std::move(temp.back()));
    else
        resultModified.push_back(target);

    auto compNodeNum = static_cast<int>(temp.size() - 1);
    for( int i=((int)(temp.size()))-1 ; i>1 ; i-- )
    {
        if(i < compNodeNum)
        {
            for(int k = 0 ; k < i ; k ++)
            {
                if(obst.check(temp[i],temp[k]) )
                {
                    resultModified.push_back(temp[k]);
                    //draw(temp[k]);
                    compNodeNum = k;
                    break;
                }
                if((k == i -1)  )
                {
                    resultModified.push_back(temp[k]);
                    //draw(temp[k]);
                    compNodeNum = k;
                }
            }
        }
    }

    if( resultModified.size() == 1)
        resultModified.push_back(temp[0]);

    //////////////////////////////////////////////////////
    RresultModified.assign(resultModified.begin() , resultModified.end());
    reverse(RresultModified.begin() , RresultModified.end());

    // miangin vazn dar, az direction harkat
    Vector2D newDir;
    if( resultModified.size() > 1 )
        newDir = (resultModified[1]-resultModified[0]).norm();
    else
        newDir = (goal - Rgoal).norm();
    int cntcnt = 8;
    int vd = 1;
    if( dirs.count() > cntcnt-1 ){
        averageDir = (averageDir*(cntcnt+(vd-1)) - dirs.at(0) - (vd-1)*dirs.last() + vd*newDir)/(cntcnt+(vd-1));
        dirs.pop_front();
    }
    else{
        if( dirs.count() )
            averageDir = (averageDir*(dirs.count()+(vd-1)) - (vd-1)*dirs.last() + vd*newDir)/(dirs.count()+vd);
        else
            averageDir = newDir;
    }
    dirs.push_back(newDir);

    for( int j=1 ; j<resultModified.size() ; j++ ){
        drawer->draw(Segment2D(resultModified[j-1] , resultModified[j]) , QColor(
                static_cast<int>(255 / (resultModified.size() / (double)j)),
                static_cast<int>(255 / (resultModified.size() / (double)j)),
                static_cast<int>(255 / (resultModified.size() / (double)j))));
//            drawer->draw(result[j]);
    }
    Draw();
    obst.draw();

}

void CPlanner::Draw(){


    for( int i=0 ; i<nodes.size() ; i++ )
        if( nodes.allNodes[i]->parent )
            drawer->draw(Segment2D(nodes.allNodes[i]->pos , nodes.allNodes[i]->parent->pos) , "white");

    for( int i=0 ; i<Rnodes.size() ; i++ )
        if( Rnodes.allNodes[i]->parent )
            drawer->draw(Segment2D(Rnodes.allNodes[i]->pos , Rnodes.allNodes[i]->parent->pos) , "gray");

}

void CPlanner::resetPlanner( Vector2D _goal ){
    if(result.empty() || lastGoal.dist(_goal) > 0.05 || (result[result.size()-1].dist(lastGoal) > 10*threshold) ){
        //		if( result.size() == 0 )
        //			qDebug() << "RESULT.SIZE()";
        //		else if( lastGoal.dist(_goal) > 0.05 ){
        //			qDebug() << lastGoal.x << lastGoal.y << "-------" << _goal.x << _goal.y;
        //			qDebug() << "GOAL";
        //		}
        //		else if( result[result.size()-1].dist(lastGoal) > threshold ){
        //			qDebug() << "THRESHOLD";
        //		}

        result.clear();
        Rresult.clear();
        averageDir.assign(0,0);
        dirs.clear();
        /*draw("NO WAYPOINTS" , Vector2D(-0.2,-2.2) , "red");*/
    }
    else{
        //		for( int i=0 ; i<2 && result.size() ; i++ )
        //			result.erase(result.begin()+0);
        //		for( int i=0 ; i<result.size() ; i++ )
        //			draw(result[i] , 0 , "red");
    }

    nodes.removeAll();
    Rnodes.removeAll();
}


void CPlanner::initPathPlanner(Vector2D _goal,const QList<int>& _ourRelaxList,const QList<int>& _oppRelaxList ,const bool& _avoidPenaltyArea ,const bool& _avoidCenterArea , const double& _ballObstacleRadius ){

    int idx = ID;

    if( wm->our[idx]->pos.dist(_goal) < 0.1 ){
        resultModified.clear();
        resultModified.push_back(wm->our[idx]->pos);
        resultModified.push_back(_goal);
        averageDir = _goal - wm->our[idx]->pos;
        emitPlan(resultModified , averageDir);
        return;

    }

    if(! wm->field->marginedField().contains(_goal) ){
        auto *sol1 = new Vector2D() , *sol2 = new Vector2D();
        wm->field->marginedField().intersection(Segment2D(Vector2D(0,0) , _goal) , sol1 , sol2 );
        if( sol1->valid() ){
            _goal = *sol1;
        }
        else
            _goal = Vector2D(0,0);
    }

    resetPlanner(_goal);
    lastGoal = goal;
    goal = _goal;
    Rgoal = wm->our[idx]->pos;
    avoidPenaltyArea = _avoidPenaltyArea;
    avoidCenterArea = _avoidCenterArea;

    ourRelaxList.clear();
    for (int i : _ourRelaxList)
        ourRelaxList.append(i);
    ourRelaxList.append(ID);


    oppRelaxList.clear();
    for (int i : _oppRelaxList){
        oppRelaxList.append(i);
    }

    ballObstacleRadius = _ballObstacleRadius;
    stepSize     = conf->Extend_Step;
    threshold    = conf->Target_Distance_Threshold;
    wayPointProb = conf->Waypoint_Catch_Probablity;
    if(! result.empty())
        goalProb = conf->Goal_Probablity;
    else
        goalProb = 0.5;

    robotVel = wm->our[idx]->vel;

    nodes.first = new state((wm->our[idx]->pos+wm->our[idx]->vel*0.09) , nullptr , result);
    nodes.add(nodes.first);

    Rnodes.first = new state(_goal , nullptr , Rresult);
    Rnodes.add(Rnodes.first);
    counter++;

    generateObstacleSpace(obst  , ourRelaxList , oppRelaxList , avoidPenaltyArea, avoidCenterArea , ballObstacleRadius,ID,goal);
    runPlanner();
    emitPlan(getResultModified(), getAverageDir());

}

double CPlanner::timeEstimator(Vector2D _pos, Vector2D _vel, Vector2D _dir, Vector2D posT){
    double _x3;
    double acc;
    double dec = conf->DecMax;
    double xSat;
    double veltan= (_vel.x)*cos(_dir.th().radian()) + (_vel.y)*sin(_dir.th().radian());
    double offset = 0.15;
    double velnorm= -1*(_vel.x)*sin(_dir.th().radian()) + (_vel.y)*cos(_dir.th().radian());

    if(_vel.length() < 0.2)
    {
        acc = (conf->AccMaxForward + conf->AccMaxNormal)/2;
    }
    else
    {
        acc =  conf->AccMaxForward*(fabs(veltan)/_vel.length()) + conf->AccMaxNormal*(fabs(velnorm)/_vel.length());
    }

    double vMaxReal = sqrt(((_pos.dist(posT)  + (_vel.length()*_vel.length()/2*acc))*2*acc*dec)/(acc+dec));
    vMaxReal = min(vMaxReal,4);
    double vMax = conf->VelMax;
    vMax = min(vMax,vMaxReal);
    xSat = ((vMax*vMax)-(_vel.length()*_vel.length()))/acc + (vMax*vMax)/dec;
    _x3 = ( -1* _vel.length()*_vel.length()) / (-2 * fabs(conf->DecMax)) ;

    if(_pos.dist(posT) < _x3 ) {
        return std::max(0.0,(_vel.length()/ conf->DecMax - offset) );
    } else if(_vel.length() < (vMax)) {
        if(_pos.dist(posT) < xSat)
        {
            return std::max(0.0,(-1*offset + vMax/dec + (vMax-_vel.length())/acc + (_pos.dist(posT) - ((vMax*vMax/(2*dec)) + ((vMax+_vel.length())*(vMax-_vel.length())/acc))/2)/vMax) );
        } else {
            return std::max(0.0,(vMax/dec + (vMax-_vel.length())/acc - offset));
        }
    } else {
        return std::max(0.0,(vMax/dec + (_pos.dist(posT) - ((vMax*vMax/(2*dec)) ))/vMax - offset) );
    }

}

void CPlanner::createObstacleProb(CObstacles &obs,Vector2D _pos, Vector2D _vel, Vector2D _ang,Vector2D &_center,
                                  double &_rad,Vector2D agentPos,Vector2D agentVel, Vector2D agentGoal, Vector2D agentDir){
    Segment2D obstaclePath;
    Vector2D intersectPoint;
    double timeForObs = 0;
    ///TODO: should read from vartypes
    double maxA = 4;
    double maxObstRad = 1;
    double maxTime = 0.5;
    if(_vel.length() < 0.2)
    {
        _center = _pos;
        _rad =Robot::robot_radius_new;
    }
    else
    {
        obstaclePath.assign(_pos,_pos+_vel.norm()*10);
        intersectPoint =agentPath.intersection(obstaclePath);
        if(intersectPoint.isValid())
        {
            ////should remove this draws
            /// TOOD: must have better time Est
            timeForObs= timeEstimator(agentPos,agentVel,agentDir,agentGoal);
            timeForObs *= agentPos.dist(intersectPoint)/agentPos.dist(agentGoal);
            timeForObs *=1;
            timeForObs = min(maxTime,timeForObs);
            for(double i = 0;i< maxTime ; i+=0.05)
            {

                timeForObs +=i;
                _center= _pos + _vel*timeForObs;
                _rad = maxA*timeForObs*timeForObs ;

                _rad = min(maxObstRad,_rad);
                _rad = min(agentPos.dist(_center) - 0.3,_rad);
                _rad = min(_pos.dist(_center) - 0.3 ,_rad );
                _rad +=Robot::robot_radius_new;
                if(timeForObs >=0)
                {
                    obs.add_circle(_center.x , _center.y , _rad , 0 , 0);
//                    draw(Circle2D(_center,_rad),QColor(Qt::blue),true);
                }
            }
        }
        else
        {
            _center = _pos;
            _rad =Robot::robot_radius_new;
        }
    }

    obs.add_circle(_center.x , _center.y , _rad , 0 , 0);
//    draw(Circle2D(_center,_rad),QColor(Qt::blue),true);
}

void CPlanner::generateObstacleSpace(CObstacles &obs, QList<int> &ourRelaxList, QList<int> &oppRelaxList, bool avoidPenaltyArea, bool avoidCenterCircle , double ballObstacleRadius, int id,Vector2D agentGoal){
    obs.clear();
    obs.targetPosition = goal;

    bool isValid = false;

    Vector2D agentPos;

    Vector2D agentVel;

    // TODO : SELF
//    for(int i = 0; i < wm->our.activeAgentsCount() ; i ++)
//    {
//        if(wm->our[i]->id == id)
//        {
    isValid = true;
    agentPos = wm->our[ID]->pos;
    agentVel = wm->our[ID]->vel;
    obs.agentPos = agentPos;

//            break;
//        }
//    }
//    if(!isValid)
//        return;




    agentPath.assign(agentPos,agentGoal);
    Vector2D _center ,dummy1,dummy2;
    double rad = 0;
    for (int j=0;j<wm->our.activeAgentsCount();j++)
    {
        if( (ourRelaxList.contains(wm->our.active(j)->id) == false) && (ID != wm->our.active(j)->id))
        {

            createObstacleProb(obs,wm->our.active(j)->pos,wm->our.active(j)->vel, Vector2D(0,0),_center,rad,agentPos,agentVel,agentGoal,Vector2D(1,1));


            double obstVelFactor = 0.15;

            //obs.add_circle(_center.x , _center.y , rad , 0 , 0);

//            if(1 || Circle2D(wm->our[j]->pos,Robot::robot_radius_new+0.07).intersection(agentPath,&dummy1,&dummy2) > 1)
//            {
//            drawer->draw(Circle2D(wm->our.active(j)->pos,0.1),QColor(Qt::red),true);

            double obsRad = std::min(wm->our.active(j)->pos.dist(wm->our[ID]->pos)-0.02,0.2);
            obs.add_circle(wm->our.active(j)->pos.x , wm->our.active(j)->pos.y , 0.17 , 0 , 0);

//            }
        }
    }
    // ROS_INFO_STREAM("active opp: "<<wm->opp.activeAgentsCount());
    // ROS_INFO_STREAM("active our: "<<wm->our.activeAgentsCount());

    for (int j=0;j<wm->opp.activeAgentsCount();j++)
    {
        if(  oppRelaxList.contains(wm->opp.active(j)->id) == false )
        {

            createObstacleProb(obs,wm->opp.active(j)->pos,wm->opp.active(j)->vel,Vector2D(0,0),_center,rad,agentPos,agentVel,agentGoal,Vector2D(1,1));
            double obstVelFactor = 0.15;
            //obs.add_circle(_center.x , _center.y , rad , 0 , 0);
            double obsRad = std::min(wm->opp.active(j)->pos.dist(wm->our[ID]->pos)-0.02,0.2);
            DBUG(QString("obsRad : %1 ID : %2").arg(obsRad).arg(j),D_MHMMD);
            obs.add_circle(wm->opp.active(j)->pos.x , wm->opp.active(j)->pos.y , 0.17 , 0 , 0);
//            drawer->draw(Circle2D(wm->opp.active(j)->pos,0.1),QColor(Qt::red),true);
        }
    }

    if( ballObstacleRadius > EPSILON )
        obs.add_circle(wm->ball->pos.x,wm->ball->pos.y,ballObstacleRadius,wm->ball->vel.x , wm->ball->vel.y);

    if( avoidPenaltyArea ){
        obs.add_circle(-1*(field._FIELD_WIDTH / 2) , 0.25,1,0,0);
        obs.add_circle(-1*(field._FIELD_WIDTH / 2) , -0.25,1,0,0);
        obs.add_rectangle(-1*(field._FIELD_WIDTH / 2)+0.5,0,1,0.5);
    }

    ////////////////////test opPenalty
    obs.add_circle(1*(field._FIELD_WIDTH / 2) , 0.25,1,0,0);
    obs.add_circle(1*(field._FIELD_WIDTH / 2) , -0.25,1,0,0);
    obs.add_rectangle(1*(field._FIELD_WIDTH / 2) - 0.5,0,1,0.5);


    obs.add_rectangle(0,0,0.5,0.5);
    if (avoidCenterCircle)
    {
        obs.add_circle(0 , 0 , field._CENTER_CIRCLE_RAD , 0 , 0);
    }

    if(!obs.check(goal))
        drawer->draw(Vector2D(0, 0));


}

vector<Vector2D> CPlanner::getResultModified (){
    return resultModified;
}
Vector2D CPlanner::getAverageDir(){
    return  averageDir;
}

int CPlanner::getID() {
    return ID;
}

void CPlanner::emitPlan(const vector<Vector2D>& _resultModified, const Vector2D& averageDir) {
    parsian_msgs::parsian_pathPtr path{new parsian_msgs::parsian_path};
    path->averageDir = averageDir.toParsianMessage();
    for(const auto& v : _resultModified) {
        path->results.push_back(v.toParsianMessage());
    }
    path_pub.publish(path);
}

