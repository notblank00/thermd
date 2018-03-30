#ifndef _GUI_H_INCLUDED
#define _GUI_H_INCLUDED
#define ALL -1

#define $extend(A,n,N,type) {type* _T=new type[N]; memcpy(_T,A,sizeof(type)*n); delete [] A; A=_T;}

#include "Font.h"
#include "_char_s.h"

#ifndef BITMAP_H_INCLUDED
    #include "Bitmap.h"
#endif

#include <stdarg.h>
#include <dirent.h>

#define SColor pixel
#define _drawLine DrawLine
#define _PutPixel PutPixel
#define _drawRect FillRect
#define _DrawImage(x,y,img) DrawImage(img,x,y)

/// GUI classes:
///
/// Button(Font* font, int x0, int y0, int Width, int Height, char* caption,bool hold=false)
///
/// Button(Font* font, int x0, int y0, char* caption, bool hold=false)
///
/// Edit  (Font* font, int x0, int y0, int Width, int Height, bool numeric=false,unsigned short style=0)
///
/// Label (Font* font, int x0, int y0, char* text)
///
/// MenuStrip(unsigned short Style,SColor color,Font* font...)
///
/// Table (Font* font, int x0, int y0, int Width, int Height, int cell_Width,int cell_Height,char* caption="Untitled")
///
/// CheckBox          (Font* font, int x0, int y0, char* text, bool checked=false)
///
/// Slider (Font* font, int x, int y,  int width, double k, char* text=0, int Space=5, Font* TxFont=0)
///
/// Dialog(Font* font, char* message, short type=ALERT,SColor color=SColor(255,235,240,250))
/// Dialog(Font* font, int x0, int y0, int Width, int Height, char* text,Button* done,Button* cancel,Edit* edit,SColor color=SColor(255,235,240,250))
///
/// Scroll(Font* font, int x0, int y0...)
///
/// FileDialog(Font* font,unsigned short type,int x,int y,char* start_path...)
///
/// MatchTable()
///
/// TextBox()

namespace _GUI
{
    enum
    {
        MENU_MAIN=0,
        MENU_NODE=1
    };

    enum
    {
        EDIT_STATIC=0,
        EDIT_NORMAL=1,
        EDIT_LABEL=2
    };

    enum
    {
        OPEN_FILE=0,
        CREATE_FILE=1
    };

    enum
    {
        PROMT=0,
        ALERT=1,
        CONFIRM=2,
        MESSAGE=3
    };

    enum
    {
        GS_NONE=0,

        GS_PLAIN=1,
        GS_GRAD=2,

        GS_HORIZ=16,
        GS_VERT=32
    };

    //char _Buffer[255];

    class GUI
    {
        protected:

        Font* _font;

        public:

        unsigned char _type;

        static bool _FOCUSSED;

        bool isVisible;
        bool isFocussed;
        int X,Y;
        unsigned Width,Height;

        //IrrlichtDevice* _device;
        //IVideoDriver* _driver;

        struct sElement
        {
            char* name;
            char* extention;

            ~sElement()
            {
                delete [] name;
                delete [] extention;
            }
        };

        virtual void Draw()=0;
        virtual bool tryFocuss()=0;
        virtual void KeyPress()=0;
    };

    bool GUI::_FOCUSSED=false;

    class Separator : public GUI
    {
        protected:

        unsigned short dr;
        pixel color;

        public:

        Separator(short r=4,pixel color=pixel(100,0,0,0))      ///pixel(150,200,210,255)
        {
            dr=r;

            Width=Height=0;

            this->color=color;

            _type=2;
        }

        ~Separator()
        {

        }

        void Draw()
        {
            if(Width==0)
            {
                DrawLine(X+dr,Y,X+dr,Y+Height,color);
                DrawLine(X+dr+1,Y,X+dr+1,Y+Height,!color);
            }
            else if(Height==0)
            {
                DrawLine(X,Y+dr,X+Width,Y+dr,color);
                DrawLine(X,Y+dr+1,X+Width,Y+dr+1,!color);
            }
        }

        bool tryFocuss()
        {
            if(Width==0)
            {
                return MouseX==X && MouseY>=Y && MouseY<=Y+Height;
            }
            else if(Height==0)
            {
                return MouseY==Y && MouseX>=X && MouseX<=X+Width;
            }
            else return false;
        }

        void KeyPress()
        {

        }
    };

    class GUIGroup : public GUI
    {
        public:

        struct component
        {
            char* name;
            GUI* P;

            component(GUI* A,char* Name=0)
            {
                P=A;

                if(Name)
                {
                    name=new char[lng(Name)+1];
                    copy(Name,name);
                }
            }

            ~component()
            {
                delete [] name;
            }
        };

        struct GSParam
        {
            int dx,dy;
            unsigned char style;

            pixel uColor;
            pixel lColor;

            bool Shadowed;

            GSParam(unsigned char Style,bool shadowed,pixel color,pixel _color=pixel(0),int Dx=4,int Dy=4)
            {
                style=Style;
                uColor=color;
                Shadowed=shadowed;

                dx=dy=4;

                uColor=color;
                lColor=_color;
            }
        };

        component** data;
        int N,N_MAX;

        GSParam* param;

        GUIGroup(unsigned char Style=GS_PLAIN+GS_HORIZ,
                 bool shadowed=true,
                 pixel color=pixel(240,245,255),
                 pixel _color=pixel(202,215,250))
        {
            data=new component*[N_MAX=8];
            N=0;

            param=new GSParam(Style,shadowed,color,_color);

            X=Y=2147483647;
            Width=Height=0;

            _type=1;

            isVisible=true;
        }

        ~GUIGroup()
        {
            delete [] data;
            N=0;
        }

        int GetHeight()
        {
            return Height+2*param->dy;
        }

        int GetWidth()
        {
            return Width+2*param->dx;
        }

        void SetCorner(int x,int y)
        {
            Width += X-x;
            Height += Y-y;

            X=x;
            Y=y;
        }

        void SetPos(int x,int y)
        {
            for(int k=0;k<N;k++)
            {
                if(data[k]->P)
                {
                    data[k]->P->X += x-X;
                    data[k]->P->Y += y-Y;
                }
            }

            X=x;
            Y=y;
        }

        void Add(unsigned short dr)        ///Separator
        {
            if(N==N_MAX)
            {
                N_MAX+=10;

                $extend(data,N,N_MAX,component*);
            }

            data[N++]=new component(new Separator(dr));

            bool dimensional = true;

            if(N>1 && data[N-1]->P->_type==2)
            {
                if(param->style & GS_HORIZ) data[N-1]->P->X = data[N-2]->P->X + data[N-2]->P->Width;
                else if(param->style & GS_VERT) data[N-1]->P->Y = data[N-2]->P->Y + data[N-2]->P->Height;
                else data[N-1]->P->_type=0;     ///no separators in plain-styled groups

                dimensional=false;
            }

            if(dimensional)
            {
                if(data[N-1]->P->X < X)
                {
                    if(X != 2147483647) Width += X-data[N-1]->P->X;
                    X=data[N-1]->P->X;
                }

                if(data[N-1]->P->Y < Y)
                {
                    if(Y != 2147483647) Height += Y-data[N-1]->P->Y;
                    Y=data[N-1]->P->Y;
                }

                if(data[N-1]->P->X + data[N-1]->P->Width > X+Width) Width = data[N-1]->P->X + data[N-1]->P->Width - X;
                if(data[N-1]->P->Y + data[N-1]->P->Height > Y+Height) Height = data[N-1]->P->Y + data[N-1]->P->Height - Y;
            }
        }

        void Add(GUI* A=0,char* name=0,bool dimensional=true)
        {
            if(N==N_MAX)
            {
                N_MAX+=10;

                $extend(data,N,N_MAX,component*);
            }

            if(A) data[N++]=new component(A,name);
            else data[N++]=new component(new Separator());

            if(N>1 && data[N-1]->P->_type==2)
            {
                if(param->style & GS_HORIZ) data[N-1]->P->X = data[N-2]->P->X + data[N-2]->P->Width;
                else if(param->style & GS_VERT) data[N-1]->P->Y = data[N-2]->P->Y + data[N-2]->P->Height;
                else data[N-1]->P->_type=0;     ///no separators in plain-styled groups

                dimensional=false;
            }

            if(dimensional)
            {
                if(data[N-1]->P->X < X)
                {
                    if(X != 2147483647) Width += X-data[N-1]->P->X;
                    X=data[N-1]->P->X;
                }

                if(data[N-1]->P->Y < Y)
                {
                    if(Y != 2147483647) Height += Y-data[N-1]->P->Y;
                    Y=data[N-1]->P->Y;
                }

                if(data[N-1]->P->X + data[N-1]->P->Width > X+Width) Width = data[N-1]->P->X + data[N-1]->P->Width - X;
                if(data[N-1]->P->Y + data[N-1]->P->Height > Y+Height) Height = data[N-1]->P->Y + data[N-1]->P->Height - Y;

                //printf("%d %d\n", data[N-1]->P->Width, data[N-1]->P->Height);
            }
        }

        component* operator[](char* name)
        {
            for(int k=0;k<N;k++)
            {
                if(data[k]->name[0]==name[0])
                {
                    if(eql(data[k]->name,name)) return data[k];
                }
            }

            return 0;
        }

        component* operator[](int k)
        {
            if(!N) return 0;
            else if(k<0 || k>=N) return data[N];
            else return data[k];
        }

        void Draw()
        {
            const int& dx=param->dx;
            const int& dy=param->dy;

            static const pixel sh_hu=pixel(120);
            static const pixel sh_hl=pixel(40);
            static const pixel sh_vl=pixel(80);
            static const pixel sh_vr=pixel(30);

            static pixel _c;
            static int _p,_w;

            if(!isVisible) return;

            switch(param->style & 15)
            {
                case GS_GRAD:

                    if(param->style & GS_HORIZ)
                    {
                        _p=max(0,Y-dy);
                        _w=Y+Height+dy;

                        for(int y=_p;y<=_w;y++)
                        {
                            _c.a = param->uColor.a - (param->uColor.a - param->lColor.a)*(y-_p)/(_w-_p);

                            _c.r = param->uColor.r - (param->uColor.r - param->lColor.r)*(y-_p)/(_w-_p);
                            _c.g = param->uColor.g - (param->uColor.g - param->lColor.g)*(y-_p)/(_w-_p);
                            _c.b = param->uColor.b - (param->uColor.b - param->lColor.b)*(y-_p)/(_w-_p);

                            DrawLine(X-dx,y,X+Width+dx,y,_c);
                        }
                    }
                    else if(param->style & GS_VERT)
                    {
                        _p=max(0,X-dx);
                        _w=X+Width+dx;

                        for(int x=_p;x<=_w;x++)
                        {
                            _c.a = param->uColor.a - (param->uColor.a - param->lColor.a)*(x-_p)/(_w-_p);

                            _c.r = param->uColor.r - (param->uColor.r - param->lColor.r)*(x-_p)/(_w-_p);
                            _c.g = param->uColor.g - (param->uColor.g - param->lColor.g)*(x-_p)/(_w-_p);
                            _c.b = param->uColor.b - (param->uColor.b - param->lColor.b)*(x-_p)/(_w-_p);

                            DrawLine(x,Y-dy,x,Y+Height+dy,_c);
                        }
                    }

                    break;

                case GS_PLAIN:
                    FillRect(X-dx,Y-dy, X+Width+dx, Y+Height+dy, param->uColor);
                    break;
            }

            if(param->Shadowed)
            {
                DrawLine(X-dx,Y+Height+dy+1, X+Width+dx,Y+Height+dy+1, sh_hu);
                DrawLine(X-dx,Y+Height+dy+2, X+Width+dx+1,Y+Height+dy+2, sh_hl);

                DrawLine(X+Width+dx+1,Y-dy, X+Width+dx+1,Y+Height+dy+1, sh_vl);
                DrawLine(X+Width+dx+2,Y-dy, X+Width+dx+2,Y+Height+dy+1, sh_vr);

                DrawLine(X-dx-1,Y-dy, X-dx-1,Y+Height+dy+1, sh_hl);
                DrawLine(X-dx,Y-dy-1, X+Width+dx+1,Y-dy-1, sh_vr);
            }

            for(int k=0;k<N;k++)
            {
                if(data[k]->P->_type==2)
                {
                    if(param->style & GS_HORIZ)
                    {
                        data[k]->P->Y = max(0,Y-dy) +3;
                        data[k]->P->Height = Height+2*dy+min(0,Y-dy) -6;
                    }
                    else if(param->style & GS_VERT)
                    {
                        data[k]->P->X = max(0,X-dx) +3;
                        data[k]->P->Width = Width+2*dx+min(0,X-dx) -6;
                    }

                    data[k]->P->_type = 0;      ///separator set up
                }

                data[k]->P->Draw();
            }
        }

        bool tryFocuss()
        {
            isFocussed=_FOCUSSED=false;

            for(int k=0;k<N;k++)
            {
                bool r=data[k]->P->tryFocuss();
                if(r) isFocussed=_FOCUSSED=true;
            }

            return _FOCUSSED;
        }

