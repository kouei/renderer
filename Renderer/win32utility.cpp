#include "win32utility.h"
#include "my_assert.h"

using namespace WIN32_UTILITY;

WndClassObject::WndClassObject(HINSTANCE hInstance, LPCWSTR szAppName, WNDPROC WndProc) noexcept
{
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = (HICON)LoadImage(NULL, TEXT("./resource/Rewrite.ico"), IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
	my_assert(wndclass.hIcon);
	wndclass.hIconSm = (HICON)LoadImage(NULL, TEXT("./resource/Rewrite.ico"), IMAGE_ICON, 16, 16, LR_LOADFROMFILE);
	my_assert(wndclass.hIconSm);
	wndclass.hCursor = (HCURSOR)LoadImage(NULL, TEXT("./resource/Rewrite.ani"), IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
	my_assert(wndclass.hCursor);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	my_assert(wndclass.hbrBackground);
	wndclass.lpszMenuName = TEXT("RENDERER_MENU");
	wndclass.lpszClassName = szAppName;
	wndclass.cbSize = sizeof(wndclass);

	my_assert(RegisterClassEx(&wndclass));
}
WndClassObject::operator WNDCLASSEX() noexcept { return wndclass; }
WndClassObject::operator const WNDCLASSEX() const noexcept { return wndclass; }

HwndObject::HwndObject(HINSTANCE hInstance, LPCWSTR szAppName, HMENU hMenu, int iCmdShow) noexcept
{
	hwnd = CreateWindowEx(NULL, szAppName, TEXT("Renderer"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, hMenu, hInstance, NULL);

	my_assert(hwnd);

	ShowWindow(hwnd, iCmdShow);
	my_assert(UpdateWindow(hwnd));
}
HwndObject::operator HWND() noexcept { return hwnd; }
HwndObject::operator const HWND() const noexcept { return hwnd; }



HDCObject::HDCObject(HWND _hwnd) noexcept
	: hwnd(_hwnd), ps(), hdc(BeginPaint(hwnd, &ps))
{
	my_assert(hdc);
	//do nothing
}
HDCObject::~HDCObject() noexcept
{
	EndPaint(hwnd, &ps);
}

HMemDCObject::HMemDCObject(HDC real_hdc) noexcept
	: hdcMem(CreateCompatibleDC(real_hdc))
{
	my_assert(hdcMem);
	//do nothing
}
HMemDCObject::~HMemDCObject() noexcept
{
	my_assert(DeleteDC(hdcMem));
}

HBITMAPObject::HBITMAPObject() noexcept
	: hbitmap(NULL)
{
	//do nothing
}
HBITMAPObject::HBITMAPObject(HDC real_hdc, int cx, int cy) noexcept
	: hbitmap(CreateCompatibleBitmap(real_hdc, cx, cy))
{
	my_assert(hbitmap);
	//do nothing
}
HBITMAPObject::~HBITMAPObject() noexcept
{
	if (hbitmap) my_assert(DeleteObject(hbitmap));
}


