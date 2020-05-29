#include <string>
#include "_GUI.h"
#include <sstream>

double **temp, **t, **p, a, dx, dy, dt, t_left, t_right, bar;
int x_border, y_border, X0, Y0, crcl_r;
bool GUI_Toggle, P_Toggle, I_Toggle, isPaused;

std::string MeterTemp;
std::stringstream strs;

using namespace _GUI;

$Font* font_12;
GUIGroup* G0;
GUIGroup* Gmtr;
GUIGroup* info;

RECT dskt;

///-----------------------------------------------------------------

///создаёт 1-ю тестовую систему
void test1_create()
{
    const int w = 400;
    const int dy = 20;

    if(isLeftPressed || isRightPressed)
    {
        double **Q, value;

        ///выбор рабочего массива
        if(!P_Toggle && !G0->CursorOn() && GUI_Toggle) Q = temp;
        else Q = p;

        for(int x = MouseX;x<w+MouseX;x++)
        {
            Q[x][MouseY] = t_left;
            Q[x][MouseY+dy] = t_right;
        }
    }
}

///-----------------------------------------------------------------

void create(int x = MouseX, int y = MouseY, int r = 0)
{
    if(isLeftPressed || isRightPressed)
    {
        double **Q = 0, value;

        ///выбор присваеваемой величины
        if(isLeftPressed) value = t_left;
        else value = t_right;

        ///выбор рабочего массива
        if(!G0->CursorOn() && GUI_Toggle)
        {
            if(!P_Toggle) Q = temp;
            else Q = p;
        }

        if(!Q) return;

        ///--------------------------------------------------------

        if(sKey.Key == 'B') ///нагреваем границу - окружность
        {
            r += 20;

            for(int x = MouseX-r; x<=MouseX+r; x++)
            {
                for(int y = MouseY-r; y<=MouseY+r; y++)
                {
                    if(x>0 && y>0 && x<x_border && y<y_border)
                    {
                        unsigned d = (x - MouseX)*(x - MouseX) + (y - MouseY)*(y - MouseY);

                        if(abs(d - (r-10)*(r-10))<=20*20)
                        {
                            Q[x][y] = value;
                        }
                    }
                }
            }
        }
        else if(sKey.Key == 'A')    ///нагреваем область - круг
        {

            for(int x = MouseX-r; x<=MouseX+r; x++)
            {
                for(int y = MouseY-r; y<=MouseY+r; y++)
                {
                    if(x>0 && y>0 && x<x_border && y<y_border)
                    {
                        if((x - MouseX)*(x - MouseX) + (y - MouseY)*(y - MouseY) <= r*r)
                        {
                            Q[x][y] = value;
                        }
                    }
                }
            }
        }
        else if(sKey.Key == 'E')    ///"стираем" постоянные источники
        {

            for(int x = MouseX-r; x<=MouseX+r; x++)
            {
                for(int y = MouseY-r; y<=MouseY+r; y++)
                {
                    if(x>0 && y>0 && x<x_border && y<y_border)
                    {
                        if((x - MouseX)*(x - MouseX) + (y - MouseY)*(y - MouseY) <= r*r)
                        {
                            p[x][y] = 0./0;     ///причём стираем даже если !P_Toggle
                        }
                    }
                }
            }
        }
        else Q[x][y] = value;   ///"нагреваем" точку
    }
}

double T0 = 100;

void s_del()
{
    for(int x = 0; x < dskt.right; x++)
    {
        for(int y = 0; y < dskt.bottom; y++)
        {
            temp[x][y] = T0;
            t[x][y] = T0;
            p[x][y] = 0./0;
        }
    }
}

void s_init()
{
    t = new double*[dskt.right];
    temp = new double*[dskt.right];
    p = new double*[dskt.right];

    for(int x = 0; x < dskt.right; x++)
    {
        t[x] = new double[dskt.bottom];
        temp[x] = new double[dskt.bottom];
        p[x] = new double[dskt.bottom];
        for(int y = 0; y < dskt.bottom; y++)
        {
            t[x][y] = T0;
            temp[x][y] = T0;
            p[x][y] = 0./0;
        }
    }
}

