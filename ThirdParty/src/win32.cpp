#include "win32.h"
#include "log.h"

#include <cassert>
// #include <atlstr.h>

using namespace std;
using namespace vinh;

static Window* window = nullptr;
UI* UI::instance = nullptr;

static Vector2f getMousePos()
{
	POINT point;
	GetCursorPos(&point);
	ScreenToClient(window->hWindow, &point);
	return Vector2f(point.x, point.y);
}

static LRESULT CALLBACK msgCallback(HWND hWnd, UINT msg, WPARAM wParm, LPARAM lParm)
{
	switch (msg)
	{
	case WM_CLOSE:
		window->shouldClose = true;
		break;
	case WM_KEYDOWN:
		window->keys[wParm & 511] = 1;
		break;
	case WM_KEYUP:
		window->keys[wParm & 511] = 0;
		break;
	case WM_LBUTTONDOWN:
		window->mouseInfo->orbitPos = getMousePos();
		window->buttons[0] = 1;
		break;
	case WM_LBUTTONUP:
		window->buttons[0] = 0;
		break;
	case WM_RBUTTONDOWN:
		window->mouseInfo->fvPos = getMousePos();
		window->buttons[1] = 1;
		break;
	case WM_RBUTTONUP:
		window->buttons[1] = 0;
		break;
	case WM_MOUSEWHEEL:
		window->mouseInfo->wheelDelta = GET_WHEEL_DELTA_WPARAM(wParm) / (float)WHEEL_DELTA;
		break;
	default:
		return DefWindowProc(hWnd, msg, wParm, lParm);
	}
	return 0;
}

static void registerWindowClass()
{
	ATOM atom;
	WNDCLASS wc;
	wc.style = CS_BYTEALIGNCLIENT;
	wc.lpfnWndProc = (WNDPROC)msgCallback;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetModuleHandle(nullptr);
	wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = ("VinHRenderer");

	atom = RegisterClass(&wc);
	assert(atom != 0);
}

static void initBMHeader(BITMAPINFOHEADER& bi, int width, int height)
{
	memset(&bi, 0, sizeof(BITMAPINFOHEADER));
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = width;
	bi.biHeight = -height;
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = width * height * 4;
}

