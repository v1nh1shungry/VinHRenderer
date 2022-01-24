#ifndef WIN32_H
#define WIN32_H

#include <windows.h>
#include <vector>
#include <array>

#include "config.h"
#include "sigslot.h"
#include "matrix.h"

// the most codes in here and 'win32.cpp' is not written by me, i just copy them by hand
// and pack them with a class 'ui' to use sigslot

namespace vinh
{
	struct MouseInfo
	{
		Vector2f orbitPos{};
		Vector2f orbitDelta{};
		Vector2f fvPos{};
		Vector2f fvDelta{};
		float wheelDelta{};
	};

	class UI;

	struct Window
	{
		HWND hWindow;
		HDC memDC;
		HBITMAP oldBM;
		HBITMAP dibBM;
		unsigned char* frameBuf;
		int width;
		int height;
		char keys[512];
		char buttons[2];
		bool shouldClose;
		MouseInfo* mouseInfo;
		UI* parent;
	};

	class UI : HASSLOT
	{
	public:
		static UI* getInstance(const char* title, int width = WIDTH, int height = HEIGHT);
		~UI();
		float getSysTime() const;
		bool run();

		SIGNAL<Vector2f, Vector2f, float, int, int> mouseEvents;
		SIGNAL<int> setShader;

		// slot
		void draw(const std::vector<std::array<unsigned char, 3>>& frame);

	private:
		UI(const char* title, int width, int height);
		void dispatch();
		void eventsHandler();
		static UI* instance;
	};
}

#endif
