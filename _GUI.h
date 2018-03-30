#ifndef _GUI_H_INCLUDED
#define _GUI_H_INCLUDED

#include "Font.h"

#include <string>

#include <stdarg.h>
#include <string.h>

#ifndef $extend
    #define $extend(A,n,N,type) {type* _T=new type[N]; memcpy(_T,A,sizeof(type)*(n)); delete [] A; A=_T;}
#endif

#define _drawLine DrawLine
#define _PutPixel PutPixel
#define _drawRect FillRect
#define _DrawImage(x,y,img) DrawImage(img,x,y)

#define eql(a, b) (strcmp(a, b) == 0)

#ifndef IsNumeric
    #define IsNumeric(a) ((a>='0' && a<='9') || a=='.' || a=='+' || a=='-' || a=='e' || a=='E')
#endif

template<class Type>
inline Type max(const Type& a, const Type& b)
{
    return a>b ? a : b;
}

template<class Type>
inline Type min(const Type& a, const Type& b)
{
    return a<b ? a : b;
}

void set(char* s,const char* res,char terminate)
{
    while(*res!=terminate && *res) *s++=*res++;
    *s = 0;
}

void t_reduce(char* s,char t)
{
    while(*s) s++;

    while(*s!=t) *s--=0;
}

/// GUI classes:
///
/// Button   ($Font* font, int x, int y, char* caption, int Width, int Height, bool hold=false)
///
/// Edit     ($Font* font, int x, int y, int Width, int Height, bool numeric=false,unsigned short style=0)
///
/// Label    ($Font* font, int x, int y, char* text)
///
/// CheckBox ($Font* font, int x, int y, char* text, bool checked=false)
///
/// Slider   ($Font* font, int x, int y, int Width, double k, char* text=0, int Space=5, $Font* TxFont=0)
///
/// Scroll   ($Font* font, int x, int y...)
///
/// TextBox  ($Font* font, int x, int y, int Width, int Height, int Encoding=0, pixel DefColor = pixel(0,0,0))

namespace _GUI
{
    ///margins for GUIGroup::Add
    #define sNone(dx, dy) 0,0, dx,dy

    #define sLeft(dx, dy) -1,0, dx,dy
    #define sRight(dx, dy) 1,0, dx,dy
    #define sUp(dx, dy) 0,-1, dx,dy
    #define sDown(dx, dy) 0,1, dx,dy

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

    enum ALIGN
    {
    	sLEFT=1,
    	sRIGHT=2,
    	sTOP=4,
    	sBOTTOM=8
    };

	enum TYPE
	{
		tNONE=0,
		tGUIGROUP,
		tSEPARATOR,
		tBUTTON,
		tEDIT,
		tLABEL,
		tCHECKBOX,
		tSLIDER,
		tSCROLL,
		tTEXTBOX
	};

    class GUI
    {
        protected:

        struct sElement
        {
            char* name;
            char* extention;

            sElement()
            {
            	name = extention = 0;
            }

            ~sElement()
            {
                if(name) delete [] name;
                if(extention) delete [] extention;
            }
        };

        $Font* _font;

        public:

        TYPE _type;

        bool isVisible;
        bool isFocussed;
        int X,Y;
        unsigned Width,Height;

        std::string Text;

        pixel color;

        GUI()
        {
        }

        GUI($Font* font, int x, int y, TYPE t = tNONE) : _font(font), X(x), Y(y), _type(t)
        {
            isVisible = true;
            isFocussed = false;

            Width = Height = 0;

            color = pixel(0, 0, 0);
        }

        virtual ~GUI()
        {
            _font = 0;
        }

        virtual void Draw()=0;

        virtual bool CursorOn()
        {
        	return MouseX>=X && MouseX<=X+Width && MouseY>=Y && MouseY<=Y+Height;
        }

        virtual bool tryFocuss()
        {
            return isFocussed = CursorOn();
        }

        virtual void KeyPress()
        {
        }

        virtual int GetX(char* s)
        {
            return X;
        }

