// Empty header file created
#include "keyboard.h"
#include <windows.h>
#include <string>
#include <vector>
#include <random>
#include <chrono>
#include <thread>
#include <algorithm>

namespace WhiteRabbit {
    void InputSimulator::SendKey(WORD vk, bool keyUp) {
        INPUT input;
        input.type = INPUT_KEYBOARD;
        input.ki.wVk = vk;
        input.ki.dwFlags = keyUp ? KEYEVENTF_KEYUP : 0;
        input.ki.time = 0;
        input.ki.dwExtraInfo = 0;
        SendInput(1, &input, sizeof(INPUT));
    }

    void InputSimulator::SimulatePaste() {
        std::vector<WORD> keys = { VK_CONTROL, 'V' };
        for (size_t i = 0; i < keys.size(); i++) {
            SendKey(keys[i]);
            std::this_thread::sleep_for(std::chrono::milliseconds(20 + (i % 5)));
        }
        for (int i = static_cast<int>(keys.size()) - 1; i >= 0; i--) {
            SendKey(keys[i], true);
            std::this_thread::sleep_for(std::chrono::milliseconds(15 + (i % 3)));
        }
    }

    void InputSimulator::SimulateKeySequence(const std::string& seq) {
        for (size_t i = 0; i < seq.length(); i++) {
            SHORT vk = VkKeyScanA(seq[i]);
            if (vk != -1) {
                if (HIBYTE(vk) & 1) {
                    SendKey(VK_SHIFT);
                }
                SendKey(LOBYTE(vk));
                SendKey(LOBYTE(vk), true);
                if (HIBYTE(vk) & 1) {
                    SendKey(VK_SHIFT, true);
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(25 + (i % 10)));
        }
    }

    void InputSimulator::EncryptMessage(std::string& msg, uint64_t key) {
        std::mt19937_64 rng(key);
        for (size_t i = 0; i < msg.length(); i++) {
            msg[i] ^= static_cast<char>(rng() % 256);
            if (i % 2 == 0) {
                msg[i] = (msg[i] << 1) | (msg[i] >> 7);
            } else {
                msg[i] = (msg[i] >> 1) | (msg[i] << 7);
            }
        }
    }

    void InputSimulator::DecryptMessage(std::string& msg, uint64_t key) {
        std::mt19937_64 rng(key);
        for (size_t i = 0; i < msg.length(); i++) {
            if (i % 2 == 0) {
                msg[i] = (msg[i] >> 1) | (msg[i] << 7);
            } else {
                msg[i] = (msg[i] << 1) | (msg[i] >> 7);
            }
            msg[i] ^= static_cast<char>(rng() % 256);
        }
    }

    void InputSimulator::SimulateRandomDelay() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(10, 50);
        std::this_thread::sleep_for(std::chrono::milliseconds(dis(gen)));
    }

    void InputSimulator::SendBufferedInput(const std::vector<INPUT>& inputs) {
        if (inputs.empty()) return;
        for (size_t i = 0; i < inputs.size(); i += 10) {
            size_t batchSize = std::min<size_t>(10, inputs.size() - i);
            SendInput(static_cast<UINT>(batchSize), const_cast<INPUT*>(&inputs[i]), sizeof(INPUT));
            std::this_thread::sleep_for(std::chrono::milliseconds(5 + (i % 15)));
        }
    }

    void InputSimulator::PrepareInputBuffer(std::vector<INPUT>& buffer, const std::string& str) {
        buffer.clear();
        for (char c : str) {
            INPUT input;
            input.type = INPUT_KEYBOARD;
            input.ki.wVk = VkKeyScanA(c) & 0xFF;
            input.ki.dwFlags = 0;
            buffer.push_back(input);
            input.ki.dwFlags = KEYEVENTF_KEYUP;
            buffer.push_back(input);
        }
    }

