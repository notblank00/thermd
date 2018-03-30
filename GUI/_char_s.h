#ifndef _CHAR_S_H_INCLUDED
#define _CHAR_S_H_INCLUDED

#define sEND 0
#define sBEG 1

///char* functions:

///     unsigned lng(char* s)

///     void copy(const char* a,char* b)

///     void overwrite(const char* from,char* to,int index)

///     char UpCase(char a)

///     bool eql(char* a,char* b)

///     bool Eql(char* a,char* b)

///     char* StrSum(char* a,char* b)

///     char* FileExt(char* s)

///     char* FileBaseName(char* s)

///     void set(char* s,const char* res,char terminate=0)

///     void add(char* s,char c)

///     void reduce(char* s)

///     void w_reduce(char* s ,char t,bool delDot=false)

///     char* W_reduce(char* s ,char t)

///     char* del_reduce(char* s,char t)

///     void t_reduce(char* s,char t)

///     void s_add(char* s,const char* res)

///     void clear(char* s)

///     bool IsNumeric(char a)

///     int StrToInt(char* s)

///     double StrToFloat(char* s)

///     char* IntToStr(int a)

///     void expandString(char* s,char* varStringName,char* baseCharName="__c")

///     void Rus(char* s)

unsigned inline lng(char* s)
{
    unsigned n=0;
    while(s[n]) n++;
    return n;
}

void inline copy(const char* a,char* b)
{
    while(*b++=*a++);
}

void overwrite(const char* from,char* to,int index)
{
    while(--index>0) from++;
    while(*from) *to++=*from++;

    while(*to) *to++=NULL;
}

inline char UpCase(char a)
{
    return (a>='a' && a<='z') ? a-32 : a;
}

inline char LowCase(char a)
{
    return (a>='A' && a<='Z') ? a+32 : a;
}

bool eql(char* a,char* b)
{
    if(lng(a)!=lng(b)) return false;
    else while(*a) if(*a++!=*b++) return false;

    return true;
}

bool Eql(char* a,char* b)
{
    if(lng(a)!=lng(b)) return false;
    else while(*a) if(UpCase(*a++) != UpCase(*b++)) return false;

    return true;
}

char* StrSum(char* a,char* b)
{
    char* c = new char[lng(a)+lng(b)+1];
    char* res=c;

    while(*c++=*a++);
    c--;
    while(*c++=*b++);

    return res;
}

char* FileExt(char* s)
{
    char* _s=s;

    while(*s) s++;
    while(*s!='.' && s!=_s) s--;

    if(s==_s) return "";
    else return s+1;
}

char* FileBaseName(char* s)
{
    char* _s=s;

    while(*s) s++;

    while(*s!='\\' && s>=_s) s--;

    return s+1;
}

void set(char* s,const char* res,char terminate=0)
{
    while(*res!=terminate && *res!=NULL) *s++=*res++;
    while(*s) *s++=NULL;
}

void add(char* s,char c)
{
    while(*s) s++;
    *s++=c;
    while(*s) *s++=NULL;
}

void reduce(char* s)
{
    if(*s)
    {
        while(*s) s++;
        --s;
        while(*s) *s++=NULL;
    }
}

void w_reduce(char* s ,char t,bool delDot=false)
{
    while(*s) s++;

    s--;

    if(*s!=t) return;

    while(*s==t) *s--=0;

    if(delDot && (*s=='.' || *s==',')) *s--=0;
}

inline char* W_reduce(char* s ,char t)
{
    while(*s) s++;

    s--;

    while(*s==t) *s--=0;

    return s;
}

inline char* del_reduce(char* s,char t)
{
    while(*s==t) s++;
    return s;
}

void t_reduce(char* s,char t)
{
    while(*s) s++;

    while(*s!=t) *s--=NULL;
}

void s_add(char* s,const char* res)
{
    while(*s) s++;
    while(*res) *s++=*res++;

    while(*s) *s++=NULL;
}

inline void clear(char* s)
{
    while(*s) *s++=NULL;
}

#define IsNumeric(a) ((a>='0' && a<='9') || a=='.' || a=='+' || a=='-' || a=='e' || a=='E')
#define IsOperator(a) (a=='+' || a=='-' || a=='*' || a=='/')
#define IsAriphm(a) (a=='+' || a=='-' || a=='*' || a=='/' || (a>='0' && a<='9') || a=='.')

