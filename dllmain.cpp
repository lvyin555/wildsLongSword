// dllmain.cpp : D��finit le point d'entr��e de l'application DLL.
// MHWDLLMod 005
#include <fstream>
#include <queue>
#include <functional>

#include <random>
#include <iostream>
#include <cmath>

#include <windows.h>

#include "minhook/MinHook.h"
#include "json/json.hpp"
#include "loader.h"
#include "ghidra_export.h"
#include "util.h"
#include <thread>

#include "Base.h"
#include "Component.h"
#include "ControlProgram.h"
#define _USE_MATH_DEFINES
#include <string>
#include <conio.h>
#include <math.h>
#include <cmath>
#include "AOB memscan.h"
#include <MMSystem.h>
#pragma comment(lib,"Winmm.lib")

#pragma warning(disable:4996)

using namespace std;
using namespace loader;
using namespace nlohmann;
using json = nlohmann::json;

json config = json::object();
json set = json::object();
bool JoyL = false;
int joyl_ = 0;

int TK, DS, CC, RS, RE, LS, TP;
static void* (*spiritBlade_LvUP)(void*) = (void* (*)(void*))0x142122570;
static void* (*spiritBlade_Refresh)(void*) = (void* (*)(void*))0x142123DBF;

void showMessage(std::string message) {
	MH::Chat::ShowGameMessage(*(undefined**)MH::Chat::MainPtr, (undefined*)&message[0], -1, -1, 0);
}

struct keyboardkeys
{
	int keyset_Y = VK_LBUTTON;
	int keyset_X = 69;
	int keyset_B = VK_RBUTTON;
	int keyset_A = VK_SPACE;
	int keyset_RT = 17;
	int keyset_MR = VK_XBUTTON2;
	int keyset_RB = VK_LSHIFT;
	int keyset_LT = 67;
	int keyset_LU = 87;
	int keyset_LD = 83;
	int keyset_LL = 65;
	int keyset_LR = 68;
	int keyset_LS = NULL;
	int keyset_IAI = NULL;
	float Y = 0;
	float X = 0;
	float B = 0;
	float A = 0;
	float RT = 0;
	float RB = 0;
	float LT = 0;
	float LU = 0;
	float LD = 0;
	float LL = 0;
	float LR = 0;
	float LS = 0;
	float IAI = 0;
} Keys;

void init_json() {
	try
	{
		int X = config["X"];
		int Y = config["Y"];
		int A = config["A"];
		int B = config["B"];
		int RT = config["RT"];
		int MR = config["MR"];
		int RB = config["RB"];
		int LT = config["LT"];
		int LU = config["LU"];
		int LD = config["LD"];
		int LL = config["LL"];
		int LR = config["LR"];
		int LS = config["LS"];
		int IAI = config["IAI"];
		Keys.keyset_X = X;
		Keys.keyset_Y = Y;
		Keys.keyset_A = A;
		Keys.keyset_B = B;
		Keys.keyset_RT = RT;
		Keys.keyset_MR = MR;
		Keys.keyset_RB = RB;
		Keys.keyset_LT = LT;
		Keys.keyset_LU = LU;
		Keys.keyset_LD = LD;
		Keys.keyset_LL = LL;
		Keys.keyset_LR = LR;
		Keys.keyset_LS = LS;
		Keys.keyset_IAI = IAI;
	}
	catch (const std::exception)
	{
		config["Y"] = VK_LBUTTON;
		config["A"] = VK_SPACE;
		config["B"] = VK_RBUTTON;
		config["X"] = 69;
		config["RT"] = 17;
		config["MR"] = VK_XBUTTON2;
		config["RB"] = VK_LSHIFT;
		config["LT"] = 67;
		config["LU"] = 87;
		config["LD"] = 83;
		config["LL"] = 65;
		config["LR"] = 68;
		config["LS"] = NULL;
		config["IAI"] = NULL;
		ofstream myfile(".\\nativePC\\plugins\\iai\\key_config.json", fstream::out);
		myfile << config;
		myfile.close();
	}

	try {
		TK = set["TK"];
		DS = set["DS"];
		CC = set["CC"];
		RS = set["RS"];
		RE = set["RE"];
		LS = set["LS"];
		TP = set["TP"];
	}
	catch (const std::exception)
	{
		set["TK"] = 0;
		set["DS"] = 0;
		set["CC"] = 1;
		set["RS"] = 1;
		set["RE"] = 0;
		set["LS"] = 0;
		set["TP"] = 1;
		ofstream myfile(".\\nativePC\\plugins\\iai\\config.json", fstream::out);
		myfile << set;
		myfile.close();
	}
	
}

bool dirExists(const std::string& dirName_in) {
	DWORD ftyp = GetFileAttributesA(dirName_in.c_str());
	if (ftyp == INVALID_FILE_ATTRIBUTES)
		return false; //something is wrong with your path!
	if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
		return true;
	return false;
}

void load_json() {
	config["Y"] = VK_LBUTTON;
	config["A"] = VK_SPACE;
	config["B"] = VK_RBUTTON;
	config["X"] = 69;
	config["RT"] = 17;
	config["MR"] = VK_XBUTTON2;
	config["RB"] = VK_LSHIFT;
	config["LT"] = 67;
	config["LU"] = 87;
	config["LD"] = 83;
	config["LL"] = 65;
	config["LR"] = 68;
	config["LS"] = NULL;
	config["IAI"] = NULL;
	set["TK"] = 0;
	set["DS"] = 0;
	set["CC"] = 1;
	set["RS"] = 1;
	set["RE"] = 0;
	set["LS"] = 1;
	set["TP"] = 1;
RESTART:
	ifstream i(".\\nativePC\\plugins\\iai\\key_config.json");
	ifstream f(".\\nativePC\\plugins\\iai\\config.json");
	if (!i.is_open() || i.eof()) {
		if (!dirExists(".\\nativePC\\plugins\\iai"))
			CreateDirectory(".\\nativePC\\plugins\\iai", NULL);
		ofstream myfile(".\\nativePC\\plugins\\iai\\key_config.json", fstream::out);
		myfile << config;
		myfile.close();
		goto RESTART;
	}
	if (!i.fail())
	{
		config.clear();
		i >> config;
		i.close();
	}
	if (!f.is_open() || f.eof()) {
		if (!dirExists(".\\nativePC\\plugins\\iai"))
			CreateDirectory(".\\nativePC\\plugins\\iai", NULL);
		ofstream myfile(".\\nativePC\\plugins\\iai\\config.json", fstream::out);
		myfile << set;
		myfile.close();
	}
	if (!f.fail())
	{
		set.clear();
		f >> set;
		f.close();
	}
	else { return; }

	init_json();
	return;
}

