#ifndef CURVE_H
#define CURVE_H
#include "BCA.h"
#include <tuple>
using namespace std;


class Curve{
public:
    BCA root;
    Point AABB[2];
    int direction;
    bool convex;

    inline Curve(){}
    inline Curve(const Bezier& x,const Bezier& v,const Bezier& a):root(x,v,a){
        AABB[0]=get<0>(root.p[0]);
        AABB[1]=get<0>(root.p[1]);

        if(AABB[0].x>AABB[1].x)
            swap(AABB[0].x,AABB[1].x);
        if(AABB[0].y>AABB[1].y)
            swap(AABB[0].y,AABB[1].y);

        auto sample=operator()(.5);

        double th=get<1>(sample).atan();



        if(th>=0){
            if(th<=M_PI/2)
                direction=1;
            else
                direction=2;
        }
        else{
            if(th<=-M_PI/2)
                direction=3;
            else
                direction=4;
        }

        convex=get<2>(sample)<0;
    }
    bool operator<(const Curve& rhs) const{
        return root.x<rhs.root.x;
    }


    inline const Curve subdiv(double t0,double t1) const{
        return Curve(root.x.subdiv(t0,t1),root.v.subdiv(t0,t1),root.a.subdiv(t0,t1));
    }

    inline const Curve reverse() const{
        return subdiv(1,0);
    }

    inline const tuple<Point,Point,double> operator()(double t) const{
        return root(t);
    }

    inline static vector<Curve> subdiv(const Bezier& bezier){
        vector<Curve> curves;

        Bezier first=bezier.deriv();
        Bezier second=first.deriv();
        Bezier third=second.deriv();
        auto zeros=bezier.get_zeros(first,second,third);

        for(int j=0;j+1<(int)zeros.size();j++){
            curves.push_back(Curve(
                bezier.subdiv(zeros[j],zeros[j+1]),
                 first.subdiv(zeros[j],zeros[j+1]),
                second.subdiv(zeros[j],zeros[j+1])));
        }
        return curves;
    }
};

#endif // CURVE_H
