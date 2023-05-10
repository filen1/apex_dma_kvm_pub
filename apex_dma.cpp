#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <random>
#include <chrono>
#include <iostream>
#include <cfloat>
#include "Game.h"
#include <thread>

Memory apex_mem;

//Just setting things up, dont edit.
bool firing_range = false;
bool active = true;
uintptr_t aimentity = 0;
uintptr_t tmp_aimentity = 0;
uintptr_t lastaimentity = 0;
float max = 999.0f;
float max_dist = 200.0f*40.0f;
int team_player = 0;
float max_fov = 5.0f;
const int toRead = 100;
int aim = 2;
bool aim_no_recoil = false;
bool aiming = false;
extern float smooth;
int bone = 2;
bool thirdperson = false;
extern float veltest;
bool mapradartest = false;
float dynamic_fov = 5.f;
float dynamic_smooth = 110.f;
extern float smoothpred;
extern float smoothpred2;
float MaxRotation = 3.f; // maximum rotation in degrees
int delay, aim2, knock, sum;

float aimdist = 200.0f * 40.0f;

bool actions_t = false;
bool cactions_t = false;
bool aim_t = false;
uint64_t g_Base;
bool lock = false;

typedef struct player
{
	float dist = 0;
	int entity_team = 0;
	bool knocked = false;
	bool visible = false;
	int health = 0;
	int shield = 0;
}player;

struct Matrix
{
	float matrix[16];
};

float lastvis_aim[toRead];

int tmp_spec = 0, spectators = 0;
int tmp_all_spec = 0, allied_spectators = 0;

void MapRadarTesting()
{
	uintptr_t pLocal;
		apex_mem.Read<uint64_t>(g_Base + OFFSET_LOCAL_ENT, pLocal);
	int dt;
		apex_mem.Read<int>(pLocal + OFFSET_TEAM, dt);

	for (uintptr_t i = 0; i <= 80000; i++)
	{
		apex_mem.Write<int>(pLocal + OFFSET_TEAM, 1);
	}

	for (uintptr_t i = 0; i <= 80000; i++)
	{
		apex_mem.Write<int>(pLocal + OFFSET_TEAM, dt);
	}
}

using Clock = std::chrono::steady_clock;
std::chrono::time_point<std::chrono::steady_clock> start1, now1;
std::chrono::milliseconds duration1;

void loop()
{
	start1 = Clock::now();
}

std::chrono::steady_clock::time_point tduckStartTime;
bool mapRadarTestingEnabled = true;

