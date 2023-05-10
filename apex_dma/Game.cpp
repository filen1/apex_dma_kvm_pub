#include "prediction.h"

extern Memory apex_mem;

extern bool firing_range;

//setting up vars, dont edit 
float smooth = 80.f;
extern bool aim_no_recoil;
extern int bone;
float veltest = 2.f;
float smoothpred = .08f;
float smoothpred2 = .05f;
extern float MaxRotation; // maximum rotation in degrees

bool Entity::Observing(uint64_t entitylist)
{
	return *(bool*)(buffer + OFFSET_OBSERVER_MODE);
}

//https://github.com/CasualX/apexbot/blob/master/src/state.cpp#L104
void get_class_name(uint64_t entity_ptr, char* out_str)
{
	uint64_t client_networkable_vtable;
	apex_mem.Read<uint64_t>(entity_ptr + 8 * 3, client_networkable_vtable);

	uint64_t get_client_class;
	apex_mem.Read<uint64_t>(client_networkable_vtable + 8 * 3, get_client_class);

	uint32_t disp;
	apex_mem.Read<uint32_t>(get_client_class + 3, disp);
	const uint64_t client_class_ptr = get_client_class + disp + 7;

	ClientClass client_class;
	apex_mem.Read<ClientClass>(client_class_ptr, client_class);

	apex_mem.ReadArray<char>(client_class.pNetworkName, out_str, 32);
}

int Entity::getTeamId()
{
	return *(int*)(buffer + OFFSET_TEAM);
}

Vector Entity::getAbsVelocity()
{
	return *(Vector*)(buffer + OFFSET_ABS_VELOCITY);
}

Vector Entity::getPosition()
{
	return *(Vector*)(buffer + OFFSET_ORIGIN);
}

bool Entity::isPlayer()
{
	return *(uint64_t*)(buffer + OFFSET_NAME) == 125780153691248;
}

//firing range dummys
bool Entity::isDummy()
{
	char class_name[33] = {};
	get_class_name(ptr, class_name);

	return strncmp(class_name, "CAI_BaseNPC", 11) == 0;
}

bool Entity::isKnocked()
{
	return *(int*)(buffer + OFFSET_BLEED_OUT_STATE) > 0;
}

bool Entity::isAlive()
{
	return *(int*)(buffer + OFFSET_LIFE_STATE) == 0;
}

float Entity::lastVisTime()
{
  return *(float*)(buffer + OFFSET_VISIBLE_TIME);
}

//https://www.unknowncheats.me/forum/apex-legends/496984-getting-hitbox-positions-cstudiohdr-externally.html
//https://www.unknowncheats.me/forum/3499185-post1334.html
//hitboxes
Vector Entity::getBonePositionByHitbox(int id)
{
	Vector origin = getPosition();
 
    //BoneByHitBox
	uint64_t Model = *(uint64_t*)(buffer + OFFSET_STUDIOHDR);
    
	//get studio hdr
	uint64_t StudioHdr;
	apex_mem.Read<uint64_t>(Model + 0x8, StudioHdr);
 
    //get hitbox array
	uint16_t HitboxCache;
	apex_mem.Read<uint16_t>(StudioHdr + 0x34, HitboxCache);
	uint64_t HitboxArray = StudioHdr + ((uint16_t)(HitboxCache & 0xFFFE) << (4 * (HitboxCache & 1)));
 
	uint16_t  IndexCache;
	apex_mem.Read<uint16_t>(HitboxArray + 0x4, IndexCache);
	int HitboxIndex = ((uint16_t)(IndexCache & 0xFFFE) << (4 * (IndexCache & 1)));
 
	uint16_t  Bone;
	apex_mem.Read<uint16_t>(HitboxIndex + HitboxArray + (id * 0x20), Bone);
 
	if(Bone < 0 || Bone > 255)
		return Vector();
 
    //hitpos
	uint64_t Bones = *(uint64_t*)(buffer + OFFSET_BONES);
 
	matrix3x4_t Matrix = {};
	apex_mem.Read<matrix3x4_t>(Bones + Bone * sizeof(matrix3x4_t), Matrix);
 
	return Vector(Matrix.m_flMatVal[0][3] + origin.x, Matrix.m_flMatVal[1][3] + origin.y, Matrix.m_flMatVal[2][3] + origin.z);
}

QAngle Entity::GetSwayAngles()
{
	return *(QAngle*)(buffer + OFFSET_BREATH_ANGLES);
}

QAngle Entity::GetViewAngles()
{
	return *(QAngle*)(buffer + OFFSET_VIEWANGLES);
}

Vector Entity::GetViewAnglesV()
{
	return *(Vector*)(buffer + OFFSET_VIEWANGLES);
}

float Entity::GetYaw()
{
	float yaw = 0;
	apex_mem.Read<float>(ptr + OFFSET_YAW, yaw);

	if (yaw < 0)
    	yaw += 360;
	yaw += 90;
	if (yaw > 360)
    	yaw -= 360;
		
	return yaw;
}

bool Entity::isZooming()
{
	return *(int*)(buffer + OFFSET_ZOOMING) == 1;
}

void Entity::SetViewAngles(SVector angles)
{
	apex_mem.Write<SVector>(ptr + OFFSET_VIEWANGLES, angles);
}

void Entity::SetViewAngles(QAngle& angles)
{
	SetViewAngles(SVector(angles));
}

Vector Entity::GetCamPos()
{
	return *(Vector*)(buffer + OFFSET_CAMERAPOS);
}

QAngle Entity::GetRecoil()
{
	return *(QAngle*)(buffer + OFFSET_AIMPUNCH);
}

