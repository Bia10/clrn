#include "stdafx.h"

#include <stdexcept>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <fstream>

#include <windows.h>

#include <boost/bind.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>

class ISerializable
{
public:
	typedef boost::shared_ptr<ISerializable> Ptr;
	virtual void Serialize(std::ostream& stream) = 0;
	virtual void Deserialize(std::istream& stream) = 0;
};

template<typename T>
std::ostream& operator << (std::ostream& os, const std::vector<T>& data) 
{
 	BOOST_FOREACH(const T& value, data)
 		os << value;
	return os;
}

template<typename T>
std::istream& operator >> (std::istream& os, std::vector<T>& data) 
{
 	BOOST_FOREACH(T& value, data)
 		os >> value;
	return os;
}

template<typename T>
class Field : public ISerializable, boost::noncopyable
{
public:
	Field(T& field, const char* name, std::size_t version = 1)
		: m_Field(field)
		, m_Name(name)
		, m_Version(version) 
	{}

private:
	void Serialize(std::ostream& stream) override
	{
		stream << m_Name << " = '" << m_Field << "' ";
	}

	void Deserialize(std::istream& stream) override
	{
		stream >> m_Field;
	}

private:
	T& m_Field;
	std::string m_Name;
	std::size_t m_Version;
};

class Serializable : public ISerializable
{
	typedef std::vector<ISerializable::Ptr> Fields;
public:
	void Serialize(std::ostream& stream) override
	{
		if (m_Fields.empty())
			Register();

		BOOST_FOREACH(const ISerializable::Ptr& field, m_Fields)
			field->Serialize(stream);
	}

	void Deserialize(std::istream& stream) override
	{
		if (m_Fields.empty())
			Register();

		BOOST_FOREACH(const ISerializable::Ptr& field, m_Fields)
			field->Deserialize(stream);
	}

protected:

	template<typename T>
	void RegisterField(T& field, const char* name, std::size_t version = 1)
	{
		m_Fields.push_back(ISerializable::Ptr(new Field<T>(field, name, version)));
	}

private:

	virtual void Register() = 0;

private:

	Fields m_Fields;
};


