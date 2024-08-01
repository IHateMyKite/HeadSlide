#include "HeadSlide.h"

SINGLETONBODY(HESL::HeadSlide)

REL::Relocation<decltype(HESL::HeadSlide::UpdatePlayer)> HESL::HeadSlide::UpdatePlayer_old;

void HESL::HeadSlide::Init()
{
    if (!_init)
    {
        _init = true;
        DEBUG("Getting racemenu API")
        RM::InterfaceExchangeMessage loc_msg;
        SKSE::GetMessagingInterface()->Dispatch(RM::InterfaceExchangeMessage::kMessage_ExchangeInterface,&loc_msg, sizeof(RM::InterfaceExchangeMessage), nullptr);
        DEBUG("Dispatching finished -> 0x{:016X}",(uintptr_t)loc_msg.interfaceMap)

        _faceinterface  = (RM::FaceMorphInterface*)loc_msg.interfaceMap->QueryInterface("FaceMorph");
        _morphinterface = (RM::IBodyMorphInterface*)loc_msg.interfaceMap->QueryInterface("BodyMorph");

        if (_faceinterface == nullptr || _morphinterface == nullptr) 
        {
            ERROR("Failed to get API from RaceMenu!!!")
            return;
        }

        Reload();

        REL::Relocation<std::uintptr_t> vtbl_player{RE::PlayerCharacter::VTABLE[0]};
        UpdatePlayer_old = vtbl_player.write_vfunc(REL::Module::GetRuntime() != REL::Module::Runtime::VR ? 0x0AD : 0x0AF, UpdatePlayer);


    }
    else
    {
        Reload();
    }
}

void HESL::HeadSlide::UpdatePlayer(RE::Actor* a_actor, float a_delta)
{
    if (a_delta > 0.0f) GetSingleton()->UpdateActor(a_actor,a_delta);
    UpdatePlayer_old(a_actor,a_delta);
}

void HESL::HeadSlide::UpdateActor(RE::Actor* a_actor, float a_delta)
{
    if (_reset)
    {
        _morphinterface->ClearBodyMorphKeys(a_actor,MORPHKEY);
        _reset = false;
    }

    // once per 5 frames
    if ((++_framecnt % (_framethd.first  > 0 ? _framethd.first : 0)) == 0)
    {
        ReadyTempVars(a_actor);
        UpdateSlidersMorphs(a_actor);
        UpdateExpression(a_actor);
        SetActorsMorphs(a_actor);
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

void HESL::HeadSlide::UpdateSlidersMorphs(RE::Actor* a_actor)
{
    LOG("UpdateSlidersMorphs({})",a_actor ? a_actor->GetName() : "NONE")
    if (a_actor == nullptr || _faceinterface == nullptr || _morphinterface == nullptr)
    {
        ERROR("Actor is either none, or there was error when importing RM API")
        return;
    }

    //_morphinterface->ClearBodyMorphKeys(a_actor,"HeadSlideMorph");

    auto loc_actorbase = a_actor->GetActorBase(); 

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

void HESL::HeadSlide::UpdateExpression(RE::Actor* a_actor)
{
    LOG("UpdateExpression({})",a_actor ? a_actor->GetName() : "NONE")

    if (a_actor == nullptr || _faceinterface == nullptr || _morphinterface == nullptr)
    {
        ERROR("Actor is either none, or there was error when importing RM API")
        return;
    }

    // Expression
    RE::BSFaceGenAnimationData* loc_expdata = a_actor->GetFaceGenAnimationData();

    if (loc_expdata != nullptr)
    {
        if (_expphon.first)
        {
            for (int i = 0; i < 16; i++) 
            {
                const std::string loc_morphexp = phonemesliders[i];
                const float loc_value = loc_expdata->phenomeKeyFrame.values[i];
                LOG("Phoneme: {} = {}",loc_morphexp,loc_value)
                UPDATEMORPH(loc_morphexp.c_str(),"HeadSlide",loc_value);
            }
        }

        if (_expmod.first)
        {
            for (int i = 0; i < 14; i++) 
            {
                const std::string loc_morphexp = modifiersliders[i];
                const float loc_value = loc_expdata->modifierKeyFrame.values[i];
                LOG("Modifier: {} = {}",loc_morphexp,loc_value)
                UPDATEMORPH(loc_morphexp.c_str(),"HeadSlide",loc_value);
            }
        }

        if (_expexp.first)
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
                    UPDATEMORPH(expressionsliders[i].c_str(),"HeadSlide",loc_expdata->expressionKeyFrame.values[i]);
                    _expressionselected = i;
                    break;
                }
            }
        }
    }
}

void HESL::HeadSlide::SetActorsMorphs(RE::Actor* a_actor)
{
    if (_differ /*(_oldvalues.size() != _newvalues.size()) || !std::equal(_oldvalues.begin(), _oldvalues.end(),_newvalues.begin())*/)
    {
        DEBUG("Difference in morphs found. Setting new morhps...")

        _morphinterface->ClearBodyMorphKeys(a_actor,MORPHKEY);



        for (auto&& [morph,value] : _newvalues)
        {
            _morphinterface->SetMorph(a_actor,morph.c_str(),MORPHKEY,value);
            LOG("{} -> {}",morph,value)
        }

        _morphinterface->ApplyBodyMorphs(a_actor);
        _morphinterface->UpdateModelWeight(a_actor);

        _differ = false;
        DEBUG("***********************************************************************************")
    }
    //else _newvalues.clear();
}

