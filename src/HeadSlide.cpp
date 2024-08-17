#include "HeadSlide.h"
#include <boost/algorithm/string.hpp>
#include "Constants.h"
#include "Manager.h"

HESL::HeadSlide::HeadSlide(RE::Actor* a_actor)
{
    _target = a_actor;
    Reload();
}

void HESL::HeadSlide::UpdateActor(float a_delta)
{
    if (_target)
    {
        LOG("HeadSlide::UpdateActor({})",_target ? _target->GetName() : "NONE")
        if (_startdelay > 0) 
        {
            _startdelay--;
            return;
        }

        if (_reset)
        {
            HeadSlideManager::morphinterface->ClearBodyMorphKeys(_target,MORPHKEY);
            _reset = false;
        }

        // once per 5 frames
        if (((_framecnt % (g_framethd.first  > 0 ? g_framethd.first : 0)) == 0) && _framecnt > 0)
        {
            ReadyTempVars();
            UpdateSlidersMorphs();
            UpdateExpression();
            SetActorsMorphs();
        }
        _framecnt++;
    }
    else
    {
        ERROR("UpdateHeadSlide - Cant update actor as they are none")
    }
}

#define UPDATEMORPH(morph,key,value)                                \
{                                                                   \
    /*float loc_val = _morphinterface->GetMorph(a_actor,morph,key);*/\
    /*DEBUG("{} -> {} == {}",morph,_oldvalues[morph],value)*/       \
    if (!_differ && (_oldvalues[morph] != value))                   \
    {                                                               \
        _differ = true;                                             \
        DEBUG("{} differs from before. {} -> {}",morph,_oldvalues[morph],value) \
        /*_morphinterface->SetMorph(a_actor,morph,key,value);*/     \
        /*loc_updated = true;*/                                     \
    }                                                               \
    _newvalues[morph] = value;                                      \
}

void HESL::HeadSlide::UpdateSlidersMorphs()
{
    LOG("UpdateSlidersMorphs({})",_target ? _target->GetName() : "NONE")
    if (_target == nullptr || HeadSlideManager::faceinterface == nullptr || HeadSlideManager::morphinterface == nullptr)
    {
        ERROR("Actor is either none, or there was error when importing RM API")
        return;
    }

    //_morphinterface->ClearBodyMorphKeys(a_actor,"HeadSlideMorph");

    auto loc_actorbase = _target->GetActorBase(); 

    // RM sliders
    UpdateRMSliders(loc_actorbase);

    // Chargen sliders
    UpdateChargenSliders(loc_actorbase);

    // Head parts
    UpdateHeadPartsSliders(loc_actorbase);

    // Race
    UpdateRaceSliders(loc_actorbase);

    // Weight sliders
    UpdateWeightSliders(loc_actorbase);
}

void HESL::HeadSlide::UpdateExpression()
{
    LOG("UpdateExpression({})",_target ? _target->GetName() : "NONE")

    if (_target == nullptr || HeadSlideManager::faceinterface == nullptr || HeadSlideManager::morphinterface == nullptr)
    {
        ERROR("Actor is either none, or there was error when importing RM API")
        return;
    }

    // Expression
    RE::BSFaceGenAnimationData* loc_expdata = _target->GetFaceGenAnimationData();

    if (loc_expdata != nullptr)
    {
        if (g_expphon.first)
        {
            for (int i = 0; i < 16; i++) 
            {
                const std::string loc_morphexp = g_phonemesliders[i];
                const float loc_value = loc_expdata->phenomeKeyFrame.values[i];
                LOG("Phoneme: {} = {}",loc_morphexp,loc_value)
                UPDATEMORPH(loc_morphexp.c_str(),"HeadSlide",loc_value);
            }
        }

        if (g_expmod.first)
        {
            for (int i = 0; i < 14; i++) 
            {
                const std::string loc_morphexp = g_modifiersliders[i];
                const float loc_value = loc_expdata->modifierKeyFrame.values[i];
                LOG("Modifier: {} = {}",loc_morphexp,loc_value)
                UPDATEMORPH(loc_morphexp.c_str(),"HeadSlide",loc_value);
            }
        }

        if (g_expexp.first)
        {
            const size_t loc_count_exp = loc_expdata->expressionKeyFrame.count;
            for (int i = 0; i < loc_count_exp;i++)
            {
                if (loc_expdata->expressionKeyFrame.values[i] != 0.0f)
                {
                    if (_expressionselected != i)
                    {
                        _differ = true;
                        DEBUG("Expression changed from {} to {}",_expressionselected,i)
                    }
                    UPDATEMORPH(g_expressionsliders[i].c_str(),"HeadSlide",loc_expdata->expressionKeyFrame.values[i]);
                    _expressionselected = i;
                    break;
                }
            }
        }
    }
}