        virtual int GetWidth()
        {
            return Width;
        }

        virtual int GetHeight()
        {
            return Height;
        }

        virtual void Set(const char* text)
        {
            Text = text;
        }

        virtual GUI* AllignTo(GUI* g, unsigned char side = sLEFT)
        {
        	if(side & sLEFT) X=g->X;
        	if(side & sTOP) Y=g->Y;

        	if(side & sRIGHT) X = g->X + g->Width - Width;
        	if(side & sBOTTOM) Y = g->Y + g->Height - Height;

        	return this;
        }

        virtual void PostDraw()
        {
        }
    };

    class Separator : public GUI
    {
        protected:

        unsigned short dr;

        public:

        Separator(short r=4, const pixel& color=pixel(100,0,0,0)) : GUI(0, 0, 0, tSEPARATOR)
        {
            dr=r;

            this->color=color;
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
    };

    class GUIGroup : public GUI
    {
        public:

        struct component
        {
            char* name;
            GUI* P;

            component(GUI* A,const char* Name=0)
            {
                P = A;
                name = 0;

                if(Name)
                {
                    name=new char[strlen(Name)+1];
                    strcpy(name,Name);
                }
            }

            ~component()
            {
                if(name) delete [] name;
                P = 0;
            }
        };

        struct GSParam
        {
            int dx,dy;
            unsigned char style;

            pixel uColor;
            pixel lColor;

            bool Shadowed;

            GSParam(unsigned char Style,bool shadowed,
                    const pixel& color, const pixel& _color = pixel(0), int Dx=4,int Dy=4)
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

        #ifdef TREE_H_INCLUDED

        Trie<char, component*>* tData;

        #endif

        GUIGroup(unsigned char Style=GS_PLAIN+GS_HORIZ,
                 bool shadowed=true,
                 const pixel& color=pixel(240,245,255),
                 const pixel& _color=pixel(202,215,250)) : GUI(0, 2147483647, 2147483647, tGUIGROUP)
        {
            data=new component*[N_MAX=8];
            N=0;

            param=new GSParam(Style,shadowed,color,_color);

            #ifdef TREE_H_INCLUDED

            tData = new Trie<char, component*>(0);

            #endif
        }

