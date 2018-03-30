#ifndef $GRAPHICS
#define $GRAPHICS

#include "Bitmap.h"
#include <stdlib.h>

#ifndef $extend
    #define $extend(A,n,N,type) {type* _T=new type[N]; memcpy(_T,A,sizeof(type)*(n)); delete [] A; A=_T;}
#endif

#define max3(a,b,c) (a>b ? (a>c ? a : c) : b>c ? b : c)

#define $end break;

#define $BK_COLOR MainDevice->BackBuffer->BK_COLOR

#define DesktopWidth MainDevice->BackBuffer->Width
#define DesktopHeight MainDevice->BackBuffer->Height

#define AppWidth MainDevice->Width
#define AppHeight MainDevice->Height

#define isLeftPressed MainDevice->Mouse.LeftPressed
#define isRightPressed MainDevice->Mouse.RightPressed
#define isDoubleClicked MainDevice->Mouse.DoubleClicked
#define MouseX MainDevice->Mouse.X
#define MouseY MainDevice->Mouse.Y
#define MouseWheel MainDevice->Mouse.Wheel

#define sMouse MainDevice->Mouse
#define sKey MainDevice->Key
#define sCnt MainDevice->Containers

#define PutPixel(x,y,color) MainDevice->BackBuffer->putPixel(x,y,color)
#define PutWuPixel(x,y,color) MainDevice->BackBuffer->putWuPixel(x,y,color)
#define GetPixel(x,y) MainDevice->BackBuffer->getPixel(x,y)

#define DrawLine(x,y,x1,y1,color) MainDevice->BackBuffer->drawLine(x,y,x1,y1,color)

#define DrawWuLine(x,y,x1,y1,color) MainDevice->BackBuffer->drawWuLine(x,y,x1,y1,color)
#define jDrawWuLine(x,y,x1,y1,color) MainDevice->BackBuffer->drawWuLine(x,y,x1,y1,color,1)

#define DrawDDALine(x,y,x1,y1,color) MainDevice->BackBuffer->drawDDALine(x,y,x1,y1,color)
#define jDrawDDALine(x,y,x1,y1,color) MainDevice->BackBuffer->drawDDALine(x,y,x1,y1,color,1)

#define DrawCircle(x,y,r,color) MainDevice->BackBuffer->drawCircle(x,y,r,color)
#define DrawDDACircle(x,y,r,color) MainDevice->BackBuffer->drawDDACircle(x,y,r,color)
#define FillCircle(x,y,r,color) MainDevice->BackBuffer->fillCircle(x,y,r,color)

#define MoveTo(x,y) {MainDevice->Containers.X=x; MainDevice->Containers.Y=y;}
#define Line(x,y,color) {if(MainDevice->Containers._line == 0) DrawLine(round(MainDevice->Containers.X),round(MainDevice->Containers.Y),x,y,color); else if(MainDevice->Containers._line == 1) jDrawWuLine(MainDevice->Containers.X,MainDevice->Containers.Y,x,y,color); else jDrawDDALine(MainDevice->Containers.X,MainDevice->Containers.Y,x,y,color);}
#define RLine(dx,dy,color) Line(MainDevice->Containers.X+dx, MainDevice->Containers.Y+dy, color)
#define LineTo(x,y,color) {Line(x,y,color); MoveTo(x,y);}
#define RLineTo(dx,dy,color) {Line(MainDevice->Containers.X+dx,MainDevice->Containers.Y+dy,color); MoveTo(MainDevice->Containers.X+dx,MainDevice->Containers.Y+dy);}

#define LineType(t) MainDevice->Containers._line = t;

#define DrawRect(x,y,x1,y1,color) MainDevice->BackBuffer->drawRect(x,y,x1,y1,color)

#define FillRect(x,y,x1,y1,color) MainDevice->BackBuffer->fillRect(x,y,x1,y1,color)
#define Fill(color) MainDevice->BackBuffer->fill(color)
#define FillAlpha(color) MainDevice->BackBuffer->fill_alpha(color)

