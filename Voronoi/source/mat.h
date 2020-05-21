#ifndef MAT_H
#define MAT_H

#include "util.h"
#include <cfloat>
#include <vector>
#include <queue>
#include <algorithm>
#include <map>
#include <tuple>
using namespace std;


//globals




//find




template <class T>
static const T mean(const T& x,const T& y){
    return (x+y)/2;
}


const Point offset(const Geometry& p,double r);

extern int frame,frame2,frame3,frame4;


//functions
static double diameter(const vector<Curve>& loop){
    double x[2]={DBL_MAX,-DBL_MAX},y[2]={DBL_MAX,-DBL_MAX};
    for(auto& bezier:loop){
        Point p=get<0>(bezier(0)); // monotone loop
        x[0]=min(x[0],p.x);
        x[1]=max(x[1],p.x);
        y[0]=min(y[0],p.y);
        y[1]=max(y[1],p.y);
    }
    return Point(x[0],y[0]).distance(Point(x[1],y[1]));

}


static const Line tangent(const Curve& c,double t){
    auto& p=c(t);
    return Line::ray(get<0>(p),get<1>(p));
}
static const Point evolute(const Curve& c,double t){
    auto& p=c(t);
    return get<0>(p)+get<1>(p).rotate()/get<2>(p);
}

static const Point evolute(const Geometry& p){
    return get<0>(p)+get<1>(p).rotate()/get<2>(p);
}
static void offset(const Curve& c){
    for(int i=0;i<=30;i++){
        double r=i/10.*0.1;
        draw(c,r);
    }
}
static void offset(const vector<Curve>& loop,double r=0.){
    for(auto& c:loop){
        draw(c,r);
    }
}

//useless
static void bv(const Curve& c){
    glBegin(GL_LINE_LOOP);
    vertex(Point(get<0>(c(1))));
    vertex(evolute(c,1));
    vertex(evolute(c,0));
    vertex(Point(get<0>(c(0))));
    vertex(Point(tangent(c,0),tangent(c,1)));
    glEnd();
}


static void clean(vector<Curve>& loop){
    for(auto& curve:loop){
        if(curve.root.child[0]){
            delete curve.root.child[0];
            delete curve.root.child[1];
            curve.root.child[0]=curve.root.child[1]=nullptr;
        }
    }
}

#endif // MAT_H
