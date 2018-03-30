#ifndef BITMAP_H_INCLUDED
#define BITMAP_H_INCLUDED

#include <stdio.h>
#include <string.h>
#include <math.h>

#define c_abs(a) ((a>=0) ? a : 256+(a))

#ifndef _Abs
	#define _Abs(a) (a<0 ? -(a) : a)
#endif

#if defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64)
	#define _WL_WINDOWS

	#include <windows.h>
#else
	#define _WL_LINUX

	#include <X11/Xlib.h>
	#include <X11/Xutil.h>
	#include <X11/keysym.h>
#endif

enum ICParam
{
    ICP_NONE = 0,
    ICP_SCREEN = 1
};

template <typename Q>
void inline swap(Q &a,Q &b)
{
    Q t=a;
    a=b;
    b=t;
}

struct pixel
{
    unsigned char a,r,g,b;

    pixel()
    {
        a=0;
    }

    pixel(int A)
    {
        a=A;
        r=g=b=0;
    }

    pixel(int R,int G,int B)
    {
        a=255;
        r=R;
        g=G;
        b=B;
    }

    pixel(int A,int R,int G,int B)
    {
        a=A;
        r=R;
        g=G;
        b=B;
    }

    double gray()
    {
        return a*(0.2126*r + 0.7152*g + 0.0722*b)/65025;
    }

    double Grey()
    {
        return (0.2126*r + 0.7152*g + 0.0722*b)/255;
    }

    void operator+=(const pixel& p)
    {
        /*a=p.a;
        r+=p.r;
        g+=p.g;
        b+=p.b;*/

        a = (255*(a+p.a)-a*p.a)/255;
        r -= (r-p.r)*p.a/255;
        g -= (g-p.g)*p.a/255;
        b -= (b-p.b)*p.a/255;
    }

    pixel operator+(const pixel& p)
    {
        return pixel((255*(a+p.a)-a*p.a)/255,
                      r - (r-p.r)*p.a/255,
                      g - (g-p.g)*p.a/255,
                      b - (b-p.b)*p.a/255);
    }

    pixel operator*(double k)
    {
        return pixel(a*k,r,g,b);
    }

    pixel& operator*=(double k)
    {
        a *= k;

        return *this;
    }

    pixel operator^(double k)
    {
        return pixel(a,round(r*k),round(g*k),round(b*k));
    }

    pixel& operator^=(double k)
    {
        r = round(r*k);
        g = round(g*k);
        b = round(b*k);

        return *this;
    }

    pixel operator/(double k)
    {
        return pixel(a/k,r,g,b);
    }

    pixel& operator/=(double k)
    {
        a /= k;

        return *this;
    }

    bool operator==(const pixel& p)
    {
        return (a==p.a && r==p.r && g==p.g && b==p.b);
    }

    bool operator!=(const pixel& p)
    {
        return (a!=p.a || r!=p.r || g!=p.g || b!=p.b);
    }

    pixel operator!()
    {
        return pixel(a,255-r,255-g,255-b);
    }
};

struct Image
{
    pixel** matrix;
    int X0,Y0;          ///for window crop
    int Width,Height;
    bool transparent;

    pixel BK_COLOR;
    unsigned cnst,Cnst;
    unsigned char depth;	///in bytes!

    unsigned char* data;

    Image(int width,int height)
    {
        data=0;

        X0=Y0=0;

        Height=height;

        depth = 3;

        matrix=new pixel*[Width=width];
        for(int k=0;k<Width;k++) matrix[k]=new pixel[Height];

        Cnst=cnst=0;

        transparent=true;
    }

    Image(ICParam p = ICP_NONE)
    {
        X0=Y0=0;
        depth=0;

        data=0;

        if(p == ICP_NONE)
        {
            matrix=0;

            Width=Height=Cnst=cnst=0;
        }
        else if(p == ICP_SCREEN)
        {
            #ifdef _WL_WINDOWS
				static RECT dskt;
				GetWindowRect(GetDesktopWindow(),&dskt);

				Width=dskt.right;
				Height=dskt.bottom;
            #else
				static XWindowAttributes attr;
				static Display* D = XOpenDisplay(0);
				XGetWindowAttributes(D, DefaultRootWindow(D), &attr);

				Width = attr.width;
				Height=attr.height;
            #endif

            transparent=true;

            depth=3;

            matrix=new pixel*[Width];
            for(int b=0;b<Width;b++) matrix[b]=new pixel[Height];

            for(int x=0;x<Width;x++)
            {
                for(int y=0;y<Height;y++)
                {
                    matrix[x][y]=pixel(0,0,0,0);
                }
            }
        }
    }

