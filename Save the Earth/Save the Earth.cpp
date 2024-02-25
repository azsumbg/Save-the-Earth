#include "framework.h"
#include "Save the Earth.h"
#include <mmsystem.h>
#include <d2d1.h>
#include <dwrite.h>
#include "d2bmploader.h"
#include "ErrH.h"
#include "FCheck.h"
#include "Engine.h"
#include <vector>
#include <fstream>
#include <ctime>
#include <chrono>

#pragma comment (lib,"winmm.lib")
#pragma comment (lib,"d2d1.lib")
#pragma comment (lib,"dwrite.lib")
#pragma comment (lib,"d2bmploader.lib")
#pragma comment (lib,"errh.lib")
#pragma comment (lib,"fcheck.lib")
#pragma comment (lib,"engine.lib")

#define bWinClassName L"BURNiT"

#define mNew 1001
#define mLvl 1002
#define mExit 1003
#define mSave 1004
#define mLoad 1005
#define mHoF 1006

#define no_record 2001
#define first_record 2002
#define record 2003

#define tmp_file ".\\res\\data\temp.dat"
#define Ltmp_file L".\\res\\data\temp.dat"
#define sound_file L".\\res\\snd\\main.wav"
#define record_file L".\\res\\data\\record.dat"
#define save_file L".\\res\\data\\save.dat"
#define help_file L".\\res\\data\\help.dat"

/////////////////////////////////////////////////

WNDCLASS bWin = { 0 };
HINSTANCE bIns = nullptr;
HICON mainIcon = nullptr;
HCURSOR mainCursor = nullptr;
HCURSOR outCursor = nullptr;
HWND bHwnd = nullptr;
MSG bMsg = { 0 };
BOOL bRet = 0;
PAINTSTRUCT bPaint = { 0 };
HDC PaintDC = nullptr;
HMENU bBar = nullptr;
HMENU bMain = nullptr;
HMENU bStore = nullptr;
UINT bTimer = 0;
POINT cur_pos = { 0,0 };

D2D1_RECT_F b1Rect = { 0, 0, 190.0f, 50.0f };
D2D1_RECT_F b2Rect = { 210, 0, 400.0f, 50.0f };
D2D1_RECT_F b3Rect = { 420, 0, scr_width, 50.0f };

bool pause = false;
bool in_client = true;
bool show_help = false;
bool sound = true;
bool name_set = false;
bool b1_hglt = false;
bool b2_hglt = false;
bool b3_hglt = false;

int score = 0;
int level = 1;
wchar_t current_player[16] = L"UNKNOWN HERO";
float cl_width = 0;
float cl_height = 0;
int start_screen_col = 0;
int minutes = 0;
int seconds = 180;
CELL ScreenGrid[6][4];
CELL Grid[24][4];

//////////////////////////////////////////////////////////////

ID2D1Factory* iFactory = nullptr;
ID2D1HwndRenderTarget* Draw = nullptr;

IDWriteFactory* iWriteFactory = nullptr;
IDWriteTextFormat* nrmText = nullptr;
IDWriteTextFormat* bigText = nullptr;

ID2D1RadialGradientBrush* ButBckgBrush = nullptr;
ID2D1LinearGradientBrush* FieldBckBrush = nullptr;
ID2D1SolidColorBrush* TxtBrush = nullptr;
ID2D1SolidColorBrush* HgltBrush = nullptr;
ID2D1SolidColorBrush* InactBrush = nullptr;

ID2D1Bitmap* bmpBrick = nullptr;
ID2D1Bitmap* bmpBullet = nullptr;
ID2D1Bitmap* bmpCloud1 = nullptr;
ID2D1Bitmap* bmpCloud2 = nullptr;
ID2D1Bitmap* bmpField = nullptr;
ID2D1Bitmap* bmpGold = nullptr;
ID2D1Bitmap* bmpKilled = nullptr;
ID2D1Bitmap* bmpLMonst1 = nullptr;
ID2D1Bitmap* bmpRMonst1 = nullptr;
ID2D1Bitmap* bmpLMonst2 = nullptr;
ID2D1Bitmap* bmpRMonst2 = nullptr;
ID2D1Bitmap* bmpLMonst3 = nullptr;
ID2D1Bitmap* bmpRMonst3 = nullptr;

