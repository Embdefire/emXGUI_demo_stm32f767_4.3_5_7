
#include <stdio.h>
#include "emXGUI.h"
#include "x_libc.h"
#include "backend_res_mgr.h"
#include "GUI_AppDef.h"




/**********************分界线*********************/

/* 各类控件ID */
#define ID_EXIT            0x3000
#define ID_BURN           0x3001
#define ID_RESET           0x3002
#define ID_INFO           0x3003
#define ID_TITLE          0x3004
#define ID_EXIT_INFO       0x3005
#define ID_PROGBAR		      0x3006

//定义控件的私有消息(所有控件的私有消息要从 WM_WIDGET 开始定义)
#define MSG_MYWRITE_RESULT WM_WIDGET+1 //烧录结果

HWND wnd_res_writer_info_textbox ;
HWND wnd_res_writer_dialog;
HWND wnd_res_writer_progbar;

/* 用于标记是否有资源文件无法找到 */
extern BOOL res_not_found_flag;


/**
  * @brief  烧录应用线程
  */
static void App_FLASH_Writer(void )
{
  u32 result;
 
	result = (u32)BurnFile();
	
	//发消息给wnd_res_writer_dialog,烧录结果
	SendMessage(wnd_res_writer_dialog,MSG_MYWRITE_RESULT,result,0);   

	/* 删除线程自己 */
	GUI_Thread_Delete(GUI_GetCurThreadHandle());
}

/**
  * @brief  烧录应用回调函数
  */