    ~Image()
    {
        if(data) delete [] data;

        if(matrix)
        {
            for(int x=0;x<Width;x++) delete [] matrix[x];
            delete [] matrix;
        }

        data=0;
        matrix=0;

        Width=Height=0;
    }

    double Norm()
    {
        if(matrix || data)
        {
            double S=0;

            for(int x=X0;x<Width;x++)
            {
                for(int y=Y0;y<Height;y++)
                {
                    if(matrix) S += matrix[x][y].gray();
                    else if(data) S+= pixel(data[cnst*y+depth*x+2],data[cnst*y+depth*x+1],data[cnst*y+depth*x]).gray();
                }
            }

            return S/((Width-X0)*(Height-Y0));
        }
    }

    void Create(Image* img)
    {
        if(img->matrix)
        {
            if(!matrix || (Width<img->Width || Height<img->Height))
            {
                if(matrix)
                {
                    for(int x=0;x<Width;x++) delete [] matrix[x];
                    delete [] matrix;
                }

                matrix=new pixel*[img->Width];
                for(int x=0;x<img->Width;x++) matrix[x]=new pixel[img->Height];
            }

            for(int x=0;x<img->Width;x++) memcpy(matrix[x],img->matrix[x],img->Height*sizeof(pixel));
        }

        if(img->data)
        {
            if(!data || Cnst<img->Cnst)
            {
                if(data) delete [] data;

                data=new unsigned char[img->Cnst];
            }

            memcpy(data,img->data,img->Cnst);
        }

        X0=img->X0;
        Y0=img->Y0;

        depth = img->depth;

        Width=img->Width;
        Height=img->Height;

        transparent=img->transparent;

        Cnst=img->Cnst;
        cnst=img->cnst;

        BK_COLOR = img->BK_COLOR;
    }

    ///create data from matrix
    void CreateData()
    {
        if(!matrix) return;

        unsigned n=0;

        if(data) delete [] data;

        data=new unsigned char[(Width+(Width%4))*Height*depth];

        for(int y=0;y<Height;y++)
        {
            for(int x=0;x<Width+Width%4;x++)
            {
                if(x>=Width) data[n++]=0;
                else
                {
                    data[n]=c_abs(matrix[x][y].b);
                    data[n+1]=c_abs(matrix[x][y].g);
                    data[n+2]=c_abs(matrix[x][y].r);
                    n += depth;
                }
            }
        }

        cnst=depth*(Width+Width%4);
        Cnst=cnst*Height;
    }

    ///create matrix from data
    void CreateMatrix()
    {
        if(!data) return;

        if(matrix) delete [] matrix;

        matrix=new pixel*[Width];
        for(int b=0;b<Width;b++) matrix[b]=new pixel[Height];

        for(int y=0;y<Height;y++)
        {
            for(int x=0;x<Width;x++)
            {
                matrix[x][y].b=c_abs(data[cnst*y+depth*x]);
                matrix[x][y].g=c_abs(data[cnst*y+depth*x+1]);
                matrix[x][y].r=c_abs(data[cnst*y+depth*x+2]);
            }
        }
    }

    void clear(const pixel& color=pixel(0),int X0=-1,int Y0=-1,int X1=-1,int Y1=-1)
    {
        if(matrix)
        {
            if(X0<this->X0) X0=this->X0;
            if(Y0<this->Y0) Y0=this->Y0;

            if(X1<X0) X1=Width-1;
            if(Y1<Y0) Y1=Height-1;

            for(int y=Y0;y<=Y1;y++)
            {
                for(int x=X0;x<=X1;x++)
                {
                    matrix[x][y]=color;
                }
            }
        }

        if(data)
        {
            unsigned n=0;

            if(BK_COLOR.r==BK_COLOR.g && BK_COLOR.r==BK_COLOR.b) memset(data,BK_COLOR.r,Cnst);
            else
            while(n<Cnst)
            {
                if(data[n]!=BK_COLOR.b) data[n]=BK_COLOR.b;
                if(data[n+1]!=BK_COLOR.g) data[n+1]=BK_COLOR.g;
                if(data[n+2]!=BK_COLOR.r) data[n+2]=BK_COLOR.r;

                n+=depth;
            }
        }
    }

    void fill(pixel color)
    {
        if(data)
        {
            unsigned n=0;

            while(n<Cnst)
            {
                if(data[n]!=color.b) data[n]=color.b;
                if(data[n+1]!=color.g) data[n+1]=color.g;
                if(data[n+2]!=color.r) data[n+2]=color.r;

                n+=depth;
            }
        }

        if(matrix)
        {
            for(int y=Y0;y<Height;y++)
            {
                for(int x=X0;x<Width;x++)
                {
                    matrix[x][y]=color;
                    matrix[x][y].a=255;
                }
            }
        }
    }