/*void GetNowKey()
{
	JOYINFO joyinfo;
	JOYINFOEX joyinfoex;
	joyinfoex.dwSize = sizeof(JOYINFOEX);
	joyinfoex.dwFlags = JOY_RETURNALL;

	for (;;)
	{
		UINT joy_nums;
		joy_nums = joyGetNumDevs();
		if (joy_nums > 0)
		{
			MMRESULT joy_ret = joyGetPosEx(JOYSTICKID1, &joyinfoex);
			if (joy_ret == JOYERR_NOERROR)
			{
				cout << joyinfoex.dwButtons << endl;
				if (joyinfoex.dwButtons == 1)
					Keys.A = Keys.A + 0.0166;

				if (joyinfoex.dwButtons == 2)
					Keys.B = Keys.B + 0.0166;

				if (joyinfoex.dwButtons == 8)
					Keys.Y = Keys.Y + 0.0166;

				if (joyinfoex.dwButtons == 4)
					Keys.X = Keys.X + 0.0166;

				if (joyinfoex.dwButtons == 32)
					Keys.RB = Keys.RB + 0.0166;

				if (joyinfoex.dwZpos < 32767)
					Keys.RT = Keys.RT + 0.0166;

				if (joyinfoex.dwZpos > 32767)
					Keys.LT = Keys.LT + 0.0166;
			}
		}

		if (GetAsyncKeyState(Keys.keyset_Y) < 0)
			Keys.Y = Keys.Y + 0.0166;

		if (GetAsyncKeyState(Keys.keyset_X) < 0)
			Keys.X = Keys.X + 0.0166;

		if (GetAsyncKeyState(Keys.keyset_B) < 0)
			Keys.B = Keys.B + 0.0166;

		if (GetAsyncKeyState(Keys.keyset_A) < 0)
			Keys.A = Keys.A + 0.0166;

		if (GetAsyncKeyState(Keys.keyset_LT) < 0)
			Keys.LT = Keys.LT + 0.0166;

		if (GetAsyncKeyState(Keys.keyset_RB) < 0)
			Keys.RB = Keys.RB + 0.0166;

		if (GetAsyncKeyState(Keys.keyset_RT) < 0)
			Keys.RT = Keys.RT + 0.0166;

		if (joyinfoex.dwButtons == 0 && GetAsyncKeyState(Keys.keyset_Y) == 0)
			Keys.Y = 0;

		if (joyinfoex.dwButtons == 0 && GetAsyncKeyState(Keys.keyset_X) == 0)
			Keys.X = 0;

		if (joyinfoex.dwButtons == 0 && GetAsyncKeyState(Keys.keyset_B) == 0)
			Keys.B = 0;

		if (joyinfoex.dwButtons == 0 && GetAsyncKeyState(Keys.keyset_A) == 0)
			Keys.A = 0;

		if (joyinfoex.dwButtons == 0 && GetAsyncKeyState(Keys.keyset_RB) == 0)
			Keys.RB = 0;

		if ((joyinfoex.dwZpos == 32767 || joyinfoex.dwZpos == 0) && GetAsyncKeyState(Keys.keyset_RT) == 0)
			Keys.RT = 0;

		if ((joyinfoex.dwZpos == 32767 || joyinfoex.dwZpos == 0) && GetAsyncKeyState(Keys.keyset_LT) == 0)
			Keys.LT = 0;

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	// return;
}*/

