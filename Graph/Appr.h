#include <stdio.h>
#include <math.h>

#ifndef $extend
    #define $extend(A,n,N,type); {type* _T=new type[N]; memcpy(_T,A,sizeof(type)*n); delete [] A; A=_T;}
#endif

///dim(A) == n -> N

enum
{
    DFT_FINITE=1,
    DFT_PERIODIC=2,
    DFT_SIMM=4,
    DFT_ASIMM=8
};

struct dFunction
{
    struct $vector
    {
        double x,y;

        $vector(double X=0,double Y=0) : x(X), y(Y)
        {
        }
    };

    $vector** data;
    int N, N_MAX;
    double a,b;     ///borders of x
    unsigned char type;

    dFunction()
    {
        data=0;
        N=0;
    }

    template<class T>
    dFunction(T (*f)(T),T a,T b,T e=1e-3,unsigned char Type=1)
    {
        data = 0;
        this->Set(f,a,b,e,Type);
    }

    ~dFunction()
    {
        delete [] data;
        data = 0;
        N=0;
    }

    template<class T>
    void Set(T (*f)(T),T a,T b,T e=1e-3, T h0 = 1e-6, unsigned char Type=1)
    {
        ///h0 = (b-a)/2^n <= 2^-p

        ///const char p = 30;
        ///double h0 = (b-a)/pow(2, (int)log2(b-a) + p);

        T h,y0,y,yn;

        h=h0;//fabs(a+b)/4;

        this->a=a;
        this->b=b;

        type=Type;

        if(!Type) return;

        const int _step=100;

        //printf(".<");

        if(!data) data=new $vector*[N_MAX = _step];

        //printf(">\n");

        N=0;

        if(a>b)
        {
            y=a;
            a=b;
            b=y;
        }

        T P=e;///*sqrt(b-a);

        data[N++] = new $vector(a,f(a));

        T c = ((type & DFT_SIMM) || (type & DFT_ASIMM)) ? (a+b)/2 : b;

        while(a<c)
        {
            y0=f(a);
            y=f(a+h);
            yn=f(a+2*h);

            if(!isfinite(y0) || !isfinite(y))       ///miscontinuity
            {
                bool _cnt = true;
                T a0 = a;

                while(!isfinite(y) && h<64*h0 && a0+h<c)
                {
                    if(isfinite(f(a0+h+h0/2)))
                    {
                        _cnt = false;

                        a += h0/2;
                        y = (*f)(a+h);

                        break;
                    }

                    h *= 2;
                    y = f(a0+h);
                }

                if(h >= 64*h0)
                {
                    h /= 2;

                    while(!isfinite(y) && a0+h<c)
                    {
                        h += 32*h0;
                        y = (*f)(a0+h);
                    }

                    if(isfinite(y))
                    {
                        h = 64*h0;
                    }
                    else break;
                }

                if(_cnt)
                {
                    if(!isfinite(y)) a0 = b;
                    else a0 = a0+h;

                    h /= 2;

                    while(1)
                    {
                        do
                        {
                            h /= 2;

                            a0 -= h;

                            y = f(a0);
                        }
                        while(isfinite(y) && h>h0);

                        if(!isfinite(f(a0+h-h0)))
                        {
                            a = a0;
                            y = f(a+h);
                            break;
                        }

                        if(h>h0) a0 += h;
                        else break;
                    }
                }
            }
            else
            {
                //printf("%g [%g]: [%g] -> ", fabs( f(a+h/2)-(y+y0)/2), P, h);

                if(fabs( f(a+h/2)-(y+y0)/2 ) > P)
                {
                    while(fabs( f(a+h/2)-(y+y0)/2 ) > P && h>h0)
                    {
                        h/=2;
                        y=f(a+h);
                    }
                }
                else
                {
                    while(fabs( y-(yn+y0)/2 ) <= P && a+2*h<=c)	///h<32*h0
                    {
                        h*=2;
                        y=yn;
                        yn=f(a+2*h);
                    }

                    ///h/=2;
                    ///y=f(a+h);
                }

                //printf("[%g]\n", h);
            }

            if(N==N_MAX)
            {
                N_MAX+=_step;
                $extend(data,N,N_MAX,$vector*);
            }

            data[N++] = new $vector(a+h,y);

            a+=h;

            h=h0;
        }
    }

    #ifdef FUNCTION_H_INCLUDED

