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
#include <chrono>

//this is a test, with seconds.
Memory apex_mem;
Memory client_mem;

//Just setting things up, dont edit.
bool firing_range = false;
bool active = true;
uintptr_t aimentity = 0;
uintptr_t tmp_aimentity = 0;
uintptr_t lastaimentity = 0;
float max = 999.0f;
float max_dist = 200.0f*40.0f;
int team_player = 0;
float max_fov = 50;
const int toRead = 100;
int aim = true;
bool esp = true;
bool item_glow = true;
bool player_glow = true;
extern bool aim_no_recoil;
bool aiming = false;
extern float smooth;
extern int bone;
bool thirdperson = false;
float smoothpred = 0.08;
float smoothpred2 = 0.05;
float veltest = 1.00;
bool mapradartest = false;
int localknock = 0;
float dynamic_fov = 5.f;
float dynamic_smooth = 110.f;
int headshot_on = 1;
//headshot mode
int snipereq = 0;
int bowheadshotmode = 0;
//TDM Toggle
bool TDMToggle = false;

//chargerifle hack, removed but not all the way, dont edit.
bool chargerifle = false;
bool shooting = false;


//Player Glow Color and Brightness. Just setting things up, dont edit.
float glowr = 0.0f; //Red 0-255, higher is brighter color.
float glowg = 120.0f; //Green 0-255, higher is brighter color.
float glowb = 120.0f; //Blue 0-255, higher is brighter color.
//visable
float glowrviz = 0.0f; //Red 0-255, higher is brighter color.
float glowgviz = 120.0f; //Green 0-255, higher is brighter color.
float glowbviz = 120.0f; //Blue 0-255, higher is brighter color.
//knocked
float glowrknocked = 0.0f; //Red 0-255, higher is brighter color.
float glowgknocked = 120.0f; //Green 0-255, higher is brighter color.
float glowbknocked = 120.0f; //Blue 0-255, higher is brighter color.


//Removed but not all the way, dont edit.
int glowtype = 1;
int glowtype2 = 2;


//Ha think i was done ?
//Backpacks
bool lightbackpack = false;
bool medbackpack = false;
bool heavybackpack = false;


//Shield upgrades
bool bodyshield = false;
bool helmet = false;
bool knockdownshield = false;


//heaing and Misc
bool accelerant = false;
bool phoenix = false;
bool healthlarge = false;
bool healthsmall = false;
bool shieldsmall = false;
bool shieldlarge = false;


//Ammo
bool ammosniper = false;
bool ammoheavy = false;
bool ammolight = false;
bool ammoenergy = false;
bool ammoshotgun = false;


//Optics
bool optic = false;
bool optic2x = false;
bool opticholo1x = false;
bool opticholo1x2x = false;
bool opticthreat = false;
bool optic3x = false;
bool optic2x4x = false;
bool opticsniper6x = false;
bool opticsniper4x8x = false;
bool opticsniperthreat = false;


//Magazines
bool magsniper = false;
bool magenergy = false;
bool maglight = false;
bool magheavy = false;


//Attachments 
bool lasersight = false;
bool stocksniper = false;
bool stockregular = false;
bool suppressor = false;
bool hopup = false;
bool shotgunbolt = false;


//Nades
bool grenade_frag = false;
bool grenade_arc_star = false;
bool grenade_thermite = false;


//Kraber
bool weapon_kraber = false;


//Shotguns
bool weapon_mastiff = false;
bool weapon_eva8  = false;
bool weapon_peacekeeper  = false;
bool weapon_mozambique  = false;


//Energy weapons
bool weapon_lstar = false;
bool weapon_nemesis = false;
bool weapon_havoc = false;
bool weapon_devotion = false;
bool weapon_triple_take = false;
bool weapon_prowler  = false;
bool weapon_volt  = false;


//Heavy Weapons
bool weapon_flatline = false;
bool weapon_hemlock  = false;
bool weapon_3030_repeater = false; 
bool weapon_car_smg  = false;
bool weapon_rampage_lmg = false;


//Light weapons
bool weapon_p2020  = false;
bool weapon_re45  = false;
bool weapon_g7_scout  = false;
bool weapon_alternator  = false;
bool weapon_r99  = false;
bool weapon_spitfire  = false;
bool weapon_r301 = false;


//Snipers.. wingman is the odd one...and the bow..
bool weapon_wingman  = false;
bool weapon_longbow  = false;
bool weapon_charge_rifle  = false;
bool weapon_sentinel  = false;
bool weapon_bow  = false;


//aim dist check. Just setting things up, dont edit.
float aimdist = 200.0f * 40.0f;


//item glow brightness. Just setting things up, dont edit.
int itemglowbrightness = 10;


//Just setting things up, dont edit.
bool actions_t = false;
bool esp_t = false;
bool aim_t = false;
bool vars_t = false;
bool item_t = false;
bool headshot_t = false;
uint64_t g_Base;
uint64_t c_Base;
bool next2 = false;
bool valid = false;
bool lock = false;



//Player Definitions, dont edit unless you know what you are doing.
typedef struct player
{
	float dist = 0;
	int entity_team = 0;
	float boxMiddle = 0;
	float h_y = 0;
	float width = 0;
	float height = 0;
	float b_x = 0;
	float b_y = 0;
	bool knocked = false;
	bool visible = false;
	int health = 0;
	int shield = 0;
	int maxshield = 0;
	int armortype = 0;
	Vector EntityPosition;
	Vector LocalPlayerPosition;
	QAngle localviewangle;
	float targetyaw = 0;
	char name[33] = { 0 };
}player;


//Your in the matrix neo.
struct Matrix
{
	float matrix[16];
};


//Visual check and aim check.?
float lastvis_esp[toRead];
float lastvis_aim[toRead];


//Specator stuff. Just setting things up, dont edit.
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
//////////////////////////////////////////////////////////////////////////////////////////////////

void SetPlayerGlow(Entity& LPlayer, Entity& Target, int index)
{
	if (player_glow >= 1)
	{
		
			
			if (!Target.isGlowing() || (int)Target.buffer[OFFSET_GLOW_THROUGH_WALLS_GLOW_VISIBLE_TYPE] != 1) {
				float currentEntityTime = 5000.f;
				if (!isnan(currentEntityTime) && currentEntityTime > 0.f) {
					GColor color;
					if (!(firing_range) && (Target.isKnocked() || !Target.isAlive()))
					{
						color = { glowrknocked, glowgknocked, glowbknocked };
					}
					else if (Target.lastVisTime() > lastvis_aim[index] || (Target.lastVisTime() < 0.f && lastvis_aim[index] > 0.f))
					{
						color = { glowrviz, glowgviz, glowbviz };
					}
					else 
					{
						color = { glowr, glowg, glowb };
					}

					Target.enableGlow(color);
					
					
				}
			}
		
		else if((player_glow == 0) && Target.isGlowing())
		{
			Target.disableGlow();
		}
	}
}

//TDM check?



uint64_t PlayerLocal;
int PlayerLocalTeamID;
int EntTeam;
int LocTeam;



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
	if (TDMToggle)
	{// Check if the target entity is on the same team as the local player
		//int entity_team = Target.getTeamId();
		//printf("Target Team: %i\n", entity_team);
		
		
		uint64_t PlayerLocal;
		apex_mem.Read<uint64_t>(g_Base + OFFSET_LOCAL_ENT, PlayerLocal);
		int PlayerLocalTeamID;
		apex_mem.Read<int>(PlayerLocal + OFFSET_TEAM, PlayerLocalTeamID);
		
		
		
		if (entity_team % 2) EntTeam = 1;
		else EntTeam = 2;
		if (PlayerLocalTeamID % 2) LocTeam = 1;
		else LocTeam = 2;
		
		//printf("Target Team: %i\nLocal Team: %i\n", EntTeam, LocTeam);
		if (EntTeam == LocTeam)
			return;

	}

	Vector EntityPosition = target.getPosition();
	Vector LocalPlayerPosition = LPlayer.getPosition();
	float dist = LocalPlayerPosition.DistTo(EntityPosition);
	//Prints POS of localplayer for map cords for full map radar. only enable when adding a new map or fixing a old one, will output to console.
	//std::printf("  X: %.6f   ||    Y:%.6f",LocalPlayerPosition.x, LocalPlayerPosition.y); //Prints x and y cords of localplayer to get mainmap radar stuff.
	if (dist > aimdist) return;
	
	
	//Firing range stuff
	if(!firing_range)
		if (entity_team < 0 || entity_team>50 || entity_team == team_player) return;
	
	
	//Vis check aiming? dunno
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
	
	SetPlayerGlow(LPlayer, target, index);
	lastvis_aim[index] = target.lastVisTime();
}


//Main stuff, dont edit.
void DoActions()
{
	actions_t = true;
	while (actions_t)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		bool tmp_thirdperson = false;
		bool tmp_chargerifle = false;
		uint32_t counter = 0;

		while (g_Base!=0 && c_Base!=0)
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

			if(thirdperson && !tmp_thirdperson)
			{
				if(!aiming)
				{
					apex_mem.Write<int>(g_Base + OFFSET_THIRDPERSON, 1);
					apex_mem.Write<int>(LPlayer.ptr + OFFSET_THIRDPERSON_SV, 1);
					tmp_thirdperson = true;
				}			
			}
			else if((!thirdperson && tmp_thirdperson) || aiming)
			{
				if(tmp_thirdperson)
				{
					apex_mem.Write<int>(g_Base + OFFSET_THIRDPERSON, -1);
					apex_mem.Write<int>(LPlayer.ptr + OFFSET_THIRDPERSON_SV, 0);
					tmp_thirdperson = false;
				}	
			}

			uint64_t entitylist = g_Base + OFFSET_ENTITYLIST;

			uint64_t baseent = 0;
			apex_mem.Read<uint64_t>(entitylist, baseent);
			if (baseent == 0)
			{
				continue;
			}

			//New Radar test
			if (mapradartest)
			{
				MapRadarTesting();
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
			
			if(chargerifle)
			{
				charge_rifle_hack(LocalPlayer);
				tmp_chargerifle = true;
			}
			else
			{
				if(tmp_chargerifle)
				{
					apex_mem.Write<float>(g_Base + OFFSET_TIMESCALE + 0x68, 1.f);
					tmp_chargerifle = false;
				}
			}
		}
	}
	actions_t = false;
}

// /////////////////////////////////////////////////////////////////////////////////////////////////////

player players[toRead];

Vector LocalPOS;

