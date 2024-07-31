Scriptname HeadSlide extends RaceMenuBase

Function _SendUpdateEvent()
    int loc_handle = ModEvent.Create("HeadSlide_UpdateEvent")
    if loc_handle
        ModEvent.Send(loc_handle)
    endif
EndFunction

bool loc_updatemutex = false
Function OnHeadUpdate()
    if loc_updatemutex
        return
    endif
    loc_updatemutex = true
    Utility.waitMenuMode(0.5)
    
    ; TODO: Call native function
    
    if UI.IsMenuOpen("RaceSex Menu")
        _SendUpdateEvent()
    endif
    loc_updatemutex = false
EndFunction

Event OnInitializeMenu(Actor player, ActorBase playerBase)
    ConsoleUtils.PrintMessage("OnInitializeMenu")
    RegisterForModEvent("HeadSlide_UpdateEvent", "OnHeadUpdate")
    OnHeadUpdate()
EndEvent

Event OnResetMenu(Actor player, ActorBase playerBase)
    ConsoleUtils.PrintMessage("OnResetMenu")
    UnregisterForModEvent("HeadSlide_UpdateEvent")
EndEvent
