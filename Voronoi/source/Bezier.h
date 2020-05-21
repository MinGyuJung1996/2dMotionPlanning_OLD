#ifndef BEZIER_H
#define BEZIER_H
#include "Point.h"
#include "Poly.h"
#include <vector>
#include <iostream>
using namespace std;


class Bezier{
public:
    Point p[4];
    inline Bezier(){}
    inline Bezier(const Point& p0,const Point& p1,const Point& p2,const Point& p3){
        p[0]=p0; p[1]=p1; p[2]=p2; p[3]=p3;
    }
    const Point operator()(const double t) const{
        return p[0]+t*(p[1]+t*(p[2]+t*p[3]));
    }

    const Bezier deriv() const{
        return Bezier(p[1],p[2]*2,p[3]*3,0);
    }
    const Poly dot(const Bezier& rhs) const{
        Poly c;
        for(int i=0;i<4;i++)
            for(int j=0;j<4&&i+j<6;j++){
                c.a[i+j]+=p[i].dot(rhs.p[j]);
            }

        return c;
    }
    const Poly cross(const Bezier& rhs) const{
        Poly c;
        for(int i=0;i<4;i++)
            for(int j=0;j<4&&i+j<6;j++){
                c.a[i+j]+=p[i].cross(rhs.p[j]);
            }

        return c;
    }
    bool operator<(const Bezier& rhs) const{
        for(int i=0;i<4;i++)
            if(p[i]<rhs.p[i])
                return true;
        return false;
    }


    const Bezier subdiv(double t0,double t1) const{
        return translate(t0).scale(t1-t0);
    }

    const vector<double> get_zeros(const Bezier& first,const Bezier& second,const Bezier& third) const{
        set<double> zeros;
        set<double> z[4]={
            Poly(first.p[0].x,first.p[1].x,first.p[2].x).solve(),
            Poly(first.p[0].y,first.p[1].y,first.p[2].y).solve(),
            first.cross(second).solve(),
            (first.cross(third)*first.dot(first)
            -first.cross(second)*first.dot(second)*3).solve()
        };
        for(int i=0;i<4;i++)
            zeros.insert(z[i].begin(),z[i].end());
        zeros.insert(0);
        zeros.insert(1);
        return vector<double>(zeros.begin(),zeros.end());
    }

private:

    const Bezier translate(double t0) const{
        Point q[4]={
            p[0]+t0*p[1]+t0*t0*p[2]+t0*t0*t0*p[3],
            p[1]+2*t0*p[2]+3*t0*t0*p[3],
            p[2]+3*t0*p[3],
            p[3]};
        return Bezier(q[0],q[1],q[2],q[3]);
    }

    const Bezier scale(double k) const{
        return Bezier(p[0],p[1]*k,p[2]*k*k,p[3]*k*k*k);
    }

};

#endif // BEZIER_H
