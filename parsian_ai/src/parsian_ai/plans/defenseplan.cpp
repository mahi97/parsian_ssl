#include "parsian_ai/plans/defenseplan.h"

using namespace std;

#define LONG_CHIP_POWER 1023

///////////////// AHZ is writing, have you my voice? ... ;) //////////////////

Rect2D DefensePlan::ourBigPenaltyArea(double scale , double tolerance ,bool doChangesWithScale){
    Rect2D penaltyArea;
    if(doChangesWithScale){
        penaltyArea = Rect2D(Vector2D(wm->field->ourGoal().x, wm->field->ourGoal().y + wm->field->_PENALTY_WIDTH/2),
                             wm->field->_PENALTY_WIDTH * scale,
                             wm->field->_PENALTY_DEPTH * scale);
    }
    else{
        penaltyArea = Rect2D(Vector2D(wm->field->ourGoal().x, wm->field->ourGoal().y + wm->field->_PENALTY_WIDTH/2 + tolerance),
                             Vector2D(wm->field->ourGoal().x + wm->field->_PENALTY_DEPTH + tolerance, wm->field->ourGoal().y - wm->field->_PENALTY_WIDTH/2 - tolerance));
    }
    return penaltyArea;
}

bool DefensePlan::isPermissionTargetToChip(Vector2D aPoint){
    Vector2D tempIntersection0;
    Vector2D tempIntersection1;
    for(int i = 0 ; i < wm->our.activeAgentsCount() ; i++){
        if(Circle2D(wm->our.active(i)->pos , 0.2).intersection(Segment2D(goalKeeperAgent->pos() , aPoint) , &tempIntersection0 , &tempIntersection1)){
            return false;
        }
    }
    for(int i = 0 ; i < wm->opp.activeAgentsCount() ; i++){
        if(Circle2D(wm->opp.active(i)->pos , 0.2).intersection(Segment2D(goalKeeperAgent->pos() , aPoint) , &tempIntersection0 , &tempIntersection1)){
            return false;
        }
    }
    return true;
}



bool DefensePlan::isAgentsStuckTogether(QList<Vector2D> agentsPosition){
    //// If defense agents stuck together , this function
    for(int i = 0 ; i < agentsPosition.size() ; i++){
        for(int j = 0 ; j < agentsPosition.size() ; j++){
            if(i != j){
                if(agentsPosition.at(i).dist(agentsPosition.at(j)) <= 2 * Robot::robot_radius_new){
                    return true;
                }
            }
        }
    }
    return false;
}

void DefensePlan::agentsStuckTogether(QList<Vector2D> agentsPosition , QList<Vector2D> &stuckPositions , QList<int> &stuckIndexs){
    //// If defense agents stuck together , this function
    stuckPositions.clear();
    stuckIndexs.clear();
    for(int i = 0 ; i < agentsPosition.size() ; i++){
        for(int j = 0 ; j < i ; j++){
            if(i != j){
                if(agentsPosition.at(i).dist(agentsPosition.at(j)) <= 2 * Robot::robot_radius_new){
                    stuckPositions.append(agentsPosition.at(i));
                    stuckPositions.append(agentsPosition.at(j));
                }
            }
        }
    }
    for (auto stuckPosition : stuckPositions) {
        for(int j = 0 ; j < agentsPosition.size() ; j++){
            if(stuckPosition == agentsPosition.at(j)){
                stuckIndexs.append(j);
            }
        }
    }
}

void DefensePlan::correctingTheAgentsAreStuckTogether(QList<Vector2D> &agentsPosition,QList<Vector2D> &stuckPositions , QList<int> &stuckIndexs){

    QList<Segment2D> centerToCenter;
    QList<Vector2D> solvedPosition;
    QList<Vector2D> temp;
    QList<Vector2D> tempSol;
    QList<int> tempIndexs;
    QList<Vector2D> tempAgentsPosition;
    QList<Vector2D> finalSolvedPosition;
    QList<Vector2D> solvedPositionsAreNotInThePenaltyArea;
    QList<Vector2D> nonRepetitiveFinalSolvedPosition;
    Vector2D tempPoint = Vector2D(0,0);
    bool isRepeated = false;
    QList<Vector2D> tempVectors;
    ///////////////////// Update the state /////////////////////////////////////
    solvedPosition.clear();
    tempAgentsPosition.clear();
    nonRepetitiveFinalSolvedPosition.clear();
    solvedPositionsAreNotInThePenaltyArea.clear();
    ///////////////////////////////////////////////////////////////////////////
    for(int i = 0 ; i < stuckPositions.size() ; i++){
        if(i % 2 == 0){
            centerToCenter.append(Segment2D(stuckPositions.at(i) , stuckPositions.at(i+1)));
        }
        else{
            centerToCenter.append(Segment2D(stuckPositions.at(i) , stuckPositions.at(i-1)));
        }
    }
    DBUG(QString("center : %1").arg(centerToCenter.size()) , D_AHZ);
    for(int i = 0 ; i < stuckPositions.size() ; i++){
        if(stuckPositions.size() == 2){
            solvedPosition.append(stuckPositions.at(i) + (1.1*Robot::robot_radius_new - centerToCenter.at(i).length()/2)*((centerToCenter.at(i).a() - centerToCenter.at(i).b()).norm()));
        }
        else{
            solvedPosition.append(stuckPositions.at(i) + (1.4*Robot::robot_radius_new - centerToCenter.at(i).length()/2)*((centerToCenter.at(i).a() - centerToCenter.at(i).b()).norm()));
        }
    }
    ///////////// Check the resulted points, don't be in the PArea /////////////
    for(int i = 0 ; i < solvedPosition.size() ; i++){
        if(wm->field->isInOurPenaltyArea(solvedPosition.at(i))){
            DBUG(QString("stuck position is penalty area") , D_AHZ);
            solvedPosition.removeAt(i);
            tempSol = wm->field->ourPAreaIntersect(Segment2D(wm->field->ourGoal() , stuckPositions.at(i)));
            if(tempSol.size()){
                if(tempSol.size() == 2){
                    solvedPositionsAreNotInThePenaltyArea.append(tempSol.at(0).dist(stuckPositions.at(i)) < tempSol.at(1).dist(stuckPositions.at(i)) ? tempSol.at(0) : tempSol.at(1));
                }
                else if(tempSol.size() == 1){
                    solvedPositionsAreNotInThePenaltyArea.append(tempSol.at(0));
                }
            }
        }
        else{
            solvedPositionsAreNotInThePenaltyArea.append(solvedPosition.at(i));
        }
    }
    ////////////////////////////////////////////////////////////////////////////
    for(int i = 0 ; i < stuckPositions.size() && i < stuckIndexs.size() && i < solvedPositionsAreNotInThePenaltyArea.size() ; i++){
        for(int j = 0 ; j < stuckIndexs.size() ; j++){
            if(stuckIndexs.at(i) == stuckIndexs.at(j) && i != j){
                isRepeated = true;
                tempIndexs.append(j);
                DBUG(QString("temp Index : %1").arg(j) , D_AHZ);
            }
        }
        if(isRepeated){
            tempIndexs.append(i);
            tempPoint = stuckPositions.at(i);
            for(int k = 0 ; k < solvedPositionsAreNotInThePenaltyArea.size() ; k++){
                for (int tempIndex : tempIndexs) {
                    if(tempIndex == k){
                        temp.append(solvedPositionsAreNotInThePenaltyArea.at(k));
                    }
                }
            }
            for (auto n : temp) {
                tempVectors.append(n - tempPoint);
            }
            for (auto tempVector : tempVectors) {
                tempPoint += tempVector;
            }
            finalSolvedPosition.append(tempPoint);
            isRepeated = false;
        }
        else{
            tempIndexs.append(i);
            finalSolvedPosition.append(solvedPositionsAreNotInThePenaltyArea.at(i));
        }
        isRepeated = false;
        tempIndexs.clear();
        tempVectors.clear();
        temp.clear();
    }
    //// Tnx for Mahi && Parsa && Mhmmd :) ////////////////////////////////////
    for(auto i : finalSolvedPosition){
        if(!nonRepetitiveFinalSolvedPosition.contains(i)){
            nonRepetitiveFinalSolvedPosition.append(i);
        }
    }
    ///////////////////////////////////////////////////////////////////////////
    for (auto stuckPosition : stuckPositions) {
        for(int j = 0 ; j < agentsPosition.size() ; j++){
            if(agentsPosition.at(j) == stuckPosition){
                agentsPosition.removeAt(j);
            }
        }
    }

    for (auto i : nonRepetitiveFinalSolvedPosition) {
        agentsPosition.append(i);
    }

    DBUG(QString("solved : %1").arg(finalSolvedPosition.size()), D_AHZ);
    DBUG(QString("match : %1").arg(agentsPosition.size()), D_AHZ);
}

float getDegree(Vector2D pos1, Vector2D origin, Vector2D pos3){
    //// get the angle of between two vector that
    //// is made up by this 3 points.

    Vector2D v1 = pos1 - origin;
    Vector2D v2 = pos3 - origin;
    return (float) (v1.th() - v2.th()).degree();
}

bool DefensePlan::isInIndirectArea(Vector2D aPoint){
    //// check that a point is in the circle around the ball
    //// with 50cm radius or not.

    bool localFlag = Circle2D(wm->ball->pos , 0.7).contains(aPoint);
    return localFlag;
}

Line2D DefensePlan::getBisectorLine(Vector2D firstPoint , Vector2D originPoint , Vector2D thirdPoint){
    //// get the bisector line of an angle
    //// that is made up by this 3 points.

    Line2D bisectorLine (originPoint , AngleDeg::bisect((firstPoint - originPoint).th() , (thirdPoint - originPoint).th()));
    return bisectorLine;
}

Segment2D DefensePlan::getBisectorSegment(Vector2D firstPoint , Vector2D originPoint , Vector2D thirdPoint){
    //// get the bisector segment of an angle
    //// that is made up by this 3 points.

    Line2D bisectorLine (originPoint , AngleDeg::bisect((firstPoint - originPoint).th() , (thirdPoint - originPoint).th()));
    Segment2D bisectorSegment(originPoint , Segment2D (thirdPoint , firstPoint).intersection(bisectorLine));
    return bisectorSegment;
}