ID2D1Bitmap* bmpHeroL[6] = { nullptr };
ID2D1Bitmap* bmpHeroR[6] = { nullptr };
///////////////////////////////////////////////////

template<typename POINTER> BOOL Unbind(POINTER** what)
{
    if ((*what))
    {
        (*what)->Release();
        (*what) = nullptr;
        return DL_OK;
    }
    return DL_FAIL;
}
void ErrLog(LPCWSTR what)
{
    std::wofstream log(L".\\res\\data\\error.log", std::ios::app);
    log << what << L" : " << std::chrono::system_clock::now() << std::endl;
    log.close();
}
void SafeReleaseCOMs()
{
    if (Unbind(&iFactory) == DL_FAIL)ErrLog(L"Error releasing iFactory ! ");
    if (Unbind(&Draw) == DL_FAIL)ErrLog(L"Error releasing Draw ! ");
    if (Unbind(&iWriteFactory) == DL_FAIL)ErrLog(L"Error releasing iWriteFactory ! ");
    if (Unbind(&nrmText) == DL_FAIL)ErrLog(L"Error releasing nrmText ! ");
    if (Unbind(&bigText) == DL_FAIL)ErrLog(L"Error releasing bigText ! ");
    if (Unbind(&ButBckgBrush) == DL_FAIL)ErrLog(L"Error releasing ButBckgBrush ! ");
    if (Unbind(&FieldBckBrush) == DL_FAIL)ErrLog(L"Error releasing FieldBckBrush ! ");
    if (Unbind(&TxtBrush) == DL_FAIL)ErrLog(L"Error releasing TxtBrush ! ");
    if (Unbind(&HgltBrush) == DL_FAIL)ErrLog(L"Error releasing HgltBrush ! ");
    if (Unbind(&InactBrush) == DL_FAIL)ErrLog(L"Error releasing InactBrush ! ");

    if (Unbind(&bmpBrick) == DL_FAIL)ErrLog(L"Error releasing bmpBrick ! ");
    if (Unbind(&bmpBullet) == DL_FAIL)ErrLog(L"Error releasing bmpBullet ! ");
    if (Unbind(&bmpCloud1) == DL_FAIL)ErrLog(L"Error releasing bmpCloud1 ! ");
    if (Unbind(&bmpCloud2) == DL_FAIL)ErrLog(L"Error releasing bmpCloud2 ! ");
    if (Unbind(&bmpField) == DL_FAIL)ErrLog(L"Error releasing bmpField ! ");
    if (Unbind(&bmpGold) == DL_FAIL)ErrLog(L"Error releasing bmpGold ! ");
    if (Unbind(&bmpKilled) == DL_FAIL)ErrLog(L"Error releasing bmpKilled ! ");
    if (Unbind(&bmpLMonst1) == DL_FAIL)ErrLog(L"Error releasing bmpLMonst1 ! ");
    if (Unbind(&bmpLMonst2) == DL_FAIL)ErrLog(L"Error releasing bmpLMonst2 ! ");
    if (Unbind(&bmpLMonst3) == DL_FAIL)ErrLog(L"Error releasing bmpLMonst3 ! ");
    if (Unbind(&bmpRMonst1) == DL_FAIL)ErrLog(L"Error releasing bmpRMonst1 ! ");
    if (Unbind(&bmpRMonst2) == DL_FAIL)ErrLog(L"Error releasing bmpRMonst2 ! ");
    if (Unbind(&bmpRMonst3) == DL_FAIL)ErrLog(L"Error releasing bmpRMonst3 ! ");

    for (int i = 0; i < 6; ++i)if (Unbind(&bmpHeroL[i]) == DL_FAIL)ErrLog(L"Error releasing bmpHeroL ! ");
    for (int i = 0; i < 6; ++i)if (Unbind(&bmpHeroR[i]) == DL_FAIL)ErrLog(L"Error releasing bmpHeroR ! ");
}
void ErrExit(int which_error)
{
    MessageBeep(MB_ICONERROR);
    MessageBox(NULL, ErrHandle(which_error), L"Критична грешка !", MB_OK | MB_APPLMODAL | MB_ICONERROR);

    SafeReleaseCOMs();
    std::remove(tmp_file);
    exit(1);
}
void InitGame()
{
    level = 1;
    score = 0;
    start_screen_col = 0;
    minutes = 0;
    seconds = 180;

    wcscpy_s(current_player, L"UNKNOWN HERO");
    name_set = false;

    
    CreateGrid(0, 51.0f, Grid);
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 6; j++)
            ScreenGrid[j][i] = Grid[j][i];
            
}

