#ifndef FONT_H_INCLUDED
#define FONT_H_INCLUDED

#ifndef $GRAPHICS
    #include "Graphics.h"
#endif

enum
{
    ENC_UNICODE=0,
    ENC_ANSI=1,
    ENC_XAARTA=2
};

struct Char
{
    unsigned char** matrix;
    unsigned char W;
};

struct $Font
{
    Char chr[256];
    unsigned char Height;
};

void LoadFont(const char* file_name,$Font* &font)
{
    int ch1,cl=0,Height;
    unsigned cnt=0;

    if(!font) font=new $Font();

    FILE* f1=fopen(file_name,"rb");

    if(!f1) return;

    Height = getc(f1);

    while(1)
    {
        ch1 = getc(f1);

        font->chr[cnt++].matrix=new unsigned char*[ch1];
        for(unsigned b=0;b<ch1;b++) font->chr[cnt-1].matrix[b]=new unsigned char[Height];

        font->chr[cnt-1].W=ch1;

        for(unsigned char x=0;x<ch1;x++)
        {
            for(unsigned char y=0;y<Height;y++)
            {
                font->chr[cnt-1].matrix[x][y]=255;
            }
        }

        while(cl!=1)
        {
            cl = getc(f1);

            if(feof(f1)) break;
            if(cl==1) break;

            ch1 = getc(f1);

            font->chr[cnt-1].matrix[ch1/16][ch1%16]=(int)cl;

            if(feof(f1)) break;
        }

        cl=0;

        if(cnt-1==255) break;
    }

    font->Height=Height;

    fclose(f1);
}

void LoadFont(unsigned char* font_data, unsigned font_N, $Font* &font)
{
    int ch1,cl=0,Height;
    unsigned cnt=0, n=0;

    if(!font) font=new $Font();

    if(!font_data) return;

    Height = font_data[n++];

    while(1)
    {
        ch1=font_data[n++];

        font->chr[cnt++].matrix=new unsigned char*[ch1];
        for(unsigned b=0;b<ch1;b++) font->chr[cnt-1].matrix[b]=new unsigned char[Height];

        font->chr[cnt-1].W=ch1;

        for(unsigned char x=0;x<ch1;x++)
        {
            for(unsigned char y=0;y<Height;y++)
            {
                font->chr[cnt-1].matrix[x][y]=255;
            }
        }

        while(cl!=1)
        {
            cl=font_data[n++];
            if(n >= font_N) break;
            if(cl==1) break;

            ch1=font_data[n++];

            font->chr[cnt-1].matrix[ch1/16][ch1%16]=(int)cl;

            if(n >= font_N) break;
        }

        cl=0;

        if(cnt-1==255) break;
    }

    font->Height=Height;
}

void LoadSFont(const char* file_name,$Font* &font)
{
    int Height,ch,cnt=0;

    if(!font) font=new $Font();

    FILE* f=fopen(file_name,"rb");

    if(!f) return;

    Height=getc(f);

    while(1)
    {
        ch=getc(f);
        if(ch==-1 || cnt==256) break;

        font->chr[cnt++].matrix=new unsigned char*[ch];
        for(int b=0;b<ch;b++) font->chr[cnt-1].matrix[b]=new unsigned char[Height];

        font->chr[cnt-1].W=ch;

        for(int y=0;y<Height;y++)
        {
            for(int x=0;x<font->chr[cnt-1].W;x++)
            {
                ch=getc(f);

                if(ch<0) return;
                else font->chr[cnt-1].matrix[x][y]=ch;
            }
        }
    }

    font->Height=Height;

    fclose(f);
}

