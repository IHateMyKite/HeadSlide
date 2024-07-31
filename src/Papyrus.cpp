#include "Papyrus.h"
#include "HeadSlide.h"

bool HESL::RegisterPapyrusFunctions(RE::BSScript::IVirtualMachine* vm)
{
    vm->RegisterFunction("UpdateHeadSlide", "HeadSlideNative", HeadSlide::UpdateHeadSlide);
    return true;
}
