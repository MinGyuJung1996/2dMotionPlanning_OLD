#include "mat.h"



extern string path;
extern vector<int> simple;
void set_frame(double xmin,double xmax,double ymin,double ymax);


namespace Lee{
;



template <int n>
struct Poly{
    static const int degree=n;
    double p[n+1];

    Poly(){};

    Poly(double p0,double p1,double p2,double p3){
        dbg("Poly::Poly");
        exit(0);
    }

    Poly reverse() const{
        Poly re;
        for(int i=0;i<=n;i++)
            re.p[i]=p[n-i];
        return re;
    }


    double operator()(double t) const{
        double q[n+1];
        for(int i=0;i<=n;i++)
            q[i]=p[i];

        for(int i=n;i>0;i--){
            for(int j=0;j<=i;j++){
                q[j]=q[j]*(1-t)+q[j+1]*t;
            }
        }
        return q[0];
    }



    pair<double,double> first(double t) const{
        double q[n+1];
        for(int i=0;i<=n;i++)
            q[i]=p[i];

        for(int i=n;i>1;i--){
            for(int j=0;j<=i;j++){
                q[j]=q[j]*(1-t)+q[j+1]*t;
            }
        }
        return make_pair(q[0]*(1-t)+q[1]*t,n*(q[1]-q[0]));
    }


    tuple<double,double,double> second(double t) const{
        double q[n+1];
        for(int i=0;i<=n;i++)
            q[i]=p[i];

        for(int i=n;i>2;i--){
            for(int j=0;j<=i;j++){
                q[j]=q[j]*(1-t)+q[j+1]*t;
            }
        }

        double second=n*(n-1)*(q[2]-2*q[1]+q[0]);

        q[0]=q[0]*(1-t)+q[1]*t;
        q[1]=q[1]*(1-t)+q[2]*t;

        return make_tuple(q[0]*(1-t)+q[1]*t,n*(q[1]-q[0]),second);
    }

    const Poly<n-1> diff() const{
        Poly<n-1> re;
        for(int i=0;i<=n-1;i++)
            re.p[i]=n*(p[i+1]-p[i]);
        return re;
    }

    const Poly<n+1> elevate() const{
        Poly<n+1> re;
        re.p[0]=p[0];
        re.p[n+1]=p[n];
        for(int i=1;i<=n;i++){
            double t=i*1./(n+1);
            re.p[i]=p[i-1]*t+p[i]*(1-t);
        }

        return re;
    }

    const Poly<n-1> reduce() const{
        Poly<n-1> re;
        re.p[0]=p[0];
        for(int i=1;i<=n-1;i++){
            double t=i*1./n;
            re.p[i]=(p[i]-t*re.p[i-1])/(1-t);
        }
        return re;
    }

/*
    const Poly<2> reduce3() const{
        Poly<2> re;
        re.p[0]=p[0];
        re.p[1]=p[0]+(p[1]-p[0])*1.5;
        re.p[2]=p[3];
        return re;
    }
*/

    void solve(vector<double>& zero,double t0=0.,double t1=1.) const{
        bool pos=false,neg=false;
        for(int i=0;i<=n;i++){
            if(p[i]>=0)
                pos=true;
            if(p[i]<=0)
                neg=true;
            if(pos&&neg){
                auto p=subdiv(.5);
                double m=mean(t0,t1);
                if(t1-t0<1e-9)
                    zero.push_back(m);
                else{
                    p.first.solve(zero,t0,m);
                    p.second.solve(zero,m,t1);
                }
                return;
            }
        }
        return;


        set_frame(0,1,-1e-1,1e-1);

        black();
        glBegin(GL_LINE_STRIP);
        for(int i=0;i<=100;i++){
            double t=i/100.;
            vertex(Point(t,operator()(t)));
            dbg(operator()(t));
        }
        glEnd();

        red();
        draw(Point(0,0),Point(1,0));

        blue();
        glBegin(GL_LINE_STRIP);
        for(int i=0;i<=2;i++){
            vertex(Point(i/5.,p[i]));
        }
        glEnd();
    }