inline int Text_out($Font* font,int X,int Y,const char* ch,const pixel& color=pixel(0,0,0),
                    int Max_Width=-1,bool selected=false,short encoding=0,bool single_line=false)
{
    unsigned char cur=ch[0];

    int x0=X;

    unsigned char p,t=208,sl=0;
    int _x=0;
    int _y=0;
    int h=0;

    if(selected) sl=255;

    unsigned char red=color.r, green=color.g, blue=color.b;

    const char* _ch=ch;

    while(*ch)
    {
        p=c_abs(*ch);

        if(p==13)
        {
            ch++;
            continue;
        }
        else if(p==10)
        {
            h+=font->Height+5;
            x0=X;
            ch++;
            continue;
        }

        if(Max_Width>0 && *(ch+1) && x0-X+font->chr[p].W>Max_Width)
        {
            if(single_line) return x0;
            else
            {
                x0=X;
                h+=font->Height+5;
            }
        }

        if(encoding==ENC_UNICODE)
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
        else if(encoding==ENC_XAARTA)
        {
            _x=_y=0;
            if(!(p=='D' || p=='F' || p=='R' || p=='S' || p=='L')) cur=p;

            if(_ch!=ch && *(ch-1)=='r' && p=='e') _x=-font->chr[p].W/4;

            //printf("%c %d\n",cur,cur);

            if(p=='y' && *(ch+1)=='D') p='Z';
            else if(_ch!=ch && ( (cur>='a' && cur<='z') || cur=='J'))
            {
                if(p=='D' || p=='F' || p=='R' || p=='S' || p=='L')
                {
                        //if(cur==0) cur=*(ch-1);

                        _x=(font->chr[cur].W+font->chr[p].W)/2;

                        if(cur=='y' && p=='D') { ch++; continue;}

                        if(p!='L')
                        {
                            if(cur=='s' || cur=='i' || cur=='j' || cur=='l' || cur=='k' || cur=='J' || (cur>='n' && cur<'t')) _y=font->Height;
                        else if(cur=='h' || cur=='f' || cur=='u' || cur=='x') _y=0.9*font->Height;
                        else _y=3*font->Height/4;

                        if(cur=='c' || cur=='o') _x/=1.2;
                        else if(cur=='d' || cur=='x' || cur=='v') _x*=1.2;
                        else if(cur=='j' && p!='S') _x/=2;
                        else if(cur=='q') _x*=1.25;
                        else if(cur=='u') _x/=1.25;
                        else if(cur=='y') _x*=1.15;
                        else if(cur=='r') _x/=1.1;
                        else if(cur=='e')
                        {
                            if(ch!=_ch+1 && *(ch-2)=='r') _x/=1.2;
                            else _x*=1.1;
                        }
                        else if(cur=='k' && p=='S') _x*=1.1;
                        else if(cur=='n' && p!='S') _x/=1.2;
                        }
                        else
                        {
                            if(cur=='d') _x*=1.14;
                            else if(cur=='e') _x*=1.09;
                            else if(cur=='h' || cur=='m') _x*=1.1;
                        }
                }
                //else cur=0;
            }
        }

        if(c_abs(*ch)<208 || encoding!=ENC_UNICODE)
        {
            for(int x=0;x<font->chr[p].W;x++)
            {
                for(int y=0;y<font->Height;y++)
                {
                    if(font->chr[p].matrix[x][y]!=255)
                    {
                        if(Max_Width<0 || x+x0-X<Max_Width)
                        {
                            PutPixel(x+x0-_x,y+Y-_y+h,pixel(255-font->chr[p].matrix[x][y],abs(sl-red),abs(sl-green),abs(sl-blue)));
                        }
                    }
                }
            }
        }

        if(c_abs(*ch)<208 || encoding!=ENC_UNICODE) x0+=font->chr[p].W;
        else if(encoding==ENC_XAARTA) if(p!='D' && p!='F' && p!='R' && p!='S' && p!='L') x0+=font->chr[p].W;

        //if(Max_Height>0 && h>Max_Height) break;

        ch++;
    }

    return x0;
}

unsigned Text_Width($Font* font,const char* s,char terminate=0,int wt=-1)
{
    unsigned x0=0;

    const char* _s=s;

    while(*s!=terminate && *s!=0 && !(wt>0 && s-_s>=wt))
    {
        if(c_abs(*s)<208 && c_abs(*s)>10) x0+=font->chr[c_abs(*s)].W;
        s++;
    }

    return x0;
}

#endif // FONT_H_INCLUDED