#define SUSPENDABLE_BEGIN \
	virtual void Register() override {

#define SUSPENDABLE_END }

#define FIELD(name) RegisterField(name, #name);
#define NAMED_FIELD(field, name) RegisterField(field, name);
#define FIELD_FROM_VER(field, name, version) RegisterField(field, name, version);

class Test : public Serializable
{
	SUSPENDABLE_BEGIN
		FIELD(m_Int)
		NAMED_FIELD(m_String, "some_string")
		FIELD_FROM_VER(m_Strings, "strings", 2)
	SUSPENDABLE_END

public:
	Test()
	{

	}

//private:
	int m_Int;
	std::string m_String;
	std::vector<std::string> m_Strings;
};

// template< class A1, class M, class T >
// void bind( M T::*f, A1 a1 )
// {
// 
// }

void BytesToString(std::ostream& stream, const void* data, std::size_t size)
{
	for (std::size_t i = 0 ; i < size; ++i)
	{
		const unsigned char symbol = reinterpret_cast<const unsigned char*>(data)[i];
		stream 
			<< std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(symbol)
			<< std::setw(0) << "'" << std::dec;
		
		if (symbol)
			stream << symbol;
		else
			stream << "0";
		stream << "' ";
	}
}

const wchar_t POKERSTARS_TABLE_WINDOW_CLASS[] = L"PokerStarsTableFrameClass";

struct Windows
{
	typedef std::vector<HWND> List;

	List m_Matched;
	List m_NotMatched;
};

BOOL CALLBACK PokerWindowFinder(HWND hWnd, LPARAM lParam)
{
	Windows& wnds = *reinterpret_cast<Windows*>(lParam);
	wchar_t name[512] = {0};
	BOOL res = GetClassNameW(hWnd, name, _countof(name));
	if (!_wcsicmp(name, POKERSTARS_TABLE_WINDOW_CLASS))
		wnds.m_Matched.push_back(hWnd);
	else
		wnds.m_NotMatched.push_back(hWnd);

	return TRUE;
}

void errhandler(const char* szString, HWND wnd)
{

}

PBITMAPINFO CreateBitmapInfoStruct(HWND hwnd, HBITMAP hBmp)
{ 
	BITMAP bmp; 
	PBITMAPINFO pbmi; 
	WORD    cClrBits; 

	// Retrieve the bitmap color format, width, and height.  
	if (!GetObject(hBmp, sizeof(BITMAP), (LPSTR)&bmp)) 
		errhandler("GetObject", hwnd); 

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

void CreateBMPFile(HWND hwnd, LPTSTR pszFile, PBITMAPINFO pbi, 
				   HBITMAP hBMP, HDC hDC) 
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

	if (!lpBits) 
		errhandler("GlobalAlloc", hwnd); 

	// Retrieve the color table (RGBQUAD array) and the bits  
	// (array of palette indices) from the DIB.  
	if (!GetDIBits(hDC, hBMP, 0, (WORD) pbih->biHeight, lpBits, pbi, 
		DIB_RGB_COLORS)) 
	{
		errhandler("GetDIBits", hwnd); 
	}

	// Create the .BMP file.  
	hf = CreateFile(pszFile, 
		GENERIC_READ | GENERIC_WRITE, 
		(DWORD) 0, 
		NULL, 
		CREATE_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL, 
		(HANDLE) NULL); 
	if (hf == INVALID_HANDLE_VALUE) 
		errhandler("CreateFile", hwnd); 
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
	if (!WriteFile(hf, (LPVOID) &hdr, sizeof(BITMAPFILEHEADER), 
		(LPDWORD) &dwTmp,  NULL)) 
	{
		errhandler("WriteFile", hwnd); 
	}

	// Copy the BITMAPINFOHEADER and RGBQUAD array into the file.  
	if (!WriteFile(hf, (LPVOID) pbih, sizeof(BITMAPINFOHEADER) 
		+ pbih->biClrUsed * sizeof (RGBQUAD), 
		(LPDWORD) &dwTmp, ( NULL)))
		errhandler("WriteFile", hwnd); 

	// Copy the array of color indices into the .BMP file.  
	dwTotal = cb = pbih->biSizeImage; 
	hp = lpBits; 
	if (!WriteFile(hf, (LPSTR) hp, (int) cb, (LPDWORD) &dwTmp,NULL)) 
		errhandler("WriteFile", hwnd); 

	// Close the .BMP file.  
	if (!CloseHandle(hf)) 
		errhandler("CloseHandle", hwnd); 

	// Free memory.  
	GlobalFree((HGLOBAL)lpBits);
}

void Screen()
{
	Windows wnds;

	EnumWindows(&PokerWindowFinder, reinterpret_cast<LPARAM>(&wnds));

	HDC hdc = GetDCEx(wnds.m_Matched.front(), 0, DCX_WINDOW);
	HDC hDest = CreateCompatibleDC (hdc);

	RECT rt;
	GetWindowRect(wnds.m_Matched.front(), &rt);

	HBITMAP hBitmap = CreateCompatibleBitmap(hdc, rt.right - rt.left, rt.bottom - rt.top);
	HGDIOBJ res1 = SelectObject(hDest, hBitmap);
	BOOL res = BitBlt(hDest, 0, 0, rt.right - rt.left, rt.bottom - rt.top, hdc, 0, 0, SRCCOPY);
	
	PBITMAPINFO info = CreateBitmapInfoStruct(wnds.m_Matched.front(), hBitmap);
	CreateBMPFile(wnds.m_Matched.front(), L"D:\\test.bmp", info, hBitmap, hdc);



// 	HDC dc = GetDC(wnds.m_Matched.front());
// 	HDC hdcCompatible = CreateCompatibleDC(dc); 
// 
// 	// Create a compatible bitmap for hdcScreen. 
// 
// 	HBITMAP hbmScreen = CreateCompatibleBitmap
// 	(
// 		hdcCompatible, 
// 		GetDeviceCaps(hdcCompatible, HORZRES), 
// 		GetDeviceCaps(hdcCompatible, VERTRES)
// 	); 
// 
// 
// 	// Select the bitmaps into the compatible DC. 
// 
// 	HGDIOBJ res = SelectObject(hdcCompatible, hbmScreen); 
// 
// 	// Hide the application window. 
// 
// 	//ShowWindow(hwnd, SW_HIDE); 
// 
// 	//Copy color data for the entire display into a 
// 	//bitmap that is selected into a compatible DC. 
// 
// 	BOOL result = BitBlt
// 	(
// 		hdcCompatible, 
// 		0,
// 		0, 
// 		100,
// 		100, 
// 		dc, 
// 		0,
// 		0, 
// 		SRCCOPY
// 	); 


	// Redraw the application window. 

	//ShowWindow(hwnd, SW_SHOW);
}

int _tmain(int argc, _TCHAR* argv[])
{
	try
	{
		Screen();
		{
			std::ofstream ofs("D:\\1.txt");

			ofs << "test";
			ofs.flush();
			
		}

		std::ostringstream oss;

		std::vector<char> bytes;
		bytes.push_back('\0');
		bytes.push_back(255);
		bytes.push_back('A');
		bytes.push_back('v');
		bytes.push_back(253);
		bytes.push_back(252);

		BytesToString(oss, &bytes.front(), bytes.size());

		const std::string result = oss.str();


// 		Test t;
// 		t.m_Int = 100;
// 		t.m_String = "100";
// 		t.m_Strings.push_back("300");
// 
// 		std::stringstream oss;
// 
// 		t.Serialize(oss);
// 
// 		Test s;
// 		s.Deserialize(oss);

// 		boost::bind(&Test::m_Strings, &t);
// 		bind(&Test::m_Strings, &t);

	}
	catch (const std::exception& e)
	{	
		std::cout << e.what() << std::endl;
	}
	catch(...)
	{

	}

	return 0;
}