void GameOver()
{
    KillTimer(bHwnd, bTimer);
    PlaySound(NULL, NULL, NULL);


    bMsg.message = WM_QUIT;
    bMsg.wParam = 0;
}

INT_PTR CALLBACK bDlgProc(HWND hwnd, UINT ReceivedMsg, WPARAM wParam, LPARAM lParam)
{
    switch (ReceivedMsg)
    {
    case WM_INITDIALOG:
        SendMessage(hwnd, WM_SETICON, NULL, NULL);
        return (INT_PTR)TRUE;
        break;

    case WM_CLOSE:
        EndDialog(hwnd, IDCANCEL);
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDCANCEL:
            EndDialog(hwnd, IDCANCEL);
            break;

        case IDOK:
            if (GetDlgItemTextW(hwnd, IDC_NAME, current_player, 15) < 1)
            {
                if (sound)MessageBeep(MB_ICONASTERISK);
                MessageBox(bHwnd, L"Ха, ха, хаааа ! Забрави си името !", L"Забраватор !", MB_OK | MB_APPLMODAL | MB_ICONASTERISK);
                wcscpy_s(current_player, L"UNKNOWN HERO");
                EndDialog(hwnd, IDCANCEL);
                break;
            }
            else EndDialog(hwnd, IDOK);
            break;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
LRESULT CALLBACK bWinProc(HWND hwnd, UINT ReceivedMsg, WPARAM wParam, LPARAM lParam)
{
    switch (ReceivedMsg)
    {
    case WM_CREATE: 
        {
            RECT clR = { 0 };
            GetClientRect(hwnd, &clR);
            cl_width = static_cast<float>(clR.right);
            cl_height = static_cast<float>(clR.bottom);

            srand((unsigned int)(time(0)));
            SetTimer(hwnd, bTimer, 1000, NULL);

            bBar = CreateMenu();
            bMain = CreateMenu();
            bStore = CreateMenu();

            AppendMenu(bBar, MF_POPUP, (UINT_PTR)(bMain), L"Основно меню");
            AppendMenu(bBar, MF_POPUP, (UINT_PTR)(bStore), L"Меню за данни");

            AppendMenu(bMain, MF_STRING, mNew, L"Нова игра");
            AppendMenu(bMain, MF_STRING, mLvl, L"Слеващо ниво");
            AppendMenu(bMain, MF_SEPARATOR, NULL, NULL);
            AppendMenu(bMain, MF_STRING, mExit, L"Изход");

            AppendMenu(bStore, MF_STRING, mSave, L"Запази игра");
            AppendMenu(bStore, MF_STRING, mLoad, L"Зареди игра");
            AppendMenu(bStore, MF_SEPARATOR, NULL, NULL);
            AppendMenu(bStore, MF_STRING, mHoF, L"Зала на славата");
            SetMenu(hwnd, bBar);
            InitGame();
        }
        break;

    case WM_CLOSE:
        pause = true;
        if (sound)MessageBeep(MB_ICONQUESTION);
        if (MessageBox(hwnd, L"Ако продължиш, ще загубиш тази игра !\n\nНаистина ли излизаш ?",
            L"Изход !", MB_YESNO | MB_APPLMODAL | MB_ICONQUESTION) == IDNO)
        {
            pause = false;
            break;
        }
        GameOver();
        break;

    case WM_PAINT:
        PaintDC = BeginPaint(hwnd, &bPaint);
        FillRect(PaintDC, &bPaint.rcPaint, CreateSolidBrush(RGB(80, 80, 80)));
        EndPaint(hwnd, &bPaint);
        break;

    case WM_SETCURSOR:
        GetCursorPos(&cur_pos);
        ScreenToClient(hwnd, &cur_pos);
        if (LOWORD(lParam) == HTCLIENT)
        {
            if (!in_client)
            {
                in_client = true;
                pause = false;
            }

            if (cur_pos.y <= 50)
            {
                if (cur_pos.x >= b1Rect.left && cur_pos.x <= b1Rect.right)
                {
                    if (!b1_hglt)
                    {
                        if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                        b1_hglt = true;
                    }
                }

                if (cur_pos.x >= b2Rect.left && cur_pos.x <= b2Rect.right)
                {
                    if (!b2_hglt)
                    {
                        if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                        b2_hglt = true;
                    }
                }

                if (cur_pos.x >= b3Rect.left && cur_pos.x <= b3Rect.right)
                {
                    if (!b3_hglt)
                    {
                        if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                        b3_hglt = true;
                    }
                }

                SetCursor(outCursor);
                return true;
            }
            else
            {
                if (b1_hglt)
                {
                    if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                    b1_hglt = false;
                }
                if (b2_hglt)
                {
                    if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                    b2_hglt = false;
                }
                if (b3_hglt)
                {
                    if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                    b3_hglt = false;
                }
            }

            SetCursor(mainCursor);
            return true;
        }
        else
        {
            if (in_client)
            {
                in_client = false;
                pause = true;
            }

            if (b1_hglt)
            {
                if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                b1_hglt = false;
            }
            if (b2_hglt)
            {
                if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                b2_hglt = false;
            }
            if (b3_hglt)
            {
                if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                b3_hglt = false;
            }

            SetCursor(LoadCursor(NULL, IDC_ARROW));
            return true;
        }
        break;

    case WM_TIMER:
        if (pause)break;
        seconds--;
        minutes = seconds / 60;
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case mNew:
            pause = true;
            if (sound)MessageBeep(MB_ICONQUESTION);
            if (MessageBox(hwnd, L"Ако продължиш, ще загубиш тази игра !\n\nНаистина ли рестартираш ?",
                L"Рестарт !", MB_YESNO | MB_APPLMODAL | MB_ICONQUESTION) == IDNO)
            {
                pause = false;
                break;
            }
            InitGame();
            break;


        case mExit:
            SendMessage(hwnd, WM_CLOSE, NULL, NULL);
            break;


        }
        break;




    default: return DefWindowProc(hwnd, ReceivedMsg, wParam, lParam);
    }

    return (LRESULT)(FALSE);
}

void SystemInit()
{
    int result = 0;
    CheckFile(Ltmp_file, &result);
    if (result == FILE_EXIST)ErrExit(eStarted);
    else
    {
        std::wofstream strt(Ltmp_file);
        strt << L"Igrata rAboti ! Ne baray !";
        strt.close();
    }

    if ((float)(GetSystemMetrics(SM_CXSCREEN) / 2) + scr_width / 2 < scr_width ||
        GetSystemMetrics(SM_CYSCREEN) < scr_height + 50) ErrExit(eScreen);

    mainIcon = (HICON)(LoadImage(NULL, L".\\res\\main.ico", IMAGE_ICON, 64, 64, LR_LOADFROMFILE));
    if (!mainIcon)ErrExit(eIcon);

    mainCursor = LoadCursorFromFile(L".\\res\\bcursor.ani");
    outCursor = LoadCursorFromFile(L".\\res\\out.ani");

    if (!mainCursor || !outCursor)ErrExit(eCursor); 

    bWin.lpszClassName = bWinClassName;
    bWin.hInstance = bIns;
    bWin.lpfnWndProc = &bWinProc;
    bWin.hbrBackground = CreateSolidBrush(RGB(80, 80, 80));
    bWin.hIcon = mainIcon;
    bWin.hCursor = mainCursor;
    bWin.style = CS_DROPSHADOW;

    if (!RegisterClass(&bWin))ErrExit(eClass);
    
    bHwnd = CreateWindow(bWinClassName, L"СПАСИ СВЕТА !", WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        (int)(GetSystemMetrics(SM_CXSCREEN) / 2 - scr_width / 2), 50, (int)(scr_width), (int)(scr_height), 
        NULL, NULL, bIns, NULL);
    if (!bHwnd)ErrExit(eWindow);
    else ShowWindow(bHwnd, SW_SHOWDEFAULT);

    HRESULT hr = S_OK;
    D2D1_GRADIENT_STOP gstop[2] = { 0 };
    ID2D1GradientStopCollection* gstop_coll = nullptr;

    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &iFactory);
    if (hr != S_OK)
    {
        ErrLog(L"Error creating D2D1 Factory !");
        ErrExit(eD2D);
    }

    hr = iFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(),
        D2D1::HwndRenderTargetProperties(bHwnd, D2D1::SizeU((UINT)(scr_width), (UINT)(scr_height))), &Draw);
    if (hr != S_OK)
    {
        ErrLog(L"Error creating D2D1 HWND RenderTarget !");
        ErrExit(eD2D);
    }

    gstop[0].position = 0;
    gstop[0].color = D2D1::ColorF(D2D1::ColorF::Aquamarine);
    gstop[1].position = 1.0f;
    gstop[1].color = D2D1::ColorF(D2D1::ColorF::DarkSlateBlue);
    
    hr = Draw->CreateGradientStopCollection(gstop, 2, &gstop_coll);
    if (hr != S_OK)
    {
        ErrLog(L"Error creating D2D1 GradientStopCollection !");
        ErrExit(eD2D);
    }

    if (gstop_coll)
        hr = Draw->CreateRadialGradientBrush(D2D1::RadialGradientBrushProperties(D2D1::Point2F(cl_width / 2, 25.0f),
            D2D1::Point2F(0, 0), cl_width / 2, 25.0f), gstop_coll, &ButBckgBrush);
    if (hr != S_OK)
    {
        ErrLog(L"Error creating D2D1 ButBckgBrush !");
        ErrExit(eD2D);
    }
    
    if (Unbind(&gstop_coll) == DL_FAIL)ErrLog(L"Error releasing GStopCollection after RadialGradientBrush init ! ");

    gstop[0].position = 0;
    gstop[0].color = D2D1::ColorF(D2D1::ColorF::AliceBlue);
    gstop[1].position = 1.0f;
    gstop[1].color = D2D1::ColorF(D2D1::ColorF::DarkSlateBlue);

    hr = Draw->CreateGradientStopCollection(gstop, 2, &gstop_coll);
    if (hr != S_OK)
    {
        ErrLog(L"Error creating D2D1 GradientStopCollection !");
        ErrExit(eD2D);
    }

    if (gstop_coll)
        hr = Draw->CreateLinearGradientBrush(D2D1::LinearGradientBrushProperties(D2D1::Point2F(0, 50.0f), 
            D2D1::Point2F(0, cl_height)), D2D1::BrushProperties(), gstop_coll, &FieldBckBrush);
    if (hr != S_OK)
    {
        ErrLog(L"Error creating D2D1 FieldBckgBrush !");
        ErrExit(eD2D);
    }
    if (Unbind(&gstop_coll) == DL_FAIL)ErrLog(L"Error releasing GStopCollection after LinearGradientBrush init ! ");

    hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Green), &TxtBrush);
    if (hr != S_OK)
    {
        ErrLog(L"Error creating D2D1 TxtBrush !");
        ErrExit(eD2D);
    }

    hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Yellow), &HgltBrush);
    if (hr != S_OK)
    {
        ErrLog(L"Error creating D2D1 HgltBrush !");
        ErrExit(eD2D);
    }

    hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Gray), &InactBrush);
    if (hr != S_OK)
    {
        ErrLog(L"Error creating D2D1 InactBrush !");
        ErrExit(eD2D);
    }

    //WRITE FACTORY ********************************

    hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&iWriteFactory));
    if (hr != S_OK)
    {
        ErrLog(L"Error creating D2D1 WriteFactory !");
        ErrExit(eD2D);
    }

    hr = iWriteFactory->CreateTextFormat(L"GABRIOLA", NULL, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_OBLIQUE,
        DWRITE_FONT_STRETCH_EXPANDED, 18.0f, L"", &nrmText);
    if (hr != S_OK)
    {
        ErrLog(L"Error creating D2D1 nrmText !");
        ErrExit(eD2D);
    }

    hr = iWriteFactory->CreateTextFormat(L"GABRIOLA", NULL, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_OBLIQUE,
        DWRITE_FONT_STRETCH_EXPANDED, 48.0f, L"", &bigText);
    if (hr != S_OK)
    {
        ErrLog(L"Error creating D2D1 bigText !");
        ErrExit(eD2D);
    }

    ////////////////////////////////////////////////

    bmpBrick = Load(L".\\res\\img\\brick.png", Draw);
    if (!bmpBrick)
    {
        ErrLog(L"Error creating D2D1 bmpBrick !");
        ErrExit(eD2D);
    }

    bmpBullet = Load(L".\\res\\img\\bullet.png", Draw);
    if (!bmpBullet)
    {
        ErrLog(L"Error creating D2D1 bmpBrick !");
        ErrExit(eD2D);
    }

    bmpCloud1 = Load(L".\\res\\img\\cloud1.png", Draw);
    if (!bmpCloud1)
    {
        ErrLog(L"Error creating D2D1 bmpCloud1 !");
        ErrExit(eD2D);
    }

    bmpCloud2 = Load(L".\\res\\img\\cloud2.png", Draw);
    if (!bmpCloud2)
    {
        ErrLog(L"Error creating D2D1 bmpCloud2 !");
        ErrExit(eD2D);
    }

    bmpField = Load(L".\\res\\img\\field.png", Draw);
    if (!bmpField)
    {
        ErrLog(L"Error creating D2D1 bmpField !");
        ErrExit(eD2D);
    }

    bmpGold = Load(L".\\res\\img\\gold.png", Draw);
    if (!bmpGold)
    {
        ErrLog(L"Error creating D2D1 bmpGold !");
        ErrExit(eD2D);
    }

    bmpKilled = Load(L".\\res\\img\\hollyroger.png", Draw);
    if (!bmpKilled)
    {
        ErrLog(L"Error creating D2D1 bmpKilled !");
        ErrExit(eD2D);
    }

    bmpLMonst1 = Load(L".\\res\\img\\lmonster1.png", Draw);
    if (!bmpLMonst1)
    {
        ErrLog(L"Error creating D2D1 bmpLMonst1 !");
        ErrExit(eD2D);
    }

    bmpLMonst2 = Load(L".\\res\\img\\lmonster2.png", Draw);
    if (!bmpLMonst2)
    {
        ErrLog(L"Error creating D2D1 bmpLMonst2 !");
        ErrExit(eD2D);
    }

    bmpLMonst3 = Load(L".\\res\\img\\lmonster3.png", Draw);
    if (!bmpLMonst3)
    {
        ErrLog(L"Error creating D2D1 bmpLMonst3 !");
        ErrExit(eD2D);
    }

    bmpRMonst1 = Load(L".\\res\\img\\rmonster1.png", Draw);
    if (!bmpRMonst1)
    {
        ErrLog(L"Error creating D2D1 bmpRMonst1 !");
        ErrExit(eD2D);
    }

    bmpRMonst2 = Load(L".\\res\\img\\Rmonster2.png", Draw);
    if (!bmpRMonst2)
    {
        ErrLog(L"Error creating D2D1 bmpRMonst2 !");
        ErrExit(eD2D);
    }

    bmpRMonst3 = Load(L".\\res\\img\\Rmonster3.png", Draw);
    if (!bmpRMonst3)
    {
        ErrLog(L"Error creating D2D1 bmpRMonst3 !");
        ErrExit(eD2D);
    }

    for (int i = 0; i < 6; i++)
    {
        wchar_t name[75] = L".\\res\\img\\soldier\\left\\";
        wchar_t add[5] = L"\0";
        wsprintf(add, L"%d", i);
        wcscat_s(name, add);
        wcscat_s(name, L".gif");
        bmpHeroL[i] = Load(name, Draw);
        if (!bmpHeroL[i])
        {
            ErrLog(L"Error creating D2D1 bmpHeroL !");
            ErrExit(eD2D);
        }
    }

    for (int i = 0; i < 6; i++)
    {
        wchar_t name[75] = L".\\res\\img\\soldier\\right\\";
        wchar_t add[5] = L"\0";
        wsprintf(add, L"%d", i);
        wcscat_s(name, add);
        wcscat_s(name, L".gif");
        bmpHeroR[i] = Load(name, Draw);
        if (!bmpHeroR[i])
        {
            ErrLog(L"Error creating D2D1 bmpHeroR !");
            ErrExit(eD2D);
        }
    }

    ////////////////////////////////////////////////////////

    wchar_t first_text[34] = L"ИЗВЪНЗЕМНИ В АТАКА !\n\ndev.Daniel";
    wchar_t show_text[34] = L"\0";

    for (int i = 0; i < 34; ++i)
    {
        Draw->BeginDraw();
        Draw->Clear(D2D1::ColorF(D2D1::ColorF::DarkSlateBlue));
        show_text[i] = first_text[i];
        if (TxtBrush && bigText)
            Draw->DrawTextW(show_text, i, bigText, D2D1::RectF(150.0f, cl_height / 2 - 50.0f, cl_width, cl_height), TxtBrush);
        Draw->EndDraw();
        mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
        Sleep(45);
    }
}