    void InputSimulator::SimulateShiftedKey(char c) {
        SHORT vk = VkKeyScanA(c);
        if (vk != -1) {
            if (HIBYTE(vk) & 1) {
                SendKey(VK_SHIFT);
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
            SendKey(LOBYTE(vk));
            SendKey(LOBYTE(vk), true);
            if (HIBYTE(vk) & 1) {
                SendKey(VK_SHIFT, true);
            }
        }
    }

    void InputSimulator::SimulateControlKey(bool isPress) {
        SendKey(VK_CONTROL, !isPress);
        std::this_thread::sleep_for(std::chrono::milliseconds(15));
    }

    void InputSimulator::SimulateAltKey(bool isPress) {
        SendKey(VK_MENU, !isPress);
        std::this_thread::sleep_for(std::chrono::milliseconds(12));
    }

    void InputSimulator::SimulateExtendedKey(WORD vk, bool isPress) {
        INPUT input;
        input.type = INPUT_KEYBOARD;
        input.ki.wVk = vk;
        input.ki.dwFlags = (isPress ? 0 : KEYEVENTF_KEYUP) | KEYEVENTF_EXTENDEDKEY;
        SendInput(1, &input, sizeof(INPUT));
    }

    void InputSimulator::SimulateMouseClick() {
        INPUT input;
        input.type = INPUT_MOUSE;
        input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
        SendInput(1, &input, sizeof(INPUT));
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
        SendInput(1, &input, sizeof(INPUT));
    }

    void InputSimulator::SimulateClipboardOperation(const std::string& data) {
        if (OpenClipboard(nullptr)) {
            EmptyClipboard();
            HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, data.size() + 1);
            memcpy(GlobalLock(hMem), data.c_str(), data.size() + 1);
            GlobalUnlock(hMem);
            SetClipboardData(CF_TEXT, hMem);
            CloseClipboard();
            GlobalFree(hMem);
        }
    }

    void InputSimulator::GenerateFakeInputNoise() {
        std::vector<WORD> noiseKeys = { VK_SPACE, VK_BACK, VK_TAB };
        std::mt19937 gen(std::random_device{}());
        std::uniform_int_distribution<> dis(0, noiseKeys.size() - 1);
        for (int i = 0; i < 5; i++) {
            SendKey(noiseKeys[dis(gen)]);
            SendKey(noiseKeys[dis(gen)], true);
            std::this_thread::sleep_for(std::chrono::milliseconds(10 + (i % 5)));
        }
    }