    void fill_alpha(pixel color)
    {
        if(data)
        {
            unsigned n=0;

            while(n<Cnst)
            {
                if(data[n]!=color.b) data[n]-=(data[n]-color.b)*(color.a+1)>>8;
                if(data[n+1]!=color.g) data[n+1]-=(data[n+1]-color.g)*(color.a+1)>>8;
                if(data[n+2]!=color.r) data[n+2]-=(data[n+2]-color.r)*(color.a+1)>>8;

                n+=depth;
            }
        }

        if(matrix)
        {
            for(int y=Y0;y<Height;y++)
            {
                for(int x=X0;x<Width;x++)
                {
                    if(color.a==255) matrix[x][y]=color;
                    else
                    {
                        matrix[x][y].a=(255*(matrix[x][y].a+color.a)-matrix[x][y].a*color.a)>>8;
                        matrix[x][y].r-=(matrix[x][y].r-color.r)*(color.a+1)>>8;
                        matrix[x][y].g-=(matrix[x][y].g-color.g)*(color.a+1)>>8;
                        matrix[x][y].b-=(matrix[x][y].b-color.b)*(color.a+1)>>8;
                    }
                }
            }
        }
    }

    void fillRect(int x,int y,int x1,int y1,pixel color)
    {
        if(color.a==0) return;

        if(x>x1) swap(x,x1);
        if(y>y1) swap(y,y1);

        if(x1<=X0 || y1<=Y0) return;

        if(x<X0) x=X0;
        if(y<Y0) y=Y0;

        //if(x1>=Width) x1=Width-1;
        //if(y1>=Height) y1=Height-1;

        int _y=y;

        while(x<=x1 && x<Width)
        {
            while(y<=y1 && y<Height)
            {
                if(data)
                {
                    if(data[cnst*y+depth*x]!=color.b) data[cnst*y+depth*x]-=(data[cnst*y+depth*x]-color.b)*(color.a+1)>>8;
                    if(data[cnst*y+depth*x+1]!=color.g) data[cnst*y+depth*x+1]-=(data[cnst*y+depth*x+1]-color.g)*(color.a+1)>>8;
                    if(data[cnst*y+depth*x+2]!=color.r) data[cnst*y+depth*x+2]-=(data[cnst*y+depth*x+2]-color.r)*(color.a+1)>>8;
                }

                if(matrix)
                {
                    if(color.a==255) matrix[x][y]=color;
                    else
                    {
                        matrix[x][y].a=(255*(matrix[x][y].a+color.a)-matrix[x][y].a*color.a)>>8;
                        matrix[x][y].r-=(matrix[x][y].r-color.r)*(color.a+1)>>8;
                        matrix[x][y].g-=(matrix[x][y].g-color.g)*(color.a+1)>>8;
                        matrix[x][y].b-=(matrix[x][y].b-color.b)*(color.a+1)>>8;
                    }
                }

                y++;
            }
            x++;
            y=_y;
        }
    }

    void putPixel(int x,int y,pixel color,double a=-1)
    {
        if(x<X0 || y<Y0 || x>=Width || y>=Height || color.a==0) return;

        if(a>-1) color.a=round(a);

        if(matrix)
        {
            if(color.a==255) matrix[x][y]=color;
            else
            {
                matrix[x][y].a=(255*(matrix[x][y].a+color.a)-matrix[x][y].a*color.a)>>8;
                matrix[x][y].r-=(matrix[x][y].r-color.r)*(color.a+1)>>8;
                matrix[x][y].g-=(matrix[x][y].g-color.g)*(color.a+1)>>8;
                matrix[x][y].b-=(matrix[x][y].b-color.b)*(color.a+1)>>8;
            }
        }

        if(data)
        {
            data[cnst*y+depth*x]-=(data[cnst*y+depth*x]-color.b)*(color.a+1)>>8;
            data[cnst*y+depth*x+1]-=(data[cnst*y+depth*x+1]-color.g)*(color.a+1)>>8;
            data[cnst*y+depth*x+2]-=(data[cnst*y+depth*x+2]-color.r)*(color.a+1)>>8;
        }
    }

    void putWuPixel(double x,double y,pixel c)
    {
        double a = x - floor(x);
        double b = y - floor(y);

        putPixel(x,y,c, c.a*(1-a)*(1-b));
        putPixel(x,ceil(y),c, c.a*(1-a)*b);
        putPixel(ceil(x),y,c, c.a*a*(1-b));
        putPixel(ceil(x),ceil(y),c, c.a*a*b);
    }

