#include "ZFImpl_sys_SDL_ZFGameKit_impl.h"
#include "ZFGameKit/protocol/ZFProtocolP2GamePad.h"
#include "ZFImpl/sys_SDL/ZFMainEntry_sys_SDL.h"

#if ZF_ENV_sys_SDL
ZF_NAMESPACE_GLOBAL_BEGIN

ZFPROTOCOL_IMPLEMENTATION_BEGIN(P2GamePadImpl_sys_SDL, P2GamePad, v_ZFProtocolLevel::e_SystemHigh)
public:
    zfoverride
    virtual void protocolOnInit(void) {
        zfsuper::protocolOnInit();
        SDL_InitSubSystem(SDL_INIT_GAMEPAD);
        implAttached() = zftrue;
        _implMappingInit();
        _implStateInit();
    }
    zfoverride
    virtual void protocolOnDealloc(void) {
        implAttached() = zffalse;
        SDL_QuitSubSystem(SDL_INIT_GAMEPAD);
        zfsuper::protocolOnDealloc();
    }

public:
    static zfbool &implAttached(void) {
        static zfbool d = zffalse;
        return d;
    }
    static void padAttach(ZF_IN SDL_JoystickID implId) {
        SDL_Gamepad *impl = SDL_GetGamepadFromID(implId);
        if(impl == NULL) {
            impl = SDL_OpenGamepad(implId);
            if(impl == NULL) {
                return;
            }
        }
        int padIndex = SDL_GetGamepadPlayerIndex(impl);
        if(padIndex < 0 || padIndex >= (int)P2GamePadMax()) {
            return;
        }
        P2GamePadImpl_notifyPadChange((zfindex)padIndex, zftrue);

        {
            _ButtonMap &m = _buttonMap();
            for(zfiter it = m.iter(); it; ++it) {
                if(SDL_GetGamepadButton(impl, m.iterKey(it))) {
                    P2GamePadImpl_notifyButtonValue((zfindex)padIndex, m.iterValue(it), 1);
                }
            }
        }
        {
            _AxisMap &m = _axisMap();
            for(zfiter it = m.iter(); it; ++it) {
                Sint16 implValue = SDL_GetGamepadAxis(impl, m.iterKey(it));
                if(implValue != 0) {
                    P2GamePadImpl_notifyButtonValue((zfindex)padIndex, m.iterValue(it), (zffloat)(implValue / 32768.0f));
                }
            }
        }
    }
    static void padDetach(ZF_IN SDL_JoystickID implId) {
        SDL_Gamepad *impl = SDL_GetGamepadFromID(implId);
        if(impl == NULL) {
            return;
        }
        int padIndex = SDL_GetGamepadPlayerIndex(impl);
        SDL_CloseGamepad(impl);
        if(padIndex < 0 || padIndex >= (int)P2GamePadMax()) {
            return;
        }
        P2GamePadImpl_notifyPadChange((zfindex)padIndex, zffalse);
    }
    static void buttonDown(ZF_IN SDL_JoystickID implId, ZF_IN SDL_GamepadButton implButton) {
        _notifyButtonValue(implId, implButton, 1);
    }
    static void buttonUp(ZF_IN SDL_JoystickID implId, ZF_IN SDL_GamepadButton implButton) {
        _notifyButtonValue(implId, implButton, 0);
    }
    static void axisMove(ZF_IN SDL_JoystickID implId, ZF_IN SDL_GamepadAxis implAxis, ZF_IN Sint16 implValue) {
        _notifyButtonValue(implId, implAxis, (zffloat)(implValue / 32768.0f));
    }
