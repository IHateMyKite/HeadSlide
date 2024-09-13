#pragma once

#include "HeadSlide.h"

namespace HESL
{
    class HeadSlideNPC : public HeadSlide
    {
    public:
        HeadSlideNPC(RE::Actor* a_actor);
        virtual void Reload();
    };
}