    pixel getPixel(int x,int y)
    {
        if(x<X0 || y<Y0 || x>=Width || y>=Height) return pixel(0,0,0,0);
        else
        {
            if(data) return pixel(data[cnst*y+depth*x+2],data[cnst*y+depth*x+1],data[cnst*y+depth*x]);
            else if(matrix) return matrix[x][y];
        }
    }

    void drawImage(Image* img,int X,int Y,int X0=-1,int Y0=-1,int X1=-1,int Y1=-1)
    {
        if(!img) return;

        unsigned char al=255;

        if(X0<this->X0) X0=this->X0;
        if(Y0<this->Y0) Y0=this->Y0;

        if(X1<this->X0) X1=img->Width;
        if(Y1<this->Y0) Y1=img->Height;

        for(unsigned x=X0;x<X1;x++)
        {
            for(unsigned y=Y0;y<Y1;y++)
            {
                if(img->transparent) al=img->matrix[x][y].a;

                if(al && x+X>=0 && y+Y>=0 && x+X<Width && y+Y<Height)
                {
                    if(data)
                    {
                        data[cnst*(y+Y)+depth*(x+X)]-=(data[cnst*(y+Y)+depth*(x+X)]-img->matrix[x][y].b)*(al+1)>>8;
                        data[cnst*(y+Y)+depth*(x+X)+1]-=(data[cnst*(y+Y)+depth*(x+X)+1]-img->matrix[x][y].g)*(al+1)>>8;
                        data[cnst*(y+Y)+depth*(x+X)+2]-=(data[cnst*(y+Y)+depth*(x+X)+2]-img->matrix[x][y].r)*(al+1)>>8;
                    }

                    if(matrix)
                    {
                        if(al==255) matrix[x+X][y+Y]=img->matrix[x][y];
                        else
                        {
                            matrix[x+X][y+Y].a=(255*(matrix[x+X][y+Y].a+al)-matrix[x+X][y+Y].a*al)>>8;
                            matrix[x+X][y+Y].r-=(matrix[x+X][y+Y].r-img->matrix[x][y].r)*(al+1)>>8;
                            matrix[x+X][y+Y].g-=(matrix[x+X][y+Y].g-img->matrix[x][y].g)*(al+1)>>8;
                            matrix[x+X][y+Y].b-=(matrix[x+X][y+Y].b-img->matrix[x][y].b)*(al+1)>>8;
                        }
                    }
                }
            }
        }
    }

    void drawLine(int x,int y,int x1,int y1,pixel color,unsigned char type=0)
    {
        int pdx=0, pdy=0, sx,sy,err,dec,inc;

        inc=_Abs(x1-x);
        dec=_Abs(y1-y);

        if(!(inc|dec))
        {
            if(type==1) putPixel(x,y,color);
            return;
        }
        else
        {
            if(x<X0 || x>=Width || y<Y0 || y>=Height)
            {
                if(x1<X0 || x1>=Width || y1<Y0 || y1>=Height)
                {
                    if((x<X0 && x1<X0) || (x>=Width && x1>=Width) ||
                       (y<Y0 && y1<Y0) || (y>=Height && y1>=Height)) return;
                    else
                    {
                        if(!inc)
                        {
                            y = Y0;
                            y1 = Height-1;
                        }
                        else if(!dec)
                        {
                            x = X0;
                            x1 = Width - 1;
                        }
                        else
                        {
                            int _y = round(y1 - (y1-y)*(x1 - X0)/(x1 - x));

                            if(_y>=Y0 && _y<Height)
                            {
                                y=_y;
                                x=X0;
                            }
                            else
                            {
                                if(_y<=Y0)
                                {
                                    x = round(x1 + (Y0-y1)*(x1-x)/(y1-y));
                                    y = Y0;
                                }
                                else
                                {
                                    x = round(x1 + (Height - 1 - y1)*(x1-x)/(y1-y));
                                    y = Height-1;
                                }

                                //if(x0>=Width) x0=Width-1;
                                //else if(x0<X0) x0=X0;
                            }

                            _y = round(y1 - (y1-y)*(x1 - Width + 1)/(x1 - x));

                            if(_y>=Y0 && _y<Height)
                            {
                                y1=_y;
                                x1=Width-1;
                            }
                            else
                            {
                                if(_y<=Y0)
                                {

                                    x1 = round(x1 + (Y0-y1)*(x1-x)/(y1-y));
                                    y1 = Y0;
                                }
                                else
                                {
                                    x1 = round(x1 + (Height - 1 - y1)*(x1-x)/(y1-y));
                                    y1 = Height-1;
                                }

                                //if(x1>=Width) x1=Width-1;
                                //else if(x1<X0) x1=X0;
                            }

                            inc=_Abs(x1-x);
                            dec=_Abs(y1-y);

                            if(!(inc|dec))
                            {
                                if(type==1) putPixel(x,y,color);
                                return;
                            }
                        }
                    }
                }
                else
                {
                    x=x+x1;
                    x1=x-x1;
                    x=x-x1;

                    y=y+y1;
                    y1=y-y1;
                    y=y-y1;
                }
            }
        }
        ///else if( (x<X0 && x1<X0) || (x>Width && x1>Width) || (y<Y0 && y1<Y0) || (y>Height && y1>Height)) return;

        sx=(x1>x) ? 1 : (x1<x) ? -1 : 0;
        sy=(y1>y) ? 1 : (y1<y) ? -1 : 0;

        if (inc > dec) pdx = sx;
        else
        {
            swap(inc,dec);
            pdy = sy;
        }

        err = inc>>1;

        if(x>=X0 && y>=Y0 && x<Width && y<Height)
        {
            if(matrix) this->putPixel(x,y,color);
            if(data)
            {
                data[cnst*y+depth*x]-=(data[cnst*y+depth*x]-color.b)*(color.a+1)>>8;
                data[cnst*y+depth*x+1]-=(data[cnst*y+depth*x+1]-color.g)*(color.a+1)>>8;
                data[cnst*y+depth*x+2]-=(data[cnst*y+depth*x+2]-color.r)*(color.a+1)>>8;
            }
        }

        for(int t=0;t<inc;t++)
        {
            err-=dec;

            if(err<0)
            {
                err+=inc;

                x+=sx;
                y+=sy;
            }
            else
            {
                x+=pdx;
                y+=pdy;
            }

            if(x>=X0 && y>=Y0 && x<Width && y<Height)
            {
                if(matrix) this->putPixel(x,y,color);
                if(data)
                {
                    data[cnst*y+depth*x]-=(data[cnst*y+depth*x]-color.b)*(color.a+1)>>8;
                    data[cnst*y+depth*x+1]-=(data[cnst*y+depth*x+1]-color.g)*(color.a+1)>>8;
                    data[cnst*y+depth*x+2]-=(data[cnst*y+depth*x+2]-color.r)*(color.a+1)>>8;
                }
            }
        }
    }

