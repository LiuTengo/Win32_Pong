// FinalTest Pong.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "FinalTest Pong.h"
#include <time.h>
#include <iostream>
#include <windows.h>
#include <mmsystem.h>

#define MAX_LOADSTRING 100
#pragma comment(lib, "winmm.lib")

typedef struct
{
    float x, y;   //位置
    float dx,dy;      //移动速度
    int w;      //宽度
    int h;      //高度
    COLORREF c; //颜色
}GameObject;    //GameObject includes paddles and ball

enum GameState // 游戏状态
{
    Null,
    StartState,       //开始阶段
    ServeState,      //发球阶段
    PlayState,       //游戏阶段
    CountingState,    //结算阶段
};

// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名


GameState gameState = Null; //创建游戏状态
GameObject ball; //创建游戏物体——小球
GameObject LPaddle; //创建游戏物体——左球拍
GameObject RPaddle; //创建游戏物体——右球拍

int winner = 1;     
int ServePlayer = 1; // 1-Left 2-Right
int RScore = 0; //左右两玩家的分数
int LScore = 0;
float PaddleSpeed = 6.5; // 球拍移动速度
int timeStep = 20;//每帧时间间隔
int nWidth = 0;  //屏幕宽度    
int nHeight = 0; //屏幕高度



// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);


//重设球拍位置
void ResetPaddle(GameObject *paddle) 
{
    paddle->dx = 0;
    paddle->dy = 0;

    paddle->h = 80;
    paddle->w = 10;

    paddle->c = RGB(255, 255, 255);
}

//重设小球位置
void ResetBall() 
{
    ball.x = nWidth / 2;
    ball.y = nHeight / 2;

    ball.dx = 0;
    ball.dy = 0;

    ball.h = ball.w = 8;

    ball.c = RGB(255, 255, 255);
}

//初始化
void Init() 
{
    ResetBall();
    gameState = StartState;
    //重设右paddle位置
    RPaddle.x = nWidth / 2 + 500;
    RPaddle.y = nHeight / 2 + 180;
    ResetPaddle(&RPaddle);
    //重设左paddle位置
    LPaddle.x = nWidth / 2 - 500;
    LPaddle.y = nHeight / 2 - 180;
    ResetPaddle(&LPaddle);
}


//判断小球是否与平板碰撞
int IsCollide(GameObject currentObject, GameObject otherObject) 
{
    if (currentObject.x < otherObject.x - otherObject.w / 2 || currentObject.x > otherObject.x + otherObject.w / 2)
        return 0;
    if (currentObject.y < otherObject.y - otherObject.h / 2 || currentObject.y > otherObject.y + otherObject.h / 2)
        return 0;

    return 1;
}

//判断小球是否与窗口边缘碰撞
int WallCollide(GameObject *ball) 
{
    if (ball->y - ball->h / 2 <= 0 || ball->y + ball->h / 2 >= nHeight)
        return 1;

    return 0;
}


void DrawGameObject(HDC hdc, GameObject *obj) 
{
    HBRUSH brush;                            //创建笔刷
    brush = CreateSolidBrush(obj -> c);//设置绘制物体的颜色
    RECT rc;
    rc.top = obj -> y - obj->h/2;
    rc.bottom = obj->y + obj->h / 2;
    rc.left = obj->x - obj->w / 2;
    rc.right = obj->x + obj->w / 2;
    FillRect(hdc,&rc,brush);

    DeleteObject(brush);                  //删除笔刷
}

