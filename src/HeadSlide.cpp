#include "HeadSlide.h"

SINGLETONBODY(HESL::HeadSlide)

REL::Relocation<decltype(HESL::HeadSlide::UpdatePlayer)> HESL::HeadSlide::UpdatePlayer_old;

void HESL::HeadSlide::Init()
{

    LOG("Dispatching msg")
    RM::InterfaceExchangeMessage loc_msg;
    SKSE::GetMessagingInterface()->Dispatch(RM::InterfaceExchangeMessage::kMessage_ExchangeInterface,&loc_msg, sizeof(RM::InterfaceExchangeMessage), nullptr);
    LOG("Dispatching finished -> 0x{:016X}",(uintptr_t)loc_msg.interfaceMap)

    _faceinterface  = (RM::FaceMorphInterface*)loc_msg.interfaceMap->QueryInterface("FaceMorph");
    _morphinterface = (RM::IBodyMorphInterface*)loc_msg.interfaceMap->QueryInterface("BodyMorph");

    REL::Relocation<std::uintptr_t> vtbl_player{RE::PlayerCharacter::VTABLE[0]};
    UpdatePlayer_old = vtbl_player.write_vfunc(REL::Module::GetRuntime() != REL::Module::Runtime::VR ? 0x0AD : 0x0AF, UpdatePlayer);
}

void HESL::HeadSlide::UpdatePlayer(RE::Actor* a_actor, float a_delta)
{
    if (a_delta > 0.0f) GetSingleton()->UpdateActor(a_actor);
    UpdatePlayer_old(a_actor,a_delta);
}

void HESL::HeadSlide::UpdateActor(RE::Actor* a_actor)
{
    //LOG("UpdateFaceSliders")
    if (a_actor == nullptr || _faceinterface == nullptr || _morphinterface == nullptr)
    {
        ERROR("Actor is either none, or there was error when importing RM API")
        return;
    }

    bool loc_updated = false;
    #define UPDATEMORPH(morph,key,value)                                \
    {                                                                   \
        float loc_val = _morphinterface->GetMorph(a_actor,morph,key);   \
        /*LOG("{} = {} / {}",morph,loc_val,value)*/                     \
        if (loc_val != value)                                           \
        {                                                               \
            _morphinterface->SetMorph(a_actor,morph,key,value);         \
            loc_updated = true;                                         \
        }                                                               \
    }

    //g_morphinterface->ClearBodyMorphKeys(a_actor,"HeadSlideMorph");

    auto loc_actorbase = a_actor->GetActorBase(); 
    auto loc_morph = _faceinterface->m_valueMap[loc_actorbase];
    for (auto&& it : loc_morph)
    {
        //LOG("RM Morphs: {} - {}",it.first.get()->c_str(),it.second)
        UPDATEMORPH(it.first.get()->c_str(),"HeadSlideMorph",it.second)
    }

    for (int i = 0; i < RE::TESNPC::FaceData::Morphs::kUnk; i++)
    {
        
        const float loc_plusvalue   = loc_actorbase->faceData->morphs[i] >= 0.0f ? loc_actorbase->faceData->morphs[i] : 0.0f;
        const float loc_minusvalue  = loc_actorbase->faceData->morphs[i] <  0.0f ? loc_actorbase->faceData->morphs[i]*(-1.0f) : 0.0f;
        UPDATEMORPH(defaultsliders[i].second.c_str(),"HeadSlide",loc_plusvalue)
        UPDATEMORPH(defaultsliders[i].first.c_str(),"HeadSlide",loc_minusvalue)

        //LOG("Default slides: {} -> {} = {} / {} = {}",loc_actorbase->faceData->morphs[i],g_defaultsliders[i].second,loc_plusvalue,g_defaultsliders[i].first,loc_minusvalue)
    }
    
    for (int i = 0; i < RE::TESNPC::FaceData::Parts::kTotal; i++)
    {
        
        if (i != RE::TESNPC::FaceData::Parts::kUnknown)
        {
            const std::string loc_selectedtype = defaultparts[i] + "SelType";
            const std::string loc_slider = defaultparts[i] + "Type" + std::to_string(loc_actorbase->faceData->parts[i]);

            UPDATEMORPH(loc_selectedtype.c_str(),"HeadSlide",loc_actorbase->faceData->parts[i])
            UPDATEMORPH(loc_slider.c_str(),"HeadSlide",1.0f)
        
            //LOG("Default parts: {} / {}",loc_selectedtype,loc_slider)
        }
    }

    //LOG("Race: {}",loc_actorbase->race->GetName())
    const std::string loc_raceslider = std::string(loc_actorbase->race->GetName()) + "Race";
    UPDATEMORPH(loc_raceslider.c_str(),"HeadSlide",1.0f)

    //LOG("Weight: {}",loc_actorbase->weight)
    UPDATEMORPH("SkinnyMorph","HeadSlide",loc_actorbase->weight/100.0f)

    // Expression

    RE::BSFaceGenAnimationData* loc_expdata = a_actor->GetFaceGenAnimationData();

    if (loc_expdata != nullptr)
    {
        for (int i = 0; i < 16; i++) 
        {
            const std::string loc_morphexp = phonemesliders[i];
            const float loc_value = loc_expdata->phenomeKeyFrame.values[i];
            //LOG("Phoneme: {} = {}",loc_morphexp,loc_value)
            UPDATEMORPH(loc_morphexp.c_str(),"HeadSlideExp",loc_value)
        }

        for (int i = 0; i < 14; i++) 
        {
            const std::string loc_morphexp = modifiersliders[i];
            const float loc_value = loc_expdata->modifierKeyFrame.values[i];
            //LOG("Modifier: {} = {}",loc_morphexp,loc_value)
            UPDATEMORPH(loc_morphexp.c_str(),"HeadSlideExp",loc_value)
        }
        //TODO
        //const size_t loc_count_exp = loc_expdata->expressionKeyFrame.count;
        //for (int i = 0; i < loc_count_exp;i++)
        //{
        //    if (loc_expdata->expressionKeyFrame.values[i] != 0.0f)
        //    {
        //
        //        break;
        //    }
        //}
    }

    _morphinterface->ApplyBodyMorphs(a_actor);

    if (loc_updated)
    {
        //LOG("Morph updated. Updating model weight")
        _morphinterface->UpdateModelWeight(a_actor);
    }
}

void HESL::HeadSlide::UpdateHeadSlide(PAPYRUSFUNCHANDLE, RE::Actor* a_actor)
{
    GetSingleton()->UpdateActor(a_actor);
}

