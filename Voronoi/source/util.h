#ifndef UTIL_H
#define UTIL_H
#include "Curve.h"
#include "Geom.h"
#include "BCA.h"
#include <GL/freeglut.h>

static void dbg(){
    cout<<endl;
}

template<class T1>
static void dbg(const T1& t1){
    cout<<t1<<endl;
}

template<class T1,class T2>
static void dbg(const T1& t1,const T2& t2){
    cout<<t1<<"\t"<<t2<<endl;
}


template<class T1,class T2,class T3>
static void dbg(const T1& t1,const T2& t2,const T3& t3){
    cout<<t1<<"\t"<<t2<<"\t"<<t3<<endl;
}

template<class T1,class T2,class T3,class T4>
static void dbg(const T1& t1,const T2& t2,const T3& t3,const T4& t4){
    cout<<t1<<"\t"<<t2<<"\t"<<t3<<"\t"<<t4<<endl;
}

template<class T1,class T2,class T3,class T4,class T5>
static void dbg(const T1& t1,const T2& t2,const T3& t3,const T4& t4,const T5& t5){
    cout<<t1<<"\t"<<t2<<"\t"<<t3<<"\t"<<t4<<"\t"<<t5<<endl;
}

static void vertex(const Point& p){
    glVertex2d(p.x,p.y);
}


static void gray(){
    glColor3f(.7,.7,.7);
}

static void black(){
    glColor3f(0,0,0);
}
static void red(){
    glColor3f(1,0,0);
}

static void blue(){
    glColor3f(0,0,1);
}

static void green(){
    glColor3f(0,1,0);
}

static void white(){
    glColor3f(1,1,1);
}

static void color(double t,double v=1.){
    t-=floor(t);
    t*=6;
    double x=t-2*(int)floor(t/2);
    x=1-abs(x-1);
    switch((int)floor(t)){
    case 0:
        glColor3f(1,x,0);
        break;

    case 1:
        glColor3f(x,1,0);
        break;
    case 2:
        glColor3f(0,1,x);
        break;
    case 3:
        glColor3f(0,x,1);
        break;
    case 4:
        glColor3f(x,0,1);
        break;
    default:
        glColor3f(1,0,x);
        break;
    }
}


extern double xmin,xmax;

static void draw(const Point& p){
    glBegin(GL_POLYGON);
    for(int i=0;i<24;i++){
        double t=2*M_PI*i/24;
        vertex(p+(xmax-xmin)/240.*Point(cos(t),sin(t)));
    }
    glEnd();
}

static void draw(const Geometry& g){
    Point p=get<0>(g);
    glBegin(GL_POLYGON);
    for(int i=0;i<24;i++){
        double t=2*M_PI*i/24;
        vertex(p+(xmax-xmin)/240.*Point(cos(t),sin(t)));
    }
    glEnd();
}

static void draw(const Point& p,const Point& q){
    glBegin(GL_LINES);
    vertex(p);
    vertex(q);
    glEnd();
}
/*
static void draw(const Geometry& g0,const Geometry& g1){
    Point p=get<0>(g0),q=get<0>(g1);
    glBegin(GL_LINES);
    vertex(p);
    vertex(q);
    glEnd();
}
*/
static void draw(const Curve& b,int n,int size){
    glBegin(GL_LINE_STRIP);
    for(int i=0;i<=100;i++){
        double t=i/100.;
        color((n+t)/size);
        vertex(get<0>(b(t)));
    }
    glEnd();
}

static void draw(const vector<Curve>& c){
    for(auto& b:c){
        glBegin(GL_LINE_STRIP);
        for(int i=0;i<=100;i++){
            double t=i/100.;
            vertex(get<0>(b(t)));
        }
        glEnd();
    }
}


static void draw(const Curve& b){
    glBegin(GL_LINE_STRIP);
    for(int i=0;i<=100;i++){
        double t=i/100.;
        vertex(get<0>(b(t)));
    }
    glEnd();
}

static void draw(const Curve& b,double r){
    glBegin(GL_LINE_STRIP);
    for(int i=0;i<=100;i++){
        double t=i/100.;
        vertex(get<0>(b(t))+get<1>(b(t)).rotate()*r);
    }
    glEnd();
}

static void draw(const Circle& c){
    glBegin(GL_LINE_STRIP);
    for(int i=0;i<=500;i++){
        double t=i/500.*2*M_PI;
        vertex(c.p+c.r*Point(cos(t),sin(t)));
    }
    glEnd();
}

static void draw(const CArc& a){
    if(a.c.r>1e6){
        draw(a.x[0],a.x[1]);
        return;
    }

    glBegin(GL_LINE_STRIP);
    for(int i=0;i<=100;i++){
        double t=i/100.;
        vertex(a.c.p+a.c.r*((1-t)*a.n[0]+t*a.n[1]).normal());
    }
    glEnd();
}

static void draw(const BCA& bca){
    draw(bca.arc[0]);
    draw(bca.arc[1]);
}


static void draw(const LSS& lss){
    draw(Circle(lss.p[0],lss.e));
    draw(Circle(lss.p[1],lss.e));

    Point dir=(lss.p[1]-lss.p[0]).rotate().normal()*lss.e;
    draw(lss.p[0]+dir,lss.p[1]+dir);
    draw(lss.p[0]-dir,lss.p[1]-dir);
}

static double unif(){
    return rand()*1./RAND_MAX;
}

static double timer[4]={0,0,0,0};

static void show_time(int n=0){
    printf("%7.3lf",timer[n]/1e6);
    cout<<"ms\t"<<n<<endl;
    timer[n]=0;
}


double gettime();

static void tic(int n=0){
    timer[n]-=gettime();
}

static void toc(int n=0){
    timer[n]+=gettime();
}

#endif // UTIL_H
