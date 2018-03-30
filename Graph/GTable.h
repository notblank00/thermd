#ifndef GTABLE_H_INCLUDED
#define GTABLE_H_INCLUDED

#ifndef FONT_H_INCLUDED
    #include "Font.h"
#endif

#ifndef Abs(x)
	#define Abs(x) (x<0 ? -(x) : (x))
#endif

#ifndef min
	#define min(a,b) (a<b ? a : b)
#endif

#ifndef max
	#define max(a,b) (a>b ? a : b)
#endif

#include "Appr.h"

enum
{
    GTS_NONE=0,
    GTS_MOVABLE=2,
    GTS_GRID=4,
    GTS_COORDS=8,
    GTS_SELECTABLE=16
};

enum
{
    GRS_LINES=1,
    GRS_POINTS=2,
    GRS_BR=4,
    GRS_WU=8,
    GRS_DDA=16
};

inline void DrawSRect(int x,int y,int x1,int y1,pixel c,unsigned char style=1)
{
    /*DrawSLine(x,y,x,y1,c,style);
    DrawSLine(x,y1,x1,y1,c,style);
    DrawSLine(x1,y1,x1,y,c,style);
    DrawSLine(x1,y,x,y,c,style);*/

    DrawLine(x,y,x,y1,c);
    DrawLine(x,y1,x1,y1,c);
    DrawLine(x1,y1,x1,y,c);
    DrawLine(x1,y,x,y,c);
}

inline double AxisScale(double x,int l)
{
    static int _nx[3]={2,5,10};
    int _i=0;
    double res=1;

    double k=1;

    while(x<l)
    {
        x*=10;
        k*=10;
    }

    while(x>l)
    {
        if(_i>0)
        {
            x*=_nx[_i-1];
            res/=_nx[_i-1];
        }

        x/=_nx[_i];
        res*=_nx[_i++];

        if(_i==3) _i=0;
    }

    return 1.*res/k;
}

#define pType double

struct GTable
{
    struct Graph
    {
        struct point : public Vector3d<pType>
        {
            unsigned char attr;

            point()
            {
                attr=0;
            }

            point(pType x,pType y,pType z=0,unsigned char A=0) : Vector3d<pType>(x,y,z)
            {
                attr=A;
            }

            operator Vector3d<pType>()
            {
                return Vector3d<pType>(x,y,z);
            }
        };

        struct Rect
        {
            Vector3d<pType> A,B;

            Rect()
            {
            }

            Rect(pType x1,pType y1)
            {
                A.x=B.x=x1;
                A.y=B.y=y1;
            }

            Rect(pType x1,pType y1,pType x2,pType y2)
            {
                A.x=x1;
                A.y=y1;

                B.x=x1;
                B.y=y1;
            }

            Rect(Vector3d<pType> a)
            {
                A=a;
                B=a;
            }

            Rect(Vector3d<pType> a,Vector3d<pType> b)
            {
                A=a;
                B=b;
            }
        };

        Rect Borders;

        bool Selected;
        char* name;
        point** F;
        unsigned N,N_MAX;

        bool isVisible;

        pixel C;

        unsigned char Style;
        ///   0    1      2       4    8   16
        /// None Lines Circles  Bres.  Wu  DDA

        Graph(unsigned N_max,char* Name="F",pixel c=pixel(0,0,255),unsigned char style=GRS_LINES + GRS_DDA)
        {
            F = new point*[N_MAX=N_max];
            for(unsigned k=0;k<N_MAX;k++) F[k] = 0;

            N=0;

            if(Name)
            {
                name=new char[strlen(Name)+1];
                //copy(Name,name);
                strcpy(name, Name);
            }

            Selected=false;

            C=c;

            Style=style;

            isVisible = true;
        }

        Graph(char* Name="F",pixel c=pixel(0,0,255),unsigned char style=GRS_LINES + GRS_DDA)
        {
            F=new point*[N_MAX=2];
            for(unsigned k=0;k<N_MAX;k++) F[k] = 0;

            N=0;

            if(Name)
            {
                name=new char[strlen(Name)+1];
                //copy(Name,name);
                strcpy(name, Name);
            }

            Selected=false;

            C=c;

            Style=style;

            isVisible = true;
        }

        ~Graph()
        {
            delete [] name;
            name=0;

            delete [] F;
            F = 0;
        }

        point* Current()
        {
            return (F && N) ? F[N-1] : 0;
        }

        void Sort()
        {
            for(unsigned k=0;k<N-1;k++)
            {
                for(unsigned i=0;i<N-1-k;i++)
                {
                    if(F[i]->x > F[i+1]->x)
                    {
                        point* c=F[i];
                        F[i]=F[i+1];
                        F[i+1]=c;
                    }
                }
            }
        }

        void Scale(Point K)
        {
            for(unsigned p=0;p<N;p++)
            {
                F[p]->x*=K.x;
                F[p]->y*=K.y;
                F[p]->z*=K.z;
            }
        }

        void Move(Point P)
        {
            for(unsigned p=0;p<N;p++)
            {
                F[p]->x+=P.x;
                F[p]->y+=P.y;
                F[p]->z+=P.z;
            }
        }