    const Poly<n> operator+(const Poly<n> rhs) const{
        Poly<n> re;
        for(int i=0;i<=n;i++)
            re.p[i]=p[i]+rhs.p[i];
        return re;
    }


    const Poly<n> operator-(const Poly<n> rhs) const{
        Poly<n> re;
        for(int i=0;i<=n;i++)
            re.p[i]=p[i]-rhs.p[i];
        return re;
    }

    const Poly<n> operator+(double rhs) const{
        Poly<n> re;
        for(int i=0;i<=n;i++)
            re.p[i]=p[i]+rhs;
        return re;
    }



    const Poly<n> operator-(double rhs) const{
        Poly<n> re;
        for(int i=0;i<=n;i++)
            re.p[i]=p[i]-rhs;
        return re;
    }

    const Poly<n> operator*(double rhs) const{
        Poly<n> re;
        for(int i=0;i<=n;i++)
            re.p[i]=p[i]*rhs;
        return re;
    }


    inline static int binomial(int m,int k){
        if(k==0||k==m)
            return 1;
        if(k==1||k==m-1)
            return m;

        switch(m){
        case 4:
            return 6;
        case 5:
            return 10;
        }

        switch(k){
        case 2:case 4:
            return 15;
        }

        return 20;
    }

    template<int m>
    const Poly<n+m> operator*(const Poly<m> rhs) const{
        Poly<n+m> re;
        for(int i=0;i<=n+m;i++)
            re.p[i]=0;

        double q[n+1],r[m+1];

        for(int i=0;i<=n;i++)
            q[i]=p[i]*binomial(n,i);

        for(int i=0;i<=m;i++)
            r[i]=rhs.p[i]*binomial(m,i);

        for(int i=0;i<=n;i++)
            for(int j=0;j<=m;j++)
                re.p[i+j]+=q[i]*r[j];

        for(int i=0;i<=n+m;i++)
            re.p[i]/=binomial(n+m,i);

        return re;
    }


    const pair<Poly<n>,Poly<n>> subdiv(double t) const{
        Poly<n> re[2];

        double q[n+1];
        for(int i=0;i<=n;i++)
            q[i]=p[i];

        for(int i=n;i>0;i--){
            re[0].p[n-i]=q[0];
            re[1].p[i]=q[i];
            for(int j=0;j<=i;j++){
                q[j]=q[j]*(1-t)+q[j+1]*t;
            }
        }
        re[0].p[n]=re[1].p[0]=q[0];
        return make_pair(re[0],re[1]);
    }

    friend ostream& operator<<(ostream& os,const Poly& rhs){
        for(int i=0;i<=n;i++)
            os<<rhs.p[i]<<" ";
        return os;
    }
};


template<>
Poly<3>::Poly(double p0,double p1,double p2,double p3){
    p[0]=p0;
    p[1]=p1;
    p[2]=p2;
    p[3]=p3;
}


template<>
void Poly<2>::solve(vector<double>& zero,double,double) const{
    double a=p[0]-2*p[1]+p[2];
    double b=-2*p[0]+2*p[1];
    double c=p[0];

    double d=b*b-4*a*c;
    if(d<0)
        return;
    d=sqrt(d);
    a*=2;

    double x[2]={
        (-b+d)/a,
        (-b-d)/a,
    };

    for(int i=0;i<2;i++){
        if(1e-3<x[i] && x[i]<1-1e-3){
            zero.push_back(x[i]);
        }
    }
}


template<int n>
struct Bezier{
    Poly<n> x,y;

    Bezier(const Poly<n>& x,const Poly<n>& y):x(x),y(y){}

    Bezier(Point p0,Point p1,Point p2,Point p3){
        dbg("Bezier::Bezier");
        exit(0);
    }

    Bezier reverse() const{
        return Bezier(x.reverse(),y.reverse());
    }

    Point operator()(double t) const{
        return Point(x(t),y(t));
    }


