#include "mat.h"
#include "util.h"
#include <cfloat>
#include <stack>
#include <string>
#include <algorithm>
using namespace std;

const Point offset(const Geometry& p,double r){
    return get<0>(p)+get<1>(p).rotate()*r;
}


vector<vector<Bezier>> raw;
vector<int> simple;


template <class T>
void load(istream& is,T& t){
    is.read((char*)&t,sizeof(T));
}

extern string path;

void init_old(){
    ifstream fin(path+"curve.dat",ios::binary);
    int frame_size,bezier_size;
    Point p[4],q[4];
    load(fin,frame_size);
    raw.resize(frame_size);
    for(int i=0;i<frame_size;i++){
        load(fin,bezier_size);
        for(int j=0;j<bezier_size;j++){
            for(int k=0;k<4;k++){
                load(fin,p[k]);
            }
            q[0]=p[0];
            q[1]=3*(p[1]-p[0]);
            q[2]=3*(p[2]-2*p[1]+p[0]);
            q[3]=p[3]-3*p[2]+3*p[1]-p[0];
            raw[i].push_back(Bezier(q[0],q[1],q[2],q[3]));
        }
    }
    fin.close();
    fin.open(path+"simple.dat");
    int size;
    fin>>size;
    simple.resize(size);
    for(int i=0;i<size;i++)
        fin>>simple[i];
}



struct BCA_dist{
    BCA* bca;
    double d_min,d_max;
    inline BCA_dist(BCA* bca,const Point& p):bca(bca){
        double d[2]={
            bca->arc[0].distance(p),
            bca->arc[1].distance(p),
        };

        if(d[0]>d[1])
            swap(d[0],d[1]);

        if(bca->arc[0].c.inside(p) && !bca->arc[1].c.inside(p)){
            d_min=0;
        }
        else{
            d_min=d[0];
        }
        d_max=d[1];
    }
    bool operator<(const BCA_dist& rhs) const{
        return d_min>rhs.d_min;
    }
};

double find_r(vector<BCA*>& candidate,const Geometry& p){
    double r_min=0,r_max=1;
    while(r_max-r_min>1e-10){
        double r=mean(r_min,r_max);
        Point q=offset(p,r);

        priority_queue<BCA_dist> pq;

        bool alive=true,dead=false;

        for(auto& i:candidate){
            BCA_dist d(i,q);
            pq.push(d);
            if(d.d_min<=r)
                alive=false;
            if(d.d_max<=r)
                dead=true;
        }

        if(!alive && !dead){
            while(true){
                auto d=pq.top();
                pq.pop();
                if(d.d_min>=r){
                    pq.push(d);
                    alive=true;
                    break;
                }
                if(d.d_max<=r){
                    pq.push(d);
                    dead=true;
                    break;
                }
                d.bca->set_child();
                pq.push(BCA_dist(d.bca->child[0],q));
                pq.push(BCA_dist(d.bca->child[1],q));
            }
        }

        if(alive){
            r_min=r;
            continue;
        }

        if(dead){
            r_max=r;
            candidate.clear();
            while(!pq.empty()){
                auto d=pq.top();
                pq.pop();
                if(d.d_min<r)
                    candidate.push_back(d.bca);
            }
            continue;
        }
    }
    return mean(r_min,r_max);
}

double projection(vector<BCA*>& candidate,const Geometry& p,double r){
    Point q=offset(p,r);
    priority_queue<BCA_dist> pq;
    for(auto& i:candidate)
        pq.push(BCA_dist(i,q));

    while(true){
        auto d=pq.top();
        pq.pop();
        if(d.d_max-d.d_min<1e-8 && d.bca->t[1]-d.bca->t[0]<1e-4){
/*
            blue();
            draw(get<0>(p),q);
            draw(get<0>(d.bca->p[0]),q);
*/
            return d.bca->param();
        }
        d.bca->set_child();
        pq.push(BCA_dist(d.bca->child[0],q));
        pq.push(BCA_dist(d.bca->child[1],q));
    }
}

void base(Curve& sp0,Curve& sp1);
void base(vector<Curve>& spiral);


typedef pair<CArc,CArc> Biarc;
typedef pair<Biarc,Biarc> Bilens;

