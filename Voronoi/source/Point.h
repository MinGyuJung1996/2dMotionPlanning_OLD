#ifndef POINT_H
#define POINT_H
#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
using namespace std;

class Line;

class Point
{
public:
    double x,y;
    inline Point(const double x=0.,const double y=0.):x(x),y(y){}
    inline Point(const Line& l0,const Line& l1);
    inline const Point operator+(const Point& rhs) const{
        return Point(x+rhs.x,y+rhs.y);
    }
    inline const Point operator-(const Point& rhs) const{
        return Point(x-rhs.x,y-rhs.y);
    }
    inline const Point operator-() const{
        return Point(-x,-y);
    }
    inline const Point operator*(const double rhs) const{
        return Point(x*rhs,y*rhs);
    }
    inline friend const Point operator*(const double lhs,const Point& rhs){
        return rhs*lhs;
    }
    inline const Point operator/(const double rhs) const{
        return Point(x/rhs,y/rhs);
    }
    inline double dot(const Point& rhs) const{
        return x*rhs.x+y*rhs.y;
    }
    inline double cross(const Point& rhs) const{
        return x*rhs.y-y*rhs.x;
    }
    inline bool operator<(const Point& rhs) const{
        return cross(rhs)>0;
    }
    inline double length() const{
        return sqrt(dot(*this));
    }
    inline double distance(const Point& rhs) const{
        return (*this-rhs).length();
    }
    inline double cos(const Point& rhs) const{
        return min(1.,dot(rhs)/(length()*rhs.length()));
    }
    inline const Point normal() const{
        return *this/length();
    }
    inline const Point rotate() const{
        return Point(-y,x);
    }
    inline double atan() const{
        return atan2(y,x);
    }
    friend ostream& operator<<(ostream& os,const Point& rhs){
        return os<<rhs.x<<" "<<rhs.y<<" ";
    }
};
typedef tuple<Point,Point,double> Geometry;

#endif // POINT_H