    void Set(function* f, double a,double b,double e = 0.01, double h0 = 1e-10, unsigned char Type=1)
    {
        if(!f->data[0] || isnan(f->data[0]->k)) return;

        ///h0 = (b-a)/2^n <= 2^-p

        ///const char p = 30;
        ///double h0 = (b-a)/pow(2, (int)log2(b-a) + p);

        double h,hmax,y0,y,yn;

        h=h0;//fabs(a+b)/4;

        this->a=a;
        this->b=b;

        type=Type;

        if(!Type) return;

        const int _step=100;

        if(!data) data=new $vector*[N_MAX = _step];

        N=0;

        if(a>b)
        {
            y=a;
            a=b;
            b=y;
        }

        double P = e;///*sqrt(b-a);

        data[N++] = new $vector(a,(*f)(a));

        double c = ((type & DFT_SIMM) || (type & DFT_ASIMM)) ? (a+b)/2 : b;

        while(a<c)
        {
            //printf("%g %g\n",a,(*f)(a));

            y0 = (*f)(a);
            y = (*f)(a+h);
            yn = (*f)(a+2*h);

            if(!isfinite(y0) || !isfinite(y))       ///miscontinuity
            {
                bool _cnt = true;
                double a0 = a;

                while(!isfinite(y) && h<64*h0 && a0+h<c)
                {
                    if(isfinite((*f)(a0+h+h0/2)))
                    {
                        _cnt = false;

                        a += h0/2;
                        y = (*f)(a+h);

                        break;
                    }

                    h *= 2;
                    y = (*f)(a0+h);
                }

                if(h >= 64*h0)
                {
                    h /= 2;

                    while(!isfinite(y) && a0+h<c)
                    {
                        h += 32*h0;
                        y = (*f)(a0+h);
                    }

                    if(isfinite(y))
                    {
                        h = 64*h0;
                    }
                    else break;
                }

                if(_cnt)
                {
                    if(!isfinite(y)) a0 = b;
                    else a0 = a0+h;

                    h /= 2;

                    while(1)
                    {
                        do
                        {
                            h /= 2;

                            a0 -= h;

                            y = (*f)(a0);
                        }
                        while(isfinite(y) && h>h0);

                        if(!isfinite((*f)(a0+h-h0)))
                        {
                            a = a0;
                            y = (*f)(a+h);
                            break;
                        }

                        if(h>h0) a0 += h;
                        else break;
                    }
                }
            }
            else
            {
                //printf("%g: %g %g\n", a, fabs( (*f)(a+h/2)-(y+y0)/2 ), P);

                if(fabs( (*f)(a+h/2)-(y+y0)/2 ) > P)
                {
                    while(fabs( (*f)(a+h/2)-(y+y0)/2 ) > P && h>h0)
                    {
                        h /= 2;
                        y = (*f)(a+h);
                    }
                }
                else
                {
                    //printf("%g %g -> ", a+h, y);

                    while(fabs( y-(yn+y0)/2 ) <= P && h<32*h0 && a+2*h<=c)
                    {
                        h *= 2;

                        y = yn;
                        yn = (*f)(a+2*h);
                    }

                    //printf("%g %g\n", a+h, y);
                }
            }

            if(N==N_MAX)
            {
                N_MAX+=_step;
                $extend(data,N,N_MAX,$vector*);
            }

            //if(a+h<0) printf("%g: %g %g\n",a+h, y, pow(a+h, a+h));

            data[N++] = new $vector(a+h,y);

            a+=h;

            if(c-a <= h)
            {
                if(N==N_MAX)
                {
                    N_MAX+=1;
                    $extend(data,N,N_MAX,$vector*);
                }

                data[N++] = new $vector(c,(*f)(c));
                break;
            }

            h=h0;
        }
    }

    #endif

