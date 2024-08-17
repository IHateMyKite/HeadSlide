#pragma once
#include "RaceMenuAPI.h"

namespace HESL
{
    class HeadSlide;

    class HeadSlideManager
    {
    SINGLETONHEADER(HeadSlideManager)
    public:
        void Init();
        void Reload();
        void UpdateNPC(RE::Actor* a_actor, float a_delta);
        static void UpdatePlayerHook(RE::Actor* a_actor, float a_delta);
        static void UpdateCharacterHook(RE::Actor* a_actor, float a_delta);
        void ForceUpdateHeadSlide(RE::Actor* a_actor);

        static SKEE::FaceMorphInterface*  faceinterface;
        static SKEE::IBodyMorphInterface* morphinterface;
    public:
        static void UpdateHeadSlide(PAPYRUSFUNCHANDLE, RE::Actor* a_actor);
    private:
        bool _init = false;
        static REL::Relocation<decltype(UpdatePlayerHook)> UpdatePlayer_old;
        static REL::Relocation<decltype(UpdateCharacterHook)> UpdateCharacter_old;
    private:
        std::unordered_map<uint32_t,std::shared_ptr<HeadSlide>> _npclist;
    };
}