pixel t_toColor(double temperature, double tmax)
{
    if(temperature > tmax)
        temperature = tmax;

    if(isnan(temperature))
        return pixel(255, 255, 255);

    if(fabs(temperature)<1e-10)
        return pixel(0, 0, 255);
    if(temperature == tmax)
        return pixel(255, 0, 0);

    double a = temperature / tmax;

    if(a < 0.25)
    {
        return pixel(0, 255 * 4 * a, 255);
    }
    else
    {
        if(a < 0.5)
        {
            return pixel(0, 255, 255 * 2 * (1 - 2 * a));
        }
        else
        {
            if(a < 0.75)
            {
                return pixel(255 * 4 * (a - 0.5), 255, 0);
            }
            else
            {
                return pixel(255, 255 * 4 * (1 - a), 0);
            }
        }
    }
}

$EventReceiver_Begin

    $EventReceiver_Start

        $Window_Size
            x_border = AppWidth;
            y_border = AppHeight;
        $end

    $Mouse_LeftClick
        G0->tryFocuss();

        if(G0->Get("Update")->isFocussed)
        {
            dx = strtod(G0->Get("dx")->Text.data(), 0);
            dy = strtod(G0->Get("dy")->Text.data(), 0);
            dt = strtod(G0->Get("dt")->Text.data(), 0);
            a = strtod(G0->Get("a")->Text.data(), 0);
            t_left = strtod(G0->Get("t_left")->Text.data(), 0);
            t_right = strtod(G0->Get("t_right")->Text.data(), 0);
            bar = strtod(G0->Get("bar")->Text.data(), 0);
            crcl_r = strtod(G0->Get("rad")->Text.data(), 0);
        }

        if(G0->Get("Reset")->isFocussed)
        {
            T0 = strtod(G0->Get("srfc")->Text.data(), 0);
            s_del();
        }

        ///--------------------------------------------------------

        ///<!-- test1 -->

        if(sKey.Key == '1')
        {
            test1_create();
        }
        else create(MouseX, MouseY, crcl_r);

        X0 = MouseX;
        Y0 = MouseY;

    $end

    $Mouse_LeftUp
        G0->Get("Update")->isFocussed = false;
        G0->Get("Reset")->isFocussed = false;
    $end

    $Mouse_RightClick

        create(MouseX, MouseY, crcl_r);

        X0 = MouseX;
        Y0 = MouseY;

    $end

    $Mouse_RightUp $end

    $Mouse_Move
        if(isLeftPressed || isRightPressed)
        {
            if(abs(X0 - MouseX) > abs(Y0 - MouseY))
            {
                double k = 1.0*(MouseY - Y0)/(MouseX - X0);

                int s = (X0 > MouseX) ? -1 : 1;

                for(int x = X0;x != MouseX; x += s)
                {
                    create(x, k*(x - X0) + Y0, crcl_r);
                }
            }
            else
            {
                double k = 1.0*(MouseX - X0)/(MouseY - Y0);

                int s = (Y0 > MouseY) ? -1 : 1;

                for(int y = Y0;y!= MouseY; y += s)
                {
                    create(k*(y - Y0) + X0, y, crcl_r);
                }
            }

            X0 = MouseX;
            Y0 = MouseY;

            //create();
        }
    $end

    $KeyBoard_Char
        {
            ///сохраняем код символа
            char ch = sKey.Key;

            sKey.Key = 0;
            G0->KeyPress();

            ///и восстанавливаем его
            ///(нужен при обработке сообщения $Mouse_LeftClick)
            sKey.Key = ch;
        }
    $end

    $KeyBoard_Down
        if(sKey.Key == 27)
            exit(0);

        if(sKey.Key == 13)
        {
            dx = strtod(G0->Get("dx")->Text.data(), 0);
            dy = strtod(G0->Get("dy")->Text.data(), 0);
            dt = strtod(G0->Get("dt")->Text.data(), 0);
            a = strtod(G0->Get("a")->Text.data(), 0);
            t_left = strtod(G0->Get("t_left")->Text.data(), 0);
            t_right = strtod(G0->Get("t_right")->Text.data(), 0);
            bar = strtod(G0->Get("bar")->Text.data(), 0);
            crcl_r = strtod(G0->Get("rad")->Text.data(), 0);
        }

        sKey.Char = 0;
        G0->KeyPress();

        if(sKey.Key == 17)
            GUI_Toggle = !GUI_Toggle;

        if(sKey.Key == 80)  ///P
        {
            P_Toggle = !P_Toggle;
            if(P_Toggle)
                G0->Get("pData")->Text = "P to create persistent sources [ON]";
            else
                G0->Get("pData")->Text = "P to create persistent sources [OFF]";
        }

        if(sKey.Key == 73)  ///I
        {
            I_Toggle = !I_Toggle;
        }

        if(sKey.Key == 32)
        {
            isPaused = !isPaused;
            if(isPaused)
                G0->Get("Label14")->Text = "SPACE to pause [ON]";
            else
                G0->Get("Label14")->Text = "SPACE to pause [OFF]";
        }
    $end

    $KeyBoard_Up $end

    $Window_Paint

        Clear();

        if(!isPaused)
        {
            for(int x = 1; x < x_border - 1; x++)
                for(int y = 1; y < y_border - 1; y++)
                {
                    if(!isnan(p[x][y]))
                        temp[x][y] = p[x][y];

                    t[x][y] = temp[x][y] + a * a * dt * ((temp[x + 1][y] - 2 * temp[x][y] + temp[x - 1][y]) / (dx * dx) +
                                                         (temp[x][y + 1] - 2 * temp[x][y] + temp[x][y - 1]) / (dy * dy));
                }
        }

        for(int x = 0; x < x_border; x++)
            for(int y = 0; y < y_border; y++)
            {
                PutPixel(x, y, t_toColor(temp[x][y], bar));
            }

        swap(t, temp);

        ///-------------------------------------------------------------

        strs.str(std::string());
        strs << "[" << MouseX << "][ " << MouseY << "] = " << temp[MouseX][MouseY];
        MeterTemp = strs.str();

        G0->Get("MeterData")->Text = MeterTemp;
        Gmtr->Get("Label1")->Text = MeterTemp;

        if(!I_Toggle)
        {
            if(GUI_Toggle)
                G0->Draw();
            else
                Gmtr->Draw();
        }
        else
            info->Draw();

        $RepaintBackground

    $end

    $Window_Close
        exit(0);
    $end