void draw(const Biarc& biarc){
    draw(biarc.first);
    draw(biarc.second);
}

void draw(const Bilens& bilens){
    draw(bilens.first);
    draw(bilens.second);
}

const Circle osculating(const Geometry& p){
    Point q=evolute(p);
    return Circle(q,q.distance(get<0>(p)));
}

const Bilens bilens(Curve& sp){
    Bilens re;
    auto& p0=sp.root.p[0],&p1=sp.root.p[1];

    Point j(Line::medial(get<0>(p0),get<0>(p1)),Line::medial(get<0>(p0)+get<1>(p0),get<0>(p1)+get<1>(p1)));
    Circle J(j,j.distance(get<0>(p0)));
    Circle C0=osculating(p0),C1=osculating(p1);
    Point q[2];
    J.intersect(C0,q);
    if(abs(get<2>(p0))<1e-6)
        J.intersect(p0,q);
    if(q[0].distance(get<0>(p0))<q[1].distance(get<0>(p0)))
        swap(q[0],q[1]);
    re.first=make_pair(CArc(get<0>(p0),get<1>(p0),q[0]),CArc(get<0>(p1),get<1>(p1),q[0]));
    J.intersect(C1,q);
    if(abs(get<2>(p1))<1e-6)
        J.intersect(p1,q);
    if(q[0].distance(get<0>(p1))<q[1].distance(get<0>(p1)))
        swap(q[0],q[1]);
    re.second=make_pair(CArc(get<0>(p0),get<1>(p0),q[0]),CArc(get<0>(p1),get<1>(p1),q[0]));
    if(get<2>(p0)<get<2>(p1))
        swap(re.first,re.second);
    return re;
}

void draw_old(){
    //frame=0;
    tic();
    vector<Curve> spiral;
    int n=0;
    for(auto& bezier:raw[frame]){
        for(auto& sp:Curve::subdiv(bezier)){
            sp.root.n=n++;
            spiral.push_back(sp);
        }
    }




    for(unsigned n=0;n<spiral.size();n++){
        draw(spiral[n],n,spiral.size());
    }


    map<double,double> contact;
    map<double,double> radius;

    for(unsigned n=0;n<spiral.size();n++){
        auto& p=spiral[n].root.p[0];

        vector<BCA*> candidate;
        for(unsigned i=0;i<spiral.size();i++)
            if(n==0 && i!=0 && i!=spiral.size()-1 ||
                    n>0 && i!=n-1 && i!=n )
                candidate.push_back(&spiral[i].root);

        bool terminal=false;
        //live: forall r_min>r
        //dead: exists r_max<r

        double r_min=0,r_max=1;
        if(get<2>(p)>1){
            double r=1/get<2>(p);
            Point q=offset(p,r);
            priority_queue<BCA_dist> pq;
            bool alive=true,dead=false;
            for(auto& i:candidate){
                BCA_dist d(i,q);
                pq.push(d);
                if(d.d_min<=r)
                    alive=false;
                if(d.d_max<=r)
                    dead=true;
            }
            if(!alive && !dead){
                while(true){
                    auto d=pq.top();
                    pq.pop();
                    if(d.d_min>=r){
                        pq.push(d);
                        alive=true;
                        break;
                    }
                    if(d.d_max<=r){
                        pq.push(d);
                        dead=true;
                        break;
                    }
                    d.bca->set_child();
                    pq.push(BCA_dist(d.bca->child[0],q));
                    pq.push(BCA_dist(d.bca->child[1],q));
                }
            }
            if(alive){
                r_min=r;
                terminal=true;
            }
        }

        if(!terminal)
            r_min=find_r(candidate,p);


        red();
        if(terminal)
            blue();

        //draw(get<0>(p),q);

        if(!terminal){
            double r=r_min;
            double param=projection(candidate,p,r);
            contact[n]=param;
            contact[param]=n;
            radius[n]=radius[param]=r_min;
        }
        else{
            contact[n]=n;
            radius[n]=r_min;
        }
    }
    contact[spiral.size()]=contact[0];

    map<double,double> edge;
    for(auto i=contact.begin(),j=++contact.begin();j!=contact.end();++i,++j){
        edge[i->first]=j->first;
    }

    vector<vector<Curve>> domain;

    for(auto& i:edge){
        vector<Curve> d;
        double left=i.first,right=i.second,begin=left;
        while(true){
            int n=floor(left);
            d.push_back(spiral[n].subdiv(left-n,right-n));
            left=contact[right];
            if(left<right){
                if(left==begin)
                    domain.push_back(d);
                break;
            }
            right=edge[left];
        }
    }


    for(auto& d:domain)
        base(d);

    toc();

    show_time(0);

}