        ~GUIGroup()
        {
            if(data) delete [] data;
            data = 0;
            N=0;

            delete param;

            #ifdef TREE_H_INCLUDED

            if(tData) delete tData;
            tData = 0;

            #endif
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

        void Add(int dr)        ///Separator
        {
            if(N==N_MAX)
            {
                N_MAX+=10;

                $extend(data,N,N_MAX,component*);
            }

            data[N++]=new component(new Separator(dr));

            bool dimensional = true;

            if(N>1 && data[N-1]->P->_type==tSEPARATOR)
            {
                if(param->style & GS_HORIZ) data[N-1]->P->X = data[N-2]->P->X + data[N-2]->P->Width;
                else if(param->style & GS_VERT) data[N-1]->P->Y = data[N-2]->P->Y + data[N-2]->P->Height;
                else data[N-1]->P->_type=tNONE;     ///no separators in plain-styled groups

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

        component* Add(GUI* A, const char* name = 0,
                       char ox = 0, char oy = 0, int dx = 0, int dy = 0, bool dimensional=true)
        {
            if(N==N_MAX)
            {
                N_MAX+=10;

                $extend(data,N,N_MAX,component*);
            }

            if(A)
            {
                data[N++]=new component(A,name);

                if((ox || oy) && N>1)
                {
                    if(!data[N-1]->P->X)
                    {
                        data[N-1]->P->X = data[N-2]->P->X +
                                      ((ox>0) ? data[N-2]->P->Width
                                              : ((ox<0) ? - data[N-1]->P->Width : 0));
                    }

                    if(!data[N-1]->P->Y)
                    {
                        data[N-1]->P->Y = data[N-2]->P->Y +
                                      ((oy>0) ? data[N-2]->P->Height
                                              : ((oy<0) ? - data[N-1]->P->Height : 0));
                    }
                }

                data[N-1]->P->X += dx;
                data[N-1]->P->Y += dy;

                #ifdef TREE_H_INCLUDED

                tData->AddKey(name, data[N-1]);

                #endif
            }
            else data[N++]=new component(new Separator());

            if(N>1 && data[N-1]->P->_type==tSEPARATOR)
            {
                if(param->style & GS_HORIZ) data[N-1]->P->X = data[N-2]->P->X + data[N-2]->P->Width;
                else if(param->style & GS_VERT) data[N-1]->P->Y = data[N-2]->P->Y + data[N-2]->P->Height;
                else data[N-1]->P->_type=tNONE;     ///no separators in plain-styled groups

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

            return data[N-1];
        }

        void RecalculateBorders()
        {
        	if(N)
        	{
        		X = data[0]->P->X;
        		Y = data[0]->P->Y;

        		Width = data[0]->P->Width;
        		Height = data[0]->P->Height;
        	}

        	for(int k=1;k<N;k++)
        	{
        		if(data[k]->P->_type > tSEPARATOR)
        		{
        			if(X > data[k]->P->X)
					{
						Width += X - data[k]->P->X;
						X = data[k]->P->X;
					}

					if(Y > data[k]->P->Y)
					{
						Height += Y - data[k]->P->Y;
						Y = data[k]->P->Y;
					}

					if(data[k]->P->X + data[k]->P->Width > X+Width) Width = data[k]->P->X + data[k]->P->Width - X;
					if(data[k]->P->Y + data[k]->P->Height > Y+Height) Height = data[k]->P->Y + data[k]->P->Height - Y;
        		}
        	}
        }

        GUI* Last()
        {
            return N ? data[N-1]->P : 0;
        }

        GUI* Get(const char* name)
        {
            #ifdef TREE_H_INCLUDED

            Trie<char, component*>* tr = tData->FindKey(name);
            if(tr) return tr->value->P;

            #else

            for(int k=0;k<N;k++)
            {
                if(data[k]->name && data[k]->name[0]==name[0])
                {
                    if(strcmp(data[k]->name, name) == 0) return data[k]->P;
                }
            }

            #endif

            return 0;
        }

        GUI* operator[](const char* name)
        {
            return this->Get(name);
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
                if(data[k]->P->_type == tSEPARATOR)
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

                    data[k]->P->_type = tNONE;      ///separator set up
                }

                data[k]->P->Draw();
                data[k]->P->PostDraw();
            }
        }

        bool tryFocuss()
        {
            isFocussed=false;

            for(int k=0;k<N;k++)
            {
                bool r=data[k]->P->tryFocuss();
                if(r) isFocussed=true;
            }

            return isFocussed;
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

        bool Enabled;
        Image* img;

        Button() : GUI()
        {
            isVisible=false;
            img = 0;

            _type = tBUTTON;
        }

        Button($Font* font,int x0,int y0, const char* caption,unsigned width = 0,unsigned height = 0,bool hold=false)
                : GUI(font, x0, y0, tBUTTON)
        {
            Width = width ? width : Text_Width(font,caption)+10;
            Height = height ? height : font->Height+8;

            Enabled=true;

            _hold=hold;

            Text = caption;
            img = 0;
        }

        bool tryFocuss()
        {
            return isFocussed = isLeftPressed && CursorOn() && Enabled;
        }

        void Draw()
        {
            if(isVisible)
            {
                unsigned char _add= (isFocussed || _hold) ? 50 : 0;

                _drawLine(X+1,Y,X+Width-1,Y,pixel(255,112+_add,112+_add,122+_add));
                _drawLine(X,Y+1,X,Y+Height-1,pixel(255,112+_add,112+_add,122+_add));
                _drawLine(X+Width,Y+1,X+Width,Y+Height-1,pixel(255,112+_add,112+_add,122+_add));
                _drawLine(X+1,Y+Height,X+Width-1,Y+Height,pixel(255,112+_add,112+_add,122+_add));

                _drawLine(X+2,Y+1,X+Width-1,Y+1,pixel(255,243-_add/2,243-_add/2,243-_add/2));
                _drawLine(X+2,Y+Height-1,X+Width-1,Y+Height-1,pixel(255,243-_add/2,243-_add/2,243-_add/2));
                _drawLine(X+2,Y+1,X+2,Y+Height-1,pixel(255,243-_add/2,243-_add/2,243-_add/2));
                _drawLine(X+Width-1,Y+1,X+Width-1,Y+Height-1,pixel(255,243-_add/2,243-_add/2,243-_add/2));

                //for(int v=Y+2;v<Y+Height/2;v++) _drawLine(X+3,v,X+Width-1,v,pixel(255,240-_add,240-_add,240-_add));
                //for(int v=Y+Height/2;v<Y+Height-2;v++) _drawLine(X+3,v,X+Width-1,v,pixel(255,212-_add,212-_add,212-_add));
                _drawRect(X+1,Y+2,X+Width-1,Y+Height/2-1,pixel(255,240-_add,240-_add,240-_add));
                _drawRect(X+1,Y+Height/2,X+Width-1,Y+Height-1,pixel(255,212-_add,212-_add,212-_add));

                if(Enabled) Text_out(_font,X+(Width-Text_Width(_font,Text.data()))/2,Y+(Height-_font->Height)/2, Text.data());
                else Text_out(_font,X+(Width-Text_Width(_font,Text.data()))/2,Y+(Height-_font->Height)/2,Text.data(),pixel(150,150,150));

                if(img) _DrawImage(X+(Width-img->Width)/2+1,Y+(Height-img->Height)/2+1,img);
            }
        }

        ~Button()
        {
            _font = 0;

            if(img) delete img;
            img = 0;
        }
    };

    class Edit : public GUI
    {
        private:

        int _offset;
        int _l;
        bool _numeric;

        public:

        bool Selected;
        unsigned short Style;

        Edit() : GUI()
        {
            isVisible=false;

            _type = tEDIT;
        }

        Edit($Font* font,int x0,int y0,int width,int height=-1,bool numeric=false,unsigned short style = EDIT_NORMAL)
                : GUI(font, x0, y0, tEDIT)
        {
            Width=width;
            Height=height;

            if(height<=0) Height=font->Height+4;

            Selected=false;
            _numeric=numeric;
            _offset=_l=0;

            Style=style;
        }

        bool tryFocuss()
        {
            if(isVisible)
            {
                isFocussed = CursorOn();

                if(!isFocussed) Selected=false;
                else if(isDoubleClicked) Selected=true;

                return isLeftPressed && isFocussed;
            }
            else return false;
        }

        void Set(const char* text)
        {
            Text = text;
            _l = Text.length();
        }

        void KeyPress()
        {
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
                            //Text[0] = 0;
                            Text.clear();
                            _l=0;
                            _offset=0;
                        }
                        else if(_l>0 || (_Key==46 && _l==0))
                        {
                            Text.erase(_l-1+(_Key==46), 1);

                            if(_Key==8) _l--;

                            if(Text_Width(_font,Text.data())<Width) _offset=0;
                        }
                    }
                    else if(_Key==39)
                    {
                        if(_l<Text.length()) _l++;
                        if(Text_Width(_font,Text.data()-_offset,0,_l)>Width) _offset--;
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

                                data = GlobalAlloc(GMEM_MOVEABLE,Text.length()+1);

                                char* _text=(char*)GlobalLock(data);
                                memcpy(_text,Text.data(),Text.length()+1);
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
                                //Text[0] = 0;
                                Text.clear();
                                _l=0;
                                _offset=0;
                            }

                            if(_Key!=39 && _Key!=37 && _Char!=0)
                            {
                                static char _s[2] = {0, 0};

                                _s[0] = _Char;
                                Text.insert(_l, _s);

                                _l++;

                                if(Text_Width(_font,Text.data()-_offset)>Width) _offset--;
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

                if(!(Style & EDIT_LABEL)) _drawRect(X,Y+1,X+Width,Y+Height,pixel(255,240,240,240));

                if(Selected)
                {
                    _drawRect(X,Y+3,
                              X+((Text_Width(_font,Text.data())<=Width) ? Text_Width(_font,Text.data()) : Width),
                              Y+_font->Height+1,
                              pixel(255,15,15,15));
                }
                else if(!Style) _drawRect(X,Y+2,X+Width,Y+Height,pixel(255,255,255,120));

                int _cur_pos = Text_out(_font,X+2,Y+(Height-_font->Height)/2,Text.data()-_offset,color,Width-2,Selected,0,true);
                if(_cur_pos>X+Width) _cur_pos=X+Width;//-1;

                if(!(Style & EDIT_LABEL))
                {
                    _drawLine(X+1,Y,X+Width,Y,pixel(255,67,69,76));

                    _drawLine(X+1,Y+1,X+Width,Y+1,pixel(255,200,200,200));
                    _drawLine(X+1,Y+2,X+1,Y+Height,pixel(200,200,200,200));

                    _drawLine(X,Y+1,X,Y+Height,pixel(255,95,96,102));
                    _drawLine(X+Width,Y+1,X+Width,Y+Height,pixel(255,130,133,141));
                }

                _drawLine(X+1,Y+Height,X+Width,Y+Height,pixel(255,138,141,153));

                if(isFocussed && Style)
                {
                    int __x;

                    if(_l+_offset==0) __x=X+2;
                    else __x=X+min(Width-1,Text_Width(_font,Text.data()-_offset,0,_l+_offset)+2);

                    _drawLine(__x,Y+2,__x,Y+_font->Height+2,pixel(255,0,0,0));
                }
            }
        }

        ~Edit()
        {
            _font = 0;
        }
    };