$EventReceiver_End

int main(int argc, char** argv)
{
    GetWindowRect(GetDesktopWindow(), &dskt);

    x_border = 700;
    y_border = 700;
    crcl_r = 50;

    dx = dy = 0.1;
    dt = 0.001;
    a = 1;
    t_left = 400;
    t_right = 0;
    bar = 200;
    GUI_Toggle = true;

    std::string path = argv[0];
    path.erase(path.find_last_of('\\')+1) += "Font_console_12.cff";
    LoadFont(path.data(), font_12);

    ///GUI initialization
    {
        G0 = new GUIGroup();

        G0->Add(new Label(font_12, 10, 20, "Parameters"),
            "Label1");
        G0->Add(new Label(font_12, 20, 0, "0"),
                "MeterData",
                sDown(0, 14));
        G0->Add(new Label(font_12, 20, 0, "Delta X:"),
                "Label4",
                sDown(0, 9));
        G0->Add(new Edit(font_12, 0, 0, 60),
                "dx",
                sRight(6, -2))->P->Set("0.1");
        G0->Add(new Label(font_12, 20, 0, "Delta Y:"),
                "Label5",
                sDown(0, 9));
        G0->Add(new Edit(font_12, 0, 0, 60),
                "dy",
                sRight(6, -2))->P->Set("0.1");
        G0->Add(new Label(font_12, 20, 0, "Delta T:"),
                "Label6",
                sDown(0, 9));
        G0->Add(new Edit(font_12, 0, 0, 60),
                "dt",
                sRight(6, -2))->P->Set("0.001");
        G0->Add(new Label(font_12, 20, 0, "Conductivity:"),
                "Label7",
                sDown(0, 9));
        G0->Add(new Edit(font_12, 0, 0, 60),
                "a",
                sRight(6, -2))->P->Set("1");
        G0->Add(new Label(font_12, 20, 0, "Left Click Set:"),
                "Label8",
                sDown(0, 9));
        G0->Add(new Edit(font_12, 0, 0, 60),
                "t_left",
                sRight(6, -2))->P->Set("400");
        G0->Add(new Label(font_12, 20, 0, "Right Click Set:"),
                "Label9",
                sDown(0, 9));
        G0->Add(new Edit(font_12, 0, 0, 60),
                "t_right",
                sRight(6, -2))->P->Set("0");
        G0->Add(new Label(font_12, 20, 0, "Visual Meter Peak:"),
                "Label10",
                sDown(0, 9));
        G0->Add(new Edit(font_12, 0, 0, 60),
                "bar",
                sRight(6, -2))->P->Set("200");
        G0->Add(new Label(font_12, 20, 0, "Surface Default:"),
                "Label15",
                sDown(0, 9));
        G0->Add(new Edit(font_12, 0, 0, 60),
                "srfc",
                sRight(6, -2))->P->Set("100");
        G0->Add(new Label(font_12, 20, 0, "Circle radius:"),
                "Label19",
                sDown(0, 9));
        G0->Add(new Edit(font_12, 0, 0, 60),
                "rad",
                sRight(6, -2))->P->Set("50");
        G0->Add(new Button(font_12,  0,  0, "Update", 60),
                "Update",
                sDown(0, 10))
        ->P->AllignTo(G0->Get("Label1"), sRIGHT);
        G0->Add(new Button(font_12,  0,  0, "Reset", 60),
                "Reset",
                sDown(0, 10))
        ->P->AllignTo(G0->Get("Label1"), sRIGHT);
        G0->Add(new Label(font_12, 20, 0, "Hold A to set round area temp"),
                "Label16",
                sDown(0, 9));
        G0->Add(new Label(font_12, 20, 0, "Hold B to set temp around the area"),
                "Label17",
                sDown(0, 9));
        G0->Add(new Label(font_12, 20, 0, "Hold E to remove persistent sources"),
                "Label18",
                sDown(0, 9));
        G0->Add(new Label(font_12, 20, 0, "CTRL to toggle this"),
                "Label11",
                sDown(0, 9));
        G0->Add(new Label(font_12, 20, 0, "ESC to exit"),
                "Label12",
                sDown(0, 9));
        G0->Add(new Label(font_12, 20, 0, "SPACE to pause [OFF]"),
                "Label14",
                sDown(0, 9));
        G0->Add(new Label(font_12, 20, 0, "P to create persistent sources [OFF]"),
                "pData",
                sDown(0, 9));
        G0->Add(new Label(font_12, 20, 0, "I to show info"),
                "Label13",
                sDown(0, 9));
        Gmtr = new GUIGroup();
        Gmtr->Add(new Label(font_12, 10, 20, "0"),
                  "Label1");
        Gmtr->Add(new Label(font_12, 20, 0, "CTRL to toggle parameters"),
                  "label2",
                  sDown(0, 14));
        info = new GUIGroup();
        info->Add(new Label(font_12, 20, 0, "thermd - Two-dimensional heat evolution modeler."),
                  "Line1",
                  sDown(0, 14));
        info->Add(new Label(font_12, 20, 0, "Created by Igor Gunin, 2018."),
                  "Line2",
                  sDown(0, 14));
        info->Add(new Label(font_12, 20, 0, "Non-commercial open-source application."),
                  "Line3",
                  sDown(0, 14));

    }

    s_init();

    createDevice(x_border, y_border,pixel(255,255,255), L"thermd - Two-dimensional real-time thermodynamics emulator");
    run();

    return 0;
}