        void KeyPress()
        {
            for(int k=0;k<N;k++) data[k]->P->KeyPress();
        }
    };

    class Button : public GUI
    {
        private:

        bool _hold;

        public:

        //int X,Y;
        //unsigned Width,Height;
        //bool isVisible;
        bool CursorOn;
        bool Enabled;
        char* Caption;
        Image* img;

        Button()
        {
            isVisible=false;
            img=new Image();
        }

        Button(Font* font,int x0,int y0,unsigned width,unsigned height,char* caption,bool hold=false)
        {
            X=x0;
            Y=y0;
            Width=width;
            Height=height;

            isVisible=true;
            CursorOn=false;
            Enabled=true;

            _font=font;
            _hold=hold;

            Caption=new char[lng(caption)+1];
            copy(caption,Caption);
            img=new Image();
        }

        Button(Font* font,int x0,int y0,char* caption,bool hold=false)
        {
            X=x0;
            Y=y0;
            Width=Text_Width(font,caption)+10;
            Height=font->Height+8;

            isVisible=true;
            CursorOn=false;
            Enabled=true;

            _font=font;
            _hold=hold;

            Caption=new char[lng(caption)+1];
            copy(caption,Caption);
            img=new Image();
        }

        bool tryFocuss()
        {
            int _m_x=sMouse.X;
            int _m_y=sMouse.Y;

            if(!isVisible) CursorOn=false;
            else CursorOn=(_m_x>=X && _m_x<=X+Width && _m_y>=Y && _m_y<=Y+Height && Y>=0);// ? true : false;

            return (isLeftPressed && CursorOn && Enabled);
        }

        void KeyPress(){}

        void Draw()
        {
            if(isVisible)
            {
                int _m_x=sMouse.X;
                int _m_y=sMouse.Y;

                CursorOn=(_m_x>=X && _m_x<=X+Width && _m_y>=Y && _m_y<=Y+Height && Y>=0);// ? true : false;

                unsigned char _add=(isLeftPressed && Enabled && (CursorOn || _hold)) ? 50 : 0;

                _drawLine(X+1,Y,X+Width-1,Y,SColor(255,112+_add,112+_add,122+_add));
                _drawLine(X,Y+1,X,Y+Height-1,SColor(255,112+_add,112+_add,122+_add));
                _drawLine(X+Width,Y+1,X+Width,Y+Height-1,SColor(255,112+_add,112+_add,122+_add));
                _drawLine(X+1,Y+Height,X+Width-1,Y+Height,SColor(255,112+_add,112+_add,122+_add));

                _drawLine(X+2,Y+1,X+Width-1,Y+1,SColor(255,243-_add/2,243-_add/2,243-_add/2));
                _drawLine(X+2,Y+Height-1,X+Width-1,Y+Height-1,SColor(255,243-_add/2,243-_add/2,243-_add/2));
                _drawLine(X+2,Y+1,X+2,Y+Height-1,SColor(255,243-_add/2,243-_add/2,243-_add/2));
                _drawLine(X+Width-1,Y+1,X+Width-1,Y+Height-1,SColor(255,243-_add/2,243-_add/2,243-_add/2));

                //for(int v=Y+2;v<Y+Height/2;v++) _drawLine(X+3,v,X+Width-1,v,SColor(255,240-_add,240-_add,240-_add));
                //for(int v=Y+Height/2;v<Y+Height-2;v++) _drawLine(X+3,v,X+Width-1,v,SColor(255,212-_add,212-_add,212-_add));
                _drawRect(X+1,Y+2,X+Width-1,Y+Height/2-1,SColor(255,240-_add,240-_add,240-_add));
                _drawRect(X+1,Y+Height/2,X+Width-1,Y+Height-1,SColor(255,212-_add,212-_add,212-_add));

                if(Enabled) Text_out(_font,X+(Width-Text_Width(_font,Caption))/2,Y+(Height-_font->Height)/2,Caption);
                else Text_out(_font,X+(Width-Text_Width(_font,Caption))/2,Y+(Height-_font->Height)/2,Caption,Color{150,150,150});

                _DrawImage(X+(Width-img->Width)/2+1,Y+(Height-img->Height)/2+1,img);
            }
        }

        ~Button()
        {
            delete [] Caption;
            _font=(Font*)0;
            delete img;
        }
    };

    class Edit : public GUI
    {
        private:

        int _offset;
        int _l;
        int _Lmax;
        bool _numeric;
        //char _Text[100];
        unsigned char _check;

        public:

        //int X,Y;
        //unsigned Height,Width;
        //bool isVisible;
        bool isFocussed;
        bool Selected;
        char* Text;
        unsigned short Style;

        Edit()
        {
            isVisible=false;
        }

        Edit(Font* font,int x0,int y0,int width,int height=-1,bool numeric=false,unsigned short style = EDIT_NORMAL)
        {
            X=x0;
            Y=y0;
            Width=width;
            Height=height;

            if(height<=0) Height=font->Height+4;

            isVisible=true;
            isFocussed=false;

            Selected=false;
            _numeric=numeric;
            _offset=_l=0;

            Style=style;

            _font=font;
            //_cur_pos=x0;

            _Lmax=128;
            delete [] Text;

            Text=new char[_Lmax+1];

            clear(Text);
            //clear(_Text);
        }

        bool tryFocuss()
        {
            if(isVisible)
            {
                int _m_x=sMouse.X;
                int _m_y=sMouse.Y;

                isFocussed=(_m_x>=X && _m_x<=X+Width && _m_y>=Y && _m_y<=Y+Height);// ? true : false;

                if(!isFocussed) Selected=false;
                else if(isDoubleClicked) Selected=true;

                return isLeftPressed && isFocussed;
            }
            else return false;
        }

        void Set(char* text)
        {
            static int L;
            L=lng(text);

            if(L>=_Lmax)
            {
                delete [] Text;

                _Lmax=L+128;
                Text=new char[_Lmax+1];

                if(_Lmax>16000000) printf("String with over 16 million characters. Seriously? -_-\n");
            }

            memcpy(Text,text,L+1);
            _l=L;
        }

        void KeyPress()
        {
            static int _i;

            unsigned char& _Char = sKey.Char;
            unsigned char& _Key = sKey.Key;

            if(isVisible && isFocussed && Style)
            {
                if(_Key || _Char)
                {
                    if(_Key==8 || _Key==46)
                    {
                        if(Selected)
                        {
                            clear(Text);
                            _l=0;
                            _offset=0;
                        }
                        else if(_l>0 || (_Key==46 && _l==0))
                        {
                            _i=_l-1+(_Key==46);
                            while(Text[_i]) Text[_i++]=Text[_i+1];

                            if(_Key==8) _l--;

                            if(Text_Width(_font,Text)<Width) _offset=0;
                        }
                    }
                    else if(_Key==39)
                    {
                        if(_l<lng(Text)) _l++;
                        if(Text_Width(_font,Text-_offset,0,_l)>Width) _offset--;
                    }
                    else if(_Key==37)
                    {
                        if(_l+_offset<=0 && _offset<0) _offset++;
                        if(_l>0) _l--;
                    }
                    else if(_Char)
                    {
                        if(_Char==3 && Selected)
                        {
                            if(OpenClipboard(MainDevice->hwnd))
                            {
                                HGLOBAL data;

                                EmptyClipboard();

                                data = GlobalAlloc(GMEM_MOVEABLE,lng(Text)+1);

                                char* _text=(char*)GlobalLock(data);
                                memcpy(_text,Text,lng(Text)+1);
                                GlobalUnlock(data);

                                SetClipboardData(CF_TEXT,data);
                                CloseClipboard();
                            }
                        }
                        else if(_Char==1)
                        {
                            Selected=true;
                            return;
                        }
                        else if(_Char==22)
                        {
                            if(OpenClipboard(MainDevice->hwnd))
                            {
                                HANDLE data=GetClipboardData(CF_TEXT);

                                if(!data) return;

                                char* str=(char*)GlobalLock(data);

                                GlobalUnlock(data);

                                Set(str);
                                _offset=0;
                                Selected=true;

                                CloseClipboard();
                                return;
                            }
                        }
                        else if(_Char>28 && (!_numeric || IsNumeric(_Char))) ///|| _Char == '%'
                        {
                            if(Selected)
                            {
                                clear(Text);
                                _l=0;
                                _offset=0;
                            }

                            if(_Key!=39 && _Key!=37 && _Char!=0)
                            {
                                _i=_l;

                                char _c=_Char;

                                while(Text[max(-1,_i-2)])
                                {
                                    Text[_i]=Text[_i]^_c;
                                    _c=Text[_i]^_c;
                                    Text[_i]=Text[_i]^_c;

                                    _i++;
                                }

                                Text[_i]=0;

                                if(_i-2>=_Lmax)
                                {
                                    char* __t=new char[_i-1];
                                    memcpy(__t,Text,_i-1);

                                    delete [] Text;

                                    _Lmax+=128;
                                    Text=new char[_Lmax+1];

                                    memcpy(Text,__t,_i-1);

                                    if(_Lmax>16000000) printf("String with over 16 million characters. Seriously? -_-\n");
                                }

                                _l++;

                                if(Text_Width(_font,Text-_offset)>Width) _offset--;
                            }
                        }
                    }

                    Selected=false;
                }
            }
        }

        void Enable()
        {
            Style = Style | EDIT_NORMAL;
        }

        void Disable()
        {
            Style = Style & EDIT_STATIC;
        }

        void Draw()
        {
            if(isVisible)
            {
                if(!Style && Selected) Selected=false;

                if(!(Style & EDIT_LABEL)) _drawRect(X,Y+1,X+Width,Y+Height,SColor(255,240,240,240));

                //int _w=((Text_Width(_font,Text)<=Width) ? Text_Width(_font,Text) : Width);

                if(Selected) _drawRect(X,Y+3,X+((Text_Width(_font,Text)<=Width) ? Text_Width(_font,Text) : Width),Y+_font->Height+1,SColor(255,15,15,15));
                else if(!Style) _drawRect(X,Y+2,X+Width,Y+Height,SColor(255,255,255,120));

                int _cur_pos=Text_out(_font,X+2,Y+(Height-_font->Height)/2,Text-_offset,Color{0,0,0},Width-2,Selected,0,true);
                if(_cur_pos>X+Width) _cur_pos=X+Width;//-1;

                if(!(Style & EDIT_LABEL))
                {
                    _drawLine(X+1,Y,X+Width,Y,SColor(255,67,69,76));

                    _drawLine(X+1,Y+1,X+Width,Y+1,SColor(255,200,200,200));
                    _drawLine(X+1,Y+2,X+1,Y+Height,SColor(200,200,200,200));

                    _drawLine(X,Y+1,X,Y+Height,SColor(255,95,96,102));
                    _drawLine(X+Width,Y+1,X+Width,Y+Height,SColor(255,130,133,141));
                }

                _drawLine(X+1,Y+Height,X+Width,Y+Height,SColor(255,138,141,153));

                if(isFocussed && Style)
                {
                    int __x;

                    if(_l+_offset==0) __x=X+2;
                    else __x=X+min(Width-1,Text_Width(_font,Text-_offset,0,_l+_offset)+2);

                    _drawLine(__x,Y+2,__x,Y+_font->Height+2,SColor(255,0,0,0));
                }

                ///printf("%d %d %d\n",_l,_offset,lng(Text));
            }
        }

        ~Edit()
        {
            delete [] Text;
            Text = 0;
            //delete [] _Text;

            _font=(Font*)0;
        }
    };

    class Label : public GUI
    {
        private:

        //int _w;
        char* _t;

        public:

        Label(Font* font, int x0, int y0, char* text)
        {
            X=x0;
            Y=y0;

            _font=font;
            Height=_font->Height;

            _t=new char[lng(text)+1];
            copy(text,_t);

            Width=Text_Width(font,text);

            isVisible=true;
        }

        void Set(char* text)
        {
            int l=lng(text);

            if(l>lng(_t))
            {
                delete [] _t;
                _t=new char[l+1];
            }

            copy(text,_t);

            Width=Text_Width(_font,_t);
        }

        int GetWidth()
        {
            return Width;
        }

        int GetHeight()
        {
            return Height;
        }

        void Draw()
        {
            if(isVisible && _t) Text_out(_font,X,Y,_t);
        }

        bool tryFocuss()
        {
            return isLeftPressed && (MouseX>=X && MouseX<=X+Width && MouseY>=Y && MouseY<=Y+_font->Height);
        }

        void KeyPress()
        {

        }

        ~Label()
        {
            delete [] _t;
            _t=0;
        }

    };

    class MenuStrip : public GUI
    {
        private:

        unsigned _n,_max;
        int _nX,_nY;
        unsigned short _style;

        unsigned char _a,_r,_g,_b;

        public:
        char** Element;
        SColor color;