    class Label : public GUI
    {
        public:

        Label($Font* font, int x0, int y0, const char* text) : GUI(font, x0, y0, tLABEL)
        {
            Height=_font->Height;

            Text = text;

            Width=Text_Width(font,text);
        }

        int GetX(char* s)
        {
            return X + Text_Width(_font, s);
        }

        void Set(const char* text)
        {
            Text = text;

            Width=Text_Width(_font, text);
        }

        void Draw()
        {
            if(isVisible && Text.data()) Text_out(_font,X,Y,Text.data());
        }

        ~Label()
        {
            _font = 0;
        }
    };

    class CheckBox : public GUI
    {
        public:

        bool isChecked;

        static const int dr = 12;

        CheckBox($Font* font,int x0,int y0,const char* text,bool checked=false)
                 : GUI(font, x0, y0, tCHECKBOX)
        {
            Text = text;

            isChecked=checked;

            Width = Text_Width(_font,text)+dr+8;
            Height = max(dr+1,(int)_font->Height);
        }

        bool tryFocuss()
        {
            const int& _m_x=sMouse.X;
            const int& _m_y=sMouse.Y;

            if(isLeftPressed)
            {
                isFocussed = _m_x>=X && _m_x<=X+Text_Width(_font,Text.data())+20 && _m_y>=Y && _m_y<=Y+10;

                if(isFocussed) isChecked=!isChecked;
            }

            return isFocussed;
        }

