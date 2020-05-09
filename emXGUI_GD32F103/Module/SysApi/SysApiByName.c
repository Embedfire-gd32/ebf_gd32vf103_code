#include	"Include.h"

/*===============================================================================================================*/
/*===============================================================================================================*/
/*===============================================================================================================*/
typedef	struct	__api_tbl
{
	const	void	*addr;
	const	char 	*name;
}API_TBL;

#define	API_ENTRY	 (void*)



static	const	API_TBL	api_tbl[]={


    API_ENTRY    GetSystemMetrics,	"GetSystemMetrics",

    API_ENTRY    MapRGB332,		"MapRGB332",
    API_ENTRY    MapRGB565,		"MapRGB565",
    API_ENTRY    MapXRGB1555,	"MapXRGB1555",
    API_ENTRY    MapARGB1555,	"MapARGB1555",
    API_ENTRY    MapXRGB4444,	"MapXRGB4444",
    API_ENTRY    MapARGB4444,	"MapARGB4444",
    API_ENTRY    MapXRGB8888,	"MapXRGB8888",
    API_ENTRY    MapARGB8888,	"MapARGB8888",
    API_ENTRY    MapRGB,		"MapRGB",
    API_ENTRY    MapARGB,		"MapARGB",
    API_ENTRY    ToARGB,		"ToARGB",
    API_ENTRY    GetRGB,		"GetRGB",
    API_ENTRY    GetARGB,		"GetARGB",
    
    API_ENTRY    SetTextAlign    ,"SetTextAlign",
    API_ENTRY    GetTextAlign    ,"GetTextAlign",
    API_ENTRY    SetTextColor    ,"SetTextColor",
    API_ENTRY    GetTextColor    ,"GetTextColor",
 
    API_ENTRY	 SetDrawRect	,"SetDrawRect",
    API_ENTRY	 GetDrawRect	,"GetDrawRect",
    API_ENTRY    CreateSurface    	,"CreateSurface",
    API_ENTRY    DeleteSurface    	,"DeleteSurface",
    API_ENTRY    CreateDC    	,"CreateDC",
    API_ENTRY    CreateMemoryDC    	,"CreateMemoryDC",
    API_ENTRY    GetDCEx    ,"GetDCEx",
    API_ENTRY    GetDC    ,"GetDC",
    API_ENTRY    GetWindowDC    ,"GetWindowDC",
    API_ENTRY    ReleaseDC    ,"ReleaseDC",
    API_ENTRY    DeleteDC    ,"DeleteDC",

    API_ENTRY    LPtoDP    ,"LPtoDP",
    API_ENTRY    DPtoLP    ,"DPtoLP",
    API_ENTRY    MoveToEx    ,"MoveToEx",


    API_ENTRY    SetPenColor,	"SetPenColor",
    API_ENTRY    GetPenColor,	"GetPenColor",
    API_ENTRY    SetPenSize,	"SetPenSize",
    API_ENTRY    GetPenSize,	"GetPenSize",

    API_ENTRY    SetBrushColor,	"SetBrushColor",
    API_ENTRY    GetBrushColor,	"GetBrushColor",
	API_ENTRY    EnableAlpha,	"EnableAlpha",
	API_ENTRY    SetAlpha,			"SetAlpha",
	API_ENTRY    EnableAntiAlias,	"EnableAntiAlias",
  
  
    API_ENTRY    CopyBits		,"CopyBits",
    API_ENTRY    DCtoBitmap		,"DCtoBitmap",

	API_ENTRY	 CreateFont,	"CreateFont",
	API_ENTRY	 DeleteFont,	"DeleteFont",
	API_ENTRY	 SetFont,		"SetFont",
    API_ENTRY    GetTextExtent    ,"GetTextExtent",

    API_ENTRY    AlphaBlendColor    ,"AlphaBlendColor",
    API_ENTRY    ClrDisplay    ,"ClrDisplay",
    API_ENTRY    SetPixel    ,"SetPixel",
    API_ENTRY    GetPixel    ,"GetPixel",
    API_ENTRY    Line    ,"Line",
    API_ENTRY    LineTo    ,"LineTo",
    API_ENTRY    HLine    ,"HLine",
    API_ENTRY    VLine   ,"VLine",
    API_ENTRY    PolyLine    ,"PolyLine",

    API_ENTRY    DrawArc    ,"DrawArc",
    API_ENTRY    DrawEllipse    ,"DrawEllipse",
    API_ENTRY    FillEllipse    ,"FillEllipse",

    API_ENTRY    DrawCircle    ,"DrawCircle",
    API_ENTRY    FillCircle    ,"FillCircle",


    API_ENTRY    DrawRect    ,"DrawRect",
    API_ENTRY    FillRect    ,"FillRect",
    API_ENTRY    Draw3DRect    ,"Draw3DRect",
    API_ENTRY    Fill3DRect    ,"Fill3DRect",

    API_ENTRY    GradientFillRect    ,"GradientFillRect",

    API_ENTRY    DrawRoundRect    ,"DrawRoundRect",
    API_ENTRY    FillRoundRect    ,"FillRoundRect",
    API_ENTRY    DrawPolygon    ,"DrawPolygon",
    API_ENTRY    FillPolygon    ,"FillPolygon",
    API_ENTRY    FillFlood    ,"FillFlood",

    API_ENTRY    TextOut    ,"TextOut",
    API_ENTRY    DrawTextEx ,"DrawTextEx",

    API_ENTRY    BitBlt,			"BitBlt",
    API_ENTRY    StretchBlt,		"StretchBlt",
    API_ENTRY    TransparentBlt,	"TransparentBlt",
    API_ENTRY    AlphaBlend,		"AlphaBlend",
    
    API_ENTRY    AA_DrawLine,		"AA_DrawLine",
    API_ENTRY    AA_DrawBoldLine,	"AA_DrawBoldLine",

    API_ENTRY    DrawBitmap,	"DrawBitmap",
    API_ENTRY    ScaleBitmap,	"ScaleBitmap",
    API_ENTRY    RotateBitmap,	"RotateBitmap",

    API_ENTRY    BMP_GetInfo,	"BMP_GetInfo",
    API_ENTRY    BMP_GetInfoEx,	"BMP_GetInfoEx",
    API_ENTRY    BMP_Draw,		"BMP_Draw",
    API_ENTRY    BMP_DrawEx,	"BMP_DrawEx",

    API_ENTRY    GIF_Open,   		"GIF_Open",
    API_ENTRY    GIF_GetInfo,  		"GIF_GetInfo",
    API_ENTRY    GIF_GetFrameCount,  "GIF_GetFrame",
    API_ENTRY    GIF_GetFrameDelay,  "GIF_GetFrameDelay",
    API_ENTRY    GIF_DrawFrame,  	"GIF_DrawFrame",
    API_ENTRY    GIF_Close,  		"GIF_Close",


    API_ENTRY    RectX    ,"RectX",
    API_ENTRY    RectY    ,"RectY",
    API_ENTRY    RectW    ,"RectW",
    API_ENTRY    RectH    ,"RectH",
    API_ENTRY    SetRectW    ,"SetRectW",
    API_ENTRY    SetRectH    ,"SetRectH",
    API_ENTRY    PtInRect    ,"PtInRect",
    API_ENTRY    SetRect    ,"SetRect",
    API_ENTRY    SetRectArea    ,"SetRectArea",

    API_ENTRY    OffsetPoint,	"OffsetPoint",
    API_ENTRY    RotatePoint,	"RotatePoint",

    API_ENTRY    CopyRect    		,"CopyRect",
    API_ENTRY    OffsetRect    		,"OffsetRect",
    API_ENTRY    InflateRect    	,"InflateRect",
    API_ENTRY    InflateRectEx    	,"InflateRectEx",
    API_ENTRY    IsEqualRect    	,"IsEqualRect",
    API_ENTRY    IsCoveredRect 		,"IsCoveredRect",
    API_ENTRY    IsIntersectRect    ,"IsIntersectRect",
    API_ENTRY    IntersectRect    ,"IntersectRect",
    API_ENTRY    MakeMatrixRect    ,"MakeMatrixRect",
    API_ENTRY    MakeProgressRect    ,"MakeProgressRect",

    API_ENTRY    CreateRectRgnIndirect	,"CreateRectRgnIndirect",
    API_ENTRY    CreateRectRgn    		,"CreateRectRgn",

    API_ENTRY    SelectClipRgn    		,"SelectClipRgn",
    API_ENTRY    GetClipRgn    			,"GetClipRgn",


    API_ENTRY    SendMessage			,"SendMessage",
    API_ENTRY    DispatchMessage		,"DispatchMessage",
    API_ENTRY    PostAsyncMessage		,"PostAsyncMessage",
    API_ENTRY    PostMessage			,"PostMessage",
    API_ENTRY    PostCloseMessage		,"PostCloseMessage",
    API_ENTRY    PostQuitMessage		,"PostQuitMessage",
    API_ENTRY    PeekMessage			,"PeekMessage",
    API_ENTRY    GetMessage				,"GetMessage",
    API_ENTRY    TranslateMessage		,"TranslateMessage",

    API_ENTRY    SetWindowFont			,"SetWindowFont",
    API_ENTRY    GetWindowFont			,"GetWindowFont",
    API_ENTRY    SetWindowText			,"SetWindowText",
    API_ENTRY    GetWindowText			,"GetWindowText",
    API_ENTRY    GetWindowTextLength	,"GetWindowTextLength",

    API_ENTRY    SetWindowLong			,"SetWindowLong",
    API_ENTRY    GetWindowLong			,"GetWindowLong",
    API_ENTRY    SetWindowProc			,"SetWindowProc",
    API_ENTRY    GetWindowProc			,"GetWindowProc",

    API_ENTRY    GetDlgItem    			,"GetDlgItem",
    API_ENTRY    IsWindowVisible		,"IsWindowVisible",
    API_ENTRY    ScreenToClient    		,"ScreenToClient",
    API_ENTRY    ClientToScreen    		,"ClientToScreen",
    API_ENTRY    ScreenToWindow    		,"ScreenToWindow",
    API_ENTRY    WindowToScreen   		,"WindowToScreen",
    API_ENTRY    GetDesktopWindow   	,"GetDesktopWindow",
    API_ENTRY    GetWindowRect    		,"GetWindowRect",
    API_ENTRY    GetClientRect    		,"GetClientRect",
    API_ENTRY    IsWindowEnabled		,"IsWindowEnabled",
    API_ENTRY    EnableWindow    		,"EnableWindow",
    API_ENTRY    InvalidateRect    		,"InvalidateRect",
    API_ENTRY    SetWindowPos    		,"SetWindowPos",
    API_ENTRY    MoveWindow    			,"MoveWindow",
    API_ENTRY    ShowWindow    			,"ShowWindow",
    API_ENTRY    RedrawWindow    		,"RedrawWindow",

    API_ENTRY    GetWindow    			,"GetWindow",
    API_ENTRY    BeginPaint    			,"BeginPaint",
    API_ENTRY    EndPaint	    		,"EndPaint",	
    API_ENTRY    UpdateWindow    		,"UpdateWindow",

    API_ENTRY    SetFocus				,"SetFocus",
    API_ENTRY    GetFocus				,"SetFocus",
    API_ENTRY    SetCapture				,"SetCapture",
    API_ENTRY    GetCapture				,"GetCapture",
    API_ENTRY    ReleaseCapture			,"ReleaseCapture",
    API_ENTRY    GetForegroundWindow	,"GetForegroundWindow",
    API_ENTRY    SetForegroundWindow	,"SetForegroundWindow",
   
    API_ENTRY    GetParent    			,"GetParent",
    API_ENTRY    GetClassInfo    		,"GetClassInfo",
    API_ENTRY    DefWindowProc    		,"DefWindowProc",

    API_ENTRY    AdjustWindowRect  		,"AdjustWindowRect",
    API_ENTRY    AdjustWindowRectEx		,"AdjustWindowRectEx",
    API_ENTRY    CreateWindowEx    		,"CreateWindowEx",
    API_ENTRY    DestroyWindow    		,"DestroyWindow",
   
    API_ENTRY    SetTimer				,"SetTimer",
    API_ENTRY    ResetTimer				,"ResetTimer",
    API_ENTRY    KillTimer				,"KillTimer",

    API_ENTRY    GetKeyState    		,"GetKeyState",
    API_ENTRY    GetAsyncKeyState    	,"GetAsyncKeyState",
    API_ENTRY    GetKeyboardState    	,"GetKeyboardState",

    API_ENTRY    DialogWindowEx			,"DialogWindowEx",
    API_ENTRY    MessageBoxEx    		,"MessageBoxEx",
  
    API_ENTRY    ShowCursor				,"ShowCursor",
    API_ENTRY    SetCursor				,"SetCursor",
    API_ENTRY    ClipCursor				,"ClipCursor",
    API_ENTRY    SetCursorPos			,"SetCursorPos",
    API_ENTRY    GetCursorPos			,"GetCursorPos",
   
	API_ENTRY    XFT_CreateFont			,"XFT_CreateFont",
	API_ENTRY    XFT_CreateFontEx		,"XFT_CreateFontEx",
    
	API_ENTRY	NULL,(const char*)0xFFFFFFFF,
	
};


/*=========================================================================================*/

/*=========================================================================================*/

static	int	dummy(void)
{
	//DebugPuts("dummy\r\n");
	return 0;
}


/*=========================================================================================*/
void*	GetSysApiByName(const char *api_name)
{
   	int i;
	char buf[64];
	
   	for(i=0; api_tbl[i].name!=(const char*)0xFFFFFFFF; i++)
	{
	  if(x_strcmp(api_tbl[i].name,api_name)==0)
	  {
	  //	DebugPrintf("GetSysApiByName: %08XH,'%s'.\r\n",api_tbl[i].addr,api_name);

	  	return (void*)api_tbl[i].addr;
	  }
	} 
	x_sprintf(buf,"GetSysApiByName Err: '%s'.\r\n",api_name);
	DebugPuts(buf);
	return (void*)NULL;
}
/*===============================================================================================================*/
/*===============================================================================================================*/
/*===============================================================================================================*/
/*===============================================================================================================*/
/*===============================================================================================================*/
/*===============================================================================================================*/
/*===============================================================================================================*/