void DefensePlan::manToManMarkBlockPassInPlayOff(QList<Vector2D> opponentAgentsToBeMarkPossition , int ourMarkAgentsSize , double proportionOfDistance){
    //// This function blocking the lines that are between ball &&
    //// opponent agents by a variable ratio along these lines.
    //// This is one of the mark plan for defending more flexible


    ////////////////////////// Variables of this function //////////////////////
    int i;
    int j;
    Vector2D ourCenterOfGoalPossition = wm->field->ourGoal();
    Vector2D sol1 , sol2;
    Vector2D sol3 , sol4;
    Vector2D sol5 , sol6;
    Vector2D sol7 , sol8;
    double opponentAgentsCircleR = 0.2;
    QList<Circle2D> opponentAgentsCircle;
    QList<Circle2D> opponentAgentsToBeMarkCircle;
    QList<Circle2D> tempOpponentAgentsToBeMarkedCircle;
    QList<Vector2D> ourMarkAgentsPossition;
    QList<Vector2D> tempOpponentAgentsToBeMarkedPosition;
    QList<QPair<Vector2D,double> > sortDangerAgentsToBeMarkBlockPassPlayOff;
    QList<QPair<Vector2D,double> > tempSortDangerAgentsToBeBlockPassPlayOff;
    Circle2D goalCircle(ourCenterOfGoalPossition , 1.43);
    Circle2D penaltyArea(wm->field->ourGoal(),1.28);
    //////////////////// Clear QLists for update the states ////////////////////
    stopMode = gameState->isPlayOff();
    ourMarkAgentsPossition.clear();
    tempOpponentAgentsToBeMarkedPosition.clear();
    markPoses.clear();
    markAngs.clear();
    markRoles.clear();
    /////////////////// Intelligent mark plan ///////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    DBUG(QString("Mark Agents Count : %1").arg(ourMarkAgentsSize) , D_SEPEHR);
    ///////// Make Cirlcles around opponent agents /////////////////////////////
    DBUG(QString("Opponent Agents to be mark count : %1").arg(opponentAgentsToBeMarkPossition.size()) , D_SEPEHR);
    for (auto i : opponentAgentsToBeMarkPossition) {
        if(!isInIndirectArea(i)){
            tempOpponentAgentsToBeMarkedPosition.append(i);
        }
    }
    opponentAgentsToBeMarkPossition.clear();
    opponentAgentsToBeMarkPossition = tempOpponentAgentsToBeMarkedPosition;
    for(i = 0 ; i < opponentAgentsToBeMarkPossition.size(); i++){
        opponentAgentsToBeMarkCircle.append(Circle2D(opponentAgentsToBeMarkPossition.at(i) , opponentAgentsCircleR));
        drawer->draw(opponentAgentsToBeMarkCircle.at(i) , "Cyan");
    }
    DBUG(QString("Opponent Agents to be mark count : %1").arg(opponentAgentsToBeMarkPossition.size()) , D_SEPEHR);
    ///////////////// Block Pass Plan ////////////////////////////////////
    if(opponentAgentsToBeMarkPossition.size() == ourMarkAgentsSize){
        for(i = 0 ; i < ourMarkAgentsSize ; i++){
            markRoles.append(QString("passBlocker"));
            //////////// Don't Enter penalty area, mark agents!!! :) ///////////
            if(penaltyArea.intersection(Segment2D(wm->ball->pos , opponentAgentsToBeMarkPossition.at(i)) , &sol7,  &sol8)){
                if(!wm->field->isInOurPenaltyArea(opponentAgentsToBeMarkPossition.at(i))){
                    opponentAgentsToBeMarkCircle.at(i).intersection(Segment2D(wm->ball->pos , opponentAgentsToBeMarkPossition.at(i)), &sol1 , &sol2);
                    Circle2D(wm->ball->pos , ballCircleR).intersection(Segment2D(wm->ball->pos , opponentAgentsToBeMarkPossition.at(i)), &sol3 , &sol4);
                    penaltyArea.intersection(Segment2D(Segment2D(sol1 , wm->ball->pos).length() < Segment2D(sol2 , wm->ball->pos).length() ? sol1 : sol2,Segment2D(sol3 , opponentAgentsToBeMarkPossition.at(i)).length() < Segment2D(sol4 , opponentAgentsToBeMarkPossition.at(i)).length() ? sol3 : sol4) , &sol5 , &sol6);
                    markPoses.append(know->getPointInDirection(Segment2D(sol1 , wm->ball->pos).length() < Segment2D(sol2 , wm->ball->pos).length() ? sol1 : sol2 ,Segment2D(sol5 , opponentAgentsToBeMarkPossition.at(i)).length() < Segment2D(sol6 , opponentAgentsToBeMarkPossition.at(i)).length() ? sol5:sol6 ,proportionOfDistance));
                }
                else{
                    goalCircle.intersection(Segment2D(wm->field->ourGoal() , opponentAgentsToBeMarkPossition.at(i)) , &sol1 , &sol2);
                    markPoses.append(Segment2D(sol1 , opponentAgentsToBeMarkPossition.at(i)).length() < Segment2D(sol2 , opponentAgentsToBeMarkPossition.at(i)).length() ? sol1 : sol2);
                }
            }
            else{
                opponentAgentsToBeMarkCircle.at(i).intersection(Segment2D(wm->ball->pos , opponentAgentsToBeMarkPossition.at(i)), &sol1 , &sol2);
                Circle2D(wm->ball->pos , ballCircleR).intersection(Segment2D(wm->ball->pos , opponentAgentsToBeMarkPossition.at(i)), &sol3 , &sol4);
                markPoses.append(know->getPointInDirection(Segment2D(sol1 , wm->ball->pos).length() < Segment2D(sol2 , wm->ball->pos).length() ? sol1 : sol2,Segment2D(sol3 , opponentAgentsToBeMarkPossition.at(i)).length() < Segment2D(sol4 , opponentAgentsToBeMarkPossition.at(i)).length() ? sol3 : sol4, proportionOfDistance));
            }
            markAngs.append(wm->ball->pos - markPoses.at(i));
            drawer->draw(Segment2D(Segment2D(sol1 , wm->ball->pos).length() < Segment2D(sol2 , wm->ball->pos).length() ? sol1 : sol2,
                                   Segment2D(sol3 , opponentAgentsToBeMarkPossition.at(i)).length() < Segment2D(sol4 , opponentAgentsToBeMarkPossition.at(i)).length() ? sol3 : sol4) , QColor(Qt::red));
            drawer->draw(Circle2D(wm->ball->pos , ballCircleR),QColor(Qt::black));
            drawer->draw(goalCircle,QColor(Qt::black));
            DBUG(QString("Man To Man Mark In PlayOff Mode / BlockPass / our = opp") , D_SEPEHR);
        }
    }
    else if(opponentAgentsToBeMarkPossition.size() < ourMarkAgentsSize){
        if(opponentAgentsToBeMarkPossition.empty()){
            for(i = 0 ; i < ourMarkAgentsSize ; i++){
                if(i % 2){
                    markPoses.append(Vector2D(-3 , i / 1.5));
                    markAngs.append(Vector2D(0,0));
                    markRoles.append(QString("shotBlocker"));
                }
                else{
                    markPoses.append(Vector2D(-3,-i  / 1.5));
                    markAngs.append(Vector2D(0,0));
                    markRoles.append(QString("shotBlocker"));
                }
            }
        }
        else if(opponentAgentsToBeMarkPossition.size() == 1){
            for(i = 0 ; i < opponentAgentsToBeMarkPossition.size() ; i++){
                markRoles.append(QString("passBlocker"));
                //////////// Don't Enter penalty area, mark agents!!! :) ///////////
                if(penaltyArea.intersection(Segment2D(wm->ball->pos , opponentAgentsToBeMarkPossition.at(i)) , &sol7,  &sol8)){
                    if(!wm->field->isInOurPenaltyArea(opponentAgentsToBeMarkPossition.at(i))){
                        opponentAgentsToBeMarkCircle.at(i).intersection(Segment2D(wm->ball->pos , opponentAgentsToBeMarkPossition.at(i)), &sol1 , &sol2);
                        Circle2D(wm->ball->pos , ballCircleR).intersection(Segment2D(wm->ball->pos , opponentAgentsToBeMarkPossition.at(i)), &sol3 , &sol4);
                        penaltyArea.intersection(Segment2D(Segment2D(sol1 , wm->ball->pos).length() < Segment2D(sol2 , wm->ball->pos).length() ? sol1 : sol2,Segment2D(sol3 , opponentAgentsToBeMarkPossition.at(i)).length() < Segment2D(sol4 , opponentAgentsToBeMarkPossition.at(i)).length() ? sol3 : sol4) , &sol5 , &sol6);
                        markPoses.append(know->getPointInDirection(Segment2D(sol1 , wm->ball->pos).length() < Segment2D(sol2 , wm->ball->pos).length() ? sol1 : sol2 ,Segment2D(sol5 , opponentAgentsToBeMarkPossition.at(i)).length() < Segment2D(sol6 , opponentAgentsToBeMarkPossition.at(i)).length() ? sol5:sol6 ,proportionOfDistance));
                    }
                    else{
                        penaltyArea.intersection(Line2D(wm->field->ourGoal() , opponentAgentsToBeMarkPossition.at(i)) , &sol1 , &sol2);
                        markPoses.append(Segment2D(sol1 , opponentAgentsToBeMarkPossition.at(i)).length() < Segment2D(sol2 , opponentAgentsToBeMarkPossition.at(i)).length() ? sol1 : sol2);
                    }
                }
                else{
                    opponentAgentsToBeMarkCircle.at(i).intersection(Segment2D(wm->ball->pos , opponentAgentsToBeMarkPossition.at(i)), &sol1 , &sol2);
                    Circle2D(wm->ball->pos , ballCircleR).intersection(Segment2D(wm->ball->pos , opponentAgentsToBeMarkPossition.at(i)), &sol3 , &sol4);
                    markPoses.append(know->getPointInDirection(Segment2D(sol1 , wm->ball->pos).length() < Segment2D(sol2 , wm->ball->pos).length() ? sol1 : sol2,Segment2D(sol3 , opponentAgentsToBeMarkPossition.at(i)).length() < Segment2D(sol4 , opponentAgentsToBeMarkPossition.at(i)).length() ? sol3 : sol4, proportionOfDistance));
                }
                markAngs.append(wm->ball->pos - markPoses.at(i));
                drawer->draw(Segment2D(Segment2D(sol1 , wm->ball->pos).length() < Segment2D(sol2 , wm->ball->pos).length() ? sol1 : sol2,Segment2D(sol3 , opponentAgentsToBeMarkPossition.at(i)).length() < Segment2D(sol4 , opponentAgentsToBeMarkPossition.at(i)).length() ? sol3 : sol4), QColor(Qt::red));
                drawer->draw(Circle2D(wm->ball->pos , ballCircleR),QColor(Qt::black));
            }
            for(i = 0 ; i < opponentAgentsToBeMarkPossition.size() ; i++){
                if(!wm->field->isInOurPenaltyArea(opponentAgentsToBeMarkPossition.at(i))){
                    opponentAgentsToBeMarkCircle.at(i).intersection(Segment2D(wm->field->ourGoal() , opponentAgentsToBeMarkPossition.at(i)), &sol1 , &sol2);
                    penaltyArea.intersection(Segment2D(opponentAgentsToBeMarkPossition.at(i) , wm->field->ourGoal()), &sol3 , &sol4);
                    markPoses.append(know->getPointInDirection(Segment2D(sol1 , wm->field->ourGoal()).length() < Segment2D(sol2 , wm->field->ourGoal()).length() ? sol1 : sol2 ,Segment2D(sol3 , opponentAgentsToBeMarkPossition.at(i)).length() < Segment2D(sol4 , opponentAgentsToBeMarkPossition.at(i)).length() ? sol3:sol4 , conf.ShootRatioBlock / 100));
                }
                else{
                    penaltyArea.intersection(Line2D(wm->field->ourGoal() , opponentAgentsToBeMarkPossition.at(i)) , &sol1 , &sol2);
                    markPoses.append(Segment2D(wm->field->center() , sol1).length() < Segment2D(wm->field->center() , sol2).length() ? sol1 : sol2);
                }
                markAngs.append(opponentAgentsToBeMarkPossition.at(i) - wm->field->ourGoal());
                markRoles.append(QString("shotBlocker"));
            }
            for(i = 0 ; i < ourMarkAgentsSize - markPoses.size() ; i++){
                if(i % 2){
                    markPoses.append(Vector2D(-3 , i / 1.5));
                    markAngs.append(Vector2D(0,0));
                    markRoles.append(QString("shotBlocker"));
                }
                else{
                    markPoses.append(Vector2D(-3,-i  / 1.5));
                    markAngs.append(Vector2D(0,0));
                    markRoles.append(QString("shotBlocker"));
                }
            }
            DBUG(QString("mark pos : %1").arg(markPoses.size()) , D_AHZ);
            DBUG(QString("our mark : %1").arg(ourMarkAgentsSize) , D_AHZ);
        }
        else{
            for(i = 0 ; i < opponentAgentsToBeMarkPossition.size() ; i++){
                markRoles.append(QString("passBlocker"));
                //////////// Don't Enter penalty area, mark agents!!! :) ///////////
                if(penaltyArea.intersection(Segment2D(wm->ball->pos , opponentAgentsToBeMarkPossition.at(i)) , &sol7,  &sol8)){
                    if(!wm->field->isInOurPenaltyArea(opponentAgentsToBeMarkPossition.at(i))){
                        opponentAgentsToBeMarkCircle.at(i).intersection(Segment2D(wm->ball->pos , opponentAgentsToBeMarkPossition.at(i)), &sol1 , &sol2);
                        Circle2D(wm->ball->pos , ballCircleR).intersection(Segment2D(wm->ball->pos , opponentAgentsToBeMarkPossition.at(i)), &sol3 , &sol4);
                        penaltyArea.intersection(Segment2D(Segment2D(sol1 , wm->ball->pos).length() < Segment2D(sol2 , wm->ball->pos).length() ? sol1 : sol2,Segment2D(sol3 , opponentAgentsToBeMarkPossition.at(i)).length() < Segment2D(sol4 , opponentAgentsToBeMarkPossition.at(i)).length() ? sol3 : sol4) , &sol5 , &sol6);
                        markPoses.append(know->getPointInDirection(Segment2D(sol1 , wm->ball->pos).length() < Segment2D(sol2 , wm->ball->pos).length() ? sol1 : sol2 ,Segment2D(sol5 , opponentAgentsToBeMarkPossition.at(i)).length() < Segment2D(sol6 , opponentAgentsToBeMarkPossition.at(i)).length() ? sol5:sol6 ,proportionOfDistance));
                    }
                    else{
                        penaltyArea.intersection(Line2D(wm->field->ourGoal() , opponentAgentsToBeMarkPossition.at(i)) , &sol1 , &sol2);
                        markPoses.append(Segment2D(sol1 , opponentAgentsToBeMarkPossition.at(i)).length() < Segment2D(sol2 , opponentAgentsToBeMarkPossition.at(i)).length() ? sol1 : sol2);
                    }
                }
                else{
                    opponentAgentsToBeMarkCircle.at(i).intersection(Segment2D(wm->ball->pos , opponentAgentsToBeMarkPossition.at(i)), &sol1 , &sol2);
                    Circle2D(wm->ball->pos , ballCircleR).intersection(Segment2D(wm->ball->pos , opponentAgentsToBeMarkPossition.at(i)), &sol3 , &sol4);
                    markPoses.append(know->getPointInDirection(Segment2D(sol1 , wm->ball->pos).length() < Segment2D(sol2 , wm->ball->pos).length() ? sol1 : sol2,Segment2D(sol3 , opponentAgentsToBeMarkPossition.at(i)).length() < Segment2D(sol4 , opponentAgentsToBeMarkPossition.at(i)).length() ? sol3 : sol4, proportionOfDistance));
                }
                markAngs.append(wm->ball->pos - markPoses.at(i));
                drawer->draw(Segment2D(Segment2D(sol1 , wm->ball->pos).length() < Segment2D(sol2 , wm->ball->pos).length() ? sol1 : sol2,Segment2D(sol3 , opponentAgentsToBeMarkPossition.at(i)).length() < Segment2D(sol4 , opponentAgentsToBeMarkPossition.at(i)).length() ? sol3 : sol4), QColor(Qt::red));
                drawer->draw(Circle2D(wm->ball->pos , ballCircleR),QColor(Qt::black));
                DBUG(QString("Man To Man Mark In PlayOn Mode / BlockPass / opp < our") , D_SEPEHR);
            }
            ////////////// With Extra mark agents ch ghalati bokonim ? :) //////////
            if(ourMarkAgentsSize - markPoses.size() == opponentAgentsToBeMarkPossition.size()){
                for(i = 0 ; i < opponentAgentsToBeMarkPossition.size() ; i++){
                    drawer->draw(opponentAgentsToBeMarkCircle.at(i),QColor(Qt::cyan));
                }
                for(i = 0 ; i < opponentAgentsToBeMarkPossition.size() ; i++){
                    if(!wm->field->isInOurPenaltyArea(opponentAgentsToBeMarkPossition.at(i))){
                        opponentAgentsToBeMarkCircle.at(i).intersection(Segment2D(wm->field->ourGoal() , opponentAgentsToBeMarkPossition.at(i)), &sol1 , &sol2);
                        penaltyArea.intersection(Segment2D(opponentAgentsToBeMarkPossition.at(i) , wm->field->ourGoal()), &sol3 , &sol4);
                        markPoses.append(know->getPointInDirection(Segment2D(sol1 , wm->field->ourGoal()).length() < Segment2D(sol2 , wm->field->ourGoal()).length() ? sol1 : sol2 ,Segment2D(sol3 , opponentAgentsToBeMarkPossition.at(i)).length() < Segment2D(sol4 , opponentAgentsToBeMarkPossition.at(i)).length() ? sol3:sol4 ,conf.ShootRatioBlock / 100));
                    }
                    else{
                        penaltyArea.intersection(Line2D(wm->field->ourGoal() , opponentAgentsToBeMarkPossition.at(i)) , &sol1 , &sol2);
                        markPoses.append(Segment2D(wm->field->center() , sol1).length() < Segment2D(wm->field->center() , sol2).length() ? sol1 : sol2);
                    }
                    markAngs.append(opponentAgentsToBeMarkPossition.at(i) - wm->field->ourGoal());
                    markRoles.append(QString("shotBlocker"));
                }
            }
            else if(ourMarkAgentsSize - markPoses.size() < opponentAgentsToBeMarkPossition.size()){
                tempSortDangerAgentsToBeBlockPassPlayOff = sortdangerpassplayoff(opponentAgentsToBeMarkPossition);
                for(i = 0 ; i < tempSortDangerAgentsToBeBlockPassPlayOff.size() ; i++){
                    tempOpponentAgentsToBeMarkedCircle.append(Circle2D(tempSortDangerAgentsToBeBlockPassPlayOff.at(i).first , opponentAgentsCircleR));
                    drawer->draw(tempOpponentAgentsToBeMarkedCircle.at(i),QColor(Qt::yellow));
                }
                for(i = 0 ; i < ourMarkAgentsSize - markPoses.size() ; i++){
                    if(!wm->field->isInOurPenaltyArea(tempSortDangerAgentsToBeBlockPassPlayOff.at(i).first)){
                        tempOpponentAgentsToBeMarkedCircle.at(i).intersection(Segment2D(wm->field->ourGoal() , tempSortDangerAgentsToBeBlockPassPlayOff.at(i).first), &sol1 , &sol2);
                        penaltyArea.intersection(Segment2D(tempSortDangerAgentsToBeBlockPassPlayOff.at(i).first , wm->field->ourGoal()), &sol3 , &sol4);
                        markPoses.append(know->getPointInDirection(Segment2D(sol1 , wm->field->ourGoal()).length() < Segment2D(sol2 , wm->field->ourGoal()).length() ? sol1 : sol2 ,Segment2D(sol3 , tempSortDangerAgentsToBeBlockPassPlayOff.at(i).first).length() < Segment2D(sol4 , tempSortDangerAgentsToBeBlockPassPlayOff.at(i).first).length() ? sol3:sol4 ,conf.ShootRatioBlock / 100));
                    }
                    else{
                        penaltyArea.intersection(Line2D(wm->field->ourGoal() , tempSortDangerAgentsToBeBlockPassPlayOff.at(i).first) , &sol1 , &sol2);
                        markPoses.append(Segment2D(wm->field->center() , sol1).length() < Segment2D(wm->field->center() , sol2).length() ? sol1 : sol2);
                    }
                    markAngs.append(tempSortDangerAgentsToBeBlockPassPlayOff.at(i).first - wm->field->ourGoal());
                    markRoles.append(QString("shotBlocker"));
                }
            }
            else if(ourMarkAgentsSize - markPoses.size() > opponentAgentsToBeMarkPossition.size()){
                for(i = 0 ; i < opponentAgentsToBeMarkPossition.size() ; i++){
                    drawer->draw(opponentAgentsToBeMarkCircle.at(i),QColor(Qt::cyan));
                }
                for(i = 0 ; i < opponentAgentsToBeMarkPossition.size() ; i++){
                    if(!wm->field->isInOurPenaltyArea(opponentAgentsToBeMarkPossition.at(i))){
                        opponentAgentsToBeMarkCircle.at(i).intersection(Segment2D(wm->field->ourGoal() , opponentAgentsToBeMarkPossition.at(i)), &sol1 , &sol2);
                        penaltyArea.intersection(Segment2D(opponentAgentsToBeMarkPossition.at(i) , wm->field->ourGoal()), &sol3 , &sol4);
                        markPoses.append(know->getPointInDirection(Segment2D(sol1 , wm->field->ourGoal()).length() < Segment2D(sol2 , wm->field->ourGoal()).length() ? sol1 : sol2 ,Segment2D(sol3 , opponentAgentsToBeMarkPossition.at(i)).length() < Segment2D(sol4 , opponentAgentsToBeMarkPossition.at(i)).length() ? sol3:sol4 ,conf.ShootRatioBlock / 100));
                    }
                    else{
                        penaltyArea.intersection(Line2D(wm->field->ourGoal() , opponentAgentsToBeMarkPossition.at(i)) , &sol1 , &sol2);
                        markPoses.append(Segment2D(wm->field->center() , sol1).length() < Segment2D(wm->field->center() , sol2).length() ? sol1 : sol2);
                    }
                    markAngs.append(opponentAgentsToBeMarkPossition.at(i) - wm->field->ourGoal());
                    markRoles.append(QString("shotBlocker"));
                }
                for(i = 0 ; i < wm->opp.activeAgentsCount() ; i++){
                    opponentAgentsCircle.append(Circle2D(wm->opp.active(i)->pos , opponentAgentsCircleR));
                }
                for(i = 0 ; i < wm->opp.activeAgentsCount() ; i++){
                    for(j = 0 ; j < opponentAgentsToBeMarkPossition.size() ; j++){
                        if(opponentAgentsToBeMarkPossition.at(j) == wm->opp.active(i)->pos){
                            opponentAgentsCircle.removeAt(i);
                        }
                    }
                }
                for(i = 0 ; i < wm->opp.activeAgentsCount() ; i++){
                    if(Circle2D(wm->ball->pos , ballCircleR).contains(wm->opp.active(i)->pos)){
                        opponentAgentsCircle.removeAt(i);
                    }
                }
                if(opponentAgentsToBeMarkPossition.empty()){
                    for(i = 0 ; i < opponentAgentsToBeMarkPossition.size() ; i++){
                        if(!wm->field->isInOurPenaltyArea(opponentAgentsCircle.at(i).center())){
                            opponentAgentsCircle.at(i).intersection(Segment2D(wm->field->ourGoal() , opponentAgentsCircle.at(i).center()), &sol1 , &sol2);
                            penaltyArea.intersection(Segment2D(opponentAgentsCircle.at(i).center() , wm->field->ourGoal()), &sol3 , &sol4);
                            markPoses.append(know->getPointInDirection(Segment2D(sol1 , wm->field->ourGoal()).length() < Segment2D(sol2 , wm->field->ourGoal()).length() ? sol1 : sol2 ,Segment2D(sol3 , opponentAgentsCircle.at(i).center()).length() < Segment2D(sol4 , opponentAgentsCircle.at(i).center()).length() ? sol3:sol4 ,conf.ShootRatioBlock / 100));
                        }
                        else{
                            penaltyArea.intersection(Line2D(wm->field->ourGoal() , opponentAgentsCircle.at(i).center()) , &sol1 , &sol2);
                            markPoses.append(Segment2D(wm->field->center() , sol1).length() < Segment2D(wm->field->center() , sol2).length() ? sol1 : sol2);
                        }
                        markAngs.append(opponentAgentsCircle.at(i).center() - wm->field->ourGoal());
                        markRoles.append(QString("shotBlocker"));
                    }
                }
                else{
                    for(i = 0 ; i < ourMarkAgentsSize - markPoses.size()+1 ; i++){
                        if(!wm->field->isInOurPenaltyArea(opponentAgentsCircle.at(i).center())){
                            opponentAgentsCircle.at(i).intersection(Segment2D(wm->field->ourGoal() , opponentAgentsCircle.at(i).center()), &sol1 , &sol2);
                            penaltyArea.intersection(Segment2D(opponentAgentsCircle.at(i).center() , wm->field->ourGoal()), &sol3 , &sol4);
                            markPoses.append(know->getPointInDirection(Segment2D(sol1 , wm->field->ourGoal()).length() < Segment2D(sol2 , wm->field->ourGoal()).length() ? sol1 : sol2 ,Segment2D(sol3 , opponentAgentsCircle.at(i).center()).length() < Segment2D(sol4 , opponentAgentsCircle.at(i).center()).length() ? sol3:sol4 ,conf.ShootRatioBlock / 100));
                        }
                        else{
                            penaltyArea.intersection(Line2D(wm->field->ourGoal() , opponentAgentsCircle.at(i).center()) , &sol1 , &sol2);
                            markPoses.append(Segment2D(wm->field->center() , sol1).length() < Segment2D(wm->field->center() , sol2).length() ? sol1 : sol2);
                        }
                        markAngs.append(opponentAgentsCircle.at(i).center() - wm->field->ourGoal());
                        markRoles.append(QString("shotBlocker"));
                    }
                }
            }
        }
        if(ourMarkAgentsSize > markPoses.size()){
            for(i = 0 ; i < ourMarkAgentsSize - markPoses.size() ; i++){
                if(i % 2){
                    markPoses.append(Vector2D(-3 , (i+1) / 0.75));
                    markAngs.append(Vector2D(0,0));
                    markRoles.append(QString("shotBlocker"));
                }
                else{
                    markPoses.append(Vector2D(-3,(-i-1)  / 0.75));
                    markAngs.append(Vector2D(0,0));
                    markRoles.append(QString("shotBlocker"));
                }
            }
        }
    }
    else if(opponentAgentsToBeMarkPossition.size() > ourMarkAgentsSize){
        sortDangerAgentsToBeMarkBlockPassPlayOff = sortdangerpassplayoff(opponentAgentsToBeMarkPossition);
        for(i = 0 ; i < ourMarkAgentsSize; i++){
            //////////// Don't Enter penalty area, mark agents!!! :) ///////////
            if(penaltyArea.intersection(Segment2D(wm->ball->pos , sortDangerAgentsToBeMarkBlockPassPlayOff.at(i).first) , &sol7,  &sol8)){
                if(!wm->field->isInOurPenaltyArea(sortDangerAgentsToBeMarkBlockPassPlayOff.at(i).first)){
                    Circle2D(sortDangerAgentsToBeMarkBlockPassPlayOff.at(i).first , opponentAgentsCircleR).intersection(Segment2D(sortDangerAgentsToBeMarkBlockPassPlayOff.at(i).first , wm->ball->pos) , &sol1 , &sol2);
                    Circle2D(wm->ball->pos , ballCircleR).intersection(Segment2D(sortDangerAgentsToBeMarkBlockPassPlayOff.at(i).first , wm->ball->pos) , &sol3 , &sol4);
                    penaltyArea.intersection(Segment2D(Segment2D(sol1 , wm->ball->pos).length() < Segment2D(sol2 , wm->ball->pos).length() ? sol1 : sol2,Segment2D(sol3 , sortDangerAgentsToBeMarkBlockPassPlayOff.at(i).first).length() < Segment2D(sol4 , sortDangerAgentsToBeMarkBlockPassPlayOff.at(i).first).length() ? sol3 : sol4) , &sol5 , &sol6);
                    markPoses.append(know->getPointInDirection(Segment2D(sol1 , wm->ball->pos).length() < Segment2D(sol2 , wm->ball->pos).length() ? sol1 : sol2 ,Segment2D(sol5 , sortDangerAgentsToBeMarkBlockPassPlayOff.at(i).first).length() < Segment2D(sol6 , sortDangerAgentsToBeMarkBlockPassPlayOff.at(i).first).length() ? sol5:sol6 ,proportionOfDistance));
                }
                else{
                    Circle2D(wm->ball->pos , ballCircleR).intersection(Segment2D(sortDangerAgentsToBeMarkBlockPassPlayOff.at(i).first , wm->ball->pos) , &sol1 , &sol2);
                    penaltyArea.intersection(Segment2D(wm->ball->pos , sortDangerAgentsToBeMarkBlockPassPlayOff.at(i).first) , &sol5 , &sol6);
                    markPoses.append(know->getPointInDirection(Segment2D(sol1 , sortDangerAgentsToBeMarkBlockPassPlayOff.at(i).first).length() < Segment2D(sol2 , sortDangerAgentsToBeMarkBlockPassPlayOff.at(i).first).length() ? sol1 : sol2 ,Segment2D(sol5 , wm->ball->pos).length() < Segment2D(sol6 , wm->ball->pos).length() ? sol5 :sol6 ,proportionOfDistance));
                }
            }
            else{
                Circle2D(sortDangerAgentsToBeMarkBlockPassPlayOff.at(i).first , opponentAgentsCircleR).intersection(Segment2D(sortDangerAgentsToBeMarkBlockPassPlayOff.at(i).first , wm->ball->pos) , &sol1 , &sol2);
                Circle2D(wm->ball->pos , ballCircleR).intersection(Segment2D(wm->ball->pos , sortDangerAgentsToBeMarkBlockPassPlayOff.at(i).first), &sol3 , &sol4);
                markPoses.append(know->getPointInDirection(Segment2D(sol1 , wm->ball->pos).length() < Segment2D(sol2 , wm->ball->pos).length() ? sol1 : sol2,Segment2D(sol3 , sortDangerAgentsToBeMarkBlockPassPlayOff.at(i).first).length() < Segment2D(sol4 , sortDangerAgentsToBeMarkBlockPassPlayOff.at(i).first).length() ? sol3 : sol4, proportionOfDistance));
            }
            markAngs.append(wm->ball->pos - markPoses.at(i));
            markRoles.append(QString("passBlocker"));
            drawer->draw(Segment2D(Segment2D(sol1 , wm->ball->pos).length() < Segment2D(sol2 , wm->ball->pos).length() ? sol1 : sol2,
                                   Segment2D(sol3 , sortDangerAgentsToBeMarkBlockPassPlayOff.at(i).first).length() < Segment2D(sol4 , sortDangerAgentsToBeMarkBlockPassPlayOff.at(i).first).length() ? sol3 : sol4)
                         , QColor(Qt::red));
            drawer->draw(Circle2D(wm->ball->pos , ballCircleR),QColor(Qt::black));
            DBUG(QString("Man To Man Mark In PlayOn Mode / BlockPass / opp > our") , D_SEPEHR);
        }
    }
    //////////////// Draw Possition of Mark Agents //////////////////////////
    for(i = 0 ; i < markPoses.size() ; i++){
        DBUG(QString("x : %1").arg(markPoses.at(i).x) , D_AHZ);
        drawer->draw(markPoses.at(i) ,"white"); //Review
        drawer->draw(markRoles.at(i) , markPoses.at(i) - Vector2D(0,0.4) , "white");
    }
}

void DefensePlan::manToManMarkBlockShotInPlayOff(int _markAgentSize){
    //// This function blocks the lines that are between center of our goal &&
    //// opponent agents by a variable ratio along these lines.
    //// This is one of the mark plan for defending more flexible.

    bool playOn = gameState->isPlayOn();
    bool playOff = gameState->theirDirectKick() || gameState->theirIndirectKick();
    int count;
    oppmarkedpos.clear();
    markPoses.clear();
    markRoles.clear();
    markAngs.clear();
    /////////////////// Intelligent mark plan ///////////////////////////////
    //////////////////////////////////////////////////////
    if(_markAgentSize == oppAgentsToMarkPos.count()){
        for(int i = 0; i < oppAgentsToMarkPos.count(); i++){
            markRoles.append(QString("shotBlocker"));
            if(!isInTheIndirectAreaShoot(oppAgentsToMarkPos[i])){
                markPoses.append(ShootBlockRatio(segmentpershoot, oppAgentsToMarkPos[i]).first());
                markAngs.append(ShootBlockRatio(segmentpershoot, oppAgentsToMarkPos[i]).last());
            }
            else{
                markPoses.append(indirectAvoidShoot(oppAgentsToMarkPos[i]).first());
                markAngs.append(indirectAvoidShoot(oppAgentsToMarkPos[i]).last());
            }
        }
    }
    else if(_markAgentSize > oppAgentsToMarkPos.count()){
        // Blocking direct shoot/
        for(int i =0;i < oppAgentsToMarkPos.count();i++){
            markRoles.append(QString("shotBlocker"));
            if(!isInTheIndirectAreaShoot(oppAgentsToMarkPos[i])){
                markPoses.append(ShootBlockRatio(segmentpershoot, oppAgentsToMarkPos[i]).first());
                markAngs.append(ShootBlockRatio(segmentpershoot, oppAgentsToMarkPos[i]).last());
            }
            else{
                markPoses.append(indirectAvoidShoot(oppAgentsToMarkPos[i]).first());
                markAngs.append(indirectAvoidShoot(oppAgentsToMarkPos[i]).last());
            }
        }
        QList<QPair<Vector2D, double> > tempsorted = sortdangerpassplayoff(oppAgentsToMarkPos);
        for(int i = 0; i < min(_markAgentSize - oppAgentsToMarkPos.count(), oppAgentsToMarkPos.count());i++){
            markRoles.append(QString("passBlocker"));
            if(!isInTheIndirectAreaPass(tempsorted[i].first)){
                markPoses.append(PassBlockRatio(segmentperpass, tempsorted[i].first).first());
                markAngs.append(PassBlockRatio(segmentperpass, tempsorted[i].first).last());
            }
            else{
                markPoses.append(indirectAvoidPass(tempsorted[i].first).first());
                markAngs.append(indirectAvoidPass(tempsorted[i].first).last());
            }
        }
        //extra robot we have
        if(_markAgentSize > markPoses.count()){
            count = 0;
            for(int i = markPoses.count() ; i < _markAgentSize; i++){
                if(markPoses.count() < _markAgentSize){
                    markRoles.append(QString("shotBlocker"));
                    markPoses.append(Vector2D(-1,  count *  4.5/ 6  * pow(-1, count) ));
                    markAngs.append(Vector2D(1,0));
                }
                count++;
            }
        }
    }
    else if(_markAgentSize < oppAgentsToMarkPos.count()){
        if(playOff || know->variables["transientFlag"].toBool() || gameState->isPlayOff()){
            QList<QPair<Vector2D, double> > tempsorted = sortdangerpassplayoff(oppAgentsToMarkPos);
            for(int i = 0; i<_markAgentSize; i++){
                markRoles.append(QString("shotBlocker"));
                if(!isInTheIndirectAreaShoot(tempsorted[i].first)){
                    markPoses.append(ShootBlockRatio(segmentpershoot, tempsorted[i].first).first());
                    markAngs.append(ShootBlockRatio(segmentpershoot, tempsorted[i].first).last());
                }
                else{
                    markPoses.append(indirectAvoidShoot(tempsorted[i].first).first());
                    markAngs.append(indirectAvoidShoot(tempsorted[i].first).last());
                }
            }
        }
        else if(playOn){
            QList<QPair<Vector2D, double> > tempsorted = sortdangerpassplayon(oppAgentsToMarkPos);
            for(int i = 0; i<_markAgentSize; i++){
                markRoles.append(QString("shotBlocker"));
                if(!isInTheIndirectAreaShoot(tempsorted[i].first)){
                    markPoses.append(ShootBlockRatio(segmentpershoot, tempsorted[i].first).first());
                    markAngs.append(ShootBlockRatio(segmentpershoot, tempsorted[i].first).last());
                }
                else{
                    markPoses.append(indirectAvoidShoot(tempsorted[i].first).first());
                    markAngs.append(indirectAvoidShoot(tempsorted[i].first).last());
                }
            }
        }
    }
    for(int i = 0 ; i < markPoses.size() ; i++){
        drawer->draw(markRoles.at(i) , markPoses.at(i) - Vector2D(0,0.4) , "white");
    }
    for(int i = 0 ; i < markPoses.count() ; i++){
        if(i < _markAgentSize){
            drawer->draw(Circle2D(markPoses[i], 0.1), QColor(Qt::white), true);
            DBUG(QString("%1 : x : %2, y : %3").arg(i).arg(markPoses[i].x).arg(markPoses[i].y),D_MAHI);
        }
    }
}