//�򵥻�ȡ����
void GetNowKey()
{
	Sleep(10000);
	void* Gamepad = *(undefined**)MH::GamePad::XboxPadPtr;
	for (;;) {
		Gamepad = *(undefined**)MH::GamePad::XboxPadPtr;
		if (Gamepad == nullptr)continue;
		if (*offsetPtr<float>(Gamepad, 0xC98) > 0)
			Keys.A = Keys.A + 0.0166;

		if (*offsetPtr<float>(Gamepad, 0xC94) > 0)
			Keys.B = Keys.B + 0.0166;

		if (*offsetPtr<float>(Gamepad, 0xC90) > 0)
			Keys.Y = Keys.Y + 0.0166;

		if (*offsetPtr<float>(Gamepad, 0xC9C) > 0)
			Keys.X = Keys.X + 0.0166;

		if (*offsetPtr<float>(Gamepad, 0xC84) > 0)
			Keys.RB = Keys.RB + 0.0166;

		if (*offsetPtr<float>(Gamepad, 0xC8C) > 0)
			Keys.RT = Keys.RT + 0.0166;

		if (*offsetPtr<float>(Gamepad, 0xC88) > 0)
			Keys.LT = Keys.LT + 0.0166;

		if (*offsetPtr<float>(Gamepad, 0xC64) > 0)
			Keys.LS = Keys.LS + 0.0166;

		if (*offsetPtr<float>(Gamepad, 0xCA0) > 0)
			Keys.LU = *offsetPtr<float>(Gamepad, 0xCA0);

		if (*offsetPtr<float>(Gamepad, 0xCA8) > 0)
			Keys.LD = *offsetPtr<float>(Gamepad, 0xCA8);

		if (*offsetPtr<float>(Gamepad, 0xCAC) > 0)
			Keys.LL = *offsetPtr<float>(Gamepad, 0xCAC);

		if (*offsetPtr<float>(Gamepad, 0xCA4) > 0)
			Keys.LR = *offsetPtr<float>(Gamepad, 0xCA4);

		if (GetAsyncKeyState(Keys.keyset_X) < 0)
			Keys.X = Keys.X + 0.0166;

		if (GetAsyncKeyState(Keys.keyset_Y) < 0)
			Keys.Y = Keys.Y + 0.0166;

		if (GetAsyncKeyState(Keys.keyset_B) < 0)
			Keys.B = Keys.B + 0.0166;

		if (GetAsyncKeyState(Keys.keyset_A) < 0)
			Keys.A = Keys.A + 0.0166;

		if (GetAsyncKeyState(Keys.keyset_LT) < 0)
			Keys.LT = Keys.LT + 0.0166;

		if (GetAsyncKeyState(Keys.keyset_RB) < 0)
			Keys.RB = Keys.RB + 0.0166;

		if (GetAsyncKeyState(Keys.keyset_RT) < 0 || GetAsyncKeyState(Keys.keyset_MR) < 0)
			Keys.RT = Keys.RT + 0.0166;

		if (GetAsyncKeyState(Keys.keyset_LU) < 0)
			Keys.LU = Keys.LU + 0.0166;

		if (GetAsyncKeyState(Keys.keyset_LD) < 0)
			Keys.LD = Keys.LD + 0.0166;

		if (GetAsyncKeyState(Keys.keyset_LL) < 0)
			Keys.LL = Keys.LL + 0.0166;

		if (GetAsyncKeyState(Keys.keyset_LR) < 0)
			Keys.LR = Keys.LR + 0.0166;

		if (GetAsyncKeyState(Keys.keyset_LS) < 0)
			Keys.LS = Keys.LS + 0.0166;

		if (GetAsyncKeyState(Keys.keyset_IAI) < 0)
			Keys.IAI = Keys.IAI + 0.0166;

		if (*offsetPtr<float>(Gamepad, 0xC90) == 0 && GetAsyncKeyState(Keys.keyset_Y) == 0)
			Keys.Y = 0;

		if (*offsetPtr<float>(Gamepad, 0xC9C) == 0 && GetAsyncKeyState(Keys.keyset_X) == 0)
			Keys.X = 0;

		if (*offsetPtr<float>(Gamepad, 0xC94) == 0 && GetAsyncKeyState(Keys.keyset_B) == 0)
			Keys.B = 0;

		if (*offsetPtr<float>(Gamepad, 0xC98) == 0 && GetAsyncKeyState(Keys.keyset_A) == 0)
			Keys.A = 0;

		if (*offsetPtr<float>(Gamepad, 0xC84) == 0 && GetAsyncKeyState(Keys.keyset_RB) == 0)
			Keys.RB = 0;

		if (*offsetPtr<float>(Gamepad, 0xC8C) == 0 && GetAsyncKeyState(Keys.keyset_RT) == 0 && GetAsyncKeyState(Keys.keyset_MR) == 0)
			Keys.RT = 0;

		if (*offsetPtr<float>(Gamepad, 0xC88) == 0 && GetAsyncKeyState(Keys.keyset_LT) == 0)
			Keys.LT = 0;

		if (*offsetPtr<float>(Gamepad, 0xCA0) == 0 && GetAsyncKeyState(Keys.keyset_LU) == 0)
			Keys.LU = 0;

		if (*offsetPtr<float>(Gamepad, 0xCA8) == 0 && GetAsyncKeyState(Keys.keyset_LD) == 0)
			Keys.LD = 0;

		if (*offsetPtr<float>(Gamepad, 0xCAC) == 0 && GetAsyncKeyState(Keys.keyset_LL) == 0)
			Keys.LL = 0;

		if (*offsetPtr<float>(Gamepad, 0xCA4) == 0 && GetAsyncKeyState(Keys.keyset_LR) == 0)
			Keys.LR = 0;

		if (*offsetPtr<float>(Gamepad, 0xC64) == 0 && GetAsyncKeyState(Keys.keyset_LS) == 0)
			Keys.LS = 0;

		if (GetAsyncKeyState(Keys.keyset_IAI) == 0)
			Keys.IAI = 0;

		if (Keys.LU + Keys.LD + Keys.LL + Keys.LR > 0)
			JoyL = true;
		else
			JoyL = false;

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	//return;
}
//ɾ����Ӻϵ��У�����ͨ����Ҳ�е���.	
void asm_edit() {
	Sleep(10000);
	DWORD pid;
	HWND hwnd = FindWindow(NULL, "MONSTER HUNTER: WORLD(421810)");
	GetWindowThreadProcessId(hwnd, &pid);
	HANDLE hprocess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	DWORD64 readByte = NULL;
	
	//��Ӻϲ�����
	char ASM_iai_not_decrease_blade_lv[] = { 0xEB };
	WriteProcessMemory(hprocess, (LPVOID)0X142122F60, ASM_iai_not_decrease_blade_lv, sizeof(ASM_iai_not_decrease_blade_lv), NULL);

	//�������������в�
	char not_cleat_gauge[] = { 0x90,0x90,0x90,0x90,0x90,0x90 };
	WriteProcessMemory(hprocess, (LPVOID)0x14212259B, not_cleat_gauge, sizeof(not_cleat_gauge), NULL);

	return;
}

void fsm_derive(int input1, int input2) {
	void* PlayerObj = *(undefined**)MH::Player::PlayerBasePlot;
	void* PlayerBase = *offsetPtr<void*>(PlayerObj, 0x50);
	*offsetPtr<int>(PlayerBase, 0x6284) = input1;
	*offsetPtr<int>(PlayerBase, 0x6288) = input2;
	return;
}

float Camera_Angle() {
	void* PlayerObj = *(undefined**)MH::Player::PlayerBasePlot;
	void* PlayerBase = *offsetPtr<void*>(PlayerObj, 0x50);
	float Camera_Location_x = *offsetPtr<float>(PlayerBase, 0x7DC0);
	float Camera_Location_z = *offsetPtr<float>(PlayerBase, 0x7DC8);
	if (Camera_Location_z >= 0) {
		return asin(Camera_Location_x);
	}
	else if (Camera_Location_x >= 0 && Camera_Location_z < 0) {
		return acos(Camera_Location_z);
	}
	else if (Camera_Location_x < 0 && Camera_Location_z < 0) {
		return -acos(Camera_Location_z);
	}
}

float Play_Angle() {
	void* PlayerObj = *(undefined**)MH::Player::PlayerBasePlot;
	void* PlayerBase = *offsetPtr<void*>(PlayerObj, 0x50);
	float Play_Location_x = *offsetPtr<float>(PlayerBase, 0x230);
	float Play_Location_z = *offsetPtr<float>(PlayerBase, 0x238);
	if (Play_Location_z >= 0) {
		return asin(Play_Location_x);
	}
	else if (Play_Location_x >= 0 && Play_Location_z < 0) {
		return acos(Play_Location_z);
	}
	else if (Play_Location_x < 0 && Play_Location_z < 0) {
		return -acos(Play_Location_z);
	}
}

float Joy_Angle() {
	void* PlayerJoy = *(undefined**)MH::Player::PlayerJoy;
	float Xbox_Left_x = *offsetPtr<int>(PlayerJoy, 0x248);
	float Xbox_Left_z = *offsetPtr<int>(PlayerJoy, 0x24C);
	float R = (float)pow(pow(Xbox_Left_x, 2) + pow(Xbox_Left_z, 2), 0.5);
	if (R > 32767 * 0.6) {
		Xbox_Left_x = Xbox_Left_x / R;
		Xbox_Left_z = Xbox_Left_z / R;
		if (Xbox_Left_z >= 0) {
			return asin(Xbox_Left_x);
		}
		else if (Xbox_Left_x >= 0 && Xbox_Left_z < 0) {
			return acos(Xbox_Left_z);
		}
		else if (Xbox_Left_x < 0 && Xbox_Left_z < 0) {
			return -acos(Xbox_Left_z);
		}
	}
	return 0;
}

void play_ToAngle(float Angle) {
	void* PlayerObj = *(undefined**)MH::Player::PlayerBasePlot;
	void* PlayerBase = *offsetPtr<void*>(PlayerObj, 0x50);
	static float PI = (float)3.14159265358979323846;
	float x = 0, y = 0, z = 0;
	if (Angle / PI > 0.5) { x = PI; y = Angle - PI; z = 0; }
	else if (Angle / PI < -0.5) { x = PI; y = Angle + PI; z = 0; }
	else { x = 0; y = -Angle; z = PI; }
	float cr = (float)cos(x * 0.5);
	float sr = (float)sin(x * 0.5);
	float cp = (float)cos(y * 0.5);
	float sp = (float)sin(y * 0.5);
	float cy = (float)cos(z * 0.5);
	float sy = (float)sin(z * 0.5);
	float qw = cy * cp * cr + sy * sp * sr;
	float qx = cy * cp * sr - sy * sp * cr;
	float qy = sy * cp * sr + cy * sp * cr;
	float qz = sy * cp * cr - cy * sp * sr;
	*offsetPtr<float>(PlayerBase, 0x174) = qx;
	*offsetPtr<float>(PlayerBase, 0x17C) = qz;
}

void mian_loop() {
	//�ȴ�ʮ���ӱ����ָ��
	Sleep(10000);

	//�ֱ������ַ
	//void* key = *((undefined**)0x145224a98);

	//��һ�ַ
	void* PlayerObject = *(undefined**)MH::Player::PlayerBasePlot;
	void* PlayerSkill = *(undefined**)MH::Player::PlayerSkill;
	//void* PlayerText = *(undefined**)MH::Player::PlayerText;

	//���뻺��
	int input[2] = { 0,0 };

	//�Ӻϳɹ����λ
	bool iai_suc = false;

	int rd = 0;
	int sp = 0;
	int spc = 1;
	int spq = 1;
	int spu = 0;
	int rs = 0;
	int sbl = 3, laisbl = 1;
	int rfs = 1;
	int fsb = 1;
	int riai = 0;
	int rh = 0;
	int a = 0;
	int lsp = 0;
	int jk = 0;
	int ndf = 0;
	float KeyA = 1;
	float KeyY = 1;
	float KeyRT = 1.0;
	int isspr = 0;
	int spg = 0;
	float angle, frame, joyAngle, camAngle;
	float Target, Difftime;

	DWORD pid;
	HWND hwnd = FindWindow(NULL, "MONSTER HUNTER: WORLD(421810)");
	GetWindowThreadProcessId(hwnd, &pid);
	HANDLE hprocess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	DWORD64 readByte = NULL;
	char CatCar_not_decrease_blade[] = { 0x90,0x90,0x90,0x90,0x90,0x90,0x90 };
	char CatCar_not_decrease_hp[] = { 0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90 };
	char CatCar_not_decrease_sp[] = { 0x90,0x90,0x90,0x90,0x90 };
	char not_splvup[] = { 0x90,0x90,0x90,0x90,0x90,0x90 };
	char decrease_blade[] = { 0x4C,0x89,0x89,0x68,0x23,0x00,0x00 };
	char decrease_bladelv[] = { 0x4C,0x89,0x89,0x70,0x23,0x00,0x00 };
	char decrease_bladeup[] = { 0x44,0x89,0x8A,0x88,0x23,0x00,0x00 };
	char decrease_bladeup2[] = { 0x44,0x89,0x89,0x79,0x23,0x00,0x00 };
	char decrease_hp[] = { 0xF3,0x0F,0x11,0x8F,0x28,0x76,0x00,0x00 };
	char decrease_sp[] = { 0xF3,0x0F,0x11,0x43,0x70 };
	char decrease_sp2[] = { 0xF3,0x0F,0x11,0x43,0x74 };
	char splvup[] = { 0x8B,0x87,0x70,0x23,0x00,0x00 };
	char blade_efx[] = { 0X0F, 0X99, 0XC3 };
	char not_blade_efx[] = { 0X0F, 0X95, 0XC3 };

	/*AllocConsole();
	FILE* fileStream;
	freopen_s(&fileStream, "CONOUT$", "w", stdout);
	freopen_s(&fileStream, "CONIN$", "r", stdin);
	freopen_s(&fileStream, "CONOUT$", "w", stderr);*/

	while (1) {
		//�߳�ÿ������60��,ģ��60֡ˢ��
		//Sleep(16);
		std::this_thread::sleep_for(std::chrono::milliseconds(16));

		//��ȡ���̰���
		//GetNowKey();

		//��ȡ��ַ,��Ϊ��ָ�������¿�ʼѭ��,������ʵ��Ƿ���ַ
		if (PlayerObject == nullptr) continue;
		void* PlayerBase = *offsetPtr<void*>(PlayerObject, 0x50);
		if (PlayerBase == nullptr) continue;
		void* MutekiPtr = *offsetPtr<void*>(PlayerBase, 0x6a8);
		if (MutekiPtr == nullptr) continue;
		void* playeroff = *offsetPtr<void*>(PlayerBase, 0x76B0);
		if (playeroff == nullptr) continue;
		void* healthoff = *offsetPtr<void*>(PlayerBase, 0x7630);
		if (healthoff == nullptr) continue;
		void* wepoff = *offsetPtr<void*>(PlayerBase, 0xc0);
		if (wepoff == nullptr) continue;
		void* actoff = *offsetPtr<void*>(PlayerBase, 0x468);
		if (actoff == nullptr) continue;
		if (PlayerSkill == nullptr) continue;
		void* Props = *offsetPtr<void*>(PlayerBase, 0x80);
		if (Props == nullptr) continue;
		Props = *offsetPtr<void*>(Props, 0x80);
		Props = *offsetPtr<void*>(Props, 0x10);
		Props = *offsetPtr<void*>(Props, 0x288);
		//if (PlayerText == nullptr) continue;
		wepoff = *offsetPtr<void*>(wepoff, 0x8);
		wepoff = *offsetPtr<void*>(wepoff, 0x78);
		//PlayerText = *offsetPtr<void*>(PlayerText, 0x948);
		//PlayerText = *offsetPtr<void*>(PlayerText, 0x580);
		//PlayerText = *offsetPtr<void*>(PlayerText, 0x30);

		//Target = 0;
		//Difftime = 0;

		//ǰ
		if (Keys.LU != 0.0 && Keys.LU >= Keys.LL && Keys.LU >= Keys.LR) {
			joyl_ = 1;
		}
		//��
		else if (Keys.LD != 0.0 && Keys.LD >= Keys.LL && Keys.LD >= Keys.LR) {
			joyl_ = 4;
		}
		//��
		else if (Keys.LR > Keys.LU && Keys.LR > Keys.LD)
			joyl_ = 3;
		//��
		else if (Keys.LL > Keys.LU && Keys.LL > Keys.LD)
			joyl_ = 2;
		//��
		else
			joyl_ = 0;

		//char t1, t2, t3, t4, t5;
		//t1 = *offsetPtr<char>(PlayerText, 0x8DB3);
		//t2 = *offsetPtr<char>(PlayerText, 0x8DBA);
		//t3 = *offsetPtr<char>(PlayerText, 0x8DCB);
		//t4 = *offsetPtr<char>(PlayerText, 0x8DD6);
		//t5 = *offsetPtr<char>(PlayerText, 0xA900);

		//è��
		if (CC) {
			if (*offsetPtr<float>(healthoff, 0x64) <= 0.1f) {
				//������
				if (*offsetPtr<int>(wepoff, 0x2e8) == 0x3) {
					WriteProcessMemory(hprocess, (LPVOID)0x1421245B4, CatCar_not_decrease_blade, sizeof(CatCar_not_decrease_blade), NULL);
					WriteProcessMemory(hprocess, (LPVOID)0x1421245BB, CatCar_not_decrease_blade, sizeof(CatCar_not_decrease_blade), NULL);
					WriteProcessMemory(hprocess, (LPVOID)0x142124703, CatCar_not_decrease_blade, sizeof(CatCar_not_decrease_blade), NULL);
					WriteProcessMemory(hprocess, (LPVOID)0x1421245C6, CatCar_not_decrease_blade, sizeof(CatCar_not_decrease_blade), NULL);
				}
				//����Ѫ������
				WriteProcessMemory(hprocess, (LPVOID)0x141F6F64D, CatCar_not_decrease_hp, sizeof(CatCar_not_decrease_hp), NULL);
				WriteProcessMemory(hprocess, (LPVOID)0x1412FDEAD, CatCar_not_decrease_sp, sizeof(CatCar_not_decrease_sp), NULL);
				WriteProcessMemory(hprocess, (LPVOID)0x1412FDEB2, CatCar_not_decrease_sp, sizeof(CatCar_not_decrease_sp), NULL);
			}
			else {
				WriteProcessMemory(hprocess, (LPVOID)0x141F6F64D, decrease_hp, sizeof(decrease_hp), NULL);
				WriteProcessMemory(hprocess, (LPVOID)0x1412FDEAD, decrease_sp, sizeof(decrease_sp), NULL);
				WriteProcessMemory(hprocess, (LPVOID)0x1412FDEB2, decrease_sp2, sizeof(decrease_sp2), NULL);
				WriteProcessMemory(hprocess, (LPVOID)0x1421245B4, decrease_blade, sizeof(decrease_blade), NULL);
				WriteProcessMemory(hprocess, (LPVOID)0x1421245BB, decrease_bladelv, sizeof(decrease_bladelv), NULL);
				WriteProcessMemory(hprocess, (LPVOID)0x142124703, decrease_bladeup, sizeof(decrease_bladeup), NULL);
				WriteProcessMemory(hprocess, (LPVOID)0x1421245C6, decrease_bladeup2, sizeof(decrease_bladeup2), NULL);
			}
		}
		//Ѫ������0.1
		if (*offsetPtr<float>(healthoff, 0x64) > 0.1f) {
			//*offsetPtr<char>(Playtext, 0xA900) = t5;
			//����
			if (*offsetPtr<int>(PlayerBase, 0x6278) == 0x298)
				if (*offsetPtr<float>(actoff, 0x10c) < 0.1f)
					if (JoyL) {
						rd = 1;
						fsm_derive(1, 0x29C); 
					}
			if (*offsetPtr<int>(PlayerBase, 0x6278) == 0x29C){
				if (rd && *offsetPtr<float>(actoff, 0x10c) >= 25.0f){
					fsm_derive(1, 0x0F);
					rd = 0;
				}
			}
			else if (*offsetPtr<int>(PlayerBase, 0x6278) != 0x298)
				rd = 0;
			//�ֶ�ĥ��
			if (*offsetPtr<int>(actoff, 0xe9c4) == 0xC1AD || *offsetPtr<int>(actoff, 0xe9c4) == 0xC046) {
				if (spq != 0) {
					*offsetPtr<int>(playeroff, 0x20F8) += 30 * (4 - sp);
					spq = 0;
					sp = 0;
				}
			}
			else {
				if (*offsetPtr<int>(actoff, 0xe9c4) == 0xC1AC) {
					if (*offsetPtr<float>(actoff, 0x10c) <= 15.0f && spc != 0) {
						*offsetPtr<int>(playeroff, 0x20F8) += 30;
						sp++;
						spc = 0;
					}
					if (*offsetPtr<float>(actoff, 0x10c) > 15.0f) spc = 1;
				}
				else if (*offsetPtr<int>(actoff, 0xe9c4) != 0xC1AC) {
					sp = 0;
					spc = 1;
					spq = 1;
					//�޸�����ĥ������
					if (*offsetPtr<int>(PlayerBase, 0x6278) == 0x295 && *offsetPtr<int>(Props, 0x28E0) == 0x72){
						if (*offsetPtr<int>(playeroff, 0x20F8) - spu == 30) {
							*offsetPtr<int>(playeroff, 0x20F8) += 120;
						}
					}
					spu = *offsetPtr<int>(playeroff, 0x20F8);
				}
			}
			if (*offsetPtr<int>(wepoff, 0x2e8) != 0x3 && *offsetPtr<int>(wepoff, 0x2e8) != 0x9) {
				//�����ж�
				if (*offsetPtr<int>(actoff, 0xe9c4) == 0x7105) {
					if (jk) {
						jk = 0;
						*offsetPtr<float>(actoff, 0x10c) = 37;
					}
					if (*offsetPtr<float>(actoff, 0x10c) >= 78) {
						*offsetPtr<float>(actoff, 0x10c) = 40;
					}
				}
				if (*offsetPtr<int>(PlayerBase, 0x1128) == 1) {
					if ((*offsetPtr<int>(actoff, 0xe9c4) == 0xC032 && (*offsetPtr<int>(wepoff, 0x2e8) != 0x1 || *offsetPtr<int>(wepoff, 0x2e8) != 0x2))
						||(*offsetPtr<int>(actoff, 0xe9c4) == 0xC048 && (*offsetPtr<int>(wepoff, 0x2e8) == 0x1 || *offsetPtr<int>(wepoff, 0x2e8) == 0x2)) 
						|| *offsetPtr<int>(actoff, 0xe9c4) == 0xC033) {
						if (*offsetPtr<float>(actoff, 0x10c) >= 5) {
							if (Keys.X > 0 || (LS && Keys.LS > 0) || (!RE && Keys.RB > 0) || (RE && Keys.RT > 0.0)) {
								//�����ɵ�
								*offsetPtr<int>(PlayerBase, 0x76a8) = 0;
								//fsm_derive(1, 0xD1);
								jk = 1;
								fsm_derive(1, 0x2A9);
								//*offsetPtr<int>(PlayerBase, 0x76a8) = 0;
							}
						}
					}
				}
				else {
					jk = 0;
				}
			}
			//�ܸ�
			if (*offsetPtr<int>(wepoff, 0x2e8) == 0x9) {
				//�����ж�
				if (*offsetPtr<int>(actoff, 0xe9c4) == 0x7105) {
					if (jk) {
						jk = 0;
						*offsetPtr<float>(actoff, 0x10c) = 37;
					}
					if (*offsetPtr<float>(actoff, 0x10c) >= 78) {
						*offsetPtr<float>(actoff, 0x10c) = 40;
					}
				}
				if (*offsetPtr<int>(PlayerBase, 0x1128) == 1) {
					if (*offsetPtr<int>(actoff, 0xe9c4) == 0xC035 || *offsetPtr<int>(actoff, 0xe9c4) == 0xC037) {
						if (*offsetPtr<float>(actoff, 0x10c) >= 5) {
							if (Keys.X > 0 || (LS && Keys.LS > 0) || (!RE && Keys.RB > 0) || (RE && Keys.RT > 0.0)) {
								//�����ɵ�
								*offsetPtr<int>(PlayerBase, 0x76a8) = 0;
								//fsm_derive(1, 0xD1);
								jk = 1;
								fsm_derive(1, 0x2A9);
								//*offsetPtr<int>(PlayerBase, 0x76a8) = 0;
							}
						}
					}
				}
				else {
					jk = 0;
				}
			}
			//Ϊ̫������Ч
			if (*offsetPtr<int>(wepoff, 0x2e8) == 0x3) {
				//if (*offsetPtr<int>(actoff, 0xe9c4) == 0xC06C)
					//*offsetPtr<char>(Playtext, 0xA900) = t2;
				//else
					//*offsetPtr<char>(Playtext, 0xA900) = t1;
				//���ű���
				//float* fsmp = (float*)(*(int*)0x1451238C8 + *offsetPtr<int>(PlayerBase, 0x10) * 0xf8 + 0x9c);
				//*fsmp = 1.0;
				//������ɫ��Ч
				if (lsp != *offsetPtr<int>(playeroff, 0x2370))
					WriteProcessMemory(hprocess, (LPVOID)0x142123DBF, not_splvup, sizeof(not_splvup), NULL);
				else
					WriteProcessMemory(hprocess, (LPVOID)0x142123DBF, splvup, sizeof(splvup), NULL);
				lsp = *offsetPtr<int>(playeroff, 0x2370);

				//�����ж�
				if (*offsetPtr<int>(actoff, 0xe9c4) == 0x7105) {
					if (jk) {
						jk = 0;
						*offsetPtr<float>(actoff, 0x10c) = 37;
					}
					if (*offsetPtr<float>(actoff, 0x10c) >= 78) {
						*offsetPtr<float>(actoff, 0x10c) = 40;
					}
				}
				if (*offsetPtr<int>(PlayerBase, 0x1128) == 1) {
					if (*offsetPtr<int>(actoff, 0xe9c4) == 0xC032 || *offsetPtr<int>(actoff, 0xe9c4) == 0xC033) {
						if (*offsetPtr<float>(actoff, 0x10c) >= 5) {
							if (Keys.X > 0 || (LS && Keys.LS > 0) || (!RE && Keys.RB > 0) || (RE && Keys.RT > 0.0)) {
								//�����ɵ�
								*offsetPtr<int>(PlayerBase, 0x76a8) = 0;
								//fsm_derive(1, 0xD1);
								jk = 1;
								fsm_derive(1, 0x2A9);
							}
						}
					}
					if (*offsetPtr<int>(actoff, 0xe9c4) == 0xC0AC) {
						if (*offsetPtr<float>(actoff, 0x10c) >= 35.0f && *offsetPtr<float>(actoff, 0x10c) <= 120.0f) {
							if (Keys.X > 0 || (LS && Keys.LS > 0) || (!RE && Keys.RB > 0) || (RE && Keys.RT > 0.0)) {
								//���������ɵ�
								*offsetPtr<int>(PlayerBase, 0x76a8) = 0;
								jk = 1;
								fsm_derive(1, 0x2A9);
							}
						}
					}
					//���в�������
					if (*offsetPtr<float>(playeroff, 0x2374) < 0.01 && *offsetPtr<int>(PlayerBase, 0x76a8) == 1) *offsetPtr<float>(playeroff, 0x2374) = 0.01;
				}
				else {
					jk = 0;
				}
				if (*offsetPtr<int>(actoff, 0xe9c4) == 0xC06C) {
					if (KeyY <= 0) {
						if (*offsetPtr<float>(actoff, 0x10c) >= 108.0f) {
							if (Keys.Y > 0 && JoyL) {
								for (int Y_count = 0; Y_count < 8; Y_count++) {
									if (Keys.RT > 0.0 || Keys.B > 0) {
										break;
									}

									if (Y_count == 7) {
										fsm_derive(3, 0x55);
										//����ն������̤��ֱն
									}
								}
							}
						}
					}
					KeyY = Keys.Y;
				}
				if (isspr) {
					if (*offsetPtr<int>(actoff, 0xe9c4) == 0xC06B ||
						*offsetPtr<int>(actoff, 0xe9c4) == 0xC077 ||
						*offsetPtr<int>(actoff, 0xe9c4) == 0xC065 ||
						*offsetPtr<int>(actoff, 0xe9c4) == 0xC153 ||
						*offsetPtr<int>(actoff, 0xe9c4) == 0xC067 ||
						*offsetPtr<int>(actoff, 0xe9c4) == 0xC07F ||
						*offsetPtr<int>(actoff, 0xe9c4) == 0xC080 ||
						*offsetPtr<int>(actoff, 0xe9c4) == 0xC0AA)
						if (*offsetPtr<float>(actoff, 0x10c) <= 1.0f) {
							//�޸������ȡ�����״̬����
							*offsetPtr<int>(PlayerBase, 0x76a8) = 1;
						}
					isspr = 0;
				}
				if (*offsetPtr<int>(actoff, 0xe9c4) == 0xC06D) {
					isspr = 1;
					if (*offsetPtr<float>(actoff, 0x10c) < 48.0f) {
						spg = 1;
					}
					if (KeyA <= 0) {
						if (*offsetPtr<float>(actoff, 0x10c) >= 48.0f && *offsetPtr<float>(actoff, 0x10c) <= 110.0f) {
							if (Keys.A > 0) {
								for (int A_count = 0; A_count < 8; A_count++) {
									//ѭ������RT������,����Ϊ���µ���RT+B��ϼ�
									if (Keys.RT > 0.0) {
										break;
									}
									//ÿ��ѭ����һ֡�ӳ�
									if (A_count == 7) {
										//�����÷���ȡ�������
										if (TP == 2) {
											if (joyl_ == 1) fsm_derive(3, 0x13); //ǰ����
											else if (joyl_ == 2) fsm_derive(3, 0x14); //�󷭹�
											else if (joyl_ == 3) fsm_derive(3, 0x15);//�ҷ���
											else if (joyl_ == 4) fsm_derive(3, 0x16);//�󷭹�
											else fsm_derive(3, 0x13);//����
										}
										else {
											if (spg == 1) {
												angle = Play_Angle();
												joyAngle = Joy_Angle();
												camAngle = Camera_Angle();
												spg = 0;
											}
											if (joyAngle != 0)
											{
												Target = camAngle - joyAngle;
												if (Target > M_PI)
													Target -= 2 * M_PI;
												else if (Target < -M_PI)
													Target += 2 * M_PI;
												Difftime = Target - angle;
											}
											else Difftime = 0;
											if (abs(Difftime) > 0.2)
											{
												if (Difftime >= 0 && Difftime < M_PI / 2) { //ǰ����
													fsm_derive(3, 0x13);
												}
												else if (Difftime >= 0 && Difftime >= M_PI / 2) { //�󷭹�
													fsm_derive(3, 0x14);
												}
												else if (Difftime < 0 && Difftime >= -M_PI / 2) { //�ҷ���
													fsm_derive(3, 0x15);
												}
												else if (Difftime < 0 && Difftime < -M_PI / 2) { //�󷭹�
													fsm_derive(3, 0x16);
												}
											}
											else fsm_derive(3, 0x13); //����
										}
										KeyA = 1;
									}
									std::this_thread::sleep_for(std::chrono::milliseconds(16));
								}
							}
						}
					}
					KeyA = Keys.A;
				}
				if (*offsetPtr<int>(actoff, 0xe9c4) == 0xC08C) {
					if (*offsetPtr<float>(actoff, 0x10c) >= 80.0f) {
						if (Keys.X > 0 || (LS && Keys.LS > 0) || (!RE && Keys.RB > 0) || (RE && Keys.RT > 0.0)) {
							//���������ɵ�
							if (JoyL)
								fsm_derive(3, 9);
							else
								fsm_derive(3, 6);
						}
					}
				}
				if (*offsetPtr<int>(actoff, 0xe9c4) == 0xC165 || *offsetPtr<int>(actoff, 0xe9c4) == 0xC16F) {
					if (ndf == 50) {
						frame = 0.18f;
						angle = Play_Angle();
						joyAngle = Joy_Angle();
						camAngle = Camera_Angle();
					}
					if (ndf > 0) {
						if (joyAngle != 0)
						{
							Target = camAngle - joyAngle;
							if (Target > M_PI)
								Target -= 2 * M_PI;
							else if (Target < -M_PI)
								Target += 2 * M_PI;
							Difftime = Target - angle;
						}
						else Difftime = 0;
						if (abs(Difftime) > 0.2)
						{
							if (Difftime >= 0 && Difftime < M_PI)
								angle += frame;
							else if (Difftime >= 0 && Difftime >= M_PI)
								angle -= frame;
							else if (Difftime < 0 && Difftime >= -M_PI)
								angle -= frame;
							else if (Difftime < 0 && Difftime < -M_PI)
								angle += frame;
							if (angle > M_PI)
								angle -= 2 * M_PI;
							else if (angle < -M_PI)
								angle += 2 * M_PI;
							play_ToAngle(angle);
						}
						ndf--;
					}
				}
				if (*offsetPtr<int>(actoff, 0xe9c4) == 0xC132) {
					if (ndf == 50) {
						frame = 0.18f;
						angle = Play_Angle();
						joyAngle = Joy_Angle();
						camAngle = Camera_Angle();
					}
					if (ndf > 0) {
						if (joyAngle != 0)
						{
							Target = camAngle - joyAngle;
							if (Target > M_PI)
								Target -= 2 * M_PI;
							else if (Target < -M_PI)
								Target += 2 * M_PI;
							Difftime = Target - angle;
						}
						else Difftime = 0;
						if (abs(Difftime) > 0.2)
						{
							if (Difftime >= 0 && Difftime < M_PI)
								angle += frame;
							else if (Difftime >= 0 && Difftime >= M_PI)
								angle -= frame;
							else if (Difftime < 0 && Difftime >= -M_PI)
								angle -= frame;
							else if (Difftime < 0 && Difftime < -M_PI)
								angle += frame;
							if (angle > M_PI)
								angle -= 2 * M_PI;
							else if (angle < -M_PI)
								angle += 2 * M_PI;
							play_ToAngle(angle);
						}
						ndf--;
					}
					if (KeyA <= 0) {
						if (*offsetPtr<float>(actoff, 0x10c) >= 100.0f) {
							if (Keys.A > 0) {
								//�����÷���ȡ���ɵ�
								if (DS) {
									*offsetPtr<int>(PlayerBase, 0x76a8) = 1;
									fsm_derive(3, 0x12);
								}
								else fsm_derive(1, 0x35);
								KeyA = 1;
							}
						}
					}
					KeyA = Keys.A;
				}
				if (*offsetPtr<int>(actoff, 0xe9c4) != 0xC132 &&
					*offsetPtr<int>(actoff, 0xe9c4) != 0xC165 &&
					*offsetPtr<int>(actoff, 0xe9c4) != 0xC16F &&
					*offsetPtr<int>(actoff, 0xe9c4) != 0xC134 && 
					*offsetPtr<int>(actoff, 0xe9c4) != 0xC135 &&
					*offsetPtr<int>(actoff, 0xe9c4) != 0xC136 &&
					*offsetPtr<int>(actoff, 0xe9c4) != 0xC137) {
					ndf = 0;
				}
				//��Ӻ���������
				if (*offsetPtr<int>(actoff, 0xe9c4) >= 49460 && *offsetPtr<int>(actoff, 0xe9c4) <= 49463) {
					if (iai_suc && *offsetPtr<float>(actoff, 0x10c) < 50.0f) {
						*offsetPtr<float>(PlayerBase, 0xE168) = 10.0f;
					}

					//�ھӺ�֡������60֡�Ϳ�ʼ��¼����
					//ͨ�����뻺�������Ԥ�����Ч��
					if (*offsetPtr<float>(actoff, 0x10c) >= 60.0f) {
						if (input[0] == 0 && input[1] == 0) {
							//��Ӻϳɹ�����
							if (iai_suc) {
								//��RT������,����ѭ��
								if ((!RE && Keys.RT > 0.0) || (RE && Keys.RB > 0)) {
									for (int RT_count = 0; RT_count < 8; RT_count++) {
										//ѭ������A������,����Ϊ���µ���RT+A��ϼ�,���ɵ����뻺��
										if (KeyA <=0 && Keys.A > 0) {
											input[0] = 3;
											input[1] = 0x62;
											ndf = 50;
											KeyA = 1.0;
											break;
										}
										if (Keys.Y > 0) {
											ndf = 50;
											break;
										}
										//ÿ��ѭ����һ֡�ӳ�
										std::this_thread::sleep_for(std::chrono::milliseconds(16));

										//��ѭ��5��Ҳû�а���������ϼ�������Ϊ���µ���RT��ͬʱ���㹻������������ն3���뻺��
										//����ϼ��ж���ʽ������Ϸ����,�����RT��5֡�ӳ�
										if (KeyRT <= 0.0 && RT_count == 7 && *offsetPtr<float>(playeroff, 0x2368) >= 0.2f) {
											input[0] = 3;
											input[1] = 0x44;
											KeyRT = 1.0;
										}
									}
								}
								//����Y������,����ѭ��
								else if (Keys.Y > 0) {
									for (int Y_count = 0; Y_count < 8; Y_count++) {
										//ѭ������RT������,����Ϊ���µ���RT+Y��ϼ�
										if ((!RE && Keys.RT > 0.0) || (RE && Keys.RB > 0)) {
											break;
										}
										//ÿ��ѭ����һ֡�ӳ�
										std::this_thread::sleep_for(std::chrono::milliseconds(16));

										//��ѭ��5��Ҳû�а���������ϼ�������Ϊ���µ���Y����̤��ն���뻺��
										//����ϼ��ж���ʽ������Ϸ����,�����Y��5֡�ӳ�
										if (KeyY <= 0 && Y_count == 7) {
											if (JoyL) {
												input[0] = 3;
												input[1] = 0x55;
											}
											else {
												input[0] = 3;
												input[1] = 0x6A;
											}
											KeyY = 1;
											break;
										}
									}
								}
								else if (Keys.IAI > 0) {
									input[0] = 3;
									input[1] = 0x62;
								}
							}
							//����Ҫ�Ӻϳɹ�������
							if ((!RE && Keys.RT > 0.0) || (RE && Keys.RB > 0)) {
								for (int RT_count = 0; RT_count < 8; RT_count++) {
									if (Keys.Y > 0) {
										ndf = 50;
										break;
									}
								}
							}
							//������RB,�򽫿����ɵ����뻺��
							if (Keys.X > 0 || (LS && Keys.LS > 0) || (!RE && Keys.RB > 0) || (RE && Keys.RT > 0.0)) {
								if (TK) {
									input[0] = 3;
									input[1] = 0x6B;
								}
								else {
									if (JoyL){
										input[0] = 3;
										input[1] = 9;
									}
									else {
										input[0] = 3;
										input[1] = 6;
									}
								}
							}
						}
					}

					//������������ʽ,����֡������70֡,�����������е���ʽ,��������
					//����ʽ������60֡�Ժ�ʼ������������,����70֡�Ժ��������,������Ϸ�����Ԥ���뷽ʽ
					if (*offsetPtr<float>(actoff, 0x10c) >= 70.0f) {
						if (input[0] != 0 && input[1] != 0) {
							if (input[1] == 6 || input[1] == 9){
								if (*offsetPtr<float>(actoff, 0x10c) >= 150.0f)
									fsm_derive(input[0], input[1]);
							}
							else
								fsm_derive(input[0], input[1]);
							input[0] = 0; input[1] = 0;
						}
					}
					KeyY = Keys.Y;
					KeyA = Keys.A;
					if (!RE)
						KeyRT = Keys.RT;
					else
						KeyRT = Keys.RB;
				}
				//������
				else { input[0] = 0; input[1] = 0; }

				//��Ӻϵ��˺�����
				if (!iai_suc && *offsetPtr<int>(actoff, 0xe9c4) >= 49460 && *offsetPtr<int>(actoff, 0xe9c4) <= 49463 && *offsetPtr<int>(playeroff, 0x2d24) != 0xFFFFFFFF) {
					*offsetPtr<int>(playeroff, 0x2d24) = 0xFFFFFFFF;
				}
				if (iai_suc) {
					if(*offsetPtr<int>(actoff, 0xe9c4) == 0xC134 || *offsetPtr<int>(actoff, 0xe9c4) == 0xC135)
						*offsetPtr<int>(playeroff, 0x2d24) = 0xC; 
					if ( *offsetPtr<int>(actoff, 0xe9c4) == 0xC136)
						*offsetPtr<int>(playeroff, 0x2d24) = 0xD;
					if (*offsetPtr<int>(actoff, 0xe9c4) == 0xC137)
						*offsetPtr<int>(playeroff, 0x2d24) = 0xE;
				}

				//��ӺϿ���
				if (*offsetPtr<BYTE>(playeroff, 0x2CED) == 1 && *offsetPtr<int>(actoff, 0xe9c4) >= 49460 && *offsetPtr<int>(actoff, 0xe9c4) <= 49463) {
					if (*offsetPtr<float>(playeroff, 0x2d10) != 0) {
						if(RS && *offsetPtr<int>(playeroff, 0x2370) >= 3) {
							if (riai && laisbl > 0) {
								*offsetPtr<float>(playeroff, 0x2374) += 0.2;
								laisbl -= 1;
								riai = 0;
							}
						}
						else {
							*offsetPtr<int>(playeroff, 0x2cec) = 1;
							spiritBlade_LvUP(playeroff);
						}
					}
				}
				else
					riai = 1;

				//��Ӻϵĳɹ���־λ�ж�
				if (iai_suc == false && *offsetPtr<BYTE>(playeroff, 0x2CED) == 1 && *offsetPtr<int>(actoff, 0xe9c4) >= 49460 && *offsetPtr<int>(actoff, 0xe9c4) <= 49463)
					iai_suc = true;
				else if (iai_suc == true && *offsetPtr<BYTE>(playeroff, 0x2CED) == 0 && (*offsetPtr<int>(actoff, 0xe9c4) < 49460 || *offsetPtr<int>(actoff, 0xe9c4) > 49463))
					iai_suc = false;

				if (*offsetPtr<int>(actoff, 0xe9c4) == 0xC155) {
					*offsetPtr<int>(PlayerBase, 0x76a8) = 0;
					if (*offsetPtr<float>(actoff, 0x10c) >= 105.0f) {
						fsm_derive(1, 0);
					}
				}

				//���л���
				if (RS){
					//�ж���ɫ
					if (*offsetPtr<int>(playeroff, 0x2370) >= 3 && lsp >= 3) {
						WriteProcessMemory(hprocess, (LPVOID)0x142123C1B, blade_efx, sizeof(blade_efx), NULL);
						*offsetPtr<float>(playeroff, 0x2388) = 1;
						if (rs) {
							if (*offsetPtr<int>(actoff, 0xe9c4) == 0xC08C) {
								//���Ƴɹ�����
								if (*offsetPtr<BYTE>(playeroff, 0x239a) == 1) {
									if (fsb) {
										*offsetPtr<float>(playeroff, 0x2374) += 0.1;
										fsb = 0;
									}
								}
								//���ƶ�������
								if (rfs) {
									*offsetPtr<float>(playeroff, 0x2374) -= 0.2;
									rfs = 0;
								}
							}
							else { 
								rfs = 1;
								fsb = 1;
							}
							//���ж�������
							if (*offsetPtr<int>(playeroff, 0x2cec) == 1 && !iai_suc) {
								*offsetPtr<int>(playeroff, 0x2cec) = 0;
								if (sbl > 0) {
									*offsetPtr<float>(playeroff, 0x2374) += 0.2;
									sbl -= 1;
								}
							}
						}
						if (*offsetPtr<float>(playeroff, 0x2374) > 1)
							*offsetPtr<float>(playeroff, 0x2374) = 1;
						*offsetPtr<float>(playeroff, 0x2368) = 1;
						rs = 1;
					}
					else {
						WriteProcessMemory(hprocess, (LPVOID)0x142123C1B, not_blade_efx, sizeof(not_blade_efx), NULL);
						if (rs == 1) {
							rs = 0;
							sbl = 3;
							laisbl = 1;
							*offsetPtr<float>(playeroff, 0x2368) = 0;
							*offsetPtr<float>(playeroff, 0x2388) = 0;
						}
						if (rh > 0) {
							rh--;
							*offsetPtr<float>(playeroff, 0x2368) += 0.02;
						}
					}
					if (*offsetPtr<float>(playeroff, 0x2378) > 0) {
						*offsetPtr<float>(playeroff, 0x2378) = 0;
						rh = 7;
					}
					
				}
			}
		}
	}
}
__declspec(dllexport) extern bool Load()
{
	load_json();
	thread th1(asm_edit);
	th1.detach();
	thread th2(mian_loop);
	th2.detach();
	thread th3(GetNowKey);
	th3.detach();
	return true;
}


BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
		return Load();

	return TRUE;
}


