#pragma once
#include "RaceMenuAPI.h"

namespace HESL
{
    #define MORPHKEY "HeadSlide"

    class HeadSlide
    {
    SINGLETONHEADER(HeadSlide)
    public:
        void Init();
        static void UpdatePlayer(RE::Actor* a_actor, float a_delta);
        void UpdateActor(RE::Actor* a_actor, float a_delta);

        void UpdateSlidersMorphs(RE::Actor* a_actor);
        void UpdateExpression(RE::Actor* a_actor);
        void SetActorsMorphs(RE::Actor* a_actor);

        void Reload();
        void ReadyTempVars(RE::Actor* a_actor);
    public:



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

        const std::vector<std::pair<std::string,std::pair<std::string,std::string>>> pairkeywords = 
        {
            {"LongShort",{"Short","Long"}},
            {"UpDown",{"Down","Up"}},
            {"WideNarrow",{"Narrow","Wide"}},
            {"ForwardBack",{"Back","Forward"}},
            {"InOut",{"Out","In"}},
            {"WideThin",{"Thin","Wide"}}
        };


        const std::vector<std::string> expressionsliders = 
        {
            "DialogueAnger",
            "DialogueFear",
            "DialogueHappy",
            "DialogueSad",
            "DialogueSurprise",
            "DialoguePuzzled",
            "DialogueDisgusted",
            "MoodNeutral",
            "MoodAnger",
            "MoodFear",
            "MoodHappy",
            "MoodSad",
            "MoodSurprise",
            "MoodPuzzled",
            "MoodDisgusted",
            "CombatAnger",
            "CombatShout",
            "MoodPuzzled",
            "MoodPuzzled",
        };

        const std::vector<std::string> sliderkeys = 
        {
            "HeadSlide",
            "HeadSlideMorph",
            "HeadSlideParts",
            "HeadSlideRace",
            "HeadSlideExpPhon",
            "HeadSlideExpMod",
            "HeadSlideExpExp"
        };

    public:
        static void UpdateHeadSlide(PAPYRUSFUNCHANDLE, RE::Actor* a_actor);

    private:
        void UpdateRaceSliders(RE::TESNPC* a_actorbase);
        void UpdateHeadPartsSliders(RE::TESNPC* a_actorbase);
        void UpdateChargenSliders(RE::TESNPC* a_actorbase);
        void UpdateRMSliders(RE::TESNPC* a_actorbase);
        void UpdateWeightSliders(RE::TESNPC* a_actorbase);
    private:
        bool _init = false;
        static REL::Relocation<decltype(UpdatePlayer)> UpdatePlayer_old;
        RM::FaceMorphInterface*     _faceinterface  = nullptr;
        RM::IBodyMorphInterface*    _morphinterface = nullptr;

        std::unordered_map<std::string,float> _oldvalues;
        std::unordered_map<std::string,float> _newvalues;

        bool _differ = false;
        bool _reset = false;
        uint64_t _framecnt = 0;

        // old selected values
        std::vector<uint16_t> _defaultpartsselected = {0,0,0,0};
        std::string _raceselected = "";
        uint8_t _expressionselected = 0;

        // config
        std::pair<int,std::string>  _framethd = {15,"General.iUpdateFrames"};
        std::pair<bool,std::string> _chargen = {true,"General.bChargen"};
        std::pair<bool,std::string> _rmmorphs = {true,"General.bRMMorphs"};
        std::pair<bool,std::string> _parts = {true,"General.bParts"};
        std::pair<bool,std::string> _race = {true,"General.bRace"};
        std::pair<bool,std::string> _weight = {true,"General.bWeight"};
        std::pair<bool,std::string> _expphon = {true,"General.bExpPhonems"};
        std::pair<bool,std::string> _expmod = {true,"General.bExpMods"};
        std::pair<bool,std::string> _expexp = {true,"General.bExpExp"};
    };
}