void HESL::HeadSlide::SetActorsMorphs()
{
    if (_target == nullptr || HeadSlideManager::morphinterface == nullptr || HeadSlideManager::faceinterface == nullptr)
    {
        ERROR("Actor is either none, or there was error when importing RM API")
        return;
    }

    if (_differ /*(_oldvalues.size() != _newvalues.size()) || !std::equal(_oldvalues.begin(), _oldvalues.end(),_newvalues.begin())*/)
    {
        LOG("Difference in morphs found. Setting new morhps...")

        HeadSlideManager::morphinterface->ClearBodyMorphKeys(_target,MORPHKEY);


        for (auto&& [morph,value] : _newvalues)
        {
            HeadSlideManager::morphinterface->SetMorph(_target,morph.c_str(),MORPHKEY,value);
            LOG("{} -> {}",morph,value)
        }

        if (_target->Is3DLoaded())
        {
            HeadSlideManager::morphinterface->UpdateModelWeight(_target, false);
            _target->Update3DModel();
        }

        _differ = false;
        LOG("***********************************************************************************")
    }
    //else _newvalues.clear();
}

void HESL::HeadSlide::Reload()
{
    //DEBUG("Reloading HeadSlide for {}")
    _oldvalues.clear();
    _newvalues.clear();

    _differ = false;
    _reset = false;
    _framecnt = 0;
    _startdelay = 200;

    _reset = true;
}

void HESL::HeadSlide::ReadyTempVars()
{
    _oldvalues = _newvalues;
    _newvalues.clear();
}

void HESL::HeadSlide::UpdateRaceSliders(RE::TESNPC* a_actorbase)
{
    if (g_race.first)
    {
        LOG("Race: {}",a_actorbase->race->GetName())
        //_morphinterface->ClearBodyMorphKeys(a_actor,"HeadSlideRace");
        std::string loc_race = a_actorbase->race->GetName();
        (void)loc_race.erase(std::remove_if(loc_race.begin(), loc_race.end(), isspace), loc_race.end());

        const std::string loc_raceslider = loc_race + "Race";
        if (_raceselected != loc_race) 
        {
            _differ = true;
            DEBUG("Race changed from {} to {}",_raceselected,loc_race)
        }

        UPDATEMORPH(loc_raceslider.c_str(),"HeadSlide",1.0f)
        _raceselected = loc_race;
    }
}

void HESL::HeadSlide::UpdateHeadPartsSliders(RE::TESNPC* a_actorbase)
{
    if (g_parts.first)
    {
        //_morphinterface->ClearBodyMorphKeys(a_actor,"HeadSlideParts");
        for (int i = 0; i < RE::TESNPC::FaceData::Parts::kTotal; i++)
        {
            if (i != RE::TESNPC::FaceData::Parts::kUnknown)
            {
                uint16_t loc_selpart = a_actorbase->faceData->parts[i];
                const std::string loc_slider = g_defaultparts[i] + "Type" + std::to_string(loc_selpart);

                if (_defaultpartsselected[i] != loc_selpart)
                {
                    _differ = true;
                    DEBUG("Head part {} changed from {} to {}",g_defaultparts[i],_defaultpartsselected[i],loc_selpart)
                }

                UPDATEMORPH(loc_slider.c_str(),"HeadSlide",1.0f)

                _defaultpartsselected[i] = loc_selpart;

                LOG("Default parts: {}",loc_slider)
            }
        }
    }
}