        MenuStrip(unsigned short Style,SColor color,Font* font...)
        {
            _font=font;
            _n=0;
            _max=0;
            _nY=font->Height+7;
            _style=Style;

            _a=color.a;
            _r=color.r;
            _g=color.g;
            _b=color.b;

            MenuStrip::color=color;

            va_list lst;
            va_start(lst,font);

            while(1)
            {
                char* p=va_arg(lst,char*);
                if(p==0) break;
                else if(Text_Width(_font,p)>_max) _max=Text_Width(_font,p);
                _n++;
            }

            _max+=10;

            va_end(lst);

            Element=new char*[_n];

            va_start(lst,font);

            for(int b=0;b<_n;b++)
            {
                char* p=va_arg(lst,char*);

                Element[b]=new char[lng(p)];
                clear(Element[b]);
                set(Element[b],p);
            }

            va_end(lst);

        }

        void Draw(int X=0,int Y=0)
        {
            _nX=X;

            int _tx=(_style==MENU_MAIN) ? 10 : Y;

            int _m_X=sMouse.X;
            int _m_Y=sMouse.Y;

            if(_style==MENU_NODE)
            {
                _drawRect(X-3,_tx-5,X+_max+5,_tx+_n*_font->Height+5,color);

                for(int b=0;b<4;b++)
                {
                    _drawLine(X+5,_tx+_n*_font->Height+5+b,X+_max+6+b,_tx+_n*_font->Height+5+b,SColor((4-b)*25,0,0,0));
                    _drawLine(X+_max+5+b,_tx,X+_max+5+b,_tx+_n*_font->Height+5+b,SColor((4-b)*25,0,0,0));
                }
            }
            else
            {
                _drawRect(0,0,getWidth(),2*(_font->Height)/3,color);
                _drawRect(0,2*(_font->Height)/3,getWidth(),_font->Height+7,SColor(_a,_r-17,_g-17,_b-17));
            }

            for(int b=0;b<_n;b++)
            {
                unsigned w=Text_Width(_font,Element[b]);

                if(_style==MENU_MAIN)
                {
                    if(_m_Y<_font->Height+5 && _m_Y>0) if(_m_X>_tx && _m_X<_tx+w) _drawRect(_tx,2,_tx+w+5,_font->Height+7,SColor(70,0,0,255));

                    Text_out(_font,_tx,_font->Height+5,Element[b]);
                    _tx+=w+10;
                }
                else
                {
                    if(_m_Y>_tx && _m_Y<_tx+_font->Height && _m_Y>0) if(_m_X>X && _m_X<X+_max) _drawRect(X,_tx,X+_max,_tx+_font->Height,SColor(70,0,0,255));

                    Text_out(_font,X,_tx+_font->Height,Element[b]);
                    _tx+=_font->Height;
                }
            }

            _nY=_tx;
        }

        int OnClick()
        {
            int _tx=10;

            int _m_X=sMouse.X;
            int _m_Y=sMouse.Y;

            if(_style==MENU_MAIN)
            {
                for(int b=0;b<_n;b++)
                {
                    unsigned w=Text_Width(_font,Element[b]);

                    if(_m_Y<_font->Height+5 && _m_Y>0) if(_m_X>_tx && _m_X<_tx+w)
                    {
                        return b;
                    }
                    else _tx+=w+10;
                }
            }
            else if( (_m_X>_nX && _m_X<_nX+_max) && (_m_Y>_font->Height+7 && _m_Y<_nY && _m_Y>0)) return (_m_Y-_font->Height+7)/(_font->Height)-1;

            return -1;
        }

        int GetX(short index)
        {
            int _tx= (_style==MENU_MAIN) ? 10 : _font->Height+7;

            if(_style==MENU_MAIN) for(int b=0;b<index;b++) _tx+=Text_Width(_font,Element[b])+10;
            else return _max+10;

            return _tx;
        }

        int GetY(short index)
        {
            int _tx=_font->Height+7;

            if(_style==MENU_NODE) for(int b=0;b<index;b++) _tx+=_font->Height;

            return _tx+10;
        }

        ~MenuStrip()
        {
            for(int b=0;b<_n;b++) delete [] Element[b];
            delete [] Element;
            _font=(Font*)0;

            _n=0;
        }
    };

    class Table : public GUI
    {
        private:

        unsigned _H,_W;
        char* _Caption;
        bool _hidden,once;

        public:
        //int X,Y;
        Edit*** cell;
        Button* hide;
        Button* draw;
        bool CursorOn;
        bool CursorOn_a;
        bool isFocussed;
        unsigned cell_width,cell_height;

        Table(Font* font,int x,int y,int Width,int Height,int cell_Width,int cell_Height,char* caption="Untitled")
        {
            x+=30;
            Height++;

            int cx=x,cy=y;

            unsigned short stl=0;

            once=true;

            _font=font;

            X=x;
            Y=y;

            cell_width=cell_Width;
            cell_height=cell_Height;

            _H=Height;
            _W=Width;

            _hidden=false;
            isFocussed=true;

            _Caption=new char[lng(caption)];

            set(_Caption,caption);

            hide=new Button(font,x+Width*cell_Width-30,y-15,30,15,"<<");
            draw=new Button(font,x+Width*cell_Width-74,y-15,44,15,"Draw");

            cell=new Edit**[Width];

            for(int b=0;b<Width;b++) cell[b]=new Edit*[Height];

            for(int i=0;i<Width;i++)
            {
                for(int j=0;j<Height;j++)
                {
                    stl=(j==0) ? 1 : 0;

                    cell[i][j]=new Edit(font,cx,cy,cell_Width,cell_Height,true,stl);

                    cell[i][j]->X=cx;
                    cell[i][j]->Y=cy;

                    cy+=cell_Height;
                }
                cy=y;
                cx+=cell_Width;
            }
        }

        void ClearData()
        {
            for(int i=0;i<_W;i++) for(int j=1;j<_H;j++) clear(cell[i][j]->Text);
        }

        void SetPosition(int x,int y)
        {
            if(isFocussed)
            {
                int cx=x,cy=y;

                unsigned w=cell[0][0]->Width,h=cell[0][0]->Height;

                for(int i=0;i<_W;i++)
                {
                    for(int j=0;j<_H;j++)
                    {
                        cell[i][j]->X=cx;
                        cell[i][j]->Y=cy;

                        cy+=h;
                    }
                    cy=y;
                    cx+=w;
                }

                hide->X=x+_W*w-30;
                hide->Y=y-15;

                draw->X=x+_W*w-74;
                draw->Y=y-15;

                X=x;
                Y=y;
            }
        }

        void SetCaption(char* caption)
        {
            if(Text_Width(_font,caption)<_W*cell[0][0]->Width-30)
            {
                clear(_Caption);

                delete [] _Caption;
                delete _Caption;

                _Caption=new char[lng(caption)];

                set(_Caption,caption);
            }
        }

        bool tryFocuss()
        {
            int _m_x=sMouse.X;
            int _m_y=sMouse.Y;

            if(_m_x>=X && _m_x<=X+_W*cell_width+30 && _m_y>=Y-15 && _m_y<=Y+_H*cell_height)
            {
                CursorOn_a=!_hidden && (_m_x>=X+_W*cell_width && _m_x<=X+_W*cell_width+15 && _m_y>=Y+15 && _m_y<=Y+_H*cell_height);

                CursorOn=(_m_x>=X && _m_x<=X+_W*cell[0][0]->Width-30 && _m_y>=Y-15 && _m_y<=Y) || CursorOn_a;
            }
            else CursorOn=CursorOn_a=false;

            if(isFocussed)
            {
                for(int i=0;i<_W;i++)
                {
                    for(int j=0;j<_H;j++)
                    {
                        if(cell[i][j]->tryFocuss())
                        {
                            if(cell[i][j]->Y<0) cell[i][j]->isFocussed=cell[i][j]->Selected=false;
                            else cell[i][j]->Selected=true;
                        }
                        else cell[i][j]->Selected=false;
                    }
                }
            }

            return (!_hidden) ? (_m_x>=X && _m_x<=X+_W*cell_width && _m_y>=Y-15 && _m_y<=Y+_H*cell_height) :
                                            (_m_x>=X && _m_x<=X+_W*cell[0][0]->Width && _m_y>=Y-15 && _m_y<=Y);
        }

        void KeyPress()
        {
           static bool once=true;

           unsigned char& _Char = sKey.Char;

           if(isFocussed)
           {
               for(int i=0;i<_W;i++)
               {
                   for(int j=0;j<_H;j++)
                   {
                       if(cell[i][j]->isFocussed)
                       {
                           if(_Char==0) once=true;

                           if((_Char==9 || _Char==13 || _Char==72 || _Char==80) && once)
                           {
                               cell[i][j]->isFocussed=false;

                               if(_Char==9) i=(i+1)%_W;
                               /*else if(_Char==75)
                               {
                                   if(i==0)
                                   {
                                       if(j>1)
                                       {
                                           j--;
                                           i=_W-1;
                                       }
                                   }
                                   else if(i>0) i--;
                               }*/

                               if((i==0 || _Char==13 || _Char==80) && !(_Char==72) && j<_H-1) j++;
                               else if((i==0 || _Char==72) && j>1) j--;

                               cell[i][j]->isFocussed=true;

                               once=false;
                           }

                           cell[i][j]->KeyPress();
                           goto loop_end;
                       }
                   }
               }
           }

           loop_end:{};
        }

