#include "include.hpp"

namespace startposSwitcher
{
    void init();
    void handleKeyPress(int keyCode);
    void switchStartPos(bool direction);
    void setAlternateKeys(bool alternate);
    void setEnabled(bool enable);

    extern bool enabled;

    extern void* playLayer;
}