private:
    typedef ZFCoreMap<SDL_GamepadButton, P2GamePadButton> _ButtonMap;
    typedef ZFCoreMap<SDL_GamepadAxis, P2GamePadButton> _AxisMap;
    static _ButtonMap &_buttonMap(void) {
        static _ButtonMap d;
        return d;
    }
    static _AxisMap &_axisMap(void) {
        static _AxisMap d;
        return d;
    }
    static void _buttonMapInit(void) {
        _ButtonMap &m = _buttonMap();
        m.access(SDL_GAMEPAD_BUTTON_DPAD_LEFT) = v_P2GamePadButton::e_DPadL;
        m.access(SDL_GAMEPAD_BUTTON_DPAD_UP) = v_P2GamePadButton::e_DPadT;
        m.access(SDL_GAMEPAD_BUTTON_DPAD_RIGHT) = v_P2GamePadButton::e_DPadR;
        m.access(SDL_GAMEPAD_BUTTON_DPAD_DOWN) = v_P2GamePadButton::e_DPadB;
        m.access(SDL_GAMEPAD_BUTTON_WEST) = v_P2GamePadButton::e_BtnL;
        m.access(SDL_GAMEPAD_BUTTON_NORTH) = v_P2GamePadButton::e_BtnT;
        m.access(SDL_GAMEPAD_BUTTON_EAST) = v_P2GamePadButton::e_BtnR;
        m.access(SDL_GAMEPAD_BUTTON_SOUTH) = v_P2GamePadButton::e_BtnB;
        m.access(SDL_GAMEPAD_BUTTON_BACK) = v_P2GamePadButton::e_Select;
        m.access(SDL_GAMEPAD_BUTTON_START) = v_P2GamePadButton::e_Start;
        m.access(SDL_GAMEPAD_BUTTON_LEFT_SHOULDER) = v_P2GamePadButton::e_L1;
        m.access(SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER) = v_P2GamePadButton::e_R1;
        m.access(SDL_GAMEPAD_BUTTON_LEFT_STICK) = v_P2GamePadButton::e_L3;
        m.access(SDL_GAMEPAD_BUTTON_RIGHT_STICK) = v_P2GamePadButton::e_R3;
    }
    static void _axisMapInit(void) {
        _AxisMap &m = _axisMap();
        m.access(SDL_GAMEPAD_AXIS_LEFT_TRIGGER) = v_P2GamePadButton::e_L2;
        m.access(SDL_GAMEPAD_AXIS_RIGHT_TRIGGER) = v_P2GamePadButton::e_R2;
        m.access(SDL_GAMEPAD_AXIS_LEFTX) = v_P2GamePadButton::e_LStickX;
        m.access(SDL_GAMEPAD_AXIS_LEFTY) = v_P2GamePadButton::e_LStickY;
        m.access(SDL_GAMEPAD_AXIS_RIGHTX) = v_P2GamePadButton::e_RStickX;
        m.access(SDL_GAMEPAD_AXIS_RIGHTY) = v_P2GamePadButton::e_RStickY;
    }
    static void _implMappingInit(void) {
        _buttonMapInit();
        _axisMapInit();
        ZFInput src = ZFInputForRes("ZFGameKit_impl/sys_SDL/gamepad/gamecontrollerdb.txt");
        if(src) {
            SDL_AddGamepadMappingsFromIO(ZFImpl_sys_SDL_ZFInputToSDL_IOStream(src), true);
        }
    }
    static void _implStateInit(void) {
        int count = 0;
        SDL_JoystickID *implIds = SDL_GetGamepads(&count);
        if(implIds == NULL) {
            return;
        }
        zfcleanupInScope_1({
                SDL_free(implIds);
            }
            , SDL_JoystickID *, implIds
            );
        for(int i = 0; i < count; ++i) {
            padAttach(implIds[i]);
        }
    }
    static void _notifyButtonValue(ZF_IN SDL_JoystickID implId, ZF_IN SDL_GamepadButton implButton, ZF_IN zffloat value) {
        int padIndex = SDL_GetGamepadPlayerIndexForID(implId);
        if(padIndex < 0 || padIndex >= (int)P2GamePadMax()) {
            return;
        }
        _ButtonMap &m = _buttonMap();
        zfiter it = m.iterFind(implButton);
        if(!it) {
            return;
        }
        P2GamePadImpl_notifyButtonValue((zfindex)padIndex, m.iterValue(it), value);
    }
    static void _notifyButtonValue(ZF_IN SDL_JoystickID implId, ZF_IN SDL_GamepadAxis implButton, ZF_IN zffloat value) {
        int padIndex = SDL_GetGamepadPlayerIndexForID(implId);
        if(padIndex < 0 || padIndex >= (int)P2GamePadMax()) {
            return;
        }
        _AxisMap &m = _axisMap();
        zfiter it = m.iterFind(implButton);
        if(!it) {
            return;
        }
        P2GamePadImpl_notifyButtonValue((zfindex)padIndex, m.iterValue(it), value);
    }
