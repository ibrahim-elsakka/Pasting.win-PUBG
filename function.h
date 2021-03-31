#pragma once

#include <string>

using namespace std;
int Width = GetSystemMetrics(SM_CXSCREEN);
int Height = GetSystemMetrics(SM_CYSCREEN);
DWORD ScreenCenterX = NULL;
DWORD ScreenCenterY = NULL;

#define DecryptData(argv)	fnDecryptFunctoin(Tmpadd, argv)
typedef int64_t(__fastcall* DecryptFunctoin)(uintptr_t key, uintptr_t argv);
DecryptFunctoin fnDecryptFunctoin = NULL;
uint64_t GNamesAddress;
uint64_t Tmpadd;
uint64_t LocalPawn;
int64_t CameraManagerAddress;

string GetGNamesByObjID(int32_t ObjectID)
{
	int64_t fNamePtr = read<int64_t>(GNamesAddress + int(ObjectID / 0x3E58) * 0x8);
	int64_t fName = read<int64_t>(fNamePtr + int(ObjectID % 0x3E58) * 0x8);
	char pBuffer[64] = { NULL };
	readwtf(fName + 0x10, pBuffer, sizeof(pBuffer));
	return string(pBuffer);
}
void Decrpyto() {

	int64_t DecryptPtr = read<int64_t>(m_base + 0x5F6E828);
	while (!DecryptPtr)
	{
		DecryptPtr = read<int64_t>(m_base + 0x5F6E828);
		Sleep(1000);
	}
	int32_t Tmp1Add = read<int32_t>(DecryptPtr + 3);
	Tmpadd = Tmp1Add + DecryptPtr + 7;
	unsigned char ShellcodeBuff[1024] = { NULL };
	ShellcodeBuff[0] = 0x90;
	ShellcodeBuff[1] = 0x90;
	readwtf(DecryptPtr, &ShellcodeBuff[2], sizeof(ShellcodeBuff) - 2);
	ShellcodeBuff[2] = 0x48;
	ShellcodeBuff[3] = 0x8B;
	ShellcodeBuff[4] = 0xC1;
	ShellcodeBuff[5] = 0x90;
	ShellcodeBuff[6] = 0x90;
	ShellcodeBuff[7] = 0x90;
	ShellcodeBuff[8] = 0x90;
	fnDecryptFunctoin = (DecryptFunctoin)VirtualAlloc(NULL, sizeof(ShellcodeBuff) + 4, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	RtlCopyMemory((LPVOID)fnDecryptFunctoin, (LPVOID)ShellcodeBuff, sizeof(ShellcodeBuff));

	GNamesAddress = DecryptData(read<int64_t>(DecryptData(read<int64_t>(m_base + 0x77AA738 - 0x20))));
	printf("1 [%s]\n", GetGNamesByObjID(1).c_str());
	printf("2 [%s]\n", GetGNamesByObjID(2).c_str());
	printf("3 [%s]\n", GetGNamesByObjID(3).c_str());

	int64_t pUWorld = DecryptData(read<int64_t>(m_base + 0x78484D8));
	int64_t PersistentLevel = DecryptData(read<int64_t>(pUWorld + 0x48));
	int64_t ActorsArray = DecryptData(read<int64_t>(PersistentLevel + 0xF0));
	int32_t ActorCount = read<int32_t>(ActorsArray + 0x08);

	printf("Over\n");
}
int DecryptId(int input)
{
	return __ROL4__(input ^ 0xC5A2FB0F, 14) ^ (__ROL4__(input ^ 0xC5A2FB0F, 14) << 16) ^ 0xA0DD6E64;
}

glm::vec3 get_obj_pos(uint64_t entity)
{
	uint64_t rootcomponent = DecryptData(read<uint64_t>(entity + 0x148));

	if (!rootcomponent)
		return {};

	return read<glm::vec3>(rootcomponent + 0x2A0);
}
std::uint32_t get_player_health(uint64_t entity)
{
	const auto player_health = read<float>(entity + 0x1874);

	if (player_health <= 0.8f)
		return 0;

	return std::lround(player_health);
}
std::wstring get_player_name(uint64_t entity)
{
	uint64_t player_name_ptr = read<uint64_t>(entity + 0xDC0);

	if (!player_name_ptr)
		return {};

	return read_unicode(player_name_ptr, 24);
}

std::vector<uint64_t> PUBGLitePlayerList;

D3DXMATRIX Matrix(glm::vec3 rot, glm::vec3 origin = glm::vec3(0, 0, 0))
{
	float radPitch = (rot.x * float(M_PI) / 180.f);
	float radYaw = (rot.y * float(M_PI) / 180.f);
	float radRoll = (rot.z * float(M_PI) / 180.f);

	float SP = sinf(radPitch);
	float CP = cosf(radPitch);
	float SY = sinf(radYaw);
	float CY = cosf(radYaw);
	float SR = sinf(radRoll);
	float CR = cosf(radRoll);

	D3DMATRIX matrix;
	matrix.m[0][0] = CP * CY;
	matrix.m[0][1] = CP * SY;
	matrix.m[0][2] = SP;
	matrix.m[0][3] = 0.f;

	matrix.m[1][0] = SR * SP * CY - CR * SY;
	matrix.m[1][1] = SR * SP * SY + CR * CY;
	matrix.m[1][2] = -SR * CP;
	matrix.m[1][3] = 0.f;

	matrix.m[2][0] = -(CR * SP * CY + SR * SY);
	matrix.m[2][1] = CY * SR - CR * SP * SY;
	matrix.m[2][2] = CR * CP;
	matrix.m[2][3] = 0.f;

	matrix.m[3][0] = origin.x;
	matrix.m[3][1] = origin.y;
	matrix.m[3][2] = origin.z;
	matrix.m[3][3] = 1.f;

	return matrix;
}
glm::vec3 WorldToScreenX(const glm::vec3& WorldLocation, glm::vec3 RotationX, glm::vec3 Location, float Fov)
{
	glm::vec3 Screenlocation = glm::vec3(0, 0, 0);

	glm::vec3 Rotation = RotationX;
	D3DMATRIX tempMatrix = Matrix(Rotation);

	glm::vec3 vAxisX, vAxisY, vAxisZ;

	vAxisX = glm::vec3(tempMatrix.m[0][0], tempMatrix.m[0][1], tempMatrix.m[0][2]);
	vAxisY = glm::vec3(tempMatrix.m[1][0], tempMatrix.m[1][1], tempMatrix.m[1][2]);
	vAxisZ = glm::vec3(tempMatrix.m[2][0], tempMatrix.m[2][1], tempMatrix.m[2][2]);

	glm::vec3 vDelta = WorldLocation - Location;
	glm::vec3 vTransformed = glm::vec3(glm::dot(vDelta, vAxisY), glm::dot(vDelta, vAxisZ), glm::dot(vDelta, vAxisX));

	if (vTransformed.z < 1.f)
		vTransformed.z = 1.f;

	float FovAngle = Fov;

	//printf("FovAngle :%f\n", FovAngle);
	float ScreenCenterX = Width / 2.0f;
	float ScreenCenterY = Height / 2.0f;

	Screenlocation.x = ScreenCenterX + vTransformed.x * (ScreenCenterX / tanf(FovAngle * (float)M_PI / 360.f)) / vTransformed.z;
	Screenlocation.y = ScreenCenterY - vTransformed.y * (ScreenCenterX / tanf(FovAngle * (float)M_PI / 360.f)) / vTransformed.z;

	return Screenlocation;
}

struct FQuat
{
	float x;
	float y;
	float z;
	float w;
};
struct FTransform
{
	FQuat rot;
	glm::vec3 translation;
	char pad[4];
	glm::vec3 scale;
	char pad1[4];
	D3DMATRIX ToMatrixWithScale()
	{
		D3DMATRIX m;
		m._41 = translation.x;
		m._42 = translation.y;
		m._43 = translation.z;

		float x2 = rot.x + rot.x;
		float y2 = rot.y + rot.y;
		float z2 = rot.z + rot.z;

		float xx2 = rot.x * x2;
		float yy2 = rot.y * y2;
		float zz2 = rot.z * z2;
		m._11 = (1.0f - (yy2 + zz2)) * scale.x;
		m._22 = (1.0f - (xx2 + zz2)) * scale.y;
		m._33 = (1.0f - (xx2 + yy2)) * scale.z;

		float yz2 = rot.y * z2;
		float wx2 = rot.w * x2;
		m._32 = (yz2 - wx2) * scale.z;
		m._23 = (yz2 + wx2) * scale.y;

		float xy2 = rot.x * y2;
		float wz2 = rot.w * z2;
		m._21 = (xy2 - wz2) * scale.y;
		m._12 = (xy2 + wz2) * scale.x;

		float xz2 = rot.x * z2;
		float wy2 = rot.w * y2;
		m._31 = (xz2 + wy2) * scale.z;
		m._13 = (xz2 - wy2) * scale.x;

		m._14 = 0.0f;
		m._24 = 0.0f;
		m._34 = 0.0f;
		m._44 = 1.0f;

		return m;
	}
};
D3DMATRIX MatrixMultiplication(D3DMATRIX pM1, D3DMATRIX pM2)
{
	D3DMATRIX pOut;
	pOut._11 = pM1._11 * pM2._11 + pM1._12 * pM2._21 + pM1._13 * pM2._31 + pM1._14 * pM2._41;
	pOut._12 = pM1._11 * pM2._12 + pM1._12 * pM2._22 + pM1._13 * pM2._32 + pM1._14 * pM2._42;
	pOut._13 = pM1._11 * pM2._13 + pM1._12 * pM2._23 + pM1._13 * pM2._33 + pM1._14 * pM2._43;
	pOut._14 = pM1._11 * pM2._14 + pM1._12 * pM2._24 + pM1._13 * pM2._34 + pM1._14 * pM2._44;
	pOut._21 = pM1._21 * pM2._11 + pM1._22 * pM2._21 + pM1._23 * pM2._31 + pM1._24 * pM2._41;
	pOut._22 = pM1._21 * pM2._12 + pM1._22 * pM2._22 + pM1._23 * pM2._32 + pM1._24 * pM2._42;
	pOut._23 = pM1._21 * pM2._13 + pM1._22 * pM2._23 + pM1._23 * pM2._33 + pM1._24 * pM2._43;
	pOut._24 = pM1._21 * pM2._14 + pM1._22 * pM2._24 + pM1._23 * pM2._34 + pM1._24 * pM2._44;
	pOut._31 = pM1._31 * pM2._11 + pM1._32 * pM2._21 + pM1._33 * pM2._31 + pM1._34 * pM2._41;
	pOut._32 = pM1._31 * pM2._12 + pM1._32 * pM2._22 + pM1._33 * pM2._32 + pM1._34 * pM2._42;
	pOut._33 = pM1._31 * pM2._13 + pM1._32 * pM2._23 + pM1._33 * pM2._33 + pM1._34 * pM2._43;
	pOut._34 = pM1._31 * pM2._14 + pM1._32 * pM2._24 + pM1._33 * pM2._34 + pM1._34 * pM2._44;
	pOut._41 = pM1._41 * pM2._11 + pM1._42 * pM2._21 + pM1._43 * pM2._31 + pM1._44 * pM2._41;
	pOut._42 = pM1._41 * pM2._12 + pM1._42 * pM2._22 + pM1._43 * pM2._32 + pM1._44 * pM2._42;
	pOut._43 = pM1._41 * pM2._13 + pM1._42 * pM2._23 + pM1._43 * pM2._33 + pM1._44 * pM2._43;
	pOut._44 = pM1._41 * pM2._14 + pM1._42 * pM2._24 + pM1._43 * pM2._34 + pM1._44 * pM2._44;

	return pOut;
}
FTransform GetBoneIndex(uint64_t mesh, int64_t index)
{
	uint64_t bonearray = read<uint64_t>(mesh + 0xB28);
	if (!bonearray) bonearray = read<uint64_t>(mesh + 0xB28);
	return read<FTransform>(bonearray + (index * 0x30));
}
glm::vec3 GetBoneWithRotation(DWORD_PTR mesh, int id)
{
	FTransform bone = GetBoneIndex(mesh, id);
	FTransform ComponentToWorld = read<FTransform>(mesh + 0x2B0);

	D3DMATRIX Matrix;
	Matrix = MatrixMultiplication(bone.ToMatrixWithScale(), ComponentToWorld.ToMatrixWithScale());

	return glm::vec3(Matrix._41, Matrix._42, Matrix._43);
}

typedef struct _TslEntity
{
	uint64_t pObjPointer;
	int ID;

	uint64_t Mesh;
	uint64_t PlayerController;
	uint64_t RootComponent;

}TslEntity;
vector<TslEntity> entityList;

void cache()
{
	while (true)
	{
		vector<TslEntity> tmpList;

		int64_t pUWorld = DecryptData(read<int64_t>(m_base + 0x78484D8));
		//printf("Uworld ->%p \n", pUWorld);
		int64_t OwningGameInstance = DecryptData(read<int64_t>(pUWorld + 0x8C8));
		//printf("OwningGameInstance ->%p \n", OwningGameInstance);
		int64_t LocalPlayers = read<int64_t>(OwningGameInstance + 0x50);
		int64_t ULocalPlayer = DecryptData(read<int64_t>(LocalPlayers));
		//printf("ULocalPlayer ->%p \n", ULocalPlayer);
		int64_t PlayerController = DecryptData(read<int64_t>(ULocalPlayer + 0x48));
		//printf("PlayerController ->%p \n", PlayerController);

		CameraManagerAddress = read<int64_t>(PlayerController + 0x4B8);

		LocalPawn = DecryptData(read<int64_t>(PlayerController + 0x498));

		int64_t PersistentLevel = DecryptData(read<int64_t>(pUWorld + 0x48));
		int64_t ActorsArray = DecryptData(read<int64_t>(PersistentLevel + 0xF0));
		int32_t ActorCount = read<int32_t>(ActorsArray + 0x08);
		//printf("ActorCount ->%i \n", ActorCount);

		for (int32_t i = 0; i < ActorCount; i++)
		{
			int64_t actor = read<int64_t>(read<int64_t>(ActorsArray) + (i * 0x8));

			if (actor == 0x00)
			{
				continue;
			}

			int32_t pObjectID = DecryptId(read<int32_t>(actor + 0x30));
			string pObjName = GetGNamesByObjID(pObjectID);

			uint64_t mesh = read<uint64_t>(actor + 0x4C0);

			const glm::vec3 player_position = get_obj_pos(actor);
			const auto player_health = get_player_health(actor);
			const int player_distance = glm::distance(player_position, get_obj_pos(LocalPawn)) / 100.f;

			if (pObjName.find("PlayerMale_A") != std::string::npos 
				|| pObjName.find("PlayerMale_A_C") != std::string::npos
				|| pObjName.find("PlayerFemale_A") != std::string::npos 
				|| pObjName.find("PlayerFemale_A_C") != std::string::npos) {

				if (mesh != 0x00)
				{
					TslEntity tslEntity{ };
					tslEntity.pObjPointer = actor;
					tslEntity.ID = pObjectID;

					tslEntity.Mesh = mesh;

					if (player_health <= 0) {
						continue;
					}
					if (player_distance > 400)
						continue;

					tmpList.push_back(tslEntity);
				}
			}
		}

		entityList = tmpList;

		Sleep(100);
	}
}

void ESP() {

	auto entityListCopy = entityList;
	RGBA ESPColor = { colors::espcol[0] * 255, colors::espcol[1] * 255, colors::espcol[2] * 255, 255 };

	if (!entityListCopy.empty())
	{
		for (unsigned long i = 0; i < entityListCopy.size(); ++i)
		{
			TslEntity entity = entityListCopy[i];

			if (entity.pObjPointer == LocalPawn)
			{
				continue;
			}

			const glm::vec3 player_position = get_obj_pos(entity.pObjPointer);

			uint64_t mesh = entity.Mesh;
			glm::vec3 Rotation = read<glm::vec3>(CameraManagerAddress + 0x107C);
			glm::vec3 Location = read<glm::vec3>(CameraManagerAddress + 0x106C);
			float FovAngle = read<float>(CameraManagerAddress + 0x1098);


			glm::vec3 BoxHead = GetBoneWithRotation(mesh, 15);
			glm::vec3 Root = GetBoneWithRotation(mesh, 0);
			glm::vec3 player_screen = WorldToScreenX(player_position, Rotation, Location, FovAngle);
			glm::vec3 head_screen = WorldToScreenX(glm::vec3(BoxHead.x, BoxHead.y + 0.3, BoxHead.z), Rotation, Location, FovAngle);
			glm::vec3 normal_head = WorldToScreenX(BoxHead, Rotation, Location, FovAngle);
			glm::vec3 foot = WorldToScreenX(Root, Rotation, Location, FovAngle);

			int64_t pPlayerState = DecryptData(read<int64_t>(entity.pObjPointer + 0x438));
			int32_t pPlayerKillCount = read<int32_t>(pPlayerState + 0x75C);
			int32_t TeamNumber = read<int32_t>(entity.pObjPointer + 0xC60);

			string pObjName = GetGNamesByObjID(entity.ID);

			const auto player_health = get_player_health(entity.pObjPointer);
			const auto player_name = get_player_name(entity.pObjPointer);

			const int player_distance = glm::distance(player_position, get_obj_pos(LocalPawn)) / 100.f;

			float BoxHeight = (float)(foot.y - normal_head.y);
			float BoxWidth = BoxHeight / 3.0f;

			if (pPlayerKillCount < 0 || pPlayerKillCount > 100) pPlayerKillCount = 0;

			if (pObjName.find("PlayerMale_A") != std::string::npos
				|| pObjName.find("PlayerMale_A_C") != std::string::npos
				|| pObjName.find("PlayerFemale_A") != std::string::npos
				|| pObjName.find("PlayerFemale_A_C") != std::string::npos || mesh != 0x00)
			{

				if (visuals::name)
				{
					char name[64];
					sprintf_s(name, " [ %dm ] - %S ", player_distance, player_name.c_str());
					DrawString(menu::font_size, head_screen.x, head_screen.y - 5, &Col.white_, true, true, name);
				}

				if (visuals::name)
				{
					char name[64];
					sprintf_s(name, " [%.2d] K:(%d) ", TeamNumber, pPlayerKillCount);
					DrawString(menu::font_size, foot.x, foot.y + 20, &Col.white_, true, true, name);
				}

				if (visuals::box)
				{
					if (visuals::boxMode == 0 || visuals::boxMode == 1)
					{
						if (visuals::outline)
						{
							DrawNormalBox(player_screen.x - BoxWidth / 2 + 1, head_screen.y, BoxWidth, BoxHeight, menu::box_thick, &Col.black);
							DrawNormalBox(player_screen.x - BoxWidth / 2 - 1, head_screen.y, BoxWidth, BoxHeight, menu::box_thick, &Col.black);
							DrawNormalBox(player_screen.x - BoxWidth / 2, head_screen.y + 1, BoxWidth, BoxHeight, menu::box_thick, &Col.black);
							DrawNormalBox(player_screen.x - BoxWidth / 2, head_screen.y - 1, BoxWidth, BoxHeight, menu::box_thick, &Col.black);
						}
						DrawNormalBox(player_screen.x - (BoxWidth / 2), head_screen.y, BoxWidth, BoxHeight, menu::box_thick, &ESPColor);
					}
					if (visuals::boxMode == 2 || visuals::boxMode == 3)
					{
						if (visuals::outline)
						{
							DrawCornerBox(player_screen.x - BoxWidth / 2 + 1, head_screen.y, BoxWidth, BoxHeight, menu::box_thick, &Col.black);
							DrawCornerBox(player_screen.x - BoxWidth / 2 - 1, head_screen.y, BoxWidth, BoxHeight, menu::box_thick, &Col.black);
							DrawCornerBox(player_screen.x - BoxWidth / 2, head_screen.y + 1, BoxWidth, BoxHeight, menu::box_thick, &Col.black);
							DrawCornerBox(player_screen.x - BoxWidth / 2, head_screen.y - 1, BoxWidth, BoxHeight, menu::box_thick, &Col.black);
						}
						DrawCornerBox(player_screen.x - (BoxWidth / 2), head_screen.y, BoxWidth, BoxHeight, menu::box_thick, &ESPColor);
					}

					if (visuals::boxMode == 1 || visuals::boxMode == 3) menu::box_thick = 1,
						DrawFilledRect(player_screen.x - (BoxWidth / 2), head_screen.y, BoxWidth, BoxHeight, &Col.filled);
				}

				if (visuals::health)
				{
					if (visuals::healthMode == 0 || visuals::healthMode == 3)
						DrawLeftProgressBar(player_screen.x, head_screen.y, BoxWidth, BoxHeight, 1, player_health);
					if (visuals::healthMode == 1 || visuals::healthMode == 4)
						DrawRightProgressBar(player_screen.x, head_screen.y, BoxWidth, BoxHeight, 1, player_health);
					if (visuals::healthMode == 2 || visuals::healthMode == 3 || visuals::healthMode == 4)
					{
						char vida[100];
						sprintf(vida, "Health: %d ", player_health);
						DrawString(12, player_screen.x, player_screen.y + 5, &Col.white, true, true, vida);
					}
				}

				if (visuals::lines)
				{
					if (visuals::lineMode == 0)
						DrawLine(ScreenCenterX, 0, head_screen.x, head_screen.y, &ESPColor, 1);
					if (visuals::lineMode == 1)
						DrawLine(ScreenCenterX, ScreenCenterY, head_screen.x, head_screen.y, &ESPColor, 1);
					if (visuals::lineMode == 2)
						DrawLine(ScreenCenterX, ScreenCenterY + ScreenCenterY + 80, head_screen.x, head_screen.y, &ESPColor, 1);
				}

				if (visuals::show_head)
					DrawCircle(head_screen.x, head_screen.y, BoxHeight / 25, &Col.green, 30);

				if (visuals::skel) {

					glm::vec3 vHeadBone = GetBoneWithRotation(mesh, 15);
					glm::vec3 vHeadBoneOut = WorldToScreenX(vHeadBone, Rotation, Location, FovAngle);

					glm::vec3 vHip = GetBoneWithRotation(mesh, 1);
					glm::vec3 vHipOut = WorldToScreenX(vHip, Rotation, Location, FovAngle);

					glm::vec3 vNeck = GetBoneWithRotation(mesh, 5);
					glm::vec3 vNeckOut = WorldToScreenX(vNeck, Rotation, Location, FovAngle);

					glm::vec3 vUpperArmLeft = GetBoneWithRotation(mesh, 88);
					glm::vec3 vUpperArmLeftOut = WorldToScreenX(vUpperArmLeft, Rotation, Location, FovAngle);

					glm::vec3 vUpperArmRight = GetBoneWithRotation(mesh, 115);
					glm::vec3 vUpperArmRightOut = WorldToScreenX(vUpperArmRight, Rotation, Location, FovAngle);

					glm::vec3 vLeftHand = GetBoneWithRotation(mesh, 89);
					glm::vec3 vLeftHandOut = WorldToScreenX(vLeftHand, Rotation, Location, FovAngle);

					glm::vec3 vRightHand = GetBoneWithRotation(mesh, 116);
					glm::vec3 vRightHandOut = WorldToScreenX(vRightHand, Rotation, Location, FovAngle);

					glm::vec3 vLeftHand1 = GetBoneWithRotation(mesh, 90);
					glm::vec3 vLeftHandOut1 = WorldToScreenX(vLeftHand1, Rotation, Location, FovAngle);

					glm::vec3 vRightHand1 = GetBoneWithRotation(mesh, 117);
					glm::vec3 vRightHandOut1 = WorldToScreenX(vRightHand1, Rotation, Location, FovAngle);

					glm::vec3 vRightThigh = GetBoneWithRotation(mesh, 174);
					glm::vec3 vRightThighOut = WorldToScreenX(vRightThigh, Rotation, Location, FovAngle);

					glm::vec3 vLeftThigh = GetBoneWithRotation(mesh, 168);
					glm::vec3 vLeftThighOut = WorldToScreenX(vLeftThigh, Rotation, Location, FovAngle);

					glm::vec3 vRightCalf = GetBoneWithRotation(mesh, 175);
					glm::vec3 vRightCalfOut = WorldToScreenX(vRightCalf, Rotation, Location, FovAngle);

					glm::vec3 vLeftCalf = GetBoneWithRotation(mesh, 169);
					glm::vec3 vLeftCalfOut = WorldToScreenX(vLeftCalf, Rotation, Location, FovAngle);

					glm::vec3 vLeftFoot = GetBoneWithRotation(mesh, 170);
					glm::vec3 vLeftFootOut = WorldToScreenX(vLeftFoot, Rotation, Location, FovAngle);

					glm::vec3 vRightFoot = GetBoneWithRotation(mesh, 176);
					glm::vec3 vRightFootOut = WorldToScreenX(vRightFoot, Rotation, Location, FovAngle);

					glm::vec3 Pelvis = GetBoneWithRotation(mesh, 1);
					glm::vec3 PelvisOut = WorldToScreenX(Pelvis, Rotation, Location, FovAngle);


					DrawLine(vNeckOut.x, vNeckOut.y, vHeadBoneOut.x, vHeadBoneOut.y, &Col.green, menu::box_thick);
					DrawLine(PelvisOut.x, PelvisOut.y, vNeckOut.x, vNeckOut.y, &Col.green, menu::box_thick);

					DrawLine(vUpperArmLeftOut.x, vUpperArmLeftOut.y, vNeckOut.x, vNeckOut.y, &Col.green, menu::box_thick);
					DrawLine(vUpperArmRightOut.x, vUpperArmRightOut.y, vNeckOut.x, vNeckOut.y, &Col.green, menu::box_thick);

					DrawLine(vLeftHandOut.x, vLeftHandOut.y, vUpperArmLeftOut.x, vUpperArmLeftOut.y, &Col.green, menu::box_thick);
					DrawLine(vRightHandOut.x, vRightHandOut.y, vUpperArmRightOut.x, vUpperArmRightOut.y, &Col.green, menu::box_thick);

					DrawLine(vLeftHandOut.x, vLeftHandOut.y, vLeftHandOut1.x, vLeftHandOut1.y, &Col.green, menu::box_thick);
					DrawLine(vRightHandOut.x, vRightHandOut.y, vRightHandOut1.x, vRightHandOut1.y, &Col.green, menu::box_thick);

					DrawLine(vLeftThighOut.x, vLeftThighOut.y, vHipOut.x, vHipOut.y, &Col.green, menu::box_thick);
					DrawLine(vRightThighOut.x, vRightThighOut.y, vHipOut.x, vHipOut.y, &Col.green, menu::box_thick);

					DrawLine(vLeftCalfOut.x, vLeftCalfOut.y, vLeftThighOut.x, vLeftThighOut.y, &Col.green, menu::box_thick);
					DrawLine(vRightCalfOut.x, vRightCalfOut.y, vRightThighOut.x, vRightThighOut.y, &Col.green, menu::box_thick);

					DrawLine(vLeftFootOut.x, vLeftFootOut.y, vLeftCalfOut.x, vLeftCalfOut.y, &Col.green, menu::box_thick);
					DrawLine(vRightFootOut.x, vRightFootOut.y, vRightCalfOut.x, vRightCalfOut.y, &Col.green, menu::box_thick);

				}
			}
		}
	}
}