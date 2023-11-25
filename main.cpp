#include <Windows.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <filesystem>
#include <cmath>
#include <map>
#include <windows.h>
#include <string>
#include <iostream>

using namespace std;

map<string, string> ParseIniFile()
{
    map<string, string> data;
    ifstream file2("test.txt");
    if (!file2)
    {
        return data;
    }

    std::string currentSection;
    std::string s1, s2;
    while (getline(file2, s1, '='))
    {
        getline(file2, s2);
        data[s1] = s2;
    }
    return data;
}

long long getMillis()
{
    auto currentTime = std::chrono::system_clock::now();
    auto millisecondsSinceEpoch = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime.time_since_epoch());
    long long millisecondsCount = millisecondsSinceEpoch.count();
    return millisecondsCount;
}

long long lastMove = getMillis();

bool executed = false;

int lastX = -1, lastY = -1;

string command1 = "java";
string command2 = "";

LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION)
    {
        MSLLHOOKSTRUCT* pMouseStruct = reinterpret_cast<MSLLHOOKSTRUCT*>(lParam);
        int x = pMouseStruct->pt.x;
        int y = pMouseStruct->pt.y;

        switch (wParam)
        {
        case WM_MOUSEMOVE:
        {
            if (lastX != -1)
                if (abs(lastX - x) > 1 || abs(lastY - y) > 1)
                {
                    if (executed)
                    {
                        cout << "execute 2nd\n";
                        system(command2.c_str());
                    }
                    executed = false;
                    lastMove = getMillis();
                }
            break;
        }
        case WM_LBUTTONDOWN:
            break;
        case WM_LBUTTONUP:
            break;
            // Add more cases for other mouse events as needed
        }

        lastX = x;
        lastY = y;

    }

    // Call the next hook in the hook chain
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}
void timerFunc(int duration)
{
    for (long long i = 0; i < 2147483647; i++)
    {
        std::this_thread::sleep_for(chrono::milliseconds(duration));
        if (getMillis() - lastMove > 5000)
        {
            if (!executed)
            {
                cout << "execute 1st\n";
                system(command1.c_str());
            }
            executed = true;
        }
    }
}

int main(int argc, char* argv[])
{
    map<string, string> cfg = ParseIniFile();
    command1 = cfg["command1"];
    command2 = cfg["command2"];

    thread timerThread(timerFunc, 500);

    HHOOK mouseHook = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, GetModuleHandle(nullptr), 0);

    if (mouseHook == nullptr)
    {
        std::cerr << "Failed\n";
        return 1;
    }
    cout << "hi\n";
    MSG msg;

    while (GetMessage(&msg, nullptr, 0, 0) != 0)
    {
        // TranslateMessage(&msg);
        //DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(mouseHook);

    while (1)
    {

    }
    return 0;
}