        template<typename type>
        void Scale(type K)
        {
            for(unsigned p=0;p<N;p++)
            {
                F[p]->x*=K;
                F[p]->y*=K;
                F[p]->z*=K;
            }
        }

        void CalculateBorders()
        {
            if(N<1) return;

            Borders=Rect((Point)*F[0],(Point)*F[0]);

            for(unsigned k=1;k<N;k++)
            {
                if(F[k]->x<Borders.A.x) Borders.A.x=F[k]->x;
                if(F[k]->y<Borders.A.y) Borders.A.y=F[k]->y;

                if(F[k]->x>Borders.B.x) Borders.B.x=F[k]->x;
                if(F[k]->y>Borders.B.y) Borders.B.y=F[k]->y;
            }
        }

        pType FindMaxAbs()
        {
            if(N<1) return 0;

            pType res=fabs(F[0]->z);

            for(unsigned k=0;k<N;k++)
            {
                if(fabs(F[k]->z)>res) res=fabs(F[k]->z);
            }

            return res;
        }

        pType FindAverage()
        {
            if(N<1) return 0;

            pType res=0;

            for(unsigned k=0;k<N;k++)
            {
                res += F[k]->z;
            }

            return res/N;
        }

        pType FindMinAbs()
        {
            if(N<1) return 0;

            pType res=fabs(F[0]->z);

            for(unsigned k=0;k<N;k++)
            {
                if(fabs(F[k]->z)<res) res=fabs(F[k]->z);
            }

            return res;
        }

        pType FindMaxY()
        {
            if(N<1) return 0;

            pType res=F[0]->y;

            for(unsigned k=0;k<N;k++)
            {
                if(F[k]->y>res) res=F[k]->y;
            }

            return res;
        }

        Graph* Merge(Graph* g)
        {
            Graph* res=new Graph(N+g->N,name);

            for(unsigned k=0;k<N;k++) res->Add(*F[k]);
            for(unsigned k=0;k<g->N;k++) res->Add(*g->F[k]);

            res->Sort();

            res->Borders.A.x=min(Borders.A.x,g->Borders.A.x);
            res->Borders.A.y=min(Borders.A.y,g->Borders.A.y);

            res->Borders.B.x=max(Borders.B.x,g->Borders.B.x);
            res->Borders.B.y=max(Borders.B.y,g->Borders.B.y);

            return res;
        }

        template<typename type>
        void Add(type x, type y, type z = 0./0)
        {
            if(N==N_MAX)
            {
                N_MAX+=10;
                $extend(F,N,N_MAX,point*);

                for(unsigned k=N;k<N_MAX;k++) F[k] = 0;
            }

            if(!F[N]) F[N++]=new point(x,y,z);
            else
            {
                F[N]->x = x;
                F[N]->y = y;
                F[N]->z = z;
                F[N]->attr = 0;

                N++;
            }

            if(N==1) Borders=Rect(x, y);
            else
            {
                if(x < Borders.A.x) Borders.A.x = x;
                if(y < Borders.A.y) Borders.A.y = y;

                if(x > Borders.B.x) Borders.B.x = x;
                if(y > Borders.B.y) Borders.B.y = y;
            }
        }

        template<typename type>
        void Add(Vector3d<type> a)
        {
            if(N==N_MAX)
            {
                N_MAX+=10;
                $extend(F,N,N_MAX,point*);

                for(unsigned k=N;k<N_MAX;k++) F[k] = 0;
            }

            if(!F[N]) F[N++]=new point(a.x,a.y,a.z);
            else
            {
                F[N]->x = a.x;
                F[N]->y = a.y;
                F[N]->z = a.z;
                F[N]->attr = 0;

                N++;
            }

            if(N==1) Borders=Rect(a);
            else
            {
                if(a.x<Borders.A.x) Borders.A.x=a.x;
                if(a.y<Borders.A.y) Borders.A.y=a.y;

                if(a.x>Borders.B.x) Borders.B.x=a.x;
                if(a.y>Borders.B.y) Borders.B.y=a.y;
            }
        }

        template<typename type>
        void Add(Vector3d<type>** data,unsigned n)
        {
            if(N+n >= N_MAX)
            {
                N_MAX = N+n;
                $extend(F,N,N_MAX,point*);

                for(unsigned k=N;k<N_MAX;k++) F[k] = 0;
            }

            for(unsigned k=0;k<n;k++)
            {
                //F[N++]=new point(data[k]->x,data[k]->y,data[k]->z);

                if(!F[N]) F[N++] = new point(data[k]->x,data[k]->y,data[k]->z);
                else
                {
                    F[N]->x = data[k]->x;
                    F[N]->y = data[k]->y;
                    F[N]->z = data[k]->z;
                    F[N]->attr = 0;

                    N++;
                }

                if(N==1) Borders=Rect(*data[k]);
                else
                {
                    if(data[k]->x<Borders.A.x) Borders.A.x=data[k]->x;
                    if(data[k]->y<Borders.A.y) Borders.A.y=data[k]->y;

                    if(data[k]->x>Borders.B.x) Borders.B.x=data[k]->x;
                    if(data[k]->y>Borders.B.y) Borders.B.y=data[k]->y;
                }
            }
        }

