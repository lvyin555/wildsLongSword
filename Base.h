#pragma once
#include <map>
#include "Base64.h"
#include "MonsterBuff.h"
#include "PlayerBuff.h"
#include "AobAddress.h"
#define _USE_MATH_DEFINES
#define sign(x) (((x) < 0) ? -1 : ((x) > 0))
#define M_PI 3.14159265358979323846
#define GUID_LEN 64

using namespace std;
using namespace loader;

extern "C" long long _stdcall Navigation(float*, float*, float*);

extern "C" void* _stdcall GetRBXPtr(void*);
extern "C" void* _stdcall GetRDIPtr(void*);
extern "C" void* _stdcall GetHitPtr(void*);
extern "C" void* _stdcall SetEDX(float*);


namespace Base {
	//?????
	struct Vector4 {
		float x, y, z, w;
		Vector4(float x = 0, float y = 0, float z = 0, float w = 0) :x(x), y(y), z(z), w(w) { };
	};
	struct Vector3 {
		float x, y, z;
		Vector3(float x = 0, float y = 0, float z = 0) :x(x), y(y), z(z) { };
	};
	struct Vector2 {
		float x, y;
		Vector2(float x = 0, float y = 0) :x(x), y(y) { };
	};
#pragma region ModConfig
	namespace ModConfig {
		//???®∞???
		bool GameDataInit = false;
		bool DrawInit = false;
		bool InitErrInfo = true;
		int InitErrCount = 0;
		vector<string> LuaFiles;
		vector<string> LuaError;
		bool ModConsole = false;
		bool HotKeyEdit = false;
		bool About = false;
		//?????®∞???
		string ModName = "LuaScript";
		string ModAuthor = "Alcedo";
		string ModVersion = "v1.2.3 Dev";
		long long ModBuild = 122005231334;
		string Version = "421470";
	}
#pragma endregion
#pragma region LuaHandle
	namespace LuaHandle {
		struct LuaCodeData {
			string name;
			string code;
			string file;
			bool start;
			bool hotReload;//hotReload????lua????????????????????????????ß’???
			void Update()
			{
				ifstream ifile(file);
				ostringstream buf;
				char ch;
				while (buf && ifile.get(ch))
					buf.put(ch);
				code = buf.str();
				if (string _Tmpy = "--NotHotReload"; string::npos != code.find(_Tmpy))
					hotReload = false;
				if (string _Tmpy = "--Disable"; string::npos != code.find(_Tmpy))
					start = false;
				if (string _Tmpy = "--About"; string::npos != code.find(_Tmpy))
					ModConfig::About = true;
			};
			LuaCodeData(
				string name = "",
				string code = "",
				string file = "",
				bool start = true,
				bool hotReload = true
			) :name(name), code(code), file(file), start(start), hotReload(hotReload) { };
		};
		vector<string> LuaFiles;
		vector<string> LuaError;
		map<string, LuaCodeData> LuaCode;
	}
#pragma endregion
	//??????????
#pragma region BasicGameData
	//??????????
	namespace BasicGameData {
		void* PlayerPlot = nullptr;
		void* PlayerInfoPlot = nullptr;
		void* PlayerDataPlot = nullptr;
		void* MapPlot = nullptr;
		void* GameTimePlot = nullptr;
		void* XboxPadPlot = nullptr;
	}
#pragma endregion
	//???????
#pragma region World
	namespace World {
		//????????
		namespace TempData {
			float t_SetFrameSpeed = -1;
		}
		//????????
		namespace EnvironmentalData {
			struct EnvironmentalData {
				void* Plot = nullptr;
				float CoordinatesX = 0;
				float CoordinatesY = 0;
				float CoordinatesZ = 0;
				int Id = 0;
				int SubId = 0;
				EnvironmentalData(
					void* Plot = nullptr,
					float CoordinatesX = 0,
					float CoordinatesY = 0,
					float CoordinatesZ = 0,
					int Id = 0,
					int SubId = 0)
					:Plot(Plot), CoordinatesX(CoordinatesX), CoordinatesY(CoordinatesY), CoordinatesZ(CoordinatesZ), Id(Id), SubId(SubId) {
				};
			};
			//?????????ß“?
			map<void*, EnvironmentalData> Environmentals;
			//????????
			pair<int, int> Filter(255, 255);
		}
		int MapId = 0;
		string Massage = "";
		map<void*, float> FrameSpeed;
	}
#pragma endregion
	//?????
#pragma region Chronoscope
	namespace Chronoscope {
		struct ChronoscopeData {
			float StartTime = 0;
			float EndTime = 0;
		};

		//????????
		float NowTime = 0;
		//??????ß“?
		map<string, ChronoscopeData> ChronoscopeList;
		//????????(???????????????????)
		static bool AddChronoscope(float duration, string name, bool Overlay = false) {
			if (ChronoscopeList.find(name) == ChronoscopeList.end() || Overlay) {
				ChronoscopeList[name].EndTime = NowTime + duration;
				ChronoscopeList[name].StartTime = NowTime;
				return true;
			}
			else
				return false;
		}
		//????????
		static void DelChronoscope(string name) {
			if (ChronoscopeList.find(name) != ChronoscopeList.end()) {
				ChronoscopeList.erase(name);
			}
		}
		//?????????????
		static bool CheckPresenceChronoscope(string name) {
			if (ChronoscopeList.find(name) != ChronoscopeList.end()) {
				return true;
			}
			return false;
		}
		//?????????????
		static bool CheckChronoscope(string name) {
			if (ChronoscopeList.find(name) != ChronoscopeList.end()) {
				if (ChronoscopeList[name].EndTime < NowTime) {
					DelChronoscope(name);
					return true;
				}
				else
					return false;
			}
			return false;
		}
	}
#pragma endregion
	//????
#pragma region Calculation
	namespace Calculation {
		static Vector3 GetVector(Vector3 p1, Vector3 p2, float l) {
			float a = (p2.x - p1.x);
			float b = (p2.y - p1.y);
			float c = (p2.z - p1.z);
			l = l * l / (a * a + b * b + c * c);
			float k = sqrt(l);
			float newx1 = k * a + p1.x;
			float newy1 = k * b + p1.y;
			float newz1 = k * c + p1.z;
			return Vector3(newx1, newy1, newz1);
		}
		static Vector2 GetExtensionVector2D(Vector2 Coordinate, float r, float angle) {
			float x, y;
			x = Coordinate.x + r * cos((4 * atan(1.0) / 180 * angle));
			y = Coordinate.y + r * sin((4 * atan(1.0) / 180 * angle));
			return Vector2(x, y);
		}
		static float myRander(float min, float max)
		{
			std::random_device rd;
			std::mt19937 eng(rd());
			std::uniform_real_distribution<float> dist(min, max);
			return dist(eng);
		}
		static float DistanceBetweenTwoCoordinates(Vector3 point1, Vector3 point2) {
			float RangeDistance = sqrt((point1.x - point2.x) * (point1.x - point2.x)) + sqrt((point1.z - point2.z) * (point1.z - point2.z));
			RangeDistance = sqrt((RangeDistance * RangeDistance) + sqrt((point1.y - point2.y) * (point1.y - point2.y)));
			return RangeDistance;
		}
		static string GetUUID() {
			char buffer[GUID_LEN] = { 0 };
			GUID guid;
			if (CoCreateGuid(&guid))
			{
				return "create guid error";
			}
			_snprintf_s(buffer, sizeof(buffer),
				"%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X",
				guid.Data1, guid.Data2, guid.Data3,
				guid.Data4[0], guid.Data4[1], guid.Data4[2],
				guid.Data4[3], guid.Data4[4], guid.Data4[5],
				guid.Data4[6], guid.Data4[7]);
			return buffer;
		}
		Vector3 inline ToEulerAngles(Vector4 quaternion)
		{
			Vector3 eulerangles;

			// roll (x-axis rotation)
			float sinr_cosp = 2 * (quaternion.w * quaternion.x + quaternion.y * quaternion.z);
			float cosr_cosp = 1 - 2 * (quaternion.x * quaternion.x + quaternion.y * quaternion.y);
			eulerangles.x = std::atan2(sinr_cosp, cosr_cosp);

			// pitch (y-axis rotation)
			float sinp = 2 * (quaternion.w * quaternion.y - quaternion.z * quaternion.x);
			if (std::abs(sinp) >= 1)
				eulerangles.y = std::copysign(M_PI / 2, sinp);
			else
				eulerangles.y = std::asin(sinp);

			// yaw (z-axis rotation)
			float siny_cosp = 2 * (quaternion.w * quaternion.z + quaternion.x * quaternion.y);
			float cosy_cosp = 1 - 2 * (quaternion.y * quaternion.y + quaternion.z * quaternion.z);
			eulerangles.z = std::atan2(siny_cosp, cosy_cosp);

			return eulerangles;
		}
		Vector4 inline ToQuaternion(Vector3 eulerangles)
		{
			float cr = cos(eulerangles.x * 0.5);
			float sr = sin(eulerangles.x * 0.5);
			float cp = cos(eulerangles.y * 0.5);
			float sp = sin(eulerangles.y * 0.5);
			float cy = cos(eulerangles.z * 0.5);
			float sy = sin(eulerangles.z * 0.5);

			Vector4 quaternion;
			quaternion.w = cy * cp * cr + sy * sp * sr;
			quaternion.x = cy * cp * sr - sy * sp * cr;
			quaternion.y = sy * cp * sr + cy * sp * cr;
			quaternion.z = sy * cp * cr - cy * sp * sr;

			return quaternion;
		}
		float QuaternionToAngle(Vector4 quaternion)
		{
			Vector4 input;
			input.w = 0;
			input.x = quaternion.x;
			input.y = 0;
			input.z = quaternion.z;
			float result = 0 - ToEulerAngles(input).y;
			if (ToEulerAngles(input).x == 0)
			{
				if (result > 0.0)
					return result;
				else
					return result;
			}
			else
			{
				if (result > 0.0)
					return M_PI - result;
				else
					return -M_PI - result;
			}
		}
		Vector4 AngleToQuaternion(float angle)//??????z????????ßﬂ????????????????????????
		{
			Vector3 input;

			if (angle / M_PI > 0.5)
			{
				input.x = M_PI;
				input.y = angle - M_PI;
				input.z = 0;
			}
			else if (angle / M_PI < -0.5)
			{
				input.x = M_PI;
				input.y = angle + M_PI;
				input.z = 0;
			}
			else
			{
				input.x = 0;
				input.y = -angle;
				input.z = M_PI;
			}

			Vector4 result;
			result = ToQuaternion(input);

			return result;
		}
		static unsigned char* Base64ToImg(string Base64Data) {
			vector<BYTE> decodedData = base64_decode(Base64Data);
			unsigned char* img = new unsigned char[decodedData.size()];
			copy(decodedData.begin(), decodedData.end(), img);
			return (img);
		}
	}
#pragma endregion
	//??¶À???
#pragma region Draw
	namespace Draw {
		struct NewImage {
			float BgAlpha = 1;
			Vector3 Channel = Vector3();
			Vector2 Pos = Vector2();
			string Name = "";
			string ImageFile = "";
			bool Base64 = false;
			int Width = 0;
			int Height = 0;
			NewImage(
				float BgAlpha = 1,
				Vector3 Channel = Vector3(1, 1, 1),
				Vector2 Pos = Vector2(),
				string Name = "",
				string ImageFile = "",
				bool Base64 = false,
				int Width = 0,
				int Height = 0
			) :BgAlpha(BgAlpha), Channel(Channel), Pos(Pos), Name(Name), ImageFile(ImageFile), Base64(Base64), Width(Width), Height(Height) { };
		};
		struct NewText {
			float BgAlpha = 1;
			Vector3 Color = Vector3();
			Vector2 Pos = Vector2();
			string Name = "";
			string Text = "";
			float Size = 1;
			NewText(
				float BgAlpha = 1,
				Vector3 Color = Vector3(1, 1, 1),
				Vector2 Pos = Vector2(),
				string Name = "",
				string Text = "",
				float Size = 1
			) :BgAlpha(BgAlpha), Color(Color), Pos(Pos), Name(Name), Text(Text), Size(Size) { };
		};
		map<string, NewImage> Img;
		map<string, NewText> Text;
		map<string, string> About;
	}
#pragma endregion
	//???
#pragma region Commission
	namespace Commission {
		namespace MoveEntity {
			struct Parameter {
				Vector3 Vector;
				void* Entity = nullptr;
				float speed = 100.0;
			};
			map<void*, Parameter> CommissionList;