void DefensePlan::setGoalKeeperState(){
    //// In this function,we determine the specific states that goalkeeper must
    //// have a logical behavior by good conditions.In other word we have some
    //// exceptions mode for goalkeeper.These modes are :
    //// 1- ball is behind the goalkeeper,
    //// 2- One touch mode(goalkeeper diving mode) ,
    //// 3- Throwing out the ball from penalty area (Clear mode) ,
    //// 4- Ball is out of the field.

    ////////////////// Variables of this function //////////////////////////////
    dangerForGoalKeeperClear = false;
    dangerForInsideOfThePenaltyArea = false;
    dangerForGoalKeeperClearByOurAgents = false;
    dangerForGoalKeeperClearByOppAgents = false;
    isCrowdedInFrontOfPenaltyAreaByOppAgents = false;
    isCrowdedInFrontOfPenaltyAreaByOurAgents = false;
    playOnMode = gameState->isPlayOn();
    ////////////////////////////////////////////////////////////////////////////
    Rect2D ourLeftPole(wm->field->ourGoalL() + Vector2D(0.2 , 0.1) , wm->field->ourGoalL() - Vector2D(0 , 0.1));
    Rect2D ourRightPole(wm->field->ourGoalR() + Vector2D(0.2 , 0.1) , wm->field->ourGoalR() - Vector2D(0 , 0.1));
    Circle2D dangerCircle;
    Circle2D dangerCircle1;
    drawer->draw(ourLeftPole);
    drawer->draw(ourRightPole);
    if(goalKeeperAgent != nullptr){
        if(wm->field->isInField(wm->ball->pos)){
            ballIsOutOfField = false;
            QList<Vector2D> solutions;
            Segment2D ballLine(wm->ball->pos, wm->ball->pos + wm->ball->vel.norm()*10);
            Segment2D goalLine(wm->field->ourGoal() + Vector2D(0 , 0.8) , wm->field->ourGoal() - Vector2D(0 , 0.8));
            QList<Circle2D> defs;
            double AZBisecOpenAngle=0,AZBigestOpenAngle=0,AZDangerPercent=0;
            for(int g=0;g<defenseAgents.count();g++){
                defs.append(Circle2D(defenseAgents[g]->pos(), Robot::robot_radius_new));
            }
            know->getEmptyAngle(wm->ball->pos, wm->field->ourGoalL(), wm->field->ourGoalR(), defs, AZDangerPercent, AZBisecOpenAngle, AZBigestOpenAngle,false);
            /////////////////////// Added danger mode for not switching between "ball behindGoalie && danger mode /////////////
            solutions = wm->field->ourPAreaIntersect(Segment2D(wm->ball->pos , wm->field->ourGoal()));
            if(solutions.size()){
                if(solutions.size() == 1){
                    if(wm->field->isInField(solutions.at(0))){
                        dangerCircle = Circle2D(solutions.at(0), 0.40);
                        dangerCircle1 = Circle2D(solutions.at(0), 0.40);
                    }
                }
                else if(solutions.size() == 2){
                    dangerCircle = Circle2D(solutions.at(0).dist(wm->ball->pos) < solutions.at(1).dist(wm->ball->pos) ? solutions.at(0) : solutions.at(1), 0.40);
                    dangerCircle1 = Circle2D(solutions.at(0).dist(wm->ball->pos) < solutions.at(1).dist(wm->ball->pos) ? solutions.at(0) : solutions.at(1), 0.40);
                }
                if(wm->our.activeAgentsCount() > 0 || wm->opp.activeAgentsCount() > 0){
                    for(int i = 0; i < wm->our.activeAgentsCount() ; i++){
                        if(wm->our.active(i)->id != goalKeeperAgent->id()){
                            if(dangerCircle.contains(wm->our.active(i)->pos)){
                                isCrowdedInFrontOfPenaltyAreaByOurAgents = true;
                            }
                        }
                    }
                    for(int i = 0 ; i < wm->opp.activeAgentsCount() ; i++){
                        if(dangerCircle.contains(wm->opp.active(i)->pos)){
                            isCrowdedInFrontOfPenaltyAreaByOppAgents = true;
                        }
                    }
                }
                if(isCrowdedInFrontOfPenaltyAreaByOurAgents){
                    DBUG("Crowded" , D_AHZ);
                    if(dangerCircle.contains(wm->ball->pos)){
                        dangerForGoalKeeperClearByOurAgents = true;
                    }
                }
                if(isCrowdedInFrontOfPenaltyAreaByOppAgents){
                    if(dangerCircle1.contains(wm->ball->pos)){
                        dangerForGoalKeeperClearByOppAgents = true;
                    }
                }
                if(dangerForGoalKeeperClearByOurAgents || dangerForGoalKeeperClearByOppAgents){
                    dangerForGoalKeeperClear = true;
                }
            }
            ///////////////////////////////////////////////////////////////////////
            if(playOnMode && !dangerForGoalKeeperClear){
                if((wm->ball->vel.length() > 1.3) && (goalLine.intersection(ballLine).valid() || oneTouchCnt < 5)){
                    ballIsBesidePoles = false;
                    goalKeeperOneTouch = true;
                    goalKeeperClearMode = false;
                    ballIsOutOfField = false;
                    if(!goalLine.intersection(ballLine).valid()){
                        oneTouchCnt++;
                        return;
                    }
                    oneTouchCnt = 0;
                    return;
                }
                else if(wm->field->isInOurPenaltyArea(wm->ball->pos)){
                    if(ourLeftPole.contains(wm->ball->pos) || ourRightPole.contains(wm->ball->pos)){
                        ballIsBesidePoles = true;
                        goalKeeperOneTouch = false;
                        goalKeeperClearMode = false;
                        ballIsOutOfField = false;
                        return;
                    }
                    else{
                        ballIsBesidePoles = false;
                        goalKeeperOneTouch = false;
                        goalKeeperClearMode = true;
                        ballIsOutOfField = false;
                        return;
                    }
                }
                else{
                    goalKeeperClearMode = false;
                    goalKeeperOneTouch = false;
                    ballIsBesidePoles = false;
                    ballIsOutOfField = false;
                }
            }
        }
        else{
            ballIsBesidePoles = false;
            goalKeeperOneTouch = false;
            goalKeeperClearMode = false;
            ballIsOutOfField = true;
            return;
        }
    } else {
        drawer->draw("GoalKeeper is gone !!!!" , Vector2D(0,0) , "red");
    }
}

void DefensePlan::setGoalKeeperTargetPoint(){
    //// This function determine the target point that goalkeeper must go to it.
    //// For producing the target point, we certainly consider the states that
    //// is result from the "setGoalKeeperState" function.  :)

    ////////////////////////// Variables of this function //////////////////////
    Vector2D ballPos;
    Vector2D ballVel;
    Vector2D predictedBall;
    Vector2D oppPasser;
    Vector2D upBallRectanglePoint;
    Vector2D downBallRectanglePoint;
    Circle2D dangerCircle;
    Circle2D dangerCircle1;
    Vector2D goalKeeperTargetOffSet = Vector2D(0.2 , 0.0);
    QList<Vector2D> tempSol;
    QList<Vector2D> ballRectanglePoints;
    QList<Vector2D> solutions;
    dangerForGoalKeeperClear = false;
    dangerForInsideOfThePenaltyArea = false;
    dangerForGoalKeeperClearByOurAgents = false;
    dangerForGoalKeeperClearByOppAgents = false;
    isCrowdedInFrontOfPenaltyAreaByOppAgents = false;
    isCrowdedInFrontOfPenaltyAreaByOurAgents = false;
    playOffMode = gameState->theirDirectKick() || gameState->theirIndirectKick();
    playOnMode = gameState->isPlayOn();
    stopMode = gameState->isPlayOff();
    tempSol.clear();
    ballRectanglePoints.clear();
    ///////////////////////////////////////////////////////////////////////////
    if(goalKeeperAgent != nullptr){
        ballPos = wm->ball->pos;
        ballVel = wm->ball->vel;
        predictedBall = ballPos + ballVel;
        solutions = wm->field->ourPAreaIntersect(Line2D(wm->ball->pos , wm->field->ourGoal()));
        if(solutions.size()){
            if(solutions.size() == 1){
                if(wm->field->isInField(solutions.at(0))){
                    dangerCircle = Circle2D(solutions.at(0), 0.40);
                    dangerCircle1 = Circle2D(solutions.at(0), 0.40);
                }
            }
            else if(solutions.size() == 2){
                dangerCircle = Circle2D(solutions.at(0).dist(wm->ball->pos) < solutions.at(1).dist(wm->ball->pos) ? solutions.at(0) : solutions.at(1), 0.40);
                dangerCircle1 = Circle2D(solutions.at(0).dist(wm->ball->pos) < solutions.at(1).dist(wm->ball->pos) ? solutions.at(0) : solutions.at(1), 0.40);
            }
        }
        drawer->draw(dangerCircle , "yellow");
        drawer->draw(dangerCircle1 , "yellow");
        if(ballIsOutOfField || stopMode){
            lastStateForGoalKeeper = QString("noBesidePoleMode");
            dangerForGoalKeeperClear = false;
            drawer->draw(QString("Ball Is Out Of Field"), Vector2D(0,1),"red");
            goalKeeperTarget = wm->field->ourGoal() + goalKeeperTargetOffSet;
            return;
        }
        else if(playOffMode){
            lastStateForGoalKeeper = QString("noBesidePoleMode");
            dangerForGoalKeeperClear = false;
            DBUG(QString("Their Indirect") , D_AHZ);
            oppPasser = wm->opp[know->nearestOppToBall()]->pos; //todo: move to wm
            if(gameState->theirIndirectKick()){
                goalKeeperTarget = wm->field->ourGoal() + goalKeeperTargetOffSet;
            }
            else{
                goalKeeperTarget = strictFollowBall(wm->ball->pos);
            }
            return;
        }
        else if(know->variables["transientFlag"].toBool()){
            lastStateForGoalKeeper = QString("noBesidePoleMode");
            dangerForGoalKeeperClear = false;
            DBUG(QString("TS Mode") , D_AHZ);
            Line2D ballLine(wm->ball->pos , wm->ball->pos + wm->ball->vel*10);
            //draw(ballLine);
            //            tempSol.append(wm->field->AHZOurPAreaIntersectForGoalKeeper(ballLine)); // TODO : AHZ
            //            if(tempSol.size()){
            //                if(tempSol.size() == 2){
            //                    tempGoalKeeperTarget = tempSol.at(0).dist(oppPasser) > tempSol.at(1).dist(oppPasser) ? tempSol.at(0) : tempSol.at(1);
            //                    if(CSkillGotoPointAvoid::timeNeeded(knowledge->goalie , tempGoalKeeperTarget , conf()->BangBang_VelMax(),ourRelax , oppRelax,0,0.2,1) <
            //                            wm->ball->vel.length() / wm->ball->pos.dist(tempGoalKeeperTarget)){
            //                        goalKeeperPredictionModeInPlayOff = true;
            //                        goalKeeperTarget = tempGoalKeeperTarget;
            //                    }
            //                }
            //                else{
            //                    goalKeeperPredictionModeInPlayOff = false;
            //                    goalKeeperTarget = know->getPointInDirection(wm->field->ourGoal() , ballPrediction(true) , 0.5);
            //                    if(!wm->field->isInOurPenaltyArea(goalKeeperTarget)){
            //                        tempSol.append(wm->field->AHZOurPAreaIntersectForGoalKeeper(Segment2D(goalKeeperTarget , wm->field->ourGoal())));
            //                        if(tempSol.size() == 1){
            //                            goalKeeperTarget = tempSol.at(0);
            //                        }
            //                        else if(tempSol.size() == 2){
            //                            goalKeeperTarget = tempSol.at(0).dist(wm->ball->pos) < tempSol.at(1).dist(wm->ball->pos) ? tempSol.at(0) : tempSol.at(1);
            //                        }
            //                    }
            //                }
            //                draw(goalKeeperTarget);
            //            }
            //            else{
            goalKeeperPredictionModeInPlayOff = false;
            goalKeeperTarget = know->getPointInDirection(wm->field->ourGoal() , ballPrediction(true) , 0.5);
            if(!wm->field->isInOurPenaltyArea(goalKeeperTarget)){
                tempSol = wm->field->ourPAreaIntersect(Segment2D(goalKeeperTarget , wm->field->ourGoal()));
                if(tempSol.size() == 1){
                    goalKeeperTarget = tempSol.at(0);
                }
                else if(tempSol.size() == 2){
                    goalKeeperTarget = tempSol.at(0).dist(wm->ball->pos) < tempSol.at(1).dist(wm->ball->pos) ? tempSol.at(0) : tempSol.at(1);
                }
            }
            //            }
        }
        else if(goalKeeperOneTouch){
            lastStateForGoalKeeper = QString("noBesidePoleMode");
            Segment2D ballLine(ballPos, ballPos + ballVel.norm()*10);
            goalKeeperTarget = ballLine.nearestPoint(goalKeeperAgent->pos());
            DBUG(QString("OneTouch To Side Point"), D_AHZ);
            return;
        }
        else if(goalKeeperClearMode){
            lastStateForGoalKeeper = QString("noBesidePoleMode");
            ////////////// Danger Mode for inside of the penalty area///////////
            if(wm->our.activeAgentsCount() > 0 || wm->opp.activeAgentsCount() > 0){
                for(int i = 0; i < wm->our.activeAgentsCount() ; i++){
                    if(wm->our.active(i)->id != goalKeeperAgent->id()){
                        if(dangerCircle.contains(wm->our.active(i)->pos)){
                            isCrowdedInFrontOfPenaltyAreaByOurAgents = true;
                        }
                    }
                }
                for(int i = 0 ; i < wm->opp.activeAgentsCount() ; i++){
                    if(dangerCircle.contains(wm->opp.active(i)->pos)){
                        isCrowdedInFrontOfPenaltyAreaByOppAgents = true;
                    }
                }
            }
            if(isCrowdedInFrontOfPenaltyAreaByOurAgents){
                DBUG("Crowded" , D_AHZ);
                if(dangerCircle1.contains(wm->ball->pos)){
                    dangerForGoalKeeperClearByOurAgents = true;
                }
            }
            if(isCrowdedInFrontOfPenaltyAreaByOppAgents){
                if(dangerCircle1.contains(wm->ball->pos)){
                    dangerForGoalKeeperClearByOppAgents = true;
                }
            }
            if(dangerForGoalKeeperClearByOurAgents || dangerForGoalKeeperClearByOppAgents){
                dangerForGoalKeeperClear = true;
            }
            if(dangerForGoalKeeperClear){
                dangerForInsideOfThePenaltyArea = true;
                DBUG(QString("inside : %1").arg(dangerForInsideOfThePenaltyArea) , D_AHZ);
                if(dangerForGoalKeeperClearByOppAgents){
                    //                     goalKeeperTarget =  know->getPointInDirection(wm->ball->pos , wm->field->ourGoal() ,0.2);
                }
                else if(dangerForGoalKeeperClearByOurAgents){
                    DBUG("danger" , D_AHZ);
                    //                    penaltyArea.intersection(Line2D(wm->ball->pos , wm->field->ourGoal()),&Solutions[0] , &Solutions[1]);
                    //                    goalieTarget = Solutions[0].dist(wm->ball->pos) < Solutions[1].dist(wm->ball->pos) ? Solutions[0] : Solutions[1];
                    //                    dangerIntersection.append(wm->field->ourPAreaIntersect(Line2D(Vector2D(wm->ball->pos.x , -3) , Vector2D(wm->ball->pos.x , 3))));
                    //                    if(!dangerIntersection.isEmpty()){
                    //                        if(dangerIntersection.size() == 2){
                    //                            goalieTarget = dangerIntersection.at(0).isValid()
                    //                                    && (dangerIntersection.at(0).dist(wm->ball->pos) < dangerIntersection.at(1).dist(wm->ball->pos)) ? dangerIntersection.at(0) : dangerIntersection.at(1);
                    //                            goalieDirection = wm->ball->pos - goalieTarget;
                    //                            goalieTarget =  know->getPointInDirection(wm->ball->pos , wm->field->ourGoal() ,0.2);

                    //                        }
                    //                        else if(dangerIntersection.size() == 1){
                    //                            goalieTarget = dangerIntersection.at(0);
                    //                        }
                    //                    }
                }
            }
            /////////////// End of Danger Mode ///////////////////////////////
            else{
                drawer->draw(QString("Clear"), Vector2D(0,1),"red");
            }
            return;
        }
        else if(ballIsBesidePoles){ // TODO : AHZ
            Rect2D ballRectangle(wm->ball->pos + Vector2D(0.25 , 0.25) , wm->ball->pos + Vector2D(-0.25 , -0.25));
            drawer->draw(ballRectangle);
            if(wm->field->isInField(ballRectangle.topLeft())){
                ballRectanglePoints.append(ballRectangle.topLeft());
            }
            if(wm->field->isInField(ballRectangle.topRight())){
                ballRectanglePoints.append(ballRectangle.topRight());
            }
            if(wm->field->isInField(ballRectangle.bottomLeft())){
                ballRectanglePoints.append(ballRectangle.bottomLeft());
            }
            if(wm->field->isInField(ballRectangle.bottomRight())){
                ballRectanglePoints.append(ballRectangle.bottomRight());
            }
            if(lastStateForGoalKeeper == QString("no")){
                isPermissionToKick = false;
                goalKeeperTarget = ballRectanglePoints.at(0).dist(goalKeeperAgent->pos()) < ballRectanglePoints.at(1).dist(goalKeeperAgent->pos()) ? ballRectanglePoints.at(0) : ballRectanglePoints.at(1);
            }
            if(goalKeeperAgent->pos().dist(goalKeeperTarget) < 0.05){
                if(wm->ball->pos.y > 0){
                    if(ballRectanglePoints.at(0).y < ballRectanglePoints.at(1).y){
                        downBallRectanglePoint = ballRectanglePoints.at(0);
                        upBallRectanglePoint = ballRectanglePoints.at(1);
                    }
                    else{
                        downBallRectanglePoint = ballRectanglePoints.at(1);
                        upBallRectanglePoint = ballRectanglePoints.at(0);
                    }
                    if(goalKeeperTarget != downBallRectanglePoint){
                        goalKeeperTarget = downBallRectanglePoint;
                    }
                    if(goalKeeperAgent->pos().dist(goalKeeperTarget) < 0.05){
                        isPermissionToKick = true;
                    }
                }
                else{
                    if(ballRectanglePoints.at(0).y < ballRectanglePoints.at(1).y){
                        upBallRectanglePoint = ballRectanglePoints.at(1);
                        downBallRectanglePoint = ballRectanglePoints.at(0);
                    }
                    else{
                        upBallRectanglePoint = ballRectanglePoints.at(0);
                        downBallRectanglePoint = ballRectanglePoints.at(1);
                    }
                    if(goalKeeperTarget != upBallRectanglePoint){
                        goalKeeperTarget = upBallRectanglePoint;
                    }
                    if(goalKeeperAgent->pos().dist(goalKeeperTarget) < 0.05){
                        isPermissionToKick = true;
                    }
                }
            }

            lastStateForGoalKeeper = QString("ballIsBesidePoles");
            drawer->draw(goalKeeperTarget);
        }
        else{
            lastStateForGoalKeeper = QString("noBesidePoleMode");
            ////////////// Danger Mode for out of the penalty area /////////////
            if(wm->our.activeAgentsCount() > 0 || wm->opp.activeAgentsCount() > 0){
                for(int i = 0 ; i < wm->opp.activeAgentsCount()  ; i++){
                    if(dangerCircle.contains(wm->opp.active(i)->pos)){
                        isCrowdedInFrontOfPenaltyAreaByOppAgents = true;
                    }
                }
                for(int i = 0; i < wm->our.activeAgentsCount() ; i++){
                    if(wm->our.active(i)->id != goalKeeperAgent->id()){
                        if(dangerCircle.contains(wm->our.active(i)->pos)){
                            isCrowdedInFrontOfPenaltyAreaByOurAgents = true;
                        }
                    }
                }
            }
            if(isCrowdedInFrontOfPenaltyAreaByOurAgents){
                if(dangerCircle.contains(wm->ball->pos)){
                    dangerForGoalKeeperClear = true;
                }
            }
            if(isCrowdedInFrontOfPenaltyAreaByOppAgents){
                if(dangerCircle.contains(wm->ball->pos)){
                    dangerForGoalKeeperClearByOppAgents = true;
                }
            }
            if(dangerForGoalKeeperClearByOurAgents || dangerForGoalKeeperClearByOppAgents){
                dangerForGoalKeeperClear = true;
            }
            if(dangerForGoalKeeperClear){
                DBUG("danger" , D_AHZ);
                goalKeeperTarget = know->getPointInDirection(wm->ball->pos , wm->field->ourGoal() , 0.15);
                if(!wm->field->isInOurPenaltyArea(goalKeeperTarget)){
                    solutions = wm->field->ourPAreaIntersect(Line2D(wm->ball->pos , wm->field->ourGoal()));
                    if(solutions.size()){
                        if(solutions.size() == 1){
                            goalKeeperTarget = solutions.at(0);
                        }
                        else if(solutions.size() == 2){
                            goalKeeperTarget = solutions.at(0).dist(wm->ball->pos) < solutions.at(1).dist(wm->ball->pos) ? solutions.at(0) : solutions.at(1);
                        }
                    }
                }
            }
            //////////////// End of Danger Mode ////////////////////////////////
            else{
                DBUG(QString("strict follow"), D_AHZ);
                predictedBall = ballPrediction(true);
                if(predictedBall.x - 0.02 < goalKeeperAgent->pos().x){
                    Segment2D ball2PredictedBall(ballPos,predictedBall);
                    Line2D robotPrGoalLine(goalKeeperAgent->pos(), Vector2D(goalKeeperAgent->pos().x,(goalKeeperAgent->pos().y + 0.01)));
                    if(ball2PredictedBall.intersection(robotPrGoalLine).valid()){
                        predictedBall = ball2PredictedBall.intersection(robotPrGoalLine);
                    }
                }
                goalKeeperTarget = strictFollowBall(predictedBall);
            }
        }
    }
}

void DefensePlan::assignSkill(Agent *_agent , Action *_skill){
    //// For run any skills,for example: kick , we must initialize
    //// some main variables that are effective to run a skill.
    //// This function is called anywhere we need to run a skill.

    _agent->action = _skill;
}

void DefensePlan::initGoalKeeper(Agent *_goalieAgent){
    //// This function determines the goalkeeper agent. Actually this function
    //// is used in Coach.cpp &&

    goalKeeperAgent = _goalieAgent;
    agents.clear();
    if(_goalieAgent){
        agents.append(_goalieAgent);
    }
}

void DefensePlan::initDefense(QList <Agent*> _defenseAgents){
    //// Just like the "initGoalKeeper" function , for inializing the defense
    //// agent , we have a function like this. :)

    defenseAgents.clear();
    defenseAgents.append(_defenseAgents);
    agents.append(_defenseAgents);
}

DefensePlan::DefensePlan()
{
    //// Constructor function of DefensePlan class

    goalieThr = 0.0;

    thr = 0;
    isOnetouch = false;
    goalieClearFlag = false;
    inPenaltyAreaFlag = false;
    behindAgent = false;
    noDefThr = 0;
    chipGKCounter = 0;

    clearCnt = 0;

    defenseCount = defenseAgents.size();

    defExceptions.active = false;
    defExceptions.exeptionMode = NoneExep;
    defExceptions.exepAgentId = -1;

    defClearThr = 0;
    defClearFlag = false;

    overDefThr = 0;

    goalKeeperOneTouch = false;
    goalKeeperClearMode = false;
    ballIsOutOfField = false;

    ballIsBesidePoles = false;

    oneTouchCnt = 5;
    markRadius = 1.6;
    markRadiusStrict = 1.39;
    segmentpershoot = conf.ShootRatioBlock / 100.0;
    segmentperpass = (100  - conf.PassRatioBlock) / 100.0;
    dir  = Vector2D(1,0);
    MantoManAllTransientFlag =  conf.ManToManAllTransiant;
    predictThresh = 0;
    isInOneTouch = false;
    oneTouchCycleTest = 0;
    cycleCounter = 0;
    timeToReach = 0;
    doBlockPass = false;
    goalieAreaHis = 0;
    isBallGoingToOppAreaCnt = -1;
    pushBallHist = 0;
    failureAtempCnt = 0;
    goaliePassBlockCnt = -1;
    GOTThresh = 0.0;
    GOTCounter = 0;
    lastClearID = -1;
    lastTouchTheGoalie = -1;
    lastStateOffPlay = -1;
    oneToucher = 0;
    firstDefenseKickLine = -1;
    secondDefenseKickLine = 0;
    goalieKickThreshold = 70;
    /////////// AHZ //////////////
    lastMarkRoles.append(markRoles);
    goalKeeperTarget = Vector2D(0,0);
    dangerModeThresholdForClear = false;
    dangerModeThresholdForDanger = false;
    /////////////// For Adding TS Mode in Mark ///////////////////////////////
    xLimitForblockingPass = 0;
    manToManMarkBlockPassFlag = conf.PlayOffManToMan;
    if(manToManMarkBlockPassFlag || wm->ball->pos.x > xLimitForblockingPass){
        know->variables["lastStateForMark"] = QString("BlockPass");
        know->variables["stateForMark"] = QString("BlockPass");
    }
    else{
        know->variables["lastStateForMark"] = QString("BlockShot");
        know->variables["stateForMark"] = QString("BlockShot");
    }
    ////////////////////////////////

    striker_Robot = new GotopointavoidAction;

    for (int i = 0; i < _MAX_NUM_PLAYERS; i++){
        lastMarker[i] = -1;

        gpa[i] = new GotopointavoidAction;
        gpa[i]->setNoavoid(true);
        gpa[i]->setOnetouchmode(true);

        gps[i] = new GotopointAction;
        gps[i]->setSlowmode(false);
    }
    kickSkill = new KickAction;
    AHZSkills = nullptr;
    upper_player = 0;
    tooFarDiffAngle = 30;
    isDefenseUpperThanGoalie = true;
    defenderForMark = false;
    doubleMarking = false;

}

void DefensePlan::preCalculate(){
    //// This function initializes some variables from GUI &&
    //// sets some properties of kick && chip skill.
    //// Actually this function is used in "execute()" function , before any
    //// work that will do.

    stopMode = gameState->isPlayOff();

    announceClearing(false);
    isItPossibleToClear = true;

    if(conf.NoClear){
        isItPossibleToClear = false;
    }
    for(int i = 0; i < count(); i++)
    {
        float dTheta = (float) (wm->ball->pos - agent(i)->pos()).th().degree();
        if(gameState->isPlayOn()
                && (wm->ball->pos.dist(agent(i)->pos()) < 1.0)
                && (fabs(dTheta) < goalieKickThreshold)){
            // todo : robot Command moved
            //            agent(i)->setKick(agent(i)->kickSpeedValue(7.2 , false));
            //            agent(i)->setChip(LONG_CHIP_POWER);
        }
        else{
            //            agent(i)->setKick(0);
            //            agent(i)->setChip(0);
        }
    }
    for (auto &i : gpa) {
        if(stopMode){
            i->setSlowmode(true);
            i->setNoavoid(false);
            i->setAvoidpenaltyarea(false);
            i->setAvoidcentercircle(false);
        }
        else{
            i->setSlowmode(false);
            i->setNoavoid(true);
        }
    }
}