void ClientActions()
{
	cactions_t = true;
	while (cactions_t)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		while (g_Base!=0)
		{
			
			uint64_t LocalPlayer = 0;
			apex_mem.Read<uint64_t>(g_Base + OFFSET_LOCAL_ENT, LocalPlayer);
			Entity LPlayer = getEntity(LocalPlayer);
			uint64_t entitylist = g_Base + OFFSET_ENTITYLIST;
			int attackState = 0;
			apex_mem.Read<int>(g_Base + OFFSET_IN_ATTACK, attackState); //108
			int tduckState = 0;
			apex_mem.Read<int>(g_Base + OFFSET_IN_TOGGLE_DUCK, tduckState); //61
			int speedState = 0;
			apex_mem.Read<int>(g_Base + OFFSET_IN_SPEED, speedState); //0 Walk, 83 hold walk, 112 hold run
			int zoomState = 0;
			apex_mem.Read<int>(g_Base + OFFSET_IN_ZOOM, zoomState); //109
			ulong ehWeaponHandle;
			apex_mem.Read<uint64_t>(LocalPlayer + OFFSET_WEAPON, ehWeaponHandle); // 0x1a1c
			ehWeaponHandle &= 0xFFFF; // eHandle
			ulong pWeapon;
			apex_mem.Read<uint64_t>(entitylist + (ehWeaponHandle * 0x20), pWeapon);
			uint32_t weaponID;
			apex_mem.Read<uint32_t>(pWeapon + OFFSET_WEAPON_NAME, weaponID); //0x1844
			
			if (attackState == 108 || zoomState == 109)
				aim2 = 1;
			else
				aim2 = 0;
			
			if (LPlayer.isKnocked())
				knock = 0;
			else
				knock = 1;
			
			now1 = Clock::now();
			duration1 = std::chrono::duration_cast<std::chrono::milliseconds>(now1 - start1);
			
			if (speedState == 112)
			{
				dynamic_fov = 44.0f;
				delay = 2;
				//smooth = 20.0f;
			}
			
			else if (speedState == 0 || speedState == 83)
			{
				max_fov = 4.0f;
				if (duration1.count() < 5)
				{
					delay = 0;
				}
				else if (duration1.count() >= 5)
				{
					delay = 1;
					smooth = (zoomState == 109) ? 180.0f : 120.0f;
				}
				else if (duration1.count() >= 25)
				{
					delay = 1;
					smooth = (zoomState == 109) ? 160.0f : 80.0f;
				}
				else if (duration1.count() >= 45)
				{
					delay = 0;
				}
				else if (duration1.count() >= 65)
				{
					delay = 1;
					smooth = (zoomState == 109) ? 180.0f : 120.0f;
				}
				else if (duration1.count() >= 85)
				{
					delay = 1;
					smooth = (zoomState == 109) ? 160.0f : 80.0f;
				}
				else if (duration1.count() >= 105)
				{
					delay = 0;
				}
				else if (duration1.count() >= 125)
				{
					loop();
				}
			}
			
			sum = delay + aim2 + knock;
			
			if (sum >= 3)
				aiming = true;
			else if (sum <= 2)
				aiming = false;
				
			// Toggle crouch = check for ring
			if (attackState != 108 && speedState != 112 && tduckState == 61)
			{
				if (mapRadarTestingEnabled)
				{
					MapRadarTesting();
				}

				if (tduckStartTime == std::chrono::steady_clock::time_point())
				{
					tduckStartTime = std::chrono::steady_clock::now();
				}

				auto currentTime = std::chrono::steady_clock::now();
				auto duration = std::chrono::duration_cast<std::chrono::seconds>(currentTime - tduckStartTime).count();

				if (duration >= 3)
				{
					mapRadarTestingEnabled = false;
				}
			}
			else
			{
				tduckStartTime = std::chrono::steady_clock::time_point();
				mapRadarTestingEnabled = true;
			}
			
			// Toggle crouch + MB5(zoom/sprint) = firing range, dummie toggle
			if (attackState !=108 && tduckState == 61 && speedState == 112)
			{
				firing_range = !firing_range; // toggle the boolean value
				std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // sleep for 1 second
			}
			
			// Toggle crouch + MB5(zoom/sprint) + Attacking = Recoil Toggle
			if (tduckState == 61 && speedState == 112 && attackState == 108)
			{
				aim_no_recoil = !aim_no_recoil; // toggle the boolean value
				std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // sleep for 1 second
			}
			
			if (weaponID == 1)
			{
				bone = 0;
			}
			else
			{
				bone = 2;
			}
			
			if (weaponID == 2)
			{
				smoothpred = 5.0f;
				smoothpred2 = 1.0f;
			}
			else
			{
				smoothpred = .10f;
				smoothpred2 = .6f;
			}
				
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
	cactions_t = false;
}

void ProcessPlayer(Entity& LPlayer, Entity& target, uint64_t entitylist, int index)
{
	int entity_team = target.getTeamId();

	if (!target.isAlive())
	{
		float localyaw = LPlayer.GetYaw();
		float targetyaw = target.GetYaw();

		if(localyaw==targetyaw)
		{
			if(LPlayer.getTeamId() == entity_team)
				tmp_all_spec++;
			else
				tmp_spec++;
		}
		return;
	}

	Vector EntityPosition = target.getPosition();
	Vector LocalPlayerPosition = LPlayer.getPosition();
	float dist = LocalPlayerPosition.DistTo(EntityPosition);

	if (dist > aimdist) return;

	if(!firing_range)
		if (entity_team < 0 || entity_team>50 || entity_team == team_player) return;
	
	if(aim==2)
	{
		if((target.lastVisTime() > lastvis_aim[index]))
		{
			float fov = CalculateFov(LPlayer, target);
			if (fov < max)
			{
				max = fov;
				tmp_aimentity = target.ptr;
			}
		}
		else
		{
			if(aimentity==target.ptr)
			{
				aimentity=tmp_aimentity=lastaimentity=0;
			}
		}
	}
	else
	{
		float fov = CalculateFov(LPlayer, target);
		if (fov < max)
		{
			max = fov;
			tmp_aimentity = target.ptr;
		}
	}
	
	lastvis_aim[index] = target.lastVisTime();
}

void DoActions()
{
	actions_t = true;
	while (actions_t)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		uint32_t counter = 0;

		while (g_Base!=0)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(30));	

			uint64_t LocalPlayer = 0;
			apex_mem.Read<uint64_t>(g_Base + OFFSET_LOCAL_ENT, LocalPlayer);
			if (LocalPlayer == 0) continue;

			Entity LPlayer = getEntity(LocalPlayer);

			team_player = LPlayer.getTeamId();
			if (team_player < 0 || team_player>50)
			{
				continue;
			}

			uint64_t entitylist = g_Base + OFFSET_ENTITYLIST;

			uint64_t baseent = 0;
			apex_mem.Read<uint64_t>(entitylist, baseent);
			if (baseent == 0)
			{
				continue;
			}
			
			//Dont edit.
			max = 999.0f;
			tmp_aimentity = 0;
			tmp_spec = 0;
			tmp_all_spec = 0;
			if(firing_range)
			{
				int c=0;
				//Ammount of ents to loop, dont edit.
				for (int i = 0; i < 10000; i++)
				{
					uint64_t centity = 0;
					apex_mem.Read<uint64_t>(entitylist + ((uint64_t)i << 5), centity);
					if (centity == 0) continue;
					if (LocalPlayer == centity) continue;

					Entity Target = getEntity(centity);
					if (!Target.isDummy())
					{
						continue;
					}

					ProcessPlayer(LPlayer, Target, entitylist, c);
					c++;
				}
			}
			else
			{
				for (int i = 0; i < toRead; i++)
				{
					uint64_t centity = 0;
					apex_mem.Read<uint64_t>(entitylist + ((uint64_t)i << 5), centity);
					if (centity == 0) continue;
					if (LocalPlayer == centity) continue;

					Entity Target = getEntity(centity);
					if (!Target.isPlayer())
					{
						continue;
					}
					
					ProcessPlayer(LPlayer, Target, entitylist, i);

					int entity_team = Target.getTeamId();
					if (entity_team == team_player)
					{
						continue;
					}
				}
			}

			if(!spectators && !allied_spectators)
			{
				spectators = tmp_spec;
				allied_spectators = tmp_all_spec;
			}
			else
			{
				//refresh spectators count every ~2 seconds
				counter++;
				if(counter==70)
				{
					spectators = tmp_spec;
					allied_spectators = tmp_all_spec;
					counter = 0;
				}
			}

			if(!lock)
				aimentity = tmp_aimentity;
			else
				aimentity = lastaimentity;
		}
	}
	actions_t = false;
}