void Update(int ts) 
{
    //与左球拍碰撞
    if (IsCollide(ball, LPaddle))
    {
        PlaySound(TEXT("D:\\Visual Studio项目\\FinalTest Pong\\Collide.wav"), NULL, SND_FILENAME | SND_ASYNC); //播放音效
        if ((LPaddle.dy > 0) ? LPaddle.dy * ts : (-1 * LPaddle.dy * ts) > 6)//当球拍速度超过6时判定打出快速球
        {
            ball.dx = -ball.dx * 1.06f;//打出快速球
            //ball.x = LPaddle.x + ball.w / 2;// 防止小球卡住
            ball.c = RGB(255, 255, 0); //如果打出快速球，则改变小球颜色
        }
        else
        {
            ball.dx = -ball.dx * 1.03f;//改变小球的dx
            //ball.x = LPaddle.x + ball.w / 2;// 防止小球卡住
            ball.c = RGB(255, 255, 255);
        }

    }
    //与右球拍碰撞
    if (IsCollide(ball, RPaddle))
    {
        PlaySound(TEXT("D:\\Visual Studio项目\\FinalTest Pong\\Collide.wav"),NULL, SND_FILENAME | SND_ASYNC);
        if((RPaddle.dy > 0) ? RPaddle.dy * ts : (-1 * RPaddle.dy * ts) > 6)
        {
            ball.dx = -ball.dx * 1.06f;
            ball.x = RPaddle.x - ball.w / 2; // 防止小球卡住
            ball.c = RGB(255,255,0);
        }
        else 
        {
            ball.dx = -ball.dx * 1.03f;//改变小球的dx
            ball.x = RPaddle.x - ball.w / 2;// 防止小球卡住
            ball.c = RGB(255, 255, 255);
        }
    }
    //与墙体碰撞
    if (WallCollide(&ball)) 
    {
        PlaySound(TEXT("D:\\Visual Studio项目\\FinalTest Pong\\Collide.wav"), NULL, SND_FILENAME | SND_ASYNC);
        ball.dy = -ball.dy * 1.03f;    //改变小球的dy
    }
    
    //触碰对方底线则给己方加分
    if (ball.x <= 0) 
    {
        PlaySound(TEXT("D:\\Visual Studio项目\\FinalTest Pong\\Defeat.wav"), NULL, SND_FILENAME | SND_ASYNC);//播放音效

        RScore ++;//加分

        ResetBall();        //重置小球位置
        ServePlayer = 1;    //交换发球权限
        gameState = ServeState;//切换游戏状态至发球状态

        //判断是否有玩家胜利
        if (RScore >= 11) 
        {
            gameState = CountingState; //切换游戏状态至结算状态
            winner = 2;// 输出胜利信息
            
        }
    }
    else if (ball.x >= nWidth)
    {
        PlaySound(TEXT("D:\\Visual Studio项目\\FinalTest Pong\\Defeat.wav"), NULL, SND_FILENAME | SND_ASYNC);
        LScore++;
        ResetBall();        //重置小球位置
        ServePlayer = 2;    //ServePlayer - Right
        gameState = ServeState;
        if (LScore >= 11) 
        {
            gameState = CountingState;
            winner = 1;
        }
    }

    //更新球拍位置
    RPaddle.y += RPaddle.dy;
    //限制球拍y坐标在0至屏幕宽度之间
    if (RPaddle.y + RPaddle.h / 2 >= nHeight)
        RPaddle.y = nHeight - RPaddle.h / 2;
    else if (RPaddle.y - RPaddle.h / 2 <= 0)
        RPaddle.y = RPaddle.h / 2;

    LPaddle.y += LPaddle.dy;
    if (LPaddle.y + LPaddle.h/2 >= nHeight)
        LPaddle.y = nHeight - LPaddle.h / 2;
    else if (LPaddle.y - LPaddle.h / 2 <= 0)
        LPaddle.y = LPaddle.h / 2;


    //更新小球位置
    ball.x += ball.dx * ts;
    ball.y += ball.dy * ts;
}