float CalculateFov(Entity& from, Entity& target)
{
	QAngle ViewAngles = from.GetSwayAngles();
	Vector LocalCamera = from.GetCamPos();
	Vector EntityPosition = target.getPosition();
	QAngle Angle = Math::CalcAngle(LocalCamera, EntityPosition);
	return Math::GetFov(ViewAngles, Angle);
}

QAngle CalculateBestBoneAim(Entity& from, uintptr_t t, float max_fov, float smooth)
{
	Entity target = getEntity(t);
	if(firing_range)
	{
		if (!target.isAlive())
		{
			return QAngle(0, 0, 0);
		}
	}
	else
	{
		if (!target.isAlive() || target.isKnocked())
		{
			return QAngle(0, 0, 0);
		}
	}
	
	Vector LocalCamera = from.GetCamPos();
	Vector TargetBonePosition = target.getBonePositionByHitbox(bone);
	QAngle CalculatedAngles = QAngle(0, 0, 0);
	
	WeaponXEntity curweap = WeaponXEntity();
	curweap.update(from.ptr);
	float BulletSpeed = curweap.get_projectile_speed();
	float BulletGrav = curweap.get_projectile_gravity();
	float zoom_fov = curweap.get_zoom_fov();

	if (zoom_fov != 0.0f && zoom_fov != 1.0f)
	{
		max_fov *= zoom_fov/90.0f;
	}
	
	//more accurate prediction
	if (BulletSpeed > 1.f)
	{
		PredictCtx Ctx;
		Ctx.StartPos = LocalCamera;
		Ctx.TargetPos = TargetBonePosition; 
		Ctx.BulletSpeed = BulletSpeed - (BulletSpeed*smoothpred);
		Ctx.BulletGravity = BulletGrav + (BulletGrav*smoothpred2);
		Ctx.TargetVel = target.getAbsVelocity();

		if (BulletPredict(Ctx))
			CalculatedAngles = QAngle{Ctx.AimAngles.x, Ctx.AimAngles.y, 0.f};
	}

	if (CalculatedAngles == QAngle(0, 0, 0))
	CalculatedAngles = Math::CalcAngle(LocalCamera, TargetBonePosition);
	QAngle ViewAngles = from.GetViewAngles();
	QAngle SwayAngles = from.GetSwayAngles();
	//remove sway and recoil
	if(aim_no_recoil)
		CalculatedAngles-=SwayAngles-ViewAngles;
	Math::NormalizeAngles(CalculatedAngles);
	QAngle Delta = CalculatedAngles - ViewAngles;
	double fov = Math::GetFov(SwayAngles, CalculatedAngles);
	if (fov > max_fov) //add deadzone evnt
	{
		return QAngle(0, 0, 0);
	}

	// limit the rotation of the aimbot
	if (Delta.y > MaxRotation) {
		Delta.y = MaxRotation;
	} else if (Delta.y < -MaxRotation) {
		Delta.y = -MaxRotation;
	}
	if (Delta.x > MaxRotation) {
		Delta.x = MaxRotation;
	} else if (Delta.x < -MaxRotation) {
		Delta.x = -MaxRotation;
	}

	Math::NormalizeAngles(Delta);
	

	QAngle SmoothedAngles = ViewAngles + Delta/smooth;
	return SmoothedAngles;
}

Entity getEntity(uintptr_t ptr)
{
	Entity entity = Entity();
	entity.ptr = ptr;
	apex_mem.ReadArray<uint8_t>(ptr, entity.buffer, sizeof(entity.buffer));
	return entity;
}

bool WorldToScreen(Vector from, float* m_vMatrix, int targetWidth, int targetHeight, Vector& to)
{
	float w = m_vMatrix[12] * from.x + m_vMatrix[13] * from.y + m_vMatrix[14] * from.z + m_vMatrix[15];

	if (w < 0.01f) return false;

	to.x = m_vMatrix[0] * from.x + m_vMatrix[1] * from.y + m_vMatrix[2] * from.z + m_vMatrix[3];
	to.y = m_vMatrix[4] * from.x + m_vMatrix[5] * from.y + m_vMatrix[6] * from.z + m_vMatrix[7];

	float invw = 1.0f / w;
	to.x *= invw;
	to.y *= invw;

	float x = targetWidth / 2;
	float y = targetHeight / 2;

	x += 0.5 * to.x * targetWidth + 0.5;
	y -= 0.5 * to.y * targetHeight + 0.5;

	to.x = x;
	to.y = y;
	to.z = 0;
	return true;
}

void WeaponXEntity::update(uint64_t LocalPlayer)
{
    extern uint64_t g_Base;
	uint64_t entitylist = g_Base + OFFSET_ENTITYLIST;
	uint64_t wephandle = 0;
    apex_mem.Read<uint64_t>(LocalPlayer + OFFSET_WEAPON, wephandle);
	
	wephandle &= 0xffff;

	uint64_t wep_entity = 0;
    apex_mem.Read<uint64_t>(entitylist + (wephandle << 5), wep_entity);

	projectile_speed = 0;
    apex_mem.Read<float>(wep_entity + OFFSET_BULLET_SPEED, projectile_speed);
	projectile_scale = 0;
    apex_mem.Read<float>(wep_entity + OFFSET_BULLET_SCALE, projectile_scale);
	zoom_fov = 0;
    apex_mem.Read<float>(wep_entity + OFFSET_ZOOM_FOV, zoom_fov);
		ammo = 0;
    apex_mem.Read<int>(wep_entity + OFFSET_AMMO, ammo);
}

float WeaponXEntity::get_projectile_speed()
{
	return projectile_speed;
}

float WeaponXEntity::get_projectile_gravity()
{
	return 750.0f * projectile_scale;
}

float WeaponXEntity::get_zoom_fov()
{
	return zoom_fov;
}

int WeaponXEntity::get_ammo()
{
	return ammo;
}