#define DrawImage(img,x,y) MainDevice->BackBuffer->drawImage(img,x,y)

#define Clear() MainDevice->BackBuffer->clear()

#define SaveScreen() if(MainDevice->BackBuffer->data && MainDevice->Containers._tdata) memcpy(MainDevice->Containers._tdata,MainDevice->BackBuffer->data,MainDevice->BackBuffer->Cnst);
#define PaintSaved() if(MainDevice->BackBuffer->data && MainDevice->Containers._tdata) memcpy(MainDevice->BackBuffer->data,MainDevice->Containers._tdata,MainDevice->BackBuffer->Cnst);

#define $Main int main(int argc,char** argv) {
#define $End return 0; }

struct Device2D
{
    struct _Cnt
    {
        double X,Y;
        unsigned char* _tdata;
        unsigned char _line;
    };

    struct MouseInput
    {
        int X,Y;
        char Wheel;
        bool LeftPressed;
        bool RightPressed;
        bool DoubleClicked;
    };

    struct KeyInput
    {
        unsigned char Char;
        unsigned char Key;
        unsigned short Layout;

        bool Shift,Control;
    };

    Image* BackBuffer;

    int Width,Height;
    bool FullScreen;
    bool Aliasing;

    #ifdef _WL_WINDOWS
		HWND hwnd;
		const wchar_t* Caption;
	#else
		const char* Caption;
		Display* display;
		Window window;
	#endif

    MouseInput Mouse;
    KeyInput Key;
    _Cnt Containers;
};

Device2D* MainDevice=new Device2D;

inline void createDevice(int width,int height,pixel color,
						#ifdef _WL_WINDOWS
							const wchar_t* caption=L"WLGraphicsApp"
						#else
							const char* caption = "WLGraphicsApp"
						#endif
						)
{
	MainDevice->FullScreen=false;

	MainDevice->BackBuffer=new Image();
	MainDevice->BackBuffer->BK_COLOR=color;

	MainDevice->Width=width;
	MainDevice->Height=height;

	MainDevice->Caption=caption;

	MainDevice->Containers.X=MainDevice->Containers.Y=0;
	MainDevice->Containers._tdata=0;

	MainDevice->Key.Char=MainDevice->Key.Key=MainDevice->Key.Shift=MainDevice->Key.Control=0;
	MainDevice->Mouse.X=MainDevice->Mouse.Y=MainDevice->Mouse.Wheel=
	isLeftPressed=isRightPressed=isDoubleClicked=0;

	MainDevice->Aliasing=false;
}

inline void createDevice(pixel color,
						#ifdef _WL_WINDOWS
							const wchar_t* caption=L"WLGraphicsApp"
						#else
							const char* caption = "WLGraphicsApp"
						#endif
						)
{
	MainDevice->FullScreen=true;

	MainDevice->BackBuffer=new Image();
	MainDevice->BackBuffer->BK_COLOR=color;

	MainDevice->Caption=caption;

	MainDevice->Containers.X=MainDevice->Containers.Y=0;
	MainDevice->Containers._tdata=0;


	MainDevice->Key.Char=MainDevice->Key.Key=MainDevice->Key.Shift=MainDevice->Key.Control=0;
	MainDevice->Mouse.X=MainDevice->Mouse.Y=MainDevice->Mouse.Wheel=
	isLeftPressed=isRightPressed=isDoubleClicked=0;

	MainDevice->Aliasing=false;
}

inline int getWidth()
{
	#ifdef _WL_WINDOWS

		static RECT cRect;
		GetClientRect(MainDevice->hwnd,&cRect);

		return cRect.right;

	#else

		static XWindowAttributes attr;
		XGetWindowAttributes(MainDevice->display, MainDevice->window, &attr);

		return attr.width;

	#endif
}

