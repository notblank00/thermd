#include "Graphics.h"
#include <string>
#include "_GUI.h"
#include <sstream>
double **temp, **t, **p, a, dx, dy, dt, t_left, t_right, bar;
int x_border, y_border;
bool GUI_Toggle, P_Toggle, I_Toggle, isPaused;
std::string MeterTemp;
std::stringstream strs;
using namespace _GUI;
$Font* font_12;
GUIGroup* G0;
GUIGroup* Gmtr;
GUIGroup* info;
RECT dskt;
void s_del()
{
    for(int x = 0; x < dskt.right; x++)
    {
        delete t[x], temp[x];
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
            t[x][y] = 0;
            temp[x][y] = 0;
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
}
if(G0->Get("Reset")->isFocussed)
{
    s_del();
    s_init();
}
$end
$Mouse_LeftUp
G0->Get("Update")->isFocussed = false;
G0->Get("Reset")->isFocussed = false;
$end
$Mouse_RightClick $end
$Mouse_RightUp $end
$Mouse_Move
if(!P_Toggle)
{
    if((isLeftPressed) && (!(G0->CursorOn()) || (!GUI_Toggle)))
        temp[MouseX][MouseY] = t_left;
    if((isRightPressed) && (!(G0->CursorOn()) || (!GUI_Toggle)))
        temp[MouseX][MouseY] = t_right;
}
else
{
    if((isLeftPressed) && (!(G0->CursorOn()) || (!GUI_Toggle)))
        p[MouseX][MouseY] = t_left;
    if((isRightPressed) && (!(G0->CursorOn()) || (!GUI_Toggle)))
        p[MouseX][MouseY] = t_right;
}
$end
$KeyBoard_Char
sKey.Key = 0;
G0->KeyPress();
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
}
sKey.Char = 0;
G0->KeyPress();
if(sKey.Key == 17)
    GUI_Toggle = !GUI_Toggle;
if(sKey.Key == 80)
{
    P_Toggle = !P_Toggle;
    if(P_Toggle)
        G0->Get("pData")->Text = "P to create persistent sources [ON]";
    else
        G0->Get("pData")->Text = "P to create persistent sources [OFF]";
}
if(sKey.Key == 73)
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
        if(!isPaused)
        {
            temp[x][y] = t[x][y];
        }
    }
$end
$Window_Close
exit(0);
$end
$EventReceiver_End
int main(int argc, char** argv)
{
    GetWindowRect(GetDesktopWindow(), &dskt);
    x_border = 500;
    y_border = 500;
    dx = dy = 1;
    dt = 0.1;
    a = 1;
    t_left = 5000;
    t_right = 0;
    bar = 200;
    GUI_Toggle = true;
    std::string path = argv[0];
    path.erase(path.find_last_of('\\')+1) += "Font_console_12.cff";
    LoadFont(path.data(), font_12);
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
            sRight(6, -2))->P->Set("1");
    G0->Add(new Label(font_12, 20, 0, "Delta Y:"),
            "Label5",
            sDown(0, 9));
    G0->Add(new Edit(font_12, 0, 0, 60),
            "dy",
            sRight(6, -2))->P->Set("1");
    G0->Add(new Label(font_12, 20, 0, "Delta T:"),
            "Label6",
            sDown(0, 9));
    G0->Add(new Edit(font_12, 0, 0, 60),
            "dt",
            sRight(6, -2))->P->Set("0.1");
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
            sRight(6, -2))->P->Set("5000");
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
    G0->Add(new Button(font_12,  0,  0, "Update", 60),
            "Update",
            sDown(0, 10))
    ->P->AllignTo(G0->Get("Label1"), sRIGHT);
    G0->Add(new Button(font_12,  0,  0, "Reset", 60),
            "Reset",
            sDown(0, 10))
    ->P->AllignTo(G0->Get("Label1"), sRIGHT);
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
    s_init();
    createDevice(x_border, y_border,pixel(255,255,255), L"thermd - Two-dimensional real-time thermodynamics emulator");
    run();
    return 0;
}
