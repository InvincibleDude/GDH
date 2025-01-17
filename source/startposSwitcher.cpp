#include "startposSwitcher.hpp"
#include "smartStartpos.hpp"

namespace startposSwitcher
{
    void(__thiscall *resetLevel)(void *) = nullptr;
    void(__thiscall *setStartPosObject)(void *, void *) = nullptr;
    
    void(__thiscall *startposObjectInit)(void *) = nullptr;

    void *GJBaseGameLayer = nullptr;
    void *playLayer = nullptr;
    std::vector<float *> startposObjects;
    int startposIndex = -1;
    bool enabled = false;
    bool alternateKeys = false;

    void __fastcall startposObjectInitHook(void *self, void *_)
    {
        startposObjects.push_back((float *)self);
        startposIndex = startposObjects.size() - 1;
        startposObjectInit(self);
    }

    void(__thiscall *GJBaseGameLayer_init)(void *) = nullptr;
    void __fastcall GJBaseGameLayer_init_H(void *self, void *_)
    {
        startposObjects = {};
        startposIndex = 0;
        GJBaseGameLayer = self;
        smartStartpos::resetObjects();
        GJBaseGameLayer_init(self);
    }

    void init()
    {
        uintptr_t base = (uintptr_t)GetModuleHandleA(0);

        MH_CreateHook((LPVOID)(base + 0x3A0D10), startposObjectInitHook, (LPVOID *)&startposObjectInit);
        MH_CreateHook((LPVOID)(base + 0x18CC80), GJBaseGameLayer_init_H, (LPVOID *)&GJBaseGameLayer_init);
        resetLevel = (decltype(resetLevel))(base + 0x2E42B0);
        setStartPosObject = (decltype(setStartPosObject))(base + 0x195FC0);
    }

    void handleKeyPress(int keyCode)
    {
        if (!enabled)
            return;
        if (!alternateKeys && (keyCode == VK_LEFT || keyCode == VK_RIGHT))
        {
            switchStartPos(keyCode == VK_RIGHT);
        }
        else if (alternateKeys && (keyCode == 'A' || keyCode == 'D'))
        {
            switchStartPos(keyCode == 'D');
        }
    }

    void switchStartPos(bool direction)
    {
        if (playLayer == nullptr || !enabled || MBO(bool, playLayer, 0x2e91) || MBO(bool, playLayer, 0x2c20))
        {
            return;
        }

        if (direction)
        {
            startposIndex = startposIndex - 1;
            if (startposIndex < -1)
            {
                startposIndex = startposObjects.size() - 1;
            }
        }
        else
        {
            startposIndex = startposIndex + 1;
            if (static_cast<unsigned int>(startposIndex) >= startposObjects.size())
            {
                startposIndex = -1;
            }
        }

        ((uint32_t *)playLayer)[0xB7D] = 0; // PlayLayer::startPosCheckpoint i believe?

        if (startposIndex >= 0)
        {
            setStartPosObject(playLayer, startposObjects[startposIndex]);
        }
        else
        {
            setStartPosObject(playLayer, nullptr);
        }

        if (MBO(bool, playLayer, 0x2a74)){
            auto gdBaseA = (DWORD)GetModuleHandleA(0);
            reinterpret_cast<void(__thiscall*)(void*)>(gdBaseA + 0x2e4210)(playLayer); //remove all checkpoints
        }

        resetLevel(playLayer);
        auto gdBaseA = (DWORD)GetModuleHandleA(0);
        reinterpret_cast<void(__thiscall*)(void*)>(gdBaseA + 0x2e5570)(playLayer); //startmusic
    }

    void setAlternateKeys(bool alternate)
    {
        alternateKeys = alternate;
    }

    void setEnabled(bool enable)
    {
        enabled = enable;
    }
}