//ESP loop.. this helps right?
static void EspLoop()
{
	esp_t = true;
	while(esp_t)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		while(g_Base!=0 && c_Base!=0)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			if (esp)
			{
				valid = false;

				uint64_t LocalPlayer = 0;
				apex_mem.Read<uint64_t>(g_Base + OFFSET_LOCAL_ENT, LocalPlayer);
				if (LocalPlayer == 0)
				{
					next2 = true;
					while(next2 && g_Base!=0 && c_Base!=0 && esp)
					{
						std::this_thread::sleep_for(std::chrono::milliseconds(1));
					}
					continue;
				}
				Entity LPlayer = getEntity(LocalPlayer);
				
				int team_player = LPlayer.getTeamId();
				if (team_player < 0 || team_player>50)
				{
					next2 = true;
					while(next2 && g_Base!=0 && c_Base!=0 && esp)
					{
						std::this_thread::sleep_for(std::chrono::milliseconds(1));
					}
					continue;
				}
				Vector LocalPlayerPosition = LPlayer.getPosition();
				LocalPOS = LPlayer.getAbsVelocity();

				uint64_t viewRenderer = 0;
				apex_mem.Read<uint64_t>(g_Base + OFFSET_RENDER, viewRenderer);
				uint64_t viewMatrix = 0;
				apex_mem.Read<uint64_t>(viewRenderer + OFFSET_MATRIX, viewMatrix);
				Matrix m = {};
				apex_mem.Read<Matrix>(viewMatrix, m);

				uint64_t entitylist = g_Base + OFFSET_ENTITYLIST;
				
				memset(players,0,sizeof(players));

				if(firing_range)
				{
					int c=0;
					//Ammount of ents to loop, dont edit.
					for (int i = 0; i < 10000; i++)
					{
						uint64_t centity = 0;
						apex_mem.Read<uint64_t>( entitylist + ((uint64_t)i << 5), centity);
						if (centity == 0)
						{
							continue;
						}		
						
						if (LocalPlayer == centity)
						{
							continue;
						}

						Entity Target = getEntity(centity);

						if (!Target.isDummy())
						{
							continue;
						}

						if (!Target.isAlive())
						{
							continue;
						}
						int entity_team = Target.getTeamId();

						Vector EntityPosition = Target.getPosition();
						float dist = LocalPlayerPosition.DistTo(EntityPosition);

						if (dist > max_dist || dist < 50.0f)
						{	
							continue;
						}
						
						Vector bs = Vector();
						//Change res to your res here, default is 1080p but can copy paste 1440p here
						WorldToScreen(EntityPosition, m.matrix, 1920, 1080, bs); //2560, 1440
						if (esp)
						{
							Vector hs = Vector();
							Vector HeadPosition = Target.getBonePositionByHitbox(0);
							//Change res to your res here, default is 1080p but can copy paste 1440p here
							WorldToScreen(HeadPosition, m.matrix, 1920, 1080, hs); //2560, 1440
							float height = abs(abs(hs.y) - abs(bs.y));
							float width = height / 2.0f;
							float boxMiddle = bs.x - (width / 2.0f);
							int health = Target.getHealth();
							int shield = Target.getShield();
							int maxshield = Target.getMaxshield();
							int armortype = Target.getArmortype();
							players[c] = 
							{
								dist,
								entity_team,
								boxMiddle,
								hs.y,
								width,
								height,
								bs.x,
								bs.y,
								0,
								(Target.lastVisTime() > lastvis_esp[c]),
								health,
								shield,
								maxshield,
								armortype
								
							};
							Target.get_name(g_Base, i-1, &players[c].name[0]);
							lastvis_esp[c] = Target.lastVisTime();
							valid = true;
							c++;
						}
					}
				}	
				else
				{
					for (int i = 0; i < toRead; i++)
					{
						uint64_t centity = 0;
						apex_mem.Read<uint64_t>( entitylist + ((uint64_t)i << 5), centity);
						if (centity == 0)
						{
							continue;
						}
						
						if (LocalPlayer == centity)
						{
							continue;
						}

						Entity Target = getEntity(centity);
						
						if (!Target.isPlayer())
						{
							continue;
						}

						if (!Target.isAlive())
						{
							continue;
						}

						int entity_team = Target.getTeamId();
						if (entity_team < 0 || entity_team>50 || entity_team == team_player)
						{
							continue;
						}

						Vector EntityPosition = Target.getPosition();
						float dist = LocalPlayerPosition.DistTo(EntityPosition);
						if (dist > max_dist || dist < 50.0f)
						{	
							continue;
						}
						
						
						
						
						
						Vector bs = Vector();
						//Change res to your res here, default is 1080p but can copy paste 1440p here
						WorldToScreen(EntityPosition, m.matrix, 1920, 1080, bs); //2560, 1440
						if (esp)
						{
							Vector hs = Vector();
							Vector HeadPosition = Target.getBonePositionByHitbox(0);
							//Change res to your res here, default is 1080p but can copy paste 1440p here
							WorldToScreen(HeadPosition, m.matrix, 1920, 1080, hs); //2560, 1440
							float height = abs(abs(hs.y) - abs(bs.y));
							float width = height / 2.0f;
							float boxMiddle = bs.x - (width / 2.0f);
							int health = Target.getHealth();
							int shield = Target.getShield();
							int maxshield = Target.getMaxshield();
							int armortype = Target.getArmortype();
							Vector EntityPosition = Target.getPosition();
							Vector LocalPlayerPosition = LPlayer.getPosition();
							QAngle localviewangle = LPlayer.GetViewAngles();
							float targetyaw = Target.GetYaw();
							players[i] = 
							{
								dist,
								entity_team,
								boxMiddle,
								hs.y,
								width,
								height,
								bs.x,
								bs.y,
								Target.isKnocked(),
								(Target.lastVisTime() > lastvis_esp[i]),
								health,
								shield,
								maxshield,
								armortype,
								EntityPosition,
								LocalPlayerPosition,
								localviewangle,
								targetyaw
							};
							Target.get_name(g_Base, i-1, &players[i].name[0]);
							lastvis_esp[i] = Target.lastVisTime();
							valid = true;
						}
					}
				}

				next2 = true;
				while(next2 && g_Base!=0 && c_Base!=0 && esp)
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(1));
				}
			}
		}
	}
	esp_t = false;
	

}
//Aimbot Loop stuff
static void AimbotLoop()
{
	aim_t = true;
	while (aim_t)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		while (g_Base!=0 && c_Base!=0)
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
				if (LocalPlayer == 0) continue;
				Entity LPlayer = getEntity(LocalPlayer);
				Entity Target = getEntity(aimentity);
				Vector EntityPosition = Target.getPosition();
				Vector LocalPlayerPosition = LPlayer.getPosition();
				float dist = LocalPlayerPosition.DistTo(EntityPosition);
				if (LPlayer.isKnocked())
				{
					localknock = 0;
				}
				else
				{
					localknock = 1;
				}
				if (dist> 52*40)
				{
					dynamic_smooth = smooth * 1.0f;
					dynamic_fov = max_fov * 1.0f;
				}
				else if (dist <=52*40 && dist >25*40)
				{
					dynamic_smooth = smooth * 1.0f;
					dynamic_fov = max_fov * 1.0f;
				}
				else if (dist <=25*40 && dist >15*40)
				{
					dynamic_smooth = smooth * 1.0f;
					dynamic_fov = max_fov * 1.0f;
				}
				else if (dist <= 15*40 && dist > 5*40)
				{
					dynamic_smooth = smooth * 1.0f;
					dynamic_fov = max_fov * 1.0f;
				}
				else if (dist <= 5*40)
				{
					dynamic_smooth = smooth * 1.0f;
					dynamic_fov = max_fov * 1.0f;
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
//Client memory vars/reads. HAVE to match windows client numbers.
static void set_vars(uint64_t add_addr)
{
	printf("Reading the client vars...\n");
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
	//Get addresses of client vars
	uint64_t check_addr = 0;
	client_mem.Read<uint64_t>(add_addr, check_addr);
	uint64_t aim_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t), aim_addr);
	uint64_t esp_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*2, esp_addr);
	uint64_t aiming_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*3, aiming_addr);
	uint64_t g_Base_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*4, g_Base_addr);
	uint64_t next2_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*5, next2_addr);
	uint64_t player_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*6, player_addr);
	uint64_t valid_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*7, valid_addr);
	uint64_t max_dist_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*8, max_dist_addr);
	uint64_t item_glow_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*9, item_glow_addr);
	uint64_t player_glow_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*10, player_glow_addr);
	uint64_t aim_no_recoil_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*11, aim_no_recoil_addr);
	uint64_t smooth_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*12, smooth_addr);
	uint64_t max_fov_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*13, max_fov_addr);
	uint64_t bone_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*14, bone_addr);
	uint64_t thirdperson_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*15, thirdperson_addr);
	uint64_t spectators_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*16, spectators_addr);
	uint64_t allied_spectators_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*17, allied_spectators_addr);
	uint64_t glowr_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*18, glowr_addr);
	uint64_t glowg_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*19, glowg_addr);
	uint64_t glowb_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*20, glowb_addr);
	uint64_t firing_range_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*21, firing_range_addr);
	uint64_t lightbackpack_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*22, lightbackpack_addr);
	uint64_t medbackpack_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*23, medbackpack_addr);
	uint64_t heavybackpack_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*24, heavybackpack_addr);
	uint64_t bodyshield_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*25, bodyshield_addr);
	uint64_t helmet_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*26, helmet_addr);
	uint64_t accelerant_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*27, accelerant_addr);
	uint64_t phoenix_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*28, phoenix_addr);
	uint64_t healthlarge_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*29, healthlarge_addr);
	uint64_t healthsmall_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*30, healthsmall_addr);
	uint64_t shieldsmall_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*31, shieldsmall_addr);
	uint64_t shieldlarge_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*32, shieldlarge_addr);
	uint64_t ammosniper_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*33, ammosniper_addr);
	uint64_t ammoheavy_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*34, ammoheavy_addr);
	uint64_t optic_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*35, optic_addr);
	uint64_t ammolight_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*36, ammolight_addr);
	uint64_t ammoenergy_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*37, ammoenergy_addr);
	uint64_t ammoshotgun_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*38, ammoshotgun_addr);
	uint64_t lasersight_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*39, lasersight_addr);
	uint64_t magsniper_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*40, magsniper_addr);
	uint64_t magenergy_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*41, magenergy_addr);
	uint64_t stocksniper_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*42, stocksniper_addr);
	uint64_t stockregular_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*43, stockregular_addr);
	uint64_t knockdownshield_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*44, knockdownshield_addr);
	uint64_t maglight_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*45, maglight_addr);
	uint64_t magheavy_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*46, magheavy_addr);
	uint64_t optic2x_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*47, optic2x_addr);
	uint64_t opticholo1x_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*48, opticholo1x_addr);
	uint64_t opticholo1x2x_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*49, opticholo1x2x_addr);
	uint64_t opticthreat_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*50, opticthreat_addr);
	uint64_t optic3x_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*51, optic3x_addr);
	uint64_t optic2x4x_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*52, optic2x4x_addr);
	uint64_t opticsniper6x_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*53, opticsniper6x_addr);
	uint64_t opticsniper4x8x_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*54, opticsniper4x8x_addr);
	uint64_t opticsniperthreat_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*55, opticsniperthreat_addr);
	uint64_t suppressor_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*56, suppressor_addr);
	uint64_t hopup_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*57, hopup_addr);
	uint64_t grenade_frag_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*58, grenade_frag_addr);
	uint64_t grenade_arc_star_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*59, grenade_arc_star_addr);
	uint64_t grenade_thermite_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*60, grenade_thermite_addr);
	uint64_t shotgunbolt_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*61, shotgunbolt_addr);
	uint64_t weapon_kraber_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*62, weapon_kraber_addr);
	uint64_t weapon_mastiff_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*63, weapon_mastiff_addr);
	uint64_t weapon_lstar_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*64, weapon_lstar_addr);
	uint64_t weapon_havoc_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*65, weapon_havoc_addr);
	uint64_t weapon_devotion_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*66, weapon_devotion_addr);
	uint64_t weapon_triple_take_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*67, weapon_triple_take_addr);
	uint64_t weapon_flatline_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*68, weapon_flatline_addr);
	uint64_t weapon_hemlock_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*69, weapon_hemlock_addr);
	uint64_t weapon_g7_scout_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*70, weapon_g7_scout_addr);
	uint64_t weapon_alternator_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*71, weapon_alternator_addr);
	uint64_t weapon_r99_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*72, weapon_r99_addr);
	uint64_t weapon_prowler_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*73, weapon_prowler_addr);
	uint64_t weapon_volt_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*74, weapon_volt_addr);
	uint64_t weapon_longbow_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*75, weapon_longbow_addr);
	uint64_t weapon_charge_rifle_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*76, weapon_charge_rifle_addr);
	uint64_t weapon_spitfire_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*77, weapon_spitfire_addr);
	uint64_t weapon_r301_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*78, weapon_r301_addr);
	uint64_t weapon_eva8_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*79, weapon_eva8_addr);
	uint64_t weapon_peacekeeper_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*80, weapon_peacekeeper_addr);
	uint64_t weapon_mozambique_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*81, weapon_mozambique_addr);
	uint64_t weapon_wingman_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*82, weapon_wingman_addr);
	uint64_t weapon_p2020_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*83, weapon_p2020_addr);
	uint64_t weapon_re45_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*84, weapon_re45_addr);
	uint64_t weapon_sentinel_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*85, weapon_sentinel_addr);
	uint64_t weapon_bow_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*86, weapon_bow_addr);
	uint64_t weapon_3030_repeater_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*87, weapon_3030_repeater_addr);
	uint64_t weapon_rampage_lmg_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*88, weapon_rampage_lmg_addr);
	uint64_t weapon_car_smg_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*89, weapon_car_smg_addr);
	uint64_t aimdist_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*90, aimdist_addr);
	uint64_t itemglowbrightness_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*91, itemglowbrightness_addr);
	//glow visable
	uint64_t glowrviz_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*92, glowrviz_addr);
	uint64_t glowgviz_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*93, glowgviz_addr);
	uint64_t glowbviz_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*94, glowbviz_addr);
	//knocked
	uint64_t glowrknocked_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*95, glowrknocked_addr);
	uint64_t glowgknocked_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*96, glowgknocked_addr);
	uint64_t glowbknocked_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*97, glowbknocked_addr);
	uint64_t smoothpred_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*98, smoothpred_addr);
	uint64_t smoothpred2_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*99, smoothpred2_addr);
	//new weap nemesis
	uint64_t weapon_nemesis_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*100, weapon_nemesis_addr);
	//new map radar
	uint64_t mapradartest_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*101, mapradartest_addr);
	//headshot stuff
	uint64_t snipereq_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*102, snipereq_addr);
	uint64_t bowheadshotmode_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*103, bowheadshotmode_addr);
	uint64_t veltest_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*104, veltest_addr);
	//team check?
	//local team check
	uint64_t PlayerLocalTeamID_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*105, PlayerLocalTeamID_addr);
	uint64_t TDMToggle_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*106, TDMToggle_addr);
	//More Team check stuff
	uint64_t EntTeam_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*107, EntTeam_addr);
	uint64_t LocTeam_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*108, LocTeam_addr);
	uint64_t localknock_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*109, localknock_addr);
	/* //lock?
	uint64_t lock_addr = 0;
	client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t)*109, lock_addr); */

	

	
	
	//good god 97..make that 110... of em.. why
	

	uint32_t check = 0;
	client_mem.Read<uint32_t>(check_addr, check);
	
	if(check != 0xABCD)
	{
		//Add offset msg
		printf("Incorrect values read. Check if the add_off is correct. Quitting.\n");
		active = false;
		return;
	}
	vars_t = true;
	while(vars_t)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		if(c_Base!=0 && g_Base!=0)
		{
			client_mem.Write<uint32_t>(check_addr, 0);
			printf("\nLet the fun begin!\n");
		}

		while(c_Base!=0 && g_Base!=0)
		{
			//same as above, has to match with eveything else
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			client_mem.Write<uint64_t>(g_Base_addr, g_Base);
			client_mem.Write<int>(spectators_addr, spectators);
			client_mem.Write<int>(allied_spectators_addr, allied_spectators);
			client_mem.Read<int>(aim_addr, aim);
			client_mem.Read<bool>(esp_addr, esp);
			client_mem.Read<bool>(aiming_addr, aiming);
			client_mem.Read<float>(max_dist_addr, max_dist);
			client_mem.Read<bool>(item_glow_addr, item_glow);
			client_mem.Read<bool>(player_glow_addr, player_glow);
			client_mem.Read<bool>(aim_no_recoil_addr, aim_no_recoil);
			client_mem.Read<float>(smooth_addr, smooth);
			client_mem.Read<float>(max_fov_addr, max_fov);
			client_mem.Read<int>(bone_addr, bone);
			client_mem.Read<bool>(thirdperson_addr, thirdperson);
			client_mem.Read<float>(glowr_addr, glowr);
			client_mem.Read<float>(glowg_addr, glowg);
			client_mem.Read<float>(glowb_addr, glowb);
			client_mem.Read<bool>(firing_range_addr, firing_range);
			client_mem.Read<bool>(lightbackpack_addr, lightbackpack);
			client_mem.Read<bool>(medbackpack_addr, medbackpack);
			client_mem.Read<bool>(heavybackpack_addr, heavybackpack);
			client_mem.Read<bool>(bodyshield_addr, bodyshield);
			client_mem.Read<bool>(helmet_addr, helmet);
			client_mem.Read<bool>(accelerant_addr, accelerant);
			client_mem.Read<bool>(phoenix_addr, phoenix);
			client_mem.Read<bool>(healthlarge_addr, healthlarge);
			client_mem.Read<bool>(healthsmall_addr, healthsmall);
			client_mem.Read<bool>(shieldsmall_addr, shieldsmall);
			client_mem.Read<bool>(shieldlarge_addr, shieldlarge);
			client_mem.Read<bool>(ammosniper_addr, ammosniper);
			client_mem.Read<bool>(ammoheavy_addr, ammoheavy);
			client_mem.Read<bool>(optic_addr, optic);
			client_mem.Read<bool>(ammolight_addr, ammolight);
			client_mem.Read<bool>(ammoenergy_addr, ammoenergy);
			client_mem.Read<bool>(ammoshotgun_addr, ammoshotgun);
			client_mem.Read<bool>(lasersight_addr, lasersight);
			client_mem.Read<bool>(magsniper_addr, magsniper);
			client_mem.Read<bool>(magenergy_addr , magenergy);
			client_mem.Read<bool>(stocksniper_addr, stocksniper);
			client_mem.Read<bool>(stockregular_addr, stockregular);
			client_mem.Read<bool>(knockdownshield_addr, knockdownshield);
			client_mem.Read<bool>(maglight_addr, maglight);
			client_mem.Read<bool>(magheavy_addr, magheavy);
			client_mem.Read<bool>(optic2x_addr, optic2x);
			client_mem.Read<bool>(opticholo1x_addr, opticholo1x);
			client_mem.Read<bool>(opticholo1x2x_addr, opticholo1x2x);
			client_mem.Read<bool>(opticthreat_addr, opticthreat);
			client_mem.Read<bool>(optic3x_addr, optic3x);
			client_mem.Read<bool>(optic2x4x_addr, optic2x4x);
			client_mem.Read<bool>(opticsniper6x_addr, opticsniper6x);
			client_mem.Read<bool>(opticsniper4x8x_addr, opticsniper4x8x);
			client_mem.Read<bool>(opticsniperthreat_addr, opticsniperthreat);
			client_mem.Read<bool>(suppressor_addr, suppressor);
			client_mem.Read<bool>(hopup_addr, hopup);
			client_mem.Read<bool>(grenade_frag, grenade_frag);
			client_mem.Read<bool>(grenade_arc_star, grenade_arc_star);
			client_mem.Read<bool>(grenade_thermite_addr, grenade_thermite);
			client_mem.Read<bool>(shotgunbolt_addr, shotgunbolt);
			client_mem.Read<bool>(weapon_kraber_addr, weapon_kraber);
			client_mem.Read<bool>(weapon_mastiff_addr, weapon_mastiff);
			client_mem.Read<bool>(weapon_lstar_addr, weapon_lstar);
			client_mem.Read<bool>(weapon_havoc_addr, weapon_havoc);
			client_mem.Read<bool>(weapon_devotion_addr, weapon_devotion);
			client_mem.Read<bool>(weapon_triple_take_addr, weapon_triple_take);
			client_mem.Read<bool>(weapon_flatline_addr, weapon_flatline);
			client_mem.Read<bool>(weapon_hemlock_addr, weapon_hemlock);
			client_mem.Read<bool>(weapon_g7_scout_addr, weapon_g7_scout);
			client_mem.Read<bool>(weapon_alternator_addr, weapon_alternator);
			client_mem.Read<bool>(weapon_r99_addr, weapon_r99);
			client_mem.Read<bool>(weapon_prowler_addr, weapon_prowler);
			client_mem.Read<bool>(weapon_volt_addr, weapon_volt);
			client_mem.Read<bool>(weapon_longbow_addr, weapon_longbow);
			client_mem.Read<bool>(weapon_charge_rifle_addr, weapon_charge_rifle);
			client_mem.Read<bool>(weapon_spitfire_addr, weapon_spitfire);
			client_mem.Read<bool>(weapon_r301_addr, weapon_r301);
			client_mem.Read<bool>(weapon_eva8_addr, weapon_eva8);
			client_mem.Read<bool>(weapon_peacekeeper_addr, weapon_peacekeeper);
			client_mem.Read<bool>(weapon_mozambique_addr, weapon_mozambique);
			client_mem.Read<bool>(weapon_wingman_addr, weapon_wingman);
			client_mem.Read<bool>(weapon_p2020_addr, weapon_p2020);
			client_mem.Read<bool>(weapon_re45_addr, weapon_re45);
			client_mem.Read<bool>(weapon_sentinel_addr, weapon_sentinel);
			client_mem.Read<bool>(weapon_bow_addr, weapon_bow);
			client_mem.Read<bool>(weapon_3030_repeater_addr, weapon_3030_repeater);
			client_mem.Read<bool>(weapon_car_smg_addr, weapon_car_smg);
			client_mem.Read<float>(aimdist_addr, aimdist);
			client_mem.Read<int>(itemglowbrightness_addr, itemglowbrightness);
			//glow visable
			client_mem.Read<float>(glowrviz_addr, glowrviz);
			client_mem.Read<float>(glowgviz_addr, glowgviz);
			client_mem.Read<float>(glowbviz_addr, glowbviz);
			//knocked
			client_mem.Read<float>(glowrknocked_addr, glowrknocked);
			client_mem.Read<float>(glowgknocked_addr, glowgknocked);
			client_mem.Read<float>(glowbknocked_addr, glowbknocked);
			client_mem.Read<float>(smoothpred_addr, smoothpred);
			client_mem.Read<float>(smoothpred2_addr, smoothpred2);
			//new weapon, nemesis
			
			client_mem.Read<bool>(weapon_nemesis_addr, weapon_nemesis);
			//new map radar test
			client_mem.Read<bool>(mapradartest_addr, mapradartest);
			client_mem.Read<bool>(weapon_rampage_lmg_addr, weapon_rampage_lmg);
			//headshot stuff
			client_mem.Write<int>(snipereq_addr, snipereq);
			client_mem.Write<int>(bowheadshotmode_addr, bowheadshotmode);
			client_mem.Read<float>(veltest_addr, veltest);
			//team check?
			//local team id
			client_mem.Write<int>(PlayerLocalTeamID_addr, PlayerLocalTeamID);
			//TDM Toggle
			client_mem.Read<bool>(TDMToggle_addr, TDMToggle);
			//More TDM toggle stuff
			client_mem.Write<int>(EntTeam_addr, EntTeam);
			client_mem.Write<int>(LocTeam_addr, LocTeam);
			client_mem.Write<int>(localknock_addr, localknock);
			//client_mem.Read<bool>(lock_addr, lock);
			
			
			
		
			
	


			if(esp && next2)
			{
				if(valid)
				client_mem.WriteArray<player>(player_addr, players, toRead);
				client_mem.Write<bool>(valid_addr, valid);
				client_mem.Write<bool>(next2_addr, true); //next2

				bool next2_val = false;
				do
				{
					client_mem.Read<bool>(next2_addr, next2_val);
					std::this_thread::sleep_for(std::chrono::milliseconds(1));
				} while (next2_val && g_Base!=0 && c_Base!=0);
				
				next2 = false;
			}
		}
	}
	vars_t = false;
}

