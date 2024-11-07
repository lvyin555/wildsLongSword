#pragma once
#include <stdio.h>
#include <vector>
#include <string>
#include <map>
#include "imgui.h"  
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <d3d11.h>
#include <dxgi.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <tchar.h>
#include "kiero.h"
#include <implot.h>
#include "IconsFontAwesome5.h"
#pragma comment ( lib, "D3D11.lib")

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
//extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace ControlProgram {
	bool initConsole = false;
	bool init = false;
	typedef HRESULT(__stdcall* Present) (IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
	typedef LRESULT(CALLBACK* WNDPROC)(HWND, UINT, WPARAM, LPARAM);
	typedef uintptr_t PTR;
	LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	Present oPresent;
	Present oResize;
	HWND window = NULL;
	WNDPROC oWndProc;
	ID3D11Device* pDevice = NULL;
	ID3D11DeviceContext* pContext = NULL;
	ID3D11RenderTargetView* mainRenderTargetView;
	HMODULE hMod;

	static map<void*, bool> MonstersState;
	//纹理缓存
	struct TextureCache {
		int width = 0;
		int height = 0;
		ID3D11ShaderResourceView* texture = NULL;
		TextureCache(
			int width = 0,
			int height = 0,
			ID3D11ShaderResourceView* texture = NULL
		) :width(width), height(height), texture(texture) { };
	};
	map<string, TextureCache> ImgTextureCache;


	bool GameInit = false;
	
	void InitImGui()
	{
		ImGui::CreateContext();
		ImPlot::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		auto fonts = ImGui::GetIO().Fonts;
		fonts->AddFontFromFileTTF("c:/windows/fonts/simhei.ttf", 13.0f, NULL, fonts->GetGlyphRangesChineseFull());
		static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
		ImFontConfig icons_config; icons_config.MergeMode = true; icons_config.PixelSnapH = true;
		fonts->AddFontFromFileTTF(FONT_ICON_FILE_NAME_FAS, 16.0f, &icons_config, icons_ranges);
		io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
		ImGui_ImplWin32_Init(window);
		ImGui_ImplDX11_Init(pDevice, pContext);
	}
	//加载图片并写入纹理
	void LoadTexture(unsigned char* image_data, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height, int image_width, int image_height) {
		// Create texture
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Width = image_width;
		desc.Height = image_height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;

		ID3D11Texture2D* pTexture = NULL;
		D3D11_SUBRESOURCE_DATA subResource;
		subResource.pSysMem = image_data;
		subResource.SysMemPitch = desc.Width * 4;
		subResource.SysMemSlicePitch = 0;
		pDevice->CreateTexture2D(&desc, &subResource, &pTexture);

		// Create texture view
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZeroMemory(&srvDesc, sizeof(srvDesc));
		srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = desc.MipLevels;
		srvDesc.Texture2D.MostDetailedMip = 0;
		pDevice->CreateShaderResourceView(pTexture, &srvDesc, out_srv);
		pTexture->Release();

		*out_width = image_width;
		*out_height = image_height;
		stbi_image_free(image_data);
	}
	bool LoadTextureFromFile(const char* filename, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height)
	{
		// Load from disk into a raw RGBA buffer
		int image_width = 0;
		int image_height = 0;
		unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
		if (image_data == NULL)
			return false;
		LoadTexture(image_data, out_srv, out_width, out_height, image_width, image_height);
		return true;
	}
	bool LoadTextureFromBase(string Base64Data, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height, int image_width, int image_height)
	{
		unsigned char* image_data = Base::Calculation::Base64ToImg(Base64Data);
		if (image_data == NULL)
			return false;
		LoadTexture(image_data, out_srv, out_width, out_height, image_width, image_height);
		return true;
	}

	HRESULT __stdcall hkResize(IDXGISwapChain* pSwapChain,UINT SyncInterval, UINT Flags) {
		mainRenderTargetView->Release();
		mainRenderTargetView = nullptr;
		return oResize(pSwapChain, SyncInterval, Flags);
	}

	HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
	{
		if (!init)
		{
			if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice)))
			{
				pDevice->GetImmediateContext(&pContext);
				DXGI_SWAP_CHAIN_DESC sd;
				pSwapChain->GetDesc(&sd);
				window = sd.OutputWindow;
				ID3D11Texture2D* pBackBuffer;
				pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
				pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
				pBackBuffer->Release();
				oWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);
				InitImGui();
				init = true;
			}

			else
				return oPresent(pSwapChain, SyncInterval, Flags);
		}
		if (mainRenderTargetView == nullptr) {
			ID3D11Texture2D* pBackBuffer;
			pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
			pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
			pBackBuffer->Release();
		}

		void* MapPlot = *(undefined**)MH::Player::PlayerBasePlot;
		void* MapPlotOffset1 = nullptr;
		if (MapPlot != nullptr)
			MapPlotOffset1 = *offsetPtr<undefined**>((undefined(*)())MapPlot, 0x50);
		void* MapPlotOffset2 = nullptr;
		if (MapPlotOffset1 != nullptr)
			MapPlotOffset2 = *offsetPtr<undefined**>((undefined(*)())MapPlotOffset1, 0x7D20);
		int MapId = 0;
		if (MapPlotOffset2 != nullptr)
			MapId = *offsetPtr<int>((int(*)())MapPlotOffset2, 0xB88);
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();		
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
			if (Base::Monster::Monsters.empty())
				window_flags |= ImGuiWindowFlags_NoBackground;
			if (!GameInit) {
				ImGui::Begin(u8"初始化", NULL, window_flags);
				static float progress = 0.0f, progress_dir = 1.0f;
				progress += progress_dir * 0.4f * ImGui::GetIO().DeltaTime;
				if (progress >= +1.1f) { GameInit = true; }
				ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f), "");
				ImGui::End();
				window_flags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;
				ImGui::SetNextWindowBgAlpha(0.10f);
				ImGui::SetNextWindowPos(ImVec2(
					ImGui::GetMainViewport()->Pos.x + ImGui::GetMainViewport()->Size.x * 0.8f,
					ImGui::GetMainViewport()->Pos.y + ImGui::GetMainViewport()->Size.y * 0.8f
				), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
				ImGui::Begin("LOGO", NULL, window_flags);
				ImGui::SetWindowFontScale(3);
				ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 0.8f), u8" Created By Caimogu.net/云雾敛");
				ImGui::SameLine();
				ImGui::End();
			}
			else {			
				window_flags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;
				ImGui::SetNextWindowPos(ImVec2(
					ImGui::GetMainViewport()->Pos.x + ImGui::GetMainViewport()->Size.x * 0.5f,
					ImGui::GetMainViewport()->Pos.y + ImGui::GetMainViewport()->Size.y * 0.87f
				), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

				/*
					图形绘制
					name: 图片名，随意写
					alpha: 透明度
					Channel:通道
					pos:绘制位置
					img:图片数据、文件路径
					Base64:是否使用base64图片数据进行绘制
					width:绘制区域宽度
					height:绘制区域高度
					Base::Draw::Img[name] = Base::Draw::NewImage(alpha, Channel, pos, name, img, Base64, width, height);
				*/				
				if (MapId != 301 && MapId != 302 && MapId != 503 && MapId != 303
					&& MapId != 305 && MapId != 306 && Base::PlayerData::Weapons::WeaponType == 3) {
					ImGui::Begin("bugs", NULL, window_flags);
					ImGui::SetWindowFontScale(2.0);
					for (int i = 2; i >= 0; i--) {
						if (Base::PlayerData::wirebugs[i] <= 0)
							Base::Draw::Img["insect" + to_string(i)] = Base::Draw::NewImage(0.8, Base::Vector3(1, 1, 1), Base::Vector2(0.54 - (0.04 * i), 0.87), "insect" + to_string(i),
								"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////AP///wD///8AVJmILlifjzxnr6F0kMm/rLrf2NS74dnklM/D1Wy7rLxlt6ewhMW4uqjWzs/A4tzhxuXd5sPj2+W/4NnjwuHb49Hq5url9PH25vPx/cnl3/Oo1czaqdbN08bk3ufU6ubzx+Td7bbc1OOx2tPes9rT4azWztux2dLZyeXf7Nrt6fS/39nZjMa7rGu1qHNdrp88W66fF2CxowNut6gAe6qeAHR+ewBYVVMAQEY+ADlEOgA7RjsAPUc9AD1HPQD///8A////AP///wBWnI04XKOUSHG2qYSo1s7Azenk5Lng2OWOzMDOd8CyvobHusG13NTW3u/s7PP5+fr3+/r98/n3/PD39fry9/b5+fv8/P7+/v/x+fj+zOfi8qjWzN6r2M7ax+Xf6tDp5PHC4drpr9nQ4afVzNmq1s3Ys9rT3cDg2ufT6eXxx+Pe45rOxLlwuaqCYKydTF6mlx9gpJgHY6idAGyuowB2o5oAcnt6AFhVUwBARz4AOkU6ADtFOwA9Rz0APUc9AP///wD///8A////AFadjztdpZdMdbmsiq/Z0sfK5+LnqtjQ34vJvcmTzMDIuN3W2eXy7/H9/v79//////////////////////////////////////3+/v/o9fL7yOXf77Ha0uS33dbgxuTd6Mbk3fC43tXoqdbN2azXz9bD4tzm1Orl88nj3+ag0MjAd72vjGCxn1VapJQnX5aPCmSQjQBlk5EAaJmXAG2TkQBudngAWFZUAEFIPwA7RTsAO0U7AD1HPQA9Rz0A////AP///wD///8AVJ2QOVymmEt2uq2KrNjQxr3h2uSe0cjYmc3Dy77f2Nrn8/Dx/v7+//////////////////////////////////////////////////3+/v/u9/b8z+jj8rXb0+O33dXhxOTd6sfl3+3G5N7ryebh6tbs5/PR6eTvqdXMzH2/spVks6NfV6qZL1WdkA9ejooBZoWIAGeGiQBkjY8AZ4qMAGtzdgBZV1QAQUg/ADtFOwA7RTsAPUc9AD1HPQD///8A////AP///wBUn5E3W6eZSXa6rYis187EvN/X5KnUzNy32tTX4/Ht7/j9+/75/fz/+fz7//7+/v/////////////////////////////////////////////////z+Pj90ejk87Pa0+Ox29Lbyuji5+b18/nq+PX72O/q8bLb09KGxbiha7WmaF6snTVTpJUTUpmPA12OigBmh4kAZ4iKAGSOkABmi40Aa3R2AFhXVABBRz8AO0U7ADtFOwA9Rz0APUc9AP///wD///8A////AFiikzdeqJtJdrqth63XzsPI493nxODa6dPo5Ojr9/P55PXw+9nv6fLn9PH2+fz7/f/////////////////////////////////////////////////////v9vb9weDb6qLTyte03dbY1O3q6Nvw7O2+4tzZkcrAqXG5q3Bmrp48XqaXFVOgkgNRmY8AXI+MAGaKigBni4wAZZGRAGiNjgBsdXYAWFhUAEFIPwA7RTsAO0U7AD1HPQA9Rz0A////AP///wD///8AXqOVNmKpm0d0uKyDptPKvM3l4OHY6+bw4PDs9N3x7PnA5Nzts93U4tPr5uz0+vj8//////////////////////////////////////////////////////z8/f/W6ufxpNPL25PNw8qc0snIo9TMxpTMw615vrF5abOkQ2Wqmhtfo5MGVZ+QAFGZjwBcj4wAZoqKAGeLjABlkZEAaI2OAGx1dwBYWFUAQUg/ADpFOgA7RTsAPUc9AD1HPQD///8A////AP///wBgoZQuYaaYPmuypHWPx7ymud3Wy9Xr5uff8ez1zenk9ajXz+Oe0sjWxuTg5fD59/r///////////////////////////////////////////////////////7//+fx7/av2NDggcS3xHC9rq9yu66cb7iqemm0pUllsaAfZqubB2CjkwBVn5EAUZmPAFyPjABmiYsAZ4qMAGWQkQBojI4Aa3R2AFlXVABBRz4AOUQ6ADtGOwA9Rz0APUc9AP///wD///8A////AF2bjSBanpAtXaeZW262qIeSysCtvt/Z0tnt6ezO6eP0qNbO45zQx9TC493j7/j3+f//////////////////////////////////////////////////////////8Pf1+rze1+aCxLfFYLOjn1qrnHhcqZtNYKucI2WwnwlmrZwAYKaWAFSfkgBQmZAAW46NAGWIiwBnh4wAZY6RAGeJjQBqcHQAWVRSAEFGPQA5RDkAO0Y7AD1HPQA9Rz0A////AP///wD///8AVo+CEFOThRhPm4w6VKaYZHC4rZCm0sy82Ovp4eDx7ffA4tvvrtnQ4crn4erx+Pj6//////////////////////7////////////////////////////////////1+fn+yebg7Y7Kv8pms6SaV6aXY1egkTFdo5QOZKubAGesmwBhppYAVp+SAFKakABckI0AZYqLAGeKjABlkJAAZ4uMAGp2dwBaVlQAP0I5ADU+MwA4QTYAO0I5ADtEOgD///8A////AP///wBSf3gET4V7CEiRgh5InY1FYK6ieZjKxK7a6+nc9/z6/Of18f3W7Of24/Pw+Pv9/f7///////////v9/P/t9vT46PTx9PP6+Pr+/v7///////////////////////r9/f/Y7er2n9LJ1HK5rJ5eqJhfXJ2NKWKdjQZmo5IAaKWUAGWhkQBfn44AXpyOAGOXjABnlIsAZ5WMAGeYjgBpmZAAbJGHAFprYwA9QTkAMDIpADQ4LgA3PDMAOD0zAP///wD///8A////AFN1dABQfXkBSY6CDkmdji5draBkjcW9ncrk4M7w+ffw9Pz6+u749v31+/r/////////////////6vXy+sXj3ei53dfe0Ojk5+739PX+/v3+/////////////////P7+/+Dy8Pus2NHae76xpGOrm2Rin44raZyMCGqdjQBrno4Aa56OAGqejgBqn44Aa5+OAGugjgBqoI8Aa6CPAG2mlQBvqJYAYYx9AEhcUgA8QzoAP0c+AEBKQQA+ST8A////AP///wD///8AVnZ4AFSAfwBRk4wFUqKWG16toEZ5vLF6n9DHqL7g2srP6OTb2ezq5eTx7+zu9/Xz8fn3+OX08fXC49zlls3DzofFu8Gj08rK0url4/f7+vn////////////////8/v7/3/Px+qvZ0th7vbGhZaqbYmWejypqmowIa5qMAGubjQBsnY4AbZ2PAG2djwBsn48Aa6CQAGugkABrn5AAbKKTAG6nlwBpn48AYo2AAF+FeQBgh3wAYYh9AGCIfQD///8A////AP///wBfg4UAX5CPAF+loAFhtaoMY7iqJ2m4qk10uq9zhMK3kpXKwael0cu2s9jSxL/g29HD5N7atN3W15TNwsRxu62wZbWmp4PDt7S+4NnU8fj39P////////////////n9/P/X7uz0otTOz3S5sJdhpZlaXpiKJmCShAdikoQAYpSGAGOVhgBjlYYAY5WGAGOVhwBilYcAYpWHAGKVhwBilYcAYpeIAGOaiwBlno4AZ6CRAGehkgBnoZEAZ6GSAP///wD///8A////AF6FhQBdjo0AXZ+aAF6pnwJgr6AOYbChIl+soDxhq55VZ66iaHK0qnh7vLGIg8O3mIbGuqN/w7efbrqrkluwn4pYrZyPeL2vqLbc1c/w+Pby////////////////9fv6/87p5u2WzcfDa7OriFqflUxVj4IdVYd3BFWHdgBWiXgAVol5AFaJeQBWiXkAVol5AFaJeQBWiXkAVYp5AFaKeQBWiXkAVot7AFiOfgBZkH8AWZGAAFmRgABakYAA////AP///wD///8AL0NEAC5HRgAsSkcAKktGADdiWQFOjX8IWKGUFFOcjyJSm44uVqGTO1qnmUtdq5xaX66fY1+un19aq5tYU6mXYVarmXp2vK6htdvVzvD49/L////////////////y+vj9xOPg5YrGv7dirKR6U5qRP1GKfhZTgnICUoFwAFKDcgBSg3MAUoNzAFKDcwBSg3MAUoNzAFKDcwBShHQAU4R0AFOEdABThHMAU4NzAFOEcwBUhHQAU4V0AFSFdQD///8A////AP///wAAAAAAAAAAAAAAAAAAAAAACREPACVFPQFAe3ADS5OFB06cjQxQnY4TUp2OHVWfkSdapJYsXqaYK1unmDFVqJdLWK2bc3i9r6K229XR8Pj39P///////////////+z39Pq23Nbafb61p1umnmpQlYwyUoh8DlWBcgFWgHAAVoJyAFaCcwBWgnMAVoJzAFaCcwBWgnMAVoJzAFaCcwBWgnMAVoJzAFaCcwBWgnMAVYJzAFaCcwBVgnMAVYJzAP///wD///8A////AAAAAAAAAAAAAAAAAAAAAAAAAAAAAggHACBDPQBHi34AVKSWAFCdjgFQl4cDW5qOBmqkmgpwq6ETaq2fK1+tnFRhsaCCgcK1r73f2tj0+/r3///////////6//7/1u3p7Z3QyMJwt6yLWKOaUVCRiiFThnsHVoFyAFeBcQBXgnMAV4JzAFeCcwBXgnMAV4JzAFeCcwBXgnMAV4JzAFeCcwBXgnQAV4J0AFeCdABXg3QAV4N1AFiDdQBYg3UA////AP///wD///8AAAAAAAAAAAAAAAAAAAAAAAAAAAELCAkAKzo2AE59cQBZk4QAUo19AFCOfgBhmo4Ad6igA32xqBlzs6VCarOjcnW6rJ+bzsXH1Onl5/39/fz/////+f/+/9/z8fCw29TOgsS5m2izpmRcopk0V5GKEViKgQJbiHwAW4l8AFuJfQBbiX0AW4l9AFuJfQBbiX0AW4l9AFuJfQBbiX0AW4l9AFuJfQBbiX0AW4l9AFuJfQBaiX0AWol9AFqIfAD///8A////AP///wACAQIAAgECAAIAAgABAAEADggMADUsMQBWWFYAXWxgAFxvXwBWd2YAVI18AGOikwB2rJ8MerGjL3O0pWN2uqyUlMm/vsPh3OLv9vT5//////r////d8u/xr9vUzIfGvJxuuq1oY7ChOGCilxhglI4FYZCKAGKRiwBik4sAYpOMAGKTjABik4wAYpOMAGKTjABik4wAYpOMAGKTjABik4wAYpOMAGKTjABik4wAY5OMAGOTjABjk4wAY5ONAP///wD///8A////ACskKwAqIyoAKCAoACYeJgA4LTcAWU5YAGlkZgFjYlgBX19PAV52YwBgnIoAZ7GfAW6vnhZwrp1Bc7anfInEua+529XX6vTx9//////7/f3+3fDt7a/a086GxrudbbiraF+woTZcqZoVXp+TBmOWjwBlk48AZJWRAGSWkABklZAAZJWQAGSVkABklZAAZJWQAGSVkABklZAAZJWQAGSVkABklZAAZJWQAGSVkABklZAAZJWQAGSVkABklZEA////AP///wD///8AXWFdAFxhXABcX1wAWl5aAF1gXQBiZmIBYGxfA1ppVQNVXkwBV2lcAF6OgQBiqpoCYq2cHGevn016u62KoNDHwM/o4+Tx+Pf28vn39tfs6OWs19DGh8W5m3K5qmlnsqE6XqyaFViklANbnI8AYpWNAGSSjwBjlI8AY5WPAGOUjwBjlI8AY5SPAGOUjwBjlI8AY5SPAGOUjwBjlI8AY5SPAGOUjwBjlI8AY5SPAGOUjwBjlI8AY5SPAGOUjwD///8A////AP///wBliWUAZYllAGWJZQBmiWYAY4ZjAF+DXwJXg1gETHZMBD9SPAI7OzkAR1JRAFOAeARappggY7WnUX/BtY+p1c7ExuTg3Mrl4tq53NnKnc7HsILAtYxwtqdhbLKgOGyynRhmrpgEWqWSAFmbjgBglI4AZJKOAGOUjgBjlI4AY5SOAGOUjgBjlI4AY5SOAGOUjgBjlI4AY5SOAGOUjgBjlI4AY5SOAGOUjgBjlI4AY5SOAGOUjgBjlI4AY5SOAP///wD///8A////AFeDVwBXhFcAWIVYAFmFWQBZhlkAWYhZAlOFVANEbEUELz0tAiQTGwAxHSkASFxeBFqimB5kvK5LecC0gZbLxaqg0c20j8nEpnu+uI1stKtwY66hT2Cqmy9nrJoUcrKbBm6xmgBfqJQAWZuPAGCUjgBkko4AY5SOAGOUjgBjlI4AY5SOAGOUjgBjlI4AY5SOAGOUjgBjlI4AY5SOAGOUjgBjlI4AY5SOAGOUjgBjlI4AY5SOAGOUjgBjlI4A////AP///wD///8AUGxQAFBsUABQbVAAUW1RAFJwUgBTclMBUGhQAUNPRAIwLzACJRcdADAiJwBLYF4EY6ebF2vAsDlvu65fdrqxdHS4sXRnsatlWqihT1KimTZRoZUeVqKUDmKnlgJxsJoAcLKbAGCplgBZm48AYJSOAGSSjgBjlI4AY5SOAGOUjgBjlI4AY5SOAGOUjgBjlI4AY5SOAGOUjgBjlI4AY5SOAGOUjgBjlI4AY5SOAGOUjgBjlI4AY5SOAGOUjgD///8A////AP///wBPYU8AT2FPAE9gTwBQYFAAUWRRAFFkUQBOVk4ARkBGATwwPAE1NTABOk81AlR9YgRvqpgNdbytHm25qDJksKI6X6ieNlmhmipSmZMcTZaPD02ckQRToJIAYKWVAG+vmgBvspsAYKmWAFmbjwBhlI4AZJOOAGOUjgBjlI4AY5SOAGOUjgBjlI4AY5SOAGOUjgBjlI4AY5SOAGOUjgBjlI4AY5SOAGOUjgBjlI4AY5SOAGOUjgBjlI4AY5SOAP///wD///8A////AE9kTwBPZE8AT2NPAFBjUABRZ1EAUWZRAE1ZTQBIQ0gARTlFAUJLPwJDbj8DWY9gBHOejAd1pZwLa6maEWOpmhJepJkPWJqSClOPjARPkIwBUJuQAFWikwBgp5YAb6+aAG+zmwBgqZYAWZuPAGGUjgBkk44AY5SOAGOUjgBjlI4AY5SOAGOUjgBjlI4AY5SOAGOUjgBjlI4AY5SOAGOUjgBjlI4AY5SOAGOUjgBjlI4AY5SOAGOUjgBjlI4A////AP///wD///8AUGVQAFBlUABQZVAAUGVQAFFoUQBRaFEATVtNAEhFSABHPkcAR05FAUpwRwJch2EEboZ7BWp8fQVhhn4DYZ6RAWGomwBanJMAVY6LAFKOjABSmpEAVqOUAGGolwBwr5sAb7ObAGCplgBZm48AYZSOAGSTjgBjlI4AY5SOAGOUjgBjlI4AY5SOAGOUjgBjlI4AY5SOAGOUjgBjlI4AY5SOAGOUjgBjlI4AY5SOAGOUjgBjlI4AY5SOAGOUjgD///8A////AP///wBQZVAAUGVQAFBlUABQZVAAUWhRAFFoUQBNXE0AR0ZHAEU8RQBIS0UATWVLAl16YQRodHAFYGJoBFhsbAJelYsAYqyeAFyflgBWkI0AUpCNAFKbkQBWo5QAYaeXAHCvmwBvs5sAYKmWAFmbjwBhlI4AZJOOAGOUjgBjlI4AY5SOAGOUjgBjlI4AY5SOAGOUjgBjlI4AY5SOAGOUjgBjlI4AY5SOAGOUjgBjlI4AY5SOAGOUjgBjlI4AY5SOAP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AA=="
								, true, 50, 50);
						else {
							/*ostringstream ptr;
							ptr << Base::PlayerData::wirebugs[i];
							string pp = ptr.str();
							ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 0.8f), pp.c_str());*/
							float value = Base::PlayerData::wirebugs[i] / 30.0f;
							Base::Draw::Img["insect" + to_string(i)] = Base::Draw::NewImage(0.2 + value, Base::Vector3(1, 0.2, 0.2), Base::Vector2(0.54 - (0.04 * i), 0.87), "insect" + to_string(i),
								"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////AP///wD///8AVJmILlifjzxnr6F0kMm/rLrf2NS74dnklM/D1Wy7rLxlt6ewhMW4uqjWzs/A4tzhxuXd5sPj2+W/4NnjwuHb49Hq5url9PH25vPx/cnl3/Oo1czaqdbN08bk3ufU6ubzx+Td7bbc1OOx2tPes9rT4azWztux2dLZyeXf7Nrt6fS/39nZjMa7rGu1qHNdrp88W66fF2CxowNut6gAe6qeAHR+ewBYVVMAQEY+ADlEOgA7RjsAPUc9AD1HPQD///8A////AP///wBWnI04XKOUSHG2qYSo1s7Azenk5Lng2OWOzMDOd8CyvobHusG13NTW3u/s7PP5+fr3+/r98/n3/PD39fry9/b5+fv8/P7+/v/x+fj+zOfi8qjWzN6r2M7ax+Xf6tDp5PHC4drpr9nQ4afVzNmq1s3Ys9rT3cDg2ufT6eXxx+Pe45rOxLlwuaqCYKydTF6mlx9gpJgHY6idAGyuowB2o5oAcnt6AFhVUwBARz4AOkU6ADtFOwA9Rz0APUc9AP///wD///8A////AFadjztdpZdMdbmsiq/Z0sfK5+LnqtjQ34vJvcmTzMDIuN3W2eXy7/H9/v79//////////////////////////////////////3+/v/o9fL7yOXf77Ha0uS33dbgxuTd6Mbk3fC43tXoqdbN2azXz9bD4tzm1Orl88nj3+ag0MjAd72vjGCxn1VapJQnX5aPCmSQjQBlk5EAaJmXAG2TkQBudngAWFZUAEFIPwA7RTsAO0U7AD1HPQA9Rz0A////AP///wD///8AVJ2QOVymmEt2uq2KrNjQxr3h2uSe0cjYmc3Dy77f2Nrn8/Dx/v7+//////////////////////////////////////////////////3+/v/u9/b8z+jj8rXb0+O33dXhxOTd6sfl3+3G5N7ryebh6tbs5/PR6eTvqdXMzH2/spVks6NfV6qZL1WdkA9ejooBZoWIAGeGiQBkjY8AZ4qMAGtzdgBZV1QAQUg/ADtFOwA7RTsAPUc9AD1HPQD///8A////AP///wBUn5E3W6eZSXa6rYis187EvN/X5KnUzNy32tTX4/Ht7/j9+/75/fz/+fz7//7+/v/////////////////////////////////////////////////z+Pj90ejk87Pa0+Ox29Lbyuji5+b18/nq+PX72O/q8bLb09KGxbiha7WmaF6snTVTpJUTUpmPA12OigBmh4kAZ4iKAGSOkABmi40Aa3R2AFhXVABBRz8AO0U7ADtFOwA9Rz0APUc9AP///wD///8A////AFiikzdeqJtJdrqth63XzsPI493nxODa6dPo5Ojr9/P55PXw+9nv6fLn9PH2+fz7/f/////////////////////////////////////////////////////v9vb9weDb6qLTyte03dbY1O3q6Nvw7O2+4tzZkcrAqXG5q3Bmrp48XqaXFVOgkgNRmY8AXI+MAGaKigBni4wAZZGRAGiNjgBsdXYAWFhUAEFIPwA7RTsAO0U7AD1HPQA9Rz0A////AP///wD///8AXqOVNmKpm0d0uKyDptPKvM3l4OHY6+bw4PDs9N3x7PnA5Nzts93U4tPr5uz0+vj8//////////////////////////////////////////////////////z8/f/W6ufxpNPL25PNw8qc0snIo9TMxpTMw615vrF5abOkQ2Wqmhtfo5MGVZ+QAFGZjwBcj4wAZoqKAGeLjABlkZEAaI2OAGx1dwBYWFUAQUg/ADpFOgA7RTsAPUc9AD1HPQD///8A////AP///wBgoZQuYaaYPmuypHWPx7ymud3Wy9Xr5uff8ez1zenk9ajXz+Oe0sjWxuTg5fD59/r///////////////////////////////////////////////////////7//+fx7/av2NDggcS3xHC9rq9yu66cb7iqemm0pUllsaAfZqubB2CjkwBVn5EAUZmPAFyPjABmiYsAZ4qMAGWQkQBojI4Aa3R2AFlXVABBRz4AOUQ6ADtGOwA9Rz0APUc9AP///wD///8A////AF2bjSBanpAtXaeZW262qIeSysCtvt/Z0tnt6ezO6eP0qNbO45zQx9TC493j7/j3+f//////////////////////////////////////////////////////////8Pf1+rze1+aCxLfFYLOjn1qrnHhcqZtNYKucI2WwnwlmrZwAYKaWAFSfkgBQmZAAW46NAGWIiwBnh4wAZY6RAGeJjQBqcHQAWVRSAEFGPQA5RDkAO0Y7AD1HPQA9Rz0A////AP///wD///8AVo+CEFOThRhPm4w6VKaYZHC4rZCm0sy82Ovp4eDx7ffA4tvvrtnQ4crn4erx+Pj6//////////////////////7////////////////////////////////////1+fn+yebg7Y7Kv8pms6SaV6aXY1egkTFdo5QOZKubAGesmwBhppYAVp+SAFKakABckI0AZYqLAGeKjABlkJAAZ4uMAGp2dwBaVlQAP0I5ADU+MwA4QTYAO0I5ADtEOgD///8A////AP///wBSf3gET4V7CEiRgh5InY1FYK6ieZjKxK7a6+nc9/z6/Of18f3W7Of24/Pw+Pv9/f7///////////v9/P/t9vT46PTx9PP6+Pr+/v7///////////////////////r9/f/Y7er2n9LJ1HK5rJ5eqJhfXJ2NKWKdjQZmo5IAaKWUAGWhkQBfn44AXpyOAGOXjABnlIsAZ5WMAGeYjgBpmZAAbJGHAFprYwA9QTkAMDIpADQ4LgA3PDMAOD0zAP///wD///8A////AFN1dABQfXkBSY6CDkmdji5draBkjcW9ncrk4M7w+ffw9Pz6+u749v31+/r/////////////////6vXy+sXj3ei53dfe0Ojk5+739PX+/v3+/////////////////P7+/+Dy8Pus2NHae76xpGOrm2Rin44raZyMCGqdjQBrno4Aa56OAGqejgBqn44Aa5+OAGugjgBqoI8Aa6CPAG2mlQBvqJYAYYx9AEhcUgA8QzoAP0c+AEBKQQA+ST8A////AP///wD///8AVnZ4AFSAfwBRk4wFUqKWG16toEZ5vLF6n9DHqL7g2srP6OTb2ezq5eTx7+zu9/Xz8fn3+OX08fXC49zlls3DzofFu8Gj08rK0url4/f7+vn////////////////8/v7/3/Px+qvZ0th7vbGhZaqbYmWejypqmowIa5qMAGubjQBsnY4AbZ2PAG2djwBsn48Aa6CQAGugkABrn5AAbKKTAG6nlwBpn48AYo2AAF+FeQBgh3wAYYh9AGCIfQD///8A////AP///wBfg4UAX5CPAF+loAFhtaoMY7iqJ2m4qk10uq9zhMK3kpXKwael0cu2s9jSxL/g29HD5N7atN3W15TNwsRxu62wZbWmp4PDt7S+4NnU8fj39P////////////////n9/P/X7uz0otTOz3S5sJdhpZlaXpiKJmCShAdikoQAYpSGAGOVhgBjlYYAY5WGAGOVhwBilYcAYpWHAGKVhwBilYcAYpeIAGOaiwBlno4AZ6CRAGehkgBnoZEAZ6GSAP///wD///8A////AF6FhQBdjo0AXZ+aAF6pnwJgr6AOYbChIl+soDxhq55VZ66iaHK0qnh7vLGIg8O3mIbGuqN/w7efbrqrkluwn4pYrZyPeL2vqLbc1c/w+Pby////////////////9fv6/87p5u2WzcfDa7OriFqflUxVj4IdVYd3BFWHdgBWiXgAVol5AFaJeQBWiXkAVol5AFaJeQBWiXkAVYp5AFaKeQBWiXkAVot7AFiOfgBZkH8AWZGAAFmRgABakYAA////AP///wD///8AL0NEAC5HRgAsSkcAKktGADdiWQFOjX8IWKGUFFOcjyJSm44uVqGTO1qnmUtdq5xaX66fY1+un19aq5tYU6mXYVarmXp2vK6htdvVzvD49/L////////////////y+vj9xOPg5YrGv7dirKR6U5qRP1GKfhZTgnICUoFwAFKDcgBSg3MAUoNzAFKDcwBSg3MAUoNzAFKDcwBShHQAU4R0AFOEdABThHMAU4NzAFOEcwBUhHQAU4V0AFSFdQD///8A////AP///wAAAAAAAAAAAAAAAAAAAAAACREPACVFPQFAe3ADS5OFB06cjQxQnY4TUp2OHVWfkSdapJYsXqaYK1unmDFVqJdLWK2bc3i9r6K229XR8Pj39P///////////////+z39Pq23Nbafb61p1umnmpQlYwyUoh8DlWBcgFWgHAAVoJyAFaCcwBWgnMAVoJzAFaCcwBWgnMAVoJzAFaCcwBWgnMAVoJzAFaCcwBWgnMAVYJzAFaCcwBVgnMAVYJzAP///wD///8A////AAAAAAAAAAAAAAAAAAAAAAAAAAAAAggHACBDPQBHi34AVKSWAFCdjgFQl4cDW5qOBmqkmgpwq6ETaq2fK1+tnFRhsaCCgcK1r73f2tj0+/r3///////////6//7/1u3p7Z3QyMJwt6yLWKOaUVCRiiFThnsHVoFyAFeBcQBXgnMAV4JzAFeCcwBXgnMAV4JzAFeCcwBXgnMAV4JzAFeCcwBXgnQAV4J0AFeCdABXg3QAV4N1AFiDdQBYg3UA////AP///wD///8AAAAAAAAAAAAAAAAAAAAAAAAAAAELCAkAKzo2AE59cQBZk4QAUo19AFCOfgBhmo4Ad6igA32xqBlzs6VCarOjcnW6rJ+bzsXH1Onl5/39/fz/////+f/+/9/z8fCw29TOgsS5m2izpmRcopk0V5GKEViKgQJbiHwAW4l8AFuJfQBbiX0AW4l9AFuJfQBbiX0AW4l9AFuJfQBbiX0AW4l9AFuJfQBbiX0AW4l9AFuJfQBaiX0AWol9AFqIfAD///8A////AP///wACAQIAAgECAAIAAgABAAEADggMADUsMQBWWFYAXWxgAFxvXwBWd2YAVI18AGOikwB2rJ8MerGjL3O0pWN2uqyUlMm/vsPh3OLv9vT5//////r////d8u/xr9vUzIfGvJxuuq1oY7ChOGCilxhglI4FYZCKAGKRiwBik4sAYpOMAGKTjABik4wAYpOMAGKTjABik4wAYpOMAGKTjABik4wAYpOMAGKTjABik4wAY5OMAGOTjABjk4wAY5ONAP///wD///8A////ACskKwAqIyoAKCAoACYeJgA4LTcAWU5YAGlkZgFjYlgBX19PAV52YwBgnIoAZ7GfAW6vnhZwrp1Bc7anfInEua+529XX6vTx9//////7/f3+3fDt7a/a086GxrudbbiraF+woTZcqZoVXp+TBmOWjwBlk48AZJWRAGSWkABklZAAZJWQAGSVkABklZAAZJWQAGSVkABklZAAZJWQAGSVkABklZAAZJWQAGSVkABklZAAZJWQAGSVkABklZEA////AP///wD///8AXWFdAFxhXABcX1wAWl5aAF1gXQBiZmIBYGxfA1ppVQNVXkwBV2lcAF6OgQBiqpoCYq2cHGevn016u62KoNDHwM/o4+Tx+Pf28vn39tfs6OWs19DGh8W5m3K5qmlnsqE6XqyaFViklANbnI8AYpWNAGSSjwBjlI8AY5WPAGOUjwBjlI8AY5SPAGOUjwBjlI8AY5SPAGOUjwBjlI8AY5SPAGOUjwBjlI8AY5SPAGOUjwBjlI8AY5SPAGOUjwD///8A////AP///wBliWUAZYllAGWJZQBmiWYAY4ZjAF+DXwJXg1gETHZMBD9SPAI7OzkAR1JRAFOAeARappggY7WnUX/BtY+p1c7ExuTg3Mrl4tq53NnKnc7HsILAtYxwtqdhbLKgOGyynRhmrpgEWqWSAFmbjgBglI4AZJKOAGOUjgBjlI4AY5SOAGOUjgBjlI4AY5SOAGOUjgBjlI4AY5SOAGOUjgBjlI4AY5SOAGOUjgBjlI4AY5SOAGOUjgBjlI4AY5SOAP///wD///8A////AFeDVwBXhFcAWIVYAFmFWQBZhlkAWYhZAlOFVANEbEUELz0tAiQTGwAxHSkASFxeBFqimB5kvK5LecC0gZbLxaqg0c20j8nEpnu+uI1stKtwY66hT2Cqmy9nrJoUcrKbBm6xmgBfqJQAWZuPAGCUjgBkko4AY5SOAGOUjgBjlI4AY5SOAGOUjgBjlI4AY5SOAGOUjgBjlI4AY5SOAGOUjgBjlI4AY5SOAGOUjgBjlI4AY5SOAGOUjgBjlI4A////AP///wD///8AUGxQAFBsUABQbVAAUW1RAFJwUgBTclMBUGhQAUNPRAIwLzACJRcdADAiJwBLYF4EY6ebF2vAsDlvu65fdrqxdHS4sXRnsatlWqihT1KimTZRoZUeVqKUDmKnlgJxsJoAcLKbAGCplgBZm48AYJSOAGSSjgBjlI4AY5SOAGOUjgBjlI4AY5SOAGOUjgBjlI4AY5SOAGOUjgBjlI4AY5SOAGOUjgBjlI4AY5SOAGOUjgBjlI4AY5SOAGOUjgD///8A////AP///wBPYU8AT2FPAE9gTwBQYFAAUWRRAFFkUQBOVk4ARkBGATwwPAE1NTABOk81AlR9YgRvqpgNdbytHm25qDJksKI6X6ieNlmhmipSmZMcTZaPD02ckQRToJIAYKWVAG+vmgBvspsAYKmWAFmbjwBhlI4AZJOOAGOUjgBjlI4AY5SOAGOUjgBjlI4AY5SOAGOUjgBjlI4AY5SOAGOUjgBjlI4AY5SOAGOUjgBjlI4AY5SOAGOUjgBjlI4AY5SOAP///wD///8A////AE9kTwBPZE8AT2NPAFBjUABRZ1EAUWZRAE1ZTQBIQ0gARTlFAUJLPwJDbj8DWY9gBHOejAd1pZwLa6maEWOpmhJepJkPWJqSClOPjARPkIwBUJuQAFWikwBgp5YAb6+aAG+zmwBgqZYAWZuPAGGUjgBkk44AY5SOAGOUjgBjlI4AY5SOAGOUjgBjlI4AY5SOAGOUjgBjlI4AY5SOAGOUjgBjlI4AY5SOAGOUjgBjlI4AY5SOAGOUjgBjlI4A////AP///wD///8AUGVQAFBlUABQZVAAUGVQAFFoUQBRaFEATVtNAEhFSABHPkcAR05FAUpwRwJch2EEboZ7BWp8fQVhhn4DYZ6RAWGomwBanJMAVY6LAFKOjABSmpEAVqOUAGGolwBwr5sAb7ObAGCplgBZm48AYZSOAGSTjgBjlI4AY5SOAGOUjgBjlI4AY5SOAGOUjgBjlI4AY5SOAGOUjgBjlI4AY5SOAGOUjgBjlI4AY5SOAGOUjgBjlI4AY5SOAGOUjgD///8A////AP///wBQZVAAUGVQAFBlUABQZVAAUWhRAFFoUQBNXE0AR0ZHAEU8RQBIS0UATWVLAl16YQRodHAFYGJoBFhsbAJelYsAYqyeAFyflgBWkI0AUpCNAFKbkQBWo5QAYaeXAHCvmwBvs5sAYKmWAFmbjwBhlI4AZJOOAGOUjgBjlI4AY5SOAGOUjgBjlI4AY5SOAGOUjgBjlI4AY5SOAGOUjgBjlI4AY5SOAGOUjgBjlI4AY5SOAGOUjgBjlI4AY5SOAP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AA=="
								, true, 30, 30);
						}
						ImGui::SameLine();
					}
					ImGui::End();
				}
				else {
					for (int i = 2; i >= 0; i--) {
						Base::Draw::Img["insect" + to_string(i)] = NULL;
					}
				}
			}
		

		//图片显示
		window_flags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysUseWindowPadding;
		for (auto [Begin, Data] : Base::Draw::Img) {
			if (ImgTextureCache.find(Data.ImageFile) != ImgTextureCache.end()) {
				if (ImgTextureCache[Data.ImageFile].texture == NULL) {
					if (Data.Base64) {
						bool ret = LoadTextureFromBase(Data.ImageFile, &ImgTextureCache[Data.ImageFile].texture, &ImgTextureCache[Data.ImageFile].width, &ImgTextureCache[Data.ImageFile].height, Data.Width, Data.Height);
						IM_ASSERT(ret);
					}
					else {
						bool ret = LoadTextureFromFile(Data.ImageFile.c_str(), &ImgTextureCache[Data.ImageFile].texture, &ImgTextureCache[Data.ImageFile].width, &ImgTextureCache[Data.ImageFile].height);
						IM_ASSERT(ret);
					}
				}
			}
			else {
				ImgTextureCache[Data.ImageFile] = TextureCache();
				if (Data.Base64) {
					bool ret = LoadTextureFromBase(Data.ImageFile, &ImgTextureCache[Data.ImageFile].texture, &ImgTextureCache[Data.ImageFile].width, &ImgTextureCache[Data.ImageFile].height, Data.Width, Data.Height);
					IM_ASSERT(ret);
				}
				else {
					bool ret = LoadTextureFromFile(Data.ImageFile.c_str(), &ImgTextureCache[Data.ImageFile].texture, &ImgTextureCache[Data.ImageFile].width, &ImgTextureCache[Data.ImageFile].height);
					IM_ASSERT(ret);
				}
			}
			//创建窗口
			ImGui::SetNextWindowBgAlpha(0);
			ImGui::SetNextWindowPos(ImVec2(
				ImGui::GetMainViewport()->Pos.x + ImGui::GetMainViewport()->Size.x * Data.Pos.x,
				ImGui::GetMainViewport()->Pos.y + ImGui::GetMainViewport()->Size.y * Data.Pos.y
			), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
			ImGui::Begin(("IMG_" + Data.Name).c_str(), NULL, window_flags);
			ImGui::Image((void*)ImgTextureCache[Data.ImageFile].texture, ImVec2(ImgTextureCache[Data.ImageFile].width, ImgTextureCache[Data.ImageFile].height), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0), ImVec4(Data.Channel.x, Data.Channel.y, Data.Channel.z, Data.BgAlpha));
			ImGui::End();
		}
		//文字显示
		for (auto [Begin, Data] : Base::Draw::Text) {
			ImGui::SetNextWindowBgAlpha(0);
			ImGui::SetNextWindowPos(ImVec2(
				ImGui::GetMainViewport()->Pos.x + ImGui::GetMainViewport()->Size.x * Data.Pos.x,
				ImGui::GetMainViewport()->Pos.y + ImGui::GetMainViewport()->Size.y * Data.Pos.y
			), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
			ImGui::Begin(("TEXT_" + Data.Name).c_str(), NULL, window_flags);
			ImGui::SetWindowFontScale(Data.Size);
			ImGui::TextColored(ImVec4(Data.Color.x, Data.Color.y, Data.Color.z, Data.BgAlpha), Data.Text.c_str());
			ImGui::End();
		}

		ImGui::Render();
		pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		return oPresent(pSwapChain, SyncInterval, Flags);
	}

	DWORD WINAPI MainThread(LPVOID lpReserved)
	{
		bool init_hook = false;
		do
		{
			if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success)
			{
				kiero::bind(8, (void**)&oPresent, hkPresent);
				kiero::bind(13, (void**)&oResize, hkResize);
				init_hook = true;
			}
		} while (!init_hook);
		return TRUE;
	}

	LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

		if (true && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
			return true;

		return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
	}

	static void InitConsole() {
		if (initConsole)
			return;
		else
			initConsole = true;
		CreateThread(nullptr, 0, MainThread, hMod, 0, nullptr);
	}
}