        int GetHeight()
        {
            return Height;
        }

        void Draw()
        {
            if(isVisible)
            {
                _drawRect(X+2,Y+2,X+dr,Y+dr,pixel(230,255,255,255));

                _drawLine(X+1,Y+1,X+1,Y+dr,pixel(200,120,120,120));
                _drawLine(X+1,Y+1,X+dr,Y+1,pixel(200,120,120,120));

                _drawLine(X,Y,X+dr,Y,pixel(255,117,119,126));
                _drawLine(X,Y,X,Y+dr,pixel(255,145,146,152));

                _drawLine(X+dr,Y+1,X+dr,Y+dr,pixel(255,180,183,191));
                _drawLine(X+1,Y+dr,X+dr-1,Y+dr,pixel(255,188,191,203));

                Text_out(_font,X+dr+6,Y - (_font->Height - dr)/2,Text.data());

                if(isChecked)
                {
                    int d = (dr - 12)/2;

                    X+=d;
                    Y+=d;

                    _drawLine(X+3,Y+5,X+3,Y+8,pixel(255,0,0,0));
                    _drawLine(X+4,Y+6,X+4,Y+9,pixel(255,0,0,0));
                    _drawLine(X+5,Y+7,X+5,Y+10,pixel(255,0,0,0));

                    _drawLine(X+6,Y+6,X+6,Y+9,pixel(255,0,0,0));
                    _drawLine(X+7,Y+5,X+7,Y+8,pixel(255,0,0,0));
                    _drawLine(X+8,Y+4,X+8,Y+7,pixel(255,0,0,0));
                    _drawLine(X+9,Y+3,X+9,Y+6,pixel(255,0,0,0));

                    X-=d;
                    Y-=d;
                }
            }
        }

