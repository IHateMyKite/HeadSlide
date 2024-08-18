Scriptname HeadSlide extends RaceMenuBase

Function OnInit()
    RegisterForModEvent("HeadSlide_UpdateEvent", "OnHeadUpdate")
EndFunction

Function _SendUpdateEvent()
    RegisterForModEvent("HeadSlide_UpdateEvent", "OnHeadUpdate")
    int loc_handle = ModEvent.Create("HeadSlide_UpdateEvent")
    if loc_handle
        ModEvent.Send(loc_handle)
    endif
EndFunction

Function OnHeadUpdate()
    Utility.waitMenuMode(0.1)
    
    if !_target
        _target = Game.GetPlayer()
    endif
    
    HeadSlideNative.UpdateHeadSlide(_target)
    
    if UI.IsMenuOpen("RaceSex Menu")
        _SendUpdateEvent()
    endif
EndFunction

Actor _target
Event OnInitializeMenu(Actor player, ActorBase playerBase)
    _target = player
    Utility.waitMenuMode(1.0)
    OnHeadUpdate()
EndEvent