			static void MoveEntityToTarget() {
				for (auto [entity, parameter] : CommissionList) {
					if (entity != nullptr) {
						//????????
						if (parameter.Entity != nullptr) {
							float EntityX = *offsetPtr<float>(entity, 0x160);
							float EntityY = *offsetPtr<float>(entity, 0x160);
							float EntityZ = *offsetPtr<float>(entity, 0x160);
							float ToEntityX = *offsetPtr<float>(parameter.Entity, 0x160);
							float ToEntityY = *offsetPtr<float>(parameter.Entity, 0x160);
							float ToEntityZ = *offsetPtr<float>(parameter.Entity, 0x160);

							if (fabs(ToEntityX - EntityX) > float(10)) {
								if (ToEntityX < EntityX)
									EntityX -= float(fabs(ToEntityX - EntityX) / 10);
								else
									EntityX += float(fabs(ToEntityX - EntityX) / 10);
							}
							else
								EntityX = ToEntityX;

							if (fabs(ToEntityY - EntityY) > float(10)) {
								if (ToEntityY < EntityY)
									EntityY -= float(fabs(ToEntityY - EntityY) / parameter.speed);
								else
									EntityY += float(fabs(ToEntityY - EntityY) / parameter.speed);
							}
							else
								EntityY = ToEntityY;

							if (fabs(ToEntityZ - EntityZ) > float(10)) {
								if (ToEntityZ < EntityZ)
									EntityZ -= float(fabs(ToEntityZ - EntityZ) / 10);
								else
									EntityZ += float(fabs(ToEntityZ - EntityZ) / 10);
							}
							else
								EntityZ = ToEntityZ;

							*offsetPtr<float>(entity, 0x160) = ToEntityX;
							*offsetPtr<float>(entity, 0x164) = ToEntityY;
							*offsetPtr<float>(entity, 0x168) = ToEntityZ;

							if (
								*offsetPtr<float>(entity, 0x160) == ToEntityX and
								*offsetPtr<float>(entity, 0x164) == ToEntityY and
								*offsetPtr<float>(entity, 0x168) == ToEntityZ
								)
								CommissionList.erase(entity);
						}
						//???????
						else {
							float EntityX = *offsetPtr<float>(entity, 0x160);
							float EntityY = *offsetPtr<float>(entity, 0x160);
							float EntityZ = *offsetPtr<float>(entity, 0x160);
							float ToEntityX = parameter.Vector.x;
							float ToEntityY = parameter.Vector.y;
							float ToEntityZ = parameter.Vector.z;
							if (EntityX - ToEntityX > 10) {
								if (EntityX > ToEntityX)
									*offsetPtr<float>(entity, 0x160) -= (EntityX - ToEntityX) / parameter.speed;
								else
									*offsetPtr<float>(entity, 0x160) += (EntityX - ToEntityX) / parameter.speed;
							}
							else
								*offsetPtr<float>(entity, 0x160) = ToEntityX;
							if (EntityY - ToEntityY > 10) {
								if (EntityY > ToEntityY)
									*offsetPtr<float>(entity, 0x164) -= (EntityY - ToEntityY) / parameter.speed;
								else
									*offsetPtr<float>(entity, 0x164) += (EntityY - ToEntityY) / parameter.speed;
							}
							else
								*offsetPtr<float>(entity, 0x164) = ToEntityY;
							if (EntityZ - ToEntityZ > 10) {
								if (EntityZ > ToEntityZ)
									*offsetPtr<float>(entity, 0x168) -= (EntityZ - ToEntityZ) / parameter.speed;
								else
									*offsetPtr<float>(entity, 0x168) += (EntityZ - ToEntityZ) / parameter.speed;
							}
							else
								*offsetPtr<float>(entity, 0x168) = ToEntityZ;

							if (
								*offsetPtr<float>(entity, 0x160) == ToEntityX and
								*offsetPtr<float>(entity, 0x164) == ToEntityY and
								*offsetPtr<float>(entity, 0x168) == ToEntityZ
								)
								CommissionList.erase(entity);
						}
					}
				}
			}
		}

		static void CleanCommission() {
			MoveEntity::CommissionList.clear();
		}

		static void Run() {
			MoveEntity::MoveEntityToTarget();
		}
	}
#pragma endregion
	//???????
#pragma region Monster
	namespace Monster {
		struct MonsterData {
			void* Plot;
			float CoordinatesX;
			float CoordinatesY;
			float CoordinatesZ;
			int Id;
			int SubId;
			MonsterData(
				void* Plot = nullptr,
				float CoordinatesX = 0,
				float CoordinatesY = 0,
				float CoordinatesZ = 0,
				int Id = 0,
				int SubId = 0)
				:Plot(Plot), CoordinatesX(CoordinatesX), CoordinatesY(CoordinatesY), CoordinatesZ(CoordinatesZ), Id(Id), SubId(SubId) {
			};
		};
		//?????ß“?
		map<void*, MonsterData> Monsters;
		//????????
		pair<int, int> Filter(255, 255);
		//??????????buff
		static void SetBuff(void* monster, string buff) {
			MonsterBuff::MonsterBuffState monsterBuff = MonsterBuff::GetMonsterBuffState(monster, buff);
			if (monsterBuff.MaxStateValue != 0) {
				MonsterBuff::SetMonsterBuffState(monster, buff);
			}
		}
		//???????buff??
		static MonsterBuff::MonsterBuffState GetBuff(void* monster, string buff) {
			return MonsterBuff::GetMonsterBuffState(monster, buff);
		}
		//???????buff??
		static void* GetHateTarget(void* monster) {
			void* HateTargetOffset1 = *offsetPtr<undefined**>((undefined(*)())monster, 0x98);
			void* HateTargetOffset2 = nullptr;
			if (HateTargetOffset1 != nullptr)
				HateTargetOffset2 = *offsetPtr<undefined**>((undefined(*)())HateTargetOffset1, 0x1B0);
			void* HateTargetOffset3 = nullptr;
			if (HateTargetOffset2 != nullptr)
				HateTargetOffset3 = *offsetPtr<undefined**>((undefined(*)())HateTargetOffset2, 0x8);
			void* HateTargetOffset4 = nullptr;
			if (HateTargetOffset3 != nullptr)
				HateTargetOffset4 = *offsetPtr<undefined**>((undefined(*)())HateTargetOffset3, 0x970);
			if (HateTargetOffset4 != nullptr)
				return *offsetPtr<void*>(HateTargetOffset4, 0x5D0);
			return nullptr;
		}
		//???®¥???????????
		static void BehaviorControl(void* monster, int Fsm) {
			//??ß›??????????????
			MH::Monster::BehaviorControl((undefined*)monster, Fsm);
		}
	}
#pragma endregion
	//??????
#pragma region PlayerData
	namespace PlayerData {
		struct FsmData {
			//???? 0?????3?????
			int Target = 0;
			//???Id
			int Id = 0;
			FsmData(int Target = 0, int Id = 0) :Target(Target), Id(Id) {
			};
		};
		//XBOX???????
		struct XBOXKEYS {
			float LB = 0;
			float RB = 0;
			float LT = 0;
			float RT = 0;
			float X = 0;
			float Y = 0;
			float A = 0;
			float B = 0;
			float LstickUp = 0;
			float LstickRight = 0;
			float LstickDown = 0;
			float LstickLeft = 0;
		};