        template<typename type>
        void Add(Vector3d<type>* data,unsigned n)
        {
            if(N+n >= N_MAX)
            {
                N_MAX = N+n;
                $extend(F,N,N_MAX,point*);

                for(unsigned k=N;k<N_MAX;k++) F[k] = 0;
            }

            for(unsigned k=0;k<n;k++)
            {
                //F[N++]=new point(data[k].x,data[k].y,data[k].z);

                if(!F[N]) F[N++]=new point(data[k].x,data[k].y,data[k].z);
                else
                {
                    F[N]->x = data[k].x;
                    F[N]->y = data[k].y;
                    F[N]->z = data[k].z;
                    F[N]->attr = 0;

                    N++;
                }

                if(N==1) Borders=Rect(data[k]);
                else
                {
                    if(data[k].x<Borders.A.x) Borders.A.x=data[k].x;
                    if(data[k].y<Borders.A.y) Borders.A.y=data[k].y;

                    if(data[k].x>Borders.B.x) Borders.B.x=data[k].x;
                    if(data[k].y>Borders.B.y) Borders.B.y=data[k].y;
                }
            }
        }

        template<typename type>
        void Add(unsigned n,type* x,type* y,type* z=0)
        {
            if(N+n >= N_MAX)
            {
                N_MAX = N+n;
                $extend(F,N,N_MAX,point*);

                for(unsigned k=N;k<N_MAX;k++) F[k] = 0;
            }

            //printf("N = %d\n",N_MAX);

            for(unsigned k=0;k<n;k++)
            {
                //F[N++]=new point(x[k],y[k],z ? z[k] : 0);

                if(!F[N]) F[N++]=new point(x[k],y[k],z ? z[k] : 0);
                else
                {
                    F[N]->x = x[k];
                    F[N]->y = y[k];
                    F[N]->z = z ? z[k] : 0;
                    F[N]->attr = 0;

                    N++;
                }

                //printf("%lf %lf | ",F[N-1]->x,F[N-1]->y);

                if(N==1) Borders=Rect(x[k],y[k]);
                else
                {
                    if(x[k]<Borders.A.x) Borders.A.x=x[k];
                    if(y[k]<Borders.A.y) Borders.A.y=y[k];

                    if(x[k]>Borders.B.x) Borders.B.x=x[k];
                    if(y[k]>Borders.B.y) Borders.B.y=y[k];
                }
            }
        }

        template<typename type>
        void Add(unsigned n,type* y, type x0 = 0, type h = 1)
        {
            if(N+n >= N_MAX)
            {
                N_MAX = N+n;
                $extend(F,N,N_MAX,point*);

                for(unsigned k=N;k<N_MAX;k++) F[k] = 0;
            }

            //printf("N = %d\n",N_MAX);

            for(unsigned k=0;k<n;k++)
            {
                //F[N++]=new point(x[k],y[k],z ? z[k] : 0);

                if(!F[N]) F[N++]=new point(x0, y[k], 0);
                else
                {
                    F[N]->x = x0;
                    F[N]->y = y[k];
                    F[N]->z = 0;
                    F[N]->attr = 0;

                    N++;
                }

                //printf("%lf %lf | ",F[N-1]->x,F[N-1]->y);

                if(N==1) Borders=Rect(x0,y[k]);
                else
                {
                    if(x0<Borders.A.x) Borders.A.x=x0;
                    if(y[k]<Borders.A.y) Borders.A.y=y[k];

                    if(x0>Borders.B.x) Borders.B.x=x0;
                    if(y[k]>Borders.B.y) Borders.B.y=y[k];
                }

                x0 += h;
            }
        }

		template<typename type>
        void Add(type (*f)(type),type a,type b,type e=1e-3, type h0 = 1e-6)
        {
            static dFunction nF;

            nF.Set(f,a,b,e,h0, DFT_FINITE);

            if(N+nF.N >= N_MAX)
            {
                N_MAX = N+nF.N;
                $extend(F,N,N_MAX,point*);

                for(unsigned k=N;k<N_MAX;k++) F[k] = 0;
            }

            for(unsigned k=0;k<nF.N;k++)
            {
                if(!F[N]) F[N++]=new point(nF.data[k]->x,nF.data[k]->y);
                else
                {
                    F[N]->x = nF.data[k]->x;
                    F[N]->y = nF.data[k]->y;
                    F[N]->z = 0;
                    F[N]->attr = 0;

                    N++;
                }

                if(N==1) Borders=Rect(nF.data[k]->x, nF.data[k]->y);
                else
                {
                    if(nF.data[k]->x<Borders.A.x) Borders.A.x=nF.data[k]->x;
                    if(nF.data[k]->y<Borders.A.y) Borders.A.y=nF.data[k]->y;

                    if(nF.data[k]->x>Borders.B.x) Borders.B.x=nF.data[k]->x;
                    if(nF.data[k]->y>Borders.B.y) Borders.B.y=nF.data[k]->y;
                }
            }
        }