        ~CheckBox()
        {
            _font = 0;
        }
    };

    class Slider : public GUI
    {
        private:

        bool _once;
        $Font* _txfont;

        public:

        static const int _dh=10;
        static const int _dw=4;

        int _x;
        double _k;
        int dx;

        Slider($Font* font,int x,int y,int width,double k,const char* text=0,int Space=5,$Font* TxFont=0)
                : GUI(font, x, y, tSLIDER)
        {
            _k=k;

            _x=0;

            _once=true;

            Width=width;
            Height=2*_dh;

            dx=0;
            Text = text;

            if(text)
            {
                if(TxFont) _txfont=TxFont;
                else _txfont=_font;

                dx=Space + Text_Width(_txfont,text);
            }

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

            _drawLine(X+dx,Y+_dh,X+Width,Y+_dh,pixel(200,50,75,100));
            _drawLine(X+dx,Y+_dh+1,X+Width,Y+_dh+1,pixel(200,130,140,170));
            _drawLine(X+dx,Y+_dh+2,X+Width,Y+_dh+2,pixel(200,140,165,190));

            _drawRect(X+dx+_x-_dw+1,Y+1,X+dx+_x+_dw-1,Y+2*_dh-1,pixel(200,225,232,255));

            _drawLine(X+dx+_x-_dw,Y+1,X+dx+_x-_dw,Y+2*_dh-1,pixel(100,0,0,100));
            _drawLine(X+dx+_x-_dw+1,Y+1,X+dx+_x-_dw+1,Y+2*_dh-1,pixel(50,0,100,100));

            _drawLine(X+dx+_x+_dw,Y+1,X+dx+_x+_dw,Y+2*_dh-1,pixel(100,0,0,100));
            _drawLine(X+dx+_x+_dw-1,Y+1,X+dx+_x+_dw-1,Y+2*_dh-1,pixel(40,0,70,100));

            _drawLine(X+dx+_x-_dw+1,Y,X+dx+_x+_dw-1,Y,pixel(100,0,0,100));
            _drawLine(X+dx+_x-_dw+1,Y+2*_dh,X+dx+_x+_dw-1,Y+2*_dh,pixel(100,0,0,100));

            if(Text.data()) Text_out(_txfont,X,Y+2,Text.data());
        }

        ~Slider()
        {
            _font = 0;
        }
    };

    class Scroll : public GUI
    {
        private:

        unsigned _n,_max_width;

        public:

        bool _checked;
        int _id;
        sElement* List;

        Scroll() : GUI()
        {
            _type = tSCROLL;
        }

        Scroll($Font* font,int x,int y...)
                : GUI(font, x, y, tSCROLL)
        {
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

                List[b].name=new char[strlen(p)+1];
                //clear(List[b].name);
                //List[b].name[0] = 0;
                set(List[b].name,p,'|');

                List[b].extention=new char[strlen(p)-strlen(List[b].name)+1];
                //overwrite(p,List[b].extention,strlen(List[b].name)+2);
                strcpy(List[b].extention, p + strlen(List[b].name)+1);
            }

