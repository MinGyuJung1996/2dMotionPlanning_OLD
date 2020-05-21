#ifndef BCA_H
#define BCA_H
#include "Bezier.h"
#include <tuple>
#include "Geom.h"
using namespace std;


class LSS{
public:
    Point p[2];
    Line l;
    double e;

    inline LSS(){}

    inline LSS(const Point& p0,const Point& p1,double e):l(p0,p1),e(e){
        p[0]=p0;
        p[1]=p1;
    }

    //warning
    inline double distance(const Point& q) const{
        if((p[1]-p[0]).dot(q-p[0])<0)
            return p[0].distance(q);
        if((p[0]-p[1]).dot(q-p[1])<0)
            return p[1].distance(q);
        return abs(l.signed_distance(q));
    }

    inline double distance(const LSS& rhs) const{
        double d[4]={
            l.signed_distance(rhs.p[0]),
            l.signed_distance(rhs.p[1]),
            rhs.l.signed_distance(p[0]),
            rhs.l.signed_distance(p[1]),
        };

        if(d[0]*d[1]<0&&d[2]*d[3]<0)
            return 0;

        double dd[4]={
            distance(rhs.p[0]),
            distance(rhs.p[1]),
            rhs.distance(p[0]),
            rhs.distance(p[1]),
        };

        return max(0.,min(min(min(dd[0],dd[1]),dd[2]),dd[3])-e-rhs.e);
    }
};

inline Geometry get_geometry(const Point& x0,const Point& v0,const Point& a0){
    double v2=v0.dot(v0);
    double v1=sqrt(v2);
    return make_tuple(x0,v0/v1,v0.cross(a0)/(v2*v1));
}

class BCA{
public:

    Bezier x,v,a;
    Geometry p[2];
    int n;
    double t[2];
    BCA* child[2];
    CArc arc[2];
    LSS lss;

    inline BCA(){child[0]=child[1]=nullptr;}

    inline ~BCA(){
        if(child[0]){
            delete child[0];
            delete child[1];
        }
    }
    inline BCA(const BCA& rhs){
        x=rhs.x;
        v=rhs.v;
        a=rhs.a;
        p[0]=rhs.p[0];
        p[1]=rhs.p[1];
        n=rhs.n;
        t[0]=rhs.t[0];
        t[1]=rhs.t[1];
        child[0]=child[1]=nullptr;
        arc[0]=rhs.arc[0];
        arc[1]=rhs.arc[1];
        lss=rhs.lss;
    }
    inline BCA& operator=(const BCA& rhs){
        cout<<"error"<<endl;
        return *this;
    }

    inline BCA(const Bezier& x,const Bezier& v,const Bezier& a):x(x),v(v),a(a){
        p[0]=(*this)(0);
        p[1]=(*this)(1);
        t[0]=0;
        t[1]=1;
        set_arc();
        child[0]=child[1]=nullptr;
    }

    inline BCA(const Bezier& x,const Bezier& v,const Bezier& a,
               const Geometry& p0,const Geometry& p1,
               double t0,double t1):x(x),v(v),a(a){
        p[0]=p0;
        p[1]=p1;
        t[0]=t0;
        t[1]=t1;
        set_arc();
        child[0]=child[1]=nullptr;
    }

    inline const tuple<Point,Point,double> operator()(double t) const{
        return get_geometry(x(t),v(t),a(t));
    }

    inline void set_arc(){
        arc[0]=CArc(get<0>(p[0]),get<1>(p[0]),get<0>(p[1]));
        arc[1]=CArc(get<0>(p[1]),get<1>(p[1]),get<0>(p[0]));

        if(arc[0].c.r>arc[1].c.r)
            swap(arc[0],arc[1]);

        Point mid=(get<0>(p[0])+get<0>(p[1]))/2;
        mid=mid-arc[0].c.p;
        double e=(arc[0].c.r-mid.length())/2;
        mid=mid.normal()*e;
        lss=LSS(get<0>(p[0])+mid,get<0>(p[1])+mid,e);
    }

    inline void set_child(){
        if(child[0])
            return;
        auto mid=(*this)(.5);
        child[0]=new BCA(x.subdiv(0,.5),v.subdiv(0,.5),a.subdiv(0.,5),p[0],mid,t[0],(t[0]+t[1])/2);
        child[1]=new BCA(x.subdiv(.5,1),v.subdiv(.5,1),a.subdiv(.5,1),mid,p[1],(t[0]+t[1])/2,t[1]);

        child[0]->n=child[1]->n=n;
    }

    inline bool inside(const Point& p) const{
        return arc[0].c.inside(p) && (!arc[1].c.inside(p));
    }

    inline double param() const{
        return n+(t[0]+t[1])/2;
    }

    void set_n(int n){
        if(child[0]){
            child[0]->set_n(n);
            child[1]->set_n(n);
        }
        this->n=n;
    }
};


struct BCA_pair{
    BCA* bca[2];
    double d_min,d_max;

    inline BCA_pair(BCA* bca0,BCA* bca1){
        bca[0]=bca0;
        bca[1]=bca1;

        Point p[4]={
            get<0>(bca0->p[0]),
            get<0>(bca0->p[1]),
            get<0>(bca1->p[0]),
            get<0>(bca1->p[1])
        };

        double d[4]={
            p[0].distance(p[2]),
            p[0].distance(p[3]),
            p[1].distance(p[2]),
            p[1].distance(p[3]),
        };

        d_max=min(min(min(d[0],d[1]),d[2]),d[3]);

        d_min=bca0->lss.distance(bca1->lss);


    }

    inline bool operator<(const BCA_pair& rhs) const{
        return rhs.d_min<d_min;
    }
};

struct BCA_proj{
    BCA* bca;
    double d_min,d_max;

    inline BCA_proj(BCA* bca,const Point& q):bca(bca){
        Point p[2]={
            get<0>(bca->p[0]),
            get<0>(bca->p[1]),
        };
        double d[2]={
            p[0].distance(q),
            p[1].distance(q),
        };
        d_max=min(d[0],d[1]);

        d_min=max(0.,bca->lss.distance(q)-bca->lss.e);

    }
    inline bool operator<(const BCA_proj& rhs) const{
        return rhs.d_min<d_min;
    }
};

#endif // BCA_H