        void Draw(bool Pressed,int cells_to_show=ALL)
        {
            char _st[10];

            if(isFocussed) _drawRect(X,Y-15,X+_W*cell[0][0]->Width,Y,SColor(255,200,215,255));
            else _drawRect(X,Y-15,X+_W*cell[0][0]->Width,Y,SColor(255,150,171,243));

            Text_out(_font,X+2,Y,_Caption);

            if(hide->tryFocuss() && Pressed && once && isFocussed)
            {
                _hidden=!_hidden;
                once=false;
            }

            if(!Pressed || !isFocussed) once=true;

            if(!_hidden)
            {
                if(cell_width!=cell[0][0]->Width)
                {
                    hide->X+=_W*(cell_width-cell[0][0]->Width);

                    for(int i=0;i<_W;i++)
                    {
                        for(int j=0;j<_H;j++)
                        {
                            if(i!=0) cell[i][j]->X+=i*(cell_width-cell[i][j]->Width);

                            cell[i][j]->Width=cell_width;
                        }
                    }
                }

                set(hide->Caption,"<<");

                hide->Draw();

                if(cells_to_show==ALL || cells_to_show>_H)
                {
                    _drawRect(X-30,Y,X,_H*cell_height+Y,SColor(255,220,225,245));
                    _drawRect(X+_W*cell_width,Y,X+_W*cell_width+15,_H*cell_height+Y,SColor(255,200,215,255));

                    _drawLine(X+_W*cell_width+3,_H*cell_height/2+Y,X+_W*cell_width+6,_H*cell_height/2+Y,SColor(255,100,100,255));
                    _drawLine(X+_W*cell_width+6,_H*cell_height/2+Y,X+_W*cell_width+9,_H*cell_height/2+Y,SColor(255,135,135,255));
                    _drawLine(X+_W*cell_width+9,_H*cell_height/2+Y,X+_W*cell_width+12,_H*cell_height/2+Y,SColor(255,165,165,255));

                    _drawLine(X+_W*cell_width+3,_H*cell_height/2+Y+1,X+_W*cell_width+5,_H*cell_height/2+Y+1,SColor(255,120,120,255));
                    _drawLine(X+_W*cell_width+5,_H*cell_height/2+Y+1,X+_W*cell_width+10,_H*cell_height/2+Y+1,SColor(255,177,177,255));
                    _drawLine(X+_W*cell_width+10,_H*cell_height/2+Y+1,X+_W*cell_width+12,_H*cell_height/2+Y+1,SColor(255,200,200,255));

                    _drawLine(X+_W*cell_width+3,_H*cell_height/2+Y+3,X+_W*cell_width+6,_H*cell_height/2+Y+3,SColor(255,100,100,255));
                    _drawLine(X+_W*cell_width+6,_H*cell_height/2+Y+3,X+_W*cell_width+9,_H*cell_height/2+Y+3,SColor(255,135,135,255));
                    _drawLine(X+_W*cell_width+9,_H*cell_height/2+Y+3,X+_W*cell_width+12,_H*cell_height/2+Y+3,SColor(255,165,165,255));

                    _drawLine(X+_W*cell_width+3,_H*cell_height/2+Y+4,X+_W*cell_width+5,_H*cell_height/2+Y+4,SColor(255,120,120,255));
                    _drawLine(X+_W*cell_width+5,_H*cell_height/2+Y+4,X+_W*cell_width+10,_H*cell_height/2+Y+4,SColor(255,177,177,255));
                    _drawLine(X+_W*cell_width+10,_H*cell_height/2+Y+4,X+_W*cell_width+12,_H*cell_height/2+Y+4,SColor(255,200,200,255));

                    _drawLine(X+_W*cell_width+3,_H*cell_height/2+Y+6,X+_W*cell_width+6,_H*cell_height/2+Y+6,SColor(255,100,100,255));
                    _drawLine(X+_W*cell_width+6,_H*cell_height/2+Y+6,X+_W*cell_width+9,_H*cell_height/2+Y+6,SColor(255,135,135,255));
                    _drawLine(X+_W*cell_width+9,_H*cell_height/2+Y+6,X+_W*cell_width+12,_H*cell_height/2+Y+6,SColor(255,165,165,255));

                    _drawLine(X+_W*cell_width+3,_H*cell_height/2+Y+7,X+_W*cell_width+5,_H*cell_height/2+Y+7,SColor(255,120,120,255));
                    _drawLine(X+_W*cell_width+5,_H*cell_height/2+Y+7,X+_W*cell_width+10,_H*cell_height/2+Y+7,SColor(255,177,177,255));
                    _drawLine(X+_W*cell_width+10,_H*cell_height/2+Y+7,X+_W*cell_width+12,_H*cell_height/2+Y+7,SColor(255,200,200,255));
                }
                else
                {
                    _drawRect(X-30,Y,X,(cells_to_show+1)*cell_height+Y,SColor(255,220,225,245));
                    _drawRect(X+_W*cell_width,Y,X+_W*cell_width+15,(cells_to_show+1)*cell_height+Y,SColor(255,200,215,255));
                }

                for(int i=0;i<_W;i++)
                {
                    for(int j=0;j<_H;j++)
                    {
                        if(cells_to_show==-1 || j<=cells_to_show)
                        {
                            cell[i][j]->Draw();

                            if(i==0)
                            {
                                _drawLine(X-30,(j+1)*cell_height+Y,X,(j+1)*cell_height+Y,SColor(255,0,0,0));

                                if(j<_H-1)
                                {
                                    clear(_st);
                                    sprintf(_st,"%d",j+1);
                                    if(cells_to_show==-1 || j+1<=cells_to_show) Text_out(_font,X-30,(j+2)*cell_height+Y,_st);
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                set(hide->Caption,">>");
                hide->Draw();
            }

            draw->Draw();
        }

        unsigned GetHeight()
        {
            return _H;
        }

        void SetHeight(int Height)
        {
            _H=Height;
        }

        ~Table()
        {
            for(int i=0;i<_W;i++)
            {
                for(int j=0;j<_H;j++) delete cell[i][j];

                delete [] cell[i];
            }

            delete [] cell;

            _W=_H=0;

            _font=(Font*)0;
            delete [] _Caption;

            delete hide;
            delete draw;
        }

    };

    class CheckBox : public GUI
    {
        public:

        bool isChecked;
        char* Text;
        //int X,Y;

        CheckBox(Font* font,int x0,int y0,char* text,bool checked=false)
        {
            _font=font;

            X=x0;
            Y=y0;

            Text=new char[lng(text)+1];
            copy(text,Text);

            isChecked=checked;

            Width=Text_Width(_font,Text)+20;
            Height=max(13,_font->Height);

            isVisible=true;
        }

        bool tryFocuss()
        {
            const int& _m_x=sMouse.X;
            const int& _m_y=sMouse.Y;

            bool _f;

            if(isLeftPressed)
            {
                _f=_m_x>=X && _m_x<=X+Text_Width(_font,Text)+20 && _m_y>=Y && _m_y<=Y+10;

                if(_f) isChecked=!isChecked;
            }

            return _f;
        }

        int GetHeight()
        {
            return Height;
        }

        void KeyPress()
        {

        }

        void Draw()
        {
            if(isVisible)
            {
                _drawRect(X+2,Y+2,X+12,Y+12,SColor(230,255,255,255));

                _drawLine(X+1,Y+1,X+1,Y+12,SColor(200,120,120,120));
                _drawLine(X+1,Y+1,X+12,Y+1,SColor(200,120,120,120));

                _drawLine(X,Y,X+12,Y,SColor(255,117,119,126));
                _drawLine(X,Y,X,Y+12,SColor(255,145,146,152));

                _drawLine(X+12,Y+1,X+12,Y+12,SColor(255,180,183,191));
                _drawLine(X+1,Y+12,X+11,Y+12,SColor(255,188,191,203));

                Text_out(_font,X+20,Y-1,Text);

                if(isChecked)
                {
                    _drawLine(X+3,Y+5,X+3,Y+8,SColor(255,0,0,0));
                    _drawLine(X+4,Y+6,X+4,Y+9,SColor(255,0,0,0));
                    _drawLine(X+5,Y+7,X+5,Y+10,SColor(255,0,0,0));

                    _drawLine(X+6,Y+6,X+6,Y+9,SColor(255,0,0,0));
                    _drawLine(X+7,Y+5,X+7,Y+8,SColor(255,0,0,0));
                    _drawLine(X+8,Y+4,X+8,Y+7,SColor(255,0,0,0));
                    _drawLine(X+9,Y+3,X+9,Y+6,SColor(255,0,0,0));
                }
            }
        }

        ~CheckBox()
        {
            _font=(Font*)0;

            delete [] Text;
        }
    };

    class Slider : public GUI
    {
        private:

        bool _once;
        Font* _txfont;

        public:

        static const int _dh=10;
        static const int _dw=4;

        int _x;
        double _k;

        char* Text;
        int dx;
        //int X,Y;
        //int Width;

        Slider(Font* font,int x,int y,int width,double k,char* text=0,int Space=5,Font* TxFont=0)
        {
            X=x;
            Y=y;

            _font=font;

            _k=k;

            _x=0;

            _once=true;

            Width=width;
            Height=2*_dh;

            isVisible=true;

            dx=0;

            if(text)
            {
                Text=new char[lng(text)+1];
                copy(text,Text);

                if(TxFont) _txfont=TxFont;
                else _txfont=_font;

                dx=Space + Text_Width(_txfont,Text);
            }

            //X+=dx;
            Width+=dx;
        }

        double GetValue()
        {
            return _x*_k;
        }

        int GetHeight()
        {
            return Height;
        }

        void SetValue(double a)
        {
            _x = round(a/_k);
        }

        bool tryFocuss()
        {
            return isLeftPressed && (sMouse.X>=X+dx+_x-_dw && sMouse.X<=X+dx+_x+_dw && sMouse.Y>=Y && sMouse.Y<=Y+2*_dh);
        }

        void KeyPress()
        {

        }

        void Draw()
        {
            static int _tx;

            static char _s[128];

            if(!isVisible) return;

            if(isLeftPressed)
            {
                if(_once && sMouse.X>=X+dx+_x-_dw && sMouse.X<=X+dx+_x+_dw && sMouse.Y>=Y && sMouse.Y<=Y+2*_dh)
                {
                    _tx=sMouse.X-_x;

                    _once=false;
                }

                if(!_once)
                {
                    _x=sMouse.X-_tx;

                    if(_x<0) _x=0;
                    else if(_x>Width-dx) _x=Width-dx;

                    sprintf(_s,"%g",_x*_k);
                    Text_out(_font,X+dx+_x+_dw+2,Y,_s);
                }
            }
            else _once=true;

            _drawLine(X+dx,Y+_dh,X+Width,Y+_dh,SColor(200,50,75,100));
            _drawLine(X+dx,Y+_dh+1,X+Width,Y+_dh+1,SColor(200,130,140,170));
            _drawLine(X+dx,Y+_dh+2,X+Width,Y+_dh+2,SColor(200,140,165,190));

            _drawRect(X+dx+_x-_dw+1,Y+1,X+dx+_x+_dw-1,Y+2*_dh-1,SColor(200,225,232,255));

            _drawLine(X+dx+_x-_dw,Y+1,X+dx+_x-_dw,Y+2*_dh-1,SColor(100,0,0,100));
            _drawLine(X+dx+_x-_dw+1,Y+1,X+dx+_x-_dw+1,Y+2*_dh-1,SColor(50,0,100,100));

            _drawLine(X+dx+_x+_dw,Y+1,X+dx+_x+_dw,Y+2*_dh-1,SColor(100,0,0,100));
            _drawLine(X+dx+_x+_dw-1,Y+1,X+dx+_x+_dw-1,Y+2*_dh-1,SColor(40,0,70,100));

            _drawLine(X+dx+_x-_dw+1,Y,X+dx+_x+_dw-1,Y,SColor(100,0,0,100));
            _drawLine(X+dx+_x-_dw+1,Y+2*_dh,X+dx+_x+_dw-1,Y+2*_dh,SColor(100,0,0,100));

            if(Text) Text_out(_txfont,X,Y+2,Text);
        }

        ~Slider()
        {
            _font=(Font*)0;
        }
    };

    class Dialog : public GUI
    {
        private:

        short _type;

        public:

        //int X,Y;
        //int Width,Height;
        char* Text;
        Button* Done;
        Button* Cancel;
        Edit* edit;
        Image* img;
        SColor color;
        //bool isVisible;

        Dialog(Font* font,char* message,short type=ALERT,SColor color=SColor(255,235,240,250))
        {
            isVisible=true;

            _font=font;

            this->color=color;

            if(type==ALERT || type==CONFIRM || type==MESSAGE) _type=type;
            else _type=ALERT;

            Text=new char[lng(message)];
            set(Text,message);

            Width=Text_Width(font,message)+20;
            Height=_font->Height+20;

            img=new Image();
            edit=new Edit();

            if(_type!=MESSAGE) Load_Image("alert.tga",img);

            Width+=img->Width;
            Height+=img->Height;

            if(_type==MESSAGE)
            {
                Done=new Button();
                Cancel=new Button();
            }
            else if(_type==ALERT)
            {
                Done=new Button(font,0,0,70,20,"OK");
                Cancel=new Button();
            }
            else if(_type==CONFIRM)
            {
                Done=new Button(font,0,0,70,20,"OK");
                Cancel=new Button(font,0,0,70,20,"Отмена");
            }
        }

        Dialog(Font* font,int x,int y,int width,int height,char* text,Button* done,Button* cancel,Edit* edit,SColor color=SColor(255,235,240,250))
        {
            isVisible=true;

            X=x;
            Y=y;

            _font=font;

            Width=width;
            Height=height;

            this->edit=edit;
            this->color=color;

            Text=new char[lng(text)];

            set(Text,text);

            Done=done;
            Cancel=cancel;

            this->edit->X+=x;
            this->edit->Y+=y;

            Done->X+=x;
            Done->Y+=y;

            Cancel->X+=x;
            Cancel->Y+=y;

            _type=PROMT;
        }

        bool tryFocuss()
        {
            return false;
        }

        void KeyPress()
        {

        }

        void Draw()
        {
            if(isVisible)
            {
                if(_type==ALERT || _type==CONFIRM || _type==MESSAGE)
                {
                    X=getWidth()/2-Width/2;
                    Y=getHeight()/2-Height;

                    if(_type==ALERT)
                    {
                        Done->X=X+Width-85;
                        Done->Y=Y+Height-22;
                    }
                    else if(_type==CONFIRM)
                    {
                        Done->X=X+Width-150;
                        Done->Y=Y+Height-22;

                        Cancel->X=X+Width-80;
                        Cancel->Y=Y+Height-22;
                    }
                }

                _drawRect(X,Y,X+Width,Y+Height,color);

                _drawLine(X,Y,X+Width,Y,SColor(150,0,0,50));
                _drawLine(X,Y+Height,X+Width,Y+Height,SColor(100,20,20,100));
                _drawLine(X,Y,X,Y+Height,SColor(200,0,0,50));
                _drawLine(X+Width,Y,X+Width,Y+Height,SColor(150,20,20,70));

                if(_type!=MESSAGE)
                {
                    if(Done->tryFocuss() && _type==ALERT && isLeftPressed) isVisible=false;
                    Done->Draw();
                }
                else Text_out(_font,X+10,Y+10+_font->Height/2,Text);

                if(_type==PROMT)
                {
                    Text_out(_font,X+10,Y+10+_font->Height/2,Text);
                    Cancel->Draw();
                    edit->Draw();
                }
                else if(_type==ALERT)
                {
                    Text_out(_font,X+img->Width+10,Y+img->Height/2+_font->Height,Text);
                    _DrawImage(X+5,Y+5,img);
                }
                else if(_type==CONFIRM)
                {
                    Text_out(_font,X+img->Width+10,Y+img->Height/2+_font->Height,Text);
                    _DrawImage(X+5,Y+5,img);
                    Cancel->Draw();
                }
            }
        }

        ~Dialog()
        {
            _font=(Font*)0;
            delete edit;
            delete img;

            delete Done;
            delete Cancel;

            delete [] Text;
        }
    };

    class Scroll : public GUI
    {
        friend class FileDialog;

        private:

        unsigned _n,_max_width;

        public:

        bool _checked;
        //int X,Y;
        int _id;
        sElement* List;

        Scroll()
        {

        }

        Scroll(Font* font,int x,int y...)
        {
            _font=font;

            isVisible=true;

            X=x;
            Y=y;

            _n=0;

            _checked=false;

            _max_width=0;
            _id=0;

            va_list lst;
            va_start(lst,y);

            while(1)
            {
                char* p=va_arg(lst,char*);

                if(p!=0) if(Text_Width(_font,p)>_max_width) _max_width=Text_Width(_font,p,'|');

                if(p==0) break;
                _n++;
            }

            _max_width+=10;

            Width=_max_width+10;
            Height=_font->Height+4;

            va_end(lst);

            List=new sElement[_n];

            va_start(lst,y);

            for(int b=0;b<_n;b++)
            {
                char* p=va_arg(lst,char*);

                List[b].name=new char[lng(p)+1];
                clear(List[b].name);
                set(List[b].name,p,'|');

                List[b].extention=new char[lng(p)-lng(List[b].name)+1];
                overwrite(p,List[b].extention,lng(List[b].name)+2);
            }

            va_end(lst);
        }

        bool CursorOn()
        {
            if(!_checked) return sMouse.X>X && sMouse.X<X+_max_width+10 && sMouse.Y>5+Y && sMouse.Y<5+Y+(_font->Height);
            else return sMouse.X>X && sMouse.X<X+_max_width+10 && sMouse.Y>5+Y && sMouse.Y<5+Y+(_font->Height)*(_n+1);
        }

        bool tryFocuss()
        {
            isFocussed=CursorOn();

            return isLeftPressed && isFocussed;
        }

        void KeyPress()
        {

        }

        int GetWidth()
        {
            return Width;
        }

        int GetHeight()
        {
            return Height;
        }

        void Draw()
        {
            static bool sPressed=true;

            static bool once=true;
            unsigned char dh=_font->Height/2+2;

            const int& _m_x=sMouse.X;
            const int& _m_y=sMouse.Y;

            if(!isVisible) return;

            _drawRect(X,Y+1,X+_max_width+10,Y+_font->Height/2+3,SColor(255,233,236,237));
            _drawRect(X,Y+_font->Height/2+3,X+_max_width+10,Y+_font->Height+4,SColor(255,217,222,225));

            _drawLine(X,Y,X+_max_width+9,Y,SColor(255,105,115,120));
            _drawLine(X,Y+_font->Height+5,X+_max_width+9,Y+_font->Height+5,SColor(255,250,250,250));
            _drawLine(X,Y+_font->Height+6,X+_max_width+9,Y+_font->Height+6,SColor(255,105,115,120));

            _drawLine(X-1,Y+1,X-1,Y+_font->Height+6,SColor(255,105,115,120));
            _drawLine(X+_max_width+9,Y+1,X+_max_width+9,Y+_font->Height+6,SColor(255,105,115,120));

            if(isLeftPressed && sPressed)
            {
                //_m_x=sMouse.X;
                //_m_y=sMouse.Y;

                if(once && _m_x>X && _m_x<X+_max_width+10 && _m_y>5+Y && _m_y<5+Y+(_font->Height))
                {
                    _checked=!_checked;
                    //sPressed=false;
                    isLeftPressed=false;
                    once=false;
                }
            }
            else once=true;

            if(_checked)
            {
                //if(!once) sPressed=false;

                _drawRect(X,Y+_font->Height+7,X+_max_width+9,5+Y+(_font->Height)*(_n+1),SColor(255,255,255,255));

                _drawLine(X,5+Y+(_font->Height)*(_n+1),X+_max_width+9,5+Y+(_font->Height)*(_n+1),SColor(200,150,150,155));
                _drawLine(X-1,Y+_font->Height+5,X-1,5+Y+(_font->Height)*(_n+1),SColor(255,150,150,155));
                _drawLine(X+_max_width+9,Y+_font->Height+5,X+_max_width+9,5+Y+(_font->Height)*(_n+1),SColor(150,150,150,155));

                for(int b=1;b<_n+1;b++) Text_out(_font,2+X,5+Y+(_font->Height)*b,List[b-1].name);

                if(isLeftPressed && sPressed)
                {
                    if(_m_x>X && _m_x<X+_max_width+10 && _m_y>5+Y && _m_y<5+Y+(_font->Height)*(_n+1))
                    {
                        _id=(_m_y-Y-5)/(_font->Height);
                        _id--;

                        if(_id<0 || _id>=_n) _id=0;
                    }

                    _checked=false;
                    isLeftPressed=false;
                }
            }
            else Text_out(_font,2+X,Y+4,List[_id].name);

            _drawLine(X+_max_width+1,Y+dh-1,X+_max_width+8,Y+dh-1,SColor(255,0,0,0));
            _drawLine(X+_max_width+2,Y+dh,X+_max_width+7,Y+dh,SColor(255,0,0,0));
            _drawLine(X+_max_width+3,Y+dh+1,X+_max_width+6,Y+dh+1,SColor(255,0,0,0));
            _drawLine(X+_max_width+4,Y+dh+2,X+_max_width+5,Y+dh+2,SColor(255,0,0,0));
        }

        char* CurrentName()
        {
            return List[_id].name;
        }

        char* CurrentExtention()
        {
            return List[_id].extention;
        }

        int GetId()
        {
            return _id;
        }

        ~Scroll()
        {
            //for(int b=0;b<_n;b++) delete List[b];

            delete [] List;

            _n=0;

            _font=(Font*)0;
        }
    };

    class FileDialog : public GUI
    {
        private:

        //io::IFileSystem* _fs;
        //io::IFileList* _list;

        struct Element
        {
            char* name;
            char* ext;
            int attr;

            Element()
            {
                name=(char*)0;
                ext=(char*)0;
            }

            Element(char* Name,char* Ext,int Attr)
            {
                attr=Attr;

                name=new char[lng(Name)+1];
                copy(Name,name);

                ext=new char[lng(Ext)+1];
                copy(Ext,ext);
            }

            ~Element()
            {
                delete [] name;
                delete [] ext;
                attr=0;
            }

            //int _attr;
        };

        Element* List;
        //Element* _List;
        Image* _folder;
        Image* _file;
        Image* _sfolder;
        Image* _nfolder;
        Edit* _t_edit;
        int _n;
        unsigned short _type;
        char t_path[1024];

        inline void _GetExtention(char* s,char* &res)
        {
            char* _s=s;

            while(*s) s++;
            while(*s!='.' && s!=_s) s--;

            if(s==_s) res=(char*)0;
            else
            {
                res=new char[lng(s+1)+1];
                copy(s+1,res);
            }
        }

        inline bool _eql(char* a,char* b)
        {
            if(lng(a)!=lng(b)) return false;
            else while(*a) if(*a++!=*b++) return false;
            return true;
        }

        void inline _ReloadDir(char* path)
        {
            //_fs->changeWorkingDirectoryTo(path);
            add(path,'*');

            HANDLE          hFile;
            WIN32_FIND_DATA FileInformation;

            hFile = FindFirstFile(path, &FileInformation);

            if(hFile==INVALID_HANDLE_VALUE)
            {
                FindClose(hFile);
                reduce(path);
                reduce(path);
                t_reduce(path,'\\');
                add(path,'*');
                hFile=FindFirstFile(path, &FileInformation);
            }

            _n=0;

            do
            {
                _n++;

            }while(FindNextFile(hFile, &FileInformation));

            FindClose(hFile);

            delete [] List;

            List=new Element[_n];

            _n=0;

            hFile = FindFirstFile(path, &FileInformation);

            do
            {
                if(FileInformation.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                {
                    List[_n].attr=1;

                    //if(!List[_n].name)
                    List[_n].name=new char[lng(FileInformation.cFileName)+1];

                    copy(FileInformation.cFileName,List[_n].name);

                    _n++;
                }

            }while(FindNextFile(hFile, &FileInformation));

            FindClose(hFile);
            hFile = FindFirstFile(path, &FileInformation);

            do
            {
                if(!(FileInformation.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
                {
                    List[_n].attr=0;

                    //if(!List[_n].name)
                    List[_n].name=new char[lng(FileInformation.cFileName)+1];

                    copy(FileInformation.cFileName,List[_n].name);
                    _GetExtention(FileInformation.cFileName,List[_n].ext);

                    _n++;
                }

            }while(FindNextFile(hFile, &FileInformation));

            FindClose(hFile);

            int _k=0;
            Element _nt;

            if(List[0].name[0]!='.')
            {
                while(_k<_n && List[_k].name[0]!='.') _k++;

                if(_k<_n-1)
                {
                    while(_k>0)
                    {
                        _nt=List[_k-1];
                        List[_k-1]=Element(".","",1);
                        List[_k]=Element("..","",1);
                        List[_k+1]=_nt;
                        _k--;
                    }
                }
            }

            reduce(path);
        }

        public:
        //int X,Y;
        //int Width,Height;
        Button* Open;
        Button* Cancel;
        Button* New;
        Edit* edit;
        Dialog* dWarning;
        Scroll* extention;
        bool ist;
        char* path;
        //bool isVisible;

        FileDialog(Font* font,unsigned short type,int x,int y,const char* root...)
        {
            /*_fs=Device->getFileSystem();

            char _tp[255];
            set(_tp,(char*)_fs->getWorkingDirectory().c_str());*/

            X=x;
            Y=y;

            _font=font;
            ist=false;
            isVisible=true;
            _type=type;

            _t_edit=0;//new Edit();

            int _l=lng((char*)root);
            path=new char[max(1024,_l+256)+2];
            //clear(path);
            copy(root,path);
            //set(path,start_path);

            Width=655;
            Height=400;

            //List=new Element[10];

            _folder=new Image();
            _file=new Image();
            _nfolder=new Image();

            if(path[_l-1]!='\\') add(path,'\\');
            s_add(path,"_Folder.tga");
            Load_Image(path,_folder);

            t_reduce(path,'\\');
            s_add(path,"_File.tga");
            Load_Image(path,_file);

            t_reduce(path,'\\');
            s_add(path,"_nFolder.tga");
            Load_Image(path,_nfolder);

            t_reduce(path,'\\');

            if(type==CREATE_FILE)
            {
                dWarning=new Dialog(font,"Файл с таким именем уже существует. Заменить?",CONFIRM);

                Open=new Button(font,x+Width-203,y+Height-24,90,23,"Создать");
            }
            else if(type==OPEN_FILE)
            {
                Open=new Button(font,x+Width-203,y+Height-24,90,23,"Открыть");
            }

            extention=new Scroll();
            extention->isVisible=true;

            ///________________

            extention->_font=font;

            extention->_checked=false;

            extention->_max_width=0;
            extention->_id=0;
            extention->_n=0;

            va_list lst;
            va_start(lst,root);

            while(1)
            {
                char* p=va_arg(lst,char*);

                if(p!=0) if(Text_Width(_font,p)>extention->_max_width) extention->_max_width=Text_Width(_font,p,'|');

                if(p==0) break;
                extention->_n++;
            }

            extention->_max_width+=20;

            va_end(lst);

            extention->List=new sElement[extention->_n];

            va_start(lst,root);

            for(int b=0;b<extention->_n;b++)
            {
                char* p=va_arg(lst,char*);

                extention->List[b].name=new char[lng(p)];
                clear(extention->List[b].name);
                set(extention->List[b].name,p,'|');

                extention->List[b].extention=new char[lng(p)-lng(extention->List[b].name)];
                extention->List[b].extention[lng(p)-lng(extention->List[b].name)-1]=0;
                overwrite(p,extention->List[b].extention,lng(extention->List[b].name)+2);
            }

            va_end(lst);

            ///________________

            extention->X=x+Width-extention->_max_width-30;
            extention->Y=y+Height-45;

            extention->Height=_font->Height+6;

            Cancel=new Button(font,x+Width-111,y+Height-24,90,23,"Отмена");

            New=new Button(font,x+Width-235,y+Height-24,30,23,"");

            edit=new Edit(font,x+15,y+Height-45,Width-extention->_max_width-47,max(font->Height+4,extention->Height));

            List=(Element*)0;

            _ReloadDir(path);

            //_fs->changeWorkingDirectoryTo(_tp);
        }

        void SetPath(char* Path)
        {
            if(Path)
            {
                int l=lng(Path);

                if(l)
                {
                    if(lng(path)<l)
                    {
                        delete [] path;
                        path=new char[l+256];
                    }

                    copy(Path,path);
                    _ReloadDir(path);
                }
            }
        }

        bool tryFocuss()
        {
            isFocussed=(MouseX>=X && MouseX<=X+Width && MouseY>=Y-_font->Height+5 && MouseY<=Y);

            return isFocussed;
        }

        void KeyPress()
        {
            unsigned char& _Char = sKey.Char;

            if(isVisible)
            {
                if(_Char!=17 && _Char!=16) edit->KeyPress();
            }
        }

        void Hide()
        {
            isVisible=false;
        }

        void Show()
        {
            t_reduce(path,'\\');
            isVisible=true;
        }

        void Draw()
        {
            const bool Static=false;

            static int _offset=0;
            static int t_x,t_y,o_tx,o_ty,c_tx,c_ty,e_tx,e_ty,s_tx,s_ty,n_tx,n_ty;
            static int b=0,_p=0,t_p,t_id=0,ft_id=-1;
            static bool _once=true;
            static bool once=true;
            static bool s_once=true;
            static bool _p_once=true;
            static bool _sp_once=true;
            static bool n_once=true;
            static bool _n_once=true;
            static bool _e_once=true;
            static bool d_visible=false;

            static bool _r_once=false;
            FILE* _tf;
            Element* _List;
            //static int _lng=0;

            const int dh=_font->Height+5;
            const int _uR=_font->Height+5;

            int _K=0;

            static char __t[2]={133,0};

            if(isVisible)
            {
                bool &Pressed=isLeftPressed;
                bool &Db_click=isDoubleClicked;

                int _n_max=(Height-45)/(_font->Height+4)-1;
                const int _h=Height-105;

                const int _m_x=sMouse.X;
                const int _m_y=sMouse.Y;

                if(_r_once)
                {
                    //if(_lng>0) path[_lng]=0;
                    _ReloadDir(path);
                    _r_once=ist=false;
                }

                if(!extention->_checked && !_e_once) _e_once=false;
                else _e_once=!extention->_checked;

                //if(d_visible) dWarning->Draw(Pressed);

                if(isLeftPressed && _p_once)
                {
                    if(d_visible)
                    {
                        if(dWarning->Done->tryFocuss()) goto cRewrite;
                        if(dWarning->Cancel->tryFocuss())
                        {
                            d_visible=false;
                            t_reduce(path,'\\');
                            //path[_lng]=0;
                        }
                    }

                    if(Open->tryFocuss() && _e_once)
                    {
                        switch(_type)
                        {
                            case OPEN_FILE:

                                cEdit:

                                if(ft_id!=-1)
                                {
                                    t_id=ft_id;
                                    goto Reload_dir;
                                }
                                else if(edit->Text[0])
                                {
                                    //_lng=lng(path);
                                    _r_once=true;

                                    s_add(path,edit->Text);
                                    add(path,'\\');
                                    edit->Text[0]=0;

                                    //goto Reload_dir;
                                }
                                break;
                            case CREATE_FILE:
                                if(!d_visible && lng(edit->Text)!=0)
                                {
                                    //_lng=lng(path);

                                    //if(isVisible)
                                    //{
                                        s_add(path,edit->Text);
                                        if(extention->CurrentExtention()[0])
                                        {
                                            add(path,'.');
                                            s_add(path,extention->CurrentExtention());
                                        }
                                    //}

                                    _tf=fopen(path,"r");

                                    //printf("%d\n",_tf);

                                    if(_tf)
                                    {
                                        d_visible=true;
                                        isLeftPressed=false;
                                        fclose(_tf);
                                    }
                                    else
                                    {
                                        fclose(_tf);

                                        cRewrite:

                                        d_visible=false;

                                        ///_tf=fopen(path,"w");
                                        ///fclose(_tf);

                                        //path[_lng]=0;
                                        _r_once=true;
                                        ///_ReloadDir(path);

                                        isVisible=false;
                                        ist=true;
                                        _offset=_p=0;

                                        return;
                                    }

                                    break;
                                }
                            default:
                                break;
                        }
                    }

                    if(Cancel->tryFocuss() && _e_once)
                    {
                        isVisible=false;

                        if(_t_edit) _t_edit->isVisible=false;

                        _r_once=true;
                        //_lng=-1;
                        _offset=_p=0;
                        return;
                    }

                    if(New->tryFocuss() && n_once && _e_once)
                    {
                        if(_t_edit)
                        {
                            //_t_edit->~Edit();
                            delete _t_edit;
                        }

                        _t_edit=new Edit(_font,X+40,Y+dh*(min(b,_n_max)-2)+_font->Height+6,Width/3,_font->Height+2);
                        set(_t_edit->Text,"New folder");
                        _t_edit->isFocussed=true;
                        _t_edit->Selected=true;

                        n_once=false;
                    }

                    if(!Static)
                    {
                        if(_m_x>=X && _m_x<=X+Width && _m_y>=Y-_uR && _m_y<=Y)
                        {
                            if(once)
                            {
                                t_x=_m_x-X;
                                t_y=_m_y-Y;

                                o_tx=_m_x-Open->X;
                                o_ty=_m_y-Open->Y;

                                c_tx=_m_x-Cancel->X;
                                c_ty=_m_y-Cancel->Y;

                                n_tx=_m_x-New->X;
                                n_ty=_m_y-New->Y;

                                e_tx=_m_x-edit->X;
                                e_ty=_m_y-edit->Y;

                                s_tx=_m_x-extention->X;
                                s_ty=_m_y-extention->Y;

                                once=false;
                            }
                        }
                    }
                }
                else
                {
                    _once=once=s_once=n_once=true;
                    if(!extention->_checked && !_e_once) _e_once=true;
                }

                if(!Static && !once && _p_once)
                {
                    X=_m_x-t_x;
                    Y=_m_y-t_y;

                    Open->X=_m_x-o_tx;
                    Open->Y=_m_y-o_ty;

                    Cancel->X=_m_x-c_tx;
                    Cancel->Y=_m_y-c_ty;

                    New->X=_m_x-n_tx;
                    New->Y=_m_y-n_ty;

                    edit->X=_m_x-e_tx;
                    edit->Y=_m_y-e_ty;

                    extention->X=_m_x-s_tx;
                    extention->Y=_m_y-s_ty;
                }

                ///_________________________________________________________

                _drawRect(X,Y-_uR,X+Width,Y,SColor(240,220,225,245));

                /*_drawRect(X+2,Y-_uR,X+Width-1,Y-10,SColor(255,235,240,255));
                _drawRect(X+2,Y-_uR+10,X+Width-1,Y-7,SColor(200,235,240,255));
                _drawRect(X+2,Y-_uR+13,X+Width-1,Y-5,SColor(155,235,240,255));
                _drawRect(X+2,Y-_uR+15,X+Width-1,Y,SColor(75,235,240,255));*/

                _drawRect(X,Y,X+Width,Y+Height,SColor(150,220,225,230));
                _drawRect(X+15,Y,X+Width-20,Y+Height-45,SColor(255,255,255,255));

                _drawLine(X,Y-_uR,X,Y+Height,SColor(255,50,50,50));
                _drawLine(X,Y-_uR,X+Width,Y-_uR,SColor(255,50,50,50));

                _drawLine(X+Width,Y-_uR+1,X+Width,Y+Height,SColor(255,120,120,120));
                _drawLine(X+Width,Y+Height,X,Y+Height,SColor(255,120,120,120));

                if(b>=_n_max)
                {
                    _drawLine(X+Width-27,Y+2,X+Width-28,Y+2,SColor(255,10,15,20));                   ///   Â¤
                    _drawLine(X+Width-26,Y+3,X+Width-29,Y+3,SColor(255,10,15,20));                   ///  Â¤Â¤Â¤
                    _drawLine(X+Width-25,Y+4,X+Width-30,Y+4,SColor(255,10,15,20));                   /// Â¤Â¤Â¤Â¤Â¤
                    _drawLine(X+Width-24,Y+5,X+Width-31,Y+5,SColor(255,10,15,20));                   ///Â¤Â¤Â¤Â¤Â¤Â¤Â¤

                    _drawLine(X+Width-24,Y+Height-52,X+Width-31,Y+Height-52,SColor(255,10,15,20));   ///Â¤Â¤Â¤Â¤Â¤Â¤Â¤
                    _drawLine(X+Width-25,Y+Height-51,X+Width-30,Y+Height-51,SColor(255,10,15,20));   /// Â¤Â¤Â¤Â¤Â¤
                    _drawLine(X+Width-26,Y+Height-50,X+Width-29,Y+Height-50,SColor(255,10,15,20));   ///  Â¤Â¤Â¤
                    _drawLine(X+Width-27,Y+Height-49,X+Width-28,Y+Height-49,SColor(255,10,15,20));   ///   Â¤

                    _drawLine(X+Width-35,Y,X+Width-36,Y+Height-45,SColor(150,0,100,255));
                    _drawLine(X+Width-36,Y,X+Width-35,Y+Height-45,SColor(150,255,100,0));

                    //printf("%d\n",sMouse.Wheel);

                    if(Pressed || sMouse.Wheel!=0)
                    {
                        if(Pressed)
                        {
                            if(_m_x>=X+Width-33 && _m_x<=X+Width-20)
                            {
                                if(_m_y>=Y+10+_p && _m_y<=Y+50+_p && _p_once)
                                {
                                    t_p=_m_y-_p;
                                    _p_once=false;
                                }
                                else if(_m_y<=Y+Height-47 && _m_y>=Y+Height-54 && _sp_once && b>_n_max)
                                {
                                    _offset++;
                                    _p+=_h/(b+_offset-_n_max-1);
                                    if(_p>_h) _p=_h;
                                    _sp_once=false;
                                }
                                else if(_m_y<=Y+7 && _m_y>=Y && _sp_once && _offset>0)
                                {
                                    _offset--;
                                    _p-=_h/(b+_offset-_n_max+1);
                                    if(_p<0) _p=0;
                                    _sp_once=false;
                                }
                            }

                            if(!_p_once && _m_y-t_p>=0 && _m_y-t_p<=Height-105 && _sp_once)
                            {
                                _p=_m_y-t_p;
                                _offset=_p*(b+_offset-_n_max)/_h;
                            }
                        }
                        else if(sMouse.Wheel!=0)
                        {
                            if(b==_n_max) _p=_h;

                            if(sMouse.Wheel==-1 && b>_n_max)
                            {
                                _offset++;
                                _p+=_h/(b+_offset-_n_max-1);

                                if(_p>_h) _p=_h;
                            }
                            else if(sMouse.Wheel==1 && _offset>0)
                            {
                                _offset--;
                                _p-=_h/(b+_offset-_n_max+1);

                                if(_p<0) _p=0;
                            }

                            sMouse.Wheel=0;
                        }
                    }
                    else _p_once=_sp_once=true;

                    _drawLine(X+Width-33,Y+10+_p,X+Width-33,Y+50+_p,SColor(255,183,198,208));       ///Â¤Â¤Â¤Â¤Â¤Â¤Â¤
                    _drawLine(X+Width-32,Y+10+_p,X+Width-32,Y+50+_p,SColor(255,191,208,218));       ///Â¤Â¤Â¤Â¤Â¤Â¤Â¤
                    _drawLine(X+Width-31,Y+10+_p,X+Width-31,Y+50+_p,SColor(255,197,214,224));       ///Â¤Â¤Â¤Â¤Â¤Â¤Â¤
                    _drawLine(X+Width-30,Y+10+_p,X+Width-30,Y+50+_p,SColor(255,220,240,250));       ///Â¤Â¤Â¤Â¤Â¤Â¤Â¤
                    _drawLine(X+Width-29,Y+10+_p,X+Width-29,Y+50+_p,SColor(255,220,240,250));       ///Â¤Â¤Â¤Â¤Â¤Â¤Â¤
                    _drawLine(X+Width-28,Y+10+_p,X+Width-28,Y+50+_p,SColor(255,197,214,224));       ///Â¤Â¤Â¤Â¤Â¤Â¤Â¤
                    _drawLine(X+Width-27,Y+10+_p,X+Width-27,Y+50+_p,SColor(255,197,214,224));       ///Â¤Â¤Â¤Â¤Â¤Â¤Â¤
                    _drawRect(X+Width-26,Y+10+_p,X+Width-22,Y+50+_p,SColor(255,191,208,218));       ///Â¤Â¤Â¤Â¤Â¤Â¤Â¤
                    _drawLine(X+Width-21,Y+10+_p,X+Width-21,Y+50+_p,SColor(255,183,198,208));       ///Â¤Â¤Â¤Â¤Â¤Â¤Â¤
                    _drawLine(X+Width-20,Y+10+_p,X+Width-20,Y+50+_p,SColor(255,183,198,208));       ///Â¤Â¤Â¤Â¤Â¤Â¤Â¤
                }

                Open->Draw();
                Cancel->Draw();
                New->Draw();
                _DrawImage(X+Width-228,Y+Height-18,_nfolder);

                if(_t_edit && Pressed && n_once && _t_edit->isVisible) _t_edit->tryFocuss();

                if(_t_edit && _t_edit->isVisible && _t_edit->isFocussed)
                {
                    if(sKey.Char==10 || sKey.Char==13)
                    {
                            if(_t_edit->Text=="") set(_t_edit->Text,"New folder");

                            t_path[0]=0;
                            set(t_path,path);
                            s_add(t_path,_t_edit->Text);
                            mkdir(t_path);

                            _ReloadDir(path);

                            _t_edit->isVisible=false;
                            _t_edit->isFocussed=false;
                            sKey.Char=0;
                    }
                    else _t_edit->KeyPress();
                }

                if(Pressed) edit->tryFocuss();

                if(sKey.Char==13 && _type==OPEN_FILE)
                {
                    sKey.Char=0;
                    goto cEdit;
                }

                ///if(sKey.Char!=17 && sKey.Char!=16) edit->KeyPress(sKey.Char);

                edit->Draw();

                extention->Draw();      ///_p_once

                ///_____________________________________

                //if(_List) delete [] _List;
                _List=new Element[_n];
                _K=0;

                for(int k=1;k<_n;k++)
                {
                    if(List[k].attr==0)
                    {
                        if(List[k].ext) if(_eql(List[k].ext,extention->CurrentExtention()) || extention->CurrentExtention()[0]==0) _List[_K++]=List[k];
                    }
                    else _List[_K++]=List[k];
                }

                ///_______________________________________

                if(_m_x>X+10 && _m_x<X+Width-40 && _m_y>Y && _m_y<Y+Height-45 && _p_once)
                {
                    t_id=_offset+(_m_y-Y-_font->Height+12)/dh;

                    if(Pressed && s_once)
                    {
                        ft_id=t_id;
                        s_once=false;
                    }
                }
                else if(Pressed) ft_id=-1;

                if(_once && Db_click && _p_once && !d_visible)
                {
                    if(t_id-_offset<_n_max && _m_y>=Y && _m_y<Y+Height-45 && _m_x>=X && _m_x<=X+Width-40 && t_id-_offset<b)
                    {
                        Reload_dir:

                        if(_eql(_List[t_id].name,".."))
                        {
                            reduce(path);
                            while(path[lng(path)-1]!='\\') reduce(path);
                        }
                        else if(_List[t_id].name[0]!='.' && _List[t_id].attr!=-1)
                        {
                            if(_type==OPEN_FILE || (_type==CREATE_FILE && _List[t_id].attr!=0))
                            {
                                //if(!_r_once) _lng=lng(path);
                                s_add(path,_List[t_id].name);
                                add(path,'\\');
                            }
                        }

                        if(_List[t_id].attr==0 && _type!=CREATE_FILE)
                        {
                            reduce(path);
                            _r_once=true;
                            isVisible=false;
                            ist=true;
                            _offset=_p=0;
                        }

                        if(_List[t_id].attr!=0) _ReloadDir(path);

                        _offset=_p=0;

                        ft_id=-1;

                        _once=false;
                    }
                    Db_click=false;
                }

                //cout << t_id << ' ' << _n_max << " | " << b << endl;

                b=0;

                for(int k=0;k<_K-_offset;k++)
                {
                    //if(16*(b-1)+_font->Height>=Height-60) break;

                    if(_List[k+_offset].attr==1)
                    {
                        if(dh*(b)+0*_font->Height<Height-60)
                        {
                            _DrawImage(X+20,Y+dh*(b-1)+_font->Height+4,_folder);

                            Text_out(_font,X+40,Y+dh*b+0*_font->Height+2,_List[k+_offset].name,Color{0,0,0},Width-80,false,ENC_ANSI);

                            //Text_out(_font,X+300,Y+16*b+_font->Height+2,IntToStr(List[k+_offset]._attr));
                        }

                        b++;
                    }
                    else if(_List[k+_offset].attr==0)
                    {
                        //if(_eql(List[k+_offset].ext,extention->CurrentExtention()) || extention->CurrentExtention()[0]==0)
                        //{
                            if(dh*(b-1)+_font->Height<Height-60)
                            {
                                _DrawImage(X+22,Y+dh*(b-1)+_font->Height+4,_file);
                                Text_out(_font,X+40,Y+dh*b+0*_font->Height+2,_List[k+_offset].name,Color{0,0,0},Width-80,false,ENC_ANSI);

                                //Text_out(_font,X+300,Y+16*b+_font->Height+2,IntToStr(List[k+_offset]._attr));
                            }

                            b++;
                        //}
                    }
                }

                Text_out(_font,X+5,Y-2-_font->Height,path,Color{0,0,0},Width-4*_font->chr['.'].W,false,ENC_ANSI);

                if(Text_Width(_font,path)>Width-4*_font->chr['.'].W-5)
                Text_out(_font,X+Width-4*_font->chr['.'].W+5,Y-2,__t,Color{0,0,0},-1,false,ENC_ANSI);

                if(ft_id-_offset<_n_max && ft_id!=-1 && dh*(ft_id-1-_offset)+_font->Height+4<Height-60 && dh*(ft_id-1-_offset)+_font->Height+4>=0) _drawRect(X+20,Y+dh*(ft_id-1-_offset)+_font->Height+4,X+Width-40,Y+dh*(ft_id-1-_offset)+2*_font->Height+8,SColor(75,100,170,255));

                if(t_id-_offset<_n_max && _m_x>X+10 && _m_x<X+Width-40 && _m_y>Y && _m_y<Y+Height-45) _drawRect(X+20,Y+dh*((_m_y-Y-_font->Height+12)/dh-1)+_font->Height+4,X+Width-40,Y+dh*((_m_y-Y-_font->Height+12)/dh-1)+2*_font->Height+8,SColor(25,100,170,255));

                if(_t_edit)
                {
                    _t_edit->Draw();
                    if(_t_edit->isVisible) _DrawImage(X+20,Y+dh*(min(b,_n_max)-2)+_font->Height+4,_folder);
                }

                if(d_visible) dWarning->Draw();

                //delete [] _List;
            }
        }

        ~FileDialog()
        {
            _font=(Font*)0;
            delete [] t_path;
            delete [] path;

            delete [] List;

            delete edit;
            delete Open;
            delete Cancel;
            delete extention;
            delete New;
            delete dWarning;

            delete _folder;
            delete _file;
            delete _sfolder;
            delete _nfolder;
            delete _t_edit;
        }
    };

    template<class Initial,class Result>
    struct MatchTable
    {
        struct $chunk
        {
            Initial* data;
            Initial _t;         ///terminator
            int N;
            Result res;
            unsigned char type;

            /// type  |     meaning
            ///-------|---------------
            ///   0   |     empty
            ///   1   |   char array    'a','d','e'     -> [...]
            ///   2   |   word array    "oosd","rtui"   -> [...]
            ///   3   |  char brackets  '#'...'\n'      -> [...]
            ///   4   |  word brackets  "begin"..."end" -> [...]

            $chunk()
            {
                N=0;
                type=0;
                _t=0;
            }

            $chunk(int n,unsigned char Type=1,Initial T=0)
            {
                data=new Initial[N=n];
                type=Type;
                _t=T;
            }

            $chunk(int n,const Initial* Data,Result Res,unsigned char Type=1,Initial T=0)
            {
                data=new Initial[N=n];

                memcpy(data,Data,n*sizeof(Initial));
                res=Res;

                type=Type;
                _t=T;
            }

            ~$chunk()
            {
                for(int b=0;b<N;b++) delete &data[b];
                delete [] data;

                delete &res;

                N=0;
                type=0;
                _t=0;
            }
        };

        int N_MAX;

        $chunk** tData;
        int N;

        MatchTable(int Nmax=1)
        {
            tData=new $chunk*[N_MAX=Nmax];
            N=0;
        }

        void Add(int n,const Initial* i,Result r,unsigned char Type=1)
        {
            const int _Nstep=128;

            int p=-1;

            for(int b=0;b<N;b++)
            {
                if(tData[b]->res==r && tData[b]->type==Type)
                {
                    p=b;
                    break;
                }
            }

            if(p<0)
            {
                if(N==N_MAX)
                {
                    N_MAX+=_Nstep;

                    $chunk** _t=new $chunk*[N_MAX];

                    memcpy(_t,tData,N*sizeof($chunk*));

                    delete [] tData;
                    tData=_t;
                }

                tData[N++]=new $chunk(n,i,r,Type);
            }
            else
            {
                tData[p]->N+=n;
                Initial* _t=new Initial[tData[p]->N];

                memcpy(_t,tData[p]->data,(tData[p]->N-n)*sizeof(Initial));
                memcpy(_t+tData[p]->N-n,i,n*sizeof(Initial));

                delete [] tData[p]->data;
                tData[p]->data=_t;
            }
        }

        void Add(Initial i,Result r,unsigned char Type=1)
        {
            Initial* _i=new Initial[1];
            _i[0]=i;

            this->Add(1,_i,r,Type);

            delete &_i[0];
            delete [] _i;
        }

        Result* GetRes(Initial i)
        {
            for(int b=0;b<N;b++)
            {
                if(tData[b]->type!=1 && tData[b]->type!=3) continue;

                for(int k=0;k<tData[b]->N;k++)
                {
                    if(i==tData[b]->data[k]) return &tData[b]->res;
                }
            }

            return (Result*)0;
        }

        Result* GetRes(Initial* i)
        {
            bool _ist=true;
            int _n=0,k;

            for(int b=0;b<N;b++)
            {
                if(tData[b]->type!=2 && tData[b]->type!=4) continue;
                k=0;
                _ist=true;

                while(k<tData[b]->N)
                {
                    if(i[_n]!=tData[b]->data[k])
                    {
                        _ist=false;
                        while(k<tData[b]->N && tData[b]->data[k]!=tData[b]->_t) k++;

                        if(k==tData[b]->N) break;
                        else
                        {
                            _n=-1;
                            _ist=true;
                        }
                    }

                    _n++;
                    k++;
                }

                if(_ist) return &tData[b]->res;
            }

            return (Result*)0;
        }

        ~MatchTable()
        {
            for(int b=0;b<N_MAX;b++) delete tData[b];
            delete [] tData;

            N_MAX=N=0;
        }
    };

    struct TextBox : public GUI
    {
        static const int _mh=5;
        static const int _mw=1;

        struct Style
        {
            Font* font;
            pixel color;

            Style()
            {
                font=(Font*)0;
                color=pixel(0,0,0,0);
            }

            Style(pixel color)
            {
                font=(Font*)0;
                this->color=color;
            }

            Style(Font* font,pixel color)
            {
                this->font=font;
                this->color=color;
            }

            void Clr()
            {
                font=(Font*)0;
                color=pixel(0,0,0,0);
            }

            void Set(Font* font,pixel color)
            {
                this->font=font;
                this->color=color;
            }

            void Set(pixel color)
            {
                this->color=color;
            }

            bool operator==(Style s)
            {
                return (font==s.font && color==s.color);
            }
        };

        int encoding;
        int _l,_Lmax,_h,_dh,_X;
        //Font* _font;

        char* Text;
        Style* sText;
        MatchTable<char,Style>* mText;

        pixel dColor;
        pixel cColor;
        //int X,Y,Width,Height;
        //bool isFocussed;
        bool Selected;
        //bool isVisible;

        int GetMaxFontWidth()
        {
            int res=0;

            for(int b=0;b<_Lmax;b++)
            {
                if(sText[b].font && sText[b].font->chr['m'].W>res)
                {
                    res=sText[b].font->chr['m'].W;
                }
            }

            return res;
        }

        int GetMaxFontHeight()
        {
            int res=0;

            for(int b=0;b<_Lmax;b++)
            {
                if(sText[b].font && sText[b].font->Height>res)
                {
                    res=sText[b].font->Height;
                }
            }

            return res;
        }

        TextBox(Font* font,int x,int y,int width,int height,int Encoding=0,pixel DefColor=pixel(0,0,0))
        {
            _l=_h=_dh=_X=0;
            _Lmax=128;

            X = x;
            Y = y;

            Width = width;
            Height = height;

            encoding = Encoding;
            dColor = DefColor;

            _font=font;

            Text=new char[_Lmax+1];
            sText=new Style[_Lmax+1];
            mText=(MatchTable<char,Style>*)0;
            //mText=new MatchTable<char,Style>();

            for(int b=0;b<=_Lmax;b++)
            {
                Text[b]=0;
                sText[b].Set(font,dColor);
            }

            //sText[5].color=pixel(15,155,5);

            int Mw=GetMaxFontWidth();

            if(Width<Mw+_mw) Width=Mw+_mw;

            ///if(Width<_font->chr['m'].W+_mw) Width=_font->chr['m'].W+_mw;

            Selected=isFocussed=false;
            isVisible=true;

            //dColor=pixel(0,0,0);
            cColor=pixel(0,0,0);        ///Cursor color
        }

        bool tryFocuss()
        {
            return false;
        }

        void Set(char* text)
        {
            if(!text || !text[0]) return;

            int L=lng(text);

            if(L>=_Lmax)
            {
                Font* __f=(Font*)0;

                if(sText) __f=sText[0].font;

                delete [] Text;
                delete [] sText;

                _Lmax=L+128;
                Text=new char[_Lmax+1];
                sText=new Style[_Lmax+1];

                sText[0].font=__f;
                sText[0].color=pixel(0,0,0);

                if(_Lmax>16000000) printf("String with over 16 million characters. Seriously? -_-\n");
            }

            //if(!sText) return;
            //if(!sText[0].font) return;

            //memcpy(Text,text,L+1);

            int _w=0;
            _h=0;

            for(int b=0;b<L;b++)
            {
                Text[b]=text[b];
                if(b!=0) sText[b].Clr();

                if(Text[b]!=10) _w+=sText[0].font->chr[(unsigned char)Text[b]].W;

                if(Text[b]==10 || _w+sText[0].font->chr[(unsigned char)Text[b+1]].W>Width-_mw)
                {
                    _w=0;
                    _h++;
                }
            }

            Text[L]=0;

            _w=sText[0].font->Height+_mh;

            if(_h*_w>Height) _dh=(_h*_w-Height)/_w;

            _l=L;
            _l=_dh=0;
        }

        void KeyPress()
        {
            int _i,_w; //_L
            int _H=Height/GetMaxFontHeight();///_font->Height;

            if(isFocussed)
            {
                if(sKey.Char || sKey.Key)
                {
                    //_L=lng(Text);

                    if(sKey.Key==8 || sKey.Key==46)     ///backspace / delete
                    {
                        if(Selected)
                        {
                            //for(int b=0;b<_L;b++) sText[b].Set(_font,dColor);

                            Text[0]=0;
                            _l=_dh=0;
                        }
                        else if(_l>0 || (sKey.Key==46 && _l==0))
                        {
                            _i=_l-1+(sKey.Key==46);

                            while(Text[_i])
                            {
                                sText[_i]=sText[_i+1];
                                Text[_i++]=Text[_i+1];
                            }

                            if(sKey.Key==8) _l--;
                        }
                    }
                    else if(sKey.Key==39)       ///->
                    {
                        //if(_l<lng(Text))
                        if(Text[_l])
                        {
                            /**if(Text[_l]==2)
                            {
                                //while(Text[++_l]!=5);
                                //_l++;
                                _l+=5;
                            }*/

                            _l++;
                        }
                    }
                    else if(sKey.Key==37)       ///<-
                    {
                        if(_l>0)
                        {
                            /**if(_l>1 && Text[_l-2]==5)
                            {
                                //while(Text[--_l]!=2);
                                _l-=6;
                            }
                            else */

                            _l--;
                        }
                    }
                    else if(sKey.Key==40)       ///down
                    {
                        if(_dh<_h && _h>_H) _dh++;

                        //_l=min(lng(Text),_l+Width/_font->chr['A'].W);

                        //if(_h<_H) _Y=_h+1;

                        //printf("%d\n",_Y);

                        _w=0;
                        _i=_l;

                        while(Text[_l]!=10 && Text[_l]) _l++;
                        while(Text[++_l]!=10 && Text[_l] && _w<_X-X) _w+=sText[0].font->chr[c_abs(Text[_l])].W;
                        if(!Text[_l]) _l=_i;
                    }
                    else if(sKey.Key==38)       ///up
                    {
                        if(_h-_dh>0)
                        {
                            if(_h>_H && _dh>1-_h) _dh--;

                            //_l=max(0,_l-Width/_font->chr['A'].W);

                            //_Y=_h-1;

                            _w=0;

                            if(_l>0)
                            {
                                while(Text[--_l]!=10 && _l>0);
                                while(Text[--_l]!=10 && _l>=0);
                                while(Text[++_l]!=10 && Text[_l] && _w<_X-X) _w+=sText[0].font->chr[c_abs(Text[_l])].W;
                            }
                        }
                    }
                    else if(sKey.Char)
                    {
                        //if(sKey.Char!=_tc)
                        //{
                            if(sKey.Char==13) sKey.Char=10;
                            //else if(sKey.Char==136) sKey.Char=5;

                            if(sKey.Char==1)
                            {
                                Selected=true;
                                return;
                            }
                            if(sKey.Char==3)
                            {
                                if(Selected && OpenClipboard(MainDevice->hwnd))
                                {
                                    HGLOBAL data;

                                    EmptyClipboard();

                                    int _L = lng(Text);

                                    data = GlobalAlloc(GMEM_MOVEABLE,_L+1);

                                    char* _text=(char*)GlobalLock(data);
                                    memcpy(_text,Text,_L+1);
                                    GlobalUnlock(data);

                                    SetClipboardData(CF_TEXT,data);
                                    CloseClipboard();
                                }
                            }
                            else if(sKey.Char==22)
                            {
                                if(OpenClipboard(MainDevice->hwnd))
                                {
                                    HANDLE data=GetClipboardData(CF_TEXT);

                                    if(data)
                                    {
                                        char* str=(char*)GlobalLock(data);

                                        GlobalUnlock(data);

                                        Set(str);
                                        Selected=true;

                                        int _L=lng(Text);

                                        for(int b=0;b<_L;b++)
                                        {
                                            if(mText && mText->N)
                                            {
                                                Style* __s=mText->GetRes(Text[b]);

                                                if(__s!=(Style*)0)
                                                {
                                                    if(__s->font) sText[b].Set(__s->font,__s->color);
                                                    else sText[b].Set(__s->color);
                                                }
                                                else sText[b].Set(_font,dColor);
                                            }
                                        }
                                    }

                                    CloseClipboard();
                                }
                            }
                            else if(sKey.Char > 28 || sKey.Char == 10)
                            {
                                ///if(sKey.Char==13) sKey.Char=10;

                                if(Selected)
                                {
                                    Text[0]=sKey.Char;
                                    Text[1]=0;

                                    _l=1;
                                    _dh=0;
                                }
                                else // || sKey.Char==6 || sKey.Char==5)
                                {
                                    //if(sKey.Char==6) cColor.r=255;
                                    //else if(sKey.Char==5) cColor.r=0;

                                    _i=_l;

                                    char _c=sKey.Char;
                                    Style* __s=(Style*)0;

                                    if(mText && mText->N) __s=mText->GetRes(_c);

                                    Style _ts;

                                    if(__s!=(Style*)0) _ts=*__s;
                                    else _ts.Set(_font,dColor);

                                    Style _t;

                                    /*while(Text[max(-1,_i-2)])
                                    {
                                        Text[_i]=Text[_i]^_c;
                                        _c=Text[_i]^_c;
                                        Text[_i]=Text[_i]^_c;

                                        _t=sText[_i];

                                        if(_ts.font) sText[_i].Set(_ts.font,_ts.color);
                                        else sText[_i].Set(_ts.color);

                                        _ts=_t;

                                        _i++;
                                    }*/

                                    char* _f0 = Text+_i;
                                    char* _f = _f0;
                                    while(*_f) _f++;

                                    _f++;

                                    if(_i + _f - _f0>= _Lmax)
                                    {
                                        _Lmax+=128;

                                        char* __t=new char[_Lmax+1];
                                        memcpy(__t,Text, _i + _f - _f0+1);

                                        delete __s;
                                        __s=new Style[_Lmax+1];
                                        memcpy(__s,sText,(_i + _f - _f0+1)*sizeof(Style));

                                        delete [] Text;
                                        delete [] sText;

                                        Text = __t;
                                        sText = __s;

                                        if(_Lmax>16000000) printf("String with over 16 million characters. Seriously? -_-\n");
                                    }

                                    for(int _q = _i + _f - _f0+(sKey.Char == 10)+1; _q>_i;_q--) Text[_q] = Text[_q-1];
                                    //memcpy(Text+_i+1, Text+_i, _f - _f0+1);`

                                    Text[_i] = _c;

                                    //Text[_i + _f - _f0+1]=0;
                                    //sText[_i].Set(_font,dColor);

                                    _l++;
                                }
                            }

                            //_tc=sKey.Char;
                        //}

                        //return;
                    }
                    //else _tc=0;

                    //if(sKey.Char==sKey.Key) return;

                    if(!sKey.Control && (sKey.Char>4 || sKey.Key)) Selected=false;

                    if(sKey.Char || sKey.Key)
                    {
                        _w=0;
                        _h=0;

                        int _L = lng(Text);

                        for(int b=0;b<_L;b++)
                        {
                            if(Text[b]!=10) _w+=sText[0].font->chr[(unsigned char)Text[b]].W;

                            if(Text[b]==10 || _w+sText[0].font->chr[(unsigned char)Text[b+1]].W>Width-_mw)
                            {
                                _w=0;
                                _h++;
                            }
                        }
                    }
                }
            }
        }

        void Draw()
        {
            if(!isVisible) return;

            int _cH=GetMaxFontHeight()+_mh;///(_font->Height+_mh);
            int _H=Height/(_cH-_mh);///_font->Height;

            if(isLeftPressed || isDoubleClicked)
            {
                isFocussed=(MouseX>=X && MouseX<=X+Width && MouseY>=Y && MouseY<=Y+Height);
            }

            ///______________________________________________________________________________________

            ///Text_out(_font,X,Y,Text,Color{0,0,0},Width,Height,Selected,encoding);

            if(Selected)
            {
                FillRect(X,Y,X+Width,Y+(_h+1)*_cH+2,pixel(0,0,0));
                FillRect(X,Y+(_h+1)*_cH+2,X+Width,Y+Height,pixel(255,255,255));
            }
            else FillRect(X,Y,X+Width,Y+Height,pixel(255,255,255));

            for(int b=0;b<11;b++)
            {
                DrawLine(X-b,Y,X-b,Y+Height,pixel(3*(10-b),0,0,50));
                DrawLine(X+Width+b,Y,X+Width+b,Y+Height,pixel(3*(10-b),0,0,50));
            }

            char* ch=Text;
            Font* _font=sText[0].font;

            int Dh=_h-_H+1+_dh;
            int _w=0;

            while(Dh>0 && *ch)
            {
                if(*ch!=10) _w+=_font->chr[(unsigned char)*ch].W;

                if(*ch==10 || _w+_font->chr[(unsigned char)*(ch+1)].W>Width-_mw)
                {
                    _w=0;
                    Dh--;
                }

                ch++;
            }

            unsigned char cur=*ch;
            unsigned char p,t=208,sl=Selected ? 255 : 0;
            int _x=0;
            int _y=0;
            int h=0;
            int x0=X;
            _w=0;

            pixel __C=sText[0].color;   ///tColor;

            while(*ch)
            {
                //if(*ch==0) break;

                if(ch-Text==_l && isFocussed)
                {
                    DrawLine(x0+1,Y+h,x0+1,Y+h+_cH,cColor);
                    _X=x0;
                }
                ///DrawLine(x0+1,h+Y+_dh*_cH+1,x0+1,h+Y+(_dh+1)*_cH+1,pixel(0,0,50));

                p=c_abs(*ch);

                if(p==13)
                {
                    ch++;
                    continue;
                }
                else if(p==10)
                {
                    h+=_font->Height+5;
                    x0=X;
                    ch++;
                    continue;
                }
                /**else if(p==6)
                {
                    _w=(int)ch;

                    while(*ch!=5 && *ch!=0) ch++;

                    if(*ch==0) ch=(char*)_w;
                    else
                    {
                        ch=(char*)_w;

                        *ch=2;

                        for(int _o=1;_o<7;_o++)
                        {
                            if(*(ch+_o)>='A' && *(ch+_o)<='F') *(ch+_o)-=7;
                            else if(*(ch+_o)>='a' && *(ch+_o)<='f') *(ch+_o)-=39;
                            else if(*(ch+_o)<'0' || *(ch+_o)>'9') *(ch+_o)=0;
                        }

                        *(ch+1)=min(255,(*(ch+1)-48)*16+*(ch+2)-47);
                        *(ch+2)=min(255,(*(ch+3)-48)*16+*(ch+4)-47);
                        *(ch+3)=min(255,(*(ch+5)-48)*16+*(ch+6)-47);

                        __C=pixel(*(ch+1),*(ch+2),*(ch+3));

                        *(ch+4)=5;
                        ch+=5;
                        _l-=2;

                        while(*ch=*(ch+3)) ch++;

                        //_l=lng(Text);

                        //for(int _o=0;_o<_l;_o++) printf("%c(%d) ",Text[_o],Text[_o]);

                        //printf("%d %d\n",Text[_l],_l);

                        //ch++;
                    }
                }
                else if(p==2)
                {
                    __C=pixel(*(ch+1),*(ch+2),*(ch+3));
                    ch+=5;
                    p=*ch;

                    //printf("%d\n",p);

                    if(!p) break;
                }*/

                if(encoding==ENC_ANSI)
                {
                    p = c_abs(*ch);
                }
                else if(encoding==ENC_UNICODE)
                {
                    if(c_abs(*ch)>207) t=c_abs(*ch);

                    if(c_abs(*ch)<208)
                    {
                        if( (p>143 && p<176) || (p>175 && p<192) )
                        {
                            if(p==145 && t==209) p=184;
                            else p+=48;
                        }
                        else if(p>127 && p<146)
                        {
                            if(p==129 && t==208) p=168;
                            else p+=112;
                        }
                    }
                }

                if(Width>0 && x0-X+_font->chr[p].W>=Width)
                {
                    //x0=X;

                    if((h+2*_font->Height+10)/_font->Height>_H) break;
                    else
                    {
                        h+=_font->Height+5;
                        x0=X;
                    }
                }

                if(sText[(int)(ch-Text)].color.a>0) __C=sText[(int)(ch-Text)].color;
                if(sText[(int)(ch-Text)].font) _font=sText[(int)(ch-Text)].font;

                if(encoding == ENC_ANSI || c_abs(*ch)<208)
                {
                    for(int x=0;x<_font->chr[p].W;x++)
                    {
                        for(int y=0;y<_font->Height;y++)
                        {
                            if(_font->chr[p].matrix[x][y]!=255)
                            {
                                if(Width<0 || x+x0-X<Width)
                                {
                                    PutPixel(x+x0-_x+_mw,y+Y-_y+h+_mh,pixel(__C.a*(255-_font->chr[p].matrix[x][y])/255,
                                                                            abs(sl-__C.r),abs(sl-__C.g),abs(sl-__C.b)));
                                }
                            }
                        }
                    }

                    x0+=_font->chr[p].W;
                }

                ch++;
            }

            if((ch-Text==_l || (h+_font->Height+5)/_font->Height>_H)&& isFocussed)
            {
                DrawLine(x0+1,Y+h,x0+1,Y+h+_cH,cColor);
                _X=x0;
            }

            ///DrawLine(x0+1,h+Y+_dh*_cH+1,x0+1,h+Y+(_dh+1)*_cH+1,pixel(0,0,50));
        }

        ~TextBox()
        {
            delete [] Text;
            delete sText;
            delete mText;
            _l=_h=_dh=_X=0;
            _Lmax=128;

            Selected=isFocussed=isVisible=false;

            //tColor=pixel(0,0,0);
            cColor=pixel(0,0,0);
        }
    };

};


#endif // _GUI_H_INCLUDED
