#ifndef PARSIAN_UTIL_FIELD_H
#define PARSIAN_UTIL_FIELD_H

#include <QtCore/QString>
#include <QtCore/QList>


#include "parsian_util/geom/geom.h"
#include "ros/ros.h"

#include <QtCore/QStringList>


class CField {
public:

    /* Field */
    double _FIELD_WIDTH;
    double _FIELD_HEIGHT;
    double _FIELD_MARGIN_HEIGHT;
    double _FIELD_MARGIN_WIDTH;
    double _FIELD_PENALTY;
    double _GOAL_WIDTH;
    double _GOAL_RAD;
    double _GOAL_DEPTH;
    double _PENALTY_WIDTH;
    double _CENTER_CIRCLE_RAD;
    double _PENALTY_AREA_CIRCLE_X;
    double _PENALTY_AREA_CIRCLE_RAD;
    double _PARSIAN_BOUNDERY;
    double _MIXTEAM_FIELD_WIDTH;
    double _MIXTEAM_FIELD_HEIGHT;


    enum class Region {
        OurCornerTop = 1 ,
        OurCornerBottom = 2 ,
        OppCornerTop = 3 ,
        OppCornerBottom = 4 ,
        MidFieldTop = 5 ,
        MidFieldBottom = 6 ,
        FrontGoalOpp = 7 ,
        FrontGoalOur = 8 ,
        Center = 9 ,
        OurPenalty = 10 ,
        OppPenalty = 11 ,
        OurField = 12 ,
        OppField = 13 ,
        OppCornerTopRight = 14 ,
        OppCornerTopLeft = 15 ,
        OppCornerBottomRight = 16 ,
        OppCornerBottomLeft = 17 ,
        OurMidFieldTop = 18 ,
        OurMidFieldBottom = 19 ,
        OppMidFieldTop = 20 ,
        OppMidFieldBottom = 21 ,
        OppFieldBottom = 22 ,
        OppFieldTop = 23 ,
        OurFieldTop = 24 ,
        OurFieldBottom = 25 ,
        OurMidFieldTopMost = 26 ,
        OurMidFieldBottomMost = 27 ,
        Field1stQuarter = 28 ,
        Field2ndQuarter = 29 ,
        Field3rdQuarter = 30 ,
        Field4thQuarter = 31 ,
        OurMidFieldTopWing = 32 ,
        OurMidFieldBottomWing = 33 ,
        // For mani
        //----------------------
        OurDeffenseOneThird = 34 ,
        OurAttackOneThird = 35 ,
        OurMidOneThird = 36 ,
        //----------------------
        OurCornerTopRight = 37 ,
        OurCornerTopLeft = 38 ,
        OurCornerBottomRight = 39 ,
        OurCornerBottomLeft = 40 ,
        OppMidField = 41 ,
        OppCornerLineTop = 42 ,
        OppCornerLineBottom = 43 ,
        FieldGrid1Top = 44 ,
        FieldGrid1Center = 45 ,
        FieldGrid1Bottom = 46 ,
        FieldGrid2Top = 47 ,
        FieldGrid2Center = 48 ,
        FieldGrid2Bottom = 49 ,
        FieldGrid3Top = 50 ,
        FieldGrid3Center = 51 ,
        FieldGrid3Bottom = 52 ,
        FieldGrid4Top = 53 ,
        FieldGrid4Center = 54 ,
        FieldGrid4Bottom = 55 ,
        FieldGrid5Top = 56 ,
        FieldGrid5Center = 57 ,
        FieldGrid5Bottom = 58 ,
        OppCornerPointTop = 59 ,
        OppCornerPointBottom = 60 ,
        TheirPenaltyTop = 61 ,
        TheirPenaltyBottom = 62 ,
        AttackRecCornerTopB = 63 ,
        AttackRecCornerTopP1 = 64 ,
        AttackRecCornerTopP2 = 65 ,
        AttackRecCornerBottomB = 66 ,
        AttackRecCornerBottomP1 = 67 ,
        AttackRecCornerBottomp2 = 68 ,
        AttackRecMidTopB = 69 ,
        AttackRecMidTopP1 = 70 ,
        AttackRecMidTopP2 = 71 ,
        AttackRecMidBottomB = 72 ,
        AttackRecMidBottomP1 = 73 ,
        AttackRecMidBottomp2 = 74 ,
        TC2012Top1 = 75 ,
        TC2012Top2 = 76 ,
        TC2012Top3 = 77 ,
        TC2012Top4 = 78 ,
        TC2012Bottom1 = 79 ,
        TC2012Bottom2 = 80 ,
        TC2012Bottom3 = 81 ,
        TC2012Bottom4 = 82 ,
        TC2012Rect1 = 83 ,
        TC2012Rect2 = 84 ,
        TC2012Rect3 = 85 ,
        TC2012Rect4 = 86 ,
        OurCornerPointTop = 87 ,
        OurCornerPointBottom = 88 ,
        LargeOppCornerTop = 89 ,
        LargeOppCornerBottom = 90 ,
        LargeOppCornerTopRight = 91 ,
        LargeOppCornerTopLeft = 92 ,
        LargeOppCornerBottomRight = 93 ,
        LargeOppCornerBottomLeft = 94 ,
        BigCenter = 95
    };