void DefensePlan::matchingDefPos(int _defenseNum){
    //// This Function matches the points that is produced by other functions to
    //// our agents we have in defense plan. Then we run the "GotoPointAvoid"
    //// skill on the agents.

    QList <Agent*> ourAgents;
    QList <Vector2D> matchPoints;
    QList <Vector2D> stuckPositions;
    QList <int> stuckIndexs;
    QList <int> matchResult;
    Vector2D tempPoint;
    stopMode = gameState->isPlayOff();
    ourAgents.clear();
    ourAgents.append(defenseAgents);
    if(defExceptions.active){
        if(defExceptions.exepAgentId != -1){
            DBUG(QString("id : %1 ").arg(defExceptions.exepAgentId) , D_AHZ);
            //ourAgents.removeOne(knowledge->getAgent(defExceptions.exepAgentId));
            for(int i = 0; i<ourAgents.length(); i++){
                if(i == defExceptions.exepAgentId)
                    ourAgents.removeAt(i);
            }
        }
    }
    for(int i = 0; i<ourAgents.length(); i++){
        if (i == defExceptions.exepAgentId)
            ourAgents.removeAt(i);
    }
    ///////////////// Added By AHZ for segment (before MRL game) ///////////////
    if(stopMode){
        ourAgents.clear();
        ourAgents.append(defenseAgents);
    }
    //////////////////////////////////////////////////////////////////////
    matchPoints.clear();
    for(int i = 0 ; i < _defenseNum ; i++) {
        drawer->draw(tempDefPos.pos[i],QColor(Qt::blue));
        matchPoints.append(tempDefPos.pos[i]);
    }
    findOppAgentsToMark();
    findPos(decideNumOfMarks());
    matchPoints.append(markPoses);
    /////////////// Stucking agents ///////////////////////////////////////////
    if(isAgentsStuckTogether(matchPoints)){
        agentsStuckTogether(matchPoints , stuckPositions , stuckIndexs);
        DBUG("Agents Stuck together" , D_AHZ);
        DBUG(QString("stuck position: %1").arg(stuckPositions.size()),  D_AHZ);
        DBUG(QString("stuck index: %1").arg(stuckIndexs.size()),  D_AHZ);
        correctingTheAgentsAreStuckTogether(matchPoints , stuckPositions , stuckIndexs);
    }
    //////////////////// Added for RC 2017 /////////////////////////////////////
    Vector2D tempMatchPoints[matchPoints.size()];
    if(ourAgents.size() > matchPoints.size()){
        for(int i = 0 ; i < ourAgents.size() - matchPoints.size() ; i++){
            ourAgents.removeAt(i);
        }
    }
    else if(ourAgents.size() < matchPoints.size()){
        for(int i = 0 ; i < matchPoints.size() ; i++){
            tempMatchPoints[i] = matchPoints.at(i);
        }
        for(int i = 0 ; i < matchPoints.size() ; i++){
            for(int j = 0 ; j < matchPoints.size() ; j++){
                if(i != j){
                    if(tempMatchPoints[i].x > tempMatchPoints[j].x){
                        tempPoint = tempMatchPoints[i];
                        tempMatchPoints[i] = tempMatchPoints[j];
                        tempMatchPoints[j] = tempPoint;
                    }
                }
            }
        }
        for(int i = matchPoints.size() - ourAgents.size() - 1 ; i >= 0 ; i--){
            matchPoints.removeOne(tempMatchPoints[i]);
        }
    }
    ////////////////////////////////////////////////////////////////////////////
    know->Matching(ourAgents,matchPoints,matchResult);
    DBUG(QString("defenseAHZ : %1 ").arg(defenseAgents.size()) , D_AHZ);
    for(int i = 0 ; i < defenseCount && i < matchPoints.size(); i++){
        defensePoints[i] = matchPoints[i];
    }
    for(int i = 0 ; i < matchPoints.count() && i < matchResult.count() ; i++){
        gpa[ourAgents[i]->id()]->clearOurrelax();
        gpa[ourAgents[i]->id()]->clearTheirrelax();


        for(int j = 0; j < ourAgents.size(); j++){
            if(j != i){
                gpa[ourAgents[i]->id()]->addOurrelax(ourAgents[j]->id());//TODO: gotopiontaction
            }
        }
        assignSkill(ourAgents[i] , gpa[ourAgents[i]->id()]);
        if(ourAgents[i]->pos().dist(matchPoints[matchResult[i]]) > 0.35){
            matchPoints[matchResult[i]] = checkDefensePoint(ourAgents[i], matchPoints[matchResult[i]]);
        }
        drawer->draw(Circle2D(matchPoints[matchResult[i]] , 0.05) , 0 , 360 , "black" , true);
        gpa[ourAgents[i]->id()]->setNoavoid(true);
        gpa[ourAgents[i]->id()]->setSlowmode(false);
        gpa[ourAgents[i]->id()]->setAvoidpenaltyarea(false);
        gpa[ourAgents[i]->id()]->setBallobstacleradius(0);
        if(gameState->theirIndirectKick()){
            gpa[ourAgents[i]->id()]->setNoavoid(false);
            gpa[ourAgents[i]->id()]->setSlowmode(false);
            gpa[ourAgents[i]->id()]->setAvoidpenaltyarea(false);
            gpa[ourAgents[i]->id()]->setBallobstacleradius(0);
        }
        if(gameState->theirIndirectKick()){
            gpa[ourAgents[i]->id()]->setNoavoid(false);
            gpa[ourAgents[i]->id()]->setSlowmode(false);
            gpa[ourAgents[i]->id()]->setAvoidpenaltyarea(false);
            gpa[ourAgents[i]->id()]->setBallobstacleradius(0.5);
        }
        else if(stopMode){
            gpa[ourAgents[i]->id()]->setNoavoid(false);
            gpa[ourAgents[i]->id()]->setSlowmode(true);
            gpa[ourAgents[i]->id()]->setDivemode(false);
        }
        //////////// Go To Point Avoid for defense agents //////////////////
        if(i < _defenseNum){
            gpa[ourAgents[i]->id()]->setTargetpos(matchPoints[matchResult[i]]); //HINT : gpa->init
            gpa[ourAgents[i]->id()]->setTargetdir(matchPoints[matchResult[i]] - wm->field->ourGoal());
        }
        ///////// Go To Point Avoid for mark agents ////////////////////
        else{
            gpa[ourAgents[i]->id()]->setTargetpos(matchPoints[matchResult[i]]); //HINT : gpa->init
            gpa[ourAgents[i]->id()]->setTargetdir(markAngs.at(i - _defenseNum));
        }
    }
}

void DefensePlan::execute(){
    ///// All of the goalKeeper && defense functions are linked in this function.
    ///// First of all, we determine the behavior of goalKeeper.
    ///// (first in penalty mode then other mode)
    ///// Now,we identify the number of defense agents && then this number is
    ///// sent to "matchingDefPos()" function to match between the produced
    ///// points && our agents in defense plan.
    int realDefSize = 0;
    DBUG(QString("defense agents size %1").arg(defenseAgents.size()), D_HAMED);
    ////////////initialize////////////////
    initVars();
    preCalculate();
    DBUG(QString("valid point: %1").arg(isValidPoint(wm->ball->pos)) , D_AHZ);
    ballPosHistory.prepend(Vector2D(wm->ball->pos.x, wm->ball->pos.y));
    Rect2D rect(midGoal + Vector2D(0.0, defenseAreaLine.length() / 2.0), defenseAreaBottomCircle.radius(), defenseAreaLine.length());
    drawer->draw(rect , "yellow");
    if(ballPosHistory.count() > 7){
        ballPosHistory.removeLast();
    }
    //////////////////////////////////////
    playOnMode = gameState->isPlayOn(); // knowledge->isStart();
    DBUG(QString("defense oneTouch mode : %1").arg(know->variables["defenseOneTouchMode"].toBool()) , D_AHZ);
    DBUG(QString("defense clear mode : %1").arg(know->variables["defenseClearMode"].toBool()) , D_AHZ);
    if(gameState->theirPenaltyKick() && !gameState->penaltyShootout()){
        if(goalKeeperAgent != nullptr){
            drawer->draw(QString("Penalty") , Vector2D(1,2) , "white");
            penaltyMode();
        }
        else{
            drawer->draw(QString("No Goalie!") , Vector2D(1,2) , "white");
        }
        return;
    }
    if(gameState->theirPenaltyKick()){
        //TO DO: add penalty goalie for penalty shootout
        penaltyShootOutMode();
        lastBallPosition = wm->ball->pos;
        return;
    }
    if (gameState->isPlayOn()/*knowledge->getGameMode() == CKnowledge::Start*/ && gameState->penaltyShootout()) {
        penaltyShootOutMode();
    }
    else{
        if(goalKeeperAgent != nullptr){
            setGoalKeeperState();
            setGoalKeeperTargetPoint();
            executeGoalKeeper();
            assignSkill(goalKeeperAgent , AHZSkills);
        }
        if(!defenseAgents.empty()){
            if(wm->our.activeAgentsCount() < 7){
                if(playOnMode){
                    checkDefenseExeptions();
                    if(defExceptions.active && !know->variables["transientFlag"].toBool()){
                        runDefenseExeptions();
                        defenseCount = defenseAgents.size() - 1;
                    }
                    else{
                        defExceptions.exepAgentId = -1;
                        defExceptions.exeptionMode = NoneExep;
                        defenseCount = defenseAgents.size();
                        know->variables["defenseClearMode"] = false;
                        know->variables["defenseOneTouchMode"] = false;
                    }
                }
                else{
                    know->variables["defenseClearMode"] = false;
                    know->variables["defenseOneTouchMode"] = false;
                    defenseCount = defenseAgents.size();
                    DBUG(QString("defense count : %1").arg(defenseCount) , D_AHZ);
                }
                if(defenseCount > 0){
                    realDefSize = defenseCount - decideNumOfMarks();
                    tempDefPos = defPos.getDefPositions(ballPrediction(false), realDefSize, 1.5, 2.5);
                    DBUG(QString("real def size : %1").arg(realDefSize) , D_AHZ);
                    matchingDefPos(realDefSize);
                }
            }
            else{
                drawer->draw("Vision Problem", Vector2D(0,0),"red");
            }
        }
    }
}

bool DefensePlan::agentEffectOnBallProbability(Vector2D ballPos, Vector2D ballVel, Vector2D agentPos, Vector2D agentVel, bool isTowardOurgoal){
    Vector2D goal;
    bool isInPenaltyRect;

    if(isTowardOurgoal){
        goal = wm->field->ourGoal();
        isInPenaltyRect = wm->field->isInOurPenaltyArea(ballPos);
    }
    else{
        goal = wm->field->oppGoal();
        isInPenaltyRect = wm->field->isInOppPenaltyArea(ballPos);
    }


    if(ballVel.length() > 4 || ballPos.dist(goal) < 3){
        agentEffectOnBallProbabilityRes = true;     // skyDive
    }
    else if(ballVel.length() < 2 || ballPos.dist(agentPos) > 1){
        agentEffectOnBallProbabilityRes = false;    // ballBisector
    }

    return agentEffectOnBallProbabilityRes;
}

Vector2D DefensePlan::getGoalieShootOutTarget(bool isSkyDive){
    Vector2D degree;
    Vector2D finalTarget;

    Line2D ballPath(wm->ball->pos , wm->ball->pos + (wm->ball->vel.norm()*10));
    //    Line2D ballLine(lastBallPos.first(), lastBallPos.last());
    //    Line2D ballRay(wm->ball->pos, wm->ball->pos + wm->opp[knowledge->nearestOppToBall]->dir);
    Line2D oppAgentLine(wm->opp[know->nearestOppToBall()]->pos,wm->opp[know->nearestOppToBall()]->pos+wm->opp[know->nearestOppToBall()]->dir*10);

    Vector2D a,b;
    Circle2D c1 = Circle2D(wm->field->ourGoal(),1);

    if(!isSkyDive){
        degree=(wm->field->ourGoalL()-(wm->ball->pos+0.2*wm->ball->vel)).norm()
                +(wm->field->ourGoalR()-(wm->ball->pos+0.2*wm->ball->vel)).norm();

        Line2D bisectorLine(wm->ball->pos+0.2*wm->ball->vel,wm->ball->pos+degree*10);

        if(know->chipGoalPropability(false, goalKeeperAgent->pos()) > 0.1 || know->chipGoalPropability(false, goalKeeperAgent->pos()) < 0.05)
            shootOutDiam = min(2*wm->ball->pos.dist(wm->field->ourGoal())/5.0, 2);

        DBUG(QString("ballBisector, diam:%1").arg(shootOutDiam), D_FATEME);
        Circle2D c = Circle2D(wm->field->ourGoal(),shootOutDiam);

        if (c.intersection(bisectorLine, &a, &b)) {
            finalTarget= (a.x > b.x) ? a : b;
        }

        drawer->draw(finalTarget, QColor(Qt::cyan));
    }
    else{
        if(wm->ball->vel.length() > 4
                || (wm->ball->pos.dist(wm->field->ourGoal()) < 2.7
                    && wm->ball->pos.dist(wm->opp[know->nearestOppToBall()]->pos) > 0.15)
                || wm->ball->pos.dist(wm->field->ourGoal()) < 1.4){
            DBUG("skydive, ballpath", D_FATEME);
            finalTarget = ballPath.perpendicular(wm->our[goalKeeperAgent->id()]->pos).intersection(ballPath);
        }else{
            finalTarget = oppAgentLine.perpendicular(wm->our[goalKeeperAgent->id()]->pos).intersection(oppAgentLine);
            DBUG("skydive, oppAgentLine", D_FATEME);
        }

        if(!wm->field->isInOurPenaltyArea(finalTarget)){
            c1.intersection(oppAgentLine, &a, &b);
            finalTarget= (a.x > b.x) ? a : b;
        }

        drawer->draw(finalTarget, "blue");
    }
    return finalTarget;
}

bool DefensePlan::canReachToBall(int ourAgentId, int theirAgentId){
    if(wm->our[ourAgentId] == nullptr
            || wm->opp[theirAgentId]== nullptr)
        return false;
    Vector2D ballPosAndVel;
    ballPosAndVel = wm->ball->pos+wm->ball->vel;

    return wm->our[ourAgentId]->pos.dist(ballPosAndVel) < wm->opp[theirAgentId]->pos.dist(ballPosAndVel) - 0.3
            && wm->ball->pos.dist(wm->field->ourGoal()) > 2.5 && wm->ball->pos.dist(wm->opp[theirAgentId]->pos) > 1;
}

int DefensePlan::decideShootOutMode(){
    if(goalKeeperAgent== nullptr)
        return 0;
    int result;


    if(lastBallPosition.dist(wm->ball->pos) < 0.04){
        DBUG("beforeTouch", D_FATEME);
        shootOutClearModeSelected = false;
        result = beforeTouch;
    }
    else if(canReachToBall(goalKeeperAgent->id(), know->nearestOppToBall())
            || (!Circle2D(wm->ball->pos,0.10).contains(wm->opp[know->nearestOppToBall()]->pos)
                && wm->ball->pos.dist(wm->field->ourGoal()) < 1.7 )
            || shootOutClearModeSelected
            ){
        DBUG("shootOutClear", D_FATEME);
        shootOutClearModeSelected = true;
        result = shootOutClear;
    }
    else if(!agentEffectOnBallProbability(wm->ball->pos, wm->ball->vel, wm->opp[know->nearestOppToBall()]->pos, wm->opp[know->nearestOppToBall()]->vel, true)){
        DBUG("ballBisector", D_FATEME);
        result = ballBisector;
    }
    else{
        DBUG("skydive", D_FATEME);
        result = skyDive;
    }

    return result;
}

void DefensePlan::penaltyShootOutMode(){
    if(goalKeeperAgent == nullptr)
    {
        return;
    }
    penaltyShootoutMode = decideShootOutMode();

    Vector2D targetDir(10, 5), agentTarget;
    targetDir=wm->opp[know->nearestOppToBall()]->pos - wm->field->ourGoal();

    if(lastBallPos.count() < 15){
        lastBallPos.append(wm->ball->pos);
    }
    else{
        lastBallPos.removeFirst();
    }


    switch(penaltyShootoutMode){
    case beforeTouch:
        assignSkill(goalKeeperAgent , gpa[goalKeeperAgent->id()]);
        gpa[goalKeeperAgent->id()]->setSlowmode(false);
        gpa[goalKeeperAgent->id()]->setDivemode(true);
        gpa[goalKeeperAgent->id()]->setTargetpos(wm->field->ourGoal()+Vector2D(0.1,0) );
        gpa[goalKeeperAgent->id()]->setTargetdir(targetDir);

        break;

    case shootOutClear:
        assignSkill(goalKeeperAgent, kickSkill);
        kickSkill->setKickspeed(1023);
        kickSkill->setTolerance(50);
        kickSkill->setDontkick(false);
        kickSkill->setSlow(false);
        kickSkill->setSpin(false);
        kickSkill->setAvoidpenaltyarea(false);
        kickSkill->setGoaliemode(false);
        kickSkill->setChip(true);
        kickSkill->setTarget(wm->field->oppGoal());
        kickSkill->setSagmode(true);

        break;

    case ballBisector:
        assignSkill(goalKeeperAgent , gpa[goalKeeperAgent->id()]);
        gpa[goalKeeperAgent->id()]->setSlowmode(false);
        gpa[goalKeeperAgent->id()]->setDivemode(false);
        gpa[goalKeeperAgent->id()]->setLookat(wm->ball->pos);
        agentTarget = getGoalieShootOutTarget(false);

        gpa[goalKeeperAgent->id()]->setTargetpos(agentTarget);
        gpa[goalKeeperAgent->id()]->setTargetdir(targetDir);

        drawer->draw(agentTarget, QColor(Qt::darkRed));
        break;

    case skyDive:
        assignSkill(goalKeeperAgent , gpa[goalKeeperAgent->id()]);
        gpa[goalKeeperAgent->id()]->setSlowmode(false);
        gpa[goalKeeperAgent->id()]->setDivemode(true);
        gpa[goalKeeperAgent->id()]->setLookat(wm->ball->pos);
        agentTarget = getGoalieShootOutTarget(true) ;

        gpa[goalKeeperAgent->id()]->setTargetpos(agentTarget);
        gpa[goalKeeperAgent->id()]->setTargetdir(targetDir);

        drawer->draw(agentTarget, QColor(Qt::darkBlue));
        break;
    default:break;
    }


}

void DefensePlan::penaltyMode(){
    //// By this function goalKeeper is able to move according to the direction
    //// of the opponent agents that will shot to our goal in pentalty mode.

    Vector2D ballPos = wm->ball->pos;
    const float goalLineExtra = 0.03;
    const double xDiff = 0.10;
    Line2D goalLine(wm->field->ourGoalL() + Vector2D(+xDiff,+goalLineExtra),
                    wm->field->ourGoalR() + Vector2D(+xDiff,-goalLineExtra));
    const double epsilon = 0.12;
    Vector2D target(-2.93, 0.0);

    Line2D ballRay(ballPos, ballPos + wm->opp[know->nearestOppToBall()]->dir);


    Vector2D intersectionPoint = goalLine.intersection(ballRay);
    double ang = ballRay.a()*goalLine.b() - ballRay.b()*goalLine.a();

    if(fabs(ang) > 0.01 && fabs(ang) < 0.95){
        if(ang*intersectionPoint.y > 0)
            intersectionPoint.y = wm->field->oppGoalR().y;
        else if(ang*intersectionPoint.y < 0)
            intersectionPoint.y = wm->field->oppGoalL().y;
    }

    if(ang <= 0.95)
        intersectionPoint.y *= -1;

    intersectionPoint.y *= (7.0/10.0);

    //    if(fabs(knowledge->getAgent(goalKeeperAgent->id())->pos().y) > fabs(wm->field->ourGoalR().y))
    //        intersectionPoint.y += 1*knowledge->getAgent(goalKeeperAgent->id())->pos().dist(intersectionPoint)*knowledge->getAgent(goalKeeperAgent->id())->pos().dist(intersectionPoint)
    //                *(fabs((intersectionPoint-knowledge->getAgent(goalKeeperAgent->id())->pos()).y)/(intersectionPoint-knowledge->getAgent(goalKeeperAgent->id())->pos()).y);   // sign

    if(intersectionPoint.valid()){
        target = intersectionPoint;
        drawer->draw(target, "red");
    }
    else{
        target.y = 0.0;
    }

    //    target.y = min(max(target.y, wm->field->ourGoalR().y + epsilon), wm->field->ourGoalL().y - epsilon + 0.03);
    Vector2D targetDir(10, 5);
    targetDir.setDir(AngleDeg(0));
    targetDir.setLength(1);

    drawer->draw(target, "blue");

    assignSkill(goalKeeperAgent , gpa[goalKeeperAgent->id()]);
    gpa[goalKeeperAgent->id()]->setSlowmode(false);
    gpa[goalKeeperAgent->id()]->setDivemode(true);

    gpa[goalKeeperAgent->id()]->setTargetpos(target); //HINT : gpa->init
    gpa[goalKeeperAgent->id()]->setTargetdir(targetDir);

    //gpa[goalKeeperAgent->id()]->init(target , targetDir);

}

velAndAccByKK DefensePlan::getBallVelocityByPos(){
    //// This function calculate ball velocity and acceleration using last three
    //// ball position. BallPosHistory is a QList that we prepend ball position
    //// in it (In execute function).

    velAndAccByKK res{};
    if(ballPosHistory.count() < 3 ){
        res.vel = 0.0;
        res.acc = 0.0;
        return res;
    }
    double vel1 = ballPosHistory.at(0).dist(ballPosHistory.at(1))/LOOP_TIME_BYKK;
    double vel2 = ballPosHistory.at(1).dist(ballPosHistory.at(2))/LOOP_TIME_BYKK;
    double acc1 = fabs(vel1 - vel2)/LOOP_TIME_BYKK;
    res.vel = vel1;
    res.acc = acc1;
    return res;
}

bool DefensePlan::checkBallDangerForOneTouch(){
    //// This function check velocity && distance of ball from our goal, &&
    //// in the suitable conditions, return TRUE that means : Defense agents must
    //// work in one touch mode.

    Segment2D ballLine = Segment2D(wm->ball->pos , wm->ball->pos + wm->ball->vel.norm()*10);
    Segment2D goalLine(wm->field->ourGoal()+Vector2D(0,1) , wm->field->ourGoal()+Vector2D(0,-1));
    Vector2D goalLineIntersect = goalLine.intersection(ballLine);
    double ballVel = getBallVelocityByPos().vel;
    double ballDist = wm->ball->pos.dist(wm->field->ourGoal());
    if(((ballDist < 6 && ballVel > 3) || (ballDist < 5 && ballVel > 2.5)
        || (ballDist < 4 && ballVel > 2)|| (ballDist < 3 && ballVel > 1.5)
        || (ballDist < 2 && ballVel > 0.5))
            && goalLineIntersect.valid() && !behindAgent){
        return true;
    }
    else{
        return false;
    }
}

Vector2D* DefensePlan::getIntersectWithDefenseArea(const Line2D& line, const Vector2D& blockPoint){
    //// This function return the intersection a line with defense area (3 part).

    Vector2D* intersectionWithBottomCircle[2];
    Vector2D* intersectionWithTopCircle[2];
    for(int i = 0; i < 2; i++){
        intersectionWithBottomCircle[i] = new Vector2D();
        intersectionWithTopCircle[i] = new Vector2D();
    }
    Vector2D intersectionWithDefenseLine = defenseAreaLine.intersection(line);
    int intersectionWithBottomCircleCount = defenseAreaBottomCircle.intersection(line, intersectionWithBottomCircle[0], intersectionWithBottomCircle[1]);
    int intersectionWithTopCircleCount = defenseAreaTopCircle.intersection(line, intersectionWithTopCircle[0], intersectionWithTopCircle[1]);
    std::vector<Vector2D*> points;
    for(int i = 0; i < intersectionWithBottomCircleCount; i++){
        float angle = getDegree(defenseAreaBottomCircle.center() + Vector2D(1, 0), defenseAreaBottomCircle.center(), *intersectionWithBottomCircle[i]);
        if(angle <= 0 && angle >= -90){
            points.push_back(intersectionWithBottomCircle[i]);
        }
        else{
            delete intersectionWithBottomCircle[i];
        }
    }
    for(int i = 0; i < intersectionWithTopCircleCount; i++){
        float angle = getDegree(defenseAreaTopCircle.center() + Vector2D(1, 0), defenseAreaTopCircle.center(), *intersectionWithTopCircle[i]);
        if(angle >= 0 and angle <= 90){
            points.push_back(intersectionWithTopCircle[i]);
        }
        else{
            delete intersectionWithTopCircle[i];
        }
    }
    double minDist = 99999;
    Vector2D* retPoint = nullptr;
    if(intersectionWithDefenseLine.valid()){
        retPoint = new Vector2D(intersectionWithDefenseLine);
        minDist = retPoint->dist(blockPoint);
    }
    for(vector<Vector2D*>::const_iterator it = points.begin(); it != points.end(); it++){
        double dist = (*it)->dist(blockPoint);
        if(dist < minDist){
            retPoint = (*it);
            minDist = dist;
        }
    }
    for (auto &point : points) {
        if (point != retPoint)
            delete point;
    }
    return retPoint;
}