static	LRESULT	win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
//	HDC hdc;
	RECT rc,rc0;
	HWND wnd;
  wnd_res_writer_dialog = hwnd;

	//static RECT rc_R, rc_G, rc_B;//RGB分量指示框
  
  const WCHAR no_res_info[] = L"Some resources not found in the FLASH.\r\n\
Follow the instructions below:\r\n\r\n\
1.Insert an SD card with [srcdata] resource.\r\n\
3.Click the button below to load the resources.\r\n\
2.Power up again the board.";
  
  const WCHAR normal_res_info[] = L"Please [Exit] if you don't know what to do!!!\r\n\
This app is use to reload resources\r\n\
If you really want to reload resources:\r\n\
1.Insert an SD card with [srcdata] resource.\r\n\
3.Click the button below to load the resources.\r\n\
2.Power up again the board.";
  
  /* 默认显示信息 */
  const WCHAR *pStr = normal_res_info;

  PROGRESSBAR_CFG cfg;	

   //HDC hdc_mem2pic;
	switch (msg)
	{
    case WM_CREATE: {  
      
          /* 找不到资源时显示信息 */
          if(res_not_found_flag)
            pStr = no_res_info;
          
          GetClientRect(hwnd,&rc); //获得窗口的客户区矩形.
          CopyRect(&rc0,&rc);
          
          /* 本窗口垂直分为5份 */
      
          /* 标题 */
          rc0.x = 0;
          rc0.y = 0;
          rc0.w = rc.w;
          rc0.h = rc.h/5;
          
          wnd = CreateWindow(TEXTBOX,L"GUI FLASH Writer" ,WS_VISIBLE,
                                rc0.x, rc0.y, rc0.w, rc0.h, hwnd, ID_TITLE, NULL, NULL); 

          SendMessage(wnd,TBM_SET_TEXTFLAG,0,DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_BKGND);          

          /* 有退出提示 */
          if(!res_not_found_flag)
          {  
            /* 退出按钮 */
            rc0.w = 60;
            rc0.x = rc.w - rc0.w - 5*2;
            rc0.h = 30;

            CreateWindow(BUTTON, L"Exit",BS_FLAT | WS_VISIBLE,
                          rc0.x, rc0.y, rc0.w, rc0.h, hwnd, ID_EXIT, NULL, NULL); 
          }
          
          /* 提示信息 */
          rc0.x = 5;
          rc0.y = 1*rc.h/5;
          rc0.w = rc.w - rc0.x*2;
          rc0.h = 2*rc.h/5;
      
          CreateWindow(TEXTBOX,pStr ,WS_VISIBLE,
                        rc0.x, rc0.y, rc0.w, rc0.h, hwnd, ID_INFO, NULL, NULL); 

          /* 进度条 */
          rc0.x = 5;
          rc0.w = rc.w - rc0.x*2;
          rc0.h = 30;
          rc0.y = 4*rc.h/5 - rc0.h-10;
 
          //PROGRESSBAR_CFG结构体的大小
					cfg.cbSize	 = sizeof(PROGRESSBAR_CFG);
          //开启所有的功能
					cfg.fMask    = PB_CFG_ALL;
          //文字格式水平，垂直居中
					cfg.TextFlag = DT_VCENTER|DT_CENTER;  

					wnd_res_writer_progbar = CreateWindow(PROGRESSBAR,L"",
                                  PBS_TEXT|PBS_ALIGN_LEFT,
                                  rc0.x, rc0.y, rc0.w, rc0.h,hwnd,ID_PROGBAR,NULL,NULL);

          SendMessage(wnd_res_writer_progbar,PBM_GET_CFG,TRUE,(LPARAM)&cfg);
          SendMessage(wnd_res_writer_progbar,PBM_SET_CFG,TRUE,(LPARAM)&cfg);
          SendMessage(wnd_res_writer_progbar,PBM_SET_VALUE,TRUE,0);

          /* 烧录按钮 */
          rc0.x = 5;
          rc0.w = rc.w/2- rc0.x*2;
          rc0.h = 45;
          rc0.y = rc.h - rc0.h - 10;

          CreateWindow(BUTTON, L"Click me to load resources",BS_FLAT | WS_VISIBLE,
                        rc0.x, rc0.y, rc0.w, rc0.h, hwnd, ID_BURN, NULL, NULL); 

          /* 复位按钮 */
          rc0.x = rc.w/2 +5;
          rc0.w = rc.w/2 - 5*2;
          rc0.h = 45;
          rc0.y = rc.h - rc0.h - 10;
          
          CreateWindow(BUTTON, L"Click me to reset system",BS_FLAT ,
                        rc0.x, rc0.y, rc0.w, rc0.h, hwnd, ID_RESET, NULL, NULL); 
          break;
	}
    
   case MSG_MYWRITE_RESULT:
    {
      u32 result = wParam;

      /* 烧录失败 */
      if(result)
      {
        EnableWindow(GetDlgItem(hwnd,ID_BURN),ENABLE);

      }
      else
      {
        /* 烧录成功 */
        ShowWindow(GetDlgItem(hwnd,ID_RESET),SW_SHOW);
        ShowWindow(GetDlgItem(hwnd,ID_BURN),SW_HIDE);
        ShowWindow(GetDlgItem(hwnd,ID_PROGBAR),SW_HIDE);

        SetWindowText(wnd_res_writer_info_textbox,L"Load resources success!\r\n\r\nClick the button below to reset system!");

      }  
      

      break;
    }
 
	case WM_NOTIFY: {
      u16 code,  id;
      id  =LOWORD(wParam);//获取消息的ID码
      code=HIWORD(wParam);//获取消息的类型
		
      if(id == ID_EXIT && code == BN_CLICKED)
      {
         PostCloseMessage(hwnd);
      }
      
      if(id == ID_BURN && code == BN_CLICKED)
      {
         wnd_res_writer_info_textbox = GetDlgItem(hwnd,ID_INFO);
//         App_FLASH_Writer();
              /* 创建线程运行自己 */
      GUI_Thread_Create((void(*)(void*))App_FLASH_Writer,  /* 任务入口函数 */
                            "Flash writer",/* 任务名字 */
                            30*1024,  /* 任务栈大小 */
                            NULL, /* 任务入口函数参数 */
                            5,    /* 任务的优先级 */
                            10); /* 任务时间片，部分任务不支持 */
         EnableWindow(GetDlgItem(hwnd,ID_BURN),DISABLE);
         ShowWindow(GetDlgItem(hwnd,ID_EXIT),SW_HIDE);
         ShowWindow(GetDlgItem(hwnd,ID_EXIT_INFO),SW_HIDE);
         ShowWindow(GetDlgItem(hwnd,ID_PROGBAR),SW_SHOW);
      }
      
      if(id == ID_RESET && code == BN_CLICKED)
      {
         NVIC_SystemReset();
      }      
      
      
		break;
	}

   case	WM_CTLCOLOR:
   {
      /* 控件在绘制前，会发送 WM_CTLCOLOR到父窗口.
       * wParam参数指明了发送该消息的控件ID;lParam参数指向一个CTLCOLOR的结构体指针.
       * 用户可以通过这个结构体改变控件的颜色值.用户修改颜色参数后，需返回TRUE，否则，系统
       * 将忽略本次操作，继续使用默认的颜色进行绘制.
       *
       */
			u16 id;
			id =LOWORD(wParam);
			if(id== ID_INFO )
			{
				CTLCOLOR *cr;
				cr =(CTLCOLOR*)lParam;
				cr->TextColor =RGB888(0,255,0);//文字颜色（RGB888颜色格式)
				cr->BackColor =RGB888(0,0,0);//背景颜色（RGB888颜色格式)
				cr->BorderColor =RGB888(255,10,10);//边框颜色（RGB888颜色格式)
				return TRUE;
			}
      else if(id == ID_TITLE)
      {
      	CTLCOLOR *cr;
				cr =(CTLCOLOR*)lParam;
				cr->TextColor =RGB888(255,255,0);//文字颜色（RGB888颜色格式)
				cr->BackColor =RGB888(0,0,0);//背景颜色（RGB888颜色格式)
				cr->BorderColor =RGB888(255,10,10);//边框颜色（RGB888颜色格式)
				return TRUE;

      }
      else if(id == ID_EXIT_INFO)
      {
      	CTLCOLOR *cr;
				cr =(CTLCOLOR*)lParam;
				cr->TextColor =RGB888(255,0,0);//文字颜色（RGB888颜色格式)
				cr->BackColor =RGB888(0,0,0);//背景颜色（RGB888颜色格式)
				cr->BorderColor =RGB888(255,10,10);//边框颜色（RGB888颜色格式)
				return TRUE;

      }
//      else if(id == ID_BURN || id == ID_EXIT)
//      {
//        CTLCOLOR *cr;
//				cr =(CTLCOLOR*)lParam;
//				cr->TextColor =RGB888(255,0,0);//文字颜色（RGB888颜色格式)
//				cr->BackColor =RGB888(0,0,0);//背景颜色（RGB888颜色格式)
//				cr->BorderColor =RGB888(0,0,0);//边框颜色（RGB888颜色格式)
//				return TRUE;
//      }

			else
			{
				return FALSE;
			}
      
   }   
   case WM_ERASEBKGND:
   {
      HDC hdc =(HDC)wParam;
      RECT rc;
      GetClientRect(hwnd, &rc);
      
      SetBrushColor(hdc, MapRGB(hdc, 0, 0, 0));
      FillRect(hdc, &rc);
      
      
      return TRUE;
      
   }

	case	WM_PAINT: //窗口需要重绘制时，会自动收到该消息.
	{	
      PAINTSTRUCT ps;
//      HDC hdc;//屏幕hdc
//      hdc = BeginPaint(hwnd, &ps); 
    BeginPaint(hwnd, &ps); 

		EndPaint(hwnd, &ps);
		return	TRUE;
	}
	default:
		return	DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return	WM_NULL;
}