		//????????
		namespace TempData {
			FsmData t_ManualFsmAction;
			bool t_executingFsmAction = false;
			void* t_HookCoordinate = nullptr;
			void* t_HookCoordinate2 = nullptr;
			float t_ActionFrameSpeed = 0;
			bool t_SetActionFrameSpeed = false;
			int t_ActionFrameSpeedTarget = 0;
		}

		//????
		namespace Coordinate {
			//????????
			namespace TempData {
				void* t_visual = nullptr;
				Vector3 t_SetVisualCoordinate;
				void* t_SetVisualBind = nullptr;
				bool t_SetVisual = false;
			}

			//???????
			Vector3 Entity = Vector3();
			//???????
			Vector3 Collimator = Vector3();
			//?????????????
			Vector3 Parabola = Vector3();
			//???????????
			Vector3 Collision = Vector3();
			//????????
			Vector3 Increment = Vector3();
			//????????
			Vector3 Navigation = Vector3();
			//???????
			Vector3 Visual = Vector3();
			//????????
			Vector3 Weapon = Vector3();
			//???????¶À??????
			Vector3 Param = Vector3();
			//???????
			Vector3 Hook = Vector3();

			//??????(X????,Y????,Z????,????)
			static void TransportCoordinate(float X, float Y, float Z, bool Across = false) {
				*offsetPtr<float>(BasicGameData::PlayerPlot, 0x160) = X;
				*offsetPtr<float>(BasicGameData::PlayerPlot, 0x164) = Y;
				*offsetPtr<float>(BasicGameData::PlayerPlot, 0x168) = Z;
				if (Across) {
					*offsetPtr<float>(BasicGameData::PlayerPlot, 0xA50) = X;
					*offsetPtr<float>(BasicGameData::PlayerPlot, 0xA54) = Y;
					*offsetPtr<float>(BasicGameData::PlayerPlot, 0xA58) = Z;
				}
			}
		}
		//?????????
		namespace WeaponOrnaments {
			//????????
			namespace TempData {
				void* t_ornaments = nullptr;
				bool t_setOrnamentsCoordinate = false;
				Vector3 t_SetOrnamentsCoordinate;
				bool t_setOrnamentsSize = false;
				Vector3 t_SetOrnamentsSize;
			}
			//?????????
			Vector3 OrnamentsCoordinate = Vector3();
			//?????????ß≥
			Vector3 OrnamentsSize = Vector3();

			//???????????????
			static void DecontrolOrnamentsCoordinate() {
				TempData::t_setOrnamentsCoordinate = false;
			}
			//?????????ß≥????
			static void DecontrolOrnamentsSize() {
				TempData::t_setOrnamentsSize = false;
			}
			//?????????????(X????,Y????,Z????)
			static void SetOrnamentsCoordinate(float X, float Y, float Z) {
				TempData::t_SetOrnamentsCoordinate = Vector3(X, Y, Z);
				TempData::t_setOrnamentsCoordinate = true;
			}
			//??????ß≥????(X????,Y????,Z????)
			static void SetOrnamentsSize(float X, float Y, float Z) {
				TempData::t_SetOrnamentsSize = Vector3(X, Y, Z);
				TempData::t_setOrnamentsSize = true;
			}
		}
		//????
		namespace Weapons {
			//????????
			namespace TempData {
				void* t_mainWeapon = nullptr;
				void* t_secondaryWeapon = nullptr;
				void* t_weaponHit = nullptr;
				bool t_setMainWeaponCoordinate = false;
				Vector3 t_SetMainWeaponCoordinate;
				bool t_setMainWeaponSize = false;
				Vector3 t_SetMainWeaponSize;
				bool t_setSecondaryWeaponCoordinate = false;
				Vector3 t_SetSecondaryWeaponCoordinate;
				bool t_setSecondaryWeaponSize = false;
				Vector3 t_SetSecondaryWeaponSize;
			}
			//????????
			int WeaponType = 0;
			//????ID
			int WeaponId = 0;
			//??????????
			Vector3 MainWeaponCoordinate = Vector3();
			//??????????ß≥
			Vector3 MainWeaponSize = Vector3();
			//??????????
			Vector3 SecondaryWeaponCoordinate = Vector3();
			//??????????ß≥
			Vector3 SecondaryWeaponSize = Vector3();
			//????????????
			Vector3 HitCoordinate = Vector3();
			//???????????????????????ID??
			static void ChangeWeapons(int type, int id, bool Complete = true) {
				if (type <= 13 and type >= 0 and id >= 0) {
					if (Complete) {
						*offsetPtr<int>(BasicGameData::PlayerPlot, 0x13860) = type;
						*offsetPtr<int>(BasicGameData::PlayerPlot, 0x13864) = id;
						MH::Weapon::CompleteChangeWeapon(BasicGameData::PlayerPlot, 1, 0);
					}
					else
						MH::Weapon::ChangeWeapon(BasicGameData::PlayerPlot, type, id);
				}
			}
			//????????????????
			static void DecontrolMainWeaponCoordinate() {
				TempData::t_setMainWeaponCoordinate = false;
			}
			//???????????ß≥????
			static void DecontrolMainWeaponSize() {
				TempData::t_setMainWeaponSize = false;
			}
			//??????????????(X????,Y????,Z????)
			static void SetMainWeaponCoordinate(float X, float Y, float Z) {
				TempData::t_SetMainWeaponCoordinate = Vector3(X, Y, Z);
				TempData::t_setMainWeaponCoordinate = true;
			}
			//????????ß≥????(X????,Y????,Z????)
			static void SetMainWeaponSize(float X, float Y, float Z) {
				TempData::t_SetMainWeaponSize = Vector3(X, Y, Z);
				TempData::t_setMainWeaponSize = true;
			}

			//????????????????
			static void DecontrolSecondaryWeaponCoordinate() {
				TempData::t_setSecondaryWeaponCoordinate = false;
			}
			//???????????ß≥????
			static void DecontrolSecondaryWeaponSize() {
				TempData::t_setSecondaryWeaponSize = false;
			}
			//??????????????(X????,Y????,Z????)
			static void SetSecondaryWeaponCoordinate(float X, float Y, float Z) {
				TempData::t_SetSecondaryWeaponCoordinate = Vector3(X, Y, Z);
				TempData::t_setSecondaryWeaponCoordinate = true;
			}
			//????????ß≥????(X????,Y????,Z????)
			static void SetSecondaryWeaponSize(float X, float Y, float Z) {
				TempData::t_SetSecondaryWeaponSize = Vector3(X, Y, Z);
				TempData::t_setSecondaryWeaponSize = true;
			}
		}
		//??ßπ
		namespace Effects {
			//??????ßπ(??ßπ?ÔÖ??ßπid)
			static void GenerateSpecialEffects(int group, int record) {
				//??ß›??????????????
				void* Effects = *offsetPtr<void*>(BasicGameData::PlayerPlot, 0x8808);
				//??ßπ????
				MH::Player::Effects((undefined*)Effects, group, record);
			}
		}
		//?????
		static void SetVisual(void* bind, float Duration = 0) {
			Coordinate::TempData::t_SetVisualBind = bind;
			Coordinate::TempData::t_SetVisual = true;
		}
		//??????????
		static void UnbindVisual() {
			Coordinate::TempData::t_SetVisual = false;
			Coordinate::TempData::t_SetVisualBind = nullptr;
		}
		//???????(X????,Y????,Z????,???????0=????)
		static void SetVisual(float X, float Y, float Z, float Duration = 0) {
			Coordinate::TempData::t_SetVisualCoordinate.x = X;
			Coordinate::TempData::t_SetVisualCoordinate.y = Y;
			Coordinate::TempData::t_SetVisualCoordinate.z = Z;
			Coordinate::TempData::t_SetVisual = true;
		}
		//???
		int wirebugs[3];

