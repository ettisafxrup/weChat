#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include <iomanip>
#include <limits>
#include <functional>

// ── Platform detection ────────────────────────────────────────────────────────
#ifdef _WIN32
#define WECHAT_WINDOWS
#include <windows.h> // Sleep(), GetStdHandle(), WaitForSingleObject()
#include <conio.h>   // _kbhit()
#include <direct.h>  // _mkdir()
#define MKDIR(d) _mkdir(d)
// Older Windows SDKs (before 10.0.10586) may not define this
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
#else
#define WECHAT_POSIX
#include <unistd.h>
#include <sys/select.h>
#include <sys/stat.h>
#define MKDIR(d) mkdir((d), 0755)
#endif

// ── Terminal Colors ───────────────────────────────────────────────────────────
// ANSI codes work on Linux/macOS natively.
// On Windows they work in Windows Terminal, VS Code terminal, and any modern
// terminal. If you use old CMD.exe, run:  reg add HKCU\Console /v VirtualTerminalLevel /t REG_DWORD /d 1
#define RESET "\033[0m"
#define BOLD "\033[1m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define CYAN "\033[36m"
#define MAGENTA "\033[35m"
#define BLUE "\033[34m"
#define WHITE "\033[37m"

namespace Utils
{

    // ── Enable ANSI on Windows (call once at startup) ────────────────────────
    inline void enableAnsiColors()
    {
#ifdef WECHAT_WINDOWS
        HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
        if (h != INVALID_HANDLE_VALUE)
        {
            DWORD mode = 0;
            if (GetConsoleMode(h, &mode))
                SetConsoleMode(h, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
        }
        // Set console code page to UTF-8
        SetConsoleOutputCP(65001);
        SetConsoleCP(65001);
#endif
    }

    // ── Screen ───────────────────────────────────────────────────────────────
    inline void clearScreen()
    {
#ifdef WECHAT_WINDOWS
        HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
        COORD origin = {0, 0};
        CONSOLE_SCREEN_BUFFER_INFO info;
        GetConsoleScreenBufferInfo(h, &info);
        DWORD written;
        DWORD cells = info.dwSize.X * info.dwSize.Y;
        FillConsoleOutputCharacter(h, ' ', cells, origin, &written);
        FillConsoleOutputAttribute(h, info.wAttributes, cells, origin, &written);
        SetConsoleCursorPosition(h, origin);
#else
        std::cout << "\033[2J\033[H";
        std::cout.flush();
#endif
    }

    inline void printLine(char c = '-', int n = 62)
    {
        for (int i = 0; i < n; ++i)
            std::cout << c;
        std::cout << '\n';
    }

    inline void printHeader(const std::string &title)
    {
        clearScreen();
        std::cout << CYAN;
        printLine('=');
        int pad = (62 - (int)title.size()) / 2;
        if (pad < 0)
            pad = 0;
        std::cout << std::string((size_t)pad, ' ') << BOLD << title << RESET << CYAN << '\n';
        printLine('=');
        std::cout << RESET;
    }

    // ── Time ─────────────────────────────────────────────────────────────────
    inline std::string getTimestamp()
    {
        time_t now = time(nullptr);
        char buf[20];
        strftime(buf, sizeof(buf), "%H:%M:%S", localtime(&now));
        return std::string(buf);
    }

    inline std::string getFullTimestamp()
    {
        time_t now = time(nullptr);
        char buf[20];
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));
        return std::string(buf);
    }

    // ── Sleep (cross-platform) ────────────────────────────────────────────────
    inline void sleepMs(int ms)
    {
#ifdef WECHAT_WINDOWS
        Sleep((DWORD)ms);
#else
        struct timeval tv;
        tv.tv_sec = ms / 1000;
        tv.tv_usec = (ms % 1000) * 1000;
        select(0, nullptr, nullptr, nullptr, &tv);
#endif
    }

    // ── Non-blocking stdin check ──────────────────────────────────────────────
    // Returns true if the user has pressed Enter (input ready) within timeout_ms.
    // On Windows uses _kbhit() + a busy-wait in small slices.
    // On POSIX uses select() which is the cleanest approach.
    inline bool stdinReady(int timeout_ms)
    {
#ifdef WECHAT_WINDOWS
        // Poll _kbhit() every 50 ms until timeout
        int elapsed = 0;
        while (elapsed < timeout_ms)
        {
            if (_kbhit())
                return true;
            Sleep(50);
            elapsed += 50;
        }
        return _kbhit();
#else
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds);
        struct timeval tv;
        tv.tv_sec = timeout_ms / 1000;
        tv.tv_usec = (timeout_ms % 1000) * 1000;
        return select(STDIN_FILENO + 1, &fds, nullptr, nullptr, &tv) > 0;
#endif
    }

    // ── Code generation ───────────────────────────────────────────────────────
    inline std::string generateCode(int len = 6)
    {
        const std::string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
        std::string code;
        srand((unsigned)time(nullptr) ^ (unsigned)(uintptr_t)&code);
        for (int i = 0; i < len; ++i)
            code += chars[rand() % (int)chars.size()];
        return code;
    }

    // ── String helpers ────────────────────────────────────────────────────────
    inline std::string trim(const std::string &s)
    {
        size_t a = s.find_first_not_of(" \t\r\n");
        size_t b = s.find_last_not_of(" \t\r\n");
        return (a == std::string::npos) ? "" : s.substr(a, b - a + 1);
    }

    inline void pressEnter(const std::string &msg = "Press Enter to continue...")
    {
        std::cout << YELLOW << "\n  " << msg << RESET;
        std::cout.flush();
        std::string dummy;
        std::getline(std::cin, dummy);
    }

    // ── Directory creation ────────────────────────────────────────────────────
    inline void ensureDir(const char *path)
    {
        MKDIR(path); // safe to call even if it already exists
    }

} // namespace Utils