        #ifdef FUNCTION_H_INCLUDED

        template<typename type>
        void Add(function* f, type a,type b, type e=1e-4, type h0 = 1e-10)
        {
            static dFunction nF;

            nF.Set(f,a,b,e,h0,DFT_FINITE);

            if(N+nF.N >= N_MAX)
            {
                N_MAX = N+nF.N;
                $extend(F,N,N_MAX,point*);

                for(unsigned k=N;k<N_MAX;k++) F[k] = 0;
            }

            for(unsigned k=0;k<nF.N;k++)
            {
                if(!F[N]) F[N++]=new point(nF.data[k]->x,nF.data[k]->y);
                else
                {
                    F[N]->x = nF.data[k]->x;
                    F[N]->y = nF.data[k]->y;
                    F[N]->z = 0;
                    F[N]->attr = 0;

                    N++;
                }

                if(N==1) Borders=Rect(nF.data[k]->x, nF.data[k]->y);
                else
                {
                    if(nF.data[k]->x<Borders.A.x) Borders.A.x=nF.data[k]->x;
                    if(nF.data[k]->y<Borders.A.y) Borders.A.y=nF.data[k]->y;

                    if(nF.data[k]->x>Borders.B.x) Borders.B.x=nF.data[k]->x;
                    if(nF.data[k]->y>Borders.B.y) Borders.B.y=nF.data[k]->y;
                }
            }
        }

        #endif

        template<class T, typename type>
        void Add(type (T::*f)(type), T* t, type a,type b,type e = 0.01, type h0 = 1e-3)
        {
            static dFunction nF;

            if(!f || !t) return;

            nF.Set(f,t,a,b,e,h0,DFT_FINITE);

            if(N+nF.N >= N_MAX)
            {
                N_MAX = N+nF.N;
                $extend(F,N,N_MAX,point*);

                for(unsigned k=N;k<N_MAX;k++) F[k] = 0;
            }

            for(unsigned k=0;k<nF.N;k++)
            {
                if(!F[N]) F[N++]=new point(nF.data[k]->x,nF.data[k]->y);
                else
                {
                    F[N]->x = nF.data[k]->x;
                    F[N]->y = nF.data[k]->y;
                    F[N]->z = 0;
                    F[N]->attr = 0;

                    N++;
                }

                if(N==1) Borders=Rect(nF.data[k]->x, nF.data[k]->y);
                else
                {
                    if(nF.data[k]->x<Borders.A.x) Borders.A.x=nF.data[k]->x;
                    if(nF.data[k]->y<Borders.A.y) Borders.A.y=nF.data[k]->y;

                    if(nF.data[k]->x>Borders.B.x) Borders.B.x=nF.data[k]->x;
                    if(nF.data[k]->y>Borders.B.y) Borders.B.y=nF.data[k]->y;
                }
            }
        }
    };

    ///--------------------------------------------------------------------------------

    Vector3d<pType> K;       ///scale
    Vector3d<pType> O;       ///offset
    Vector3d<pType> Cr;      ///axis cross point

    Coord Sz;      ///(Width, Height)

    static const int _l=120;            ///scale unit length
    static const unsigned char tr=40;   ///grid transparency

    double _hx,_hy;

    ///-----------------------------------------------------------

    Graph** G;      ///plot-objects
    unsigned N,N_MAX;

    bool isFocussed;
    $Font* font;

    unsigned char Style;
    ///  0       2      4      8           16
    /// none  movable  grid show coord. selectable

    GTable($Font* fnt=0, unsigned char style = GTS_MOVABLE + GTS_GRID + GTS_COORDS,
		   Vector3d<pType> k = Vector3d<pType>(1,1),
		   Vector3d<pType> o = Vector3d<pType>(0,0),
		   Vector3d<pType> cr = Vector3d<pType>(0,0),
		   unsigned N_max=10)
    {
        G=new Graph*[N_MAX=N_max];
        for(unsigned k=0;k<N_MAX;k++) G[k] = 0;

        N=0;

        //sG=new unsigned[sN_MAX=N_max];
        //sN=0;

        isFocussed=true;

        font=fnt;

        K=k;
        O=o;
        Cr=cr;

        _hx=1/AxisScale(K.x, _l);
        _hy=1/AxisScale(K.y, _l);

        Style=style;
    }

    ~GTable()
    {
        for(unsigned k=0;k<N;k++) delete G[k];
        delete [] G;
        G = 0;
    }

    Graph* &last()
    {
        return G[N-1];
    }

    void Reset()
    {
        for(unsigned k=0;k<N;k++)
        {
            delete G[k];
            G[k] = 0;
        }

        delete [] G;
        G = 0;

        G=new Graph*[N_MAX=10];
        for(unsigned k=0;k<N_MAX;k++) G[k] = 0;
        N=0;
    }

    void Add(char* Name,pixel C=pixel(0,0,255),unsigned char style=1)
    {
        if(N==N_MAX)
        {
            N_MAX+=1;
            $extend(G,N,N_MAX,Graph*);

            for(unsigned k=N;k<N_MAX;k++) G[k] = 0;
        }

        G[N++]=new Graph(Name,C,style);
    }