    void drawDDALine(double x0,double y0,double x1,double y1,pixel color,unsigned char type=0)
    {
        double dx = (x1-x0);
        double dy = (y1-y0);

        if(fabs(dx)<0.5 && fabs(dy)<0.5)
        {
            putWuPixel((x0+x1)/2,(y0+y1)/2,color);
            return;
        }

        if(x0<X0 || x0>=Width || y0<Y0 || y0>=Height)
            {
                if(x1<X0 || x1>=Width || y1<Y0 || y1>=Height)
                {
                    if((x0<X0 && x1<X0) || (x0>=Width && x1>=Width) ||
                       (y0<Y0 && y1<Y0) || (y0>=Height && y1>=Height)) return;
                    else
                    {
                        if(fabs(dx)<1e-3)
                        {
                            y0 = Y0;
                            y1 = Height-1;
                        }
                        else if(fabs(dy)<1e-3)
                        {
                            x0 = X0;
                            x1 = Width - 1;
                        }
                        else
                        {
                            double y = y1 - (y1-y0)*(x1 - X0)/(x1 - x0);

                            if(y>=Y0 && y<Height)
                            {
                                y0=y;
                                x0=X0;
                            }
                            else
                            {
                                if(y<=Y0)
                                {
                                    x0 = round(x1 + (Y0-y1)*(x1-x0)/(y1-y0));
                                    y0 = Y0;
                                }
                                else
                                {
                                    x0 = round(x1 + (Height - 1 - y1)*(x1-x0)/(y1-y0));
                                    y0 = Height-1;
                                }

                                if(x0>=Width) x0=Width-1;
                                else if(x0<X0) x0=X0;
                            }

                            y = y1 - (y1-y0)*(x1 - Width + 1)/(x1 - x0);

                            if(y>=Y0 && y<Height)
                            {
                                y1=y;
                                x1=Width-1;
                            }
                            else
                            {
                                if(y<=Y0)
                                {

                                    x1 = round(x1 + (Y0-y1)*(x1-x0)/(y1-y0));
                                    y1 = Y0;
                                }
                                else
                                {
                                    x1 = round(x1 + (Height - 1 - y1)*(x1-x0)/(y1-y0));
                                    y1 = Height-1;
                                }

                                if(x1>=Width) x1=Width-1;
                                else if(x1<X0) x1=X0;
                            }

                            dy=(y1-y0);
                            dx=(x1-x0);

                            if(fabs(dx)<0.5 && fabs(dy)<0.5)
                            {
                                putWuPixel((x0+x1)/2,(y0+y1)/2,color);
                                return;
                            }
                        }
                    }
                }
                else
                {
                    x0=x0+x1;
                    x1=x0-x1;
                    x0=x0-x1;

                    y0=y0+y1;
                    y1=y0-y1;
                    y0=y0-y1;

                    dx*=-1;
                    dy*=-1;
                }
        }

        double L = (fabs(dx) > fabs(dy)) ? fabs(dx)+1 : fabs(dy)+1;

        dx = dx/L;
        dy = dy/L;

        L=round(L);

        double x,y;

        for(int k=0;k<=L;k++)
        {
            x=x0+k*dx;
            y=y0+k*dy;

            if((type==1 && k==(int)L) || x<X0 || x>Width || y<Y0 || y>Height) break;

            putWuPixel(x, y, color);
        }
    }

