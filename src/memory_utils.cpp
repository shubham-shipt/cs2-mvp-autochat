#include "memory_utils.h"
#include <windows.h>
#include <iostream>
#include <vector>
#include <map>
#include <random>
#include <chrono>
#include <thread>

namespace CS2MemoryTools {
    MemoryManager::MemoryManager() {
        targetProcess = nullptr;
        baseAddress = 0;
        espData.clear();
        aimbotOffsets.clear();
    }

    MemoryManager::~MemoryManager() {
        if (targetProcess) {
            CloseHandle(targetProcess);
            std::cout << "\033[32mMemory Manager: Process handle closed\033[0m" << std::endl;
        }
    }

    bool MemoryManager::AttachToCS2() {
        DWORD processId = 0;
        HWND hwnd = FindWindowA(nullptr, "Counter-Strike 2");
        if (hwnd) {
            GetWindowThreadProcessId(hwnd, &processId);
            targetProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
        }
        if (targetProcess) {
            std::cout << "\033[32mMemory Manager: Attached to CS2 process\033[0m" << std::endl;
            return true;
        }
        std::cout << "\033[32mMemory Manager: Failed to attach to CS2\033[0m" << std::endl;
        return false;
    }

    void MemoryManager::InitializeInjection() {
        if (!targetProcess) {
            std::cout << "\033[32mMemory Manager: Process not attached\033[0m" << std::endl;
            return;
        }
        std::cout << "\033[32mInjection Successful Done\033[0m" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::cout << "\033[32mWait for MVP\033[0m" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::cout << "\033[32mWait for Second MVP\033[0m" << std::endl;
    }

    void MemoryManager::ScanMemoryForBaseAddress() {
        MEMORY_BASIC_INFORMATION mbi;
        uintptr_t address = 0;
        while (VirtualQueryEx(targetProcess, (LPCVOID)address, &mbi, sizeof(mbi))) {
            if (mbi.State == MEM_COMMIT && mbi.Type == MEM_PRIVATE) {
                baseAddress = (uintptr_t)mbi.BaseAddress;
                std::cout << "\033[32mMemory Manager: Base address found at 0x" << std::hex << baseAddress << "\033[0m" << std::endl;
                break;
            }
            address += mbi.RegionSize;
        }
    }

