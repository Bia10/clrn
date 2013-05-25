#ifndef Screenshot_h__
#define Screenshot_h__

#include <windows.h>
#include <string>

namespace pcmn
{
class Screenshot
{
public:
	Screenshot(HWND window);
	void Save(const std::wstring& fileName);
private:
	PBITMAPINFO CreateBitmapInfoStruct(HWND hwnd, HBITMAP hBmp);
	void CreateBMPFile(HWND hwnd, const wchar_t* pszFile, PBITMAPINFO pbi, HBITMAP hBMP, HDC hDC);
private:
	HWND m_Window;
};
}

#endif // Screenshot_h__
