//
// Created by mahi on 4/2/18.
//

#ifndef PARSIAN_AI_MAHI_H
#define PARSIAN_AI_MAHI_H


inline double getangle(double x1, double y1, double x2, double y2) {
    return atan2(y2 - y1, x2 - x1);
}

struct range {
    double a, b;
};


inline double len(double x1, double y1, double x2, double y2) {
    return hypot(x1 - x2, y1 - y2);
}

inline double len2(double x1, double y1, double x2, double y2) {
    return (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);
}


inline double normalang(double dir) {
    const double _2PI = 2.0 * M_PI;
    if (dir < -2.0 * M_PI || 2.0 * M_PI < dir) {
        dir = fmod(dir, _2PI);
    }
    if (dir < -M_PI) {
        dir += 2.0 * M_PI;
    }
    if (dir > M_PI) {
        dir -= 2.0 * M_PI;
    }
    return dir;
}

inline double normalangabs(double dir) {
    const double _2PI = 2.0 * M_PI;
    if (dir < -2.0 * M_PI || 2.0 * M_PI < dir) {
        dir = fmod(dir, _2PI);
    }
    if (dir < -M_PI) {
        dir += 2.0 * M_PI;
    }
    if (dir > M_PI) {
        dir -= 2.0 * M_PI;
    }
    if (dir < 0) {
        return -dir;
    }
    return dir;
}

double getEmptyAngle(const Vector2D& p, const Vector2D& p1, const Vector2D& p2, const QList<Circle2D>& obs, double &percent, double &mostOpenAngle, double &biggestAngle) {

    double gx1 = p1.x;
    double gy1 = p1.y;
    double gx2 = p2.x;
    double gy2 = p2.y;
    double x = p.x;
    double y = p.y;
    double d, a1, a2, a, l, a0, q1, q2, al;
    double la, lb, lc;
    double ox, oy;
    la = gy2 - gy1;
    lb = gx1 - gx2;
    lc = -gx1 * la - gy1 * lb;
    bool inobs = false;
    range tmpr{};
    int count = 0;
    int i, j;
    d = 0;
    range r[20];
    bool flag[20];
    for (i = 0; i < 20; i++) {
        flag[i] = false;
    }
    al = getangle(x, y, (gx1 + gx2) * 0.5, (gy1 + gy2) * 0.5);
    q1 = getangle(x, y, gx1, gy1) - al;
    q2 = getangle(x, y, gx2, gy2) - al;
    q1 = normalang(q1);
    q2 = normalang(q2);
    if (normalang(q1 - q2) > 0) {
        a = q1;
        q1 = q2;
        q2 = a;
    }
    double openangle = 0;
    for (i = 0; i < obs.count(); ++i) {
        ox = obs[i].center().x;
        oy = obs[i].center().y;
        double rad = obs[i].radius();
        l = len(x, y, ox, oy);
        if (l < rad) {
            inobs = true;
            break;
        }
        a1 = ox * la + oy * lb + lc;
        a2 = x * la + y * lb + lc;
        if (a1 > 0) {
            a1 = 1;
        } else {
            a1 = -1;
        }
        if (a2 > 0) {
            a2 = 1;
        } else {
            a2 = -1;
        }
        a1 = a1 * a2;
        if (a1 > 0) {
            a = normalang(getangle(x, y, ox, oy) - al);
            a0 = asin(rad / l);
            a1 = a - a0;
            a2 = a + a0;
            if (a1 < -M_PI * 0.95 * 0.5) {
                a1 = -M_PI * 0.95 * 0.5;
            }
            if (a2 < -M_PI * 0.95 * 0.5) {
                a2 = -M_PI * 0.95 * 0.5;
            }
            if (a1 > +M_PI * 0.95 * 0.5) {
                a1 =  M_PI * 0.95 * 0.5;
            }
            if (a2 > +M_PI * 0.95 * 0.5) {
                a2 =  M_PI * 0.95 * 0.5;
            }
            if (normalang(a1 - a2) > 0) {
                a = a1;
                a1 = a2;
                a2 = a;
            }
            a1 = normalang(a1);
            a2 = normalang(a2);
            if (normalang(a1 - q1) <= 0) {
                a1 = q1;
            }
            if (normalang(a1 - q2) >= 0) {
                a1 = q2;
            }
            if (normalang(a2 - q1) <= 0) {
                a2 = q1;
            }
            if (normalang(a2 - q2) >= 0) {
                a2 = q2;
            }
            if (normalangabs(a1 - a2) >= 0.001) {
                r[count].a = a1;
                r[count].b = a2;
                count ++;
            }
        }
    }
    if (!inobs) {
        for (i = 0; i < count; i++)
            for (j = 0; j < count - 1; j++) {
                if (normalang(r[j].a - r[j + 1].a) > 0) {
                    tmpr = r[j];
                    r[j] = r[j + 1];
                    r[j + 1] = tmpr;
                }
            }
        for (i = 0; i < count - 1; i++) {
            if (normalang(r[i + 1].a - r[i].b) < 0) {
                r[i + 1].a = r[i].a;
                if (normalang(r[i + 1].b - r[i].b) < 0) {
                    r[i + 1].b = r[i].b;
                }
                flag[i] = true;
            }
        }
        bool changed = false;
        if (count > 0) {
            double lastBlockedDir = q1;
            int k = 0;
            mostOpenAngle = 0;
            biggestAngle = 0;
            for (i = 0; i < count; i++) {
                if (! flag[i]) {
                    if ((k == 0) && (normalang(q1 - r[i].a) >= 0)) {

                    } else {
                        double dist = normalangabs(r[i].a - lastBlockedDir);
                        double bisect = normalang(normalang(r[i].a - lastBlockedDir) / 2.0 + lastBlockedDir);
                        if (dist >= biggestAngle) {
                            biggestAngle = dist;
                            mostOpenAngle = bisect;
                            changed = true;
                        }
                    }
                    lastBlockedDir = r[i].b;
                    k ++;
                }
            }
            if (normalang(r[count - 1].b - q2) <= 0) {
                double dist = normalangabs(q2 - lastBlockedDir);
                double bisect = normalang(normalang(q2 - lastBlockedDir) / 2.0 + lastBlockedDir);
                if (dist >= biggestAngle) {
                    biggestAngle = dist;
                    mostOpenAngle = bisect;
                    changed = true;
                }
            }
        }
        for (i = 0; i < count; i++) {
            if (! flag[i]) {
                d += normalang(r[i].b - r[i].a);
                flag[i] = true;
            }
        }
        openangle = d;
        openangle = normalangabs(q2 - q1) - openangle;
        d /= normalangabs(q2 - q1);
        d = 1 - d;
        if (!changed || (d < 0.001)) {
            if (count == 0) {
                biggestAngle = normalangabs(q2 - q1);
            } else {
                biggestAngle = 0;
            }
            mostOpenAngle = normalang(normalang(q2 - q1) / 2.0 + q1);
        }
        mostOpenAngle = normalang(mostOpenAngle + al);
        biggestAngle *= 180.0 / M_PI;
        mostOpenAngle *= 180.0 / M_PI;
    } else {
        mostOpenAngle = normalang(0.5 * (q1 + q2) + al);
        biggestAngle = 0;
        mostOpenAngle *= 180.0 / M_PI;
        d = 0.0;
    }
    percent = d;

    return openangle * 180.0 / M_PI;
}



#endif //PARSIAN_AI_MAHI_H