//绘制场景
void DrawScene(HDC hdc)
{
    //创建字体
    HFONT hf;

    //创建储存文本信息的char数组
    WCHAR Rscore[32];
    WCHAR line [2];
    WCHAR Lscore[32];
    WCHAR title[16];
    WCHAR Text[32];
    WCHAR Notice[64];
    long lfHeight;
    lfHeight = -MulDiv(16, GetDeviceCaps(hdc, LOGPIXELSY), 72);

    hf = CreateFont(lfHeight*2, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, L"04b03");
    HFONT hfOld = (HFONT)SelectObject(hdc, hf);

    //改变背景颜色
    SetBkColor(hdc, RGB(0,0,0));
    //设置文本颜色
    SetTextColor(hdc,RGB(255,255,255));

    //根据游戏状态判断展示哪些信息
    switch (gameState) 
    {
    case StartState:
        wsprintf(title, L"Pong!");
        wsprintf(Text, L"Press 'Enter' To Start");

        TextOut(hdc, nWidth / 2 - 60, nHeight / 2-30, title, wcslen(title));
        TextOut(hdc, nWidth / 2 - 200, nHeight / 2 +200, Text, wcslen(Text));
        break;
    case PlayState:
        wsprintf(Rscore, L"%d", RScore);
        wsprintf(line, L":");
        wsprintf(Lscore, L"%d", LScore);

        TextOut(hdc, nWidth / 2, nHeight / 2 - 200, line, wcslen(line));
        TextOut(hdc, nWidth / 2 + 50, nHeight / 2 - 200, Rscore, wcslen(Rscore));
        TextOut(hdc, nWidth / 2 - 60, nHeight / 2 - 200, Lscore, wcslen(Lscore));

        DrawGameObject(hdc, &ball);				// 绘制小球
        DrawGameObject(hdc, &LPaddle);          // 绘制左球拍
        DrawGameObject(hdc, &RPaddle);          // 绘制右球拍
        break;
    case ServeState:
        wsprintf(Rscore, L"%d", RScore);
        wsprintf(line, L":");
        wsprintf(Lscore, L"%d", LScore);
        wsprintf(Text, L"Press 'Space' To Serve");
        wsprintf(Notice, L"Is Player%d's Turn", ServePlayer);

        TextOut(hdc, nWidth / 2, nHeight / 2 - 200, line, wcslen(line));
        TextOut(hdc, nWidth / 2 + 50, nHeight / 2 - 200, Rscore, wcslen(Rscore));
        TextOut(hdc, nWidth / 2 - 60, nHeight / 2 - 200, Lscore, wcslen(Lscore));
        TextOut(hdc, nWidth / 2 - 200, nHeight / 2 + 200, Text, wcslen(Text));
        TextOut(hdc, nWidth / 2 - 150, nHeight / 2 + 100, Notice, wcslen(Notice));

        DrawGameObject(hdc, &ball);				// 绘制小球
        DrawGameObject(hdc, &LPaddle);          // 绘制左球拍
        DrawGameObject(hdc, &RPaddle);          // 绘制右球拍
        break;
    case CountingState:

        wsprintf(Rscore, L"%d", RScore);
        wsprintf(line, L":");
        wsprintf(Lscore, L"%d", LScore);

        TextOut(hdc, nWidth / 2, nHeight / 2 - 200, line, wcslen(line));
        TextOut(hdc, nWidth / 2 + 50, nHeight / 2 - 200, Rscore, wcslen(Rscore));
        TextOut(hdc, nWidth / 2 - 60, nHeight / 2 - 200, Lscore, wcslen(Lscore));

        DrawGameObject(hdc, &ball);				// 绘制小球
        DrawGameObject(hdc, &LPaddle);          // 绘制左球拍
        DrawGameObject(hdc, &RPaddle);          // 绘制右球拍

        wsprintf(Text, L"Press 'Enter' To Restart");
        wsprintf(Notice, L"Congratulation! Player%d Is The Winner", winner);

        TextOut(hdc, nWidth / 2 - 230, nHeight / 2 + 200, Text, wcslen(Text));
        TextOut(hdc, nWidth / 2 - 330, nHeight / 2 + 100, Notice, wcslen(Notice));
        break;
    }

    SelectObject(hdc, hfOld);
    DeleteObject(hf);

}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此处放置代码。
    // 
    // 
    
    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_FINALTESTPONG, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_FINALTESTPONG));

    MSG msg;
    //调用初始化函数
    Init();
    // 主消息循环:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FINALTESTPONG));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_FINALTESTPONG);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 分析菜单选择:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    //创建计时器
    case WM_CREATE:
        SetTimer(hWnd, 1, timeStep, NULL);
        break;
    //在计时器中调用Update函数
    case WM_TIMER:
        if (wParam == 1) 
        {
            Update(timeStep / 10);
            InvalidateRect(hWnd,NULL,TRUE);
        }
        break;
    //获取屏幕的宽度及高度
    case WM_SIZE:
        nWidth = LOWORD(lParam);
        nHeight = HIWORD(lParam);
        break;
    //获取按键信息
    case WM_KEYDOWN:
        InvalidateRect(hWnd, NULL, TRUE);
        switch (wParam) 
        {
        case VK_UP:
            RPaddle.dy = -PaddleSpeed;
            break;
        case VK_DOWN:
            RPaddle.dy = PaddleSpeed;
            break;
        case 'W':
            LPaddle.dy = -PaddleSpeed;
            break;
        case 'S':
            LPaddle.dy = PaddleSpeed;
            break;
        case VK_RETURN:
            PlaySound(TEXT("D:\\Visual Studio项目\\FinalTest Pong\\ButtonDown.wav"), NULL, SND_FILENAME | SND_ASYNC);
            if (gameState == StartState)
                gameState = ServeState;
            if (gameState == CountingState) 
            {
                gameState = StartState;
                RScore = 0;
                LScore = 0;
            }
            break;
        }
       
        if (wParam == VK_SPACE && gameState == ServeState)//改变游戏状态并判断由谁来发球 
        {
            PlaySound(TEXT("D:\\Visual Studio项目\\FinalTest Pong\\ButtonDown.wav"), NULL, SND_FILENAME | SND_ASYNC);
            ball.dx = rand() % 5 + 2;
            ball.dy = rand() % 4 + 2;
            gameState = PlayState;
            if (ServePlayer == 2) 
            {
                ball.dx = -ball.dx;
                ball.dy = -ball.dy;
            }
        }
        break;
    //当按键抬起时，归零球拍位移速度
    case WM_KEYUP:
        InvalidateRect(hWnd, NULL, TRUE);
        switch (wParam) 
        {
        case VK_UP:
            RPaddle.dy =0;
            break;
        case VK_DOWN:
            RPaddle.dy = 0;
            break;
        case 'W':
            LPaddle.dy = 0;
            break;
        case 'S':
            LPaddle.dy = 0;
            break;
        }
        break;
    case WM_ERASEBKGND:
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 在此处添加使用 hdc 的任何绘图代码...
            //创建内存HDC
            HDC memHDC = CreateCompatibleDC(hdc);
            
            RECT rectClient;
            GetClientRect(hWnd, &rectClient);

            HBITMAP bmpBuff = CreateCompatibleBitmap(hdc,nWidth,nHeight);
            HBITMAP pOldBMP = (HBITMAP)SelectObject(memHDC, bmpBuff);
            
            srand(time(NULL));
            DrawScene(memHDC);

            //拷贝内存HDC内容到实际HDC
            BOOL tt = BitBlt(hdc, rectClient.left, rectClient.top, nWidth,
                nHeight, memHDC, rectClient.left, rectClient.top, SRCCOPY);

            //内存回收
            SelectObject(memHDC, pOldBMP);
            DeleteObject(bmpBuff);
            DeleteDC(memHDC);

            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