    void drawWuLine(double x0, double y0, double x1, double y1,pixel color,unsigned char type=0)
    {
            double dy=(y1-y0),dx=(x1-x0);

            if(fabs(dx)<0.5 && fabs(dy)<0.5)
            {
                putWuPixel((x0+x1)/2,(y0+y1)/2,color);
                return;
            }

            if(x0<X0 || x0>=Width || y0<Y0 || y0>=Height)
            {
                if(x1<X0 || x1>=Width || y1<Y0 || y1>=Height)
                {
                    if((x0<X0 && x1<X0) || (x0>=Width && x1>=Width) ||
                       (y0<Y0 && y1<Y0) || (y0>=Height && y1>=Height)) return;
                    else
                    {
                        if(fabs(dx)<1e-3)
                        {
                            y0 = Y0;
                            y1 = Height-1;
                        }
                        else if(fabs(dy)<1e-3)
                        {
                            x0 = X0;
                            x1 = Width - 1;
                        }
                        else
                        {
                            double y = y1 - (y1-y0)*(x1 - X0)/(x1 - x0);

                            if(y>=Y0 && y<Height)
                            {
                                y0=y;
                                x0=X0;
                            }
                            else
                            {
                                if(y<=Y0)
                                {
                                    x0 = round(x1 + (Y0-y1)*(x1-x0)/(y1-y0));
                                    y0 = Y0;
                                }
                                else
                                {
                                    x0 = round(x1 + (Height - 1 - y1)*(x1-x0)/(y1-y0));
                                    y0 = Height-1;
                                }

                                //if(x0>=Width) x0=Width-1;
                                //else if(x0<X0) x0=X0;
                            }

                            y = y1 - (y1-y0)*(x1 - Width + 1)/(x1 - x0);

                            if(y>=Y0 && y<Height)
                            {
                                y1=y;
                                x1=Width-1;
                            }
                            else
                            {
                                if(y<=Y0)
                                {

                                    x1 = round(x1 + (Y0-y1)*(x1-x0)/(y1-y0));
                                    y1 = Y0;
                                }
                                else
                                {
                                    x1 = round(x1 + (Height - 1 - y1)*(x1-x0)/(y1-y0));
                                    y1 = Height-1;
                                }

                                //if(x1>=Width) x1=Width-1;
                                //else if(x1<X0) x1=X0;
                            }

                            dy=(y1-y0);
                            dx=(x1-x0);

                            if(fabs(dx)<0.5 && fabs(dy)<0.5)
                            {
                                putWuPixel((x0+x1)/2,(y0+y1)/2,color);
                                return;
                            }
                        }
                    }
                }
                else
                {
                    x0=x0+x1;
                    x1=x0-x1;
                    x0=x0-x1;

                    y0=y0+y1;
                    y1=y0-y1;
                    y0=y0-y1;

                    dx*=-1;
                    dy*=-1;
                }
            }

            int xi=(x1>x0) ? 1 : (x1<x0) ? -1 : 0;
            int yi=(y1>y0) ? 1 : (y1<y0) ? -1 : 0;

            double x=round(x0),y=round(y0);

            x1=round(x1);
            y1=round(y1);

            int A=color.a;

            if(fabs(dy)>fabs(dx))
            {
                while(yi*(y-y1)<=0)
                {
                    if(yi*(y+yi-y1)>0)
                    {
                        x=x1;//round(x);
                        y=y1;//round(y);
                    }
                    else x=dx*(y-y0)/dy+x0;

                    color.a=A*(x-(int)x);
                    if(color.a) putPixel((int)x+1,y,color);

                    color.a=A*(1-x+(int)x);
                    if(color.a) putPixel((int)x,y,color);

                    y+=yi;

                    if(type==1 && yi*(y+yi-y1)>0.6) break;

                    //if(x<X0 || x>Width || y<Y0 || y>Height) break;
                }
            }
            else
            {
                while(xi*(x-x1)<=0)
                {
                    if(xi*(x+xi-x1)>0)
                    {
                        x=x1;//round(x);
                        y=y1;//round(y);
                    }
                    else y=dy*(x-x0)/dx+y0;

                    color.a=A*(y-(int)y);
                    if(color.a) putPixel(x,(int)y+1,color);

                    color.a=A*(1-y+(int)y);
                    if(color.a) putPixel(x,(int)y,color);

                    x+=xi;

                    if(type==1 && xi*(x+xi-x1)>0.6) break;

                    if(x<X0 || x>Width || y<Y0 || y>Height) break;
                }
            }
    }

