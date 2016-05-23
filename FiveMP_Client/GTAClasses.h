#ifndef GTACLASSES_H
#define GTACLASSES_H

#include "StdInc.h"

class CPrimaryAmmoCount;
class CAmmoCount;
class CAmmoInfo;
class CWeaponInfo;
class CPedWeaponManager;
class CModelInfo;
class CObjectNavigation;
class CWeaponObject;
class CInventory;
class CPlayerInfo;
class CPed;

class CPrimaryAmmoCount
{
public:
	char pad_0x0000[0x18]; //0x0000
	__int32 AmmoCount; //0x0018 

};//Size=0x0040

class CAmmoCount
{
public:
	CPrimaryAmmoCount* _PrimaryAmmoCount; //0x0000 
	char pad_0x0008[0x38]; //0x0008

};//Size=0x0040

class CAmmoInfo
{
public:
	char pad_0x0000[0x8]; //0x0000
	CAmmoCount* _AmmoCount; //0x0008 
	char pad_0x0010[0x30]; //0x0010

};//Size=0x0040

class CWorld
{
public:
	char pad_0x0000[0x8]; //0x0000
	CPed* _Local; //0x0008 
	char pad_0x0010[0x30]; //0x0010

	static CWorld* Singleton()
	{
		return *(CWorld**)((uintptr_t)GetModuleHandle(0) + 0x3D0B4500); //World is 0x3D0B4500
	}

	bool GetLocal(CPed*& Local)
	{
		if (!this)
			return 0;

		if (!this->_Local)
			return 0;

		Local = _Local;

		return 1;
	}

};//Size=0x0040

class CWeaponInfo
{
public:
	char pad_0x0000[0x48]; //0x0000
	CAmmoInfo* _AmmoInfo; //0x0048 
	char pad_0x0050[0xC]; //0x0050
	float Spread; //0x005C 
	char pad_0x0060[0x38]; //0x0060
	float Damage; //0x0098 
	char pad_0x009C[0x24]; //0x009C
	float MinImpulse; //0x00C0 
	float MaxImpulse; //0x00C4 
	char pad_0x00C8[0x34]; //0x00C8
	float Velocity; //0x00FC 
	__int32 BulletsPeShot; //0x0100 
	char pad_0x0104[0x18]; //0x0104
	float TimeToShoot; //0x011C 
	char pad_0x0120[0x194]; //0x0120
	float Recoil; //0x02B4 

};//Size=0x0120

class CPedWeaponManager
{
public:
	char pad_0x0000[0x20]; //0x0000
	CWeaponInfo* _WeaponInfo; //0x0020 
	char pad_0x0028[0x60]; //0x0028
	void* _CurrentWeapon; //0x0088 
	char pad_0x0090[0x18]; //0x0090

};//Size=0x00A8


class CModelInfo
{
public:
	char pad_0x0000[0x270]; //0x0000
	char Name[1]; //0x0270 

};//Size=0x02B0

class CObjectNavigation
{
public:
	char pad_0x0000[0x50]; //0x0000
	Vector3 Position; //0x0050 
	char pad_0x005C[0x24]; //0x005C

};//Size=0x0080

class CWeaponObject
{
public:
	char _0x0000[8];
	CAmmoInfo* m_pAmmoInfo; //0x0008 
	char _0x0010[16];
	__int32 m_iAmmo; //0x0020 
	char _0x0024[20];

};//Size=0x0038

class CInventory
{
public:
	char _0x0000[72];
	CWeaponObject** WeaponList; //0x0048 

	CWeaponObject* getWeapon(int index){
		return (CWeaponObject*)WeaponList[index];
	}
};//Size=0x0050

class CPlayerInfo
{
public:
	char pad_0x0000[0x7C]; //0x0000
	char Name[64]; //0x007C 
	char pad_0x00BC[0x24]; //0x00BC
	float UnkSpeed; //0x00E0 
	float SwimmingSpeed; //0x00E4 
	float WalkingSpeed; //0x00E8 
	char pad_0x00EC[0x74]; //0x00EC
	CPed* _Ped; //0x0160 
	char pad_0x0168[0x28]; //0x0168
	__int32 FrameFlags; //0x0190 
	char pad_0x0194[0x604]; //0x0194
	__int32 WantedLevel; //0x0798 
	__int32 WantedLevelReal; //0x079C 

	void SetWanted(int Level)
	{
		WantedLevel = Level;
		WantedLevelReal = Level;
	}

};//Size=0x07A0

class CPed
{
public:
	char pad_0x0000[0x20]; //0x0000
	CModelInfo* _ModelInfo; //0x0020 
	char pad_0x0028[0x8]; //0x0028
	CObjectNavigation* _ObjectNavigation; //0x0030 
	char pad_0x0038[0x2C]; //0x0038
	float Width; //0x0064 
	char pad_0x0068[0x28]; //0x0068
	Vector3 Position; //0x0090 
	char pad_0x009C[0x74]; //0x009C
	Vector3 Position2; //0x0110 
	char pad_0x011C[0x4]; //0x011C
	Vector3 Position3; //0x0120 
	char pad_0x012C[0x4]; //0x012C
	float Height; //0x0130 
	char pad_0x0134[0x2C]; //0x0134
	Vector3 Position4; //0x0160 
	char pad_0x016C[0x114]; //0x016C
	float HP; //0x0280 
	float MaxHP; //0x0284 
	char pad_0x0288[0xE00]; //0x0288
	CPlayerInfo* _PlayerInfo; //0x1088 
	CInventory* _Inventory; //0x1090 
	CPedWeaponManager* _WeaponManager; //0x1098 
	char pad_0x10A0[0x3D4]; //0x10A0
	float Armor; //0x1474 
	char pad_0x1478[0x348]; //0x1478

	Vector3 GetHead()
	{
		return Vector3();
	}

	Vector3 GetOrigin()
	{
		return Vector3();
	}

	char* GetModelName()
	{
		if (!this)
			return "null";

		if (!this->_ModelInfo)
			return "null";

		if (!this->_ModelInfo->Name)
			return "null";

		return this->_ModelInfo->Name;
	}

	void SetAmmo(int Amt)
	{
		if (!this)
			return;

		if (!(this->_WeaponManager))
			return;

		if (!(this->_WeaponManager->_WeaponInfo))
			return;

		if (!(this->_WeaponManager->_WeaponInfo->_AmmoInfo))
			return;

		if (!(this->_WeaponManager->_WeaponInfo->_AmmoInfo->_AmmoCount))
			return;

		if (!(this->_WeaponManager->_WeaponInfo->_AmmoInfo->_AmmoCount->_PrimaryAmmoCount))
			return;

		this->_WeaponManager->_WeaponInfo->_AmmoInfo->_AmmoCount->_PrimaryAmmoCount->AmmoCount = Amt;
	}
};//Size=0x17C0
#endif