		//xbox???????		
		XBOXKEYS xboxkeys;

		//??????
		float Angle = 0;
		//?????
		float Radian = 0;
		//?????
		Vector3 EulerAngle = Vector3();
		//?????????
		float Gravity = 0;
		//????????
		float Fallspeedrate = 0;
		//???????
		float VisualDistance = 0;
		//??????????
		bool AimingState = false;
		//??????
		bool PlayerAirState = false;
		//?????¶À??ß÷??????
		void* AttackMonsterPlot = nullptr;
		//????id
		int ActionId = 0;
		//????????
		float ActionFrame = 0;
		float ActionFrameEnd = 0;
		float ActionFrameSpeed = 0;
		//???????
		FsmData Fsm = FsmData();
		FsmData NowFsm = FsmData();
		//???????
		string Name = "";
		//hr???
		int Hr = 0;
		//mr???
		int Mr = 0;
		//??????
		float CurrentHealth = 0;
		//?????????0-150??
		float BasicHealth = 0;
		//???????
		float MaxHealth = 0;
		//???????
		float CurrentEndurance = 0;
		//?????????25-150??
		float MaxEndurance = 0;
		//???????????(??ßÿ???,???Id)
		static void RunDerivedAction(int type, int id) {
			TempData::t_ManualFsmAction = FsmData(type, id);
			TempData::t_executingFsmAction = true;
			*offsetPtr<int>(BasicGameData::PlayerPlot, 0x6284) = type;
			*offsetPtr<int>(BasicGameData::PlayerPlot, 0x6288) = id;
			*offsetPtr<int>(BasicGameData::PlayerPlot, 0x628C) = type;
			*offsetPtr<int>(BasicGameData::PlayerPlot, 0x6290) = id;
			Fsm = FsmData(type, id);
		}
		//????????????????????
		static bool CheckDerivedAction() {
			if (TempData::t_executingFsmAction) {
				if (NowFsm.Id != TempData::t_ManualFsmAction.Id and NowFsm.Target != TempData::t_ManualFsmAction.Target) {
					TempData::t_executingFsmAction = false;
					return true;
				}
				else
					return false;
			}
			return false;
		}
		//??ßÿ???(???Id)
		static void RunAction(int id) {
			MH::Player::CallLmt((undefined*)BasicGameData::PlayerPlot, id, 0);
		}
		//???????????
		static void SetActionFrame(float Frame) {
			void* ActionFramePlot = *offsetPtr<void*>(BasicGameData::PlayerPlot, 0x468);
			*offsetPtr<float>(ActionFramePlot, 0x10c) = Frame;
		}
		//??????Buff???????
		static float GetPlayerBuff(string buff) {
			void* BuffsPlot = *offsetPtr<void*>(BasicGameData::PlayerPlot, 0x7D20);
			int buffPtr = PlayerBuff::GetBuffPtr(buff);
			return *offsetPtr<float>(BuffsPlot, buffPtr);
		}
		//???????Buff???????
		static void SetPlayerBuff(string buff, float duration) {
			void* BuffsPlot = *offsetPtr<void*>(BasicGameData::PlayerPlot, 0x7D20);
			int buffPtr = PlayerBuff::GetBuffPtr(buff);
			*offsetPtr<float>(BuffsPlot, buffPtr) = duration;
		}
		static void SetGravity(float gravity) {
			*offsetPtr<float>(BasicGameData::PlayerPlot, 0x14B0) = gravity;
		}
		static void SetFallSpeedRate(float fallSpeedRate) {
			*offsetPtr<float>(BasicGameData::PlayerPlot, 0xE178) = fallSpeedRate;
		}
		//?????????????????
		static void SetPlayerAimAngle(float angle)//??????Z????????ßﬂ?
		{
			Vector4 quaternion = Calculation::AngleToQuaternion(angle);
			*offsetPtr<float>(BasicGameData::PlayerPlot, 0x174) = quaternion.x;//PlotData::PlayerPlot = 145073ED0 -> 50
			*offsetPtr<float>(BasicGameData::PlayerPlot, 0x17C) = quaternion.z;//?????????????????????????????????????????????????
		}
		static void SetPlayerAimCoordinate(float aim_x, float aim_z)//????????x??z??????
		{
			float direction_x = (aim_x - PlayerData::Coordinate::Entity.x);//???x?? ??? ????x??
			float direction_z = (aim_z - PlayerData::Coordinate::Entity.z);//???z?? ??? ????z??

			float aim_angle = std::atan(direction_x / direction_z);

			aim_angle = aim_angle + sign(direction_x) * (1 - sign(direction_z)) * M_PI / 2;

			SetPlayerAimAngle(aim_angle);//??????
		}
		//??????????
		Vector3 HookCoordinateChange = Vector3();
		//??????????
		bool HookChange = false;
		//??????????
		static void Updata() {
			Coordinate::Entity = Vector3(
				*offsetPtr<float>(BasicGameData::PlayerPlot, 0x160),
				*offsetPtr<float>(BasicGameData::PlayerPlot, 0x164),
				*offsetPtr<float>(BasicGameData::PlayerPlot, 0x168)
			);
			Coordinate::Collision = Vector3(
				*offsetPtr<float>(BasicGameData::PlayerPlot, 0xA50),
				*offsetPtr<float>(BasicGameData::PlayerPlot, 0xA54),
				*offsetPtr<float>(BasicGameData::PlayerPlot, 0xA58)
			);
			Coordinate::Collimator = Vector3(
				*offsetPtr<float>(BasicGameData::PlayerPlot, 0x7D30),
				*offsetPtr<float>(BasicGameData::PlayerPlot, 0x7D34),
				*offsetPtr<float>(BasicGameData::PlayerPlot, 0x7D38)
			);
			Coordinate::Parabola = Vector3(
				*offsetPtr<float>(BasicGameData::PlayerPlot, 0x7D40),
				*offsetPtr<float>(BasicGameData::PlayerPlot, 0x7D44),
				*offsetPtr<float>(BasicGameData::PlayerPlot, 0x7D48)
			);
			Coordinate::Param = Vector3(
				*offsetPtr<float>(BasicGameData::PlayerPlot, 0x14a0),
				*offsetPtr<float>(BasicGameData::PlayerPlot, 0x14a4),
				*offsetPtr<float>(BasicGameData::PlayerPlot, 0x14a8)
			);

			void* WeaponEntityPlot = *offsetPtr<void*>(BasicGameData::PlayerPlot, 0x76B0);
			if (WeaponEntityPlot != nullptr)
				Coordinate::Weapon = Vector3(
					*offsetPtr<float>(WeaponEntityPlot, 0x160),
					*offsetPtr<float>(WeaponEntityPlot, 0x164),
					*offsetPtr<float>(WeaponEntityPlot, 0x168)
				);
			VisualDistance = *offsetPtr<float>(BasicGameData::PlayerPlot, 0x7690);
			Coordinate::Increment = Vector3(
				*offsetPtr<float>(BasicGameData::PlayerPlot, 0x1530),
				*offsetPtr<float>(BasicGameData::PlayerPlot, 0x1534),
				*offsetPtr<float>(BasicGameData::PlayerPlot, 0x1538));
			/*
			void* IncrementPlot = *offsetPtr<undefined**>((undefined(*)())BasicGameData::PlayerPlot, 0x468);
			if (IncrementPlot != nullptr) {
				Coordinate::Increment.x = *offsetPtr<float>(IncrementPlot, 0x7D30);
				Coordinate::Increment.y = *offsetPtr<float>(IncrementPlot, 0x7D34);
				Coordinate::Increment.z = *offsetPtr<float>(IncrementPlot, 0x7D38);
			}
			else {
				Coordinate::Increment.x = 0.0;
				Coordinate::Increment.y = 0.0;
				Coordinate::Increment.z = 0.0;
			}
			*/
			void* AimingStatePlot = *offsetPtr<undefined**>((undefined(*)())BasicGameData::PlayerPlot, 0xC0);
			if (AimingStatePlot != nullptr)
				AimingState = *offsetPtr<bool>(AimingStatePlot, 0xC28);
			else
				AimingState = false;
			Gravity = *offsetPtr<float>(Base::BasicGameData::PlayerPlot, 0x14B0);
			Fallspeedrate = *offsetPtr<float>(Base::BasicGameData::PlayerPlot, 0xE178);
			PlayerAirState = *offsetPtr<bool>(Base::BasicGameData::PlayerPlot, 0x112C);
			Angle = Calculation::QuaternionToAngle(Vector4(
				*offsetPtr<float>(BasicGameData::PlayerPlot, 0x174),
				*offsetPtr<float>(BasicGameData::PlayerPlot, 0x178),
				*offsetPtr<float>(BasicGameData::PlayerPlot, 0x17C),
				*offsetPtr<float>(BasicGameData::PlayerPlot, 0x180)
				));
			if (BasicGameData::PlayerDataPlot != nullptr) {
				AttackMonsterPlot = *offsetPtr<undefined**>((undefined(*)())BasicGameData::PlayerDataPlot, 0x2C8);
				EulerAngle = Vector3(
					*offsetPtr<float>(BasicGameData::PlayerDataPlot, 0x1F0),
					*offsetPtr<float>(BasicGameData::PlayerDataPlot, 0x1F4),
					*offsetPtr<float>(BasicGameData::PlayerDataPlot, 0x1F8)
				);
				//Angle = EulerAngle.x * 180;
				Radian = 4 * atan(1.0) / 180 * PlayerData::Angle;
			}
			else {
				void* PlayerDataHandlePlot = *(undefined**)MH::Player::PlayerDataHandlePlot;
				void* PlayerDataHandleOffset1 = nullptr;
				if (PlayerDataHandlePlot != nullptr)
					PlayerDataHandleOffset1 = *offsetPtr<undefined**>((undefined(*)())PlayerDataHandlePlot, 0x48);
				void* PlayerDataHandleOffset2 = nullptr;
				if (PlayerDataHandleOffset1 != nullptr)
					PlayerDataHandleOffset2 = *offsetPtr<undefined**>((undefined(*)())PlayerDataHandleOffset1, 0x58);
				void* PlayerDataHandleOffset3 = nullptr;
				if (PlayerDataHandleOffset2 != nullptr)
					PlayerDataHandleOffset3 = *offsetPtr<undefined**>((undefined(*)())PlayerDataHandleOffset2, 0x58);
				void* PlayerDataHandleOffset4 = nullptr;
				if (PlayerDataHandleOffset3 != nullptr)
					PlayerDataHandleOffset4 = *offsetPtr<undefined**>((undefined(*)())PlayerDataHandleOffset3, 0x40);
				void* PlayerDataHandleOffset5 = nullptr;
				if (PlayerDataHandleOffset4 != nullptr)
					PlayerDataHandleOffset5 = *offsetPtr<undefined**>((undefined(*)())PlayerDataHandleOffset4, 0xD0);
				if (PlayerDataHandleOffset5 != nullptr)
					BasicGameData::PlayerDataPlot = *offsetPtr<undefined**>((undefined(*)())PlayerDataHandleOffset5, 0x8);
			}
			void* ActionPlot = *offsetPtr<undefined**>((undefined(*)())BasicGameData::PlayerPlot, 0x468);
			if (ActionPlot != nullptr) {
				ActionId = *offsetPtr<int>(ActionPlot, 0xE9C4);
				void* HookOffset1 = *offsetPtr<undefined**>((undefined(*)())ActionPlot, 0x70);
				void* HookOffset2 = nullptr;
				if (HookOffset1 != nullptr)
					HookOffset2 = *offsetPtr<undefined**>((undefined(*)())HookOffset1, 0x10);
				void* HookOffset3 = nullptr;
				if (HookOffset2 != nullptr)
					HookOffset3 = *offsetPtr<undefined**>((undefined(*)())HookOffset2, 0x18);
				if (HookOffset3 != nullptr)
					Coordinate::Hook = Vector3(
						*offsetPtr<float>(HookOffset3, 0x160),
						*offsetPtr<float>(HookOffset3, 0x164),
						*offsetPtr<float>(HookOffset3, 0x168)
					);
			}
			else {
				ActionId = 0;
				Coordinate::Hook = Vector3();
			}
			void* WeaponPlot = *offsetPtr<undefined**>((undefined(*)())BasicGameData::PlayerPlot, 0xc0);
			void* WeaponOffset1 = *offsetPtr<undefined**>((undefined(*)())WeaponPlot, 0x8);
			void* WeaponOffset2 = *offsetPtr<undefined**>((undefined(*)())WeaponOffset1, 0x78);
			Weapons::WeaponType = *offsetPtr<int>(WeaponOffset2, 0x2E8);
			Weapons::WeaponId = *offsetPtr<int>(WeaponOffset2, 0x2EC);
			Fsm = FsmData(
				*offsetPtr<int>(BasicGameData::PlayerPlot, 0x628C),
				*offsetPtr<int>(BasicGameData::PlayerPlot, 0x6290)
			);
			NowFsm = FsmData(
				*offsetPtr<int>(BasicGameData::PlayerPlot, 0x6274),
				*offsetPtr<int>(BasicGameData::PlayerPlot, 0x6278)
			);
			void* ActionFramePlot = *offsetPtr<void*>(BasicGameData::PlayerPlot, 0x468);
			if (ActionFramePlot != nullptr) {
				ActionFrame = *offsetPtr<float>(ActionFramePlot, 0x10C);
				ActionFrameEnd = *offsetPtr<float>(ActionFramePlot, 0x114);
				ActionFrameSpeed = *offsetPtr<float>(BasicGameData::PlayerPlot, 0x6c);
			}
			BasicHealth = *offsetPtr<float>(BasicGameData::PlayerPlot, 0x7628);
			void* StatusManagementPlot = *offsetPtr<undefined**>((undefined(*)())BasicGameData::PlayerPlot, 0x7630);
			if (StatusManagementPlot != nullptr) {
				CurrentHealth = *offsetPtr<float>(StatusManagementPlot, 0x64);
				MaxHealth = *offsetPtr<float>(StatusManagementPlot, 0x60);
				CurrentEndurance = *offsetPtr<float>(StatusManagementPlot, 0x13C);
				MaxEndurance = *offsetPtr<float>(StatusManagementPlot, 0x144);
			}
			else {
				CurrentHealth = 0;
				MaxHealth = 0;
				CurrentEndurance = 0;
				MaxEndurance = 0;
			}
			/*
			char* PlayerName = offsetPtr<char>(BasicGameData::PlayerInfoPlot, 0x50);
			if(strcmp(PlayerName, "\0")) Name = PlayerName;
			*/
			Hr = *offsetPtr<int>(BasicGameData::PlayerInfoPlot, 0x90);
			Mr = *offsetPtr<int>(BasicGameData::PlayerInfoPlot, 0xD4);
			void* key = *((undefined**)0x145224a98);
			if (key != nullptr) {
				xboxkeys.LB = *offsetPtr<float>(key, 0xc80);
				xboxkeys.RB = *offsetPtr<float>(key, 0xc84);
				xboxkeys.LT = *offsetPtr<float>(key, 0xc88);
				xboxkeys.RT = *offsetPtr<float>(key, 0xc8c);
				xboxkeys.X = *offsetPtr<float>(key, 0xc9c);
				xboxkeys.Y = *offsetPtr<float>(key, 0xc90);
				xboxkeys.A = *offsetPtr<float>(key, 0xc98);
				xboxkeys.B = *offsetPtr<float>(key, 0xc94);
				xboxkeys.LstickUp = *offsetPtr<float>(key, 0xca0);
				xboxkeys.LstickRight = *offsetPtr<float>(key, 0xca4);
				xboxkeys.LstickDown = *offsetPtr<float>(key, 0xca8);
				xboxkeys.LstickLeft = *offsetPtr<float>(key, 0xcac);
			}
			else {
				xboxkeys.LB = 0;
				xboxkeys.RB = 0;
				xboxkeys.LT = 0;
				xboxkeys.RT = 0;
				xboxkeys.X = 0;
				xboxkeys.Y = 0;
				xboxkeys.A = 0;
				xboxkeys.B = 0;
				xboxkeys.LstickUp = 0;
				xboxkeys.LstickRight = 0;
				xboxkeys.LstickDown = 0;
				xboxkeys.LstickLeft = 0;
			
			}
		}
	}
#pragma endregion
	//???????
#pragma region Keyboard
	namespace Keyboard {
		namespace TempData {
			map<int, bool> t_KeyDown;
			map<int, int> t_KeyCount;
		}
		//??¯é??
		static bool CheckWindows() {
			string GameName = "MONSTER HUNTER: WORLD(" + ModConfig::Version + ")";
			HWND wnd = GetForegroundWindow();;
			HWND mhd = FindWindow("MT FRAMEWORK", GameName.c_str());
			if (wnd == mhd)
				return true;
			else
				return false;
		}
		//???????
		static bool CheckKey(int vk, int ComboClick = 1, float Duration = 0.3) {
			if (!CheckWindows())
				return false;
			//????????????
			if (TempData::t_KeyDown.find(vk) == TempData::t_KeyDown.end()) {
				TempData::t_KeyDown[vk] = false;
			}
			//???????
			if (GetKeyState(vk) < 0 and !TempData::t_KeyDown[vk]) {
				TempData::t_KeyDown[vk] = true;
				//???????
				if (TempData::t_KeyCount.find(vk) != TempData::t_KeyCount.end()) {
					//????????
					if (TempData::t_KeyCount[vk] == 1)
						Chronoscope::AddChronoscope(Duration, "KEY_" + to_string(vk), true);
					if (Chronoscope::CheckChronoscope("KEY_" + to_string(vk))) {
						TempData::t_KeyCount[vk] = 0;
					}
					TempData::t_KeyCount[vk]++;
				}
				else {
					Chronoscope::AddChronoscope(Duration, "KEY_" + to_string(vk), true);
					TempData::t_KeyCount[vk] = 1;
				}

				//?????
				if (TempData::t_KeyCount[vk] == ComboClick)
					return true;
				else
					return false;
			}
			else if (GetKeyState(vk) >= 0)
				TempData::t_KeyDown[vk] = false;
			return false;
		}
	}
#pragma endregion
	//?????
#pragma region ProjectilesOperation
	namespace ProjectilesOperation {
		//????????????
		static bool CallProjectilesGenerate(int Id, float* Coordinate, int From = 0) {
			//???????????????
			void* Weapon = *offsetPtr<void*>(BasicGameData::PlayerPlot, 0x76B0);
			void* WeaponShlpPlot = *offsetPtr<void*>(Weapon, 0x1D90);
			//????????????
			void* BowgunShlpPlot = *offsetPtr<void*>(BasicGameData::PlayerPlot, 0x56E8);
			if (WeaponShlpPlot == nullptr || BowgunShlpPlot == nullptr)
				return false;

			void* ShlpPlot = nullptr;
			switch (From)
			{
			case 0:
				ShlpPlot = WeaponShlpPlot;
				break;
			case 1:
				ShlpPlot = BowgunShlpPlot;
				break;
			default:
				return false;
			}

			void* ShlpRoute = MH::Shlp::GetShlp(ShlpPlot, Id);
			if (ShlpRoute == nullptr)
				return false;
			ShlpRoute = *offsetPtr<void*>(ShlpRoute, 0x8);
			MH::Shlp::CallShlp(ShlpRoute, BasicGameData::PlayerPlot, BasicGameData::PlayerPlot, Coordinate);
			return true;
		}
		//?????????°§??????
		static void GenerateProjectilesCoordinateData(float*& CalculationCoordinates, Vector3 startPoint, Vector3 endPoint) {
			//???????
			float* temp_float = CalculationCoordinates;
			//ß’?????????
			*temp_float = startPoint.x;
			temp_float++;
			*temp_float = startPoint.y;
			temp_float++;
			*temp_float = startPoint.z;
			temp_float++;
			//???????ß’????????4?????
			*temp_float = 0;
			temp_float++;
			//??????????????ß’??1
			unsigned char* temp_byte = (unsigned char*)temp_float;
			*temp_byte = 1;

			//???›U?????????????40??
			temp_float = offsetPtr<float>(CalculationCoordinates, 0x40);
			//ß’?????????
			*temp_float = endPoint.x;
			temp_float++;
			*temp_float = endPoint.y;
			temp_float++;
			*temp_float = endPoint.z;
			temp_float++;
			//????????ß’????????4?????
			*temp_float = 0;
			temp_float++;
			//??????????????ß’??1
			temp_byte = (unsigned char*)temp_float;
			*temp_byte = 1;

			//???›U?????????????A0??
			int* tempCoordinateTailData = offsetPtr<int>(CalculationCoordinates, 0xA0);
			//ß’??????????¶¬?????
			*tempCoordinateTailData = 0x12;
			tempCoordinateTailData++;
			longlong* tempCoordinateTailData_longlong = (longlong*)tempCoordinateTailData;
			*tempCoordinateTailData_longlong = -1;
		}
		//?????????
		static bool CreateProjectiles(int Id, Vector3 startPoint, Vector3 endPoint, int From = 0) {
			//?????????°§????????????
			float* CoordinatesData = new float[73];
			//??Å®????????
			memset(CoordinatesData, 0, 73 * 4);
			//?????????°§??????
			GenerateProjectilesCoordinateData(CoordinatesData, startPoint, endPoint);
			//????????????
			bool GenerateResults = CallProjectilesGenerate(Id, CoordinatesData, From);
			//??????????
			delete[]CoordinatesData;
			return GenerateResults;
		}
	}
#pragma endregion