inline int getHeight()
{
	#ifdef _WL_WINDOWS

		static RECT cRect;
		GetClientRect(MainDevice->hwnd,&cRect);

		return cRect.bottom;

	#else

		static XWindowAttributes attr;
		XGetWindowAttributes(MainDevice->display, MainDevice->window, &attr);

		return attr.height;

	#endif
}

///Windows
#ifdef _WL_WINDOWS

#define $WinProcHeader \
        static HDC dc; \
        static PAINTSTRUCT ps; \
        static PBITMAPV4HEADER bi = 0; \
        if(!bi) \
        { \
            bi = new BITMAPV4HEADER; \
            \
            ZeroMemory (bi, sizeof(BITMAPV4HEADER)); \
            bi->bV4Size= sizeof(BITMAPINFOHEADER); \
            bi->bV4BitCount= 24*sizeof(unsigned char); \
            bi->bV4Planes= 1; \
            bi->bV4Width= MainDevice->BackBuffer->Width; \
            bi->bV4Height= -MainDevice->BackBuffer->Height; \
            bi->bV4V4Compression = BI_RGB; \
        }

#define run() WinMain(NULL,NULL,NULL,1)

#define $RepaintBackground \
        dc=GetDC(hwnd); \
        if(!dc) break; \
        StretchDIBits(dc,0,0,MainDevice->BackBuffer->Width,\
					  		 MainDevice->BackBuffer->Height, 0,0,\
					  		 MainDevice->BackBuffer->Width,\
					  		 MainDevice->BackBuffer->Height,\
					  		 MainDevice->BackBuffer->data, (BITMAPINFO*)bi,DIB_RGB_COLORS,SRCCOPY); \
        ReleaseDC(hwnd, dc);

