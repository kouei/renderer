#ifndef _WIN32_UTILITY_
#define _WIN32_UTILITY_

#include <windows.h>

namespace WIN32_UTILITY
{
	struct WndClassObject
	{
		WndClassObject(HINSTANCE hInstance, LPCWSTR szAppName, WNDPROC WndProc) noexcept;
		WndClassObject() = delete;
		WndClassObject(const WndClassObject & rhs) = delete;
		WndClassObject(WndClassObject && rhs) = delete;
		WndClassObject & operator= (const WndClassObject & rhs) = delete;
		WndClassObject & operator= (WndClassObject && rhs) = delete;

		operator WNDCLASSEX() noexcept;
		operator const WNDCLASSEX() const noexcept;

		WNDCLASSEX wndclass;
	};

	struct HwndObject
	{
		HwndObject(HINSTANCE hInstance, LPCWSTR szAppName, HMENU hMenu, int iCmdShow) noexcept;
		HwndObject(const HwndObject & rhs) = delete;
		HwndObject(HwndObject && rhs) = delete;
		HwndObject & operator= (const HwndObject & rhs) = delete;
		HwndObject & operator= (HwndObject && rhs) = delete;

		using CHWND = const HWND;
		operator HWND() noexcept;
		operator CHWND() const noexcept;

		HWND hwnd;
	};

	struct HDCObject
	{
		HDCObject(HWND _hwnd) noexcept;
		~HDCObject() noexcept;
		HDCObject(const HDCObject & rhs) = delete;
		HDCObject(HDCObject && rhs) = delete;
		HDCObject & operator= (const HDCObject & rhs) = delete;
		HDCObject & operator= (HDCObject && rhs) = delete;

		HWND hwnd;
		PAINTSTRUCT ps;
		HDC hdc;
	};

	struct HMemDCObject
	{
		HMemDCObject(HDC real_hdc) noexcept;
		~HMemDCObject() noexcept;
		HMemDCObject(const HMemDCObject & rhs) = delete;
		HMemDCObject(HMemDCObject && rhs) = delete;
		HMemDCObject & operator= (const HMemDCObject & rhs) = delete;
		HMemDCObject & operator= (HMemDCObject && rhs) = delete;

		HDC hdcMem;
	};

	struct HBITMAPObject
	{
		HBITMAPObject() noexcept;
		HBITMAPObject(HDC real_hdc, int cx, int cy) noexcept;
		~HBITMAPObject() noexcept;
		HBITMAPObject(const HBITMAPObject & rhs) = delete;
		HBITMAPObject(HBITMAPObject && rhs) = delete;
		HBITMAPObject & operator= (const HBITMAPObject & rhs) = delete;
		HBITMAPObject & operator= (HBITMAPObject && rhs) = delete;

		HBITMAP hbitmap;
	};

	

};



#endif