Vector2D* DefensePlan::getIntersectWithDefenseArea(const Segment2D& segment, const Vector2D& blockPoint){
    //// This function gets us the intersection point between defense area && input
    //// segment. If defense area with input segment have two intersecton points,
    //// this function returns the point that is more near to the blockPoint.

    Vector2D* intersectionWithBottomCircle[2];
    Vector2D* intersectionWithTopCircle[2];
    for (int i = 0; i < 2; i++) {
        intersectionWithBottomCircle[i] = new Vector2D();
        intersectionWithTopCircle[i] = new Vector2D();
    }
    Vector2D intersectionWithDefenseLine = defenseAreaLine.intersection(segment);
    int intersectionWithBottomCircleCount = defenseAreaBottomCircle.intersection(segment, intersectionWithBottomCircle[0], intersectionWithBottomCircle[1]);
    int intersectionWithTopCircleCount = defenseAreaTopCircle.intersection(segment, intersectionWithTopCircle[0], intersectionWithTopCircle[1]);

    std::vector<Vector2D*> points;
    for (int i = 0; i < intersectionWithBottomCircleCount; i++){
        float angle = getDegree(defenseAreaBottomCircle.center() + Vector2D(1, 0), defenseAreaBottomCircle.center(), *intersectionWithBottomCircle[i]);
        if(angle <= 0 and angle >= -90)
            points.push_back(intersectionWithBottomCircle[i]);
        else
            delete intersectionWithBottomCircle[i];
    }
    for (int i = 0; i < intersectionWithTopCircleCount; i++){
        float angle = getDegree(defenseAreaTopCircle.center() + Vector2D(1, 0), defenseAreaTopCircle.center(), *intersectionWithTopCircle[i]);
        if (angle >= 0 and angle <= 90)
            points.push_back(intersectionWithTopCircle[i]);
        else
            delete intersectionWithTopCircle[i];
    }

    double minDist = 99999;
    Vector2D* retPoint = nullptr;
    if (intersectionWithDefenseLine.valid()) {
        retPoint = new Vector2D(intersectionWithDefenseLine);
        minDist = retPoint->dist(blockPoint);
    }
    for (vector<Vector2D*>::const_iterator it = points.begin(); it != points.end(); it++) {
        double dist = (*it)->dist(blockPoint);
        if (dist < minDist) {
            retPoint = (*it);
            minDist = dist;
        }
    }

    for (auto &point : points)
        if (point != retPoint)
            delete point;

    return retPoint;
}

void DefensePlan::executeGoalKeeper(){
    //// This Function execute the goalkeeper skills according to the
    //// target point that have been produced in the "setGoalKeeperTargetPoint"
    //// function. In this function also like the other functions for goalkeeper,
    //// we have some mode for handling the goalkeeper behavior.

    playOffMode = gameState->theirDirectKick()  || gameState->theirIndirectKick();
    playOnMode = gameState->isPlayOn();
    stopMode = gameState->isPlayOff();
    QList<Vector2D> tempSol;
    tempSol.clear();
    if(goalKeeperAgent != nullptr){
        DBUG(QString("goalKeeper clear mode : %1").arg(know->variables["goalKeeperClearMode"].toBool()) , D_AHZ);
        DBUG(QString("goalKeeper oneTouch mode : %1").arg(know->variables["goalKeeperOneTouchMode"].toBool()) , D_AHZ);
        if(playOffMode){
            know->variables["goalKeeperClearMode"] = false;
            know->variables["goalKeeperOneTouchMode"] = false;
            AHZSkills = gpa[goalKeeperAgent->id()];
            DBUG("Their Indirect" , D_AHZ);
            gpa[goalKeeperAgent->id()]->setDivemode(false);
            gpa[goalKeeperAgent->id()]->setSlowmode(false);
            kickSkill->setKickspeed(0);
            gpa[goalKeeperAgent->id()]->setTargetpos(goalKeeperTarget); //HINT : gpa->init
            gpa[goalKeeperAgent->id()]->setTargetdir(wm->ball->pos - wm->field->ourGoal());
            goalKeeperAgent->action = gpa[goalKeeperAgent->id()];

        }
        else if(know->variables["transientFlag"].toBool()){
            know->variables["goalKeeperClearMode"] = false;
            know->variables["goalKeeperOneTouchMode"] = false;
            AHZSkills = gpa[goalKeeperAgent->id()];
            DBUG("TS Mode" , D_AHZ);
            gpa[goalKeeperAgent->id()]->setDivemode(true);
            gpa[goalKeeperAgent->id()]->setSlowmode(false);
            gpa[goalKeeperAgent->id()]->setOnetouchmode(true);
            gpa[goalKeeperAgent->id()]->setNoavoid(true);
            gpa[goalKeeperAgent->id()]->setAvoidpenaltyarea(false);
            kickSkill->setKickspeed(0);
            if(goalKeeperPredictionModeInPlayOff){
                gpa[goalKeeperAgent->id()]->setTargetpos(goalKeeperTarget); //HINT : gpa->init
                gpa[goalKeeperAgent->id()]->setTargetdir(wm->ball->pos - goalKeeperTarget);
            }
            else{
                gpa[goalKeeperAgent->id()]->setTargetpos(goalKeeperTarget); //HINT : gpa->init
                gpa[goalKeeperAgent->id()]->setTargetdir(ballPrediction(true) - wm->field->ourGoal());
            }
            goalKeeperAgent->action = gpa[goalKeeperAgent->id()];
        }
        else if(stopMode){
            know->variables["goalKeeperClearMode"] = false;
            know->variables["goalKeeperOneTouchMode"] = false;
            AHZSkills = gpa[goalKeeperAgent->id()];
            DBUG("Stop Mode" , D_AHZ);
            gpa[goalKeeperAgent->id()]->setDivemode(false);
            gpa[goalKeeperAgent->id()]->setSlowmode(true);
            kickSkill->setKickspeed(0);
            gpa[goalKeeperAgent->id()]->setTargetpos(goalKeeperTarget); //HINT : gpa->init
            gpa[goalKeeperAgent->id()]->setTargetdir(wm->ball->pos - wm->field->ourGoal());
            goalKeeperAgent->action = gpa[goalKeeperAgent->id()];
        }
        else if(ballIsOutOfField){
            know->variables["goalKeeperClearMode"] = false;
            know->variables["goalKeeperOneTouchMode"] = false;
            AHZSkills = gpa[goalKeeperAgent->id()];
            DBUG("Ball is out of field" , D_AHZ);
            gpa[goalKeeperAgent->id()]->setDivemode(false);
            gpa[goalKeeperAgent->id()]->setSlowmode(true);
            kickSkill->setKickspeed(0);
            gpa[goalKeeperAgent->id()]->setTargetpos(goalKeeperTarget); //HINT : gpa->init
            gpa[goalKeeperAgent->id()]->setTargetdir(wm->ball->pos - wm->field->ourGoal());
            goalKeeperAgent->action = gpa[goalKeeperAgent->id()];
        }
        else if(ballIsBesidePoles){
            know->variables["goalKeeperClearMode"] = false;
            know->variables["goalKeeperOneTouchMode"] = false;
            AHZSkills = gpa[goalKeeperAgent->id()];
            DBUG("Ball is beside the poles" , D_AHZ);
            gpa[goalKeeperAgent->id()]->setDivemode(false);
            gpa[goalKeeperAgent->id()]->setSlowmode(true);
            kickSkill->setKickspeed(0);
            gpa[goalKeeperAgent->id()]->setTargetpos(goalKeeperTarget); //HINT : gpa->init
            gpa[goalKeeperAgent->id()]->setTargetdir(wm->ball->pos - goalKeeperTarget);
            goalKeeperAgent->action = gpa[goalKeeperAgent->id()];
        }
        else if(goalKeeperClearMode && !dangerForGoalKeeperClear){
            know->variables["goalKeeperClearMode"] = true;
            know->variables["goalKeeperOneTouchMode"] = false;
            if(wm->ball->vel.length() > 0.4 && wm->ball->vel.length() < 1.3){
                AHZSkills = gpa[goalKeeperAgent->id()];
                DBUG("Clear slow ball" , D_AHZ);
                gpa[goalKeeperAgent->id()]->setDivemode(false);
                gpa[goalKeeperAgent->id()]->setSlowmode(false);
                kickSkill->setKickspeed(0);
                goalKeeperTarget = Segment2D(wm->ball->pos , wm->ball->pos + wm->ball->vel.norm()*100).nearestPoint(goalKeeperAgent->pos());
                gpa[goalKeeperAgent->id()]->setTargetpos(goalKeeperTarget); //HINT : gpa->init
                gpa[goalKeeperAgent->id()]->setTargetdir(wm->ball->pos - wm->field->ourGoal());
            }
            else{
                DBUG("Clear Mode" , D_AHZ);
                AHZSkills = kickSkill;
                kickSkill->setTolerance(10);
                kickSkill->setDontkick(false);
                kickSkill->setSlow(false);
                kickSkill->setSpin(false);
                kickSkill->setAvoidpenaltyarea(false);
                kickSkill->setGoaliemode(true);
                if(wm->ball->pos.y >= 0){
                    kickSkill->setTarget(Vector2D(-3.5 , -2.5) - wm->field->ourGoal());
                }
                else{
                    kickSkill->setTarget(Vector2D(-3.5 , 2.5) - wm->field->ourGoal());
                }
                kickSkill->setChip(true);
                kickSkill->setKickspeed(512);
            }
        }
        else{
            if(goalKeeperOneTouch){
                know->variables["goalKeeperClearMode"] = false;
                know->variables["goalKeeperOneTouchMode"] = true;
                AHZSkills = gpa[goalKeeperAgent->id()];
                DBUG("One touch Mode" , D_AHZ);
                gpa[goalKeeperAgent->id()]->setSlowmode(false);
                gpa[goalKeeperAgent->id()]->setDivemode(true);
                gpa[goalKeeperAgent->id()]->setTargetpos(goalKeeperTarget); //HINT : gpa->init
                gpa[goalKeeperAgent->id()]->setTargetdir( goalKeeperAgent->pos() - wm->field->ourGoal());
                gpa[goalKeeperAgent->id()]->setAvoidpenaltyarea(false);
                gpa[goalKeeperAgent->id()]->setNoavoid(true);
                goalKeeperAgent->action = gpa[goalKeeperAgent->id()];
            }
            else if(dangerForGoalKeeperClear){
                if(dangerForInsideOfThePenaltyArea){
                    know->variables["goalKeeperClearMode"] = true;
                    know->variables["goalKeeperOneTouchMode"] = false;
                    DBUG("Danger Mode" , D_AHZ);
                    AHZSkills = kickSkill;
                    kickSkill->setTolerance(10);
                    kickSkill->setDontkick(false);
                    kickSkill->setSlow(false);
                    kickSkill->setSpin(false);
                    kickSkill->setChip(false);
                    kickSkill->setAvoidpenaltyarea(false);
                    kickSkill->setGoaliemode(true);
                    if(wm->ball->pos.y >= 0){
                        kickSkill->setTarget(Vector2D(-4.5 , -6) - wm->field->ourGoal());
                    }
                    else{
                        kickSkill->setTarget(Vector2D(-4.5 , 6) - wm->field->ourGoal());
                    }
                    kickSkill->setChip(true);
                    kickSkill->setKickspeed(512);
                }
                else{
                    know->variables["goalKeeperClearMode"] = false;
                    know->variables["goalKeeperOneTouchMode"] = false;
                    AHZSkills = gpa[goalKeeperAgent->id()];
                    gpa[goalKeeperAgent->id()]->setSlowmode(false);
                    gpa[goalKeeperAgent->id()]->setDivemode(false);
                    kickSkill->setKickspeed(0);
                    chipGKCounter = 0;
                    gpa[goalKeeperAgent->id()]->setTargetpos(goalKeeperTarget); //HINT : gpa->init
                    gpa[goalKeeperAgent->id()]->setTargetdir(goalKeeperAgent->pos() - wm->field->ourGoal());
                    gpa[goalKeeperAgent->id()]->setAvoidpenaltyarea(false);
                    gpa[goalKeeperAgent->id()]->setNoavoid(true);
                    goalKeeperAgent->action = gpa[goalKeeperAgent->id()];
                }
            }
            else{
                //// strict follow
                know->variables["goalKeeperClearMode"] = false;
                know->variables["goalKeeperOneTouchMode"] = false;
                AHZSkills = gpa[goalKeeperAgent->id()];
                gpa[goalKeeperAgent->id()]->setSlowmode(false);
                gpa[goalKeeperAgent->id()]->setDivemode(false);
                DBUG("No Danger" , D_AHZ);
                gpa[goalKeeperAgent->id()]->setTargetpos(goalKeeperTarget); //HINT : gpa->init
                gpa[goalKeeperAgent->id()]->setTargetdir(wm->ball->pos - wm->field->ourGoal());
                gpa[goalKeeperAgent->id()]->setAvoidpenaltyarea(false);
                goalKeeperAgent->action = gpa[goalKeeperAgent->id()];
            }
            drawer->draw(Circle2D(goalKeeperTarget , 0.05) , 0 , 360 , "black" , true);
        }
        DBUG(QString("beside flag : %1").arg(ballIsBesidePoles) , D_AHZ);
        if(!wm->field->isInOurPenaltyArea(goalKeeperTarget) && !goalKeeperOneTouch && defenseCount != 1){
            tempSol = wm->field->ourPAreaIntersect(Segment2D(wm->field->ourGoal() , goalKeeperTarget));
            if(tempSol.size()){
                if(tempSol.size() == 1){
                    goalKeeperTarget = tempSol.at(0);
                }
                else if(tempSol.size() == 2){
                    goalKeeperTarget = tempSol.at(0).dist(wm->ball->pos) < tempSol.at(1).dist(wm->ball->pos) ? tempSol.at(0) : tempSol.at(1);
                }
            }
        }
        if(!wm->field->isInOurPenaltyArea(goalKeeperAgent->pos()) && !goalKeeperOneTouch && defenseCount != 1){
            tempSol.append(wm->AHZOurPAreaIntersect(Segment2D(wm->field->ourGoal() , goalKeeperAgent->pos()), "goalKeeper"));
            if(tempSol.size() == 1){
                goalKeeperTarget = tempSol.at(0);
            }
            else if(tempSol.size() == 2){
                goalKeeperTarget = tempSol.at(0).dist(wm->ball->pos) < tempSol.at(1).dist(wm->ball->pos) ? tempSol.at(0) : tempSol.at(1);
            }
        }
    }
}

void DefensePlan::announceClearing(bool state){
    if(state){
        know->variables["clearing"] = "true";
    }
    else{
        know->variables["clearing"] = "false";
    }
}

bool DefensePlan::isValidPoint(const Vector2D& point){
    //// This function checks that the input point :
    //// 1- Is in the field or not?
    //// 2- Is in the defense area (3 part) ?

    float distThreshold = 0.10f;

    if(point.x < wm->field->fieldRect().left() - distThreshold){
        return false;
    }
    if(point.x > wm->field->fieldRect().right() + distThreshold){
        return false;
    }
    if(point.y < wm->field->fieldRect().bottom() - distThreshold){
        return false;
    }
    if(point.y > wm->field->fieldRect().top() + distThreshold){
        return false;
    }
    Rect2D rect(midGoal + Vector2D(0.0, defenseAreaLine.length() / 2.0), defenseAreaBottomCircle.radius(), defenseAreaLine.length());
    return  !rect.contains(point) &&
            !defenseAreaBottomCircle.contains(point) &&
            !defenseAreaTopCircle.contains(point);
}

void DefensePlan::initVars(float goalCircleRad){

    //    fastestToBall = know->newFastestToBall(0.016 , wm->our.data->activeAgents , wm->opp.data->activeAgents);
    int ourFastest = fastestToBall.ourFastest();
    catch_time = fastestToBall.catch_time;
    isDefenseFastest = false;
    if(fastestToBall.isFastestOurs){
        for (auto &defenseAgent : defenseAgents) {
            if(defenseAgent->id() == ourFastest){
                isDefenseFastest = true;
                break;
            }
        }
    }

    ballPos = wm->ball->pos;
    ballVel = wm->ball->vel;
    topGoal = wm->field->ourGoalL();
    downGoal = wm->field->ourGoalR();
    midGoal = (topGoal + downGoal) / 2.0;
    ourAgentsRad = 0.09;
    ourGoalAreaCircleRad = goalCircleRad + ourAgentsRad * 1.5 + 0.3;
    know->variables["defenseClearer"] = -1;

    Vector2D upperGoalForCircle = topGoal - Vector2D(0,0.25); //// ahsani
    upperGoalForCircle.y = (wm->field->_GOAL_WIDTH / 2.0) / 2.f;
    Vector2D bottomGoalForCircle = downGoal + Vector2D(0,0.25);//// ahsani
    bottomGoalForCircle.y = - (wm->field->_GOAL_WIDTH / 2.0) / 2.f;

    defenseAreaBottomCircle = Circle2D(upperGoalForCircle , ourGoalAreaCircleRad);
    defenseAreaTopCircle = Circle2D(bottomGoalForCircle , ourGoalAreaCircleRad);
    defenseAreaLine = Segment2D(upperGoalForCircle + Vector2D(ourGoalAreaCircleRad, 0), bottomGoalForCircle + Vector2D(ourGoalAreaCircleRad, 0));

    Vector2D firstBallPos = wm->ball->pos;
    if (firstBallPos.x > wm->field->oppGoal().x - 0.01)
        firstBallPos.x = wm->field->oppGoal().x - 0.01;
    if (firstBallPos.y > wm->field->ourCornerL().y - 0.01)
        firstBallPos.y = wm->field->ourCornerL().y - 0.01;
    if (firstBallPos.y < wm->field->ourCornerR().y + 0.01)
        firstBallPos.y = wm->field->ourCornerR().y + 0.01;
    if (firstBallPos.x < topGoal.x + 0.05)
        firstBallPos.x  = topGoal.x + 0.05;

    ballPos = wm->ball->getPosInFuture(catch_time);
    if (ballVel.length() > 0.3 /*&& fabs(Vector2D::angleBetween(Vector2D(-1, 0), wm->ball->vel).degree()) < 45*/)
        ballPos = firstBallPos;

    if (ballPos.x > wm->field->oppGoal().x || ballPos.y > wm->field->ourCornerL().y
            || ballPos.y < wm->field->ourCornerR().y
            || ballPos.x < topGoal.x + 0.05) {
        Segment2D segment(firstBallPos, ballPos);
        Vector2D vecs[] = { Segment2D(wm->field->ourCornerL() + Vector2D(0.05, 0), wm->field->oppCornerL()).intersection(segment)
                            , Segment2D(wm->field->ourCornerR() + Vector2D(0.05, 0), wm->field->oppCornerR()).intersection(segment)
                            , Segment2D(wm->field->ourCornerL() + Vector2D(0.05, 0), wm->field->ourCornerR() + Vector2D(0.05, 0)).intersection(segment)
                            , Segment2D(wm->field->oppCornerL(), wm->field->oppCornerR()).intersection(segment) };
        float minDist = 1000;
        for (auto vec : vecs)
            if (vec.valid())
            {
                double dist = firstBallPos.dist(vec);
                if (dist < minDist)
                {
                    ballPos = vec;
                    minDist = static_cast<float>(dist);
                }
            }
    }
    clearDistanceForGoalie = static_cast<float>(ourGoalAreaCircleRad + 0.365);
    if (ballPos.dist(midGoal) < clearDistanceForGoalie)
        ballPos = midGoal + Vector2D().setPolar(clearDistanceForGoalie, (ballPos - midGoal).th().degree());

    drawer->draw(Circle2D(ballPos, 0.3), QColor("magenta"));

    goalieDegThreshold = 65.0;
    MinDefenseDistance = 0.1800;
}

bool DefensePlan::defenseOneTouchOrNot(){
    //// First of All this function checks if other defense agents (that isn't
    //// one toucher) can be one toucher , those role changes to it. Then we
    //// check that if ball is shot to the our goal, we active the one touch flag.
    Vector2D sol[2];
    bool isIntersect = false;
    if(!defenseAgents.empty()){
        QList<int> otherAgents;
        otherAgents.clear();
        if(oneToucher < 0 || oneToucher > defenseAgents.size() - 1){
            oneToucher = 0;
        }
        for(int i = 0; i < defenseAgents.size(); i++){
            if(i != oneToucher){
                otherAgents.append(i);
            }
        }
        for (int &otherAgent : otherAgents) {
            Segment2D otherAgentIntersect(wm->ball->pos , pointForKick);
            Vector2D sol1 , sol2;
            Circle2D circ(defenseAgents.at(otherAgent)->pos() , Robot::robot_radius_new);
            int num = circ.intersection(otherAgentIntersect , &sol1 , &sol2);
            Segment2D oneToucherToOther(defensePoints[oneToucher],defensePoints[otherAgent]);
            Line2D ballWay(wm->ball->pos , wm->ball->vel+wm->ball->pos);
            Segment2D ballWay1(wm->ball->pos , wm->ball->vel+wm->ball->pos);
            //////// danger for defense one touch ///////
            for (auto defenseAgent : defenseAgents) {
                if(Circle2D(defenseAgent->pos() , Robot::robot_radius_new).intersection(ballWay1 , &sol[0] , &sol[1])){
                    isIntersect = true;
                    break;
                }
            }
            if(oneToucherToOther.intersection(ballWay).valid() && oneToucherToOther.length() < 0.19 && isIntersect){/// must be test
                return false;
            }
            //////////////////////
            if(pointForKick.valid() && (pointForKick.dist(defenseAgents.at(oneToucher)->pos()) - pointForKick.dist(defenseAgents.at(
                                                                                                                       otherAgent)->pos()) > 0.2
                                        || (num && defenseAgents.at(oneToucher)->pos().dist(defenseAgents.at(otherAgent)->pos()) > 0.19))){//// ?????
                int temp = oneToucher;
                oneToucher = otherAgent;
                otherAgent = temp;
                setPointToKick();
            }
            else if(!pointForKick.valid()){
                int temp = oneToucher;
                oneToucher = otherAgent;
                otherAgent = temp;
                setPointToKick();
                if(!pointForKick.valid()){
                    return false;
                }
            }
        }
    }
    bool oneTouchFlag = false;
    Segment2D ballLine = Segment2D(wm->ball->pos , wm->ball->pos + wm->ball->vel.norm()*10);
    Segment2D goalLine(wm->field->ourGoal()+Vector2D(0,0.8) , wm->field->ourGoal()+Vector2D(0,-0.8));
    Vector2D goalLineIntersect = goalLine.intersection(ballLine);
    bool ballDistVelFlag = defenseCheckBallDangerForOneTouch();
    bool isItClearInFrontOfBall = know->isPointClear(pointForKick , wm->ball->pos , 0.025);
    if(ballDistVelFlag && isItClearInFrontOfBall){
        if(goalLineIntersect.valid()){
            oneTouchFlag = true;
        }
    }
    if(oneTouchFlag){
        if(!isInOneTouch){
            isInOneTouch = true;
            velAndAccByKK temp = getBallVelocityByPos();
            double ballDist = wm->ball->pos.dist(pointForKick);
            double cycle =  ballDist/temp.vel;
            oneTouchCycleTest = cycle / LOOP_TIME_BYKK;
        }
    }
    return oneTouchFlag;
}

void DefensePlan::setPointToKick(){
    //// This function produce a point that one toucher agent must be place on it.
    //// This point is the intersection of the ball line && a line that is
    //// perpendicular on the ball line && is drawn from kicker pos of the agent.

    if(oneToucher < 0 || oneToucher >= defenseAgents.size()){
        oneToucher = 0;
    }
    Vector2D ballPos = wm->ball->pos;
    Line2D ballLine(wm->ball->pos , wm->ball->pos + wm->ball->vel.norm());
    if(!defenseAgents.empty()){
        Line2D perpBallLine(defenseAgents.at(oneToucher)->pos()+defenseAgents.at(oneToucher)->dir().norm()*0.08 , defenseAgents.at(oneToucher)->self.getKickerPos() + wm->ball->vel.norm());
        perpBallLine = perpBallLine.perpendicular(defenseAgents.at(oneToucher)->self.getKickerPos());
        Vector2D intersect = perpBallLine.intersection(ballLine);
        if(intersect.valid()){
            if(isValidPoint(intersect)){
                pointForKick = intersect;
            }
            else{
                Vector2D* inter1 = getIntersectWithDefenseArea(ballLine, ballPos);
                if(inter1 != nullptr && inter1->valid()){
                    pointForKick = *inter1;
                }
                else{
                    pointForKick.invalidate();
                    return;
                }
            }
        }
        else{
            Vector2D *inter = getIntersectWithDefenseArea(Line2D(ballPos, midGoal), ballPos);
            if(inter == nullptr || !inter->valid()){
                pointForKick.invalidate();
                return;
            }
            pointForKick = *inter;
            delete inter;
        }
        oneToucherDir = (Vector2D::unitVector(CKnowledge::oneTouchAngle(pointForKick, Vector2D(0, 0), wm->ball->vel, //TODO: add action property
                                                                        (pointForKick - ballPos).norm()
                                                                        , wm->field->oppGoal()
                                                                        , 0.2
                                                                        , 0.9)));
        pointForKick -= oneToucherDir * (Robot::center_from_kicker_new + CBall::radius);
    }
}