inline int StrToInt(char* s)
{
    int a=0;
    int _n=0;

    if(s[0]=='-' || s[0]=='+') _n++;

    while(s[_n]!=0)
    {
        if(s[_n]>='0' && s[_n]<='9') a=a*10+(int)(s[_n]-48);
        else
        {
            if(s[0]=='-') a=-a;
            return a;
        }
        _n++;
    }

    if(s[0]=='-') return -a;
    else return a;
}

double StrToFloat(char* s)
{
    double res=0;
    double sgn=1;
    bool _sgn=false;
    bool _dot=false;
    int _10=0;

    if(*s==' ') while(*s==' ') s++;

    if(*s=='-')
    {
        sgn=-1;
        _sgn=true;
        s++;
    }
    else if(*s=='+') s++;

    char* _o=s;

    while(*s)
    {
        if(*s==',' || *s=='.' || *s=='e' || *s=='E')
        {
            if(*s=='.' || *s==',') _dot=true;
            break;
        }

        if(*s>='0' && *s<='9')
        {
            res*=10;
            res+=((*s)-48);
        }

        s++;
    }

    sgn*=res;
    res=0;

    if(*s=='.' || *s==',' || *s=='e' || *s=='E')
    {
        while(*s && *s!='e' && *s!='E') s++;
        if(*s=='e' || *s=='E') _10=StrToInt(s+1);
        s--;

        if(_dot)
        {
            while(*s!='.' && *s!=',' && s>_o)
            {
                if(*s>='0' && *s<='9')
                {
                    res+=((*s)-48);
                    res*=0.1;

                }
                s--;
            }
        }
    }

    res=(sgn>=0) ? sgn+res : sgn-res;

    if(_10>=1) for(int b=0;b<_10;b++) res*=10;
    else if(_10<1) for(int b=0;b<-_10;b++) res/=10;

    if(_sgn && res>0) res=-res;

    return res;
}

char* IntToStr(int a)
{
    static char s[16];

    if(a==0)
    {
        s[0]='0';
        s[1]=0;

        return s;
    }

    int sgn=(a>=0) ? 0 : 1;

    if(a<0) a=-a;

    int _a=a;

    int n=1;

    while(_a>0)
    {
        n++;
        _a/=10;
    }

    //char *s=new char[--n+sgn];

    n--;
    s[sgn+n--]=0;

    while(a>0)
    {
        s[sgn+n--]=(char) (a%10+48);
        a/=10;
    }

    if(sgn==1) s[0]='-';

    return s;
}

#ifdef _IO_H_INCLUDED

#ifndef abs()
    #define abs(x) (x<0 ? -(x) : (x))
#endif

void expandString(char* s,char* varStringName,char* baseCharName="__c")
{
    _io Rw;

    char ch=s[0];
    int n=lng(s);

    Rw << "char " << baseCharName << "='" << s[0] << "';" << endl << endl;
    Rw << "add(" << varStringName << ',' << baseCharName << ");" << endl;

    for(int i=1;i<n;i++)
    {
        if(s[i]!=ch)
        {
            if(s[i]-ch==1) Rw << baseCharName << "++;" << endl;
            else if(s[i]-ch==-1) Rw << baseCharName << "--;" << endl;
            else Rw << baseCharName << (s[i]-ch>0 ? '+' : '-') << '=' << abs(s[i]-ch) << ';' << endl;

            ch=s[i];
        }

        Rw << "add(" << varStringName << ',' << baseCharName << ");" << endl;
    }
}

#endif

void Rus(char* s)
{
    unsigned short p,ch;

    while(*s)
    {
        p=(int) (unsigned char) *s++;
        if(p>207) ch=p;

        if(p<208)
        {
            if(p>127 && p<144)
            {
                if(p==129 && ch==208) p=240;
                else p+=96;
            }
            else if(p>143 && p<176)
            {
                if(p==145 && ch==209) p=241;
                else p-=16;
            }
            else if(p>175 && p<192) p-=16;

            putchar(p);
        }
    }
}

#endif // _CHAR_S_H_INCLUDED