	template<typename type>
    void Add(unsigned n,type* x,type* y,type* z,pixel C=pixel(0,0,255),unsigned char style=1,char* Name="")
    {
        if(N==N_MAX)
        {
            N_MAX+=1;
            $extend(G,N,N_MAX,Graph*);

            for(unsigned k=N;k<N_MAX;k++) G[k] = 0;
        }

        G[N++]=new Graph(Name,C,style);

        G[N-1]->Add(n,x,y,z);
    }

    template<typename type>
    void Add(unsigned n,Vector3d<type>** data,pixel C=pixel(0,0,255),unsigned char style=1,char* Name="")
    {
        if(N==N_MAX)
        {
            N_MAX+=1;
            $extend(G,N,N_MAX,Graph*);

            for(unsigned k=N;k<N_MAX;k++) G[k] = 0;
        }

        G[N++]=new Graph(Name,C,style);

        G[N-1]->Add(data,n);
    }

    template<typename type>
    void Add(unsigned n,Vector3d<type>* data,pixel C=pixel(0,0,255),unsigned char style=1,char* Name="")
    {
        if(N==N_MAX)
        {
            N_MAX+=1;
            $extend(G,N,N_MAX,Graph*);

            for(unsigned k=N;k<N_MAX;k++) G[k] = 0;
        }

        G[N++]=new Graph(Name,C,style);

        G[N-1]->Add(data,n);
    }

    template<typename type>
    void Add(type (*f)(type),type a,type b,type e=1e-4,pixel C=pixel(0,0,255),unsigned char style=1,char* Name="")
    {
        if(N==N_MAX)
        {
            N_MAX+=1;
            $extend(G,N,N_MAX,Graph*);

            for(unsigned k=N;k<N_MAX;k++) G[k] = 0;
        }

        G[N++]=new Graph(Name,C,style);

        G[N-1]->Add(f,a,b,e);
    }

    void Select(unsigned char attr=1)
    {
        //sN=0;

        for(unsigned k=0;k<N;k++)
        {
            if(!G[k]) continue;

            G[k]->Selected=(attr & 1);

            /*if(G[k]->Selected)
            {
                sG[sN++] = k;
            }*/

            for(unsigned p=0;p<G[k]->N;p++)
            {
                G[k]->F[p]->attr=attr;
            }
        }
    }

    void Optimize()
    {
        for(unsigned k=0;k<N;k++)
        {
            if(G[k] && G[k]->N<1)
            {
                delete G[k];
                G[k]=0;
            }
        }

        unsigned n=this->Count();

        if(n!=N)
        {
            Graph** _G=new Graph*[n];

            n=0;

            for(unsigned k=0;k<N;k++)
            {
                if(G[k]) _G[n++]=G[k];
            }

            N=n;

            memcpy(G,_G,n*sizeof(_G[0]));
            delete [] _G;

            //delete [] G;
            //G=_G;
        }
    }

    void Cut(pType z_min)
    {
        for(unsigned k=0;k<N;k++)
        {
            if(G[k] && G[k]->FindMaxAbs()<z_min)
            {
                delete G[k];
                G[k]=0;
            }
        }
    }

    void SetStyle(unsigned char style, bool state = true)
    {
        if(state) Style = Style | style;
        else Style = Style & ~style;
    }

    void SetLineType(unsigned char style)
    {
        for(unsigned k=0;k<N;k++)
        {
            if(G[k])
            {
                G[k]->Style = G[k]->Style & ~(GRS_DDA + GRS_BR + GRS_WU);      ///clear (G[k]->Style)'s line type
                G[k]->Style = G[k]->Style | style;
            }
        }
    }

    void Merge(pType dy)
    {
        for(unsigned k=0;k<N;k++)
        {
            for(unsigned p=0;p<N;p++)
            {
                if(!G[k] || !G[p] || k==p) continue;

                ///if(fabs(G[k]->FindMaxY() - G[p]->FindMaxY())<dy)
                if(max(G[k]->Borders.A.y,G[p]->Borders.A.y) - min(G[k]->Borders.B.y,G[p]->Borders.B.y) < dy)
                {
                    Graph* res=G[k]->Merge(G[p]);

                    delete G[k];
                    delete G[p];

                    G[p]=0;
                    G[k]=res;
                }
            }
        }
    }

    void Sort()
    {
        for(unsigned k=0;k<N;k++)
        {
            if(G[k]) G[k]->Sort();
        }
    }

    unsigned Count()
    {
        unsigned cnt=0;

        for(unsigned k=0;k<N;k++)
        {
            if(G[k]) cnt++;
        }

        return cnt;
    }

    void Scale(pType a)
    {
        K*=a;

        _hx=1/AxisScale(K.x, _l);
        _hy=1/AxisScale(K.y, _l);

        O *= a;             ///!!!
    }

    void Scale(Vector3d<pType> a)
    {
        K*=a;

        _hx=1/AxisScale(K.x, _l);
        _hy=1/AxisScale(K.y, _l);

        O *= a;
    }