            va_end(lst);
        }

        bool tryFocuss()
        {
            isFocussed = sMouse.X>X && sMouse.X<X+_max_width+10 &&
                         sMouse.Y>5+Y && sMouse.Y<5+Y+(_font->Height)*(_checked ? _n+1 : 1);

            return isFocussed;
        }

        void Draw()
        {
            static bool sPressed=true;

            static bool once=true;
            unsigned char dh=_font->Height/2+2;

            const int& _m_x=sMouse.X;
            const int& _m_y=sMouse.Y;

            if(!isVisible) return;

            _drawRect(X,Y+1,X+_max_width+10,Y+_font->Height/2+3,pixel(255,233,236,237));
            _drawRect(X,Y+_font->Height/2+3,X+_max_width+10,Y+_font->Height+4,pixel(255,217,222,225));

            _drawLine(X,Y,X+_max_width+9,Y,pixel(255,105,115,120));
            _drawLine(X,Y+_font->Height+5,X+_max_width+9,Y+_font->Height+5,pixel(255,250,250,250));
            _drawLine(X,Y+_font->Height+6,X+_max_width+9,Y+_font->Height+6,pixel(255,105,115,120));

            _drawLine(X-1,Y+1,X-1,Y+_font->Height+6,pixel(255,105,115,120));
            _drawLine(X+_max_width+9,Y+1,X+_max_width+9,Y+_font->Height+6,pixel(255,105,115,120));

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

                _drawRect(X,Y+_font->Height+7,X+_max_width+9,5+Y+(_font->Height)*(_n+1),pixel(255,255,255,255));

                _drawLine(X,5+Y+(_font->Height)*(_n+1),X+_max_width+9,5+Y+(_font->Height)*(_n+1),pixel(200,150,150,155));
                _drawLine(X-1,Y+_font->Height+5,X-1,5+Y+(_font->Height)*(_n+1),pixel(255,150,150,155));
                _drawLine(X+_max_width+9,Y+_font->Height+5,X+_max_width+9,5+Y+(_font->Height)*(_n+1),pixel(150,150,150,155));

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

            _drawLine(X+_max_width+1,Y+dh-1,X+_max_width+8,Y+dh-1,pixel(255,0,0,0));
            _drawLine(X+_max_width+2,Y+dh,X+_max_width+7,Y+dh,pixel(255,0,0,0));
            _drawLine(X+_max_width+3,Y+dh+1,X+_max_width+6,Y+dh+1,pixel(255,0,0,0));
            _drawLine(X+_max_width+4,Y+dh+2,X+_max_width+5,Y+dh+2,pixel(255,0,0,0));
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

            if(List) delete [] List;

            _n=0;

            _font = 0;
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
            $Font* font;
            pixel color;

            Style()
            {
                font=($Font*)0;
                color=pixel(0,0,0,0);
            }

            Style(pixel color)
            {
                font=($Font*)0;
                this->color=color;
            }

            Style($Font* font,pixel color)
            {
                this->font=font;
                this->color=color;
            }

            void Clr()
            {
                font=($Font*)0;
                color=pixel(0,0,0,0);
            }

            void Set($Font* font,pixel color)
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
        int _l,_h,_dh,_X;

        Style* sText;
        MatchTable<char,Style>* mText;

        pixel cColor;       ///cursor color

        bool Selected;

        int GetMaxFontWidth()
        {
            int res=0;

            unsigned _Lmax = Text.capacity();

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

            unsigned _Lmax = Text.capacity();

            for(int b=0;b<_Lmax;b++)
            {
                if(sText[b].font && sText[b].font->Height>res)
                {
                    res=sText[b].font->Height;
                }
            }

            return res;
        }

        TextBox($Font* font,int x,int y,int width,int height,int Encoding=0,const pixel& DefColor=pixel(0,0,0))
                : GUI(font, x, y, tTEXTBOX)
        {
            _l=_h=_dh=_X=0;

            Width = width;
            Height = height;

            encoding = Encoding;
            color = DefColor;

            unsigned _Lmax = Text.capacity();

            sText=new Style[_Lmax+1];
            mText=(MatchTable<char,Style>*)0;
            //mText=new MatchTable<char,Style>();

            for(int b=0;b<=_Lmax;b++)
            {
                Text[b]=0;
                sText[b].Set(font,color);
            }

            //sText[5].color=pixel(15,155,5);

            int Mw=GetMaxFontWidth();

            if(Width<Mw+_mw) Width=Mw+_mw;

            ///if(Width<_font->chr['m'].W+_mw) Width=_font->chr['m'].W+_mw;

            Selected=false;

            cColor=pixel(0,0,0);        ///Cursor color
        }

        void Set(const char* text)
        {
            if(!text || !text[0]) return;

            int L=strlen(text);
            unsigned _Lmax = Text.capacity();

            if(L>=_Lmax)
            {
                $Font* __f = 0;

                if(sText)
                {
                    __f=sText[0].font;
                    delete [] sText;
                }

                sText=new Style[_Lmax+1];

                sText[0].font=__f;
                sText[0].color=pixel(0,0,0);
            }

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
            int _i,_w;
            int _H=Height/GetMaxFontHeight();///_font->Height;

            if(isFocussed)
            {
                if(sKey.Char || sKey.Key)
                {
                    if(sKey.Key==8 || sKey.Key==46)     ///backspace / delete
                    {
                        if(Selected)
                        {
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
                        //if(_l<strlen(Text))
                        if(Text[_l]) _l++;
                    }
                    else if(sKey.Key==37)       ///<-
                    {
                        if(_l>0) _l--;
                    }
                    else if(sKey.Key==40)       ///down
                    {
                        if(_dh<_h && _h>_H) _dh++;

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

                                    int _L = Text.length();

                                    data = GlobalAlloc(GMEM_MOVEABLE,_L+1);

                                    char* _text=(char*)GlobalLock(data);
                                    memcpy(_text,Text.data(),_L+1);
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

                                        int _L = Text.length();

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
                                                else sText[b].Set(_font,color);
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
                                    else _ts.Set(_font,color);

                                    Style _t;

                                    const char* _f0 = Text.data()+_i;
                                    const char* _f = _f0;
                                    while(*_f) _f++;

                                    _f++;

                                    unsigned _Lmax = Text.capacity();

                                    if(_i + _f - _f0>= _Lmax)
                                    {
                                        _Lmax+=128;

                                        Text.resize(_Lmax);

                                        delete __s;
                                        __s=new Style[_Lmax+1];
                                        memcpy(__s,sText,(_i + _f - _f0+1)*sizeof(Style));

                                        delete [] sText;

                                        sText = __s;
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

                        int _L = Text.length();

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

            const char* ch=Text.data();
            $Font* _font=sText[0].font;

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

            //unsigned char cur=*ch;
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

                if(ch-Text.data()==_l && isFocussed)
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

                        //_l=strlen(Text);

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

                if(sText[(int)(ch-Text.data())].color.a>0) __C=sText[(int)(ch-Text.data())].color;
                if(sText[(int)(ch-Text.data())].font) _font=sText[(int)(ch-Text.data())].font;

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

            if((ch-Text.data()==_l || (h+_font->Height+5)/_font->Height>_H)&& isFocussed)
            {
                DrawLine(x0+1,Y+h,x0+1,Y+h+_cH,cColor);
                _X=x0;
            }

            ///DrawLine(x0+1,h+Y+_dh*_cH+1,x0+1,h+Y+(_dh+1)*_cH+1,pixel(0,0,50));
        }

        ~TextBox()
        {
            delete sText;
            delete mText;
            _l=_h=_dh=_X=0;

            Selected=isFocussed=isVisible=false;

            //tColor=pixel(0,0,0);
            cColor=pixel(0,0,0);
        }
    };
};

#endif // _GUI_H_INCLUDED