	//?????
	static bool Init() {
		if (ModConfig::GameDataInit)
			return true;
		else
		{
			BasicGameData::XboxPadPlot = *(undefined**)MH::GamePad::XboxPadPtr;
			void* PlayerPlot = *(undefined**)MH::Player::PlayerBasePlot;
			void* PlayerInfoPlot = *(undefined**)MH::Player::BasePtr;
			BasicGameData::PlayerPlot = *offsetPtr<undefined**>((undefined(*)())PlayerPlot, 0x50);
			void* PlayerDataOffset1 = *offsetPtr<undefined**>((undefined(*)())PlayerPlot, 0x50);
			void* PlayerDataHandlePlot = *(undefined**)MH::Player::PlayerDataHandlePlot;
			void* PlayerDataHandleOffset1 = nullptr;
			if (PlayerDataHandlePlot != nullptr)
				PlayerDataHandleOffset1 = *offsetPtr<undefined**>((undefined(*)())PlayerDataHandlePlot, 0x48);
			void* PlayerDataHandleOffset2 = nullptr;
			if (PlayerDataHandleOffset1 != nullptr)
				PlayerDataHandleOffset2 = *offsetPtr<undefined**>((undefined(*)())PlayerDataHandleOffset1, 0x58);
			void* PlayerDataHandleOffset3 = nullptr;
			if (PlayerDataHandleOffset2 != nullptr)
				PlayerDataHandleOffset3 = *offsetPtr<undefined**>((undefined(*)())PlayerDataHandleOffset2, 0x58);
			void* PlayerDataHandleOffset4 = nullptr;
			if (PlayerDataHandleOffset3 != nullptr)
				PlayerDataHandleOffset4 = *offsetPtr<undefined**>((undefined(*)())PlayerDataHandleOffset3, 0x40);
			void* PlayerDataHandleOffset5 = nullptr;
			if (PlayerDataHandleOffset4 != nullptr)
				PlayerDataHandleOffset5 = *offsetPtr<undefined**>((undefined(*)())PlayerDataHandleOffset4, 0xD0);
			if (PlayerDataHandleOffset5 != nullptr)
				BasicGameData::PlayerDataPlot = *offsetPtr<undefined**>((undefined(*)())PlayerDataHandleOffset5, 0x8);
			BasicGameData::PlayerInfoPlot = *offsetPtr<undefined**>((undefined(*)())PlayerInfoPlot, 0xA8);
			BasicGameData::GameTimePlot = (undefined(*)())MH::World::GameClock;
			BasicGameData::MapPlot = *offsetPtr<undefined**>((undefined(*)())BasicGameData::PlayerPlot, 0x7D20);
			if (
				BasicGameData::PlayerPlot != nullptr and
				BasicGameData::MapPlot != nullptr and
				BasicGameData::GameTimePlot != nullptr
				) {
				//????????
				MH_Initialize();
				//??????????
				HookLambda(MH::World::WaypointZLocal,
					[](auto x1, auto x2) {
						Navigation(
							&Base::PlayerData::Coordinate::Navigation.x,
							&Base::PlayerData::Coordinate::Navigation.y,
							&Base::PlayerData::Coordinate::Navigation.z
						);
						return original(x1, x2);
					});
				//?????????????
				HookLambda(MH::EnvironmentalBiological::ctor,
					[](auto environmental, auto id, auto subId) {
						auto ret = original(environmental, id, subId);
						Base::World::EnvironmentalData::Environmentals[environmental] = Base::World::EnvironmentalData::EnvironmentalData(
							environmental, 0, 0, 0, id, subId
						);
						return ret;
					});
				//?????????
				HookLambda(MH::Monster::ctor,
					[](auto monster, auto id, auto subId) {
						auto ret = original(monster, id, subId);
						Base::Monster::Monsters[monster] = Base::Monster::MonsterData(
							monster, 0, 0, 0, id, subId
						);
						return ret;
					});
				HookLambda(MH::Monster::dtor,
					[](auto monster) {
						Base::Monster::Monsters.erase(monster);
						return original(monster);
					});
				//?????????????
				HookLambda(MH::Player::Visual,
					[]() {
						GetRBXPtr(&Base::PlayerData::Coordinate::TempData::t_visual);
						if (Base::PlayerData::Coordinate::TempData::t_visual != nullptr) {
							Base::PlayerData::Coordinate::Visual.x = *offsetPtr<float>(Base::PlayerData::Coordinate::TempData::t_visual, 0x0);
							Base::PlayerData::Coordinate::Visual.y = *offsetPtr<float>(Base::PlayerData::Coordinate::TempData::t_visual, 0x4);
							Base::PlayerData::Coordinate::Visual.z = *offsetPtr<float>(Base::PlayerData::Coordinate::TempData::t_visual, 0x8);
							if (Base::PlayerData::Coordinate::TempData::t_SetVisual) {
								*offsetPtr<float>(Base::PlayerData::Coordinate::TempData::t_visual, 0x0) = Base::PlayerData::Coordinate::TempData::t_SetVisualCoordinate.x;
								*offsetPtr<float>(Base::PlayerData::Coordinate::TempData::t_visual, 0x4) = Base::PlayerData::Coordinate::TempData::t_SetVisualCoordinate.y;
								*offsetPtr<float>(Base::PlayerData::Coordinate::TempData::t_visual, 0x8) = Base::PlayerData::Coordinate::TempData::t_SetVisualCoordinate.z;
							}
						}
						return original();
					});
				//????????????
				HookLambda(MH::Weapon::WeaponOrnaments,
					[]() {
						GetRBXPtr(&Base::PlayerData::WeaponOrnaments::TempData::t_ornaments);
						if (Base::PlayerData::WeaponOrnaments::TempData::t_ornaments != nullptr) {
							Base::PlayerData::WeaponOrnaments::OrnamentsCoordinate.x = *offsetPtr<float>(Base::PlayerData::WeaponOrnaments::TempData::t_ornaments, 0x160);
							Base::PlayerData::WeaponOrnaments::OrnamentsCoordinate.y = *offsetPtr<float>(Base::PlayerData::WeaponOrnaments::TempData::t_ornaments, 0x164);
							Base::PlayerData::WeaponOrnaments::OrnamentsCoordinate.z = *offsetPtr<float>(Base::PlayerData::WeaponOrnaments::TempData::t_ornaments, 0x168);
							Base::PlayerData::WeaponOrnaments::OrnamentsSize.x = *offsetPtr<float>(Base::PlayerData::WeaponOrnaments::TempData::t_ornaments, 0x180);
							Base::PlayerData::WeaponOrnaments::OrnamentsSize.y = *offsetPtr<float>(Base::PlayerData::WeaponOrnaments::TempData::t_ornaments, 0x184);
							Base::PlayerData::WeaponOrnaments::OrnamentsSize.z = *offsetPtr<float>(Base::PlayerData::WeaponOrnaments::TempData::t_ornaments, 0x188);
							if (Base::PlayerData::WeaponOrnaments::TempData::t_setOrnamentsCoordinate) {
								*offsetPtr<float>(Base::PlayerData::WeaponOrnaments::TempData::t_ornaments, 0x160) = Base::PlayerData::WeaponOrnaments::TempData::t_SetOrnamentsCoordinate.x;
								*offsetPtr<float>(Base::PlayerData::WeaponOrnaments::TempData::t_ornaments, 0x164) = Base::PlayerData::WeaponOrnaments::TempData::t_SetOrnamentsCoordinate.y;
								*offsetPtr<float>(Base::PlayerData::WeaponOrnaments::TempData::t_ornaments, 0x168) = Base::PlayerData::WeaponOrnaments::TempData::t_SetOrnamentsCoordinate.z;
							}
							if (Base::PlayerData::WeaponOrnaments::TempData::t_setOrnamentsSize) {
								*offsetPtr<float>(Base::PlayerData::WeaponOrnaments::TempData::t_ornaments, 0x180) = Base::PlayerData::WeaponOrnaments::TempData::t_SetOrnamentsSize.x;
								*offsetPtr<float>(Base::PlayerData::WeaponOrnaments::TempData::t_ornaments, 0x184) = Base::PlayerData::WeaponOrnaments::TempData::t_SetOrnamentsSize.y;
								*offsetPtr<float>(Base::PlayerData::WeaponOrnaments::TempData::t_ornaments, 0x188) = Base::PlayerData::WeaponOrnaments::TempData::t_SetOrnamentsSize.z;
							}
						}
						return original();
					});
				//???????????
				HookLambda(MH::Weapon::MainWeaponPtr,
					[]() {
						GetRBXPtr(&Base::PlayerData::Weapons::TempData::t_mainWeapon);
						if (Base::PlayerData::Weapons::TempData::t_mainWeapon != nullptr) {
							Base::PlayerData::Weapons::MainWeaponCoordinate = Vector3(
								*offsetPtr<float>(Base::PlayerData::Weapons::TempData::t_mainWeapon, 0x160),
								*offsetPtr<float>(Base::PlayerData::Weapons::TempData::t_mainWeapon, 0x164),
								*offsetPtr<float>(Base::PlayerData::Weapons::TempData::t_mainWeapon, 0x168)
							);
							Base::PlayerData::Weapons::MainWeaponSize = Vector3(
								*offsetPtr<float>(Base::PlayerData::Weapons::TempData::t_mainWeapon, 0x180),
								*offsetPtr<float>(Base::PlayerData::Weapons::TempData::t_mainWeapon, 0x184),
								*offsetPtr<float>(Base::PlayerData::Weapons::TempData::t_mainWeapon, 0x188)
							);
							if (Base::PlayerData::Weapons::TempData::t_setMainWeaponCoordinate) {
								*offsetPtr<float>(Base::PlayerData::Weapons::TempData::t_mainWeapon, 0x160) = Base::PlayerData::Weapons::TempData::t_SetMainWeaponCoordinate.x;
								*offsetPtr<float>(Base::PlayerData::Weapons::TempData::t_mainWeapon, 0x164) = Base::PlayerData::Weapons::TempData::t_SetMainWeaponCoordinate.y;
								*offsetPtr<float>(Base::PlayerData::Weapons::TempData::t_mainWeapon, 0x168) = Base::PlayerData::Weapons::TempData::t_SetMainWeaponCoordinate.z;
							}
							if (Base::PlayerData::Weapons::TempData::t_setMainWeaponSize) {
								*offsetPtr<float>(Base::PlayerData::Weapons::TempData::t_mainWeapon, 0x180) = Base::PlayerData::Weapons::TempData::t_SetMainWeaponSize.x;
								*offsetPtr<float>(Base::PlayerData::Weapons::TempData::t_mainWeapon, 0x184) = Base::PlayerData::Weapons::TempData::t_SetMainWeaponSize.y;
								*offsetPtr<float>(Base::PlayerData::Weapons::TempData::t_mainWeapon, 0x188) = Base::PlayerData::Weapons::TempData::t_SetMainWeaponSize.z;
							}
						}
						return original();
					});
				HookLambda(MH::Weapon::SecondaryWeaponPtr,
					[]() {
						GetRBXPtr(&Base::PlayerData::Weapons::TempData::t_secondaryWeapon);
						if (Base::PlayerData::Weapons::TempData::t_secondaryWeapon != nullptr) {
							Base::PlayerData::Weapons::SecondaryWeaponCoordinate = Vector3(
								*offsetPtr<float>(Base::PlayerData::Weapons::TempData::t_secondaryWeapon, 0x160),
								*offsetPtr<float>(Base::PlayerData::Weapons::TempData::t_secondaryWeapon, 0x164),
								*offsetPtr<float>(Base::PlayerData::Weapons::TempData::t_secondaryWeapon, 0x168)
							);
							Base::PlayerData::Weapons::SecondaryWeaponSize = Vector3(
								*offsetPtr<float>(Base::PlayerData::Weapons::TempData::t_secondaryWeapon, 0x180),
								*offsetPtr<float>(Base::PlayerData::Weapons::TempData::t_secondaryWeapon, 0x184),
								*offsetPtr<float>(Base::PlayerData::Weapons::TempData::t_secondaryWeapon, 0x188)
							);
							if (Base::PlayerData::Weapons::TempData::t_setSecondaryWeaponCoordinate) {
								*offsetPtr<float>(Base::PlayerData::Weapons::TempData::t_secondaryWeapon, 0x160) = Base::PlayerData::Weapons::TempData::t_SetSecondaryWeaponCoordinate.x;
								*offsetPtr<float>(Base::PlayerData::Weapons::TempData::t_secondaryWeapon, 0x164) = Base::PlayerData::Weapons::TempData::t_SetSecondaryWeaponCoordinate.y;
								*offsetPtr<float>(Base::PlayerData::Weapons::TempData::t_secondaryWeapon, 0x168) = Base::PlayerData::Weapons::TempData::t_SetSecondaryWeaponCoordinate.z;
							}
							if (Base::PlayerData::Weapons::TempData::t_setSecondaryWeaponSize) {
								*offsetPtr<float>(Base::PlayerData::Weapons::TempData::t_secondaryWeapon, 0x180) = Base::PlayerData::Weapons::TempData::t_SetSecondaryWeaponSize.x;
								*offsetPtr<float>(Base::PlayerData::Weapons::TempData::t_secondaryWeapon, 0x184) = Base::PlayerData::Weapons::TempData::t_SetSecondaryWeaponSize.y;
								*offsetPtr<float>(Base::PlayerData::Weapons::TempData::t_secondaryWeapon, 0x188) = Base::PlayerData::Weapons::TempData::t_SetSecondaryWeaponSize.z;
							}
						}
						return original();
					});
				//??????????
				HookLambda(MH::Weapon::Hit,
					[]() {
						GetHitPtr(&Base::PlayerData::Weapons::TempData::t_weaponHit);
						Base::PlayerData::Weapons::HitCoordinate = Vector3(
							*offsetPtr<float>(Base::PlayerData::Weapons::TempData::t_weaponHit, 0x60),
							*offsetPtr<float>(Base::PlayerData::Weapons::TempData::t_weaponHit, 0x64),
							*offsetPtr<float>(Base::PlayerData::Weapons::TempData::t_weaponHit, 0x68)
						);
						return original();
					});
				//????????????
				HookLambda(MH::Player::ActionFrameSpeed,
					[](auto RCX) {
						if (PlayerData::TempData::t_SetActionFrameSpeed) {
							if (PlayerData::TempData::t_ActionFrameSpeedTarget == 0)
								SetEDX(&PlayerData::TempData::t_ActionFrameSpeed);
							if (PlayerData::TempData::t_ActionFrameSpeedTarget == 1 and RCX == Base::BasicGameData::PlayerPlot)
								SetEDX(&PlayerData::TempData::t_ActionFrameSpeed);
							if (PlayerData::TempData::t_ActionFrameSpeedTarget == 2 and RCX != Base::BasicGameData::PlayerPlot)
								SetEDX(&PlayerData::TempData::t_ActionFrameSpeed);
						}
						if (World::TempData::t_SetFrameSpeed >= 0) {
							SetEDX(&World::TempData::t_SetFrameSpeed);
							World::TempData::t_SetFrameSpeed = -1;
						}
						return original(RCX);
					});
				HookLambda(MH::World::ActionFrameSpeed,
					[](auto rcx) {
						for (auto [ptr, frameSpeed] : Base::World::FrameSpeed) {
							if (rcx == ptr) {
								World::TempData::t_SetFrameSpeed = frameSpeed;
							}
						}
						return original(rcx);
					});
				
				//?????????
				HookLambda(MH::Player::HookCoordinateChange,
					[](auto ptr) {
						if (Base::PlayerData::HookChange) {
							*offsetPtr<float>(ptr, 0x60) = Base::PlayerData::HookCoordinateChange.x;
							*offsetPtr<float>(ptr, 0x64) = Base::PlayerData::HookCoordinateChange.y;
							*offsetPtr<float>(ptr, 0x68) = Base::PlayerData::HookCoordinateChange.z;
						}
						return original(ptr);
					});
				HookLambda(MH::Player::HookCoordinateChange2,
					[](auto RCX, auto RDX, auto ptr) {
						if (Base::PlayerData::HookChange) {
							*offsetPtr<float>(ptr, 0x0) = Base::PlayerData::HookCoordinateChange.x;
							*offsetPtr<float>(ptr, 0x4) = Base::PlayerData::HookCoordinateChange.y;
							*offsetPtr<float>(ptr, 0x8) = Base::PlayerData::HookCoordinateChange.z;
						}
						return original(RCX, RDX, ptr);
					});
				HookLambda(MH::Player::HookCoordinateChange3,
					[]() {
						GetRDIPtr(&Base::PlayerData::TempData::t_HookCoordinate);
						if (Base::PlayerData::HookChange) {
							*offsetPtr<float>(Base::PlayerData::TempData::t_HookCoordinate, 0x0) = Base::PlayerData::HookCoordinateChange.x;
							*offsetPtr<float>(Base::PlayerData::TempData::t_HookCoordinate, 0x4) = Base::PlayerData::HookCoordinateChange.y;
							*offsetPtr<float>(Base::PlayerData::TempData::t_HookCoordinate, 0x8) = Base::PlayerData::HookCoordinateChange.z;
						}
						return original();
					});
				HookLambda(MH::Player::HookCoordinateChange4,
					[]() {
						GetRBXPtr(&Base::PlayerData::TempData::t_HookCoordinate2);
						if (Base::PlayerData::HookChange) {
							*offsetPtr<float>(Base::PlayerData::TempData::t_HookCoordinate2, 0x130) = Base::PlayerData::HookCoordinateChange.x;
							*offsetPtr<float>(Base::PlayerData::TempData::t_HookCoordinate2, 0x134) = Base::PlayerData::HookCoordinateChange.y;
							*offsetPtr<float>(Base::PlayerData::TempData::t_HookCoordinate2, 0x138) = Base::PlayerData::HookCoordinateChange.z;
						}
						return original();
					});

				MH_ApplyQueued();
				
				ModConfig::GameDataInit = true;
				LOG(INFO) << ModConfig::ModName << " : Game data initialization complete!";
				LOG(INFO) << " |  Mod??" << ModConfig::ModName;
				LOG(INFO) << " |  Author??" << ModConfig::ModAuthor;
				LOG(INFO) << " |  Version??" << ModConfig::ModVersion;
				//Draw::GameInit = true;
				return true;
			}
			else {
				if (ModConfig::InitErrInfo) {
					LOG(ERR) << ModConfig::ModName << " : Game data initialization failed!";
					LOG(ERR) << "The following address failed to complete the initialization. We will try again later. If the address is still not initialized successfully, please contact the mod author for solution.";
					if (BasicGameData::PlayerPlot == nullptr)
						LOG(ERR) << " |  PlayerPlot";
					if (BasicGameData::PlayerDataPlot == nullptr)
						LOG(ERR) << " |  PlayerDataPlot";
					if (BasicGameData::MapPlot == nullptr)
						LOG(ERR) << " |  MapPlot";
					if (BasicGameData::GameTimePlot == nullptr)
						LOG(ERR) << " |  GameTimePlot";
					if (BasicGameData::XboxPadPlot == nullptr)
						LOG(ERR) << " |  XboxPadPlot";
					ModConfig::InitErrCount++;
					if (ModConfig::InitErrCount > 10)
						ModConfig::InitErrInfo = false;
				}
				return false;
			}
		}
	}
	//???????????
	static void RealTimeUpdate() {
		if (ModConfig::GameDataInit) {
			if (!ModConfig::DrawInit) ModConfig::DrawInit = true;
			//??????????????
			BasicGameData::MapPlot = *offsetPtr<undefined**>((undefined(*)())BasicGameData::PlayerPlot, 0x7D20);
			//ß’????????????????
			if (Chronoscope::NowTime > *offsetPtr<float>(BasicGameData::MapPlot, 0xC24)) {
				World::MapId = *offsetPtr<int>(BasicGameData::MapPlot, 0xB88);
				//????????????
				Chronoscope::ChronoscopeList.clear();
				//???????????????
				World::EnvironmentalData::Environmentals.clear();
				//??????????
				PlayerData::Coordinate::TempData::t_SetVisualBind = nullptr;
				PlayerData::Coordinate::TempData::t_SetVisual = false;
				//???????
				Commission::CleanCommission();
				//????????????
				Monster::Filter = pair<int, int>(255, 255);
				//?????????ß÷??????
				PlayerData::AttackMonsterPlot = nullptr;
				//???????Fsm
				PlayerData::Fsm = PlayerData::FsmData(0, 0);
				PlayerData::NowFsm = PlayerData::FsmData(0, 0);
				PlayerData::TempData::t_ManualFsmAction = PlayerData::FsmData(0, 0);
				PlayerData::TempData::t_executingFsmAction = false;
				//?????????
				void* PlayerPlot = *(undefined**)MH::Player::PlayerBasePlot;
				BasicGameData::PlayerPlot = *offsetPtr<undefined**>((undefined(*)())PlayerPlot, 0x50);
				//???????????
				Keyboard::TempData::t_KeyCount.clear();
				Keyboard::TempData::t_KeyDown.clear();
			}
			//???????????
			PlayerData::Updata();

			//????????????????,????????????????????????????????????????
			for (auto [Environmental, EData] : World::EnvironmentalData::Environmentals) {
				if (EData.Plot == nullptr) {
					World::EnvironmentalData::Environmentals.erase(Environmental);
				}
			}
			//???????????????
			for (auto [environmental, environmentalData] : Base::World::EnvironmentalData::Environmentals) {
				if (environmental != nullptr) {
					Base::World::EnvironmentalData::Environmentals[environmental].CoordinatesX = *offsetPtr<float>(environmental, 0x160);
					Base::World::EnvironmentalData::Environmentals[environmental].CoordinatesY = *offsetPtr<float>(environmental, 0x164);
					Base::World::EnvironmentalData::Environmentals[environmental].CoordinatesZ = *offsetPtr<float>(environmental, 0x168);
				}
			}
			//??????????
			for (auto [monster, monsterData] : Base::Monster::Monsters) {
				if (monster != nullptr) {
					Base::Monster::Monsters[monster].CoordinatesX = *offsetPtr<float>(monster, 0x160);
					Base::Monster::Monsters[monster].CoordinatesY = *offsetPtr<float>(monster, 0x164);
					Base::Monster::Monsters[monster].CoordinatesZ = *offsetPtr<float>(monster, 0x168);
				}
			}

			//???????????
			Chronoscope::NowTime = *offsetPtr<float>(BasicGameData::MapPlot, 0xC24);

			//????????????
			if (!PlayerData::Coordinate::TempData::t_SetVisual) {
				PlayerData::Coordinate::TempData::t_SetVisualBind = nullptr;
			}
			//?????????????
			if (PlayerData::Coordinate::TempData::t_SetVisualBind != nullptr) {
				PlayerData::Coordinate::TempData::t_SetVisualCoordinate.x = *offsetPtr<float>(PlayerData::Coordinate::TempData::t_SetVisualBind, 0x160);
				PlayerData::Coordinate::TempData::t_SetVisualCoordinate.y = *offsetPtr<float>(PlayerData::Coordinate::TempData::t_SetVisualBind, 0x164);
				PlayerData::Coordinate::TempData::t_SetVisualCoordinate.z = *offsetPtr<float>(PlayerData::Coordinate::TempData::t_SetVisualBind, 0x168);
			}
			//???????
			Commission::Run();

			//??????????????
			void* MassagePlot = *(undefined**)MH::World::Message;
			void* PlayerMessageOffset = *offsetPtr<undefined**>((undefined(*)())MassagePlot, 0x38);
			if (PlayerMessageOffset != nullptr)
				World::Massage = offsetPtr<char>(PlayerMessageOffset, 0x80);
		}
	}
}