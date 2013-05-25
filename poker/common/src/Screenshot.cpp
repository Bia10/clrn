#include "Screenshot.h"
#include "Exception.h"

#include <boost/scope_exit.hpp>

namespace pcmn
{
Screenshot::Screenshot(HWND window) : m_Window(window)
{

}

void Screenshot::Save(const std::wstring& fileName)
{
	HDC hdc = GetDCEx(m_Window, 0, DCX_WINDOW);
	HDC hDest = CreateCompatibleDC (hdc);

	RECT rt;
	GetWindowRect(m_Window, &rt);

	HBITMAP hBitmap = CreateCompatibleBitmap(hdc, rt.right - rt.left, rt.bottom - rt.top);
	HGDIOBJ res1 = SelectObject(hDest, hBitmap);
	BOOL res = BitBlt(hDest, 0, 0, rt.right - rt.left, rt.bottom - rt.top, hdc, 0, 0, SRCCOPY);

	PBITMAPINFO info = CreateBitmapInfoStruct(m_Window, hBitmap);
	CreateBMPFile(m_Window, fileName.c_str(), info, hBitmap, hdc);
}

PBITMAPINFO Screenshot::CreateBitmapInfoStruct(HWND hwnd, HBITMAP hBmp)
{
	BITMAP bmp; 
	PBITMAPINFO pbmi; 
	WORD    cClrBits; 

	// Retrieve the bitmap color format, width, and height.  
	CHECK(GetObject(hBmp, sizeof(BITMAP), (LPSTR)&bmp));

	// Convert the color format to a count of bits.  
	cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel); 
	if (cClrBits == 1) 
		cClrBits = 1; 
	else if (cClrBits <= 4) 
		cClrBits = 4; 
	else if (cClrBits <= 8) 
		cClrBits = 8; 
	else if (cClrBits <= 16) 
		cClrBits = 16; 
	else if (cClrBits <= 24) 
		cClrBits = 24; 
	else cClrBits = 32; 

	// Allocate memory for the BITMAPINFO structure. (This structure  
	// contains a BITMAPINFOHEADER structure and an array of RGBQUAD  
	// data structures.)  

	if (cClrBits < 24) 
		pbmi = (PBITMAPINFO) LocalAlloc(LPTR, 
		sizeof(BITMAPINFOHEADER) + 
		sizeof(RGBQUAD) * (1<< cClrBits)); 

	// There is no RGBQUAD array for these formats: 24-bit-per-pixel or 32-bit-per-pixel 

	else 
		pbmi = (PBITMAPINFO) LocalAlloc(LPTR, 
		sizeof(BITMAPINFOHEADER)); 

	// Initialize the fields in the BITMAPINFO structure.  

	pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER); 
	pbmi->bmiHeader.biWidth = bmp.bmWidth; 
	pbmi->bmiHeader.biHeight = bmp.bmHeight; 
	pbmi->bmiHeader.biPlanes = bmp.bmPlanes; 
	pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel; 
	if (cClrBits < 24) 
		pbmi->bmiHeader.biClrUsed = (1<<cClrBits); 

	// If the bitmap is not compressed, set the BI_RGB flag.  
	pbmi->bmiHeader.biCompression = BI_RGB; 

	// Compute the number of bytes in the array of color  
	// indices and store the result in biSizeImage.  
	// The width must be DWORD aligned unless the bitmap is RLE 
	// compressed. 
	pbmi->bmiHeader.biSizeImage = ((pbmi->bmiHeader.biWidth * cClrBits +31) & ~31) /8
		* pbmi->bmiHeader.biHeight; 
	// Set biClrImportant to 0, indicating that all of the  
	// device colors are important.  
	pbmi->bmiHeader.biClrImportant = 0; 
	return pbmi; 
}

void Screenshot::CreateBMPFile(HWND hwnd, const wchar_t* pszFile, PBITMAPINFO pbi, HBITMAP hBMP, HDC hDC)
{
	HANDLE hf;                 // file handle  
	BITMAPFILEHEADER hdr;       // bitmap file-header  
	PBITMAPINFOHEADER pbih;     // bitmap info-header  
	LPBYTE lpBits;              // memory pointer  
	DWORD dwTotal;              // total count of bytes  
	DWORD cb;                   // incremental count of bytes  
	BYTE *hp;                   // byte pointer  
	DWORD dwTmp; 

	pbih = (PBITMAPINFOHEADER) pbi; 
	lpBits = (LPBYTE) GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);

	BOOST_SCOPE_EXIT((lpBits))
	{
		GlobalFree((HGLOBAL)lpBits);
	} BOOST_SCOPE_EXIT_END

	CHECK(lpBits);

	// Retrieve the color table (RGBQUAD array) and the bits  
	// (array of palette indices) from the DIB.  
	CHECK(GetDIBits(hDC, hBMP, 0, (WORD) pbih->biHeight, lpBits, pbi, DIB_RGB_COLORS)); 


	// Create the .BMP file.  
	hf = CreateFileW(pszFile, 
		GENERIC_READ | GENERIC_WRITE, 
		(DWORD) 0, 
		NULL, 
		CREATE_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL, 
		(HANDLE) NULL); 

	CHECK(hf != INVALID_HANDLE_VALUE);
	hdr.bfType = 0x4d42;        // 0x42 = "B" 0x4d = "M"  
	// Compute the size of the entire file.  
	hdr.bfSize = (DWORD) (sizeof(BITMAPFILEHEADER) + 
		pbih->biSize + pbih->biClrUsed 
		* sizeof(RGBQUAD) + pbih->biSizeImage); 
	hdr.bfReserved1 = 0; 
	hdr.bfReserved2 = 0; 

	// Compute the offset to the array of color indices.  
	hdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) + 
		pbih->biSize + pbih->biClrUsed 
		* sizeof (RGBQUAD); 

	// Copy the BITMAPFILEHEADER into the .BMP file.  
	CHECK(WriteFile(hf, (LPVOID) &hdr, sizeof(BITMAPFILEHEADER), (LPDWORD) &dwTmp,  NULL)); 

	// Copy the BITMAPINFOHEADER and RGBQUAD array into the file.  
	CHECK(WriteFile(hf, (LPVOID) pbih, sizeof(BITMAPINFOHEADER) 
		+ pbih->biClrUsed * sizeof (RGBQUAD), 
		(LPDWORD) &dwTmp, ( NULL)));

	// Copy the array of color indices into the .BMP file.  
	dwTotal = cb = pbih->biSizeImage; 
	hp = lpBits; 
	CHECK(WriteFile(hf, (LPSTR) hp, (int) cb, (LPDWORD) &dwTmp,NULL));

	// Close the .BMP file.  
	CHECK(CloseHandle(hf)); 
}

}
