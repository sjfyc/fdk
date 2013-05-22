#include <windows.h>
#include <windowsx.h>

LRESULT CALLBACK wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

int WINAPI wWinMain(HINSTANCE hinst, HINSTANCE hprevinst, wchar_t *cmdline, int cmdshow)
{ 
  const wchar_t *classname  = L"My Demo Class";
  const wchar_t *wndcaption = L"My Demo Window";
  WNDCLASSW   wc;
  HWND        hwnd;
  MSG         msg;
  
  wc.style          = CS_HREDRAW | CS_VREDRAW;
  wc.lpfnWndProc    = wndproc;
  wc.cbClsExtra     = 0;
  wc.cbWndExtra     = 0;
  wc.hInstance      = hinst;
  wc.hIcon          = LoadIcon(NULL, IDI_APPLICATION);
  wc.hCursor        = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground  = (HBRUSH)GetStockObject(WHITE_BRUSH);
  wc.lpszMenuName   = NULL;
  wc.lpszClassName  = classname;
  
  if (!RegisterClassW(&wc))
    return -1;
  
  hwnd = CreateWindowW(
    classname, 
    wndcaption,
    WS_OVERLAPPEDWINDOW,
    0, 0, 640, 480,
    NULL, NULL, hinst, NULL);   
  if (hwnd == NULL)
    return -1;
    
  ShowWindow(hwnd, SW_SHOW);
 
  while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  
  return 0;
}

LRESULT CALLBACK wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  PAINTSTRUCT ps;
  HDC         hdc;
  
  switch (msg) {
    case WM_DESTROY:
      PostQuitMessage(0);
      break;     
    
    case WM_PAINT:
      hdc = BeginPaint(hwnd, &ps);
      TextOutW(hdc, 0, 0, L"Hello World!", (int)wcslen(L"Hello World!"));
      EndPaint(hwnd, &ps);
      break;
      
    default: 
      return DefWindowProc(hwnd, msg, wparam, lparam);     
      break;
  }
  return 0;  
}
