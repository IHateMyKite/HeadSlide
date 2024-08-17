#include "Manager.h"
#include "Constants.h"
#include "Config.h"
#include <boost/algorithm/string.hpp>
#include "HeadSlide.h"

SINGLETONBODY(HESL::HeadSlideManager)

REL::Relocation<decltype(HESL::HeadSlideManager::UpdatePlayerHook)> HESL::HeadSlideManager::UpdatePlayer_old;
REL::Relocation<decltype(HESL::HeadSlideManager::UpdateCharacterHook)> HESL::HeadSlideManager::UpdateCharacter_old;

SKEE::FaceMorphInterface*  HESL::HeadSlideManager::faceinterface    = nullptr;
SKEE::IBodyMorphInterface* HESL::HeadSlideManager::morphinterface   = nullptr;

#undef Config

void HESL::HeadSlideManager::Init()
{
    if (!_init)
        {
        DEBUG("Getting racemenu API")
        SKEE::InterfaceExchangeMessage loc_msg;
        SKSE::GetMessagingInterface()->Dispatch(SKEE::InterfaceExchangeMessage::kMessage_ExchangeInterface,(void*)&loc_msg, sizeof(SKEE::InterfaceExchangeMessage*), "skee");
        DEBUG("Dispatching finished -> 0x{:016X}",(uintptr_t)loc_msg.interfaceMap)

        // === For some reason, these functions are broken, and sometimes returns wrong interface.......
        auto loc_facemorph = static_cast<SKEE::FaceMorphInterface*>(loc_msg.interfaceMap->QueryInterface("FaceMorph"));
        auto loc_bodymorph = static_cast<SKEE::IBodyMorphInterface*>(loc_msg.interfaceMap->QueryInterface("BodyMorph"));

        if (loc_facemorph == nullptr || loc_bodymorph == nullptr) 
        {
            ERROR("Failed to get API from RaceMenu!!!")
            return;
        }

        auto loc_ver = loc_bodymorph->GetVersion();
        DEBUG("Bodymorph interface version = {}",loc_ver)

        if (loc_ver >= 4)
        {
            faceinterface  = static_cast<SKEE::FaceMorphInterface*>(loc_facemorph);
            morphinterface = static_cast<SKEE::IBodyMorphInterface*>(loc_bodymorph);
        }
        else
        {
            ERROR("Bodymorph interface version too old! Disabling mod...",loc_ver)
            return;
        }

        auto loc_player = RE::PlayerCharacter::GetSingleton();
        _npclist[loc_player->GetHandle().native_handle()] = std::shared_ptr<HeadSlide>(new HeadSlide(loc_player));

        REL::Relocation<std::uintptr_t> vtbl_player{RE::PlayerCharacter::VTABLE[0]};
        UpdatePlayer_old = vtbl_player.write_vfunc(REL::Module::GetRuntime() != REL::Module::Runtime::VR ? 0x0AD : 0x0AF, UpdatePlayerHook);

        REL::Relocation<std::uintptr_t> vtbl_character{RE::Character::VTABLE[0]};
        UpdateCharacter_old = vtbl_character.write_vfunc(REL::Module::GetRuntime() != REL::Module::Runtime::VR ? 0x0AD : 0x0AF, UpdateCharacterHook);

        Reload();

        _init = true;
    }
    else
    {
        Reload();
    }
}

void HESL::HeadSlideManager::Reload()
{
    DEBUG("Reloading HeadSlide")

    #define UPDATECONFIG(val,type)                                                   \
    {                                                                                \
        val.first = Config::GetSingleton()->GetVariable<type>(val.second,val.first); \
        DEBUG("Config variable loaded: {} -> {}",val.second,val.first)               \
    }
    #define UPDATECONFIGARR(val,type)                                                \
    {                                                                                \
        val.first = Config::GetSingleton()->GetArray<type>(val.second);              \
        DEBUG("Config variable loaded: {} -> {}",val.second,boost::join(val.first,",")) \
    }

    UPDATECONFIG(g_framethd,int)
    UPDATECONFIG(g_chargen ,bool)
    UPDATECONFIG(g_rmmorphs,bool)
    UPDATECONFIG(g_parts   ,bool)
    UPDATECONFIG(g_race    ,bool)
    UPDATECONFIG(g_weight  ,bool)
    UPDATECONFIG(g_expphon ,bool)
    UPDATECONFIG(g_expmod  ,bool)
    UPDATECONFIG(g_expexp  ,bool)
    UPDATECONFIG(g_npcenable ,int)
        

    UPDATECONFIGARR(g_rmblacklist,std::string)

    #undef UPDATECONFIG

    for (auto&& [handle,headslide] : _npclist)
    {
        headslide->Reload();
    }
}

