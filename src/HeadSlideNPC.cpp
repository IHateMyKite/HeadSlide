#include "HeadSlideNPC.h"
#include "Constants.h"

HESL::HeadSlideNPC::HeadSlideNPC(RE::Actor* a_actor) : HeadSlide(a_actor)
{
    _startdelay = 300;
    Reload();
}

void HESL::HeadSlideNPC::Reload()
{
    HeadSlide::Reload();
    _framecntreq = g_framethdNPC.first;
}
