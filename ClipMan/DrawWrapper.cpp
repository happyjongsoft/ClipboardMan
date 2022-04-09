#include "stdafx.h"
#include "DrawWrapper.h"

///////////////////////////////////////////////////////////////////////////////

static BOOL unico = TRUE;
static BOOL Strech(RECT *pRc, int bmWidth, int bmHeight, int &width, int &height);
static CString strCvrt(PBYTE pData, long size, BOOL single);
static LPCTSTR FormatCompression(DWORD compression);
static void ErrorMsg(CDC *pDC, RECT *pRc);

///////////////////////////////////////////////////////////////////////////////
// Displays the picture stored in the specified HBIMAP

void DrawBmp(CDC *pDC, HBITMAP hBm, RECT *pRc, BOOL strech)
{
	HDC hdcMem = CreateCompatibleDC(pDC->m_hDC);
	int width, height, right = pRc->right;
	CString msg;
	BITMAP bm;

	::SelectObject(hdcMem, hBm);
	::GetObject(hBm, sizeof(bm), &bm);

	msg.Format("Bitmap %d x %d pixels", bm.bmWidth, bm.bmHeight);
	PutText(pDC, msg, pRc);

	if (strech && Strech(pRc, bm.bmWidth, bm.bmHeight, width, height)) {
		::StretchBlt(pDC->m_hDC, pRc->left, pRc->top, width, height,
			hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
	}
	else {
		::BitBlt(pDC->m_hDC, pRc->left, pRc->top, bm.bmWidth, bm.bmHeight,
			hdcMem, 0, 0, SRCCOPY);

		if (right > (pRc->right = pRc->left + bm.bmWidth)) pRc->right = right;
		pRc->bottom = pRc->top + bm.bmHeight;
	}
	::DeleteDC(hdcMem);
}
///////////////////////////////////////////////////////////////////////////////
// Displays the picture stored in the specified DIB bitmap

void DrawDib(CDC *pDC, HGLOBAL hGlobal, RECT *pRc, BOOL strech)
{
#define PALSIZE(x) ((x) > 8? 0: 1 << (x))
	BITMAPINFOHEADER* pBih = (BITMAPINFOHEADER*)::GlobalLock(hGlobal);
	int width, height, right = pRc->right;
	CString msg;
	BITMAP bm;

	if (pBih->biSize == sizeof(BITMAPCOREHEADER)) {
		bm.bmWidth = ((BITMAPCOREHEADER*)pBih)->bcWidth;
		bm.bmHeight = ((BITMAPCOREHEADER*)pBih)->bcHeight;
		bm.bmBitsPixel = ((BITMAPCOREHEADER*)pBih)->bcBitCount;
		bm.bmType = BI_RGB;
		bm.bmBits = PBYTE(pBih) + pBih->biSize + PALSIZE(bm.bmBitsPixel) * 3;
	}
	else {
		bm.bmWidth = pBih->biWidth;
		bm.bmHeight = pBih->biHeight;
		bm.bmBitsPixel = pBih->biBitCount;
		bm.bmType = pBih->biCompression;
		long color = pBih->biClrUsed ? pBih->biClrUsed : PALSIZE(bm.bmBitsPixel);
		bm.bmBits = PBYTE(pBih) + pBih->biSize + color * 4;
	}

	msg.Format("Dib %d x %d pixels, %d bits/pixel %s",
		bm.bmWidth, bm.bmHeight, bm.bmBitsPixel, FormatCompression(bm.bmType));
	PutText(pDC, msg, pRc);

	if (strech && Strech(pRc, bm.bmWidth, bm.bmHeight, width, height)) {

		::StretchDIBits(pDC->m_hDC,
			pRc->left, pRc->top,// xy-coordinates of upper-left corner of dest. rect.
			width, height,// width & height of destination rectangle
			0, 0,// xy-coordinates of lower-left corner of source rect.
			bm.bmWidth, bm.bmHeight,// width & height of source rectangle
			bm.bmBits,// address of array with DIB bits
			(BITMAPINFO*)pBih,// address of structure with bitmap info
			DIB_RGB_COLORS,// usage flags
			SRCCOPY);// raster operation code

	}
	else {

		::SetDIBitsToDevice(pDC->m_hDC,
			pRc->left, pRc->top,// xy-coordinates of upper-left corner of dest. rect.
			bm.bmWidth, bm.bmHeight,// source rectangle width & height
			0, 0,// xy-coordinates of lower-left corner of source rect.
			0,// first scan line in array
			bm.bmHeight,// number of scan lines
			bm.bmBits,// address of array with DIB bits
			(BITMAPINFO*)pBih,// address of structure with bitmap info
			DIB_RGB_COLORS);// usage flags

		if (right > (pRc->right = pRc->left + bm.bmWidth)) pRc->right = right;
		pRc->bottom = pRc->top + bm.bmHeight;
	}
	::GlobalUnlock(hGlobal);
}
///////////////////////////////////////////////////////////////////////////////
// Displays the picture stored in the specified enhanced-format metafile

void DrawEMF(CDC *pDC, HENHMETAFILE hEmf, RECT *pRc, BOOL strech)
{
	int width, height, emWidth, emHeight;
	ENHMETAHEADER emh;
	CString msg;

	// read a picture size
	::GetEnhMetaFileHeader(hEmf, sizeof(ENHMETAHEADER), &emh);
	emWidth = emh.rclBounds.right - emh.rclBounds.left;
	emHeight = emh.rclBounds.bottom - emh.rclBounds.top;

	// Displays a picture informations
	UINT len = ::GetEnhMetaFileDescription(hEmf, 0, NULL);
	LPTSTR buffer = (LPTSTR)malloc(len + 1);
	::GetEnhMetaFileDescription(hEmf, len, buffer);
	buffer[len] = TCHAR('\0');
	msg.Format("Metafile %d x %d pixels %s", emWidth, emHeight, buffer);
	PutText(pDC, msg, pRc);
	free(buffer);

	// Comput a size of displayed picture
	int right = pRc->right;
	if (strech && Strech(pRc, emWidth, emHeight, width, height)) {
		pRc->right = pRc->left + width;
		pRc->bottom = pRc->top + height;
	}
	else {
		pRc->right = pRc->left + emWidth;
		pRc->bottom = pRc->top + emHeight;
	}

	// Displays picture
	::PlayEnhMetaFile(pDC->m_hDC, hEmf, pRc);
	if (right > pRc->right) pRc->right = right;
}
///////////////////////////////////////////////////////////////////////////////
// Display a list of Drop Files

void DrawDropFiles(CDC *pDC, HGLOBAL hGlobal, RECT *pRc)
{
	DROPFILES* pDf = (DROPFILES*)GlobalLock(hGlobal);
	CString buffer;
	TCHAR c;

	if (pDf->fWide) {
		// The file contains Unicode characters
		PWORD pW = PWORD(PBYTE(pDf) + pDf->pFiles);

		buffer = "CF_HDROP Unicode characters :";
		// Extract a file names
		while (*pW) {
			buffer += "\r\n ";
			while (c = (TCHAR)*pW++) buffer += c;
		}
	}
	else {
		// The file contains ANSI characters
		PBYTE pB = PBYTE(PBYTE(pDf) + pDf->pFiles);

		buffer = "CF_HDROP ANSI characters :";
		// Extract a file names
		while (*pB) {
			buffer += "\r\n ";
			while (c = (TCHAR)*pB++) buffer += c;
		}
	}
	PutText(pDC, buffer, pRc);
	::GlobalUnlock(hGlobal);
}
///////////////////////////////////////////////////////////////////////////////
// Owner Display

BOOL OwnerDisplay(HWND hWnd)
{
	HWND hwndOwner = ::GetClipboardOwner();
	BOOL err = TRUE;

	if (hwndOwner) {
		HGLOBAL hGlobal = ::GlobalAlloc(GMEM_DDESHARE, sizeof(PAINTSTRUCT));
		PAINTSTRUCT* pPs = (PAINTSTRUCT*)::GlobalLock(hGlobal);

		// Prepares the specified window for painting and fills a PAINTSTRUCT
		//    structure with information about the painting 
		::BeginPaint(hWnd, pPs);
		::GlobalUnlock(hGlobal);

		// Send the painting message at the Clipboard owner
		err = ::SendMessage(hwndOwner, WM_PAINTCLIPBOARD,
			(WPARAM)hWnd, (LPARAM)hGlobal);
		// The end of painting in the specified window
		::EndPaint(hWnd, pPs);
		::GlobalFree(hGlobal);
	}
	return err;
}

void OwnerSize(HWND hWnd)
{
	HWND hwndOwner = ::GetClipboardOwner();
	if (hwndOwner) {
		HGLOBAL hGlobal = ::GlobalAlloc(GMEM_DDESHARE, sizeof(RECT));

		// Get window rect and send it
		::GetClientRect(hWnd, (RECT*)::GlobalLock(hGlobal));
		::GlobalUnlock(hGlobal);

		::SendMessage(hwndOwner, WM_SIZECLIPBOARD, (WPARAM)hWnd, (LPARAM)hGlobal);

		::GlobalFree(hGlobal);
	}
}
///////////////////////////////////////////////////////////////////////////////
// Display a buffer contents in hexadecimal format

void DrawHexa(CDC *pDC, HGLOBAL hGlobal, CString title, RECT *pRc)
{
	CString msg, tmp, line[2];
	long cnt, n, max, addr = 0;
	PWORD pData = NULL, pBuf = NULL;
	long size;

	// Read the data corresponding the HGLOBAL handle
	try {
		if ((size = ::GlobalSize(hGlobal)) > 0) {
			pData = (PWORD)::GlobalLock(hGlobal);

			// it's a gdi object ?
		}
		else if ((size = ::GetObject(hGlobal, 0, NULL)) > 0) {
			pBuf = pData = (PWORD)malloc(size);
			::GetObject(hGlobal, size, pData);
		}
		else if ((size = ::GetEnhMetaFileBits((HENHMETAFILE)hGlobal, 0, NULL)) > 0) {
			pBuf = pData = (PWORD)malloc(size);
			::GetEnhMetaFileBits((HENHMETAFILE)hGlobal, size, (PBYTE)pData);
		}
		else {
			PutText(pDC, "*** Clipboard not hexa reading ***", pRc);
			return;
		}
		BYTE b = PBYTE(pData)[0] + PBYTE(pData)[size];  // validity check
	}
	catch (...) {
		PutText(pDC, "*** Clipboard reading error ***", pRc);
		return;
	}

	// Display a title
	msg.Format("Clipboard type \'%s\', size = %.3f Ko", title, double(size) / 1000.);
	PutText(pDC, msg, pRc);

	// Loop to code in hexa format the buffer
	while (size > 0) {
		size -= (cnt = (size > 16 * 8 ? 16 * 8 : size));
		pRc->top += 6;
		msg.Empty();
		while (cnt > 0) {
			// Format a block of data 16 x 8 bytes (max size)
			line[0].Empty(); line[1].Empty();
			cnt -= (n = (cnt > 16 ? 16 : cnt)), max = (n + 1) / 2;
			for (int i = 0; i < max; i++) {
				tmp.Format("%04X ", pData[i]);
				line[i / 4] += tmp;
			}
			// Comput a line dump formated text
			tmp.Format("%6X:   %-20.20s %-20.20s   %s%s", addr, line[0], line[1],
				strCvrt((PBYTE)pData, n, unico), cnt ? "\n" : "");
			msg += tmp;
			addr += 16;
			pData += 8;
		}
		PutText(pDC, msg, pRc);

		// Limit a length of display buffer
		if (addr > (100 - 8) * 16 && size > 8 * 16) { // bytes of data values
			// Computs a omited data in bytes
			long omited = (size - 16 * 8) & 0xFFFFFF80;
			// Advance ptr at the end of the buffer
			addr += omited;
			size -= omited;
			pData += omited / 2;          // word ptr
			PutText(pDC, "   ...", pRc);  // mark of ommited data
		}
	}

	// Free or unlock a buffer
	if (pData) {
		if (pBuf) free(pBuf);
		else ::GlobalUnlock(hGlobal);
	}
}
///////////////////////////////////////////////////////////////////////////////
// Display textes

void PutText(CDC *pDC, const CString &text, RECT *pRc, UINT uFormat)
{
	int right = pRc->right, bottom = pRc->bottom;
	pDC->DrawText(text, pRc, uFormat | DT_CALCRECT);
	pDC->DrawText(text, pRc, uFormat);

	// update the out rect
	pRc->top = pRc->bottom;
	if (right > pRc->right) pRc->right = right;
	if (bottom > pRc->bottom) pRc->bottom = bottom;
}

void PutBigText(CDC *pDC, LPCTSTR pBuf, DWORD len, RECT *pRc, UINT uFormat)
{
#define DLIMIT 16000 // pixels value

	pDC->DrawText(pBuf, len, pRc, uFormat | DT_CALCRECT);
	if (pRc->right > 5000) pRc->right = 5100;

	// Test if length of the text is bigger ! the scoll bar are limited !
	if ((pRc->bottom - pRc->top) > DLIMIT) {
		CRect rect(pRc->left, pRc->top, pRc->right, pRc->top);
		CString end = CString("...") + CString(pBuf + len - 300, 300);

		// too big, display only the beginning and the end of the text
		pDC->DrawText(end, rect, uFormat | DT_CALCRECT);
		pRc->bottom = pRc->top + DLIMIT - rect.Height() - 100;

		// Display the beginning
		pDC->DrawText(pBuf, len, pRc, uFormat);
		pRc->top = pRc->bottom;

		// Display the end
		PutText(pDC, "........", pRc, uFormat);
		PutText(pDC, end, pRc, uFormat);

	}
	else {

		// Display the entire text
		pDC->DrawText(pBuf, len, pRc, uFormat);
		pRc->top = pRc->bottom;
	}
}
///////////////////////////////////////////////////////////////////////////////
// Subroutines

BOOL Strech(RECT *pRc, int bmWidth, int bmHeight, int &width, int &height)
{
	int w = pRc->right - pRc->left, h = pRc->bottom - pRc->top;// size of display
	BOOL strech;

	// Computs a picture size with the same aspect
	if (strech = (bmWidth > w)) width = w, height = MulDiv(bmHeight, w, bmWidth);
	else width = bmWidth, height = bmHeight;
	if (height > h) width = MulDiv(bmWidth, h, bmHeight), height = h, strech = TRUE;

	return strech;
}

LPCTSTR FormatCompression(DWORD compression)
{
	switch (compression) {
	case BI_RGB:     return "RGB";
	case BI_RLE4:    return "RLE4";
	case BI_RLE8:    return "RLE8";
	case BI_BITFIELDS:return "BITFIELDS";
	case 4:          return "JPEG";
	case 5:          return "PNG";
	default:         return "";
	}
}

CString strCvrt(PBYTE pData, long size, BOOL single)
{
	CString buffer;

	// Convert all unprintable characters
	while (--size >= 0) {
		TCHAR car = TCHAR(*pData++);

		if (single && (car || (DWORD(pData) & 1)))
			if(isprint(unsigned char(car)))
				buffer += CString(car);
			else
				buffer += CString("-");
	}
	return buffer;
}

long getLength(LPTSTR pData, long size)
{
	long cnt = size;
	while (cnt > 0 && *pData++) --cnt;
	return size - cnt;
}

LPTSTR copyCvrt(PWORD pWord, long &size)
{
	long cnt = size;
	LPTSTR pByte = (LPTSTR)malloc(cnt), pTmp = pByte;
	if (pByte) {
		while (cnt > 0 && (*pTmp++ = TCHAR(*pWord++))) --cnt;
	}
	size -= cnt;
	return pByte;
}

void ErrorMsg(CDC *pDC, RECT *pRc)
{
	pDC->DrawText("Clipboard acces error", pRc, 0);
}