    template<class T>
    void Set(double (T::*f)(double), T* t, double a,double b,double e = 0.01, double h0 = 1e-3, unsigned char Type=1)
    {
        if(!t) return;

        ///h0 = (b-a)/2^n <= 2^-p

        ///const char p = 30;
        ///double h0 = (b-a)/pow(2, (int)log2(b-a) + p);

        double h,hmax,y0,y,yn;

        h=h0;//fabs(a+b)/4;

        this->a=a;
        this->b=b;

        type=Type;

        if(!Type) return;

        const int _step=100;

        if(!data) data=new $vector*[N_MAX = _step];

        N=0;

        if(a>b)
        {
            y=a;
            a=b;
            b=y;
        }

        double P = e;///*sqrt(b-a);

        data[N++] = new $vector(a,(t->*f)(a));

        double c = ((type & DFT_SIMM) || (type & DFT_ASIMM)) ? (a+b)/2 : b;

        while(a<c)
        {
            y0 = (t->*f)(a);
            y = (t->*f)(a+h);
            yn = (t->*f)(a+2*h);

            if(!isfinite(y0) || !isfinite(y))       ///miscontinuity
            {
                bool _cnt = true;
                double a0 = a;

                while(!isfinite(y) && h<64*h0 && a0+h<c)
                {
                    if(isfinite((t->*f)(a0+h+h0/2)))
                    {
                        _cnt = false;

                        a += h0/2;
                        y = (t->*f)(a+h);

                        break;
                    }

                    h *= 2;
                    y = (t->*f)(a0+h);
                }

                if(h >= 64*h0)
                {
                    h /= 2;

                    while(!isfinite(y) && a0+h<c)
                    {
                        h += 32*h0;
                        y = (t->*f)(a0+h);
                    }

                    if(isfinite(y))
                    {
                        h = 64*h0;
                    }
                    else break;
                }

                if(_cnt)
                {
                    if(!isfinite(y)) a0 = b;
                    else a0 = a0+h;

                    h /= 2;

                    while(1)
                    {
                        do
                        {
                            h /= 2;

                            a0 -= h;

                            y = (t->*f)(a0);
                        }
                        while(isfinite(y) && h>h0);

                        if(!isfinite((t->*f)(a0+h-h0)))
                        {
                            a = a0;
                            y = (t->*f)(a+h);
                            break;
                        }

                        if(h>h0) a0 += h;
                        else break;
                    }
                }
            }
            else
            {
                //printf("%g: %g %g\n", a, fabs( (*f)(a+h/2)-(y+y0)/2 ), P);

                if(fabs( (t->*f)(a+h/2)-(y+y0)/2 ) > P)
                {
                    while(fabs( (t->*f)(a+h/2)-(y+y0)/2 ) > P && h>h0)
                    {
                        h /= 2;
                        y = (t->*f)(a+h);
                    }
                }
                else
                {
                    //printf("%g %g -> ", a+h, y);

                    while(fabs( y-(yn+y0)/2 ) <= P && h<32*h0 && a+2*h<=c)
                    {
                        h *= 2;

                        y = yn;
                        yn = (t->*f)(a+2*h);
                    }

                    //printf("%g %g\n", a+h, y);
                }
            }

            if(N==N_MAX)
            {
                N_MAX+=_step;
                $extend(data,N,N_MAX,$vector*);
            }

            //if(a+h<0) printf("%g: %g %g\n",a+h, y, pow(a+h, a+h));

            data[N++] = new $vector(a+h,y);

            a+=h;

            if(c-a <= h)
            {
                if(N==N_MAX)
                {
                    N_MAX+=1;
                    $extend(data,N,N_MAX,$vector*);
                }

                data[N++] = new $vector(c,(t->*f)(c));
                break;
            }

            h=h0;
        }
    }

    double operator()(double x)
    {
        if(!data || N<1 || !((x>=a && x<=b) || (type & DFT_PERIODIC))) return 0;
        else
        {
            int k=0;
            short sgn=1;

            if(type & DFT_PERIODIC) x-=(b-a)*(int)((x-a)/(b-a));
            else if((type & (DFT_SIMM | DFT_ASIMM)) && 2*x>a+b)
            {
                x=a+b-x;
                if(type & DFT_ASIMM) sgn=-1;
            }

            if(x==data[0]->x) return sgn*data[0]->y;
            else if(x==data[N-1]->x) return sgn*data[N-1]->y;
            else
            {
                if(x<data[N/2]->x) while(k<N-1 && x>data[k]->x) k++;
                else
                {
                    k=N/2;
                    while(k<N-1 && x>data[k]->x) k++;
                }
            }
            /*else
            {
                int n=N-1;
                int _st=N/2;

                while(x<data[n-1].x || x>data[n].x)
                {
                    if(x<data[n].x)
                    {
                        while(n>-1 && x<data[n].x) n-=_st;
                    }
                    else while(n<N-1 && x>data[n].x) n+=_st;

                    if(n<1) n=1;
                    else if(n>N-1) n=N-1;

                    _st/=2;
                }

                k=n;
            }*/

            ///while(k<N-1 && x>data[k].x) k++;

            return sgn*(data[k]->y*(x-data[k-1]->x) + data[k-1]->y*(data[k]->x-x))/(data[k]->x-data[k-1]->x);
        }
    }

    void WriteToFile(char* name)
    {
        static char s[128];

        FILE* f=fopen(name,"w");

        for(int k=0;k<N;k++)
        {
            sprintf(s,"%lf %lf\n",data[k]->x,data[k]->y);
            fputs(s,f);
        }

        fclose(f);

        /*f=fopen("data.bin","wb");

        fwrite(data,sizeof($vector),N,f);

        fclose(f);*/
    }
};