void	GUI_RES_Writer_Dialog(void *param)
{
	HWND	hwnd;
	WNDCLASS	wcex;
	MSG msg;

  /* 重设默认字体为ASCII 内部flash字库，防止擦除时出错 */
  GUI_SetDefFont(defaultFontEn);  //设置默认的字体

	wcex.Tag = WNDCLASS_TAG;

	wcex.Style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = win_proc; //设置主窗口消息处理的回调函数.
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;//hInst;
	wcex.hIcon = NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor = NULL;//LoadCursor(NULL, IDC_ARROW);
	//创建主窗口
	hwnd = CreateWindowEx(WS_EX_FRAMEBUFFER,
                        &wcex,
                        L"GUI FLASH Writer",
                        WS_CLIPCHILDREN,
                        0, 0, GUI_XSIZE, GUI_YSIZE,
                        NULL, NULL, NULL, NULL);
   //显示主窗口
	ShowWindow(hwnd, SW_SHOW);
	//开始窗口消息循环(窗口关闭并销毁时,GetMessage将返回FALSE,退出本消息循环)。
	while (GetMessage(&msg, hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

  /* 恢复中文默认字体 */
  GUI_SetDefFont(defaultFont);  

  /* res_not_found_flag为真时，res writer应用是独立创建线程的，所以退出时要删除自己
     res_not_found_flag为假时，res writer应用是可能是用户点开的app，不删除app主线程
     请根据自己的需要进行调整
  */
  if(res_not_found_flag)
  {
    /* 部分操作系统在退出任务函数时，必须删除线程自己 */
    GUI_Thread_Delete(GUI_GetCurThreadHandle());
  }
}
