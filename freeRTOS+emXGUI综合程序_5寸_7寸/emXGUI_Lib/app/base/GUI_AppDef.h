/**
  *********************************************************************
  * @file    GUI_AppDef.h
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   应用示例头文件
  **********************************************************************
  */ 
#ifndef	__GUI_APPDEF_H__
#define	__GUI_APPDEF_H__

#include "def.h"
#include "emXGUI.h"

/* 启动界面 */
/* 要加载外部字体的文件数目 */
#define FONT_NUM          (8 + 17 + 48 + 5 + 1) 

/* 外部资源加载完成标志 */
extern BOOL Load_state;

/* 启动界面的进度条句柄 */
extern HWND Boot_progbar;


#define ID_SLIDE_WINDOW   1200

#define HEAD_INFO_HEIGHT   90
#define WM_MSG_FRAME_DOWN  (WM_USER+50)

#define COLOR_DESKTOP_BACK_GROUND         215,61,50
#define COLOR_DESKTOP_BACK_GROUND_HEX     0xd73d32
#define COLOR_INVALID                       165,160,160
#define RGB888_COLOR_DESKTOP_BACK_GROUND   RGB888(215,61,50)
#define RGB888_COLOR_INVALID                 RGB888(165,160,160)
//#define COLOR_BACK_GRUND RGB888(215,61,50)

#define ICON_VIEWER_ID_PREV   0x1003
#define ICON_VIEWER_ID_NEXT		0x1004
#define ICON_VIEWER_ID_LIST   0x1005

extern int thread_ctrl;

/*************************** 兼容7系列字库 ************************/
#if defined(STM32H743xx) || defined(STM32F767xx)

	#if(GUI_ICON_LOGO_EN)
	///* logo字体 */
	//#define  logoFont
	//#define  logoFont100
	//#define  logoFont_200
	///* 图标字体 */
	//#define  iconFont_100
	//#define  iconFont_200
	//#define  iconFont_252
	/* 控制图标字体 */
	#define ctrlFont32  controlFont_32
	#define ctrlFont48  controlFont_48
	#define ctrlFont64  controlFont_64
	#define ctrlFont72  controlFont_72
	#define ctrlFont100 controlFont_100
	#endif

	#define ICON_Typedef icon_S

#define logoFont252 iconFont_252

#endif
/**************************************************************************/

#endif	/*__GUI_APPDEF_H__*/
