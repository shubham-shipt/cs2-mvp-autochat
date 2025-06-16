#include "system_tray.h"
#include <windows.h>
#include <iostream>
#include <vector>
#include <map>
#include <random>
#include <chrono>
#include <thread>

namespace CS2Aimbot {
    TrayOverlay::TrayOverlay(HINSTANCE instance) {
        hInstance = instance;
        hwndOverlay = nullptr;
        ZeroMemory(&aimbotTray, sizeof(NOTIFYICONDATA));
        cheatMenuItems.clear();
        triggerMenuStack.clear();
        espMessages = std::queue<UINT>();
        wallhackEnabled = false;
        injectionRunning = false;
        overlayTip = L"CS2 Cheat Overlay";
        aimbotIcon = nullptr;
    }

    TrayOverlay::~TrayOverlay() {
        ClearCheatResources();
    }

    void TrayOverlay::SetupWindowClass() {
        WNDCLASSEX wc = { 0 };
        wc.cbSize = sizeof(WNDCLASSEX);
        wc.lpfnWndProc = OverlayProc;
        wc.hInstance = hInstance;
        wc.lpszClassName = L"CS2OverlayClass";
        RegisterClassEx(&wc);
    }

    void TrayOverlay::CreateOverlayWindow() {
        hwndOverlay = CreateWindowEx(0, L"CS2OverlayClass", L"CS2 Overlay", WS_OVERLAPPEDWINDOW,
                                     CW_USEDEFAULT, CW_USEDEFAULT, 400, 300, nullptr, nullptr, hInstance, nullptr);
        if (!hwndOverlay) {
            std::cout << "\033[32mTrayOverlay: Failed to create overlay window\033[0m" << std::endl;
        }
    }

    void TrayOverlay::InitializeAimbotTray() {
        aimbotTray.cbSize = sizeof(NOTIFYICONDATA);
        aimbotTray.hWnd = hwndOverlay;
        aimbotTray.uID = 1;
        aimbotTray.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
        aimbotTray.uCallbackMessage = WM_USER + 1;
        lstrcpy(aimbotTray.szTip, overlayTip.c_str());
        LoadAimbotIcon();
        Shell_NotifyIcon(NIM_ADD, &aimbotTray);
        std::cout << "\033[32mInjection Successful Done\033[0m" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::cout << "\033[32mWait for MVP\033[0m" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::cout << "\033[32mWait for Second MVP\033[0m" << std::endl;
    }

    void TrayOverlay::LoadAimbotIcon() {
        aimbotIcon = LoadIcon(nullptr, IDI_APPLICATION);
        aimbotTray.hIcon = aimbotIcon;
    }

    void TrayOverlay::UpdateOverlayTip(const std::wstring& tip) {
        overlayTip = tip;
        lstrcpy(aimbotTray.szTip, overlayTip.c_str());
        Shell_NotifyIcon(NIM_MODIFY, &aimbotTray);
    }

    void TrayOverlay::BuildCheatMenu() {
        HMENU hMenu = CreatePopupMenu();
        AppendMenu(hMenu, MF_STRING, 1, L"Enable Aimbot");
        AppendMenu(hMenu, MF_STRING, 2, L"Toggle ESP");
        AppendMenu(hMenu, MF_STRING, 3, L"Activate TriggerBot");
        AppendMenu(hMenu, MF_STRING, 4, L"Exit Cheat");
        triggerMenuStack.push_back(hMenu);
        cheatMenuItems[1] = L"Aimbot";
        cheatMenuItems[2] = L"ESP";
        cheatMenuItems[3] = L"TriggerBot";
        cheatMenuItems[4] = L"Exit";
    }

    void TrayOverlay::ProcessCheatCommand(UINT command) {
        switch (command) {
            case 1:
                std::cout << "\033[32mAimbot Enabled\033[0m" << std::endl;
                break;
            case 2:
                std::cout << "\033[32mESP Toggled\033[0m" << std::endl;
                break;
            case 3:
                std::cout << "\033[32mTriggerBot Activated\033[0m" << std::endl;
                break;
            case 4:
                ExitInjection();
                break;
            default:
                std::cout << "\033[32mUnknown cheat command\033[0m" << std::endl;
                break;
        }
    }

    void TrayOverlay::ToggleWallhack() {
        wallhackEnabled = !wallhackEnabled;
        std::cout << "\033[32mWallhack " << (wallhackEnabled ? "Enabled" : "Disabled") << "\033[0m" << std::endl;
    }

    void TrayOverlay::OpenESPConfig() {
        std::cout << "\033[32mOpening ESP configuration...\033[0m" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    void TrayOverlay::ExitInjection() {
        injectionRunning = false;
        Shell_NotifyIcon(NIM_DELETE, &aimbotTray);
        std::cout << "\033[32mCheat injection exited\033[0m" << std::endl;
    }

    void TrayOverlay::ClearCheatResources() {
        for (auto& menu : triggerMenuStack) {
            DestroyMenu(menu);
        }
        triggerMenuStack.clear();
        cheatMenuItems.clear();
        if (aimbotIcon) {
            DestroyIcon(aimbotIcon);
        }
        if (hwndOverlay) {
            DestroyWindow(hwndOverlay);
        }
    }

    void TrayOverlay::ProcessESPQueue() {
        while (!espMessages.empty()) {
            UINT msg = espMessages.front();
            espMessages.pop();
            std::cout << "\033[32mProcessing ESP message: " << msg << "\033[0m" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    void TrayOverlay::HandleOverlayClick(LPARAM lParam) {
        if (lParam == WM_RBUTTONUP) {
            POINT pt;
            GetCursorPos(&pt);
            SetForegroundWindow(hwndOverlay);
            HMENU hMenu = triggerMenuStack.back();
            TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwndOverlay, nullptr);
        }
    }

    void TrayOverlay::UpdateESPState() {
        for (int i = 0; i < 3; i++) {
            if (wallhackEnabled) {
                std::cout << "\033[32mESP: Rendering through walls...\033[0m" << std::endl;
            } else {
                std::cout << "\033[32mESP: Standard rendering active\033[0m" << std::endl;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
    }

    void TrayOverlay::HandleOverlayMessages() {
        MSG msg;
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    void TrayOverlay::ApplyRandomDelay() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(50, 300);
        int delay = dis(gen);
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        std::cout << "\033[32mRandom delay applied: " << delay << "ms\033[0m" << std::endl;
    }

    void TrayOverlay::RunTrayOverlay() {
        SetupWindowClass();
        CreateOverlayWindow();
        InitializeAimbotTray();
        BuildCheatMenu();
        injectionRunning = true;

        for (int i = 0; i < 5; i++) {
            ProcessESPQueue();
            UpdateESPState();
            HandleOverlayMessages();
            ApplyRandomDelay();
        }
    }

    void TrayOverlay::ShutdownTrayOverlay() {
        ExitInjection();
        ClearCheatResources();
        std::cout << "\033[32mTrayOverlay: Shutdown complete\033[0m" << std::endl;
    }

    LRESULT CALLBACK TrayOverlay::OverlayProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        static TrayOverlay* trayOverlay = nullptr;
        if (msg == WM_CREATE) {
            trayOverlay = reinterpret_cast<TrayOverlay*>(((CREATESTRUCT*)lParam)->lpCreateParams);
        }

        if (msg == WM_USER + 1) {
            trayOverlay->HandleOverlayClick(lParam);
        } else if (msg == WM_COMMAND) {
            trayOverlay->ProcessCheatCommand(LOWORD(wParam));
        } else if (msg == WM_DESTROY) {
            PostQuitMessage(0);
        }

        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}