Vector2D* DefensePlan::getIntersectWithDefenseArea(const Circle2D& circle, bool upperPoint){
    //// This function get us intersection between a circle and defense area
    //// (3 part). the boolian ,"upperPOint", is used for choosing only one of the
    //// intersection points according to the Y of the point.For example if input
    //// circle has two intersection with defense area && "upperPoint" is 1 , this
    //// function returns the point that has a more Y in proportion of the Y cecnter
    //// of the input circle.

    Vector2D* intersectionWithBottomCircle[2];
    Vector2D* intersectionWithTopCircle[2];
    Vector2D* intersectionWithLine[2];
    for (int i = 0; i < 2; i++) {
        intersectionWithBottomCircle[i] = new Vector2D();
        intersectionWithTopCircle[i] = new Vector2D();
        intersectionWithLine[i] = new Vector2D();
    }
    int intersectionWithLineCount = circle.intersection(defenseAreaLine, intersectionWithLine[0], intersectionWithLine[1]);
    int intersectionWithBottomCircleCount = defenseAreaBottomCircle.intersection(circle, intersectionWithBottomCircle[0], intersectionWithBottomCircle[1]);
    int intersectionWithTopCircleCount = defenseAreaTopCircle.intersection(circle, intersectionWithTopCircle[0], intersectionWithTopCircle[1]);

    std::vector<Vector2D*> points;
    for(int i = 0; i < intersectionWithLineCount; i++){
        points.push_back(intersectionWithLine[i]);
    }
    for(int i = 0; i < intersectionWithBottomCircleCount; i++){
        float angle = getDegree(defenseAreaBottomCircle.center() + Vector2D(1, 0), defenseAreaBottomCircle.center(), *intersectionWithBottomCircle[i]);
        if(angle <= 0 and angle >= -90){
            points.push_back(intersectionWithBottomCircle[i]);
        }
        else{
            delete intersectionWithBottomCircle[i];
        }
    }
    for(int i = 0; i < intersectionWithTopCircleCount; i++){
        float angle = getDegree(defenseAreaTopCircle.center() + Vector2D(1, 0), defenseAreaTopCircle.center(), *intersectionWithTopCircle[i]);
        if (angle >= 0 and angle <= 90){
            points.push_back(intersectionWithTopCircle[i]);
        }
        else{
            delete intersectionWithTopCircle[i];
        }
    }

    Vector2D* retPoint = nullptr;
    for(vector<Vector2D*>::const_iterator it = points.begin(); it != points.end(); it++){
        if((upperPoint && (*it)->y > circle.center().y) || (!upperPoint && (*it)->y < circle.center().y)){
            retPoint = (*it);
            break;
        }
    }
    for (auto &point : points)
        if (point != retPoint)
            delete point;

    return retPoint;
}

bool DefensePlan::defenseClearOrNot(){
    Vector2D ballPos = wm->ball->pos;
    defenseClearIndex = -1;
    bool isOutOfPenaltyArea = true;
    bool ballVelOrDirection = true;
    bool isOurAgentNearestToTheBall = false;
    bool isGameStarted = gameState->isPlayOn();
    if(ballPos.dist(wm->field->ourGoal()) > 3.5){
        distClearHysteresis = false;
    }
    else if(ballPos.dist(wm->field->ourGoal()) < 2.5){
        distClearHysteresis = true;
    }
    if(wm->ball->vel.length() > 1.5 || isBallGoingToOppArea()){
        ballVelOrDirection = false;
    }
    //Goalie Clear Condition
    if(wm->field->isInOurPenaltyArea(wm->ball->pos)){
        isOutOfPenaltyArea = false;
    }
    if(!distClearHysteresis || !isGameStarted || ballVelOrDirection == false
            || isItPossibleToClear == false || isOutOfPenaltyArea == false){
        lastClearID = defenseClearIndex = -1;
        clearFrameCnt = 0;
        savedClearDist = 0;
        return false;
    }
    if(lastClearID != -1 && ((clearFrameCnt < 30 && savedClearDist < 0.05)
                             || (clearFrameCnt < 80
                                 && savedClearDist >= 0.05))/////// ????????
            && !isBallGoingToOppArea()){
        defenseClearIndex = lastClearID;
        if(goalKeeperAgent != nullptr){
            for(int i = 0; i < wm->our.activeAgentsCount() && i < defenseAgents.size() && defenseClearIndex < defenseAgents.size(); i++){
                if(defenseAgents[defenseClearIndex]->id() && false != wm->our.active(i)->id && wm->our.active(i)->id != goalKeeperAgent->id()){
                    if(defenseAgents[defenseClearIndex]->pos().dist(wm->ball->pos) - wm->our.active(i)->pos.dist(ballPos) > 0.3
                            || wm->ball->pos.dist(defenseAgents[defenseClearIndex]->pos()) > 1.5 + defClearThr){
                        return false;
                    }
                }
            }
        }
        else{
            for(int i = 0; i < wm->our.activeAgentsCount() && i < defenseAgents.size() && defenseClearIndex < defenseAgents.size(); i++){
                if(defenseAgents[defenseClearIndex]->id() != wm->our.active(i)->id){
                    if(/*defenseAgents[defenseClearIndex]->distToBall().length()*/ - wm->our.active(i)->pos.dist(ballPos) > 0.3
                            || wm->ball->pos.dist(defenseAgents[defenseClearIndex]->pos()) > 1.5 + defClearThr){
                        return false;
                    }
                }
            }
            for(int i = 0; i < wm->opp.activeAgentsCount() && i < defenseAgents.size() && i < defenseClearIndex; i++){
                if(wm->ball->pos.dist(defenseAgents[defenseClearIndex]->pos()) - wm->opp.active(i)->pos.dist(ballPos) > 0.3
                        || wm->ball->pos.dist(defenseAgents[defenseClearIndex]->pos()) > 1.5 + defClearThr){
                    return false;
                }
            }
        }
        clearFrameCnt++;
        return true;
    }

    double ourF = fastestToBall.ourFastestTime();
    double oppF = fastestToBall.oppFastestTime();

    if(oppF != -1 && ourF == -1){
        isOurAgentNearestToTheBall = false;
    }
    else if(isDefenseFastest && ((oppF == -1 && ourF != -1) || oppF - ourF > 0.2)){
        isOurAgentNearestToTheBall = true;
    }
    if(isOurAgentNearestToTheBall){
        double minDist = 1000;
        for(int i = 0; i < defenseAgents.size(); i++){
            double dist = defenseAgents.at(i)->pos().dist(ballPos);
            if(i == lastClearID){
                continue;
            }
            if(dist < minDist){
                defenseClearIndex = i;
                minDist = dist;
            }
        }
        if(defenseClearIndex != -1 && lastClearID != -1){
            if(minDist > lastClearDist - 0.05){
                defenseClearIndex = lastClearID;
            }
            else{
                lastClearID = defenseClearIndex;
                lastClearDist = minDist;
            }
        }
        if(defenseClearIndex != -1 && wm->our.activeAgentsCount() > 0){
            if(goalKeeperAgent != nullptr){
                for(int i = 0; i < wm->our.activeAgentsCount(); i++){
                    if(defenseAgents[defenseClearIndex]->id() != wm->our.active(i)->id
                            && wm->our.active(i)->id != goalKeeperAgent->id()){
                        if((wm->ball->pos.dist(defenseAgents[defenseClearIndex]->pos()) - wm->our.active(i)->pos.dist(ballPos) >  0.3)
                                || wm->ball->pos.dist(defenseAgents[defenseClearIndex]->pos()) > 1.5 + defClearThr){
                            return false;
                        }
                    }
                }
            }
            else{
                for(int i = 0; i < wm->our.activeAgentsCount(); i++){
                    if (defenseAgents[defenseClearIndex]->id() != wm->our.active(i)->id){
                        if( (wm->ball->pos.dist(defenseAgents[defenseClearIndex]->pos()) - wm->our.active(i)->pos.dist(ballPos)) > 0.3
                                || wm->ball->pos.dist(defenseAgents[defenseClearIndex]->pos()) > 1.5 + defClearThr)
                            return false;
                    }
                }
                for(int i = 0; i < wm->opp.activeAgentsCount(); i++){
                    if((wm->ball->pos.dist(defenseAgents[defenseClearIndex]->pos()) - wm->opp.active(i)->pos.dist(ballPos) > 0.3)
                            || wm->ball->pos.dist(defenseAgents[defenseClearIndex]->pos()) > 1.5 + defClearThr)
                        return false;
                }
            }
            lastClearID = defenseClearIndex;
            clearFrameCnt = 0;
            return true;
        }
        else{
            lastClearID = defenseClearIndex = -1;
            clearFrameCnt = 0;
            savedClearDist = 0;
            return false;
        }
    }
    else{
        lastClearID = defenseClearIndex = -1;
        clearFrameCnt = 0;
        isBallGoingToOppAreaCnt = -1;
        savedClearDist = 0;
        return false;
    }
    savedClearDist = 0;
    return false;
}

void DefensePlan::checkDefenseExeptions(){
    if(!defenseAgents.empty()){
        setPointToKick();
        doOneTouch = defenseOneTouchOrNot();
        doClear = defenseClearOrNot();
        bool forceBeingInOneTouch = checkStillBeingInOneTouch();
        if(forceBeingInOneTouch){
            oneTouchPointFlag = false;
        }
        if(doOneTouch || forceBeingInOneTouch){
            defExceptions.active = true;
            defExceptions.exeptionMode = defOneTouch;
        }
        else if((doClear || (lastOneTouchClearState == ClearState && histOneTouchClearCnt < 30))
                && !conf.NoClear
                && wm->ball->pos.dist(wm->field->ourGoal()) > 1.4){
            if(doClear){
                lastOneTouchClearState = ClearState;
                histOneTouchClearCnt = 0;
            }
            if(isBallGoingToOppArea()){
                lastOneTouchClearState = NoState;
            }
            histOneTouchClearCnt++;
            defExceptions.active = true;
            defExceptions.exeptionMode = defClear;
            defClearThr = 0.5;
        }
        else{
            defExceptions.active = false;
            defExceptions.exeptionMode = NoneExep;
            defClearThr = 0;
        }
    }
}

Vector2D DefensePlan::checkDefensePoint(Agent* agent, const Vector2D& point){
    Vector2D agentPos = agent->pos();
    double distFromGoal = 1.6;
    if(agentPos.dist(midGoal) < distFromGoal){
        agentPos = midGoal + Vector2D().setPolar(distFromGoal, (agentPos - midGoal).th().degree());
        Vector2D* inter = getIntersectWithDefenseArea(Segment2D(agentPos, midGoal), agentPos);
        if(inter != nullptr && inter->isValid()){
            agentPos = midGoal + Vector2D().setPolar(inter->dist(midGoal) + 0.1, (agentPos - midGoal).th().degree());
            delete inter;
        }
    }
    drawer->draw(Circle2D(agentPos , 0.02) , 0 , 360 , "brown" , true);
    Vector2D* inter = getIntersectWithDefenseArea(Segment2D(agentPos, point), agentPos);
    if(inter == nullptr || agentPos.dist(*inter) < 0.1){
        return point;
    }
    drawer->draw(Circle2D(*inter , 0.02) , 0 , 360 , "pink" , true);
    Vector2D crossPoint = *inter;
    delete inter;
    AngleDeg deg = (crossPoint - midGoal).th();
    AngleDeg mainDeg = (point - midGoal).th();
    AngleDeg diff = mainDeg - deg;
    double s = min(10.0, fabs(diff.degree()));
    double finalDeg = deg.degree() + s * sign(diff.degree());
    Vector2D finalPos = midGoal + Vector2D().setPolar(100, finalDeg);
    inter = getIntersectWithDefenseArea(Segment2D(finalPos, midGoal), finalPos);
    Vector2D retPoint;
    if(inter != nullptr){
        retPoint = *inter;
        delete inter;
    }
    else{
        retPoint.invalidate();
    }
    drawer->draw(Circle2D(retPoint , 0.02) , 0 , 360 , "cyan" , true);
    Vector2D vecDiff = retPoint - crossPoint;
    double dist = crossPoint.dist(point);
    if(dist > 1){
        retPoint = retPoint + vecDiff.setLength(dist * 2);
    }
    drawer->draw(Circle2D(retPoint , 0.02) , 0 , 360 , "blue" , true);
    return retPoint;
}

Vector2D DefensePlan::runDefenseOneTouch(){
    if(!defenseAgents.empty()){
        if(!oneTouchPointFlag){
            //save targets in order not to switch target rapidaly
            defensePoints[oneToucher] = pointForKick;
            defenseDirs[oneToucher] = oneToucherDir;
            oneTouchPointFlag = true;
            calcPointForOneTouch();
            defenseTargets[oneToucher] = defensePoints[oneToucher];
            defenseTargets[oneToucher] = checkDefensePoint(defenseAgents[oneToucher], defenseTargets[oneToucher]);
            return defenseTargets[oneToucher];
        }
        return Vector2D(-100,100);
    }
}

bool DefensePlan::checkStillBeingInOneTouch(){
    Segment2D ballLine = Segment2D(wm->ball->pos , wm->ball->pos + wm->ball->vel.norm()*10);
    Segment2D goalLine(wm->field->ourGoal()+Vector2D(0,1) , wm->field->ourGoal()+Vector2D(0,-1));
    Vector2D goalLineIntersect = goalLine.intersection(ballLine);
    ////////////////////////////Commented by Arash.Z/////////////////////////
    ////check oneTouch timer so it wont switch state machine
    if(!isInOneTouch){
        cycleCounter = 0;
        return false;
    }
    if(!goalLineIntersect.valid()){
        cycleCounter = 0;
        isInOneTouch = false;
        return false;
    }
    drawer->draw(QString("cyclePredict %1").arg(oneTouchCycleTest - cycleCounter) , Vector2D(1,1.4) , "white");
    cycleCounter++;
    if(cycleCounter < oneTouchCycleTest + 5){
        return true;
    }
    else{
        cycleCounter = 0;
        isInOneTouch = false;
        return false;
    }
}

bool DefensePlan::isPathToOppGoalieClear(){
    //// This function checks that if
    Circle2D oppCircleTemp;
    Vector2D point1, point2;
    double radiusTemp;
    radiusTemp = 0.1;
    Segment2D lineToGoal;

    for(int k = 0 ; k <= 2; k++){
        if(k == 0){
            lineToGoal.assign(wm->ball->pos, wm->field->oppGoalL() - Vector2D(0, 0.05));
        }
        else if(k == 1){
            lineToGoal.assign(wm->ball->pos, wm->field->oppGoal());
        }
        else if(k == 2){
            lineToGoal.assign(wm->ball->pos, wm->field->oppGoalR() + Vector2D(0, 0.05));
        }
        for(int i = 0; i < wm->opp.activeAgentsCount(); i++){
            oppCircleTemp.assign(wm->opp.active(i)->pos, radiusTemp);
            if(oppCircleTemp.intersection(lineToGoal, &point1, &point2)){
                return false;
            }
        }
        for(int i = 0; i < wm->our.activeAgentsCount(); i++){
            if(wm->our.active(i)->id == defenseClearIndex){
                continue;
            }
            oppCircleTemp.assign(wm->our.active(i)->pos , radiusTemp);
            if(oppCircleTemp.intersection(lineToGoal, &point1, &point2)){
                return false;
            }
        }
    }
    return true;
}

void DefensePlan::runDefenseExeptions(){
    if(!defenseAgents.empty()){
        Vector2D agentTarget;
        if(defExceptions.exeptionMode == defOneTouch){
            know->variables["defenseOneTouchMode"] = true;
            agentTarget = runDefenseOneTouch();
            drawer->draw(QString("Defense OneTouch"), Vector2D(0,2), "red");
            if(agentTarget.x != -100){
                drawer->draw(QString("Defense OneTouch"), Vector2D(0,2), "red");
                assignSkill(defenseAgents.at(oneToucher) , gpa[defenseAgents.at(oneToucher)->id()]);
                gpa[defenseAgents.at(oneToucher)->id()]->setTargetpos(defenseTargets[oneToucher]); //HINT : gpa->init
                gpa[defenseAgents.at(oneToucher)->id()]->setTargetdir(defenseDirs[oneToucher]);
                drawer->draw(Circle2D(defenseTargets[0] , 0.05) , 0 , 360 , "black" , true);
                defExceptions.exepAgentId = defenseAgents.at(oneToucher)->id();
            }
        }
        else if(defExceptions.exeptionMode == defClear){
            know->variables["defenseClearMode"] = true;
            drawer->draw(QString("Defense Clear"), Vector2D(0,2), "red");
            if(defenseClearIndex != -1){
                if(defExceptions.exepAgentId == -1){
                    defExceptions.exepAgentId = defenseAgents.at(defenseClearIndex)->id();
                }
                else if(defExceptions.exepAgentId != defenseAgents.at(defenseClearIndex)->id()){
                    defenseClearIndex = (defenseClearIndex == 0) ? 1 : 0;
                }
            }
            if(defExceptions.exepAgentId != -1 && defenseClearIndex == -1){
                defenseClearIndex = (defenseAgents.at(0)->id() == defExceptions.exepAgentId) ? 0 : 1;
            }
            runClear();
        }
        else{
            know->variables["defenseOneTouchMode"] = false;
            know->variables["defenseClearMode"] = false;
        }
    }
}

int DefensePlan::decideNumOfMarks(){
    //// This function returns the "defenseCount" in all states, except when ball
    //// is near the corners , returns the 1.

    Vector2D BallPos= wm->ball->pos;
    Vector2D ourGoal = wm->field->ourGoal();
    Vector2D leftCorner = wm->field->ourCornerL();
    Vector2D rightCorner = wm->field->ourCornerR();
    playOnMode = gameState->isPlayOn();
    playOffMode = gameState->theirDirectKick()|| gameState->theirIndirectKick();
    if(defenseCount > 0){
        if(gameState->isPlayOff() && defenseCount >=2){
            if(checkOverdef()){
                return defenseCount - 1;
            }
            else{
                return defenseCount - 2;
            }
        }
        if(playOffMode){
            return decideNumOfMarksInPlayOff(defenseCount);
        }
        else if(know->variables["transientFlag"].toBool()){
            return defenseCount;//TO DO:
        }
        else if(playOnMode){
            if((Vector2D::angleOf(BallPos,ourGoal,leftCorner).abs() < 20 + overDefThr
                ||Vector2D::angleOf(BallPos,ourGoal,rightCorner).abs() < 20 + overDefThr)
                    && defenseCount > 1 && !Circle2D((wm->field->ourGoal() - Vector2D(0.2,0)),1.60).contains(wm->ball->pos)) {
                overDefThr = 5;
                return 0;
            }
            else{
                overDefThr = 0;
            }
        }
    }
    return 0;
}

int DefensePlan::decideNumOfMarksInPlayOff(int _defenseCount){
    // used in playoff without counting goalie
    // TODO: knowlege->tobemark should be replaced
    return _defenseCount;
}

Vector2D DefensePlan::ballPrediction(bool _isGoalie){
    //// When ballLine is in field we predict the ball line : If ball moves toward the
    //// our field, we consider the ballLine (ballPos + ballVel) && If moves toward
    //// the opponent field we consider the ballPos + ballVel.y for the location
    //// of the ball. When ballLine is out of field, we consider the intersection
    //// of field with ballLine.(algorithm is just like when ballLine isn't in the
    //// field).Also , If the ballLine has intersection with opponent agent, we
    //// consider the intersection point for the locaiton of the ball.

    Vector2D BallPos = wm->ball->pos;
    Vector2D BallVel = wm->ball->vel * 0.5;
    Segment2D ballPosVel(BallPos,BallPos+(BallVel * 0.5));
    Vector2D predictedBall;
    Vector2D solu[2];
    Rect2D fieldRect(Vector2D(- wm->field->_FIELD_WIDTH/2.0 , - wm->field->_FIELD_HEIGHT/2.0)+Vector2D(-0.005,-0.005),Vector2D(wm->field->_FIELD_WIDTH/2.0 , wm->field->_FIELD_HEIGHT/2.0)+Vector2D(+0.005,+0.005));
    double dist2Ball = 1000;
    if(BallVel.x > 0 && BallPos.x > 0){
        return BallPos;
    }
    if(wm->opp.activeAgentsCount() > 0){
        for(int i=0 ; i < wm->opp.activeAgentsCount() ; i++){
            Circle2D oppCircle(wm->opp.active(i)->pos,0.1);
            if(oppCircle.intersection(ballPosVel,&solu[0],&solu[1]) > 0 && BallVel.length() > 0.5){
                if(wm->opp.active(i)->pos.dist(BallPos) < dist2Ball){
                    dist2Ball = wm->opp.active(i)->pos.dist(BallPos);
                    predictedBall = wm->opp.active(i)->pos;
                }
            }
        }
    }
    if(dist2Ball != 1000){
        drawer->draw(QString("Def Predicted Level 1"), Vector2D(0,2),"red");
        drawer->draw(predictedBall);
        return predictedBall;
    }
    else if(wm->field->isInField(BallPos + BallVel) && BallVel.length() > 0.5){
        if((BallVel.x <= 0.2 || _isGoalie)){
            predictedBall = BallPos + BallVel;
        }
        else{
            drawer->draw(QString("Def Predicted"), Vector2D(0,1),"red");
            predictedBall = BallPos + Vector2D(0,BallVel.y);
        }
        drawer->draw(predictedBall);
        return predictedBall;
    }
    else if(BallVel.length() > 0.5){
        fieldRect.intersection(ballPosVel, &solu[0], &solu[1]);
        if(BallVel.x <= 0 || _isGoalie){
            predictedBall = (BallPos.dist(solu[0]) < BallPos.dist(solu[1]) && solu[0].isValid()) ? (solu[0]):(solu[1]);
        }
        else{
            drawer->draw(QString("Def Predicted Level 2"), Vector2D(0,2),"red");
            predictedBall = (BallPos.dist(solu[0]) < BallPos.dist(solu[1])) ? Vector2D(BallPos.x,solu[0].y):Vector2D(BallPos.x,solu[1].y);
        }
    }
    else{
        predictedBall = BallPos;
        drawer->draw(QString("Def follow"), Vector2D(0,2),"red");
    }
    drawer->draw(predictedBall);
    return predictedBall;
}

void DefensePlan::findOppAgentsToMark(){

    oppAgentsToMark.clear();
    oppAgentsToMarkPos.clear();
    DBUG(QString("opp goalie ID : %1").arg(wm->opp.data->goalieID), D_AHZ);
    for(int i = 0 ; i < wm->opp.activeAgentsCount() ; i++){
        if(wm->opp.activeAgentID(i) != wm->opp.data->goalieID ){
            oppAgentsToMark.append(wm->opp.active(i));
        }
    }

    if(gameState->theirRestart() || gameState->isPlayOff()){
        //Ommiting nearest to ball
        int nearestToBall = -1;
        double nearestToBallDist = 100000;

        for(int i = 0 ; i < oppAgentsToMark.count() ; i++)
        {
            if((oppAgentsToMark[i]->pos).dist(wm->ball->pos) < nearestToBallDist)
            {
                nearestToBall = i;
                nearestToBallDist = oppAgentsToMark[i]->pos.dist(wm->ball->pos);
                // DBUG(QString("the nearest id is:%1").arg(oppAgentsToMark[i]->id),D_HAMED);
                drawer->draw(oppAgentsToMark[i]->pos + oppAgentsToMark[i]->vel);
            }
        }
        if(nearestToBall != -1)
            oppAgentsToMark.removeOne(oppAgentsToMark[nearestToBall]);
        ///////////////////////
    }
    if(gameState->theirKickoff()){
        for(int i = 0; i < oppAgentsToMark.count(); i++){
            if(oppAgentsToMark[i]->pos.x > conf.OppOmitLimitKickOff){
                oppAgentsToMark.removeOne(oppAgentsToMark[i]);
                // TODO: chage the transeint this flag
                /*if(oppAgentsToMark[i]->vel.length() > 1)
                    HMDtransient = 1;*/
                i--;
            }
        }
    }
    else if(gameState->theirIndirectKick() || gameState->theirDirectKick() || gameState->isPlayOff() || know->variables["transientFlag"].toBool()){
        for(int i = 0; i < oppAgentsToMark.count(); i++){
            if(oppAgentsToMark[i]->pos.x > conf.OppOmitLimitPlayoff){
                oppAgentsToMark.removeOne(oppAgentsToMark[i]);
                // a minor change
                // TODO: chage the transeint this flag
                /*if(oppAgentsToMark[i]->vel.length() > 1)
                HMDtransient = 1;*/
                i--;
            }
        }
    }
    for(int i = 0; i < oppAgentsToMark.count(); i++){
        drawer->draw(oppAgentsToMark[i]->pos);
        oppAgentsToMarkPos.append(posvel(oppAgentsToMark[i], conf.VelReliability));
    }
    DBUG(QString("OppAgenttoMark count %1").arg(oppAgentsToMarkPos.count()), D_HAMED);
}