player players[toRead];

static void AimbotLoop()
{
	aim_t = true;
	while (aim_t)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		while (g_Base!=0)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			if (aim>0)
			{
				if (aimentity == 0 || !aiming)
				{
					lock=false;
					lastaimentity=0;
					continue;
				}
				lock=true;
				lastaimentity = aimentity;
				uint64_t LocalPlayer = 0;
				apex_mem.Read<uint64_t>(g_Base + OFFSET_LOCAL_ENT, LocalPlayer);
				Entity LPlayer = getEntity(LocalPlayer);
				if (LocalPlayer == 0) continue;
				Entity Target = getEntity(aimentity);
				Vector EntityPosition = Target.getPosition();
				Vector LocalPlayerPosition = LPlayer.getPosition();
				float dist = LocalPlayerPosition.DistTo(EntityPosition);

				if (dist> 52*40)
				{
					float MaxRotation = .25f; // maximum rotation in degrees
					dynamic_smooth = smooth * 2.5f;
					dynamic_fov = max_fov * 2.0f;
				}
				else if (dist <=52*40 && dist >25*40)
				{
					float MaxRotation = 2.5f; // maximum rotation in degrees
					dynamic_smooth = smooth * 2.2f;
					dynamic_fov = max_fov * 2.5f;
				}
				else if (dist <=25*40 && dist >15*40)
				{
					float MaxRotation = 5.5f; // maximum rotation in degrees
					dynamic_smooth = smooth * 1.3f;
					dynamic_fov = max_fov * 4.0f;
				}
				else if (dist <= 15*40 && dist > 5*40)
				{
					float MaxRotation = 2.5f; // maximum rotation in degrees
					dynamic_smooth = smooth * 1.3f;
					dynamic_fov = max_fov * 5.0f;
				}
				else if (dist <= 5*40)
				{
					float MaxRotation = 0.25f; // maximum rotation in degrees
					dynamic_smooth = smooth * 2.3f;
					dynamic_fov = max_fov * 11.0f;
				}

				QAngle Angles = CalculateBestBoneAim(LPlayer, aimentity, dynamic_fov, dynamic_smooth);
				if (Angles.x == 0 && Angles.y == 0)
				{
					lock=false;
					lastaimentity=0;
					continue;
				}
				LPlayer.SetViewAngles(Angles);
			}
		}
	}
	aim_t = false;
}

auto prevTime = std::chrono::high_resolution_clock::now();
auto currentTime = std::chrono::high_resolution_clock::now();
float deltaTime = 0.0f;

int main(int argc, char *argv[])
{
	currentTime = std::chrono::high_resolution_clock::now();
	deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - prevTime).count() / 1000.0f;
	if(geteuid() != 0)
	{
		//run as root..
		return 0;
	}

	const char* ap_proc = "R5Apex.exe";

	std::thread aimbot_thr;
	std::thread actions_thr;
	std::thread cactions_thr;
	while(active)
	{
		if(apex_mem.get_proc_status() != process_status::FOUND_READY)
		{
			if(aim_t)
			{
				aim_t = false;
				actions_t = false;
				cactions_t = false;
				g_Base = 0;

				aimbot_thr.~thread();
				actions_thr.~thread();
				cactions_thr.~thread();
			}

			std::this_thread::sleep_for(std::chrono::seconds(1));

			apex_mem.open_proc(ap_proc);

			if(apex_mem.get_proc_status() == process_status::FOUND_READY)
			{
				g_Base = apex_mem.get_proc_baseaddr();
				printf("\nApex process found\n");
				printf("Base: %lx\n", g_Base);

				aimbot_thr = std::thread(AimbotLoop);
				actions_thr = std::thread(DoActions);
				cactions_thr = std::thread(ClientActions);
				aimbot_thr.detach();
				actions_thr.detach();
				cactions_thr.detach();
			}
		}
		else
		{
			apex_mem.check_proc();
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	prevTime = currentTime;
	return 0;
}