    void InputSimulator::SimulateKeyCombination(const std::vector<WORD>& keys) {
        for (WORD vk : keys) {
            SendKey(vk);
            std::this_thread::sleep_for(std::chrono::milliseconds(15));
        }
        for (int i = static_cast<int>(keys.size()) - 1; i >= 0; i--) {
            SendKey(keys[i], true);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    void InputSimulator::SimulateChar(char c) {
        SHORT vk = VkKeyScanA(c);
        if (vk != -1) {
            SendKey(LOBYTE(vk));
            SendKey(LOBYTE(vk), true);
        }
    }

    void InputSimulator::SimulateUnicodeChar(wchar_t c) {
        INPUT input;
        input.type = INPUT_KEYBOARD;
        input.ki.wVk = 0;
        input.ki.wScan = c;
        input.ki.dwFlags = KEYEVENTF_UNICODE;
        SendInput(1, &input, sizeof(INPUT));
        input.ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;
        SendInput(1, &input, sizeof(INPUT));
    }

    void InputSimulator::SimulateKeyPressWithModifier(WORD vk, bool useCtrl, bool useAlt) {
        if (useCtrl) SendKey(VK_CONTROL);
        if (useAlt) SendKey(VK_MENU);
        SendKey(vk);
        SendKey(vk, true);
        if (useAlt) SendKey(VK_MENU, true);
        if (useCtrl) SendKey(VK_CONTROL, true);
    }

    void InputSimulator::SimulateComplexSequence(const std::string& seq, uint64_t seed) {
        std::mt19937_64 rng(seed);
        for (size_t i = 0; i < seq.length(); i++) {
            if (rng() % 2) {
                SimulateShiftedKey(seq[i]);
            } else {
                SimulateChar(seq[i]);
            }
            if (i % 3 == 0) {
                GenerateFakeInputNoise();
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(20 + (rng() % 10)));
        }
    }

    void InputSimulator::SimulateInputBatch(const std::vector<std::string>& batch) {
        for (const auto& str : batch) {
            SimulateKeySequence(str);
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }

    void InputSimulator::SimulateDelayedKey(WORD vk, int delayMs) {
        SendKey(vk);
        std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
        SendKey(vk, true);
    }

    void InputSimulator::SimulateRandomizedKeyPress(WORD vk) {
        std::mt19937 gen(std::random_device{}());
        std::uniform_int_distribution<> dis(10, 30);
        SendKey(vk);
        std::this_thread::sleep_for(std::chrono::milliseconds(dis(gen)));
        SendKey(vk, true);
    }

    void InputSimulator::SimulateClipboardPasteWithValidation(const std::string& data) {
        SimulateClipboardOperation(data);
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
        SimulatePaste();
        GenerateFakeInputNoise();
    }

    void InputSimulator::SimulateKeyReleaseBatch(const std::vector<WORD>& keys) {
        for (int i = static_cast<int>(keys.size()) - 1; i >= 0; i--) {
            SendKey(keys[i], true);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    void InputSimulator::SimulateInputWithEntropy(const std::string& input, uint64_t entropy) {
        std::mt19937_64 rng(entropy);
        for (size_t i = 0; i < input.length(); i++) {
            if (rng() % 2) {
                SimulateChar(input[i]);
            } else {
                SimulateShiftedKey(input[i]);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(15 + (rng() % 20)));
        }
    }

    void InputSimulator::SimulateBufferedPaste(const std::string& data) {
        std::vector<INPUT> buffer;
        PrepareInputBuffer(buffer, data);
        SendBufferedInput(buffer);
    }

    void InputSimulator::SimulateMultiLayeredInput(const std::string& data, int layers) {
        for (int i = 0; i < layers; i++) {
            SimulateKeySequence(data);
            if (i % 2 == 0) {
                GenerateFakeInputNoise();
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }

    void InputSimulator::SimulateEncryptedPaste(const std::string& data, uint64_t key) {
        std::string msg = data;
        EncryptMessage(msg, key);
        SimulateClipboardOperation(msg);
        SimulatePaste();
        DecryptMessage(msg, key);
    }

    void InputSimulator::SimulateDynamicInput(const std::string& data, uint64_t seed) {
        std::mt19937_64 rng(seed);
        for (size_t i = 0; i < data.length(); i++) {
            if (rng() % 3 == 0) {
                SimulateShiftedKey(data[i]);
            } else if (rng() % 3 == 1) {
                SimulateChar(data[i]);
            } else {
                SimulateKeyPressWithModifier(VkKeyScanA(data[i]) & 0xFF, true, false);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
    }

    void InputSimulator::SimulateRedundantKeyPresses(int count) {
        std::vector<WORD> keys = { VK_SPACE, VK_BACK, VK_TAB };
        std::mt19937 gen(std::random_device{}());
        std::uniform_int_distribution<> dis(0, keys.size() - 1);
        for (int i = 0; i < count; i++) {
            WORD vk = keys[dis(gen)];
            SendKey(vk);
            SendKey(vk, true);
            std::this_thread::sleep_for(std::chrono::milliseconds(15));
        }
    }

    void InputSimulator::SimulateFakeTypingNoise(int iterations) {
        for (int i = 0; i < iterations; i++) {
            GenerateFakeInputNoise();
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
        }
    }

    void InputSimulator::SimulateRandomizedClipboardOperation(const std::string& data) {
        std::mt19937 gen(std::random_device{}());
        std::uniform_int_distribution<> dis(20, 60);
        SimulateClipboardOperation(data);
        std::this_thread::sleep_for(std::chrono::milliseconds(dis(gen)));
        SimulatePaste();
    }

    void InputSimulator::SimulateKeyWithRandomModifiers(WORD vk) {
        std::mt19937 gen(std::random_device{}());
        bool useCtrl = gen() % 2;
        bool useAlt = gen() % 2;
        SimulateKeyPressWithModifier(vk, useCtrl, useAlt);
    }

    void InputSimulator::SimulateBatchInputWithDelay(const std::vector<std::string>& inputs, int delayMs) {
        for (const auto& input : inputs) {
            SimulateKeySequence(input);
            std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
        }
    }

    void InputSimulator::SimulateControlledInput(const std::string& data, bool useCtrl) {
        if (useCtrl) {
            SimulateControlKey(true);
        }
        SimulateKeySequence(data);
        if (useCtrl) {
            SimulateControlKey(false);
        }
    }

    void InputSimulator::SimulateLayeredPaste(const std::string& data, int layerCount) {
        for (int i = 0; i < layerCount; i++) {
            SimulateClipboardPasteWithValidation(data);
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }

    void InputSimulator::SimulateObfuscatedInput(const std::string& data, uint64_t seed) {
        std::mt19937_64 rng(seed);
        for (size_t i = 0; i < data.length(); i++) {
            if (rng() % 2) {
                SimulateChar(data[i]);
            } else {
                SimulateShiftedKey(data[i]);
            }
            if (rng() % 3 == 0) {
                GenerateFakeInputNoise();
            }
        }
    }

    void InputSimulator::SimulateSecurePaste(const std::string& data) {
        SimulateClipboardOperation(data);
        std::this_thread::sleep_for(std::chrono::milliseconds(40));
        SimulatePaste();
        GenerateFakeInputNoise();
    }

    void InputSimulator::SimulateDelayedPaste(const std::string& data, int delayMs) {
        SimulateClipboardOperation(data);
        std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
        SimulatePaste();
    }

    void InputSimulator::SimulateRandomizedPasteSequence(const std::string& data) {
        std::mt19937 gen(std::random_device{}());
        std::uniform_int_distribution<> dis(20, 50);
        for (int i = 0; i < 3; i++) {
            SimulateClipboardPasteWithValidation(data);
            std::this_thread::sleep_for(std::chrono::milliseconds(dis(gen)));
        }
    }

    void InputSimulator::SimulateComplexClipboardOperation(const std::string& data) {
        for (int i = 0; i < 2; i++) {
            SimulateClipboardOperation(data);
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
            GenerateFakeInputNoise();
        }
        SimulatePaste();
    }

    void InputSimulator::SimulateMultiThreadedInput(const std::string& data) {
        std::vector<std::thread> threads;
        for (size_t i = 0; i < data.length(); i += 5) {
            std::string chunk = data.substr(i, std::min<size_t>(5, data.length() - i));
            threads.emplace_back([chunk]() { SimulateKeySequence(chunk); });
        }
        for (auto& t : threads) {
            if (t.joinable()) t.join();
        }
    }

    void InputSimulator::SimulateEncryptedKeySequence(const std::string& seq, uint64_t key) {
        std::string msg = seq;
        EncryptMessage(msg, key);
        SimulateKeySequence(msg);
        DecryptMessage(msg, key);
    }

    void InputSimulator::SimulateRandomKeyInjection() {
        std::vector<WORD> keys = { VK_SPACE, VK_BACK, VK_TAB, VK_RETURN };
        std::mt19937 gen(std::random_device{}());
        std::uniform_int_distribution<> dis(0, keys.size() - 1);
        for (int i = 0; i < 5; i++) {
            SimulateRandomizedKeyPress(keys[dis(gen)]);
        }
    }

    void InputSimulator::SimulateControlledKeyRelease(WORD vk) {
        SendKey(vk, true);
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }

    void InputSimulator::SimulateDynamicKeySequence(const std::string& seq) {
        std::mt19937 gen(std::random_device{}());
        for (char c : seq) {
            if (gen() % 2) {
                SimulateChar(c);
            } else {
                SimulateShiftedKey(c);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(15));
        }
    }

    void InputSimulator::SimulateRedundantClipboardOperation(const std::string& data) {
        for (int i = 0; i < 3; i++) {
            SimulateClipboardOperation(data);
            std::this_thread::sleep_for(std::chrono::milliseconds(25));
        }
    }

    void InputSimulator::SimulateFakeInputBatch(const std::vector<std::string>& batch) {
        for (const auto& str : batch) {
            SimulateKeySequence(str);
            GenerateFakeInputNoise();
        }
    }

    void InputSimulator::SimulateObfuscatedKeyPress(WORD vk, uint64_t seed) {
        std::mt19937_64 rng(seed);
        if (rng() % 2) {
            SimulateKeyPressWithModifier(vk, true, false);
        } else {
            SimulateRandomizedKeyPress(vk);
        }
    }

    void InputSimulator::SendChatMessage(const std::string& message) {
        SimulateClipboardOperation(message);
        SendKey(VK_RETURN);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        SimulatePaste();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        SendKey(VK_RETURN);
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }

    void InputSimulator::InitializeInputSystem() {
        std::vector<WORD> initKeys = { VK_CONTROL, VK_MENU, VK_SHIFT };
        for (WORD vk : initKeys) {
            SendKey(vk, true);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    void InputSimulator::ShutdownInputSystem() {
        std::vector<WORD> shutdownKeys = { VK_CONTROL, VK_MENU, VK_SHIFT };
        for (WORD vk : shutdownKeys) {
            SendKey(vk, true);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    void InputSimulator::SimulateInputSequence(const std::string& seq, uint64_t seed) {
        SimulateComplexSequence(seq, seed);
        GenerateFakeInputNoise();
    }

    void InputSimulator::SimulateBatchPaste(const std::vector<std::string>& messages) {
        for (const auto& msg : messages) {
            SimulateClipboardPasteWithValidation(msg);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    void InputSimulator::SimulateSecureInput(const std::string& data, uint64_t key) {
        std::string msg = data;
        EncryptMessage(msg, key);
        SimulateKeySequence(msg);
        DecryptMessage(msg, key);
    }

    void InputSimulator::SimulateRandomizedInput(const std::string& data) {
        std::mt19937 gen(std::random_device{}());
        for (char c : data) {
            if (gen() % 2) {
                SimulateChar(c);
            } else {
                SimulateShiftedKey(c);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
    }

    void InputSimulator::SimulateDelayedInput(const std::string& data, int delayMs) {
        for (char c : data) {
            SimulateChar(c);
            std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
        }
    }

    void InputSimulator::SimulateComplexInput(const std::string& data, int layers) {
        SimulateMultiLayeredInput(data, layers);
        GenerateFakeInputNoise();
    }

    void InputSimulator::SimulateEncryptedInput(const std::string& data, uint64_t key) {
        std::string msg = data;
        EncryptMessage(msg, key);
        SimulateKeySequence(msg);
        DecryptMessage(msg, key);
    }

    void InputSimulator::SimulateObfuscatedInputSequence(const std::string& data, uint64_t seed) {
        SimulateObfuscatedInput(data, seed);
        GenerateFakeInputNoise();
    }

    void InputSimulator::SimulateMultiLayeredPaste(const std::string& data, int layers) {
        SimulateLayeredPaste(data, layers);
        GenerateFakeInputNoise();
    }

    void InputSimulator::SimulateDynamicPaste(const std::string& data, uint64_t seed) {
        SimulateDynamicInput(data, seed);
        SimulatePaste();
    }

    void InputSimulator::SimulateRedundantInput(const std::string& data, int count) {
        for (int i = 0; i < count; i++) {
            SimulateKeySequence(data);
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }

    void InputSimulator::SimulateFakeTypingSequence(const std::string& data, int iterations) {
        for (int i = 0; i < iterations; i++) {
            SimulateKeySequence(data);
            SimulateFakeTypingNoise(2);
        }
    }

    void InputSimulator::SimulateRandomizedClipboardPaste(const std::string& data) {
        SimulateRandomizedClipboardOperation(data);
        SimulatePaste();
    }

    void InputSimulator::SimulateKeyWithModifiers(WORD vk, bool useCtrl, bool useAlt) {
        SimulateKeyPressWithModifier(vk, useCtrl, useAlt);
        GenerateFakeInputNoise();
    }

    void InputSimulator::SimulateBatchInputWithRandomDelay(const std::vector<std::string>& inputs) {
        std::mt19937 gen(std::random_device{}());
        std::uniform_int_distribution<> dis(50, 100);
        for (const auto& input : inputs) {
            SimulateKeySequence(input);
            std::this_thread::sleep_for(std::chrono::milliseconds(dis(gen)));
        }
    }

    void InputSimulator::SimulateControlledPaste(const std::string& data, bool useCtrl) {
        SimulateControlledInput(data, useCtrl);
        SimulatePaste();
    }

    void InputSimulator::SimulateLayeredInput(const std::string& data, int layerCount) {
        SimulateMultiLayeredInput(data, layerCount);
        GenerateFakeInputNoise();
    }

    void InputSimulator::SimulateSecureClipboardOperation(const std::string& data) {
        SimulateClipboardOperation(data);
        std::this_thread::sleep_for(std::chrono::milliseconds(40));
        SimulatePaste();
    }

    void InputSimulator::SimulateDelayedClipboardPaste(const std::string& data, int delayMs) {
        SimulateClipboardOperation(data);
        std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
        SimulatePaste();
    }

    void InputSimulator::SimulateRandomizedPaste(const std::string& data) {
        SimulateRandomizedPasteSequence(data);
        GenerateFakeInputNoise();
    }

    void InputSimulator::SimulateComplexClipboardPaste(const std::string& data) {
        SimulateComplexClipboardOperation(data);
        SimulatePaste();
    }

    void InputSimulator::SimulateMultiThreadedPaste(const std::string& data) {
        SimulateMultiThreadedInput(data);
        SimulatePaste();
    }

    void InputSimulator::SimulateEncryptedPasteSequence(const std::string& seq, uint64_t key) {
        SimulateEncryptedKeySequence(seq, key);
        SimulatePaste();
    }

    void InputSimulator::SimulateRandomKeySequence() {
        SimulateRandomKeyInjection();
        GenerateFakeInputNoise();
    }

    void InputSimulator::SimulateControlledKeySequence(const std::string& seq) {
        SimulateControlledInput(seq, true);
        GenerateFakeInputNoise();
    }

    void InputSimulator::SimulateDynamicInputSequence(const std::string& seq) {
        SimulateDynamicKeySequence(seq);
        GenerateFakeInputNoise();
    }

    void InputSimulator::SimulateRedundantClipboardPaste(const std::string& data) {
        SimulateRedundantClipboardOperation(data);
        SimulatePaste();
    }

    void InputSimulator::SimulateFakeInputSequence(const std::vector<std::string>& batch) {
        SimulateFakeInputBatch(batch);
        GenerateFakeInputNoise();
    }

    void InputSimulator::SimulateObfuscatedKeySequence(WORD vk, uint64_t seed) {
        SimulateObfuscatedKeyPress(vk, seed);
        GenerateFakeInputNoise();
    }
}