#pragma once
extern void DrawBmp(CDC *pDC, HBITMAP hBm, RECT *pRc, BOOL strech);
extern void DrawDib(CDC *pDC, HGLOBAL hGlobal, RECT *pRc, BOOL strech);
extern void DrawEMF(CDC *pDC, HENHMETAFILE hEmf, RECT *pRc, BOOL strech);
extern void DrawDropFiles(CDC *pDC, HGLOBAL hGlobal, RECT *pRc);

extern BOOL OwnerDisplay(HWND hWnd);
extern void OwnerSize(HWND hWnd);

extern void DrawHexa(CDC *pDC, HGLOBAL hGlobal, CString title, RECT *pRc);

extern void PutText(CDC *pDC, const CString &text, RECT *pRc, UINT uFormat = 0);
extern void PutBigText(CDC *pDC, LPCTSTR pBuf, DWORD len, RECT *pRc, UINT uFormat);

extern long getLength(LPTSTR pData, long size);
extern LPTSTR copyCvrt(PWORD pWord, long &size);