void HESL::HeadSlide::UpdateChargenSliders(RE::TESNPC* a_actorbase)
{
    if (g_chargen.first)
    {
        for (int i = 0; i < RE::TESNPC::FaceData::Morphs::kUnk; i++)
        {
            const float loc_plusvalue   = a_actorbase->faceData->morphs[i] >= 0.0f ? a_actorbase->faceData->morphs[i] : 0.0f;
            const float loc_minusvalue  = a_actorbase->faceData->morphs[i] <  0.0f ? a_actorbase->faceData->morphs[i]*(-1.0f) : 0.0f;
            UPDATEMORPH(g_defaultsliders[i].second.c_str(),"HeadSlide",loc_plusvalue)
            UPDATEMORPH(g_defaultsliders[i].first.c_str(),"HeadSlide",loc_minusvalue)

            LOG("Default slides: {} -> {} = {} / {} = {}",a_actorbase->faceData->morphs[i],g_defaultsliders[i].second,loc_plusvalue,g_defaultsliders[i].first,loc_minusvalue)
        }
    }
}

void HESL::HeadSlide::UpdateRMSliders(RE::TESNPC* a_actorbase)
{
    if (g_rmmorphs.first)
    {
        auto loc_morph = HeadSlideManager::faceinterface->m_valueMap[a_actorbase];
        for (auto&& [morph,value] : loc_morph)
        {
            LOG("RM Morphs: {} - {}",morph.get()->c_str(),value)

            std::string loc_morphraw = morph.get()->c_str();
            std::transform(loc_morphraw.begin(), loc_morphraw.end(), loc_morphraw.begin(), ::tolower);;

            bool loc_blacklisted = false;
            for (auto&& it : g_rmblacklist.first)
            {
                
                if (loc_morphraw == it)
                {
                    LOG("Morph {} is blacklisted, and skipped",morph.get()->c_str())
                    loc_blacklisted = true;
                    break;
                }
            }
            if (loc_blacklisted) continue;

            bool loc_simplevalue = true;
            for (auto&& [kw,suf] : g_pairkeywords)
            {
            
                auto loc_regstr = std::format(R"regex((.*){}\b)regex",kw);
                std::regex loc_regex(loc_regstr);
                if (std::regex_match(morph.get()->c_str(),loc_regex))
                {
                    auto loc_base = std::regex_replace(morph.get()->c_str(),loc_regex,"$1");
                    std::string loc_neg = suf.first  != "" ? loc_base + suf.first : "";
                    std::string loc_pos = suf.second != "" ? loc_base + suf.second : "";

                    if (loc_neg != "" || loc_pos != "")
                    {
                        const float loc_plusvalue   = value >= 0.0f ? value : 0.0f;
                        const float loc_minusvalue  = value <  0.0f ? value*(-1.0f) : 0.0f;
                        LOG("Pair found: {} -> {} = {} / {} = {}",morph.get()->c_str(),loc_neg,loc_minusvalue,loc_pos,loc_plusvalue)
                        UPDATEMORPH(loc_pos.c_str(),"HeadSlide",loc_plusvalue)
                        UPDATEMORPH(loc_neg.c_str(),"HeadSlide",loc_minusvalue)
                        loc_simplevalue = false;
                        break;
                    }
            
                }
            }

            if (loc_simplevalue) 
            {
                bool loc_series = false;
                for (auto&& [kw,type] : g_seriesparse)
                {
                    if (morph.get()->c_str() == kw)
                    {
                        LOG("{} is series",morph.get()->c_str())
                        const int loc_newvalue = static_cast<int>(value + 0.5f); // round the value
                        if (type.lasttype != loc_newvalue)
                        {
                            _differ = true;
                            DEBUG("Series {} type changed from {} to {}",morph.get()->c_str(),type.lasttype,loc_newvalue)
                        }
                        std::string loc_type = type.slider + std::to_string(loc_newvalue);
                        UPDATEMORPH(loc_type.c_str(),"HeadSlide",1.0f)
                        type.lasttype = loc_newvalue;

                        loc_series = true;
                        break;
                    }
                }
                
                if (!loc_series) UPDATEMORPH(morph.get()->c_str(),"HeadSlide",value)
            }
        }
    }
}

void HESL::HeadSlide::UpdateWeightSliders(RE::TESNPC* a_actorbase)
{
    if (g_weight.first)
    {
        LOG("Weight: {}",a_actorbase->weight)
        UPDATEMORPH("SkinnyMorph","HeadSlide",1.0f - (a_actorbase->weight/100.0f))
    }
}

#undef UPDATEMORPH
