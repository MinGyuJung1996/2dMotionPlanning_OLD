#include "BCA.h"
#include "Curve.h"
#include "util.h"
#include <queue>
using namespace std;

double distance(vector<Curve>& loop,const Point& q){
    priority_queue<BCA_proj> pq;
    for(auto& c:loop){
        pq.push(BCA_proj(&c.root,q));
    }
    while(true){
        BCA_proj p=pq.top();
        pq.pop();
        if(p.d_max-p.d_min<1e-6)
            return p.d_min;
        p.bca->set_child();
        pq.push(BCA_proj(p.bca->child[0],q));
        pq.push(BCA_proj(p.bca->child[1],q));
    }
}


double footpoint(vector<Curve>& loop,const Point& q,int i,int j){
    priority_queue<BCA_proj> pq;
    int n=0;
    for(auto& c:loop){
        //if(n!=i&&n!=j)
            pq.push(BCA_proj(&c.root,q));
        n++;
    }

    while(true){
        BCA_proj p=pq.top();
        pq.pop();
        if(p.d_max-p.d_min<1e-9){
            //draw(p.bca->p[0]);
            return p.bca->n+(p.bca->t[0]+p.bca->t[1])/2;
        }
        p.bca->set_child();
        pq.push(BCA_proj(p.bca->child[0],q));
        pq.push(BCA_proj(p.bca->child[1],q));
    }
}


bool live(vector<Curve>& loop,const Point& q,double r,int i){
    priority_queue<BCA_proj> pq;
    int n=0;
    for(auto& c:loop){
        if(n!=i)
            pq.push(BCA_proj(&c.root,q));
        n++;
    }

    while(!pq.empty()){
        BCA_proj p=pq.top();
        pq.pop();

        if(p.d_max<r-1e-8){
            return false;
        }
        if(p.d_max-p.d_min<1e-8)
            if(p.d_min>r-1e-7)
                return true;
        p.bca->set_child();
        pq.push(BCA_proj(p.bca->child[0],q));
        pq.push(BCA_proj(p.bca->child[1],q));
    }
    return true;
}

const Point offset(const Geometry& p,double r);

double live_radius(vector<Curve>& loop,const Geometry& p,double r_min,double r_max,int n){
    double r;
    do{
        r=(r_min+r_max)/2;
        if(live(loop,offset(p,r),r,n))
            r_min=r;
        else
            r_max=r;
    }while(r_max-r_min>1e-7);
    return r;
}
