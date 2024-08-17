#pragma once

namespace HESL
{
    inline const std::vector<std::string> g_defaultparts = 
    {
        "Nose",
        "Unk",
        "Eyes",
        "Lip"
    };

    inline const std::vector<std::string> g_phonemesliders = 
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

    inline const std::vector<std::string> g_modifiersliders = 
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

    inline const std::vector<std::pair<std::string,std::string>> g_defaultsliders = 
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

    inline const std::vector<std::pair<std::string,std::pair<std::string,std::string>>> g_pairkeywords = 
    {
        {"LongShort",{"Short","Long"}},
        {"UpDown",{"Down","Up"}},
        {"WideNarrow",{"Narrow","Wide"}},
        {"ForwardBack",{"Back","Forward"}},
        {"InOut",{"Out","In"}},
        {"WideThin",{"Thin","Wide"}},
        {"ECE_EarSize",{"ece_EarSmall","ece_EarLarge"}},
        {"CME_Breton",{"CME_BretonRace_inv","CME_BretonRace"}},
        {"CME_DarkElf",{"CME_DarkElfRace_inv","CME_DarkElfRace"}},
        {"CME_Dremora",{"CME_DremoraRace_inv","CME_DremoraRace"}},
        {"CME_Elder",{"CME_ElderRace_inv","CME_ElderRace"}},
        {"CME_HighElf",{"CME_HighElfRace_inv","CME_HighElfRace"}},
        {"CME_Imperial",{"CME_ImperialRace_inv","CME_ImperialRace"}},
        {"CME_Nord",{"CME_NordRace_inv","CME_NordRace"}},
        {"CME_Orc",{"CME_OrcRace_inv","CME_OrcRace"}},
        {"CME_Redguard",{"CME_RedguardRace_inv","CME_RedguardRace"}},
        {"CME_WoodElf",{"CME_WoodElfRace_inv","CME_WoodElfRace"}},
    };


    inline const std::vector<std::string> g_expressionsliders = 
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

    inline const std::vector<std::string> g_sliderkeys = 
    {
        "HeadSlide",
        "HeadSlideMorph",
        "HeadSlideParts",
        "HeadSlideRace",
        "HeadSlideExpPhon",
        "HeadSlideExpMod",
        "HeadSlideExpExp"
    };

    struct RMSeries
    {
        int lasttype = 0;
        std::string slider;
    };

    inline std::unordered_map<std::string,RMSeries> g_seriesparse = 
    {
        {"ECE_EarShape",{0,"ece_HumEarType"}}
    };

    // config
    inline std::pair<int,std::string>  g_framethd = {15,"General.iUpdateFrames"};
    inline std::pair<bool,std::string> g_chargen = {true,"General.bChargen"};
    inline std::pair<bool,std::string> g_rmmorphs = {true,"General.bRMMorphs"};
    inline std::pair<bool,std::string> g_parts = {true,"General.bParts"};
    inline std::pair<bool,std::string> g_race = {true,"General.bRace"};
    inline std::pair<bool,std::string> g_weight = {true,"General.bWeight"};
    inline std::pair<bool,std::string> g_expphon = {true,"General.bExpPhonems"};
    inline std::pair<bool,std::string> g_expmod = {true,"General.bExpMods"};
    inline std::pair<bool,std::string> g_expexp = {true,"General.bExpExp"};
    inline std::pair<int,std::string> g_npcenable = {0,"General.iNPCMode"};

    inline std::pair<std::vector<std::string>,std::string> g_rmblacklist = {{},"General.asRMBlacklist"};
}