Vector2D DefensePlan::posvel(CRobot* opp, double VelReliabiity){
    //// This function predicts the opponent agent with considering the position
    //// && velocity of the opponent agent.

    if(VelReliabiity == 0){
        return opp->pos;
    }
    if(!opp->vel.length() > 0.5  || opp->vel.x > 0 || wm->field->isInOurPenaltyArea(opp->pos)){
        VelReliabiity = 0;
    }
    Segment2D tempseg;
    Vector2D solutions[2];
    Vector2D desiredSolution;
    Vector2D temppos = opp->pos + VelReliabiity * opp->vel;
    tempseg.assign(opp->pos, opp->pos + VelReliabiity * opp->vel);
    drawer->draw(tempseg,QColor(Qt::yellow));
    ourBigPenaltyArea(1,0.3,0).intersection(tempseg , &solutions[0] , &solutions[1]);
    desiredSolution = solutions[0].dist(opp->pos) < solutions[1].dist(opp->pos) ? solutions[0] : solutions[1];
    if(wm->field->isInField(desiredSolution) && desiredSolution.isValid() && tempseg.length() != 0){
        return desiredSolution;
    }
    else if(temppos.x < -4.4){
        return Vector2D(-4.4,(opp->pos + VelReliabiity * opp->vel).y) ;
    }
    else{
        return opp->pos + VelReliabiity * opp->vel;
    }
}

void DefensePlan::findPos(int _markAgentSize){
    //// In this function, we choose the different plans of the mark in different
    //// conditions && states.Some flags are used to stay in previous state && for
    //// not switching between PlayOff && PlayOn.

    bool playOn = gameState->isPlayOn();
    bool playOff = ((gameState->theirDirectKick())/*|| (knowledge->getGameState() == CKnowledge::TheirKickOff)*/|| (gameState->theirIndirectKick()));
    bool MantoManAllTransientFlag = conf.ManToManAllTransiant;
    xLimitForblockingPass = 0;
    manToManMarkBlockPassFlag = conf.PlayOffManToMan;
    stopMode = gameState->isPlayOff();
    markPoses.clear();
    markAngs.clear();
    ///////////////// Man To Man AllTransiant Mode for Mark ////////////////////
    DBUG(know->variables["lastStateForMark"].toString() , D_AHZ);
    DBUG(know->variables["stateForMark"].toString() , D_AHZ);
    if(MantoManAllTransientFlag){
        if(know->variables["transientFlag"].toBool()){
            segmentpershoot = 0.1;
        }
        else{
            segmentpershoot = conf.ShootRatioBlock / 100;
        }
        segmentperpass = (100 - conf.PassRatioBlock) / 100;
    }
    else{
        segmentpershoot = conf.ShootRatioBlock / 100;
        segmentperpass = (100 - conf.PassRatioBlock) / 100;
    }
    //////////////// Determine the plan of mark from GUI ////////////////////
    if(manToManMarkBlockPassFlag || wm->ball->pos.x > xLimitForblockingPass){
        if(playOff || stopMode){
            know->variables["stateForMark"] = QString("BlockPass");
            manToManMarkBlockPassInPlayOff(oppAgentsToMarkPos,_markAgentSize , conf.PassRatioBlock / 100);
        }
        else if(know->variables["transientFlag"].toBool()){
            if(know->variables["lastStateForMark"] == QString("BlockPass")){
                know->variables["stateForMark"] = QString("BlockPass");
                manToManMarkBlockPassInPlayOff(oppAgentsToMarkPos,_markAgentSize , conf.PassRatioBlock / 100);
            }
            else{
                know->variables["stateForMark"] = QString("BlockShot");
                manToManMarkBlockShotInPlayOff(_markAgentSize);
            }
        }
    }
    else{
        if(playOff || stopMode){
            know->variables["stateForMark"] = QString("BlockShot");;
            manToManMarkBlockShotInPlayOff(_markAgentSize);
        }
        else{
            if(know->variables["lastStateForMark"].toString() == QString("BlockShot")){
                know->variables["stateForMark"] = QString("BlockShot");;
                manToManMarkBlockShotInPlayOff(_markAgentSize);
            }
            else{
                know->variables["stateForMark"] = QString("BlockPass");
                manToManMarkBlockPassInPlayOff(oppAgentsToMarkPos,_markAgentSize , conf.PassRatioBlock / 100);
            }
        }
    }
    know->variables["lastStateForMark"] = know->variables["stateForMark"];
}

QList<Vector2D> DefensePlan::ShootBlockRatio(double ratio, Vector2D opp){
    //// This function produces a point between opponent agents to be mark &&
    //// ourGoal to block the shot path. Also checks if this point is in the
    //// penalty area, produces a point that is intersection of penalty area with
    //// shot path.
    Vector2D solutions[2];
    QList<Vector2D> tempQlist;
    tempQlist.clear();
    Segment2D tempSeg;
    tempSeg.assign(opp + (wm->field->ourGoal() - opp) * (-10), wm->field->ourGoal());
    Vector2D pos = opp + (wm->field->ourGoal() - opp) * ratio;
    if((wm->field->ourGoal() - pos).length() < markRadiusStrict){
        ourBigPenaltyArea(1,0.3,0).intersection(tempSeg,&solutions[0] , &solutions[1]);
        for(int i = 0 ; i < 2 ; i++){
            if(solutions[i].isValid()){
                tempQlist.append(solutions[i]);
            }
        }
        tempQlist.append(opp - wm->field->ourGoal());
        drawer->draw(tempSeg, "blue");
    }
    else{
        tempQlist.append(pos);
        tempQlist.append(opp - wm->field->ourGoal());
        drawer->draw(tempSeg, "blue");
    }
    return tempQlist;
}

bool DefensePlan::isInTheIndirectAreaShoot(Vector2D opp){
    //// This function checks the point that is resulted from block shot plan,
    //// is in the ball circle or not.

    Circle2D indirectAvoidCircle(wm->ball->pos, 0.5 + 0.2);
    if(indirectAvoidCircle.contains(ShootBlockRatio(segmentpershoot, opp).first()) && !know->variables["transientFlag"].toBool()){
        return true;
    }
    else{
        return false;
    }
}

QList<Vector2D> DefensePlan::indirectAvoidShoot(Vector2D opp){
    //// If a point (that is resulted from block shot plan) is in the ball circle,
    //// this funcrion produces a suitable point instead of it.This point is
    //// intersection of ball circle && shot path.

    Segment2D tempseg;
    tempseg.assign(opp, wm->field->ourGoal());
    double indirectAvoidRadius = 0.5 + .2;
    Circle2D indirectAvoidCircle(wm->ball->pos,indirectAvoidRadius);
    Vector2D sol1, sol2, sol;
    indirectAvoidCircle.intersection(tempseg, &sol1, &sol2);
    if((wm->field->ourGoal() - sol1).length() > (wm->field->ourGoal() - sol2).length()){
        sol = sol2;
    }
    else{
        sol = sol1;
    }
    QList<Vector2D> temp;
    temp.clear();
    temp.append(sol);
    temp.append(wm->ball->pos - wm->field->ourGoal());
    return temp;
}

bool DefensePlan::isInTheIndirectAreaPass(Vector2D opp){
    //// This function checks the point that is resulted from block pass plan,
    //// is in the ball circle or noShootBlockRatiot.
    DBUG(QString("IndirectAreaPass"),D_HAMED);
    double indirectAvoidRadius = 0.5 + 0.2;
    Circle2D indirectAvoidCircle(wm->ball->pos, indirectAvoidRadius);
    if (indirectAvoidCircle.contains(PassBlockRatio(segmentperpass, opp).first()) && !know->variables["transientFlag"].toBool()) return true;

    return false;

}

QList<Vector2D> DefensePlan::indirectAvoidPass(Vector2D opp){
    //// If a point(that is resulted from block pass plan) is in the ball circle
    //// , this function produces a suitable point instead of it.The point is
    //// intersection of ball circle && pass path.

    Segment2D tempseg;
    tempseg.assign(wm->ball->pos, opp + 10 * (opp - wm->ball->pos));
    double indirectAvoidRadius = 0.5 + .2;
    Circle2D indirectAvoidCircle(wm->ball->pos,indirectAvoidRadius);
    Vector2D sol1, sol2, sol;
    indirectAvoidCircle.intersection(tempseg, &sol1, &sol2);
    if(sol1.valid()){
        sol = sol1;
    }
    else if(sol2.valid()){
        sol = sol2;
    }
    QList<Vector2D> temp;
    temp.clear();
    temp.append(sol);
    temp.append(wm->ball->pos - opp);
    return temp;
}

bool DefensePlan::checkOverdef(){
    if((Vector2D::angleOf(wm->ball->pos,wm->field->ourGoal(),wm->field->ourCornerL()).abs() < 20 + overDefThr
        ||Vector2D::angleOf(wm->ball->pos,wm->field->ourGoal(),wm->field->ourCornerR()).abs() < 20 + overDefThr)
            && !Circle2D((wm->field->ourGoal() - Vector2D(0.2,0)),1.60).contains(wm->ball->pos)) {
        overDefThr = 5;
        return true;
    }
    else{
        overDefThr = 0;
        return false;
    }
}

QList<Vector2D> DefensePlan::PassBlockRatio(double ratio, Vector2D opp){
    //// This function produces a point that block the pass path.Also if the
    //// resulted point is in the penalty area, this function geneates a suitable
    //// point.
    Vector2D solutions[2];
    for(int i = 0 ; i < 2 ; i++){
        solutions[i].invalidate();
    }
    Segment2D tempSeg;
    QList<Vector2D> tempQlist;
    tempQlist.clear();
    tempSeg.assign(wm->ball->pos, wm->ball->pos + (opp - wm->ball->pos) * 10);
    Vector2D pos = wm->ball->pos + (opp - wm->ball->pos) * ratio;
    double distance = (wm->ball->pos - opp).length();
    Vector2D sol;
    Segment2D isInPenaltyArea;
    isInPenaltyArea.assign(opp, wm->ball->pos);
    QList<Vector2D> tempVec;
    tempVec.clear();
    Segment2D posToGoal;
    posToGoal.assign(pos,wm->field->ourGoal());
    DBUG(QString("Dist %1").arg(distance), D_HAMED);
    if(gameState->theirRestart() || gameState->isPlayOff())
    {
        if(distance > 1){
            if((pos - wm->ball->pos).length() > 0.7){
                DBUG(QString("First"),D_HAMED);
            }else{
                DBUG(QString("second"),D_HAMED);
                pos = wm->ball->pos + (opp - wm->ball->pos).norm() * 0.7;
            }
        }
        else{
            DBUG(QString("Third"),D_HAMED);
            Vector2D oppAng;
            oppAng.setLength(opp.length());
            oppAng.setDir(opp.dir() + 0.2);
            if(!isInTheIndirectAreaShoot(opp)){
                tempQlist.append(ShootBlockRatio(0.3, opp).first());
                tempQlist.append(ShootBlockRatio(0.3, opp).last());
            }
            else{
                tempQlist.append(indirectAvoidShoot(opp).first());
                tempQlist.append(indirectAvoidShoot(opp).last());
            }
            return tempQlist;
        }
    }
    if(ourBigPenaltyArea(1,0.15,0).intersection(isInPenaltyArea , &solutions[0] , &solutions[1])){
        for(int i = 0 ; i < 2 ; i++){
            if(solutions[i].isValid()){
                tempVec.append(solutions[i]);
            }
        }
        if(tempVec.size() == 1)
        {
            tempQlist.append(tempVec.first());
        }
        else if(tempVec.size() == 2)
        {
            if((tempVec.first() - wm->ball->pos).length() > (tempVec.last() - wm->ball->pos).length())
            {
                sol = tempVec.first();
            }
            else if ((tempVec.last() - wm->ball->pos).length() > (tempVec.first() - wm->ball->pos).length())
            {
                if((tempVec.first() - opp).length() > (tempVec.last() - opp).length())
                {
                    sol = tempVec.last();
                }
                else if ((tempVec.last() - opp).length() > (tempVec.first() - opp).length())
                {
                    sol = tempVec.first();
                }
                tempQlist.append(sol);
            }
            tempQlist.append(sol);
        }
        tempQlist.append( wm->ball->pos - opp);
        drawer->draw(tempSeg, "red");
        DBUG(QString("this is in the penalty area, Block pass Mode"), D_HAMED);
    }
    else{
        tempQlist.append(pos);
        drawer->draw(pos,QColor(100,100,100));

        tempQlist.append( wm->ball->pos - pos);
        drawer->draw(tempSeg, "red");
    }
    Segment2D oppToGoal;
    oppToGoal.assign(wm->field->ourGoal(), opp);
    if(!ourBigPenaltyArea(1,0.15,0).intersection(isInPenaltyArea , &solutions[0] , &solutions[1])){
        tempQlist.clear();
        Vector2D tempPos;
        tempPos = ShootBlockRatio(1, opp + (wm->ball->pos - opp).norm()*.1).first();
        tempQlist.append(tempPos);
        tempQlist.append(wm->ball->pos - tempPos);
    }
    return tempQlist;
}

void DefensePlan::fillDefencePositionsTo(Vector2D *poses){
    //// This function sends the points (that is generated in the defense plan)
    //// to the "coach.cpp".

    for(int i = 0 ; i < defenseAgents.count() ; i++) {
        poses[i] = defensePoints[i];
    }
}

Vector2D DefensePlan::NearestDistanceToBallSegment(Vector2D point){
    //// This function gets us the nearest point of input point in proportion of
    //// the our goal line, if intersection of ball line && our goal line is valid.

    Segment2D OurGoalAreaLine(wm->field->ourGoal()+Vector2D(0,-0.6) , wm->field->ourGoal()+Vector2D(0,0.6));
    Segment2D Ballsegment(wm->ball->pos , wm->ball->pos + wm->ball->vel.norm()*10);
    Vector2D  OurGoalAreaLineIntersectionByBallSegment(OurGoalAreaLine.intersection(Ballsegment));
    Vector2D NearestTarget = OurGoalAreaLine.nearestPoint(point);
    if(OurGoalAreaLineIntersectionByBallSegment.valid()){
        return NearestTarget;
    }
}

int DefensePlan::predictMostDangrousOppToBall(){
    //// This function gets us the ID of the agents that is in front of the ball
    //// && probably ball reaches to their.

    if(wm->opp.activeAgentsCount() > 0){
        Vector2D OppPos[wm->opp.activeAgentsCount()];
        for(int i = 0; i < wm->opp.activeAgentsCount();i++){
            OppPos[i] = wm->opp.active(i)->pos;
        }
        Vector2D OppPosProjectionsOnBallSegmentByVel[wm->opp.activeAgentsCount()];
        Segment2D BallSegmentByVel(wm->ball->pos , wm->ball->pos + wm->ball->vel.norm()*10);
        double DistanceFromProjectionsToBallPos[wm->opp.activeAgentsCount()];
        double DistanceFromOppPosToProjections[wm->opp.activeAgentsCount()];
        double BallVelocity= wm->ball->vel.length();
        double MinDistanceToBallPosY = 10000;
        double  DistanceToBallPosY[wm->opp.activeAgentsCount()];
        int MinDistanceToBallPosYRoboID = 100;
        for(int i = 0 ; i < wm->opp.activeAgentsCount() ; i++){
            OppPosProjectionsOnBallSegmentByVel[i] = NearestDistanceToBallSegment(OppPos[i]);
            DistanceFromProjectionsToBallPos[i] = (wm->ball->pos - OppPosProjectionsOnBallSegmentByVel[i]).length();
            DistanceFromOppPosToProjections[i] = (OppPosProjectionsOnBallSegmentByVel[i] - OppPos[i]).length();
            Segment2D OppSegmentByvel(wm->opp.active(i)->pos,wm->opp.active(i)->pos + wm->opp.active(i)->vel.norm()*10);
            Segment2D OppPosTotheirProjections(OppPosProjectionsOnBallSegmentByVel[i],OppPos[i]);
            Segment2D OppSegmentByDir(wm->opp.active(i)->pos,wm->opp.active(i)->pos + wm->opp.active(i)->dir*10);
            if(OppPosTotheirProjections.intersection(BallSegmentByVel).isValid()){
                // is front to ball
                if(wm->opp.active(i)->vel.length() > 0.5){
                    // is moving
                    if(OppSegmentByvel.intersection(BallSegmentByVel).valid() && DistanceFromProjectionsToBallPos[i] * BallVelocity / DistanceFromOppPosToProjections[i] > 5){
                        // find the lowest amount of distToBallY
                        DistanceToBallPosY[i] = fabs(OppPos[i].y - wm->ball->pos.y);
                        if(MinDistanceToBallPosY > DistanceToBallPosY[i]){
                            MinDistanceToBallPosY = DistanceToBallPosY[i];
                            MinDistanceToBallPosYRoboID = wm->opp.active(i)->id;
                        }
                    }
                }
                else{
                    // is n't moving
                    if(OppSegmentByDir.intersection(BallSegmentByVel).valid() && DistanceFromOppPosToProjections[i] < 0.3){
                        DistanceToBallPosY[i] = fabs(wm->opp.active(i)->pos.y - wm->ball->pos.y);
                        if(DistanceToBallPosY[i] < MinDistanceToBallPosY){
                            MinDistanceToBallPosY=DistanceToBallPosY[i];
                            MinDistanceToBallPosYRoboID=wm->opp.active(i)->id;
                        }
                    }
                }
            }
            else{
                // is behind to ball
            }
        }
        return MinDistanceToBallPosYRoboID;
    }
    else{
        return 100;
    }
}

Vector2D DefensePlan::strictFollowBall(Vector2D _ballPos){
    //// This function is  the main behavior of goalkeeper that is a geometric
    //// behavior.

    //////////////////////// Variables of this function //////////////////////
    Vector2D ballPos = _ballPos;
    Segment2D goal2Ball;
    QList<Vector2D> tempSol;
    Vector2D target(Vector2D(0,0));
    Vector2D goalKeeperTargetOffSet = Vector2D(0.11 , -0.06);
    QList<Circle2D> defs;
    double AZBisecOpenAngle = 0,AZBigestOpenAngle = 0,AZDangerPercent = 0,aimLessChord = 0;
    double topFaceLength;
    double bottomFaceLength;
    double ballheight;
    double nearestDist2Ball;
    int nearestDef2BallId= -1 ;
    int g;
    //////////////////////////////////////////////////////////////////////////
    tempSol.clear();
    if(goalKeeperAgent != nullptr){
        ballPos = _ballPos;
        Segment2D goalLine(wm->field->ourGoal()+Vector2D(0,-0.8) , wm->field->ourGoal() + Vector2D(0,0.8));
        Segment2D downFieldLine(Vector2D(-wm->field->_FIELD_WIDTH/2,-wm->field->_FIELD_HEIGHT/2),Vector2D(-wm->field->_FIELD_WIDTH/2,wm->field->_FIELD_HEIGHT/2));
        //////////////////////////////// Appending circles on defense agents /////////////////////////////////////////
        for (g = 0; g < defenseAgents.count() ; g++){
            defs.append(Circle2D(defenseAgents[g]->pos(), Robot::robot_radius_new));
            if(defenseAgents[g]->pos().dist(wm->ball->pos) < nearestDist2Ball) {
                nearestDef2BallId = defenseAgents[g]->id();
                nearestDist2Ball = defenseAgents[g]->pos().dist(wm->ball->pos);
            }
        }
        drawer->draw(ballPrediction(true));
        ///////////////////////////// Empty region between defense agents //////////////////////////
        know->getEmptyAngle(ballPos, wm->field->ourGoalL(), wm->field->ourGoalR(), defs, AZDangerPercent, AZBisecOpenAngle, AZBigestOpenAngle,true);
        ////////// Bisector of triangle that is made up of with this points : [ballPossition , topGoal , bottom Goal]  //////////////////////////
        Segment2D AZBisecOpenSeg(ballPos , ballPos + Vector2D(cos(_PI*(AZBisecOpenAngle)/180),sin(_PI*(AZBisecOpenAngle)/180)).norm()*12);
        ////////// Top and bottom line of triangle that is made up of with this points : [ballPossition , topGoal , bottom Goal]  //////////////////////////
        Segment2D AZTopOfOpenSeg(ballPos , ballPos + Vector2D(cos(_PI*(AZBisecOpenAngle + (AZBigestOpenAngle/2))/180),sin(_PI*(AZBisecOpenAngle + (AZBigestOpenAngle/2))/180)).norm()*12);
        Segment2D AZBottomOfOpenSeg(ballPos , ballPos + Vector2D(cos(_PI*(AZBisecOpenAngle - (AZBigestOpenAngle/2))/180),sin(_PI*(AZBisecOpenAngle - (AZBigestOpenAngle/2))/180)).norm()*12);
        ///////////// Intersection of the top and bottom line of the triangle with goalLine ////////////////////////////////////////////
        Vector2D openAngGoalIntersectionTop(AZTopOfOpenSeg.intersection(goalLine));
        Vector2D openAngGoalIntersectionBottom(AZBottomOfOpenSeg.intersection(goalLine));
        /////////////////// Real length of top and bottom line of the triangle for talles //////////////////////////////////////////////////////////////
        topFaceLength = ballPos.dist(openAngGoalIntersectionTop);
        bottomFaceLength = ballPos.dist(openAngGoalIntersectionBottom);
        ///////////////////  top and bottom line of the triangle for talles for talles (with real length)//////////////////////////////////////////////////////////////
        Segment2D topFaceLength_forTalles(ballPos , ballPos + Vector2D(cos(_PI*(AZBisecOpenAngle + (AZBigestOpenAngle/2))/180),sin(_PI*(AZBisecOpenAngle + (AZBigestOpenAngle/2))/180)).norm()*topFaceLength);
        Segment2D bottomFaceLength_forTalles(ballPos , ballPos + Vector2D(cos(_PI*(AZBisecOpenAngle - (AZBigestOpenAngle / 2))/180),sin(_PI*(AZBisecOpenAngle - (AZBigestOpenAngle/2))/180)).norm()*bottomFaceLength);
        //////////////////////////Height of the triangle ///////////////////////////////////////////////////////
        ballheight = ballPos.dist(downFieldLine.nearestPoint(ballPos));
        Line2D aimLessLine(Vector2D(0,0),Vector2D(-1,-1));
        drawer->draw(AZBisecOpenSeg,"red");
        goal2Ball.assign(wm->field->ourGoal(),wm->ball->pos);
        if(Vector2D::angleOf(wm->ball->pos,wm->field->ourGoal(),wm->field->ourGoalL()).degree() < 15 + angleDegreeThrNotStop){
            target = wm->field->ourGoalL() + goalKeeperTargetOffSet;
            angleDegreeThrNotStop = 2;
        }
        else if(Vector2D::angleOf(wm->ball->pos,wm->field->ourGoal(),wm->field->ourGoalR()).degree() < 15 + angleDegreeThrNotStop){
            target = wm->field->ourGoalR() + goalKeeperTargetOffSet;
            angleDegreeThrNotStop = 2;
        }
        else {
            angleDegreeThrNotStop = 0;
            if(goalKeeperAgent->pos().dist(AZBisecOpenSeg.nearestPoint(goalKeeperAgent->pos())) > 0.3 + thr){
                target = AZBisecOpenSeg.nearestPoint(goalKeeperAgent->pos());
                thr = 0;
                if(!wm->field->isInField(target)){
                    target = AZBisecOpenSeg.intersection(goalLine);
                }
            }
            else{
                thr = 0.3;
                if(topFaceLength < bottomFaceLength){
                    aimLessChord = bottomFaceLength_forTalles.nearestPoint(openAngGoalIntersectionTop).dist(openAngGoalIntersectionTop);
                    DBUG(QString("top koochik tar"),D_SEPEHR);
                    aimLessLine = Line2D(bottomFaceLength_forTalles.nearestPoint(openAngGoalIntersectionTop),openAngGoalIntersectionTop);
                }
                else{
                    aimLessChord = topFaceLength_forTalles.nearestPoint(openAngGoalIntersectionBottom).dist(openAngGoalIntersectionBottom);
                    DBUG(QString("bottom koochik tar"),D_SEPEHR);
                    aimLessLine = Line2D(topFaceLength_forTalles.nearestPoint(openAngGoalIntersectionBottom),openAngGoalIntersectionBottom);
                    drawer->draw(Segment2D(topFaceLength_forTalles.nearestPoint(openAngGoalIntersectionBottom),openAngGoalIntersectionBottom),QColor(Qt::black));
                }
                if(aimLessChord > 2 * Robot::robot_radius_new){
                    DBUG(QString("chord > 2radius"),D_SEPEHR);
                    aimLessLine = Line2D(Vector2D(ballPos.x-(.18*ballheight/aimLessChord),ballPos.y),Vector2D(ballPos.x-(.18*ballheight/aimLessChord),ballPos.y-0.1));
                    drawer->draw(Segment2D(Vector2D(ballPos.x-(.18*ballheight/aimLessChord),ballPos.y),Vector2D(ballPos.x-(.18*ballheight/aimLessChord),ballPos.y-1)),QColor(Qt::black));
                    if(AZBisecOpenSeg.intersection(aimLessLine).isValid()){
                        if(defenseCount == 2){
                            if(know->getEmptyAngle(ballPos, wm->field->ourGoalL(), wm->field->ourGoalR(), defs, AZDangerPercent, AZBisecOpenAngle, AZBigestOpenAngle,true) > 10 + threshOld){
                                target = AZBisecOpenSeg.intersection(aimLessLine);
                                threshOld = 0;
                            }
                            else{
                                threshOld = 5;
                                target = know->getPointInDirection(wm->field->ourGoal() , wm->ball->pos , 0.1);
                            }
                        }
                        else if(defenseCount == 1){
                            target = getGoaliePositionInOneDef(wm->ball->pos , 1 , 1.5);
                        }
                        else{
                            tempSol.append(wm->AHZOurPAreaIntersect(Segment2D(wm->field->ourGoal() , wm->ball->pos), QString("goalKeeper")));
                            if(tempSol.size() == 1){
                                target = tempSol.at(0);
                            }
                            else if(tempSol.size() == 2){
                                target = tempSol.at(0).dist(wm->ball->pos) < tempSol.at(1).dist(wm->ball->pos) ? tempSol.at(0) : tempSol.at(1);
                            }
                        }
                    }
                    else{
                        if(defenseCount == 2){
                            if(know->getEmptyAngle(ballPos, wm->field->ourGoalL(), wm->field->ourGoalR(), defs, AZDangerPercent, AZBisecOpenAngle, AZBigestOpenAngle,true) > 10 + threshOld){
                                target = AZBisecOpenSeg.intersection(aimLessLine);
                                threshOld = 0;
                            }
                            else{
                                threshOld = 5;
                                target = know->getPointInDirection(wm->field->ourGoal() , wm->ball->pos , 0.1);
                            }
                        }
                        else if(defenseCount == 1){
                            target = getGoaliePositionInOneDef(wm->ball->pos , 1 , 1.5);
                        }
                        else{
                            tempSol.append(wm->AHZOurPAreaIntersect(Segment2D(wm->field->ourGoal() , wm->ball->pos), QString("goalKeeper")));
                            if(tempSol.size() == 1){
                                target = tempSol.at(0);
                            }
                            else if(tempSol.size() == 2){
                                target = tempSol.at(0).dist(wm->ball->pos) < tempSol.at(1).dist(wm->ball->pos) ? tempSol.at(0) : tempSol.at(1);
                            }
                        }
                    }
                }
                else{
                    if(defenseCount == 2){

                        if(know->getEmptyAngle(ballPos, wm->field->ourGoalL(), wm->field->ourGoalR(), defs, AZDangerPercent, AZBisecOpenAngle, AZBigestOpenAngle,true) > 10 + threshOld){

                            tempSol.append(wm->AHZOurPAreaIntersect(Segment2D(wm->field->ourGoal() , wm->ball->pos), QString("goalKeeper")));
                            if(tempSol.size() == 1){
                                target = tempSol.at(0);
                            }
                            else if(tempSol.size() == 2){
                                target = tempSol.at(0).dist(wm->ball->pos) < tempSol.at(1).dist(wm->ball->pos) ? tempSol.at(0) : tempSol.at(1);
                            }
                            threshOld = 0;
                        }
                        else{
                            threshOld = 5;

                            target = know->getPointInDirection(wm->field->ourGoal() , wm->ball->pos , 0.1); // move to knowledge

                        }
                    }
                    else if(defenseCount == 1){
                        target = getGoaliePositionInOneDef(wm->ball->pos , 1 , 1.5);
                    }
                    else{
                        tempSol.append(wm->AHZOurPAreaIntersect(AZBisecOpenSeg, QString("goalKeeper") ));
                        if(tempSol.size() == 1){
                            target = tempSol.at(0);
                        }
                        else if(tempSol.size() == 2){
                            target = tempSol.at(0).dist(wm->ball->pos) < tempSol.at(1).dist(wm->ball->pos) ? tempSol.at(0) : tempSol.at(1);
                        }
                    }
                }
            }
            if(!wm->field->isInOurPenaltyArea(target) && defenseCount != 1){
                tempSol.append(wm->AHZOurPAreaIntersect(Segment2D(wm->field->ourGoal() , wm->ball->pos), QString("goalKeeper") ));
                if(tempSol.size() == 1){
                    target = tempSol.at(0);
                }
                else if(tempSol.size() == 2){
                    target = tempSol.at(0).dist(wm->ball->pos) < tempSol.at(1).dist(wm->ball->pos) ? tempSol.at(0) : tempSol.at(1);
                }
            }
            if(!wm->field->isInField(target) || target.x < -4.3){
                target = know->getPointInDirection(wm->field->ourGoal() , wm->ball->pos , 0.25);
            }
        }
    }
    DBUG(QString("x: %1 , y: %2").arg(target.x).arg(target.y), D_ATOUSA);
    return target;
}