    void pScale(pType a, int X0, int Y0)
    {
        K*=a;

        _hx=1/AxisScale(K.x, _l);
        _hy=1/AxisScale(K.y, _l);

        O.x = O.x*a + (1-a)*(X0 - Sz.x);
        O.y = O.y*a + (1-a)*(Y0 - Sz.y);
    }

    void pScale(Vector3d<pType> a, int X0, int Y0)
    {
        K*=a;

        _hx=1/AxisScale(K.x, _l);
        _hy=1/AxisScale(K.y, _l);

        O.x = O.x*a.x + (1-a.x)*(X0 - Sz.x);
        O.y = O.y*a.y + (1-a.y)*(Y0 - Sz.y);
    }

    Graph* &operator[](unsigned k)				///!!!!!!!!!!!!!! (-1 ?)
    {
        return G[k];
    }

    Graph* GetByName(char* name)
    {
        for(unsigned k=0;k<N;k++)
        {
            if(G[k] && !strcmp(name,G[k]->name)) return G[k];
        }

        return 0;
    }

    void Draw(int Width=-1,int Height=-1, int X0=0,int Y0=0)
    {
        static char _s[128];
        static Graph::Rect sArea=Graph::Rect(Vector3d<pType>(-1,-1));
        static bool Selected=false;
        //static bool _A=false;
        static bool _once=true;
        static Vector3d<pType> _T;
        static pType cnt,_v,_a;
        static pixel gC;

        if(!Style) return;

        if(Width<0) Width = AppWidth; //getWidth();
        if(Height<0) Height = AppHeight; //getHeight();

        ///Crop------------------------------------------

        int _X0=MainDevice->BackBuffer->X0;
        int _Y0=MainDevice->BackBuffer->Y0;
        int _W=MainDevice->BackBuffer->Width;
        int _H=MainDevice->BackBuffer->Height;

        if(X0<0) X0=0;
        else if (X0>_W) X0=_W;

        if(Y0<0) Y0=0;
        else if (Y0>_H) Y0=_H;

        Width=min(Width+X0,_W);
        Height=min(Height+Y0,_H);

        MainDevice->BackBuffer->X0=X0;
        MainDevice->BackBuffer->Y0=Y0;
        MainDevice->BackBuffer->Width=Width;
        MainDevice->BackBuffer->Height=Height;

        ///----------------------------------------------

        Sz=Coord(Width/2,Height/2);

        pType x0,y0, _x,_y, x_,y_;

        ///Plots.Moving----------------------------------

        if(isLeftPressed && (Style & GTS_MOVABLE) && MouseX>=X0 && MouseX<=Width && MouseY>=Y0 && MouseY<=Height)
        {
            if(_once)
            {
                _T=O-Vector3d<pType>(MouseX,MouseY);

                _once=false;
            }
            else
            {
                O=_T+Vector3d<pType>(MouseX,MouseY);
            }
        }
        else
        {
            _once=true;
        }

        ///Plots.Grid---------------------------------------

        if(Style & GTS_GRID)
        {
            DrawLine(X0,O.y+Height/2,Width,O.y+Height/2,pixel(100,0,0,0));
            DrawLine(O.x+Width/2,Y0,O.x+Width/2,Height,pixel(100,0,0,0));

            cnt=Width/2+O.x;

            if(_hx>0)
            {
                _a = round((X0 - cnt)/(K.x*_hx));

                cnt += _a*K.x*_hx;
                _v = _a*_hx;

                _s[0]='0';
                _s[1]=0;

                Text_out(font,O.x + Width/2 - font->chr['0'].W-5,Height/2+O.y+5,_s);

                while(cnt<Width)
                {
                    if(abs(cnt - O.x - Width/2)<2)
                    {
                        _s[0]='0';
                        _s[1]=0;
                        _v=0;

                        cnt = O.x+Width/2;
                    }
                    else
                    {
                        if(_hx<1e-4 && (int)_v!=0) sprintf(_s,".%d",(int)_v);
                        else sprintf(_s,"%g",(double)_v);

                        DrawLine(cnt,0,cnt,Height,pixel(tr,0,20,50));
                    }

                    if(*_s!='0' || *(_s+1))
                    {
                        if(_s[0]=='.')
                        {
                            Text_out(font,cnt - (Text_Width(font,_s+1) + (_v<0)*font->chr['-'].W)/2,Height/2+O.y+5,_s+1);

                            sprintf(_s,"%c%.8g",_v>0 ? '+' : '-',(double)fabs(_v - (int)_v));
                            Text_out(font,cnt - (Text_Width(font,_s) + (_v<0)*font->chr['-'].W)/2,Height/2+O.y+5 + font->Height+2,_s);
                        }
                        else Text_out(font,cnt - (Text_Width(font,_s) + (_v<0)*font->chr['-'].W)/2,Height/2+O.y+5,_s);
                    }

                    DrawLine(cnt,O.y+Height/2-4,cnt,O.y+Height/2+4,pixel(100,0,0,0));

                    cnt+=K.x*_hx;
                    _v+=_hx;
                }
            }

            cnt=Height/2+O.y;

            if(_hy>0)
            {
                _a = round((Y0 - cnt)/(K.y*_hy));

                cnt += _a*K.y*_hy;
                _v = _a*_hy;

                while(cnt<Height)
                {
                    if(abs(cnt - O.y - Height/2)<2)
                    {
                        _s[0]='0';
                        _s[1]=0;
                        _v=0;

                        cnt = O.y+Height/2;
                    }
                    else
                    {
                        if(_hy<1e-4 && (int)_v!=0) sprintf(_s,"%d%c%.8g",(int)_v,-_v>0 ? '+' : '-',(double)fabs(_v - (int)_v));
                        else sprintf(_s,"%g",-(double)_v);

                        DrawLine(0,cnt,Width,cnt,pixel(tr,0,20,50));
                    }

                    if( *_s!='0' || *(_s+1)) Text_out(font,Width/2+O.x-Text_Width(font,_s)-5,cnt-font->Height/2+1,_s);

                    DrawLine(O.x+Width/2-4,cnt,O.x+Width/2+4,cnt,pixel(100,0,0,0));

                    cnt+=K.y*_hy;
                    _v+=_hy;
                }
            }
        }

        if(Style & GTS_COORDS)
        {
            ///Drawing point-coordinates if necessary...

            if(isRightPressed)
            {
                sprintf(_s,"%g %g",(double)(MouseX-Width/2-O.x)/K.x+Cr.x, (double)(Height/2-MouseY+O.y)/K.y+Cr.y);
                Text_out(font,MouseX,MouseY-font->Height,_s);
            }
        }

        ///-----------------------------------------------------------------

        ///try to select -> get selection borders
        if(Style & GTS_SELECTABLE)
        {
            if(isFocussed)
            {
                if(isRightPressed)
                {
                    if(!Selected)
                    {
                        Select(0);      ///clear selection
                        ///sN=0;

                        Selected=true;
                        //_t=false;
                        sArea.A=Vector3d<pType>(MouseX,MouseY);
                        sArea.B=sArea.A;
                    }
                    else
                    {
                        sArea.B=Vector3d<pType>(MouseX,MouseY);
                    }
                }
                else Selected=false;

                ///if(Selected) DrawRect(sArea.A.x,sArea.A.y,sArea.B.x,sArea.B.y,pixel(150,0,0,0));
            }
        }

        ///Plots.Drawing----------------------------------------------------

        for(unsigned k=0;k<N;k++)
        {
            if(!G[k] || !G[k]->N || !G[k]->isVisible) continue;

            if(sKey.Control && G[k]->Selected) gC = !G[k]->C;
            else gC = G[k]->C;

            unsigned p=0;
            while(isnan(G[k]->F[p]->x) || isnan(G[k]->F[p]->y)) p++;

            x0 = (O.x + Sz.x + K.x*G[k]->F[0]->x);
            y0 = (O.y + Sz.y - K.y*G[k]->F[0]->y);

            _x = x0;
            _y = y0;

            for(p;p<G[k]->N;p++)
            {
                if(isnan(G[k]->F[p]->x) || isnan(G[k]->F[p]->y)) continue;

                if(Selected && (isLeftPressed || isRightPressed))
                {
                    if(x0>=min(sArea.A.x,sArea.B.x) && x0<=max(sArea.A.x,sArea.B.x) &&
                       y0>=min(sArea.A.y,sArea.B.y) && y0<=max(sArea.A.y,sArea.B.y))
                    {
                        //_t=true;
                        G[k]->F[p]->attr = G[k]->F[p]->attr | 1;
                        G[k]->Selected=true;

                        /**if(sN==0 || sG[sN-1]!=k)
                        {
                            sG[sN++]=k;
                            ///printf("sG[%d] -> %d\n",sN-1,sG[sN-1]);
                        }*/
                    }
                }

                if(p<G[k]->N-1)
                {
                    if(G[k]->Style & GRS_LINES)
                    {
                        x_ = O.x + Sz.x + K.x*G[k]->F[p+1]->x;
                        y_ = O.y + Sz.y - K.y*G[k]->F[p+1]->y;

                        if(Abs(x_-_x)>0.5 || Abs(y_-_y)>0.5)
                        {
                            if(G[k]->Style & GRS_BR) DrawLine(_x, _y, x_, y_,gC);
                            else if(G[k]->Style & GRS_WU) jDrawWuLine(_x, _y, x_, y_,gC);
                            else jDrawDDALine(_x, _y, x_, y_,gC);

                            _x=x_;
                            _y=y_;
                        }
                    }
                    else
                    {
                        _x = O.x + Sz.x + K.x*G[k]->F[p+1]->x;
                        _y = O.y + Sz.y - K.y*G[k]->F[p+1]->y;
                    }
                }

                if(G[k]->Style & GRS_POINTS)
                {
                    x0=round(x0);
                    y0=round(y0);

                    if(G[k]->F[p]->attr==1)
                    {
                        FillCircle(x0,y0,2,(!G[k]->C)/2);
                        DrawCircle(x0,y0,2,!G[k]->C);
                    }
                    else
                    {
                        FillCircle(x0,y0,2,G[k]->C/2);
                        DrawCircle(x0,y0,2,G[k]->C);
                    }
                }

                x0=_x;
                y0=_y;

                //x0=x_;
                //y0=y_;
            }
        }

        ///--------------------------------------------------------------------------------------------

        if(isFocussed && Selected) DrawSRect(sArea.A.x,sArea.A.y,sArea.B.x,sArea.B.y,pixel(150,0,0,0));

        ///Uncrop--------------------------------------------------------------------------------------

        MainDevice->BackBuffer->X0=_X0;
        MainDevice->BackBuffer->Y0=_Y0;
        MainDevice->BackBuffer->Width=_W;
        MainDevice->BackBuffer->Height=_H;

        ///----------------------------------------------
    }

