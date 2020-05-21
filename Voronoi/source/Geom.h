#ifndef GEOM_H
#define GEOM_H
#include "Point.h"

class Line{
public:
    double a[3];
    inline Line(){}

    inline Line(const Point& p,const Point& q){
        a[0]=p.y-q.y;
        a[1]=q.x-p.x;
        a[2]=p.x*q.y-q.x*p.y;
    }
    inline static Line ray(const Point& p,const Point& v){
        return Line(p,p+v);
    }
    inline static Line medial(const Point& p,const Point& q){
        Point mid=(p+q)/2;
        Point dir=(q-p).rotate();
        return Line::ray(mid,dir);
    }
    inline double signed_distance(const Point& p) const{
        return (a[0]*p.x+a[1]*p.y+a[2])/sqrt(a[0]*a[0]+a[1]*a[1]);
    }
};

inline Point::Point(const Line& l0,const Line& l1){
    double a[3];
    a[0]=l0.a[1]*l1.a[2]-l0.a[2]*l1.a[1];
    a[1]=l0.a[2]*l1.a[0]-l0.a[0]*l1.a[2];
    a[2]=l0.a[0]*l1.a[1]-l0.a[1]*l1.a[0];
    x=a[0]/a[2];
    y=a[1]/a[2];
}

class Circle{
public:
    Point p;
    double r;
    inline Circle(){}
    inline Circle(Point p,double r):p(p),r(r){}
    inline Circle(Point x0,Point v0,Point x1){
        p=Point(Line::medial(x0,x1),Line(x0,x0+v0.rotate()));
        r=p.distance(x0);
    }
    inline bool inside(Point q) const{
        q=p-q;
        return q.dot(q)<r*r;
    }
    int intersect(const Circle& rhs,Point q[]){
        double d=p.distance(rhs.p);
        if(r+rhs.r<d)
            return 0;
        Point e1=(rhs.p-p).normal();
        Point e2=e1.rotate();
        double x=(d*d-rhs.r*rhs.r+r*r)/(2*d);
        double y=sqrt(r*r-x*x);
        e1=p+x*e1;
        e2=y*e2;
        q[0]=e1+e2;
        q[1]=e1-e2;
        return 2;
    }
    int intersect(const Geometry& g,Point q[]){
        double x=Line::ray(get<0>(g),get<1>(g)).signed_distance(p);
        if(r<x)
            return 0;
        double y=sqrt(r*r-x*x);
        Point e2=get<1>(g);
        Point e1=-e2.rotate();
        e1=p+x*e1;
        e2=y*e2;
        q[0]=e1+e2;
        q[1]=e1-e2;
        return 2;
    }
};

class CArc{
public:
    Circle c;
    Point x[2],n[2];
    //Point v[2];
    inline CArc(){}
    inline CArc(Point x0,Point v0,Point x1):
        c(Circle(x0,v0,x1)){
        x[0]=x0;
        x[1]=x1;
        n[0]=(x0-c.p).normal();
        n[1]=(x1-c.p).normal();
        //v[0]=n[0].rotate();
        //v[1]=n[1].rotate();
        if(n[1]<n[0]){
            swap(n[0],n[1]);
        }
    }
    inline double distance(const CArc& rhs) const{
        cout<<"NOT IMPLEMENTED "<<rhs.c.r<<endl;
        return 0;
    }
    inline double distance(const Point& p) const{
        Point q=p-c.p;
        if(n[0]<q && q<n[1]){
            return abs(q.length()-c.r);
        }
        return min(p.distance(x[0]),p.distance(x[1]));
    }
    /*
    inline Geometry endpoint(int i) const{
        if(i==0)
            return make_tuple(x[0],v[0],0);
        else
            return make_tuple(x[1],v[1],0);

    }
    */
};

#endif // GEOM_H