const Point medial(const Geometry& p,const Geometry& q){
    if(get<0>(p).distance(get<0>(q))<1e-9 && get<1>(p).distance(get<1>(q))<1e-9)return evolute(p);
    return Point(Line::ray(get<0>(p),get<1>(p).rotate()),Line::medial(get<0>(p),get<0>(q)));
}
void draw(const Geometry& p,const Geometry& q){
    Point r=medial(p,q);
    Point r2=medial(q,p);
    draw(r,r2);
    //draw(get<0>(p),r);
    //draw(get<0>(q),r);
}

void medial(const CArc& c0,const CArc& c1){
}

void medial(const Biarc& b0,const Biarc& b1){
    medial(b0.first,b1.first);
    medial(b0.first,b1.second);
    medial(b0.second,b1.first);
    medial(b0.second,b1.second);
}


void base(Curve& sp0,Curve& sp1){
    sp1.root.set_n(0);
    {
        Point p=medial(sp0.root.p[0],sp1.root.p[1]),q=medial(sp0.root.p[1],sp1.root.p[0]);
        black();
        if(p.distance(q)<1e-2){
            draw(p,q);
            return;
        }
    }

    vector<BCA*> candidate(1,&sp1.root);
    auto p=sp0(.5);
    double r=find_r(candidate,p);
    double t=projection(candidate,p,r);

    Curve c[4]={
        sp0.subdiv(0,.5),
        sp0.subdiv(.5,1),
        sp1.subdiv(0,t),
        sp1.subdiv(t,1),
    };

    base(c[0],c[3]);
    base(c[1],c[2]);
}


void base(vector<Curve>& spiral){
    if(spiral.size()==2){
        base(spiral[0],spiral[1]);
/*
        auto& sp0=spiral[0],&sp1=spiral[1];
        //draw(p);draw(q);
        auto b0=bilens(sp0);
        auto b1=bilens(sp1);

        red();
        draw(b0.first);
        draw(b1.second);
        blue();
        draw(b1.first);
        draw(b0.second);
        medial(b1.first,b0.second);

        green();
        medial(b0.first,b1.second);
        Point q=medial(sp0.root.p[0],sp1.root.p[1]);
        //draw(get<0>(sp0.root.p[0]),q);
        //draw(get<0>(sp1.root.p[1]),q);
*/
        return;
    }
    if(spiral.size()==3){
        for(int i=0;i<3;i++){
            int j=(i+1)%3,k=(i+2)%3;
            if(get<0>(spiral[i].root.p[0]).distance(get<0>(spiral[i].root.p[1]))<1e-7
                    &&get<1>(spiral[i].root.p[0]).distance(get<1>(spiral[i].root.p[1]))<1e-4){
                base(spiral[j],spiral[k]);
                return;
            }
        }
    }


    for(int i=0;i<spiral.size();i++)
        spiral[i].root.set_n(i);
    vector<BCA*> candidate;
    for(int i=1;i<spiral.size();i++)
        candidate.push_back(&spiral[i].root);
    auto p=spiral[0](.5);
    double r=find_r(candidate,p);
    double t=projection(candidate,p,r);
    int n=floor(t-1e-9);
    t-=n;
    auto q=spiral[n](t);
    Point s=medial(p,q);

    vector<Curve> sub[2];
    for(int i=1;i<n;i++)
        sub[0].push_back(spiral[i]);
    sub[0].push_back(spiral[n].subdiv(0,t));
    sub[0].push_back(spiral[0].subdiv(.5,1));

    for(int i=n+1;i<spiral.size();i++)
        sub[1].push_back(spiral[i]);
    sub[1].push_back(spiral[0].subdiv(0,.5));
    sub[1].push_back(spiral[n].subdiv(t,1));


    base(sub[0]);
    base(sub[1]);
}
