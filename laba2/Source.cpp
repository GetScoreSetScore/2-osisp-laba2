#include <windows.h>
#include <tchar.h>
#include <wingdi.h>
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wcex; HWND hWnd; MSG msg;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_DBLCLKS;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = _T("HelloWorldClass");
	wcex.hIconSm = wcex.hIcon;
	RegisterClassEx(&wcex);
	hWnd = CreateWindow(_T("HelloWorldClass"), _T("Hello, World!"), WS_OVERLAPPEDWINDOW| WS_VSCROLL, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}
HBITMAP hBitmap = NULL;
HBITMAP Mask = NULL; 
#define TIMER1 1001
wchar_t text[10000] = L"";
int length = 0;
int N = 5;
int M = 3;
int CellWidth = 0;
int CellHeight = 0;

int nPos=0;
static void
CustomOnWmSize(HWND hWnd,UINT uHeight)
{
	SCROLLINFO si;

	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_PAGE;
	si.nPage = uHeight;
	SetScrollInfo(hWnd, SB_VERT, &si, TRUE);
}
static void Paint(HWND hWnd, LPPAINTSTRUCT lpPS)
{
	RECT rc,tmp;
	HDC BufferDC;
	HBITMAP BufferBackgroundBmp;
	GetClientRect(hWnd, &rc);

	BufferDC = CreateCompatibleDC(lpPS->hdc);
	BufferBackgroundBmp = CreateCompatibleBitmap(lpPS->hdc, rc.right - rc.left, rc.bottom - rc.top);
	SelectObject(BufferDC, BufferBackgroundBmp);
	FillRect(BufferDC, &rc, CreateSolidBrush(RGB(255, 255, 255)));

	rc.top -= nPos;
	rc.bottom -= nPos;
	int bottom=0;
	int tbottom = rc.top;
	int height = tbottom;
	for (int j = 0;j < N;j++)
	{
		for (int i = 0;i < M;i++) {
			SetRect(&tmp, i * CellWidth + 1, tbottom+1, (i + 1) * CellWidth - 1, 100000);
			bottom=DrawText(BufferDC, text, -1, &tmp, DT_WORDBREAK);
		}
		tbottom += bottom;
		MoveToEx(BufferDC, 0, tbottom, NULL);
		LineTo(BufferDC, rc.right, tbottom);

	}
	height = tbottom-rc.bottom;
	SCROLLINFO si;
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS | SIF_TRACKPOS;
	GetScrollInfo(hWnd, SB_VERT, &si);
	si.nMax = height;
	if (si.nPos > si.nMax)
		si.nPos = si.nMax;
	SetScrollInfo(hWnd, SB_VERT, &si,FALSE);
	for (int i = 0;i < M + 1;i++) {
		MoveToEx(BufferDC, i * CellWidth, 0, NULL);
		LineTo(BufferDC, i * CellWidth, tbottom);
	}
	GetClientRect(hWnd, &rc);
	BitBlt(lpPS->hdc, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, BufferDC, 0, 0, SRCCOPY);
	DeleteObject(BufferBackgroundBmp);
	DeleteDC(BufferDC);
}
BOOL IsTimed = 0;
int HorSpeed =10, VertSpeed=10;
wchar_t tmp0;
wchar_t* tmp1;
static void CustomHandleVScroll(HWND hwnd, int iAction)
{
	int nOldPos;
	SCROLLINFO si;

	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS | SIF_TRACKPOS;
	GetScrollInfo(hwnd, SB_VERT, &si);

	nOldPos = si.nPos;
	switch (iAction) {
	case SB_TOP:            nPos = si.nMin; break;
	case SB_BOTTOM:         nPos = si.nMax; break;
	case SB_LINEUP:         nPos = si.nPos - 10; break;
	case SB_LINEDOWN:       nPos = si.nPos + 10; break;
	case SB_THUMBPOSITION:  nPos = si.nPos; break;
	}
	SetScrollPos(hwnd, SB_VERT, nPos, TRUE);
	nPos = GetScrollPos(hwnd, SB_VERT);

	//ScrollWindowEx(hwnd, 0, (nOldPos - nPos),NULL, NULL, NULL, NULL, SW_ERASE | SW_INVALIDATE);
	RECT tmp;
	GetClientRect(hwnd, &tmp);
	InvalidateRect(hwnd, &tmp, FALSE);
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hDC;
	PAINTSTRUCT ps;
	static RECT rect;
	RECT tmp;
	switch (message)
	{
	case WM_VSCROLL:
		CustomHandleVScroll(hWnd, LOWORD(wParam));
		return 0;
	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);
		Paint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	case WM_MOUSEHWHEEL:
		GetClientRect(hWnd, &tmp);
		InvalidateRect(hWnd, &tmp, FALSE);
		if (LOWORD(wParam) == MK_SHIFT) {
			N += (short)HIWORD(wParam) / 120;
			if (N <= 0)
				N = 1;
		}
		else {
			M += (short)HIWORD(wParam) / 120;
			if (M <= 0)
				M = 1;
			CellWidth = (tmp.right - tmp.left) / M;
		}
		break;
	case WM_CHAR:
		GetClientRect(hWnd, &tmp);
		InvalidateRect(hWnd, &tmp, FALSE);
		tmp0 = static_cast<wchar_t>(wParam);
		tmp1 = &tmp0;
		wcscat_s(text, tmp1);
		break;
	case WM_SIZE:
		GetClientRect(hWnd, &tmp);
		CellWidth = (tmp.right - tmp.left) / M;
		InvalidateRect(hWnd, &tmp, FALSE);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_CREATE:
		GetClientRect(hWnd, &tmp);
		CellWidth = (tmp.right - tmp.left) / M;
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_LEFT:
			GetClientRect(hWnd, &tmp);
			InvalidateRect(hWnd, &tmp, FALSE);
			M--;
			if (M <= 0)
				M = 1;
			CellWidth = (tmp.right - tmp.left) / M;
			break;
		case VK_RIGHT:
			GetClientRect(hWnd, &tmp);
			InvalidateRect(hWnd, &tmp, FALSE);
			M++;
			CellWidth = (tmp.right - tmp.left) / M;
			break;
		case VK_DOWN:
			GetClientRect(hWnd, &tmp);
			InvalidateRect(hWnd, &tmp, FALSE);
			N++;
			break;
		case VK_UP:
			N--;
			GetClientRect(hWnd, &tmp);
			InvalidateRect(hWnd, &tmp, FALSE);
			if (N <= 0)
				N = 1;
			break;
		case VK_DELETE:
			GetClientRect(hWnd, &tmp);
			InvalidateRect(hWnd, &tmp, FALSE);
			if (wcslen(text) > 0) {
				text[wcslen(text) - 1] = '\0';
			}
			break;
		case VK_HOME:
			GetClientRect(hWnd, &tmp);
			InvalidateRect(hWnd, &tmp, FALSE);
			nPos--;
			break;
		case VK_END:
			GetClientRect(hWnd, &tmp);
			InvalidateRect(hWnd, &tmp, FALSE);
			nPos++;
			break;
		default:
			break;
		}

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}