ZFPROTOCOL_IMPLEMENTATION_END(P2GamePadImpl_sys_SDL)

ZFIMPL_SYS_SDL_EVENT_HANDLER(P2GamePadImplAttach, SDL_EVENT_GAMEPAD_ADDED, ZFLevelZFFrameworkNormal) {
    if(ZFPROTOCOL_IMPLEMENTATION_CLASS(P2GamePadImpl_sys_SDL)::implAttached()) {
        ZFPROTOCOL_IMPLEMENTATION_CLASS(P2GamePadImpl_sys_SDL)::padAttach(sdlEvent->gdevice.which);
    }
    return zftrue;
}
ZFIMPL_SYS_SDL_EVENT_HANDLER(P2GamePadImplDetach, SDL_EVENT_GAMEPAD_REMOVED, ZFLevelZFFrameworkNormal) {
    if(ZFPROTOCOL_IMPLEMENTATION_CLASS(P2GamePadImpl_sys_SDL)::implAttached()) {
        ZFPROTOCOL_IMPLEMENTATION_CLASS(P2GamePadImpl_sys_SDL)::padDetach(sdlEvent->gdevice.which);
    }
    return zftrue;
}
ZFIMPL_SYS_SDL_EVENT_HANDLER(P2GamePadImplButtonDown, SDL_EVENT_GAMEPAD_BUTTON_DOWN, ZFLevelZFFrameworkNormal) {
    if(ZFPROTOCOL_IMPLEMENTATION_CLASS(P2GamePadImpl_sys_SDL)::implAttached()) {
        ZFPROTOCOL_IMPLEMENTATION_CLASS(P2GamePadImpl_sys_SDL)::buttonDown(sdlEvent->gbutton.which, (SDL_GamepadButton)sdlEvent->gbutton.button);
    }
    return zftrue;
}
ZFIMPL_SYS_SDL_EVENT_HANDLER(P2GamePadImplButtonUp, SDL_EVENT_GAMEPAD_BUTTON_UP, ZFLevelZFFrameworkNormal) {
    if(ZFPROTOCOL_IMPLEMENTATION_CLASS(P2GamePadImpl_sys_SDL)::implAttached()) {
        ZFPROTOCOL_IMPLEMENTATION_CLASS(P2GamePadImpl_sys_SDL)::buttonUp(sdlEvent->gbutton.which, (SDL_GamepadButton)sdlEvent->gbutton.button);
    }
    return zftrue;
}
ZFIMPL_SYS_SDL_EVENT_HANDLER(P2GamePadImplAxisMove, SDL_EVENT_GAMEPAD_AXIS_MOTION, ZFLevelZFFrameworkNormal) {
    if(ZFPROTOCOL_IMPLEMENTATION_CLASS(P2GamePadImpl_sys_SDL)::implAttached()) {
        ZFPROTOCOL_IMPLEMENTATION_CLASS(P2GamePadImpl_sys_SDL)::axisMove(
                sdlEvent->gaxis.which
                , (SDL_GamepadAxis)sdlEvent->gaxis.axis
                , sdlEvent->gaxis.value
                );
    }
    return zftrue;
}

ZF_NAMESPACE_GLOBAL_END
#endif // #if ZF_ENV_sys_SDL

