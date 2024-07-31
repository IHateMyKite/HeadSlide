#pragma once
#include "RaceMenuAPI.h"

namespace HESL
{
    class HeadSlide
    {
    SINGLETONHEADER(HeadSlide)
    public:
        void Init();
        static void UpdatePlayer(RE::Actor* a_actor, float a_delta);
        void UpdateActor(RE::Actor* a_actor);

    public:
        const std::vector<std::pair<std::string,std::string>> defaultsliders = 
        {
            {"NoseShort","NoseLong"},
            {"NoseDown","NoseUp"},
            {"JawDown","JawUp"},
            {"JawNarrow","JawWide"},
            {"JawBack","JawForward"},
            {"CheeksDown","CheeksUp"},
            {"CheeksOut","CheeksIn"},
            {"EyesMoveDown","EyesMoveUp"},
            {"EyesMoveOut","EyesMoveIn"},
            {"BrowDown","BrowUp"},
            {"BrowOut","BrowIn"},
            {"BrowBack","BrowForward"},
            {"LipMoveDown","LipMoveUp"},
            {"LipMoveOut","LipMoveIn"},
            {"ChinThin","ChinWide"},
            {"ChinMoveDown","ChinMoveUp"},
            {"Overbite","Underbite"},
            {"EyesBack","EyesForward"},
        };


        const std::vector<std::string> defaultparts = 
        {
            "Nose",
            "Unk",
            "Eyes",
            "Lip"
        };

        const std::vector<std::string> phonemesliders = 
        {
            "Aah",
            "BigAah",
            "BMP",
            "ChJSh",
            "DST",
            "Eee",
            "Eh",
            "FV",
            "I",
            "K",
            "N",
            "Oh",
            "OohQ",
            "R",
            "Th",
            "W"
        };

        const std::vector<std::string> modifiersliders = 
        {
            "BlinkLeft",
            "BlinkRight",
            "BrowDownLeft",
            "BrowDownRight",
            "BrowInLeft",
            "BrowInRight",
            "BrowUpLeft",
            "BrowUpRight",
            "LookDown",
            "LookLeft",
            "LookRight",
            "LookUp",
            "SquintLeft",
            "SquintRight"
        };

    public:
        static void UpdateHeadSlide(PAPYRUSFUNCHANDLE, RE::Actor* a_actor);

    private:
        static REL::Relocation<decltype(UpdatePlayer)> UpdatePlayer_old;
        RM::FaceMorphInterface*     _faceinterface  = nullptr;
        RM::IBodyMorphInterface*    _morphinterface = nullptr;
    };
}