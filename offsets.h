
#define VERSION STEAM
//GameVersion=v3.0.31.53

#if VERSION == STEAM

#define OFFSET_ENTITYLIST 0x1d0fe58 //cl_entitylist
#define OFFSET_LOCAL_ENT 0x020c1130 + 0x8 //.?AVC_GameMovement@@

#define OFFSET_TEAM 0x044c //m_iTeamNum
#define OFFSET_NAME 0x0589 //m_iName

#define OFFSET_ABS_VELOCITY 0x0140 //m_vecAbsVelocity
#define OFFSET_VISIBLE_TIME 0x1A78 //CPlayer!lastVisibleTime
#define OFFSET_ZOOMING 0x1c51 //m_bZooming
#define OFFSET_YAW 0x22b4 - 0x8 //m_currentFramePlayer.m_ammoPoolCount - 0x8

#define OFFSET_LIFE_STATE 0x0798 //m_lifeState, >0 = dead
#define OFFSET_BLEED_OUT_STATE 0x2740 //m_bleedoutState, >0 = knocked

#define OFFSET_ORIGIN 0x014c //m_vecAbsOrigin - 3rd offset after the first int and vector

#define OFFSET_BONES 0x0e98 + 0x48 //m_nForceBone + 0x48
#define OFFSET_STUDIOHDR 0x10f0 //CBaseAnimating!m_pStudioHdr

#define OFFSET_AIMPUNCH 0x24b0 //m_currentFrameLocalPlayer.m_vecPunchWeapon_Angle, first one
#define OFFSET_CAMERAPOS 0x1f48 //CPlayer!camera_origin
#define OFFSET_VIEWANGLES 0x25ac - 0x14 //m_ammoPoolCapacity - 0x   14
#define OFFSET_BREATH_ANGLES OFFSET_VIEWANGLES - 0x10
#define OFFSET_OBSERVER_MODE 0x34d4 //m_iObserverMode
#define OFFSET_OBSERVING_TARGET 0x34e0 //m_hObserverTarget

#define OFFSET_MATRIX 0x11a210 //ViewMatrix
#define OFFSET_RENDER 0x786f390 //ViewRender

#define OFFSET_IN_SPEED 0x07871c18 //in_speed
#define OFFSET_IN_ATTACK 0x07871c50 //in_attack
#define OFFSET_IN_TOGGLE_DUCK 0x0c3fe350//in_toggle_duck
#define OFFSET_IN_ZOOM 0x0bfff7e0 //in_zoom=
#define OFFSET_WEAPON 0x1a1c //m_latestPrimaryWeapons
#define OFFSET_WEAPON_NAME 0x1874 //m_weaponNameIndex 2nd one
#define OFFSET_BULLET_SPEED 0x1F48 //CWeaponX!m_flProjectileSpeed
#define OFFSET_BULLET_SCALE  0x1F50 //CWeaponX!m_flProjectileScale
#define OFFSET_ZOOM_FOV 0x16c0 + 0xb8 //m_playerData + m_curZoomFOV
#define OFFSET_AMMO 0x1644 //m_ammoInClip first offset 

//trigger
#define CROSSHAIRSTART				0x1a74 // 	crosshairTargetStartTime
#define CROSSHAIR_TARGET_TIME				0x1a78 // 	lastCrosshairTargetTime
#define OFFSET_GAMEMODE 			0x020fa0b0 //mp_gamemode

#endif