    void PrintNames()
    {
        for(unsigned k=0;k<N;k++)
        {
            if(G[k])
            {
                printf("[%s] (%d): %g\n",G[k]->name,G[k]->N,G[k]->FindMaxAbs());
            }
        }
    }

    ///save graph table data in binary format
    void Save(char* path)
    {
        FILE* f=fopen(path,"wb");

        unsigned l=this->Count();

        fwrite(&l,sizeof(l),1,f);

        for(unsigned k=0;k<N;k++)
        {
            if(!G[k]) continue;

            fwrite(&G[k]->N,sizeof(G[k]->N),1,f);

            l = (G[k]->name) ? strlen(G[k]->name) : 0;

            fwrite(&l,sizeof(l),1,f);
            if(G[k]->name) fwrite(&G[k]->name,sizeof(char),l,f);

            for(unsigned p=0;p<G[k]->N;p++)
            {
                if(!G[k]->F[p]) continue;
                ///fwrite(G[k]->F[p],sizeof(*G[k]->F[p]),1,f);

                fwrite(&G[k]->F[p]->x,sizeof(G[k]->F[p]->x),1,f);
                fwrite(&G[k]->F[p]->y,sizeof(G[k]->F[p]->y),1,f);
                fwrite(&G[k]->F[p]->z,sizeof(G[k]->F[p]->z),1,f);
            }
        }

        fclose(f);
    }