    CField ();

    ~CField ();

    Vector2D center ();

    Vector2D ourGoal ();

    Vector2D oppGoal ();

    Vector2D ourCornerL ();

    Vector2D ourCornerR ();

    Vector2D oppCornerL ();

    Vector2D oppCornerR ();

    Vector2D ourPenalty ();

    Vector2D oppPenalty ();

    Vector2D ourGoalL ();

    Vector2D ourGoalR ();

    Vector2D oppGoalL ();

    Vector2D oppGoalR ();

    Vector2D ourOneThirdL ();

    Vector2D ourOneThirdR ();

    Vector2D oppOneThirdL ();

    Vector2D oppOneThirdR ();

    Rect2D fieldRect ();

    Rect2D marginedField ();

    Rect2D ourPenaltyRect ();

    Rect2D oppPenaltyRect ();

    bool isInField (Vector2D point);

    bool isInOurPenaltyArea (Vector2D point);

    bool isInOppPenaltyArea (Vector2D point);

    Rect2D getRegion (Region region , double k = 0.25);

    Rect2D getRegion (QString name , double k = 0.25);

    Rect2D getCircleRegion (int n , int i);

    QList<Vector2D> ourBigPAreaIntersect (Line2D line , double scale = - 1 , float bias = 0);

    QList<Vector2D> ourPAreaIntersect (Line2D line);

    QList<Vector2D> ourPAreaIntersect (Segment2D segment);

    /////////////////////////////////////////////////////////////////////
    QList<Vector2D> ourPAreaIntersect (Circle2D circle);

    QList<Vector2D> oppPAreaIntersect (Line2D line);

    QList<Vector2D> oppPAreaIntersect (Segment2D segment);

    QList<Vector2D> oppPAreaIntersect (Circle2D circle);

    Vector2D ourPAreaPerpendicularVector (double angle , Vector2D &intersectpoint);

    double ourPAreaPos (double angle);

private:
    Vector2D fCenter;
    Vector2D fOurGoal;
    Vector2D fOppGoal;
    Vector2D fOurCornerL;
    Vector2D fOurCornerR;
    Vector2D fOppCornerL;
    Vector2D fOppCornerR;
    Vector2D fOurPenalty;
    Vector2D fOppPenalty;
    Vector2D fOurGoalL;
    Vector2D fOurGoalR;
    Vector2D fOppGoalL;
    Vector2D fOppGoalR;
    Rect2D fFieldRect;
    Rect2D fMarginedFieldRect;
    Rect2D fOurPenaltyRect;
    Rect2D fOppPenaltyRect;
    Vector2D fOurOneThirdL;
    Vector2D fOppOneThirdL;
    Vector2D fOurOneThirdR;
    Vector2D fOppOneThirdR;
};

#endif //PARSIAN_UTIL_FIELD_H