void HESL::HeadSlideManager::UpdateNPC(RE::Actor* a_actor, float a_delta)
{
    auto loc_handle = a_actor->GetHandle().native_handle();
    SKSE::GetTaskInterface()->AddTask([this,loc_handle,a_delta]
    {
        auto loc_actor = RE::Actor::LookupByHandle(loc_handle).get();
        LOG("HeadSlideManager::UpdateNPC({}) called",loc_actor ? loc_actor->GetName() : "NONE")
        if (loc_actor == nullptr) return;
        auto loc_headslide = _npclist[loc_handle];
        if (!loc_headslide) 
        {
            loc_headslide = std::shared_ptr<HeadSlide>(new HeadSlide(loc_actor));
            _npclist[loc_handle] = loc_headslide;
        }
        loc_headslide->UpdateActor(a_delta);
    });
}

void HESL::HeadSlideManager::UpdatePlayerHook(RE::Actor* a_actor, float a_delta)
{
    if (a_delta > 0.0f) HeadSlideManager::GetSingleton()->UpdateNPC(a_actor,a_delta);
    UpdatePlayer_old(a_actor,a_delta);
}

void HESL::HeadSlideManager::UpdateCharacterHook(RE::Actor* a_actor, float a_delta)
{
    if (g_npcenable.first > 0)
    {
        if (a_actor)
        {
            const auto loc_refBase = a_actor->GetActorBase();
            if(a_actor->Is(RE::FormType::NPC) || (loc_refBase && loc_refBase->Is(RE::FormType::NPC)))
            {
                if (a_actor->GetRace()->GetPlayable())
                {
                    bool loc_update = false;
                    switch(g_npcenable.first)
                    {
                    case 1:
                        loc_update = true;
                        break;
                    case 2:
                        loc_update = a_actor->GetActorRuntimeData().boolBits.any(RE::Actor::BOOL_BITS::kPlayerTeammate);
                        break;
                    case 3:
                        auto loc_player = RE::PlayerCharacter::GetSingleton();
                        if (a_actor->GetParentCell() == loc_player->GetParentCell())
                        {
                            auto loc_pos1 = a_actor->GetPosition();
                            auto loc_pos2 = loc_player->GetPosition();
                            auto loc_distance = loc_pos1.GetDistance(loc_pos2);
                            loc_update = (loc_distance <= 2000.0f);
                        }
                        break;
                    }
                    if (loc_update)
                    {
                        HeadSlideManager::GetSingleton()->UpdateNPC(a_actor,a_delta);
                    }
                }
            }
        }
    }
    UpdateCharacter_old(a_actor,a_delta);
}

void HESL::HeadSlideManager::ForceUpdateHeadSlide(RE::Actor* a_actor)
{
    if (a_actor != nullptr)
    {
        auto loc_handle = a_actor->GetHandle().native_handle();
        auto loc_headslide = _npclist[loc_handle];
        if (!loc_headslide) 
        {
            _npclist[loc_handle] = std::shared_ptr<HeadSlide>(new HeadSlide(a_actor));
            loc_headslide = _npclist[loc_handle];
        }

        loc_headslide->ReadyTempVars();
        loc_headslide->UpdateSlidersMorphs();
        loc_headslide->UpdateExpression();
        loc_headslide->SetActorsMorphs();
    }
}

void HESL::HeadSlideManager::UpdateHeadSlide(PAPYRUSFUNCHANDLE, RE::Actor* a_actor)
{
    LOG("HeadSlideManager::UpdateHeadSlide({}) called", a_actor ? a_actor->GetName() : "NONE")
    if (a_actor)
    {
        GetSingleton()->ForceUpdateHeadSlide(a_actor);
    }
    else
    {
        ERROR("UpdateHeadSlide - Cant update actor as they are none")
    }
}