    ///load graph table data in binary format
    void Load(char* path)
    {
        static char s[255];
        unsigned l,n;

        FILE* f=fopen(path,"rb");

        if(!f) return;

        delete [] G;
        //delete [] sG;

        fread(&N,sizeof(N),1,f);

        G=new Graph*[N_MAX=N];

        //sG=new unsigned[sN_MAX=N];
        //sN=0;

        for(unsigned k=0;k<N;k++)
        {
            fread(&n,sizeof(n),1,f);
            fread(&l,sizeof(l),1,f);

            if(l)
            {
                fread(s,sizeof(char),l,f);
                s[l]=0;
            }

            G[k]=new Graph(n,s);

            for(unsigned p=0;p<n;p++)
            {
                G[k]->F[p]=new Graph::point;
                ///fread(G[k]->F[p],sizeof(*G[k]->F[p]),1,f);

                fread(&G[k]->F[p]->x,sizeof(G[k]->F[p]->x),1,f);
                fread(&G[k]->F[p]->y,sizeof(G[k]->F[p]->y),1,f);
                fread(&G[k]->F[p]->z,sizeof(G[k]->F[p]->z),1,f);

                G[k]->F[p]->attr=0;
            }

            G[k]->N=n;
        }

        fclose(f);
    }

    ///save graph table data in CSV format (separator is ';')
    void Print(char* path, unsigned char dpn=5) ///dpn = digits per number (after .)
    {
        static char t[256];
        static char _fmt[16];

        sprintf(_fmt,"%%.%dg;%%.%dg",dpn+1,dpn+1);

        FILE* f=fopen(path,"w");

        for(unsigned k=0;k<N;k++)
        {
            if(!G[k] || G[k]->N<2) continue;

            sprintf(t,"%s.X;%s.Y",G[k]->name,G[k]->name);
            fputs(t,f);

            if(k<N-1) fputc(';',f);
        }

        fputc('\n',f);
        //fputc(10,f);

        unsigned n=0;

        for(unsigned k=0;k<N;k++) if(G[k] && G[k]->N>n) n=G[k]->N;

        for(unsigned p=0;p<n;p++)
        {
            for(unsigned k=0;k<N;k++)
            {
                if(!G[k] || G[k]->N<2) continue;

                t[0]=0;

                //printf("%d ",G[k]);
                //printf("|%d %d| ",G[k]->N,p);
                //printf("%d ",G[k]->F);
                //printf("%d\n",G[k]->F[p]);

                if(G[k]->N>p) sprintf(t,_fmt,G[k]->F[p]->x,G[k]->F[p]->y);
                else sprintf(t," ; ");

                fputs(t,f);

                if(k<N-1) fputc(';',f);
            }

            fputc('\n',f);
        }

        fclose(f);
    }
};

#endif // GTABLE_H_INCLUDED