////////////////////////////////////////////////

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    bIns = hInstance;
    if (!bIns)ErrExit(eClass);

    PlaySound(NULL, NULL, NULL);
    SystemInit();

    while (bMsg.message != WM_QUIT)
    {
        if ((bRet = PeekMessage(&bMsg, bHwnd, NULL, NULL, PM_REMOVE)) != 0)
        {
            if (bRet == -1)ErrExit(eMsg);
            TranslateMessage(&bMsg);
            DispatchMessage(&bMsg);
        }

        if (pause)
        {
            if (show_help)continue;
            Draw->BeginDraw();
            Draw->Clear(D2D1::ColorF(D2D1::ColorF::DarkSlateBlue));
            if (TxtBrush && bigText)
                Draw->DrawTextW(L"ПАУЗА", 6, bigText, D2D1::RectF(cl_width / 2 - 50.0f, cl_height / 2 - 50.0f, 
                    cl_width, cl_height), TxtBrush);
            Draw->EndDraw();
            continue;
        }

        ////////////////////////////




        // DRAW THINGS ***********************

        Draw->BeginDraw();
        if (ButBckgBrush)
            Draw->FillRectangle(D2D1::RectF(0, 0, scr_width, 50.0f), ButBckgBrush);
        if (FieldBckBrush)
            Draw->FillRectangle(D2D1::RectF(0, 50, scr_width, scr_height), FieldBckBrush);
        if (nrmText && ButBckgBrush && TxtBrush && HgltBrush && InactBrush)
        {
            if (name_set)
                Draw->DrawText(L"Име на играч", 13, nrmText, D2D1::RectF(b1Rect.left + 5.0f, b1Rect.top + 5.0f, 
                    b1Rect.right, b1Rect.bottom), InactBrush);
            else
            {
                if(!b1_hglt)
                    Draw->DrawText(L"Име на играч", 13, nrmText, D2D1::RectF(b1Rect.left + 5.0f, b1Rect.top + 5.0f,
                        b1Rect.right, b1Rect.bottom), TxtBrush);
                else
                    Draw->DrawText(L"Име на играч", 13, nrmText, D2D1::RectF(b1Rect.left + 5.0f, b1Rect.top + 5.0f,
                        b1Rect.right, b1Rect.bottom), HgltBrush);
            }

            if (!b2_hglt)
                Draw->DrawText(L"Звуци ON / OFF", 15, nrmText, D2D1::RectF(b2Rect.left + 5.0f, b2Rect.top + 5.0f,
                    b2Rect.right, b2Rect.bottom), TxtBrush);
            else
                Draw->DrawText(L"Звуци ON / OFF", 15, nrmText, D2D1::RectF(b2Rect.left + 5.0f, b2Rect.top + 5.0f,
                    b2Rect.right, b2Rect.bottom), HgltBrush);

            if (!b3_hglt)
                Draw->DrawText(L"Помощ за играта", 16, nrmText, D2D1::RectF(b3Rect.left + 5.0f, b3Rect.top + 5.0f,
                    b3Rect.right, b3Rect.bottom), TxtBrush);
            else
                Draw->DrawText(L"Помощ за играта", 16, nrmText, D2D1::RectF(b3Rect.left + 5.0f, b3Rect.top + 5.0f,
                    b3Rect.right, b3Rect.bottom), HgltBrush);
        }




        /////////////////////////////////////

        Draw->EndDraw();

    }

    SafeReleaseCOMs();
    std::remove(tmp_file);
    return (int) bMsg.wParam;
}