    void drawRect(int x,int y,int x1,int y1,pixel color)
    {
        if(x>x1)
        {
            x = x+x1;
            x1 = x-x1;
            x = x-x1;
        }

        if(y>y1)
        {
            y = y+y1;
            y1 = y-y1;
            y = y-y1;
        }

        drawLine(x,y, x1,y,color);
        drawLine(x1,y+1, x1,y1-1,color);
        drawLine(x1,y1, x,y1,color);
        drawLine(x,y1-1, x,y+1,color);
    }

    /**void drawCircle(int X,int Y,int R,pixel color)
    {
        int x=R,y=0,d=1-R;

        while(x >= y)
        {
            putPixel(x+X,y+Y,color);
            putPixel(-x+X,y+Y,color);

            if(x!=y)
            {
                putPixel(y+X,x+Y,color);
                putPixel(y+X,-x+Y,color);
            }

            if(y!=0)
            {
                putPixel(x+X,-y+Y,color);
                putPixel(-x+X,-y+Y,color);

                if(x!=y)
                {
                    putPixel(-y+X,x+Y,color);
                    putPixel(-y+X,-x+Y,color);
                }
            }

            y++;

            if(d<0) d += 2*y+1;
            else
            {
                x--;
                d += 2*(y-x+1);
            }
        }
    }*/

    void drawCircle(int X,int Y,int R,pixel color)
    {
        int x=0,y=R,d=3-2*R;

        while(x <= y)
        {
            this->putPixel(x+X,y+Y,color);
            this->putPixel(x+X,-y+Y,color);
            this->putPixel(-x+X,-y+Y,color);
            this->putPixel(-x+X,y+Y,color);
            this->putPixel(y+X,x+Y,color);
            this->putPixel(y+X,-x+Y,color);
            this->putPixel(-y+X,-x+Y,color);
            this->putPixel(-y+X,x+Y,color);

            if(d<0) d=d+4*x+6;
            else
            {
                d=d+4*(x-y)+10;
                y--;
            }

            x++;
        }
    }

    #ifndef M_SQRT2
        #define M_SQRT2 1.414213562373095
    #endif

    void drawDDACircle(double X,double Y,double R,pixel color)
    {
        double x = R*M_SQRT2/2;
        double y = x;

        putWuPixel(X+x,Y+y, color);
        putWuPixel(X+x,Y-y, color);
        putWuPixel(X-x,Y+y, color);
        putWuPixel(X-x,Y-y, color);

        x--;
        y = sqrt(R*R - x*x);

        while(x>=0)
        {
            putWuPixel(X+x,Y+y, color);
            putWuPixel(X-x,Y-y, color);

            putWuPixel(X+y,Y+x, color);
            putWuPixel(X-y,Y-x, color);

            putWuPixel(X+x,Y-y, color);
            putWuPixel(X-x,Y+y, color);

            putWuPixel(X+y,Y-x, color);
            putWuPixel(X-y,Y+x, color);

            x--;
            y = sqrt(R*R - x*x);
        }
    }

    void fillCircle(int X,int Y,int R,pixel color)
    {
        int x=R,y=0,d=1-R;

        while(x >= y)
        {
            drawLine(x+X,y+Y,-x+X,y+Y,color);

            if(y!=0)
            {
                drawLine(x+X,-y+Y,-x+X,-y+Y,color);
            }

            y++;

            if(d<0) d += 2*y+1;
            else
            {
                x--;
                d += 2*(y-x+1);
            }
        }

        int _y=y;

        x=R,y=0,d=1-R;

        while(x >= y)
        {
            if(x!=y)
            {
                drawLine(y+X,x+Y,y+X,Y+_y,color,1);
                if(y!=0) drawLine(-y+X,x+Y,-y+X,Y+_y,color,1);

                if(x!=0)
                {
                    drawLine(y+X,-x+Y,y+X,Y-_y,color,1);
                    if(y!=0) drawLine(-y+X,-x+Y,-y+X,Y-_y,color,1);
                }
            }

            y++;

            if(d<0) d += 2*y+1;
            else
            {
                x--;
                d += 2*(y-x+1);
            }
        }
    }
};

