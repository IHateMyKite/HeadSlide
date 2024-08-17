#include "Papyrus.h"
#include "manager.h"

bool HESL::RegisterPapyrusFunctions(RE::BSScript::IVirtualMachine* vm)
{
    vm->RegisterFunction("UpdateHeadSlide", "HeadSlideNative", HeadSlideManager::UpdateHeadSlide,true);
    return true;
}
