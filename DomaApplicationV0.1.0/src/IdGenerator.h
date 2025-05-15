#pragma once
#include "imgui.h"

struct FontHolder {
    ImFont* Small;
    ImFont* Medium;
    ImFont* Large;
};

namespace DomaApp {

    void RenderIdGeneratorUI();
}
