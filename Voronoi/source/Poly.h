#ifndef POLY_H
#define POLY_H
#include <set>
#include <gsl/gsl_poly.h>
using namespace std;

class Poly{
public:
    double a[6];
    inline Poly(double a0=0.,double a1=0.,double a2=0.,
                double a3=0.,double a4=0.,double a5=0.){
        a[0]=a0; a[1]=a1; a[2]=a2; a[3]=a3; a[4]=a4; a[5]=a5;
    }
    const set<double> solve() const{

        set<double> zero;
        if(a[3]==0&&a[4]==0&&a[5]==0){
            double d=a[1]*a[1]-4*a[0]*a[2];
            if(d<0)
                return zero;
            d=sqrt(d);
            double z[2]={(-a[1]+d)/(2*a[2]),(-a[1]-d)/(2*a[2])};
            for (int i = 0; i < 2; i++){
                //if(1e-6<z[i]&&z[i]<1-1e-6){
                if(1e-3<z[i]&&z[i]<1-1e-3){
                    zero.insert(z[i]);
                }
            }
        }
        else{
            double z[10];
            gsl_poly_complex_workspace * w
                = gsl_poly_complex_workspace_alloc (6);
            gsl_poly_complex_solve (a, 6, w, z);
            gsl_poly_complex_workspace_free (w);
            for (int i = 0; i < 5; i++){
                if(abs(z[2*i+1])<1e-6){
                    if(1e-3<z[2*i]&&z[2*i]<1-1e-3){
                        zero.insert(z[2*i]);
                    }
                }
            }
        }
        return zero;
    }
    const Poly operator*(const Poly& rhs) const{
        Poly p;
        for(int i=0;i<6;i++)
            for(int j=0;j<6&&i+j<6;j++)
                p.a[i+j]+=a[i]*rhs.a[j];
        return p;
    }
    const Poly operator-(const Poly& rhs) const{
        Poly p;
        for(int i=0;i<6;i++)
            p.a[i]=a[i]-rhs.a[i];
        return p;
    }
};

#endif // POLY_H