static void msgDispatch()
{
	MSG msg;
	while (true)
	{
		if (!PeekMessage(&msg, nullptr, 0, 0, PM_NOREMOVE)) break;
		if (!GetMessage(&msg, nullptr, 0, 0)) break;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

static int initWindow(const char* title, int width, int height)
{
	window = (Window*)malloc(sizeof(Window));
	assert(window != nullptr);
	memset(window, 0, sizeof(Window));
	window->mouseInfo = new MouseInfo;
	window->shouldClose = false;

	RECT rect{ 0, 0, width, height };
	int wx, wy, sx, sy;
	LPVOID ptr;
	HDC hDC;
	BITMAPINFOHEADER bi;

	registerWindowClass();

	window->hWindow = CreateWindow(("VinHRenderer"), (title),
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		0, 0, 0, 0, nullptr, nullptr, GetModuleHandle(nullptr), nullptr);
	assert(window != nullptr);

	initBMHeader(bi, width, height);

	hDC = GetDC(window->hWindow);
	window->memDC = CreateCompatibleDC(hDC);
	ReleaseDC(window->hWindow, hDC);

	window->dibBM = CreateDIBSection(window->memDC, (BITMAPINFO*)&bi, DIB_RGB_COLORS, &ptr, nullptr, 0);
	assert(window->dibBM != nullptr);

	window->oldBM = (HBITMAP)SelectObject(window->memDC, window->dibBM);
	window->frameBuf = (unsigned char*)ptr;

	window->height = height;
	window->width = width;

	AdjustWindowRect(&rect, GetWindowLong(window->hWindow, GWL_STYLE), 0);
	wx = rect.right - rect.left;
	wy = rect.bottom - rect.top;
	sx = (GetSystemMetrics(SM_CXSCREEN) - wx) / 2;
	sy = (GetSystemMetrics(SM_CYSCREEN) - wy) / 2;
	if (sy < 0) sy = 0;

	SetWindowPos(window->hWindow, nullptr, sx, sy, wx, wy, SWP_NOCOPYBITS | SWP_NOZORDER | SWP_SHOWWINDOW);
	SetForegroundWindow(window->hWindow);
	ShowWindow(window->hWindow, SW_NORMAL);

	msgDispatch();

	memset(window->frameBuf, 0, width * height * 4);
	memset(window->keys, 0, sizeof(unsigned char) * 512);
	return 0;
}

static int destroyWindow()
{
	if (window->memDC)
	{
		if (window->oldBM)
		{
			SelectObject(window->memDC, window->oldBM);
		}
		DeleteDC(window->memDC);
	}
	if (window->dibBM)
	{
		DeleteObject(window->dibBM);
	}
	if (window->hWindow)
	{
		CloseWindow(window->hWindow);
	}
	free(window);
	return 0;
}

static void windowDisplay()
{
	HDC hDC = GetDC(window->hWindow);
	BitBlt(hDC, 0, 0, window->width, window->height, window->memDC, 0, 0, SRCCOPY);
	ReleaseDC(window->hWindow, hDC);
}

static void windowDraw(const std::vector<std::array<unsigned char, 3>>& frame)
{
	for (int i = 0; i < window->height; ++i)
	{
		for (int j = 0; j < window->width; ++j)
		{
			int idx1 = i * window->width + j;
			int idx2 = idx1 * 4;
			window->frameBuf[idx2] = frame[idx1][2];
			window->frameBuf[idx2 + 1] = frame[idx1][1];
			window->frameBuf[idx2 + 2] = frame[idx1][0];
		}
	}
	windowDisplay();
}

static double getNativeTime()
{
	static double period = -1;
	LARGE_INTEGER counter;
	if (period < 0)
	{
		LARGE_INTEGER frequency;
		QueryPerformanceFrequency(&frequency);
		period = 1 / (double)frequency.QuadPart;
	}
	QueryPerformanceCounter(&counter);
	return counter.QuadPart * period;
}

static float getTime()
{
	static double initial = -1;
	if (initial < 0)
	{
		initial = getNativeTime();
	}
	return (float)(getNativeTime() - initial);
}

UI::UI(const char* title, int width, int height)
{
	initWindow(title, width, height);
}

UI::~UI()
{
	destroyWindow();
}

UI* UI::getInstance(const char* title, int width /* = WIDTH */, int height /* = HEIGHT */)
{
	if (!instance)
	{
		instance = new UI(title, width, height);
	}
	return instance;
}

float UI::getSysTime() const
{
	return getTime();
}

void UI::draw(const std::vector<std::array<unsigned char, 3>>& frame)
{
	windowDraw(frame);
}

void UI::dispatch()
{
	msgDispatch();
}

void UI::eventsHandler()
{
	// mouse events
	if (window->buttons[0])
	{
		auto curr = getMousePos();
		window->mouseInfo->orbitDelta = window->mouseInfo->orbitPos - curr;
		window->mouseInfo->orbitPos = curr;
	}
	if (window->buttons[1])
	{
		auto curr = getMousePos();
		window->mouseInfo->fvDelta = window->mouseInfo->fvPos - curr;
		window->mouseInfo->fvPos = curr;
	}
	mouseEvents(window->mouseInfo->orbitDelta, window->mouseInfo->fvDelta,
		window->mouseInfo->wheelDelta, window->width, window->height);
	// clear
	window->mouseInfo->orbitDelta = { 0.f, 0.f };
	window->mouseInfo->fvDelta = { 0.f, 0.f };
	window->mouseInfo->wheelDelta = 0.f;

	// keyboard events
	if (window->keys[VK_ESCAPE])
	{
		window->shouldClose = true;
	}
	if (window->keys[VK_SPACE])
	{
		static int type = 0;
		if (++type >= SHADER_NUMS) type -= SHADER_NUMS;
		setShader(type);
		window->keys[VK_SPACE] = 0;
	}
}

bool UI::run()
{
	eventsHandler();
	dispatch();
	return !window->shouldClose;
}