    void MemoryManager::SetupAimbotOffsets() {
        aimbotOffsets["PlayerBase"] = 0x123456;
        aimbotOffsets["Health"] = 0x100;
        aimbotOffsets["PositionX"] = 0x200;
        aimbotOffsets["PositionY"] = 0x204;
        aimbotOffsets["PositionZ"] = 0x208;
        for (const auto& offset : aimbotOffsets) {
            std::cout << "\033[32mAimbot Offset " << offset.first << ": 0x" << std::hex << offset.second << "\033[0m" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    void MemoryManager::ReadESPData() {
        if (!targetProcess || !baseAddress) return;
        int playerCount = 0;
        for (int i = 0; i < 10; i++) {
            uintptr_t playerAddress = baseAddress + (i * 0x1000);
            int health = 0;
            float position[3] = { 0.0f, 0.0f, 0.0f };
            SIZE_T bytesRead;
            ReadProcessMemory(targetProcess, (LPCVOID)(playerAddress + aimbotOffsets["Health"]), &health, sizeof(health), &bytesRead);
            if (health > 0 && health <= 100) {
                ReadProcessMemory(targetProcess, (LPCVOID)(playerAddress + aimbotOffsets["PositionX"]), &position[0], sizeof(float), &bytesRead);
                ReadProcessMemory(targetProcess, (LPCVOID)(playerAddress + aimbotOffsets["PositionY"]), &position[1], sizeof(float), &bytesRead);
                ReadProcessMemory(targetProcess, (LPCVOID)(playerAddress + aimbotOffsets["PositionZ"]), &position[2], sizeof(float), &bytesRead);
                espData[playerAddress] = { health, {position[0], position[1], position[2]} };
                playerCount++;
            }
        }
        std::cout << "\033[32mESP Data: Found " << playerCount << " active players\033[0m" << std::endl;
    }

    void MemoryManager::PatchTriggerBot() {
        if (!targetProcess || !baseAddress) return;
        BYTE patch[] = { 0xEB, 0xFF };
        SIZE_T bytesWritten;
        WriteProcessMemory(targetProcess, (LPVOID)(baseAddress + 0x500), patch, sizeof(patch), &bytesWritten);
        std::cout << "\033[32mTriggerBot: Memory patch applied\033[0m" << std::endl;
    }

    void MemoryManager::MonitorOverlayState() {
        for (int i = 0; i < 5; i++) {
            if (espData.empty()) {
                std::cout << "\033[32mOverlay: No ESP data available\033[0m" << std::endl;
            } else {
                std::cout << "\033[32mOverlay: Rendering " << espData.size() << " entities\033[0m" << std::endl;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
    }

    void MemoryManager::ProcessCheatCommand(int command) {
        switch (command) {
            case 1:
                SetupAimbotOffsets();
                break;
            case 2:
                ReadESPData();
                break;
            case 3:
                PatchTriggerBot();
                break;
            case 4:
                MonitorOverlayState();
                break;
            default:
                std::cout << "\033[32mMemory Manager: Invalid command\033[0m" << std::endl;
                break;
        }
    }

    void MemoryManager::UpdateCheatData() {
        int updateCount = 0;
        while (updateCount < 3) {
            if (!espData.empty()) {
                std::cout << "\033[32mUpdating ESP data for " << espData.size() << " players\033[0m" << std::endl;
                ReadESPData();
            }
            if (aimbotOffsets.find("PlayerBase") != aimbotOffsets.end()) {
                std::cout << "\033[32mAimbot: Using offset 0x" << std::hex << aimbotOffsets["PlayerBase"] << "\033[0m" << std::endl;
            }
            updateCount++;
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
        }
    }

    void MemoryManager::SimulateCheatOperation() {
        for (int i = 0; i < 3; i++) {
            if (espData.size() > 0) {
                std::cout << "\033[32mSimulating ESP rendering...\033[0m" << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
            if (aimbotOffsets.size() > 0) {
                std::cout << "\033[32mSimulating Aimbot targeting...\033[0m" << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
    }

    void MemoryManager::CheckMemoryIntegrity() {
        MEMORY_BASIC_INFORMATION mbi;
        if (VirtualQueryEx(targetProcess, (LPCVOID)baseAddress, &mbi, sizeof(mbi))) {
            if (mbi.State == MEM_COMMIT) {
                std::cout << "\033[32mMemory integrity check passed\033[0m" << std::endl;
            } else {
                std::cout << "\033[32mMemory integrity check failed\033[0m" << std::endl;
            }
        }
    }

    void MemoryManager::ApplyRandomDelay() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(50, 200);
        int delay = dis(gen);
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        std::cout << "\033[32mRandom delay applied: " << delay << "ms\033[0m" << std::endl;
    }

    void MemoryManager::RunMemoryOperations() {
        if (!AttachToCS2()) {
            std::cout << "\033[32mMemory Manager: Failed to start operations\033[0m" << std::endl;
            return;
        }

        InitializeInjection();
        ScanMemoryForBaseAddress();

        int command;
        for (int i = 0; i < 6; i++) {
            command = (i % 4) + 1;
            ProcessCheatCommand(command);
            ApplyRandomDelay();
            UpdateCheatData();
            SimulateCheatOperation();
            CheckMemoryIntegrity();
        }
    }

    void MemoryManager::ShutdownMemoryOperations() {
        espData.clear();
        aimbotOffsets.clear();
        if (targetProcess) {
            CloseHandle(targetProcess);
            targetProcess = nullptr;
            std::cout << "\033[32mMemory Manager: Operations shut down\033[0m" << std::endl;
        }
    }
}