Vector2D DefensePlan::getGoaliePositionInOneDef(Vector2D _ballPos, double _limit1, double _limit2){
    auto *tempCDef = new CDefPos();
    Vector2D goalieTarget;
    double tempBestRadius = _ballPos.dist(wm->field->ourGoal())/2;
    if(tempCDef->findBestRadius(tempDefPos.size) != -1){
        if(wm->ball->pos.x < -2.5){
            tempBestRadius = tempDefPos.pos[0].dist(wm->field->ourGoal()) - 0.5;
            DBUG(QString("radius:%1").arg(tempBestRadius), D_ATOUSA);
        }
        else{
            tempBestRadius = tempCDef->findBestRadius(tempDefPos.size);
        }
    }
    if(tempBestRadius > _limit2){
        tempBestRadius = _limit2;
    }
    else if(tempBestRadius < _limit1){
        tempBestRadius = _limit1;
    }

    kk2Angles tempAngles = tempCDef->getIntersections(wm->ball->pos, tempBestRadius);
    //double tempOpenAngle = fabs(tempAngles.angle2 - tempAngles.angle1);
    //drawer->draw(QString::number(tempBestRadius), Vector2D(-1, -_FIELD_HEIGHT/2 - 0.2));

    double agentAngle = tempCDef->getRobotAngle(tempBestRadius);
    drawer->draw(QString::number(agentAngle), Vector2D(-1, wm->field->_FIELD_HEIGHT/2 - 0.4));
    //double openAngleAfterPositioning = tempOpenAngle - (agentAngle * _size);

    //draw(Circle2D(wm->field->ourGoal(), tempBestRadius), "yellow");

    if(wm->ball->pos.y + wm->ball->vel.y < 0 + goalieThr){
        goalieTarget = tempCDef->getXYByAngle(tempAngles.angle2-agentAngle/2, tempBestRadius);
        goalieThr = 0.2;
        //oneDefThr = 1;
    }
    else {
        goalieTarget = tempCDef->getXYByAngle(tempAngles.angle1+agentAngle/2, tempBestRadius);
        goalieThr = -0.2;
        //oneDefThr = -1;
    }

    return goalieTarget;
}

bool DefensePlan::defenseCheckBallDangerForOneTouch(){
    //// This function checks that ball is shot to the our goal, is danger to
    //// intercept its line or not (according the distance of ball from our goal
    //// && ball velocity)

    double defCircleRad = 0;
    Circle2D defCircle;
    if(defenseAgents.count() > 0){
        for(int i = 0; i < defenseAgents.count(); i++){
            if(defenseAgents[i]->pos().dist(ballPos) > defCircleRad){//// ??????
                defCircleRad = defenseAgents[i]->pos().dist(wm->field->ourGoal());
            }
        }
    }
    if(defCircleRad > 0){
        defCircle = Circle2D(wm->field->ourGoal(),defCircleRad);
    }
    else{
        defCircle = Circle2D((wm->field->ourGoal() - Vector2D(0.2,0)),1.50);
    }
    double ballVel = getBallVelocityByPos().vel;
    double ballDist = wm->ball->pos.dist(wm->field->ourGoal());
    return ((ballDist < 6 && ballVel > 3)
            || (ballDist < 5 && ballVel > 2.5)
            || (ballDist < 4 && ballVel > 2)
            || (ballDist < 3 && ballVel > 1.5)
            || (ballDist < 2 && ballVel > 0.5)) && !defCircle.contains(ballPos);
}

bool DefensePlan::isBallGoingToOppArea(){
    //// This function checks that the ball is going to the opponent goal or not.
    //// But, Why only in 20 code loop?
    if(wm->ball->vel.length() > 1.5){
        Segment2D ballPathTemp(ballPos, ballPos + wm->ball->vel.norm()*10);
        Segment2D oppGoalieLine(wm->field->oppGoal() + Vector2D(0,5), wm->field->oppGoal() + Vector2D(0,-5));
        if(oppGoalieLine.intersection(ballPathTemp).valid()){ //// ??????????
            double velTemp = getBallVelocityByPos().vel;
            if(velTemp > 2 && isBallGoingToOppAreaCnt == -1){
                isBallGoingToOppAreaCnt = 0;
                return true;
            }
            if(isBallGoingToOppAreaCnt >= 0){
                isBallGoingToOppAreaCnt++;
                return true;
            }
            if(isBallGoingToOppAreaCnt > 20){
                isBallGoingToOppAreaCnt = -1;
                return false;
            }
        }
    }
    isBallGoingToOppAreaCnt = -1;
    return false;
}

void DefensePlan::calcPointForOneTouch(){
    if(wm->field->isInOurPenaltyArea(defensePoints[0]) || !wm->field->fieldRect().contains(defensePoints[0])){ ////// ?????
        //calculate intersect point for one touch!
        Vector2D* inter2 = getIntersectWithDefenseArea(Line2D(wm->ball->pos , defensePoints[0]) , ballPos);
        if(inter2 != nullptr && inter2->valid()){
            defensePoints[0] = *inter2;
            delete inter2;
        }
        else{
            drawer->draw("oneDefenseAndGoalie! ERROR1" , Vector2D(-0.1,2.2) , "red");
        }
    }
}

void DefensePlan::runClear(){
    if(defenseClearIndex < 0 || defenseClearIndex >= defenseAgents.size()){
        return;
    }
    announceClearing(true);
    assignSkill(defenseAgents.at(defenseClearIndex),kickSkill);
    kickSkill->setKickspeed(1023);
    kickSkill->setTolerance(1.5);
    kickSkill->setDontkick(false);
    kickSkill->setInterceptmode(true);
    kickSkill->setTarget(wm->field->oppGoal());
    kickSkill->setSlow(false);
    kickSkill->setSpin(false);
    kickSkill->setChip(false);
    kickSkill->setAvoidpenaltyarea(true);
    know->variables["defenseClearer"] = defenseAgents.at(defenseClearIndex)->id();
    if(!isPathToOppGoalieClear() || savedClearDist > 0.05){ /////  must be refine
        kickSkill->setChip(true);
    }
}

Vector2D DefensePlan::findBestPointForChipTarget(double &chipDist,bool isGoalie){
    double region;
    QList <int> ourRel;
    QList <int> oppRel;
    QList <Vector2D> points;
    QList <int> IDs;
    Vector2D dangerPoint;
    Vector2D dangerPointOnMiddleLine;
    Vector2D bestPos;
    Vector2D dirTemp;
    Vector2D regionPoses;
    Vector2D agentPos = Vector2D(0,0);
    Vector2D dir = Vector2D(0,0);
    //in sharte ezafe shode vase vaghti ke defence nis at(0) ro ke mikhonim segment mide
    if(!defenseAgents.empty() && !isGoalie){
        agentPos = (defenseClearIndex == -1) ? defenseAgents.at(0)->pos() : defenseAgents.at(defenseClearIndex)->pos();
        dir = (defenseClearIndex == -1)? defenseAgents.at(0)->dir() : defenseAgents.at(defenseClearIndex)->dir();
    }
    else if(isGoalie && goalKeeperAgent->isVisible()){
        agentPos = goalKeeperAgent->pos();
        dir = goalKeeperAgent->dir();
    }
    ourRel.clear();
    double minRegion = 1000;
    int num = 0;
    for(double i = -(wm->field->_FIELD_HEIGHT/2) ; i <= (wm->field->_FIELD_HEIGHT/2) ; i = i + (wm->field->_FIELD_HEIGHT/25)){
        if(!isGoalie){
            dangerPoint = know->getEmptyPosOnGoal(Vector2D(3, i), region, false, oppRel, ourRel); //todo: knowledge
            dangerPointOnMiddleLine = Vector2D(3, i);
        }
        else{
            dangerPoint = know->getEmptyPosOnGoal(Vector2D(0, i), region, false, oppRel, ourRel);
            dangerPointOnMiddleLine = Vector2D(0, i);
        }
        if(region < minRegion){
            minRegion = region;
            bestPos = dangerPointOnMiddleLine;
        }
        if(region < 0.25){
            points.append(dangerPointOnMiddleLine);
            IDs.append(num);
        }
        num++;
    }
    num = 1;
    if(points.count() <= 0){
        drawer->draw(bestPos, QColor(Qt::darkRed));
        drawer->draw(QString("chip target") , bestPos + Vector2D(0, 0.13), "white", 10);
        chipDist = agentPos.dist(bestPos);
        return bestPos;
    }
    else{
        Vector2D ballPosTemp = wm->ball->pos;
        double minDiff = 1000;
        for(int j = 0; j < IDs.count(); j++){
            regionPoses = Vector2D(0, -(wm->field->_FIELD_HEIGHT/2) + (wm->field->_FIELD_HEIGHT/25)* IDs.at(j));
            dirTemp = regionPoses - ballPosTemp;
            if(fabs(dirTemp.th().degree() - dir.th().degree()) < minDiff){
                bestPos = regionPoses;
                minDiff = fabs(dirTemp.th().degree() - dir.th().degree());
            }
            drawer->draw(Segment2D(regionPoses, regionPoses + dirTemp.norm()*2),QColor(Qt::darkRed));
        }
        drawer->draw(bestPos ,QColor(Qt::darkRed));
        drawer->draw(QString("chip target") , bestPos + Vector2D(0, 0.13), "white", 10);
        chipDist = agentPos.dist(bestPos);
        return bestPos;
    }
}

//////////////////////////////////////////////////////////////////////////////

QList<QPair<Vector2D, double> > DefensePlan::sortdangerpassplayoff(QList<Vector2D> oppposdanger){
    double danger = 0;
    /////////////// Polygon
    double radius = .1;
    double treshold = 2;

    Vector2D sol1,sol2,sol3;
    Vector2D _pos1 = wm->ball->pos;

    Vector2D _pos2 = wm->ball->pos + (2 * wm->ball->vel.norm() * wm->ball->vel.length());
    Line2D _path(_pos1,_pos2);
    Polygon2D _poly;
    Circle2D(_pos2,radius + treshold).
            intersection(_path.perpendicular(_pos2), &sol1, &sol2);

    sol3 = sol1;
    _poly.addVertex(sol1);
    //sol3 = sol1;
    _poly.addVertex(sol2);
    Circle2D(_pos1,Robot::robot_radius_new).
            intersection(_path.perpendicular(_pos1),&sol1,&sol2);
    _poly.addVertex(sol2);
    _poly.addVertex(sol1);
    _poly.addVertex(sol3);

    drawer->draw(_poly,"cyan");

    double KAP = 1; //Angle parameter
    double KDBP = 1; //distancetoball
    double KDIP = 2; //distancetointersect

    double AngleP;
    double distanceToBallProjectionP;
    double distanceToIntersectP;


    double RangeofAngleP = 90;
    double RangeofdistanceToIntersectP =  radius + treshold;
    double RangeofdistanceToBallProjectionP = Segment2D(Vector2D(-1.0 * wm->field->_FIELD_WIDTH / 2, -1.0 * wm->field->_FIELD_HEIGHT /2 ), Vector2D(wm->field->_FIELD_WIDTH / 2 , wm->field->_FIELD_HEIGHT / 2)).length();
    double danger2 = 0;


    /////////////////////


    double KA=1; //Angle Coefficient
    double KDB=0;  //Distance To Ball
    double KDG=1;  //Distnce To Goal
    double RangeofAngle = Vector2D::angleOf(wm->field->ourGoalR(),Vector2D(-1.0 * (wm->field->_FIELD_WIDTH / 2 - wm->field->_PENALTY_DEPTH), 0), wm->field->ourGoalL()).degree();
    //drawer->draw(Vector2D(-1.0 * (_FIELD_WIDTH - _GOAL_WIDTH), 0), QColor(Qt::red));
    // double RangeofAngle2 = Vector2D::angleOf(wm->field->ou,Vector2D(0, -1.0 * (_FIELD_WIDTH - _GOAL_WIDTH)), wm->field->ourGoalL()).degree();

    double RangeofDistancetoBall = fabs(Segment2D(Vector2D(wm->field->_FIELD_WIDTH/2,wm->field->_FIELD_HEIGHT /2), Vector2D(-1.0 * wm->field->_FIELD_WIDTH/2,-1.0 * wm->field->_FIELD_HEIGHT /2)).length());

    double RangeofDistancetoGoal = fabs(Segment2D(Vector2D(wm->field->_FIELD_WIDTH/2,wm->field->_FIELD_HEIGHT /2), wm->field->ourGoal()).length());

    //double RangeofTempDis = 2;
    double angle, distancetoball, distancetogoal,danger1  = 0;


    QPair<Vector2D, double> temp;
    QList<QPair<Vector2D, double> > output;
    double Polycontain = 0;
    for(int i = 0; i<oppposdanger.count(); i++) {
        if(_poly.contains(oppposdanger[i]))
        {
            Polycontain = 1;
        }
        else
        {
            Polycontain = 0;
        }
        temp.first = oppposdanger[i];


        angle = Vector2D::angleOf(wm->field->ourGoalR(), oppposdanger[i], wm->field->ourGoalL() ).degree();
        distancetoball =  (oppposdanger[i] - wm->ball->pos).length();
        distancetogoal =  (oppposdanger[i] - wm->field->ourGoal()).length();

        ////poly

        AngleP = Vector2D::angleOf( oppposdanger[i], wm->ball->pos, _path.projection(oppposdanger[i]) ).degree();
        distanceToIntersectP = _path.dist(oppposdanger[i]); //distanse of opponent to the path
        distanceToBallProjectionP = (wm->ball->pos - _path.projection(oppposdanger[i])).length(); //distance of the ball to the projection of opponent to the path

        danger1 = (KA * fabs(angle) / RangeofAngle) + ( KDB *( 1 - (distancetoball / RangeofDistancetoBall)) ) + (KDG * (1 -(distancetogoal / RangeofDistancetoGoal)));
        danger2 = KAP * ( 1 - AngleP/RangeofAngleP) + KDBP * (1 - distanceToBallProjectionP/RangeofdistanceToBallProjectionP ) + KDIP * (1 - distanceToIntersectP / RangeofdistanceToIntersectP);
        // DBUG(QString("angle: %1, rangeofangle: %2, distansetoball:%3, RangeofDistancetoBall:%4,distancetogoal:%5,rangeofdistansetogoal:%6").arg(angle).arg(RangeofAngle).arg(distancetoball).arg(RangeofDistancetoBall).arg(distancetogoal).arg(RangeofDistancetoGoal),D_HAMED);
        // DBUG(QString("angleP: %1, rangeofangleP: %2, distansetoballProjectionP:%3, RangeofDistancetoBallProjectionP:%4,distancetointersect:%5,rangeofdistansetointesrsect:%6").arg(AngleP).arg(RangeofAngleP).arg(distanceToBallProjectionP).arg(RangeofdistanceToBallProjectionP).arg(distanceToIntersectP).arg(RangeofdistanceToIntersectP),D_HAMED);
        if( wm->ball->vel.length() < .1)
            danger = danger1;
        else
            danger = 100.0 * Polycontain + fabs(danger2) + fabs(danger1);

        temp.second = danger;
        output.append(temp);
        drawer->draw(QString("HMD danger=%1").arg(danger), oppposdanger[i] + Vector2D(0,0.3), QColor(Qt::red));
        //drawer->draw(_poly, QColor(Qt::blue));



        //        drawer->draw(QString("mindistance%1").arg(mintempdis), oppposdanger[i] + Vector2D(0,0.5), QColor(Qt::blue));
    }
    ///sorting the Qlist
    for(int i = 0; i< output.count(); i++)
    {
        for(int j = 0; j< output.count() - 1; j++ )
        {
            if(output[j].second < output[j + 1].second)
                output.swap(j, j+1);
        }
    }

    return output;
}

QList<QPair<Vector2D, double> > DefensePlan::sortdangerpassplayon(QList<Vector2D> oppposdanger) {

    double KA = 1; //Angle Coefficient
    double KDB = 1;  //Distance To Ball
    double KDG = 1;  //Distnce To Goal
    double RangeofAngle = Vector2D::angleOf(wm->field->ourGoalR(),
                                            Vector2D(-1.0 * (wm->field->_FIELD_WIDTH / 2 - wm->field->_PENALTY_DEPTH), 0),
                                            wm->field->ourGoalL()).degree();
    //drawer->draw(Vector2D(-1.0 * (_FIELD_WIDTH - _GOAL_WIDTH), 0), QColor(Qt::red));
    // double RangeofAngle2 = Vector2D::angleOf(wm->field->ou,Vector2D(0, -1.0 * (_FIELD_WIDTH - _GOAL_WIDTH)), wm->field->ourGoalL()).degree();

    double RangeofDistancetoBall = fabs(Segment2D(Vector2D(wm->field->_FIELD_WIDTH / 2, wm->field->_FIELD_HEIGHT / 2),
                                                  Vector2D(-1.0 * wm->field->_FIELD_WIDTH / 2,
                                                           -1.0 * wm->field->_FIELD_HEIGHT / 2)).length());

    double RangeofDistancetoGoal = fabs(Segment2D(Vector2D(wm->field->_FIELD_WIDTH / 2, wm->field->_FIELD_HEIGHT / 2),
                                                  wm->field->ourGoal()).length());

    double RangeofTempDis = 2;
    double angle, distancetoball, distancetogoal, danger;


    QPair<Vector2D, double> temp;
    QList<QPair<Vector2D, double> > output;
    for (int i = 0; i < oppposdanger.count(); i++) {
        temp.first = oppposdanger[i];


        angle = Vector2D::angleOf(wm->field->ourGoalR(), oppposdanger[i], wm->field->ourGoalL()).degree();
        distancetoball = (oppposdanger[i] - wm->ball->pos).length();
        distancetogoal = (oppposdanger[i] - wm->field->ourGoal()).length();
        danger = (KA * fabs(angle) / RangeofAngle) + (KDB * 1 - (distancetoball / RangeofDistancetoBall)) +
                (KDG * 1 - (distancetogoal / RangeofDistancetoGoal));


        temp.second = danger;
        output.append(temp);
        //drawer->draw(QString("HMD danger=%1").arg(danger), oppposdanger[i] + Vector2D(0,0.3), QColor(Qt::red));


        // finding nearest to intersect
        Segment2D tempsegment;
        tempsegment.assign(oppposdanger[i], wm->field->ourGoal());

        double mintempdis = 0.0;
        if (wm->our.activeAgentsCount() != 0)
            mintempdis = tempsegment.dist(wm->our.active(0)->pos);

        for (int j = 0; j < wm->our.activeAgentsCount(); j++) {
            if (tempsegment.dist(wm->our.active(j)->pos) < mintempdis) {
                mintempdis = tempsegment.dist(wm->our.active(j)->pos);
            }

        }


        //        drawer->draw(QString("mindistance%1").arg(mintempdis), oppposdanger[i] + Vector2D(0,0.5), QColor(Qt::blue));




    }

    ///sorting the Qlist
    for (int i = 0; i < output.count(); i++) {
        for (int j = 0; j < output.count() - 1; j++) {
            if (output[j].second < output[j + 1].second)
                output.swap(j, j + 1);
        }
    }

    //for(int i=0; i<output.count(); i++)
    //{
    //drawer->draw(QString("HMD Danger New%1" ).arg(output[i].second),output[i].first + Vector2D(0,.2),QColor(Qt::red));
    //}

    return output;
}