void HESL::HeadSlide::Reload()
{
    DEBUG("Reloading HeadSlide")
    _oldvalues.clear();
    _newvalues.clear();

    #define UPDATECONFIG(val,type)                                                   \
    {                                                                                \
        val.first = Config::GetSingleton()->GetVariable<type>(val.second,val.first); \
        DEBUG("Config variable loaded: {} -> {}",val.second,val.first)               \
    }

    UPDATECONFIG(_framethd,int)
    UPDATECONFIG(_chargen ,bool)
    UPDATECONFIG(_rmmorphs,bool)
    UPDATECONFIG(_parts   ,bool)
    UPDATECONFIG(_race    ,bool)
    UPDATECONFIG(_weight  ,bool)
    UPDATECONFIG(_expphon ,bool)
    UPDATECONFIG(_expmod  ,bool)
    UPDATECONFIG(_expexp  ,bool)

    #undef UPDATECONFIG

    _reset = true;
}

void HESL::HeadSlide::ReadyTempVars(RE::Actor* a_actor)
{
    _oldvalues = _newvalues;
    _newvalues.clear();
}

void HESL::HeadSlide::UpdateHeadSlide(PAPYRUSFUNCHANDLE, RE::Actor* a_actor)
{
    GetSingleton()->ReadyTempVars(a_actor);
    GetSingleton()->UpdateSlidersMorphs(a_actor);
    GetSingleton()->UpdateExpression(a_actor);
    GetSingleton()->SetActorsMorphs(a_actor);
}

void HESL::HeadSlide::UpdateRaceSliders(RE::TESNPC* a_actorbase)
{
    if (_race.first)
    {
        LOG("Race: {}",a_actorbase->race->GetName())
        //_morphinterface->ClearBodyMorphKeys(a_actor,"HeadSlideRace");
        const std::string loc_race = a_actorbase->race->GetName();
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
    if (_parts.first)
    {
        //_morphinterface->ClearBodyMorphKeys(a_actor,"HeadSlideParts");
        for (int i = 0; i < RE::TESNPC::FaceData::Parts::kTotal; i++)
        {
            if (i != RE::TESNPC::FaceData::Parts::kUnknown)
            {
                uint16_t loc_selpart = a_actorbase->faceData->parts[i];
                const std::string loc_slider = defaultparts[i] + "Type" + std::to_string(loc_selpart);

                if (_defaultpartsselected[i] != loc_selpart)
                {
                    _differ = true;
                    DEBUG("Head part {} changed from {} to {}",defaultparts[i],_defaultpartsselected[i],loc_selpart)
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
    if (_chargen.first)
    {
        for (int i = 0; i < RE::TESNPC::FaceData::Morphs::kUnk; i++)
        {
            const float loc_plusvalue   = a_actorbase->faceData->morphs[i] >= 0.0f ? a_actorbase->faceData->morphs[i] : 0.0f;
            const float loc_minusvalue  = a_actorbase->faceData->morphs[i] <  0.0f ? a_actorbase->faceData->morphs[i]*(-1.0f) : 0.0f;
            UPDATEMORPH(defaultsliders[i].second.c_str(),"HeadSlide",loc_plusvalue)
            UPDATEMORPH(defaultsliders[i].first.c_str(),"HeadSlide",loc_minusvalue)

            LOG("Default slides: {} -> {} = {} / {} = {}",a_actorbase->faceData->morphs[i],defaultsliders[i].second,loc_plusvalue,defaultsliders[i].first,loc_minusvalue)
        }
    }
}

void HESL::HeadSlide::UpdateRMSliders(RE::TESNPC* a_actorbase)
{
    if (_rmmorphs.first)
    {
        auto loc_morph = _faceinterface->m_valueMap[a_actorbase];
        for (auto&& [morph,value] : loc_morph)
        {
            LOG("RM Morphs: {} - {}",morph.get()->c_str(),value)

            bool loc_simplevalue = true;
            for (auto&& [kw,suf] : pairkeywords)
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
                        LOG("{} -> {} = {} / {} = {}",morph.get()->c_str(),loc_neg,loc_minusvalue,loc_pos,loc_plusvalue)
                        UPDATEMORPH(loc_pos.c_str(),"HeadSlide",loc_plusvalue)
                        UPDATEMORPH(loc_neg.c_str(),"HeadSlide",loc_minusvalue)
                        loc_simplevalue = false;
                        break;
                    }
            
                }
            }

            if (loc_simplevalue) UPDATEMORPH(morph.get()->c_str(),"HeadSlide",value)
        }
    }
}

void HESL::HeadSlide::UpdateWeightSliders(RE::TESNPC* a_actorbase)
{
    if (_weight.first)
    {
        LOG("Weight: {}",a_actorbase->weight)
        UPDATEMORPH("SkinnyMorph","HeadSlide",1.0f - (a_actorbase->weight/100.0f))
    }
}

#undef UPDATEMORPH