///---------------------------------------------------------------------------------

inline bool Load_Image(const char* path,Image* &img)              ///*.bmp or *.tga  only
{
    int cr,cg;
    unsigned int skip1=0,skip2=0;
    bool withAlpha=false;

    FILE* f=fopen(path,"rb");

    if(!f) return false;

    if(img)
    {
        delete [] img->data;
        delete img;
    }

    img=new Image();

    cr=getc(f);
    cg=getc(f);

    int _r=0;

    if(cr=='B' || cg=='M')
    {
        skip1=16;
        skip2=30;
        _r=-1;
    }
    else if(cr==0 && cg==0)
    {
        cr=getc(f);

        if(cr==2) skip1=9;
        else return false;
    }
    else return false;

    for(unsigned short int k=0;k<skip1;k++) getc(f);    ///f.read((char*) &cr,1);

    cr=getc(f);
    cg=getc(f);

    img->Width=(int)cr+(int)cg*256;

    if(_r==-1) _r=img->Width%4;

    if(skip1==16)
    {
        getc(f);
        getc(f);
    }

    cr=getc(f);
    cg=getc(f);

    img->Height=(int)cr+(int)cg*256;

    if(skip1==9)
    {
        cr=getc(f);
        cg=getc(f);

        if(cr==32 && cg==8) withAlpha=true;
    }

    img->transparent=withAlpha;
    img->depth = 3 + (withAlpha);

    img->matrix=new pixel*[img->Width];
    for(unsigned b=0;b<img->Width;b++) img->matrix[b]=new pixel[img->Height];

    for(unsigned short int k=0;k<skip2;k++) getc(f);    ///f.read((char*) &cr,1);

    for(unsigned int y=img->Height-1;y>=0;y--)
    {
        for(unsigned int x=0;x<img->Width;x++)
        {
            img->matrix[x][y].b=getc(f);
            img->matrix[x][y].g=getc(f);
            img->matrix[x][y].r=getc(f);
            if(withAlpha) img->matrix[x][y].a=getc(f);
            else img->matrix[x][y].a=255;
        }

        if(y==0) break;
        else for(int _i=0;_i<_r;_i++) getc(f);
    }

    fclose(f);

    return true;
}

inline bool Save_Image(const char* path,Image* img,int width=-1,int height=-1)
{
    if(!img) return false;

    if(width<0 || width>img->Width) width=img->Width;
    if(height<0 || height>img->Height) height=img->Height;

    int _n=3*height*width+54;

    FILE* f=fopen(path,"wb");

    fputs("BM",f);
    fwrite(&_n,4,1,f);

    fputc(0,f);
    fputc(0,f);

    fputc(0,f);
    fputc(0,f);

    _n=54;
    fwrite(&_n,4,1,f);

    _n=40;
    fwrite(&_n,4,1,f);

    _n=width;
    fwrite(&_n,4,1,f);

    _n=height;
    fwrite(&_n,4,1,f);

    fputc(1,f);
    fputc(0,f);

    fputc(24,f);
    fputc(0,f);

    fputc(0,f);
    fputc(0,f);
    fputc(0,f);
    fputc(0,f);

    _n=3*height*width;
    fwrite(&_n,4,1,f);

    _n=2795;
    fwrite(&_n,4,1,f);
    fwrite(&_n,4,1,f);

    _n=0;
    fwrite(&_n,4,1,f);
    fwrite(&_n,4,1,f);

    if(img->data)
    {
        for(int y=height-1;y>=img->Y0;y--)
        {
            for(int x=img->Y0;x<width;x++)
            {
                fputc(img->data[img->cnst*y+img->depth*x],f);
                fputc(img->data[img->cnst*y+img->depth*x+1],f);
                fputc(img->data[img->cnst*y+img->depth*x+2],f);
            }

            for(int b=0;b<width%4;b++) fputc(0,f);
            if(y==0) break;
        }
    }
    else if(img->matrix)
    {
        for(int y=height-1;y>=img->Y0;y--)
        {
            for(int x=img->X0;x<width;x++)
            {
                fputc(img->matrix[x][y].b,f);
                fputc(img->matrix[x][y].g,f);
                fputc(img->matrix[x][y].r,f);
            }

            for(int b=0;b<width%4;b++) fputc(0,f);
            if(y==0) break;
        }
    }
    else return false;

    fclose(f);

    return true;
}

#endif // BITMAP_H_INCLUDED