    pair<Bezier,Bezier> subdiv(double t) const{
        auto xs=x.subdiv(t);
        auto ys=y.subdiv(t);
        return make_pair(Bezier(xs.first,ys.first),Bezier(xs.second,ys.second));
    }

    Bezier subdiv(double t0,double t1) const{
        return subdiv(t1).first.subdiv(t0/t1).second;
    }

    Bezier operator+(const Point& rhs) const{
        return Bezier(x+rhs.x,y+rhs.y);
    }

    Bezier operator-(const Point& rhs) const{
        return Bezier(x-rhs.x,y-rhs.y);
    }

    Bezier operator*(double rhs) const{
        return Bezier(x*rhs,y*rhs);
    }

    Bezier<n-1> diff() const{
        return Bezier<n-1>(x.diff(),y.diff());
    }

    Poly<6> dot(const Bezier& rhs) const{
        return x*rhs.x+y*rhs.y;
    }

    Poly<6> cross(const Bezier& rhs) const{
        return x*rhs.y-x*rhs.x;
    }

    Poly<3> cross(const Point& rhs) const{
        Poly<3> re;
        for(int i=0;i<=3;i++){
            re.p[i]=Point(x.p[i],y.p[i]).cross(rhs);
        }
        return re;
    }


    Circle bv(){
        double xm=mean(mean(x.p[0],x.p[1]),mean(x.p[2],x.p[3]));
        double ym=mean(mean(y.p[0],y.p[1]),mean(y.p[2],y.p[3]));
        double r=0;
        for(int i=0;i<4;i++)
            r=max(r,Point(xm,ym).distance(Point(x.p[i],y.p[i])));
        return Circle(Point(xm,ym),r);
    }
};



template<>
Bezier<3>::Bezier(Point p0,Point p1,Point p2,Point p3)
    :x(p0.x,p1.x,p2.x,p3.x),y(p0.y,p1.y,p2.y,p3.y){}


template <class T>
void load(istream& is,T& t){
    is.read((char*)&t,sizeof(T));
}

vector<vector<Bezier<3>>> bezier;


void init(){
    ifstream fin(path+"curve.dat",ios::binary);
    int frame_size,bezier_size;
    Point p[4],q[4];
    load(fin,frame_size);
    bezier.resize(frame_size);
    for(int i=0;i<frame_size;i++){
        load(fin,bezier_size);
        for(int j=0;j<bezier_size;j++){
            for(int k=0;k<4;k++){
                load(fin,p[k]);
            }
            bezier[i].push_back(Bezier<3>(p[0],p[1],p[2],p[3]));
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

template<int n>
void draw(const Bezier<n>& b){
    glBegin(GL_LINE_STRIP);
    for(int i=0;i<=100;i++){
        double t=i/100.;
        vertex(b(t));
    }
    glEnd();
}
/*
struct Spiral:public Bezier{
    Circle BV;
    set<double> domain;
    Spiral(const Bezier& b):Bezier(b),BV(b.bv()){
        domain.insert(0);
        domain.insert(1);
    }
};
*/

struct Curve{
    Bezier<3> x;
    Bezier<2> v;
    Bezier<1> a;
    Circle BV;
    Curve(const Bezier<3>& bezier)
        :x(bezier),v(x.diff()),a(v.diff()),BV(Point(0,0),DBL_MAX){}
    Curve(const Bezier<3>& x,const Bezier<2>& v,const Bezier<1>& a)
        :x(x),v(v),a(a),BV(Point(0,0),DBL_MAX){}
    void bv(){
        BV=x.bv();
    }


    pair<Point,Point> first(double t) const{
        return make_pair(x(t),v(t));
    }


    tuple<Point,Point,Point> second(double t) const{
        return make_tuple(x(t),v(t),a(t));
    }

    Geometry geometry(double t) const{
        auto p=second(t);
        double v2=get<1>(p).dot(get<1>(p));
        double v=sqrt(v2);
        return make_tuple(get<0>(p),get<1>(p)/v,get<1>(p).cross(get<2>(p))/(v2*v));
    }


    pair<Curve,Curve> subdiv(double t) const{
        auto xs=x.subdiv(t);
        auto vs=v.subdiv(t);
        auto as=a.subdiv(t);
        return make_pair(Curve(xs.first,vs.first,as.first),Curve(xs.second,vs.second,as.second));
    }

    Curve subdiv(double t0,double t1) const{
        return subdiv(t1).first.subdiv(t0/t1).second;
    }

};


void decompose(const Bezier<3>& bezier,vector<Curve>& spiral){
    vector<double> zero;
    auto dx=bezier.x.diff();
    auto dy=bezier.y.diff();
    auto ddx=dx.diff();
    auto ddy=dy.diff();
    auto dddx=ddx.diff();
    auto dddy=ddy.diff();
    dx.solve(zero);
    dy.solve(zero);
    (dx*ddy-dy*ddx).reduce().solve(zero);
    ((dx*dddy-dy*dddx)*(dx*dx+dy*dy)-(dx*ddy-dy*ddx)*(dx*ddx+dy*ddy)*3.).reduce().solve(zero);

    sort(zero.begin(),zero.end());
    Bezier<3> b=bezier;

    double z0=0;

    for(auto& z:zero){
        if(z-z0<1e-4)continue;//REMARK!! remove singularity
        auto bs=b.subdiv((z-z0)/(1-z0));
        spiral.push_back(bs.first);
        b=bs.second;
        z0=z;
        red();
        //draw(bs.first(1));
    }
    b.bv();
    spiral.push_back(b);
}


void draw(const Curve& c){
    draw(c.x);
}

struct Rational{
    Poly<6> a,b;
    double fmax,fmin;
    int n;
    double t[2];
    Rational(){}
    Rational(const Poly<6>& a,const Poly<6>& b,int n,double t0=0.,double t1=1.)
        :a(a),b(b),fmax(-DBL_MAX),n(n){
        t[0]=t0;
        t[1]=t1;
        double c[7];
        for(int i=0;i<=6;i++){
            c[i]=a.p[i]/b.p[i];
            fmax=max(fmax,c[i]);
        }
        fmin=max(c[0],c[6]);


        //CHECK!!!
        for(int i=0;i<=6;i++)
            if(b.p[i]<=0)
                fmax=DBL_MAX;
    }
    double operator()(double t){
        return a(t)/b(t);
    }
    inline bool operator<(const Rational& rhs) const{
        return fmax<rhs.fmax;
    }
    inline void subdiv(priority_queue<Rational>& rhs) const{
        auto as=a.subdiv(.5);
        auto bs=b.subdiv(.5);
        double m=mean(t[0],t[1]);
        rhs.push(Rational(as.first,bs.first,n,t[0],m));
        rhs.push(Rational(as.second,bs.second,n,m,t[1]));
    }

};

Point medial(const pair<Point,Point>& p0,const pair<Point,Point>& p1){
    Point q(Line::ray(p0.first,p0.second.rotate()),Line::medial(p0.first,p1.first));
//    gray();
//    glLineWidth(1);
//    draw(p0.first,q);
//    draw(p1.first,q);
    return q;
}

Circle osculating(const Geometry& p){
    double r=1/get<2>(p);
    return Circle(get<0>(p)+get<1>(p).rotate()*r,r);
}

bool mode=false;

pair<int,double> find(vector<Curve>& spiral,int i,double t,int left,int right){
    auto g=spiral[i].geometry(t);
    Point v=get<1>(g);
    Point n=v.rotate();
    auto k=get<2>(g);
    auto p=make_pair(get<0>(g),v);

    Point c(0,0);
    double r=DBL_MAX;
    double bottomline=0;


    if(1e-2<k){
        r=1/k;
        c=p.first+n*r;
        r+=1e-3;//REMARK!!
        bottomline=k-1e-5;//WHY???
        //bottomline-=1e-9;//WHY?
        //if(mode)green();draw(Circle(c,r));
    }

    priority_queue<Rational> pq;
    for(size_t j=0;j<spiral.size();j++){
        if(j==left || j==right)
            continue;

        if(spiral[j].BV.p.distance(c)>spiral[j].BV.r+r)continue;

        auto q=spiral[j].x-p.first;
        auto q1=q.cross(-2*v).elevate().elevate().elevate();
        auto q2=q.dot(q);
        auto d=Rational(q1,q2,j);

        if(d.fmax<=bottomline)continue;
        bottomline=max(bottomline,d.fmin);

        pq.push(d);

        double rc=1/d.fmin;
        if(0<rc && rc<r){
            r=rc;
            c=p.first+n*r;
        }

    }

    Rational d;
    if(!pq.empty()){
        while(true){
            d=pq.top();
            pq.pop();
            bottomline=max(bottomline,d.fmin);
            if(d.t[1]-d.t[0]<1e-6 && (pq.empty() || bottomline>=pq.top().fmax)){
                if(d.fmax==DBL_MAX)dbg("DBL_MAX");//REMARK!!
                break;
            }
            d.subdiv(pq);
        }
        if(left==right || d.fmax>=k-1e-9){//REMARK!! bottomline->k changed
            auto p1=spiral[d.n].first(mean(d.t[0],d.t[1]));
            medial(p,p1);

            return make_pair(d.n,mean(d.t[0],d.t[1]));
        }
    }
    if(k>1e-2){
        black();
        if(mode){
            dbg("weird",d.fmax,k,bottomline,left==right);
            dbg(pq.empty(),spiral.size());
            /*
            if(spiral.size()==3){
                red();
                draw(spiral[0].x(0));
                draw(spiral[1].x(0));
                draw(spiral[2].x(0));
                blue();
                draw(spiral[0].x(1));
                draw(spiral[1].x(1));
                draw(spiral[2].x(1));
            }
            */
            glLineWidth(4);
            draw(spiral[0]);
            draw(spiral[1]);
            glLineWidth(1);
            red();
            dbg(spiral[0].x(0).distance(spiral[0].x(1)));
            dbg(spiral[1].x(0).distance(spiral[1].x(1)));

            //auto g=spiral[i].second(t);
            //dbg(get<0>(g),get<1>(g),get<2>(g));
            //dbg("bt",bottomline,k);
            //draw(Circle(p.first+n/k,1/k));
        }
        return make_pair(left,1);
    }
    //dbg("ERR");
    return make_pair(-1,0);
}


Point circumcenter(Point p0,Point p1,Point p2){
    return Point(Line::medial(p0,p1),Line::medial(p0,p2));
}

bool epsilon(Curve& curve){
    return curve.x(0).distance(curve.x(1))<1e-9;
}

void medial(vector<Curve>& bezier,vector<double>& c,int depth=0){
    if(bezier.size()==3 && depth>=15){
        if(epsilon(bezier[0])||epsilon(bezier[1])||epsilon(bezier[2]))
            return;

        auto p0=medial(bezier[0].first(1),bezier[1].first(0));
        auto p1=medial(bezier[1].first(1),bezier[2].first(0));
        auto p2=medial(bezier[2].first(1),bezier[0].first(0));
        auto q=circumcenter(bezier[0].x(.5),bezier[1].x(.5),bezier[2].x(.5));

        color(c[0]);
        glBegin(GL_POLYGON);
        vertex(bezier[0].x(0));
        vertex(bezier[0].x(.5));
        vertex(q);
        vertex(p2);
        glEnd();
        glBegin(GL_POLYGON);
        vertex(bezier[0].x(.5));
        vertex(bezier[0].x(1));
        vertex(p0);
        vertex(q);
        glEnd();

        color(c[1]);
        glBegin(GL_POLYGON);
        vertex(bezier[1].x(0));
        vertex(bezier[1].x(.5));
        vertex(q);
        vertex(p0);
        glEnd();
        glBegin(GL_POLYGON);
        vertex(bezier[1].x(.5));
        vertex(bezier[1].x(1));
        vertex(p1);
        vertex(q);
        glEnd();

        color(c[2]);
        glBegin(GL_POLYGON);
        vertex(bezier[2].x(0));
        vertex(bezier[2].x(.5));
        vertex(q);
        vertex(p1);
        glEnd();
        glBegin(GL_POLYGON);
        vertex(bezier[2].x(.5));
        vertex(bezier[2].x(1));
        vertex(p2);
        vertex(q);
        glEnd();

        black();
        draw(p0,q);
        draw(p1,q);
        draw(p2,q);


        return;
    }
    if(depth>=20)return;

    mode=true;

    //black();
    //if(depth==0)for(auto& i:bezier)draw(i);
    //red();
    if(bezier.size()==2){
        auto p00=bezier[0].first(0);
        auto p01=bezier[0].first(1);
        auto p10=bezier[1].first(0);
        auto p11=bezier[1].first(1);

        auto p=medial(p00,p11);
        auto q=medial(p01,p10);

        bool terminal=false;


        if(p00.first.distance(p11.first)<1e-9){
            p=osculating(bezier[0].geometry(0)).p;
            swap(p,q);
            //draw(p);
            terminal=true;
        }
        else if(p01.first.distance(p10.first)<1e-9){
            q=osculating(bezier[0].geometry(1)).p;
            //draw(q);
            terminal=true;
        }
        else if(p00.first.distance(p01.first)<1e-9)
            terminal=true;
        else if(p10.first.distance(p11.first)<1e-9)
            terminal=true;
        if(p.distance(q)<5e-2 || terminal){// || terminal){
            toc();
            if(false){
                glBegin(GL_LINES);
                red();
                vertex(p);
                blue();
                vertex(q);
                glEnd();
            }
            else{
                glLineWidth(1);
                color(c[0]);
                glBegin(GL_POLYGON);
                vertex(bezier[0].x(0));
                vertex(bezier[0].x(1));
                vertex(q);
                vertex(p);
                glEnd();
                color(c[1]);
                glBegin(GL_POLYGON);
                vertex(bezier[1].x(0));
                vertex(bezier[1].x(1));
                vertex(p);
                vertex(q);
                glEnd();
                color(c[0]);
                glBegin(GL_POLYGON);
                vertex(bezier[0].x(0));
                vertex(bezier[0].x(1));
                vertex(p);
                vertex(q);
                glEnd();
                color(c[1]);
                glBegin(GL_POLYGON);
                vertex(bezier[1].x(0));
                vertex(bezier[1].x(1));
                vertex(q);
                vertex(p);
                glEnd();
                black();
                draw(p,q);
            }
            tic();
            return;
        }

        if(epsilon(bezier[0])||epsilon(bezier[1]))
            return;
    }

    auto foot=find(bezier,0,.5,0,0);
    auto first=bezier[0].subdiv(.5);
    auto second=bezier[foot.first].subdiv(foot.second);

    if(foot.first<1){
        //dbg(depth,bezier.size());
        dbg("ERR3",frame,foot.first,bezier.size());
        red();
        //for(int i=0;i<bezier.size();i++)draw(bezier[i].x(0));
        glLineWidth(1);
        return;
    }

    vector<Curve> sub[2];
    vector<double> subcolor[2];
    for(int i=1;i<foot.first;i++){
        sub[0].push_back(bezier[i]);
        subcolor[0].push_back(c[i]);
    }
    sub[0].push_back(second.first);
    subcolor[0].push_back(c[foot.first]);
    sub[0].push_back(first.second);
    subcolor[0].push_back(c[0]);

    for(size_t i=foot.first+1;i<bezier.size();i++){
        sub[1].push_back(bezier[i]);
        subcolor[1].push_back(c[i]);
    }
    sub[1].push_back(first.first);
    subcolor[1].push_back(c[0]);
    sub[1].push_back(second.second);
    subcolor[1].push_back(c[foot.first]);


    medial(sub[0],subcolor[0],depth+1);
    medial(sub[1],subcolor[1],depth+1);
}

void mat(vector<Curve>& spiral,vector<int>& left,vector<double>& c){

    map<pair<int,double>,pair<int,double>> transition,piece;
    vector<set<double>> domain(spiral.size());


    for(size_t i=0;i<spiral.size();i++){
        //int left=i==0?spiral.size()-1:i-1;

        auto foot=find(spiral,i,0,left[i],i);
        domain[i].insert(0);
        domain[i].insert(1);
        if(foot.first>=0)
            domain[foot.first].insert(foot.second);
        //transition[make_pair(i,0)]=foot;
        transition[make_pair(left[i],1)]=foot;
        transition[foot]=make_pair(i,0);
        //transition[foot]=make_pair(left,1);
    }


    for(size_t i=0;i<spiral.size();i++){
        //auto& d=spiral[i].domain;
        auto& d=domain[i];
        for(auto j=d.begin(),k=++d.begin();k!=d.end();++j,++k){
            piece[make_pair(i,*j)]=make_pair(i,*k);
        }
    }


    int n=0;
    while(!piece.empty()){
        black();
        //color((n++)*234345.345);

        vector<Curve> cycle;
        vector<double> color;
        auto begin=*piece.begin();
        piece.erase(begin.first);
        auto left=begin.first;
        auto right=begin.second;
        cycle.push_back(spiral[left.first].subdiv(left.second,right.second));
        color.push_back(c[left.first]);
        while(true){
            left=transition[right];
            auto next=piece.find(left);
            //dbg(left.first,left.second);

            if(next==piece.end()){
                break;
            }
            right=next->second;
            piece.erase(left);
            cycle.push_back(spiral[left.first].subdiv(left.second,right.second));
            color.push_back(c[left.first]);
        }
        medial(cycle,color);
    }



    glLineWidth(4);
    black();
    for(size_t i=0;i<spiral.size();i++){
        //color(i*1./spiral.size());
        //black();
        draw(spiral[i]);
        //red();draw(spiral[i](0));
    }


}


void draw(){
    dbg(frame);

    tic();

    int n,i;
    //for(int i=0;i<8;i++)
    {
        mode=false;
        vector<Curve> spiral;
        //auto raw=bezier[i*100];


        auto raw=bezier[frame];
        reverse(raw.begin(),raw.end());
        for(auto& b:raw)
            decompose(b.reverse()+Point(1,1),spiral);
        vector<int> left(spiral.size());
        vector<double> color(spiral.size());

        for(int i=0;i<left.size();i++){
            left[i]=i-1;
            color[i]=0;
        }
        left[0]=spiral.size()-1;

        n=spiral.size();
        raw=bezier[frame2];
        reverse(raw.begin(),raw.end());
        for(auto& b:raw)
            decompose(b.reverse()+Point(-.3,0),spiral);

        for(int i=n;i<spiral.size();i++){
            left.push_back(i-1);
            color.push_back(.2);
        }
        left[n]=left.size()-1;


        n=spiral.size();
        raw=bezier[frame3];
        reverse(raw.begin(),raw.end());
        for(auto& b:raw)
            decompose(b.reverse()+Point(-.5,1.5),spiral);

        for(int i=n;i<spiral.size();i++){
            left.push_back(i-1);
            color.push_back(.4);
        }
        left[n]=left.size()-1;



        n=spiral.size();
        raw=bezier[frame4];
        reverse(raw.begin(),raw.end());
        for(auto& b:raw)
            decompose(b.reverse()+Point(0,-1.5),spiral);

        for(int i=n;i<spiral.size();i++){
            left.push_back(i-1);
            color.push_back(.6);
        }
        left[n]=left.size()-1;


        n=spiral.size();

        for(auto& b:bezier[400]){
            decompose(b*5,spiral);
        }

        for(int i=n;i<spiral.size();i++){
            left.push_back(i-1);
            color.push_back(.8);
        }

        left[n]=left.size()-1;


        //int left=i==0?spiral.size()-1:i-1;

        /*
        for(auto& b:bezier[i*100])
            decompose(b,spiral);
            */

        mat(spiral,left,color);
    }



    toc();
    show_time();
}
}


void init_old();
void draw_old();

void init_mat(){
    Lee::init();
    //init_old();
}

void draw_mat(){
    Lee::draw();
    //draw_old();
}
