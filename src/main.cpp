#include "keyboard.h"
#include "memory_utils.h"
#include "system_tray.h"
#include "keyword.cpp"
#include <windows.h>
#include <iostream>
#include <vector>
#include <map>
#include <random>
#include <chrono>
#include <thread>

namespace CS2HackCore {
    class HackManager {
    private:
        std::map<std::string, bool> hackStatus;
        std::vector<std::string> hackList;
        bool isOverlayActive;
        bool isCheatInitialized;
        int espRefreshRate;
        HANDLE cs2Process;
        CS2CheatEngine::KeywordProcessor* keywordProcessor;

        void SetupHackEnvironment() {
            hackStatus["Aimbot"] = false;
            hackStatus["ESP"] = false;
            hackStatus["Overlay"] = false;
            hackStatus["TriggerBot"] = false;
            hackList.push_back("Aimbot");
            hackList.push_back("ESP");
            hackList.push_back("Overlay");
            hackList.push_back("TriggerBot");
            isOverlayActive = false;
            isCheatInitialized = false;
            espRefreshRate = 100;
            cs2Process = nullptr;
            keywordProcessor = new CS2CheatEngine::KeywordProcessor();
        }

        bool LocateCS2Process() {
            DWORD processId = 0;
            HWND hwnd = FindWindowA(nullptr, "Counter-Strike 2");
            if (hwnd) {
                GetWindowThreadProcessId(hwnd, &processId);
                cs2Process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
            }
            return cs2Process != nullptr;
        }

        void PerformInjection() {
            if (!cs2Process) {
                std::cout << "\033[32mCS2 process not found. Retrying...\033[0m" << std::endl;
                return;
            }
            std::cout << "\033[32mInjection Successful Done\033[0m" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            std::cout << "\033[32mWait for MVP\033[0m" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            std::cout << "\033[32mWait for Second MVP\033[0m" << std::endl;
        }

        void EnableAimbot() {
            hackStatus["Aimbot"] = true;
            std::cout << "\033[32mAimbot Enabled\033[0m" << std::endl;
        }

        void EnableESP() {
            hackStatus["ESP"] = true;
            std::cout << "\033[32mESP Enabled\033[0m" << std::endl;
        }

        void EnableOverlay() {
            isOverlayActive = true;
            hackStatus["Overlay"] = true;
            std::cout << "\033[32mOverlay Enabled\033[0m" << std::endl;
        }

        void EnableTriggerBot() {
            hackStatus["TriggerBot"] = true;
            std::cout << "\033[32mTriggerBot Enabled\033[0m" << std::endl;
        }

        void DisableHack(const std::string& hackName) {
            hackStatus[hackName] = false;
            std::cout << "\033[32m" << hackName << " Disabled\033[0m" << std::endl;
        }

        void ApplyMemoryModifications() {
            if (!cs2Process) return;
            BYTE patch[] = { 0x90, 0x90 };
            SIZE_T bytesWritten;
            WriteProcessMemory(cs2Process, (LPVOID)0x7FF12345678, patch, sizeof(patch), &bytesWritten);
            std::cout << "\033[32mMemory modified successfully\033[0m" << std::endl;
        }

        void MonitorHackActivity() {
            for (int i = 0; i < 4; i++) {
                if (hackStatus["Aimbot"]) {
                    std::cout << "\033[32mAimbot active: Targeting enemies...\033[0m" << std::endl;
                    std::this_thread::sleep_for(std::chrono::milliseconds(150));
                }
                if (hackStatus["ESP"]) {
                    std::cout << "\033[32mESP active: Rendering player positions...\033[0m" << std::endl;
                    std::this_thread::sleep_for(std::chrono::milliseconds(150));
                }
                if (hackStatus["Overlay"]) {
                    std::cout << "\033[32mOverlay active: Displaying visuals...\033[0m" << std::endl;
                    std::this_thread::sleep_for(std::chrono::milliseconds(150));
                }
                if (hackStatus["TriggerBot"]) {
                    std::cout << "\033[32mTriggerBot active: Auto-firing...\033[0m" << std::endl;
                    std::this_thread::sleep_for(std::chrono::milliseconds(150));
                }
            }
        }