// Item Glow Stuff
//testing



//done testing





static void item_glow_t()
{
	item_t = true;
	while(item_t)
	{
		//std::this_thread::sleep_for(std::chrono::milliseconds(1));
		int k = 0;
		while(g_Base!=0 && c_Base!=0)
		{
			//std::this_thread::sleep_for(std::chrono::milliseconds(1));
			uint64_t entitylist = g_Base + OFFSET_ENTITYLIST;
			if (item_glow)
			{
				//item ENTs to loop, 10k-15k is normal. 10k might be better but will not show all the death boxes i think.
				for (int i = 0; i < 15000; i++)
				{
					uint64_t centity = 0;
					apex_mem.Read<uint64_t>(entitylist + ((uint64_t)i << 5), centity);
					if (centity == 0) continue;
					Item item = getItem(centity);
					//testing
					uint64_t LocalPlayer = 0;
					apex_mem.Read<uint64_t>(g_Base + OFFSET_LOCAL_ENT, LocalPlayer);
					/*
					// CREDITS to Rikkie https://www.unknowncheats.me/forum/members/169606.html
					// for all the weapon ids and item ids code, you are a life saver!
					//Floor Item ID's
					ulong pEntityList = g_Base + OFFSET_ENTITYLIST; // 0x1b37938
					for (int i = 100; i < 16000; i++)
					{
					
					ulong pEntity;
					apex_mem.Read<uint64_t>(pEntityList + (i * 0x20), pEntity);
					
					uint32_t itemID;
					apex_mem.Read<uint32_t>(pEntity + OFFSET_M_CUSTOMSCRIPTINT, itemID); // 0x1648
					if (itemID < 1 || itemID > 300 )
					continue;

					if (heavybackpack && itemID == 190) 
					{
						apex_mem.Write<int>(pEntity + OFFSET_GLOW_ENABLE, 1);
						apex_mem.Write<int>(pEntity + OFFSET_GLOW_THROUGH_WALLS, 1); // 1 = far, 2 = close
						apex_mem.Write<GlowMode>(pEntity + GLOW_START_TIME, { 101,101,99,90 });

						apex_mem.Write<float>(pEntity + GLOW_COLOR_R, 255 / itemglowbrightness); // r
						apex_mem.Write<float>(pEntity + GLOW_COLOR_G, 215 / itemglowbrightness); // g
						apex_mem.Write<float>(pEntity + GLOW_COLOR_B, 0 / itemglowbrightness); // b
					
					}
					if (heavybackpack && itemID == 189) 
					{
						apex_mem.Write<int>(pEntity + OFFSET_GLOW_ENABLE, 1);
						apex_mem.Write<int>(pEntity + OFFSET_GLOW_THROUGH_WALLS, 1); // 1 = far, 2 = close
						apex_mem.Write<GlowMode>(pEntity + GLOW_START_TIME, { 101,101,99,90 });

						apex_mem.Write<float>(pEntity + GLOW_COLOR_R, 160 / itemglowbrightness); // r
						apex_mem.Write<float>(pEntity + GLOW_COLOR_G, 32 / itemglowbrightness); // g
						apex_mem.Write<float>(pEntity + GLOW_COLOR_B, 240 / itemglowbrightness); // b
					
					}
					//Rare shield
					if (bodyshield && itemID == 174) 
					{
						apex_mem.Write<int>(pEntity + OFFSET_GLOW_ENABLE, 1);
						apex_mem.Write<int>(pEntity + OFFSET_GLOW_THROUGH_WALLS, 1); // 1 = far, 2 = close
						apex_mem.Write<GlowMode>(pEntity + GLOW_START_TIME, { 101,101,99,90 });

						apex_mem.Write<float>(pEntity + GLOW_COLOR_R, 0 ); // r
						apex_mem.Write<float>(pEntity + GLOW_COLOR_G, 0 ); // g
						apex_mem.Write<float>(pEntity + GLOW_COLOR_B, 255 ); // b
					}
					//Epic shield
					if (bodyshield && itemID == 175) 
					{
						apex_mem.Write<int>(pEntity + OFFSET_GLOW_ENABLE, 1);
						apex_mem.Write<int>(pEntity + OFFSET_GLOW_THROUGH_WALLS, 1); // 1 = far, 2 = close
						apex_mem.Write<GlowMode>(pEntity + GLOW_START_TIME, { 101,101,99,90 });

						apex_mem.Write<float>(pEntity + GLOW_COLOR_R, 160 / itemglowbrightness); // r
						apex_mem.Write<float>(pEntity + GLOW_COLOR_G, 32 / itemglowbrightness); // g
						apex_mem.Write<float>(pEntity + GLOW_COLOR_B, 240 / itemglowbrightness); // b
					}
					//Legend shield
					if (bodyshield && itemID == 176) 
					{
						apex_mem.Write<int>(pEntity + OFFSET_GLOW_ENABLE, 1);
						apex_mem.Write<int>(pEntity + OFFSET_GLOW_THROUGH_WALLS, 1); // 1 = far, 2 = close
						apex_mem.Write<GlowMode>(pEntity + GLOW_START_TIME, { 101,101,99,90 });

						apex_mem.Write<float>(pEntity + GLOW_COLOR_R, 255 / itemglowbrightness); // r
						apex_mem.Write<float>(pEntity + GLOW_COLOR_G, 215 / itemglowbrightness); // g
						apex_mem.Write<float>(pEntity + GLOW_COLOR_B, 0 / itemglowbrightness); // b
					}
					//HEIRLOOM shield
					if (bodyshield && itemID == 181) 
					{
						apex_mem.Write<int>(pEntity + OFFSET_GLOW_ENABLE, 1);
						apex_mem.Write<int>(pEntity + OFFSET_GLOW_THROUGH_WALLS, 1); // 1 = far, 2 = close
						apex_mem.Write<GlowMode>(pEntity + GLOW_START_TIME, { 101,101,99,90 });

						apex_mem.Write<float>(pEntity + GLOW_COLOR_R, 255); // r
						apex_mem.Write<float>(pEntity + GLOW_COLOR_G, 0); // g
						apex_mem.Write<float>(pEntity + GLOW_COLOR_B, 0); // b
					}
					if (helmet && itemID == 170)
					{
						apex_mem.Write<int>(pEntity + OFFSET_GLOW_ENABLE, 1);
						apex_mem.Write<int>(pEntity + OFFSET_GLOW_THROUGH_WALLS, 1); // 1 = far, 2 = close
						apex_mem.Write<GlowMode>(pEntity + GLOW_START_TIME, { 101,101,99,90 });

						apex_mem.Write<float>(pEntity + GLOW_COLOR_R, 0 ); // r
						apex_mem.Write<float>(pEntity + GLOW_COLOR_G, 0 ); // g
						apex_mem.Write<float>(pEntity + GLOW_COLOR_B, 255 ); // b
					}
					if (helmet && itemID == 171)
					{
						apex_mem.Write<int>(pEntity + OFFSET_GLOW_ENABLE, 1);
						apex_mem.Write<int>(pEntity + OFFSET_GLOW_THROUGH_WALLS, 1); // 1 = far, 2 = close
						apex_mem.Write<GlowMode>(pEntity + GLOW_START_TIME, { 101,101,99,90 });

						apex_mem.Write<float>(pEntity + GLOW_COLOR_R, 160 / itemglowbrightness); // r
						apex_mem.Write<float>(pEntity + GLOW_COLOR_G, 32 / itemglowbrightness); // g
						apex_mem.Write<float>(pEntity + GLOW_COLOR_B, 240 / itemglowbrightness); // b
					}
					if (helmet && itemID == 172)
					{
						apex_mem.Write<int>(pEntity + OFFSET_GLOW_ENABLE, 1);
						apex_mem.Write<int>(pEntity + OFFSET_GLOW_THROUGH_WALLS, 1); // 1 = far, 2 = close
						apex_mem.Write<GlowMode>(pEntity + GLOW_START_TIME, { 101,101,99,90 });

						apex_mem.Write<float>(pEntity + GLOW_COLOR_R, 255 / itemglowbrightness); // r
						apex_mem.Write<float>(pEntity + GLOW_COLOR_G, 215 / itemglowbrightness); // g
						apex_mem.Write<float>(pEntity + GLOW_COLOR_B, 0 / itemglowbrightness); // b
					}
					if (accelerant && itemID == 163)
					{
						apex_mem.Write<int>(pEntity + OFFSET_GLOW_ENABLE, 1);
						apex_mem.Write<int>(pEntity + OFFSET_GLOW_THROUGH_WALLS, 1); // 1 = far, 2 = close
						apex_mem.Write<GlowMode>(pEntity + GLOW_START_TIME, { 101,101,99,90 });

						apex_mem.Write<float>(pEntity + GLOW_COLOR_R, 160 / itemglowbrightness); // r
						apex_mem.Write<float>(pEntity + GLOW_COLOR_G, 32 / itemglowbrightness); // g
						apex_mem.Write<float>(pEntity + GLOW_COLOR_B, 240 / itemglowbrightness); // b
					}
					
					//Nades
					if (grenade_frag && itemID == 193) 
					{
						apex_mem.Write<int>(pEntity + OFFSET_GLOW_ENABLE, 1);
						apex_mem.Write<int>(pEntity + OFFSET_GLOW_THROUGH_WALLS, 1); // 1 = far, 2 = close
						apex_mem.Write<GlowMode>(pEntity + GLOW_START_TIME, { 101,101,99,90 });

						apex_mem.Write<float>(pEntity + GLOW_COLOR_R, 255 / itemglowbrightness); // r
						apex_mem.Write<float>(pEntity + GLOW_COLOR_G, 255 / itemglowbrightness); // g
						apex_mem.Write<float>(pEntity + GLOW_COLOR_B, 255 / itemglowbrightness); // b
					}
					
					if (grenade_thermite && itemID == 192) 
					{
						apex_mem.Write<int>(pEntity + OFFSET_GLOW_ENABLE, 1);
						apex_mem.Write<int>(pEntity + OFFSET_GLOW_THROUGH_WALLS, 1); // 1 = far, 2 = close
						apex_mem.Write<GlowMode>(pEntity + GLOW_START_TIME, { 101,101,99,90 });

						apex_mem.Write<float>(pEntity + GLOW_COLOR_R, 255); // r
						apex_mem.Write<float>(pEntity + GLOW_COLOR_G, 0); // g
						apex_mem.Write<float>(pEntity + GLOW_COLOR_B, 0); // b
					}
					if (grenade_arc_star && itemID == 194) 
					{
						apex_mem.Write<int>(pEntity + OFFSET_GLOW_ENABLE, 1);
						apex_mem.Write<int>(pEntity + OFFSET_GLOW_THROUGH_WALLS, 1); // 1 = far, 2 = close
						apex_mem.Write<GlowMode>(pEntity + GLOW_START_TIME, { 101,101,99,90 });

						apex_mem.Write<float>(pEntity + GLOW_COLOR_R, 0 ); // r
						apex_mem.Write<float>(pEntity + GLOW_COLOR_G, 0 ); // g
						apex_mem.Write<float>(pEntity + GLOW_COLOR_B, 255 ); // b
					}
					//Barrel Stabilizer 2 rare
					if (suppressor &&  itemID == 206) 
					{
						apex_mem.Write<int>(pEntity + OFFSET_GLOW_ENABLE, 1);
						apex_mem.Write<int>(pEntity + OFFSET_GLOW_THROUGH_WALLS, 1); // 1 = far, 2 = close
						apex_mem.Write<GlowMode>(pEntity + GLOW_START_TIME, { 101,101,99,90 });

						apex_mem.Write<float>(pEntity + GLOW_COLOR_R, 0 ); // r
						apex_mem.Write<float>(pEntity + GLOW_COLOR_G, 0 ); // g
						apex_mem.Write<float>(pEntity + GLOW_COLOR_B, 255 ); // b
					}
					//Barrel Stabilizer 3 epic
					if (suppressor &&  itemID == 207) 
					{
						apex_mem.Write<int>(pEntity + OFFSET_GLOW_ENABLE, 1);
						apex_mem.Write<int>(pEntity + OFFSET_GLOW_THROUGH_WALLS, 1); // 1 = far, 2 = close
						apex_mem.Write<GlowMode>(pEntity + GLOW_START_TIME, { 101,101,99,90 });

						apex_mem.Write<float>(pEntity + GLOW_COLOR_R, 160 / itemglowbrightness); // r
						apex_mem.Write<float>(pEntity + GLOW_COLOR_G, 32 / itemglowbrightness); // g
						apex_mem.Write<float>(pEntity + GLOW_COLOR_B, 240 / itemglowbrightness); // b
					}
					//Barrel Stabilizer 4
					if (suppressor &&  itemID == 208) 
					{
						apex_mem.Write<int>(pEntity + OFFSET_GLOW_ENABLE, 1);
						apex_mem.Write<int>(pEntity + OFFSET_GLOW_THROUGH_WALLS, 1); // 1 = far, 2 = close
						apex_mem.Write<GlowMode>(pEntity + GLOW_START_TIME, { 101,101,99,90 });

						apex_mem.Write<float>(pEntity + GLOW_COLOR_R, 255 / itemglowbrightness); // r
						apex_mem.Write<float>(pEntity + GLOW_COLOR_G, 215 / itemglowbrightness); // g
						apex_mem.Write<float>(pEntity + GLOW_COLOR_B, 0 / itemglowbrightness); // b
					}
					//Med Backpack
					if (medbackpack &&  itemID == 188)  
					{
						apex_mem.Write<int>(pEntity + OFFSET_GLOW_ENABLE, 1);
						apex_mem.Write<int>(pEntity + OFFSET_GLOW_THROUGH_WALLS, 2); // 1 = far, 2 = close
						apex_mem.Write<GlowMode>(pEntity + GLOW_START_TIME, { 101,101,99,90 });
 
						apex_mem.Write<float>(pEntity + GLOW_COLOR_R, 0); // r
						apex_mem.Write<float>(pEntity + GLOW_COLOR_G, 0); // g
						apex_mem.Write<float>(pEntity + GLOW_COLOR_B, 255 / itemglowbrightness); // b
					
					}
					if (shieldlarge &&  itemID == 167) 
					{
						apex_mem.Write<int>(pEntity + OFFSET_GLOW_ENABLE, 1);
						apex_mem.Write<int>(pEntity + OFFSET_GLOW_THROUGH_WALLS, 1); // 1 = far, 2 = close
						apex_mem.Write<GlowMode>(pEntity + GLOW_START_TIME, { 101,101,99,90 });
 
						apex_mem.Write<float>(pEntity + GLOW_COLOR_R, 0); // r
						apex_mem.Write<float>(pEntity + GLOW_COLOR_G, 0); // g
						apex_mem.Write<float>(pEntity + GLOW_COLOR_B, 225); // b
					}
				
				
					//printf("Item ID: %i\n", itemID);
					if (itemID == 194)
					{
						apex_mem.Write<int>(pEntity + OFFSET_GLOW_ENABLE, 1);
						apex_mem.Write<int>(pEntity + OFFSET_GLOW_THROUGH_WALLS, 1); // 1 = far, 2 = close
						apex_mem.Write<GlowMode>(pEntity + GLOW_START_TIME, { 101,101,99,90 });
 
						apex_mem.Write<float>(pEntity + GLOW_COLOR_R, 0 / itemglowbrightness); // r
						apex_mem.Write<float>(pEntity + GLOW_COLOR_G, 0 / itemglowbrightness); // g
						apex_mem.Write<float>(pEntity + GLOW_COLOR_B, 255 / itemglowbrightness); // b
					}
					
					}
					
						
					
					
					
					
					*/
					//Weapon ID in hand.
					
					// CREDITS to Rikkie https://www.unknowncheats.me/forum/members/169606.html
					// for all the weapon ids and item ids code, you are a life saver!

					if(item.isItem() && !item.isGlowing())
					{
						//item.enableGlow();
					}
					//Item filter glow name setup and search.
					char glowName[200] = { 0 };
					uint64_t name_ptr;
					apex_mem.Read<uint64_t>(centity + OFFSET_MODELNAME, name_ptr);
					apex_mem.ReadArray<char>(name_ptr, glowName, 200);

					apex_mem.Read<uint64_t>(entitylist + (i * 0x20), centity);
					
					uint32_t itemID;
					apex_mem.Read<uint32_t>(centity + OFFSET_M_CUSTOMSCRIPTINT, itemID); // 0x1648
					//Prints stuff you want to console
					//if (strstr(glowName, "mdl/")) 
					//{
					//printf("%s\n", glowName);
					//}
					//Search model name and if true sets glow, must be a better way to do this.. if only i got the item id to work..
					//White/Grey
					auto CommonGlowNoFill = [centity]()
					{
						apex_mem.Write<int>(centity + OFFSET_GLOW_ENABLE, 1);
						apex_mem.Write<int>(centity + OFFSET_GLOW_THROUGH_WALLS, 1); // 1 = far, 2 = close
						apex_mem.Write<GlowMode>(centity + GLOW_START_TIME, { 0,145,50,90 });

						apex_mem.Write<float>(centity + GLOW_COLOR_R, 35 / itemglowbrightness); // r
						apex_mem.Write<float>(centity + GLOW_COLOR_G, 35 / itemglowbrightness); // g
						apex_mem.Write<float>(centity + GLOW_COLOR_B, 35 / itemglowbrightness); // b
					};
					//Blue
					auto RareGlowNoFill = [centity]()
					{
						apex_mem.Write<int>(centity + OFFSET_GLOW_ENABLE, 1);
						apex_mem.Write<int>(centity + OFFSET_GLOW_THROUGH_WALLS, 1); // 1 = far, 2 = close
						apex_mem.Write<GlowMode>(centity + GLOW_START_TIME, { 0,145,50,90 });

						apex_mem.Write<float>(centity + GLOW_COLOR_R, 10 / itemglowbrightness); // r
						apex_mem.Write<float>(centity + GLOW_COLOR_G, 35 / itemglowbrightness); // g
						apex_mem.Write<float>(centity + GLOW_COLOR_B, 100 / itemglowbrightness); // b
					};
					//Purple
					auto EpicGlowNoFill = [centity]()
					{
						apex_mem.Write<int>(centity + OFFSET_GLOW_ENABLE, 1);
						apex_mem.Write<int>(centity + OFFSET_GLOW_THROUGH_WALLS, 1); // 1 = far, 2 = close
						apex_mem.Write<GlowMode>(centity + GLOW_START_TIME, { 0,145,50,90 });

						apex_mem.Write<float>(centity + GLOW_COLOR_R, 160 / itemglowbrightness); // r
						apex_mem.Write<float>(centity + GLOW_COLOR_G, 32 / itemglowbrightness); // g
						apex_mem.Write<float>(centity + GLOW_COLOR_B, 240 / itemglowbrightness); // b
					};
					//Gold needs better rgb for sure
					auto LegendaryGlowNoFill = [centity]()
					{
						apex_mem.Write<int>(centity + OFFSET_GLOW_ENABLE, 1);
						apex_mem.Write<int>(centity + OFFSET_GLOW_THROUGH_WALLS, 1); // 1 = far, 2 = close
						apex_mem.Write<GlowMode>(centity + GLOW_START_TIME, { 0,145,50,90 });

						apex_mem.Write<float>(centity + GLOW_COLOR_R, 255 / itemglowbrightness); // r
						apex_mem.Write<float>(centity + GLOW_COLOR_G, 215 / itemglowbrightness); // g
						apex_mem.Write<float>(centity + GLOW_COLOR_B, 0 / itemglowbrightness); // b
					};
					//Red
					auto HeirloomGlowNoFill = [centity]() 
					{
						apex_mem.Write<int>(centity + OFFSET_GLOW_ENABLE, 1);
						apex_mem.Write<int>(centity + OFFSET_GLOW_THROUGH_WALLS, 1); // 1 = far, 2 = close
						apex_mem.Write<GlowMode>(centity + GLOW_START_TIME, { 0,145,50,90 });

						apex_mem.Write<float>(centity + GLOW_COLOR_R, 255 / itemglowbrightness); // r
						apex_mem.Write<float>(centity + GLOW_COLOR_G, 215 / itemglowbrightness); // g
						apex_mem.Write<float>(centity + GLOW_COLOR_B, 0 / itemglowbrightness); // b
					};
					//HeirloomGlow
					auto HeirloomGlowCryptoCustom = [centity]()
					{
						apex_mem.Write<int>(centity + OFFSET_GLOW_ENABLE, 1);
						apex_mem.Write<int>(centity + OFFSET_GLOW_THROUGH_WALLS, 1); // 1 = far, 2 = close
						apex_mem.Write<GlowMode>(centity + GLOW_START_TIME, { 139,101,70,90 });
 
						apex_mem.Write<float>(centity + GLOW_COLOR_R, 255 / itemglowbrightness); // r
						apex_mem.Write<float>(centity + GLOW_COLOR_G, 255 / itemglowbrightness); // g
						apex_mem.Write<float>(centity + GLOW_COLOR_B, 255 / itemglowbrightness); // b
					};
					//SniperGlow
					auto SniperGlowCryptoCustom = [centity]()
					{
						apex_mem.Write<int>(centity + OFFSET_GLOW_ENABLE, 1);
						apex_mem.Write<int>(centity + OFFSET_GLOW_THROUGH_WALLS, 1); // 1 = far, 2 = close
						apex_mem.Write<GlowMode>(centity + GLOW_START_TIME, { 139,101,70,90 });
 
						apex_mem.Write<float>(centity + GLOW_COLOR_R, 40 / itemglowbrightness); // r //92
						apex_mem.Write<float>(centity + GLOW_COLOR_G, 40 / itemglowbrightness); // g //92
						apex_mem.Write<float>(centity + GLOW_COLOR_B, 150 / itemglowbrightness); // b //255
					};
					//EnergyGlow
					auto EnergyGlowCryptoCustom = [centity]()
					{
						apex_mem.Write<int>(centity + OFFSET_GLOW_ENABLE, 1);
						apex_mem.Write<int>(centity + OFFSET_GLOW_THROUGH_WALLS, 1); // 1 = far, 2 = close
						apex_mem.Write<GlowMode>(centity + GLOW_START_TIME, { 139,101,70,90 });
 
						apex_mem.Write<float>(centity + GLOW_COLOR_R, 160 / itemglowbrightness); // r 
						apex_mem.Write<float>(centity + GLOW_COLOR_G, 180 / itemglowbrightness); // g
						apex_mem.Write<float>(centity + GLOW_COLOR_B, 0 / itemglowbrightness); // b
					};
					//LightGlow
					auto LightGlowCryptoCustom = [centity]()
					{
						apex_mem.Write<int>(centity + OFFSET_GLOW_ENABLE, 1);
						apex_mem.Write<int>(centity + OFFSET_GLOW_THROUGH_WALLS, 1); // 1 = far, 2 = close
						apex_mem.Write<GlowMode>(centity + GLOW_START_TIME, { 139,101,70,90 });
 
						apex_mem.Write<float>(centity + GLOW_COLOR_R, 120 / itemglowbrightness); // r
						apex_mem.Write<float>(centity + GLOW_COLOR_G, 55 / itemglowbrightness); // g
						apex_mem.Write<float>(centity + GLOW_COLOR_B, 15 / itemglowbrightness); // b
					};
					//HeavyGlow
					auto HeavyGlowCryptoCustom = [centity]()
					{
						apex_mem.Write<int>(centity + OFFSET_GLOW_ENABLE, 1);
						apex_mem.Write<int>(centity + OFFSET_GLOW_THROUGH_WALLS, 1); // 1 = far, 2 = close
						apex_mem.Write<GlowMode>(centity + GLOW_START_TIME, { 139,101,70,90 });
 
						apex_mem.Write<float>(centity + GLOW_COLOR_R, 20 / itemglowbrightness); // r // 0
						apex_mem.Write<float>(centity + GLOW_COLOR_G, 64 / itemglowbrightness); // g // 250
						apex_mem.Write<float>(centity + GLOW_COLOR_B, 20 / itemglowbrightness); // b // 154
					};
					//ShotgunGlow
					auto ShotgunGlowCryptoCustom = [centity]()
					{
						apex_mem.Write<int>(centity + OFFSET_GLOW_ENABLE, 1);
						apex_mem.Write<int>(centity + OFFSET_GLOW_THROUGH_WALLS, 1); // 1 = far, 2 = close
						apex_mem.Write<GlowMode>(centity + GLOW_START_TIME, { 139,101,70,90 });
 
						apex_mem.Write<float>(centity + GLOW_COLOR_R, 150 / itemglowbrightness); // r
						apex_mem.Write<float>(centity + GLOW_COLOR_G, 0 / itemglowbrightness); // g
						apex_mem.Write<float>(centity + GLOW_COLOR_B, 0 / itemglowbrightness); // b
					};
					//Shield Regen
					auto ShieldRegenGlow = [centity]()
					{
						apex_mem.Write<int>(centity + OFFSET_GLOW_ENABLE, 1);
						apex_mem.Write<int>(centity + OFFSET_GLOW_THROUGH_WALLS, 1); // 1 = far, 2 = close
						apex_mem.Write<GlowMode>(centity + GLOW_START_TIME, { 141,145,90,90 });
 
						apex_mem.Write<float>(centity + GLOW_COLOR_R, 10 / itemglowbrightness); // r // 0
						apex_mem.Write<float>(centity + GLOW_COLOR_G, 35 / itemglowbrightness); // g // 0
						apex_mem.Write<float>(centity + GLOW_COLOR_B, 100 / itemglowbrightness); // b // 225
					};
					//Flesh Regen
					auto FleshRegenGlow = [centity]()
					{
						apex_mem.Write<int>(centity + OFFSET_GLOW_ENABLE, 1);
						apex_mem.Write<int>(centity + OFFSET_GLOW_THROUGH_WALLS, 1); // 1 = far, 2 = close
						apex_mem.Write<GlowMode>(centity + GLOW_START_TIME, { 141,145,90,90 });
 
						apex_mem.Write<float>(centity + GLOW_COLOR_R, 20 / itemglowbrightness); // r
						apex_mem.Write<float>(centity + GLOW_COLOR_G, 128 / itemglowbrightness); // g
						apex_mem.Write<float>(centity + GLOW_COLOR_B, 20 / itemglowbrightness); // b
					};

					////////////////////////////Nades/////////////////////////////
					//Patchwork fix for Nades becoming lost in the sauce when not toggled on.If item glow is toggled on these still technically glow even if selectively toggled off
					//Frag
					if (!grenade_frag && itemID == 193)
					{
						CommonGlowNoFill();
					}
					//Thermite
					if (!grenade_thermite && itemID == 192)
					{
						CommonGlowNoFill();
					}
					//Arc Star
					if (!grenade_arc_star && itemID == 194)
					{
						CommonGlowNoFill();
					}
					////////////////////////////Nades////////////////////////////

					////////////////////////////Mags////////////////////////////
					//Patchwork fix for Mags becoming lost in the sauce when not toggled on.If item glow is toggled on these still technically glow even if selectively toggled off
					//Legendary sniper mag
					if (!magsniper && itemID == 227)
					{
						LegendaryGlowNoFill();
					}
					//Epic sniper mag
					if (!magsniper && itemID == 226)
					{
						EpicGlowNoFill();
					}
					//Rare sniper mag
					if (!magsniper && itemID == 225)
					{
						RareGlowNoFill();
					}
					//Common sniper mag
					if (!magsniper && itemID == 224)
					{
						CommonGlowNoFill();
					}
					//Legendary energy mag
					if (!magenergy && itemID == 223)
					{
						LegendaryGlowNoFill();
					}
					//Epic energy mag
					if (!magenergy && itemID == 222)
					{
						EpicGlowNoFill();
					}
					//Rare energy mag
					if (!magenergy && itemID == 221)
					{
						RareGlowNoFill();
					}
					//Common energy mag
					if (!magenergy && itemID == 220)
					{
						CommonGlowNoFill();
					}
					//Legendary light mag
					if (!maglight && itemID == 215)
					{
						LegendaryGlowNoFill();
					}
					//Epic light mag
					if (!maglight && itemID == 214)
					{
						EpicGlowNoFill();
					}
					//Rare light mag
					if (!maglight && itemID == 213)
					{
						RareGlowNoFill();
					}
					//Common light mag
					if (!maglight && itemID == 212)
					{
						CommonGlowNoFill();
					}
					//Legendary heavy mag
					if (!magheavy && itemID == 219)
					{
						LegendaryGlowNoFill();
					}
					//Epic heavy mag
					if (!magheavy && itemID == 218)
					{
						EpicGlowNoFill();
					}
					//Rare heavy mag
					if (!magheavy && itemID == 217)
					{
						RareGlowNoFill();
					}
					//Common heavy mag
					if (!magheavy && itemID == 216)
					{
						CommonGlowNoFill();
					}
					//Legendary shotgun bolt
					if (!shotgunbolt && itemID == 231)
					{
						LegendaryGlowNoFill();
					}
					//Epic shotgun bolt
					if (!shotgunbolt && itemID == 230)
					{
						EpicGlowNoFill();
					}
					//Rare shotgun bolt
					if (!shotgunbolt && itemID == 229)
					{
						RareGlowNoFill();
					}
					//Common shotgun bolt
					if (!shotgunbolt && itemID == 228)
					{
						CommonGlowNoFill();
					}
					//Sniper mag
					if (magsniper && strstr(glowName, "mdl/weapons_r5/loot/_master/w_loot_wep_mods_mag_sniper_v1.rmdl"))
					{
						apex_mem.Write<int>(centity + OFFSET_ITEM_GLOW, 1363184265);
					}
					//Energy mag
					if (magenergy && strstr(glowName, "mdl/weapons_r5/loot/_master/w_loot_wep_mods_mag_energy_v1.rmdl"))
					{
						apex_mem.Write<int>(centity + OFFSET_ITEM_GLOW, 1363184265);
					}
					//Light mag
					if (maglight && strstr(glowName, "mdl/weapons_r5/loot/_master/w_loot_wep_mods_mag_v1b.rmdl"))
					{
						apex_mem.Write<int>(centity + OFFSET_ITEM_GLOW, 1363184265);
					}
					//Heavy mag
					if (magheavy && strstr(glowName, "mdl/weapons_r5/loot/_master/w_loot_wep_mods_mag_v2b.rmdl"))
					{
						apex_mem.Write<int>(centity + OFFSET_ITEM_GLOW, 1363184265);
					}
					//Shotgun bolt
					if (shotgunbolt && strstr(glowName, "mdl/weapons_r5/loot/_master/w_loot_wep_mods_mag_v3b.rmdl"))
					{
						apex_mem.Write<int>(centity + OFFSET_ITEM_GLOW, 1363184265);
					}
					////////////////////////////Mags////////////////////////////

					////////////////////////////Helmets////////////////////////////
					//Generic Helmet
					if (helmet && strstr(glowName, "mdl/weapons_r5/loot/_master/w_loot_cha_shield_upgrade_head.rmdl"))
					{
						apex_mem.Write<int>(centity + OFFSET_ITEM_GLOW, 1363184265);
					}
					////////////////////////////Helmets////////////////////////////

					////////////////////////////Shields////////////////////////////
					//Generic Shield
					if (bodyshield && strstr(glowName, "mdl/weapons_r5/loot/_master/w_loot_cha_shield_upgrade_body.rmdl"))
					{
						apex_mem.Write<int>(centity + OFFSET_ITEM_GLOW, 1363184265);
					}
					////////////////////////////Shields////////////////////////////

					////////////////////////////Knockdown Shields////////////////////////////
					//Generic Knockdown Shield
					if (knockdownshield && strstr(glowName, "mdl/weapons_r5/loot/w_loot_wep_iso_shield_down_v1.rmdl"))
					{
						apex_mem.Write<int>(centity + OFFSET_ITEM_GLOW, 1363184265);
					}
					////////////////////////////Knockdown Shields////////////////////////////

					////////////////////////////Backpacks////////////////////////////
					//Common backpack
					if (lightbackpack && strstr(glowName, "mdl/humans_r5/loot/w_loot_char_backpack_light.rmdl"))
					{
						apex_mem.Write<int>(centity + OFFSET_GLOW_ENABLE, 1);
						apex_mem.Write<int>(centity + OFFSET_GLOW_THROUGH_WALLS, 1); // 1 = far, 2 = close
						apex_mem.Write<GlowMode>(centity + GLOW_START_TIME, { 141,145,90,90 });
 
						apex_mem.Write<float>(centity + GLOW_COLOR_R, 35 / itemglowbrightness); // r
						apex_mem.Write<float>(centity + GLOW_COLOR_G, 35 / itemglowbrightness); // g
						apex_mem.Write<float>(centity + GLOW_COLOR_B, 35 / itemglowbrightness); // b
					}
					//Rare backpack
					if (medbackpack && strstr(glowName, "mdl/humans_r5/loot/w_loot_char_backpack_medium.rmdl"))
					{
						apex_mem.Write<int>(centity + OFFSET_GLOW_ENABLE, 1);
						apex_mem.Write<int>(centity + OFFSET_GLOW_THROUGH_WALLS, 1); // 1 = far, 2 = close
						apex_mem.Write<GlowMode>(centity + GLOW_START_TIME, { 141,145,90,90 });
 
						apex_mem.Write<float>(centity + GLOW_COLOR_R, 15 / itemglowbrightness); // r // 0
						apex_mem.Write<float>(centity + GLOW_COLOR_G, 35 / itemglowbrightness); // g // 191
						apex_mem.Write<float>(centity + GLOW_COLOR_B, 100 / itemglowbrightness); // b // 255
					
					}
					//Both Epic and Legendary
					if (heavybackpack && strstr(glowName, "mdl/humans_r5/loot/w_loot_char_backpack_heavy.rmdl"))
					{
						apex_mem.Write<int>(centity + OFFSET_ITEM_GLOW, 1363184265);
					
					}
					////////////////////////////Backpacks///////////////////////////

					////////////////////////////Regen////////////////////////////
					//Phoenix Kit
					if (phoenix && strstr(glowName, "mdl/weapons_r5/loot/w_loot_wep_iso_phoenix_kit_v1.rmdl"))
					{
						apex_mem.Write<int>(centity + OFFSET_GLOW_ENABLE, 1);
						apex_mem.Write<int>(centity + OFFSET_GLOW_THROUGH_WALLS, 2); // 1 = far, 2 = close
						apex_mem.Write<GlowMode>(centity + GLOW_START_TIME, { 141,145,90,90 });
 
						apex_mem.Write<float>(centity + GLOW_COLOR_R, 255 / itemglowbrightness); // r
						apex_mem.Write<float>(centity + GLOW_COLOR_G, 0 / itemglowbrightness); // g
						apex_mem.Write<float>(centity + GLOW_COLOR_B, 255 / itemglowbrightness); // b
					}
					//Shield Battery
					if (shieldlarge && strstr(glowName, "mdl/weapons_r5/loot/w_loot_wep_iso_shield_battery_large.rmdl"))
					{
						ShieldRegenGlow();
					}
					//Med Kit
					if (healthlarge && strstr(glowName, "mdl/weapons_r5/loot/w_loot_wep_iso_health_main_large.rmdl"))
					{
						FleshRegenGlow();
					}
					//Shield Cell
					if (shieldsmall && strstr(glowName, "mdl/weapons_r5/loot/w_loot_wep_iso_shield_battery_small.rmdl"))
					{
						ShieldRegenGlow();
					}
					//Syringe
					if (healthsmall && strstr(glowName, "mdl/weapons_r5/loot/w_loot_wep_iso_health_main_small.rmdl"))
					{
						FleshRegenGlow();
					}
					//Ultimate Accelerant
					if (accelerant && strstr(glowName, "mdl/weapons_r5/loot/w_loot_wep_iso_ultimate_accelerant.rmdl"))
					{
						apex_mem.Write<int>(centity + OFFSET_ITEM_GLOW, 1363184265);
					}
					////////////////////////////Regen////////////////////////////

					////////////////////////////Death Box////////////////////////////
					//Death Box
					if (item.isBox())
					{
						apex_mem.Write<int>(centity + OFFSET_GLOW_T1, 16256);
						apex_mem.Write<int>(centity + OFFSET_GLOW_T2, 1193322764);
						apex_mem.Write<int>(centity + OFFSET_GLOW_ENABLE, 7);
						apex_mem.Write<int>(centity + OFFSET_GLOW_THROUGH_WALLS, 2);
					}
					////////////////////////////Death Box////////////////////////////

					////////////////////////////Traps////////////////////////////
					//Generic Trap
					if (item.isTrap())
					{
						apex_mem.Write<int>(centity + OFFSET_GLOW_T1, 16256);
						apex_mem.Write<int>(centity + OFFSET_GLOW_T2, 1193322764);
						apex_mem.Write<int>(centity + OFFSET_GLOW_ENABLE, 7);
						apex_mem.Write<int>(centity + OFFSET_GLOW_THROUGH_WALLS, 2);
					}
					//Caustic Nox Gas Trap
					if (strstr(glowName, "mdl/props/caustic_gas_tank/caustic_gas_tank.rmdl"))
					{
						apex_mem.Write<int>(centity + OFFSET_GLOW_ENABLE, 1);
						apex_mem.Write<int>(centity + OFFSET_GLOW_THROUGH_WALLS, 1); // 1 = far, 2 = close
						apex_mem.Write<GlowMode>(centity + GLOW_START_TIME, { 141,145,90,90 });
 
						apex_mem.Write<float>(centity + GLOW_COLOR_R, 255 / itemglowbrightness); // r
						apex_mem.Write<float>(centity + GLOW_COLOR_G, 0 / itemglowbrightness); // g
						apex_mem.Write<float>(centity + GLOW_COLOR_B, 0 / itemglowbrightness); // b
					}
					////////////////////////////Traps////////////////////////////

					////////////////////////////Ammo////////////////////////////
					//Sniper Ammo
					if (ammosniper && strstr(glowName, "mdl/weapons_r5/loot/_master/w_loot_wep_ammo_sniper.rmdl"))
					{
						SniperGlowCryptoCustom();
					}
					//Energy Ammo
					if (ammoenergy && strstr(glowName, "mdl/weapons_r5/loot/_master/w_loot_wep_ammo_nrg.rmdl"))
					{
						EnergyGlowCryptoCustom();
					}
					//Light Ammo
					if (ammolight && strstr(glowName, "mdl/weapons_r5/loot/_master/w_loot_wep_ammo_sc.rmdl"))
					{
						LightGlowCryptoCustom();
					}
					//Heavy Ammo
					if (ammoheavy && strstr(glowName, "mdl/weapons_r5/loot/_master/w_loot_wep_ammo_hc.rmdl"))
					{
						HeavyGlowCryptoCustom();
					}
					//Shotgun Shells
					if (ammoshotgun && strstr(glowName, "mdl/weapons_r5/loot/_master/w_loot_wep_ammo_shg.rmdl"))
					{
						ShotgunGlowCryptoCustom();
					}
					////////////////////////////Ammo////////////////////////////

					////////////////////////////Optics////////////////////////////
					//1x Holo
					if (opticholo1x && strstr(glowName, "mdl/weapons_r5/loot/_master/w_loot_wep_mods_optic_cq_holo_var.rmdl"))
					{
						apex_mem.Write<int>(centity + OFFSET_ITEM_GLOW, 1363184265);
					}
					//1x HCOG (Classic)
					if (optic && strstr(glowName, "mdl/weapons_r5/loot/_master/w_loot_wep_mods_optic_cq_hcog_r1.rmdl"))
					{
						apex_mem.Write<int>(centity + OFFSET_ITEM_GLOW, 1363184265);
					}
					//1x Digital Threat
					if (opticthreat && strstr(glowName, "mdl/weapons_r5/loot/_master/w_loot_wep_mods_optic_cq_threat.rmdl"))
					{
						apex_mem.Write<int>(centity + OFFSET_ITEM_GLOW, 1363184265);
					}
					//1x-2x Variable Holo
					if (opticholo1x2x && strstr(glowName, "mdl/weapons_r5/loot/_master/w_loot_wep_mods_optic_cq_holo_var_2x.rmdl"))
					{
						apex_mem.Write<int>(centity + OFFSET_ITEM_GLOW, 1363184265);
					}
					//2x HCOG (Bruiser)
					if (optic2x && strstr(glowName, "mdl/weapons_r5/loot/_master/w_loot_wep_mods_optic_cq_hcog_r2.rmdl"))
					{
						apex_mem.Write<int>(centity + OFFSET_ITEM_GLOW, 1363184265);
					}
					//2x-4x Variable AOG
					if (optic2x4x && strstr(glowName, "mdl/weapons_r5/loot/_master/w_loot_wep_mods_optic_rng_aog_var_r1.rmdl"))
					{
						apex_mem.Write<int>(centity + OFFSET_ITEM_GLOW, 1363184265);
					}
					//3x HCOG (Ranger)
					if (optic3x && strstr(glowName, "mdl/weapons_r5/loot/_master/w_loot_wep_mods_optic_rng_hcog_acgs.rmdl"))
					{
						apex_mem.Write<int>(centity + OFFSET_ITEM_GLOW, 1363184265);
					}
					//6x Sniper
					if (opticsniper6x && strstr(glowName, "mdl/weapons_r5/loot/_master/w_loot_wep_mods_optic_sni_dcom.rmdl"))
					{
						apex_mem.Write<int>(centity + OFFSET_ITEM_GLOW, 1363184265);
					}
					//4x-8x Variable Sniper
					if (opticsniper4x8x && strstr(glowName, "mdl/weapons_r5/loot/_master/w_loot_wep_mods_optic_sni_var_talon.rmdl"))
					{
						apex_mem.Write<int>(centity + OFFSET_ITEM_GLOW, 1363184265);
					}
					//4x-10x Digital Sniper Threat
					if (opticsniperthreat && strstr(glowName, "mdl/weapons_r5/loot/_master/w_loot_wep_mods_optic_sni_threat_wyeon.rmdl"))
					{
						apex_mem.Write<int>(centity + OFFSET_ITEM_GLOW, 1363184265);
					}
					////////////////////////////Optics////////////////////////////

					////////////////////////////Laser-sights\Barrels////////////////////////////
					//Generic Laser-Sight
					if (lasersight && strstr(glowName, "mdl/weapons_r5/loot/_master/w_loot_wep_mods_lasersight_v1.rmdl"))
					{
						apex_mem.Write<int>(centity + OFFSET_ITEM_GLOW, 1363184265);
					}
					//Generic Barrel
					if (suppressor && strstr(glowName, "mdl/weapons_r5/loot/_master/w_loot_wep_mods_suppr_v2b.rmdl"))
					{
						apex_mem.Write<int>(centity + OFFSET_ITEM_GLOW, 1363184265);
					}
					////////////////////////////Laser-sights\Barrels////////////////////////////
					
					////////////////////////////Stocks////////////////////////////
					//Generic Sniper Stock
					if (stocksniper && strstr(glowName, "mdl/weapons_r5/loot/w_loot_wep_iso_stock_folded_sniper.rmdl"))
					{
						apex_mem.Write<int>(centity + OFFSET_ITEM_GLOW, 1363184265);
					}
					//Generic Standard Stock
					if (stockregular && strstr(glowName, "mdl/weapons_r5/loot/w_loot_wep_iso_stock_folded_regular.rmdl"))
					{
						apex_mem.Write<int>(centity + OFFSET_ITEM_GLOW, 1363184265);
					}
					////////////////////////////Stocks////////////////////////////

					////////////////////////////Hop-Ups////////////////////////////
					//Generic Hop-Up
					if (hopup && strstr(glowName, "mdl/weapons_r5/loot/_master/w_loot_wep_mods_chip.rmdl"))
					{
						apex_mem.Write<int>(centity + OFFSET_ITEM_GLOW, 1363184265);
					}
					////////////////////////////Hop-Ups////////////////////////////

					////////////////////////////Weapons////////////////////////////

					////////////////////////////Heirloom Weapons////////////////////////////
					if (weapon_kraber && strstr(glowName, "mdl/weapons/at_rifle/w_at_rifle.rmdl"))
					{
						HeirloomGlowCryptoCustom();
					}
					if (weapon_re45 && strstr(glowName, "mdl/weapons/p2011_auto/w_p2011_auto.rmdl"))
					{
						HeirloomGlowCryptoCustom();
					}
					if (weapon_bow && strstr(glowName, "mdl/Weapons/compound_bow/w_compound_bow.rmdl"))
					{
						HeirloomGlowCryptoCustom();
					}
					if (weapon_hemlock && strstr(glowName, "mdl/weapons/m1a1_hemlok/w_hemlok.rmdl"))
					{
						HeirloomGlowCryptoCustom();
					}
					////////////////////////////Heirloom Weapons////////////////////////////

					////////////////////////////Sniper Weapons////////////////////////////
					if (weapon_wingman && strstr(glowName, "mdl/weapons/b3wing/w_b3wing.rmdl"))
					{
						SniperGlowCryptoCustom();
					}
					if (weapon_sentinel && strstr(glowName, "mdl/Weapons/sentinel/w_sentinel.rmdl"))
					{
						SniperGlowCryptoCustom();
					}
					if (weapon_longbow && strstr(glowName, "mdl/weapons/rspn101_dmr/w_rspn101_dmr.rmdl"))
					{
						SniperGlowCryptoCustom();
					}
					if (weapon_charge_rifle && strstr(glowName, "mdl/weapons/defender/w_defender.rmdl"))
					{
						SniperGlowCryptoCustom();
					}
					////////////////////////////Sniper Weapons////////////////////////////

					////////////////////////////Energy Weapons////////////////////////////
					if (weapon_lstar && strstr(glowName, "mdl/weapons/lstar/w_lstar.rmdl"))
					{
						EnergyGlowCryptoCustom();
					}
					if (weapon_nemesis && strstr(glowName, "mdl/techart/mshop/weapons/class/assault/nemesis/nemesis_base_w.rmdl"))
					{
						EnergyGlowCryptoCustom();
					}
					if (weapon_havoc && strstr(glowName, "mdl/Weapons/beam_ar/w_beam_ar.rmdl"))
					{
						EnergyGlowCryptoCustom();
					}
					if (weapon_devotion && strstr(glowName, "mdl/weapons/hemlock_br/w_hemlock_br.rmdl"))
					{
						EnergyGlowCryptoCustom();
					}
					if (weapon_triple_take && strstr(glowName, "mdl/weapons/doubletake/w_doubletake.rmdl"))
					{
						EnergyGlowCryptoCustom();
					}
					if (weapon_volt && strstr(glowName, "mdl/weapons/hemlok_smg/w_hemlok_smg.rmdl"))
					{
						EnergyGlowCryptoCustom();
					}
					////////////////////////////Energy Weapons////////////////////////////

					////////////////////////////Light Weapons////////////////////////////
					if (weapon_g7_scout && strstr(glowName, "mdl/weapons/g2/w_g2a4.rmdl"))
					{
						LightGlowCryptoCustom();
					}
					if (weapon_alternator && strstr(glowName, "mdl/weapons/alternator_smg/w_alternator_smg.rmdl"))
					{
						LightGlowCryptoCustom();
					}
					if (weapon_r99 && strstr(glowName, "mdl/weapons/r97/w_r97.rmdl"))
					{
						LightGlowCryptoCustom();
					}
					if (weapon_spitfire && strstr(glowName, "mdl/weapons/lmg_hemlok/w_lmg_hemlok.rmdl"))
					{
						LightGlowCryptoCustom();
					}
					if (weapon_r301 && strstr(glowName, "mdl/weapons/rspn101/w_rspn101.rmdl"))
					{
						LightGlowCryptoCustom();
					}
					if (weapon_p2020 && strstr(glowName, "mdl/weapons/p2011/w_p2011.rmdl"))
					{
						LightGlowCryptoCustom();
					}
					////////////////////////////Light Weapons////////////////////////////

					////////////////////////////Heavy Weapons////////////////////////////
					if (weapon_rampage_lmg && strstr(glowName, "mdl/techart/mshop/weapons/class/lmg/dragon/dragon_base_w.rmdl"))
					{
						HeavyGlowCryptoCustom();
					}
					if (weapon_flatline  && strstr(glowName, "mdl/techart/mshop/weapons/class/assault/flatline/flatline_base_w.rmdl"))
					{
						HeavyGlowCryptoCustom();
					}
					if (weapon_prowler && strstr(glowName, "mdl/Weapons/prowler_smg/w_prowler_smg.rmdl"))
					{
						HeavyGlowCryptoCustom();
					}
					if (weapon_3030_repeater && strstr(glowName, "mdl/weapons/3030repeater/w_3030repeater.rmdl"))
					{
						HeavyGlowCryptoCustom();
					}
					if (weapon_car_smg && strstr(glowName, "mdl/techart/mshop/weapons/class/smg/car/car_base_w.rmdl"))
					{
						HeavyGlowCryptoCustom();
					}
					////////////////////////////Heavy Weapons////////////////////////////

					////////////////////////////Shotgun Weapons////////////////////////////
					if (weapon_mastiff && strstr(glowName, "mdl/weapons/mastiff_stgn/w_mastiff.rmdl"))
					{
						ShotgunGlowCryptoCustom();
					}
					if (weapon_eva8 && strstr(glowName, "mdl/weapons/w1128/w_w1128.rmdl"))
					{
						ShotgunGlowCryptoCustom();
					}
					if (weapon_peacekeeper && strstr(glowName, "mdl/weapons/peacekeeper/w_peacekeeper.rmdl"))
					{
						ShotgunGlowCryptoCustom();
					}
					if (weapon_mozambique && strstr(glowName, "mdl/weapons/pstl_sa3/w_pstl_sa3.rmdl"))
					{
						ShotgunGlowCryptoCustom();
					}
					////////////////////////////Shotgun Weapons////////////////////////////

					////////////////////////////Weapons////////////////////////////
				}
				k=1;
				//Change the 60 ms to lower to make the death boxes filker less.
				//std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
			else
			{		
				if(k==1)
				{
					//same and the ents above to turn the glow off
					for (int i = 0; i < 15000; i++)
					{
						uint64_t centity = 0;
						apex_mem.Read<uint64_t>(entitylist + ((uint64_t)i << 5), centity);
						if (centity == 0) continue;
 
						Item item = getItem(centity);
 
						if(item.isItem() && item.isGlowing())
						{
							item.disableGlow();
						}
					}
					k=0;
				}
			}	
		}
	}
	item_t = false;
}

template <typename IDComp, typename... T>
bool is_in(IDComp&& Current, T&&... ID)
{
	return ((Current == ID) || ...);
}
template <typename NegIDComp, typename... T>
bool is_out(NegIDComp&& Current, T&&... ID)
{
	return ((Current != ID) || ...);
}

static void Headshot()
{
	headshot_t = true;
	while(headshot_t)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		while (g_Base !=0)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			uint64_t entitylist = g_Base + OFFSET_ENTITYLIST;
			if(headshot_on) // add headshot toggle or change here
			{
				uint64_t LocalPlayer = 0;
				apex_mem.Read<uint64_t>(g_Base + OFFSET_LOCAL_ENT, LocalPlayer);

				ulong ehWeaponHandle;
				apex_mem.Read<uint64_t>(LocalPlayer + OFFSET_WEAPON, ehWeaponHandle); // 0x1a1c
				ehWeaponHandle &= 0xFFFF; // eHandle
				ulong pWeapon;
				apex_mem.Read<uint64_t>(entitylist + (ehWeaponHandle * 0x20), pWeapon);
				
				uint32_t weaponID;
				apex_mem.Read<uint32_t>(pWeapon + OFFSET_WEAPON_NAME, weaponID); //0x1844
				//printf("%d\n", weaponID);
				/* if (is_in(weaponID, 119))
				{
					snipereq = 1;
				}
				else if (is_out(weaponID, 119))
				{
					snipereq = 0;
				} */
				
				if (weaponID == 2)
				{
					bowheadshotmode = 1;
					
				}
				else if (weaponID != 2)
				{
					bowheadshotmode = 0;
					
				}
			}
		}
	}
	headshot_t = false;
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
		printf("Error: %s is not running as root\n", argv[0]);
		return 0;
	}

	const char* cl_proc = "YourProcess.exe";
	const char* ap_proc = "R5Apex.exe";
	//const char* ap_proc = "EasyAntiCheat_launcher.exe";

	//Client "add" offset
	uint64_t add_off = 0x138a80; //todo make this auto update..

	std::thread aimbot_thr;
	std::thread esp_thr;
	std::thread actions_thr;
	std::thread itemglow_thr;
	std::thread headshot_thr;
	std::thread vars_thr;
	while(active)
	{
		if(apex_mem.get_proc_status() != process_status::FOUND_READY)
		{
			if(aim_t)
			{
				aim_t = false;
				esp_t = false;
				actions_t = false;
				item_t = false;
				headshot_t = false;
				g_Base = 0;

				aimbot_thr.~thread();
				esp_thr.~thread();
				actions_thr.~thread();
				itemglow_thr.~thread();
				headshot_thr.~thread();
			}

			std::this_thread::sleep_for(std::chrono::seconds(1));
			printf("Searching for apex process...\n");

			apex_mem.open_proc(ap_proc);

			if(apex_mem.get_proc_status() == process_status::FOUND_READY)
			{
				g_Base = apex_mem.get_proc_baseaddr();
				printf("\nApex process found\n");
				printf("Base: %lx\n", g_Base);

				aimbot_thr = std::thread(AimbotLoop);
				esp_thr = std::thread(EspLoop);
				actions_thr = std::thread(DoActions);
				itemglow_thr = std::thread(item_glow_t);
				headshot_thr = std::thread(Headshot);
				aimbot_thr.detach();
				esp_thr.detach();
				actions_thr.detach();
				itemglow_thr.detach();
				headshot_thr.detach();
			}
		}
		else
		{
			apex_mem.check_proc();
		}

		if(client_mem.get_proc_status() != process_status::FOUND_READY)
		{
			if(vars_t)
			{
				vars_t = false;
				c_Base = 0;

				vars_thr.~thread();
			}
			
			std::this_thread::sleep_for(std::chrono::seconds(1));
			printf("Searching for client process...\n");

			client_mem.open_proc(cl_proc);

			if(client_mem.get_proc_status() == process_status::FOUND_READY)
			{
				c_Base = client_mem.get_proc_baseaddr();
				printf("\nClient process found\n");
				printf("Base: %lx\n", c_Base);

				vars_thr = std::thread(set_vars, c_Base + add_off);
				vars_thr.detach();
			}
		}
		else
		{
			client_mem.check_proc();
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	prevTime = currentTime;
	return 0;
}