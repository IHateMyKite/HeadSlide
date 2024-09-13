#pragma once
#include "RaceMenuAPI.h"

namespace HESL
{
    #define MORPHKEY "HeadSlide"

    struct RMSeries;
    class HeadSlideManager;

    class HeadSlide
    {
    public:
        HeadSlide(RE::Actor* a_actor);
        virtual void UpdateActor(float a_delta);

        virtual void UpdateSlidersMorphs();
        virtual void UpdateExpression();
        virtual void SetActorsMorphs();

        virtual void Reload();
        void ReadyTempVars();

        RE::Actor* GetTarget() {return _target;}
    protected:
        void UpdateRaceSliders(RE::TESNPC* a_actorbase);
        void UpdateHeadPartsSliders(RE::TESNPC* a_actorbase);
        void UpdateChargenSliders(RE::TESNPC* a_actorbase);
        void UpdateRMSliders(RE::TESNPC* a_actorbase);
        void UpdateWeightSliders(RE::TESNPC* a_actorbase);
    protected:
        bool _init = false;
        RE::Actor* _target;

        std::unordered_map<std::string,float> _oldvalues;
        std::unordered_map<std::string,float> _newvalues;

        bool _differ = false;
        bool _reset = false;
        uint64_t _framecnt = 0;
        uint64_t _framecntreq = 0;
        int _startdelay = 200;

        // old selected values
        std::vector<uint16_t> _defaultpartsselected = {0,0,0,0};
        std::string _raceselected = "";
        uint8_t _expressionselected = 0;
    };
}