#define $SRepaintBackground \
        if(!dc) break; \
        StretchDIBits(dc,0,0,MainDevice->BackBuffer->Width,\
					  		 MainDevice->BackBuffer->Height, 0,0,\
					  		 MainDevice->BackBuffer->Width,\
					  		 MainDevice->BackBuffer->Height,\
					  		 MainDevice->BackBuffer->data, (BITMAPINFO*)bi,DIB_RGB_COLORS,SRCCOPY); \
        EndPaint(hwnd,&ps);

	#define $Refresh Clear(); InvalidateRect(hwnd,NULL,false);

    #define $EventReceiver_Begin \
            LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){ $WinProcHeader

        #define $EventReceiver_Start switch (message){

        #define $Mouse_Move case WM_MOUSEMOVE: MouseX=LOWORD(lParam); MouseY=HIWORD(lParam);
        #define $Mouse_Wheel case WM_MOUSEWHEEL: sMouse.Wheel = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
        #define $Mouse_LeftClick case WM_LBUTTONDOWN: isLeftPressed=true;
        #define $Mouse_RightClick case WM_RBUTTONDOWN: isRightPressed=true;
        #define $Mouse_DoubleClick case WM_LBUTTONDBLCLK: isDoubleClicked=true;
        #define $Mouse_LeftUp case WM_LBUTTONUP: isLeftPressed=false; isDoubleClicked=false;
        #define $Mouse_RightUp case WM_RBUTTONUP: isRightPressed=false;
        #define $Mouse_BM_Default $Mouse_Move $end $Mouse_LeftClick $end $Mouse_RightClick $end $Mouse_DoubleClick $end $Mouse_LeftUp $end $Mouse_RightUp $end
        #define $Mouse_Default $Mouse_Wheel $end $Mouse_BM_Default

        #define $Window_Close case WM_DESTROY:
        #define $Window_Paint case WM_PAINT:
        #define $Window_SPaint case WM_PAINT: dc=BeginPaint(hwnd,&ps);
        #define $Window_Size case WM_SIZE: MainDevice->Width = LOWORD(lParam); MainDevice->Height = HIWORD(lParam);
        #define $Window_Create case WM_CREATE:

        #define $KeyBoard_Char case WM_CHAR: if(message==WM_CHAR) {sKey.Char=c_abs(wParam);}
        #define $KeyBoard_Down case WM_KEYDOWN: \
                if(message==WM_KEYDOWN) \
                { \
                    static BYTE allKeys[256]; \
                    GetKeyboardState(allKeys); \
                    sKey.Layout=(short)LOWORD(GetKeyboardLayout(0)); \
                    sKey.Shift= (allKeys[VK_SHIFT] & 0x80); \
                    sKey.Control= (allKeys[VK_CONTROL] & 0x80); \
                    sKey.Key=c_abs(wParam); \
                }

        #define $KeyBoard_Up case WM_KEYUP: sKey.Char=sKey.Key=sKey.Shift=sKey.Control=0;
        #define $KeyBoard_Default $KeyBoard_Char $end $KeyBoard_Down $end $KeyBoard_Up $end

    #define $EventReceiver_End default: return DefWindowProc (hwnd, message, wParam, lParam); } return 0; }

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain (HINSTANCE hThisInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpszArgument,
                     int nCmdShow)
{
	MSG messages;
	WNDCLASSEX wincl;

	wincl.hInstance = hThisInstance;
	wincl.lpszClassName = "сWindowsApp";
	wincl.lpfnWndProc = WindowProcedure;
	wincl.style = CS_DBLCLKS;
	wincl.cbSize = sizeof (WNDCLASSEX);

	wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
	wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
	wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
	wincl.lpszMenuName = NULL;
	wincl.cbClsExtra = 0;
	wincl.cbWndExtra = 0;
	wincl.hbrBackground = NULL;//(HBRUSH) RGB(0,0,0);//COLOR_BACKGROUND;

	if (!RegisterClassEx (&wincl)) return 0;

	RECT dskt;

	GetWindowRect(GetDesktopWindow(),&dskt);

	dskt.right+=4-dskt.right%4;

	MainDevice->BackBuffer->Width=dskt.right;
	MainDevice->BackBuffer->Height=dskt.bottom;

	MainDevice->BackBuffer->transparent=false;
	MainDevice->BackBuffer->depth = 3;

	MainDevice->BackBuffer->cnst=3*dskt.right;
	MainDevice->BackBuffer->Cnst=MainDevice->BackBuffer->cnst*dskt.bottom;
	MainDevice->BackBuffer->data=new unsigned char[MainDevice->BackBuffer->Cnst];

	MainDevice->Containers._tdata=new unsigned char[MainDevice->BackBuffer->Cnst];

	sMouse.LeftPressed=sMouse.RightPressed=false;

	MainDevice->BackBuffer->clear();

	int WS;

	if(MainDevice->FullScreen)
	{
		WS=WS_POPUP;

		MainDevice->Width=dskt.right;
		MainDevice->Height=dskt.bottom;
	}
	else WS=WS_OVERLAPPEDWINDOW;

	MainDevice->hwnd = CreateWindowEx (
               0,                    //Extended possibilites for variation
               "сWindowsApp",         //Classname
               NULL,       //Title Text
               WS,
               CW_USEDEFAULT,       //Windows decides the position
               CW_USEDEFAULT,       //where the window ends up on the screen
               MainDevice->Width,                 //The programs width
               MainDevice->Height,                 //and height in pixels
               HWND_DESKTOP,        //The window is a child-window to desktop
               NULL,                //No menu
               hThisInstance,       //Program Instance handler
               NULL                 //No Window Creation data
               );

	SetWindowTextW(MainDevice->hwnd,MainDevice->Caption);

	ShowWindow (MainDevice->hwnd, nCmdShow);

	while (GetMessage (&messages, NULL, 0, 0))
	{
		TranslateMessage(&messages);
		DispatchMessage(&messages);
	}

	return messages.wParam;
}

///Linux
#else

enum sEvent
{
	eExpose = 1,
	eMouse_Move,
	eMouse_LeftClick,
	eMouse_RightClick,
	eMouse_Wheel,
	eMouse_LeftUp,
	eMouse_RightUp,
	eKeyBoard_Down,
	eKeyBoard_Up,
	eWindow_Create,
	eWindow_Size,
	eWindow_Close
};

inline int WindowProcedure(XEvent* message);

#define $exit return 0;

#define $RepaintBackground \
        XPutImage(MainDevice->display, MainDevice->window, __gc, _MainImage, 0,0, 0,0,\
				  MainDevice->BackBuffer->Width, MainDevice->BackBuffer->Height);

    #define $EventReceiver_Begin \
            inline int WindowProcedure(XEvent* message) \
            { \
            	static GC __gc = XCreateGC(MainDevice->display, MainDevice->window, 0, 0); \
				static XImage* _MainImage = XCreateImage(MainDevice->display, CopyFromParent, 24, \
															ZPixmap, 0, (char*)MainDevice->BackBuffer->data, \
															MainDevice->BackBuffer->Width, \
															MainDevice->BackBuffer->Height, 8, 0);
        #define $EventReceiver_Start \
        		int _ecode = 0;\
        		switch (message->type)\
        		{\
        			case Expose: _ecode = eExpose; break;\
        			case MotionNotify: _ecode = eMouse_Move; break;\
        			case MappingNotify: _ecode = eWindow_Create; break;\
        			case ButtonPress: \
        				switch(message->xbutton.button)\
        				{\
							case Button1: _ecode = eMouse_LeftClick; break;\
							case Button3: _ecode = eMouse_RightClick; break;\
							case Button4: _ecode = eMouse_Wheel; break;\
							case Button5: _ecode = -eMouse_Wheel; break;\
        				}\
        				break;\
					case ButtonRelease: \
						switch(message->xbutton.button)\
        				{\
							case Button1: _ecode = eMouse_LeftUp; break;\
							case Button3: _ecode = eMouse_RightUp; break;\
        				}\
        				break;\
					case KeyPress: _ecode = eKeyBoard_Down; break;\
        			case KeyRelease: _ecode = eKeyBoard_Up; break;\
        			case ConfigureNotify: _ecode = eWindow_Size; break;\
        			case DestroyNotify: _ecode = eWindow_Close; break;\
        		}\
        		switch(_ecode) {\

        #define $Mouse_Move case eMouse_Move: MouseX=message->xmotion.x; MouseY=message->xmotion.y;
        #define $Mouse_Wheel case eMouse_Wheel: case -eMouse_Wheel: sMouse.Wheel = (_ecode>0 ? 1 : -1);
        #define $Mouse_LeftClick case eMouse_LeftClick: isLeftPressed=true;
        #define $Mouse_RightClick case eMouse_RightClick: isRightPressed=true;
        #define $Mouse_DoubleClick case -3:
        #define $Mouse_LeftUp case eMouse_LeftUp: isLeftPressed=false; isDoubleClicked=false;
        #define $Mouse_RightUp case eMouse_RightUp: isRightPressed=false;
        #define $Mouse_BM_Default $Mouse_Move $end $Mouse_LeftClick $end $Mouse_RightClick $end $Mouse_DoubleClick $end $Mouse_LeftUp $end $Mouse_RightUp $end
        #define $Mouse_Default $Mouse_Wheel $end $Mouse_BM_Default

        #define $Window_Close case eWindow_Close: XDestroyImage(_MainImage);
        #define $Window_Paint case 0:
        #define $Window_Size case eWindow_Size:\
        						MainDevice->Width = message->xconfigure.width;\
        						MainDevice->Height = message->xconfigure.height;
        #define $Window_Create case eWindow_Create:

        #define $KeyBoard_Char case -2:
        #define $KeyBoard_Down case eKeyBoard_Down: \
                {\
                	static char s[2] = {0,0};\
                	static KeySym _KeyS[2] = {0,0}; \
                	XLookupString(&(message->xkey), s, 2, _KeyS, 0);\
                	sKey.Layout = message->xkey.state;\
                	sKey.Key = message->xkey.keycode;\
                	sKey.Char = s[0] ? s[0] : _KeyS[0];\
                	sKey.Shift = (_KeyS[0] == XK_Shift_L || _KeyS[0] == XK_Shift_R);\
                	sKey.Control = (_KeyS[0] == XK_Control_L || _KeyS[0] == XK_Control_R);\
                }

        #define $KeyBoard_Up case eKeyBoard_Up: sKey.Char=sKey.Key=sKey.Shift=sKey.Control=0;
        #define $KeyBoard_Default $KeyBoard_Char $end $KeyBoard_Down $end $KeyBoard_Up $end

    #define $EventReceiver_End } return 1; }

int run()
{
	MainDevice->display = XOpenDisplay(0);
	if(!MainDevice->display) return 1;

	int scr = DefaultScreen(MainDevice->display);

	int blackColor = BlackPixel(MainDevice->display, scr);

	MainDevice->window = XCreateSimpleWindow(MainDevice->display,
											 RootWindow(MainDevice->display, scr),
											 0, 0, MainDevice->Width, MainDevice->Height,
											 0, blackColor, blackColor);
	XSetWindowBackgroundPixmap(MainDevice->display, MainDevice->window, None);
	XStoreName(MainDevice->display, MainDevice->window, MainDevice->Caption);

	XSelectInput(MainDevice->display, MainDevice->window,
				 StructureNotifyMask | ExposureMask | PointerMotionMask |
				 ButtonPressMask | KeyPressMask | ButtonReleaseMask | KeyReleaseMask);

	XMapWindow(MainDevice->display, MainDevice->window);

	XWindowAttributes attr;
	XGetWindowAttributes(MainDevice->display, DefaultRootWindow(MainDevice->display), &attr);

	MainDevice->BackBuffer->Width=attr.width;
	MainDevice->BackBuffer->Height=attr.height;

	MainDevice->BackBuffer->transparent=false;
	MainDevice->BackBuffer->depth = 4;

	MainDevice->BackBuffer->cnst = MainDevice->BackBuffer->depth*attr.width;
	MainDevice->BackBuffer->Cnst = MainDevice->BackBuffer->cnst*attr.height;
	MainDevice->BackBuffer->data=new unsigned char[MainDevice->BackBuffer->Cnst];
	MainDevice->BackBuffer->clear();

	MainDevice->Containers._tdata=new unsigned char[MainDevice->BackBuffer->Cnst];

	sMouse.LeftPressed=sMouse.RightPressed=false;

	setbuf(stdout, NULL);

	XEvent e;

	while(1)
	{
		int p = XPending(MainDevice->display);

	    if(p) XNextEvent(MainDevice->display, &e);
	    else e.type = 0;

	    if(!WindowProcedure(&e)) break;
	}

	XCloseDisplay(MainDevice->display);
}

#endif

///---------------------------------------------------------------------------------------------------------------

template<typename T>
struct Vector3d
{
    T x,y,z;

    #ifdef USE_VECTOR_ID
    unsigned id;
    #endif

    ///пустой конструктор
    Vector3d()
    {
    }

    ///однопараметрный конструтор
    Vector3d(T a)
    {
        x=y=z=a;
    }

    ///урезанный конструктор
    Vector3d(T X,T Y) : x(X), y(Y)
    {
        z=0;
    }

    ///нормальный конструктор
    Vector3d(T X,T Y,T Z) : x(X),y(Y),z(Z)
    {

    }

    ///определяем операторы сложения/умножения/деления... для точки
    ///(для удобства записи, чтобы покомпонентно не расписывать)

    /*operator double()
    {
        return sqrt(x*x+y*y+z*z);
    }*/

    Vector3d& operator=(const Vector3d& v)
    {
        x = v.x;
        y = v.y;
        z = v.z;

        return *this;
    }

    Vector3d& Normalize()
    {
        static double r;

        r=(x*x+y*y+z*z);

        if(r>1e-18 && fabs(r-1)>1e-10)
        {
            r=sqrt(r);

            x/=r;
            y/=r;
            z/=r;
        }

        return *this;
    }

    Vector3d& Ortho()   ///orthogonalization
    {
        z = x;
        x = y;
        y = -z;
        z = 0;

        return *this;
    }

    T sAbs() const
    {
        return max3(fabs(x),fabs(y),fabs(z));
    }

    void Abs()
    {
        x=_Abs(x);
        y=_Abs(y);
        z=_Abs(z);
    }

    T sqr() const
    {
        return x*x+y*y+z*z;
    }

    Vector3d operator*(const Vector3d& a) const
    {
        return Vector3d(x*a.x,y*a.y,z*a.z);
    }

    bool operator==(const Vector3d& a) const
    {
        return (x==a.x && y==a.y && z==a.z);
    }

    Vector3d operator+(const Vector3d& a) const
    {
        return Vector3d(x+a.x,y+a.y,z+a.z);
    }

    void operator+=(const Vector3d& a)
    {
        x+=a.x;
        y+=a.y;
        z+=a.z;
    }

    void operator-=(const Vector3d& a)
    {
        x-=a.x;
        y-=a.y;
        z-=a.z;
    }

    Vector3d operator-(const Vector3d& a) const
    {
        return Vector3d(x-a.x,y-a.y,z-a.z);
    }

    Vector3d operator*(double k) const
    {
        return Vector3d(x*k,y*k,z*k);
    }

    Vector3d& operator*=(double k)
    {
        x*=k;
        y*=k;
        z*=k;

        return *this;
    }

    Vector3d& operator*=(const Vector3d& k)
    {
        x*=k.x;
        y*=k.y;
        z*=k.z;

        return *this;
    }

    friend Vector3d operator*(double k, const Vector3d& a)
    {
        return Vector3d(a.x*k,a.y*k,a.z*k);
    }

    T operator&(const Vector3d& v)     ///scalar
    {
        return x*v.x+y*v.y+z*v.z;
    }

    T operator|(const Vector3d& v)     ///z-component of a vector product
    {
        return x*v.y - y*v.x;
    }

    Vector3d operator^(const Vector3d& v)  ///vector
    {
        return Vector3d(y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
    }

    Vector3d operator/(double k) const
    {
        return Vector3d(x/k,y/k,z/k);
    }

    Vector3d& operator/=(double k)
    {
        x/=k;
        y/=k;
        z/=k;

        return *this;
    }

    bool operator>=(const Vector3d& a) const
    {
        return (x>=a.x && y>=a.y && z>=a.z);
    }

    bool operator<=(const Vector3d& a) const
    {
        return (x<=a.x && y<=a.y && z<=a.z);
    }

    operator pixel() const
    {
        return pixel(x<0 ? 0 : round(x)>255 ? 255 : round(x),
                     y<0 ? 0 : round(y)>255 ? 255 : round(y),
                     z<0 ? 0 : round(z)>255 ? 255 : round(z));
    }
};

#define Point Vector3d<double>
#define Coord Vector3d<int>

///поворот вокруг произвольной оси, задаваемой ортом l (|l|=1)
inline void rotate(Point** v,int cnt_v,Point Center,double angle,Point l)
{
    double cosa=cos(angle),sina=sin(angle);
    double _r,x,y,z;

    for(int b=0;b<cnt_v;b++)
    {
        _r=(1-cosa)*(v[b]->x*l.x + v[b]->y*l.y + v[b]->z*l.z);

        x=v[b]->x;
        y=v[b]->y;
        z=v[b]->z;

        v[b]->x = _r*l.x + x*cosa + sina*(z*l.y - y*l.z);
        v[b]->y = _r*l.y + y*cosa + sina*(x*l.z - z*l.x);
        v[b]->z = _r*l.z + z*cosa + sina*(y*l.x - x*l.y);
    }
}

#endif