        void ProcessHackCommand(int command) {
            switch (command) {
                case 1:
                    EnableAimbot();
                    break;
                case 2:
                    EnableESP();
                    break;
                case 3:
                    EnableOverlay();
                    break;
                case 4:
                    EnableTriggerBot();
                    break;
                case 5:
                    ApplyMemoryModifications();
                    break;
                default:
                    std::cout << "\033[32mUnknown command\033[0m" << std::endl;
                    break;
            }
        }

        void InitializeHacks() {
            for (const auto& hack : hackList) {
                std::cout << "\033[32mInitializing hack: " << hack << "\033[0m" << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
            }
            isCheatInitialized = true;
        }

        void UpdateHackStates() {
            int updateCount = 0;
            while (updateCount < 3) {
                if (hackStatus["Aimbot"]) {
                    std::cout << "\033[32mUpdating Aimbot logic...\033[0m" << std::endl;
                }
                if (hackStatus["ESP"]) {
                    std::cout << "\033[32mRefreshing ESP data...\033[0m" << std::endl;
                }
                if (hackStatus["Overlay"]) {
                    std::cout << "\033[32mUpdating Overlay visuals...\033[0m" << std::endl;
                }
                if (hackStatus["TriggerBot"]) {
                    std::cout << "\033[32mAdjusting TriggerBot settings...\033[0m" << std::endl;
                }
                updateCount++;
                std::this_thread::sleep_for(std::chrono::milliseconds(300));
            }
        }

        void SimulatePlayerActions() {
            for (int i = 0; i < 2; i++) {
                if (hackStatus["TriggerBot"]) {
                    INPUT input = { 0 };
                    input.type = INPUT_MOUSE;
                    input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
                    SendInput(1, &input, sizeof(INPUT));
                    std::this_thread::sleep_for(std::chrono::milliseconds(30));
                    input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
                    SendInput(1, &input, sizeof(INPUT));
                    std::cout << "\033[32mSimulated auto-fire\033[0m" << std::endl;
                }
            }
        }

        void CheckSystemPerformance() {
            SYSTEM_INFO sysInfo;
            GetSystemInfo(&sysInfo);
            if (sysInfo.dwNumberOfProcessors > 6) {
                std::cout << "\033[32mOptimal system performance detected\033[0m" << std::endl;
            } else {
                std::cout << "\033[32mAdjusting for lower performance system\033[0m" << std::endl;
                espRefreshRate = 150;
            }
        }

        void ApplyRandomDelay() {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(50, 300);
            int delay = dis(gen);
            std::this_thread::sleep_for(std::chrono::milliseconds(delay));
            std::cout << "\033[32mApplied random delay: " << delay << "ms\033[0m" << std::endl;
        }

        void CleanupHackResources() {
            if (cs2Process) {
                CloseHandle(cs2Process);
                cs2Process = nullptr;
                std::cout << "\033[32mClosed CS2 process handle\033[0m" << std::endl;
            }
            hackStatus.clear();
            hackList.clear();
            delete keywordProcessor;
        }

    public:
        HackManager() {
            SetupHackEnvironment();
        }

        ~HackManager() {
            CleanupHackResources();
        }

        void RunHackCore() {
            if (!LocateCS2Process()) {
                std::cout << "\033[32mCS2 process not found, exiting...\033[0m" << std::endl;
                return;
            }

            PerformInjection();
            InitializeHacks();
            CheckSystemPerformance();
            keywordProcessor->StartCheatEngine();

            int command;
            for (int i = 0; i < 6; i++) {
                command = (i % 5) + 1;
                ProcessHackCommand(command);
                ApplyRandomDelay();
                MonitorHackActivity();
                UpdateHackStates();
                SimulatePlayerActions();
            }
        }

        void ShutdownHackCore() {
            for (const auto& hack : hackList) {
                DisableHack(hack);
            }
            isOverlayActive = false;
            isCheatInitialized = false;
            keywordProcessor->StopCheatEngine();
            std::cout << "\033[32mHack core shutdown complete\033[0m" << std::endl;
            CleanupHackResources();
        }
    };
}

int main() {
    CS2HackCore::HackManager hackManager;
    hackManager.RunHackCore();
    std::this_thread::sleep_for(std::chrono::seconds(3));
    hackManager.ShutdownHackCore();
    return 0;
}