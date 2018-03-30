#ifndef TREE_H_INCLUDED
#define TREE_H_INCLUDED

#ifndef $extend
    #define $extend(A,n,N,type); {type* _T=new type[N]; memcpy(_T,A,sizeof(type)*n); delete [] A; A=_T;}
#endif

template<class Key, class Value>
struct Trie
{
    struct Edge
    {
        Key ch;
        Trie* T;

        Edge(Key Ch, Value value)
        {
            ch = Ch;
            T = new Trie(value, 0);
        }

        ~Edge()
        {
            if(T)
            {
                delete T;
                T = 0;
            }
        }
    };

    Value value;

    Edge** data;
    int N, N_MAX;

    Trie(Value value, int n_max = 8)
    {
        if(n_max<1)
        {
            N_MAX = 0;
            data = 0;
        }
        else
        {
            data = new Edge*[N_MAX = n_max];
            for(int k=0;k<N_MAX;k++) data[k] = 0;
        }

        N = 0;

        this->value = value;
    }

    ~Trie()
    {
        if(data)
        {
            for(int k=0;k<N_MAX;k++)
            {
                if(data[k]) delete data[k];
            }

            delete [] data;
            data = 0;
        }

        N = 0;
    }

    Edge* AddEdge(Key ch, Value value = 0, int idx = -1)
    {
        if(!data)
        {
            if(N_MAX<1) N_MAX = 1;

            data = new Edge*[N_MAX];
            for(int k=0; k<N_MAX; k++) data[k] = 0;

            N = 0;
        }

        if(N == N_MAX)
        {
            N_MAX += 8;
            $extend(data, N, N_MAX, Edge*);

            for(int k=N; k<N_MAX; k++) data[k] = 0;
        }

        ///--------------------------------

        if(idx < 0 || idx > N)
        {
            int k, a = 0, b = N-1;

            while(a <= b)
            {
                k = a + (b-a)/2;

                if(ch == data[k]->ch)
                {
                    return data[k];
                }
                else
                {
                    if(ch > data[k]->ch) a = k+1;
                    else b = k-1;
                }
            }

            ///not found
            if(a>b)
            {
                b = a;
                while(data[b]) b++;
                data[b+1]=0;
                while(b>a) data[b--] = data[b-1];

                data[a] = new Edge(ch, value);
                N++;

                return data[a];
            }
        }
        else
        {
            int b = idx;
            while(data[b]) b++;
            data[b+1]=0;
            while(b>idx) data[b--] = data[b-1];

            data[idx] = new Edge(ch, value);
            N++;

            return data[idx];
        }

        /*if(!data[N]) data[N] = new Edge(ch, value);
        else
        {
            data[N]->ch = ch;
            if(data[N]->T) data[N]->T->value = value;
        }

        N++;

        return data[N-1];*/
    }

    void AddKey(Key* key, Value value)
    {
        if(!key) return;

        Trie* Tr = this;

        while(*key)
        {
            if(!Tr) Tr = new Trie(0, 0);

            if(!Tr->data) Tr = Tr->AddEdge(*key)->T;
            else
            {
                if(!Tr->N) Tr = Tr->AddEdge(*key)->T;
                else
                {
                    /*for(int k=0;k<Tr->N;k++)
                    {
                        if(Tr->data[k]->ch == *key)
                        {
                            Tr = Tr->data[k]->T;
                            break;
                        }

                        if(k == Tr->N-1) Tr = Tr->AddEdge(*key)->T;
                    }*/

                    int k, a = 0, b = Tr->N-1;

                    while(a <= b)
                    {
                        k = a + (b-a)/2;

                        if(*key == Tr->data[k]->ch)
                        {
                            Tr = Tr->data[k]->T;
                            break;
                        }
                        else
                        {
                            if(*key > Tr->data[k]->ch) a = k+1;
                            else b = k-1;
                        }
                    }

                    ///not found
                    if(a>b) Tr = Tr->AddEdge(*key, 0, a)->T;
                }
            }

            key++;
        }

        Tr->value = value;
    }

    Trie* FindKey(Key* key)
    {
        if(!key) return 0;

        Trie* Tr = this;

        while(*key)
        {
            if(!Tr || !Tr->data || !Tr->N) return 0;
            else
            {
                /*for(int k=0;k<Tr->N;k++)
                {
                    if(Tr->data[k]->ch == *key)
                    {
                        Tr = Tr->data[k]->T;
                        break;
                    }

                    if(k == Tr->N-1) return 0;
                }*/

                int k, a = 0, b = Tr->N-1;

                while(a <= b)
                {
                    k = a + (b-a)/2;

                    if(*key == Tr->data[k]->ch)
                    {
                        Tr = Tr->data[k]->T;
                        break;
                    }
                    else
                    {
                        if(*key > Tr->data[k]->ch) a = k+1;
                        else b = k-1;
                    }
                }

                ///not found
                if(a>b) return 0;
            }

            key++;
        }

        return Tr;
    }

    #if defined($GRAPHICS) && defined(FONT_H_INCLUDED)

    void Draw(Font* font, int X, int Y, int dr = 150)
    {
        static int _depth = 0;
        const int R = 15;
        const int dY = 100;

        int dR = (dr<0) ? R : dr;
        int dL = 2*R + dR;

        static char s[32];

        if(value) FillCircle(X,Y,R, pixel(100,0,0,255));
        DrawDDACircle(X,Y,R, pixel(0,0,255));

        _depth++;

        /*sprintf(s, "%d", _depth);

        Text_out(font,
                 X - Text_Width(font,s)/2,
                 Y - font->Height/2,
                 s);*/

        if(data)
        {
            int X0 = X - (N-1)*dL/2;

            for(int k=0;k<N;k++)
            {
                if(data[k])
                {
                    int dx = dL*(2*k-N+1)/2;
                    int dy = dY/(N==1 ? 2 : 1) + 3*(_depth-1)*floor(N*N*N/8.);

                    double l = sqrt(dx*dx + dy*dy);
                    double p = R/l;

                    DrawWuLine(X+p*dx, Y+p*dy, X0 + k*dL -p*dx, Y+dy-p*dy, pixel(50, 0,0,150));

                    sprintf(s, "%c", data[k]->ch);
                    Text_out(font,
                             X + dx/2 - font->chr[c_abs(s[0])].W/2,
                             Y + dy/2-font->Height/2, s);

                    if(data[k]->T) data[k]->T->Draw(font,
                                                    X0+k*dL,
                                                    Y + dy,
                                                    round(1.*dR/20 + 2*R));
                }
            }
        }

        _depth--;
    }

    #endif

    void Print()
    {
        static int _depth = 0;

        for(int q=0;q<_depth;q++) putchar(32);
        printf("[value: %d| data: %d| N: %d]\n",value, data, N);

        if(data)
        {
            for(int k=0;k<N_MAX;k++)
            {
                if(data[k])
                {
                    for(int q=0;q<_depth;q++) putchar(32);
                    printf("edge: %c| T: %d\n",data[k]->ch, data[k]->T);

                    if(data[k]->T)
                    {
                        putchar(10);

                        _depth++;
                        data[k]->T->Print();
                        _depth--;
                    }
                }
            }

            putchar(10);
        }
    }
};

#endif // TREE_H_INCLUDED
