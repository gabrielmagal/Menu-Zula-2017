#include "Headers.h"
#include "Hook.h"
#include "Menu.h"
#include <vector>
#include <iostream>
#include "XOR.h"
#include <WinInet.h>
#include <process.h>
#include <tlhelp32.h>
#include "Model Recs.h"
#include "mySprite.h"
#include "Functions.h"
#include <stdio.h>
#pragma comment(lib, "wininet.lib")

using namespace std;

#define WHITE			D3DCOLOR_ARGB(255, 255, 255, 255)
#define RED				D3DCOLOR_ARGB(255, 255, 000, 000)
#define GREEN			D3DCOLOR_ARGB(255, 000, 255, 000)
#define BLUE			D3DCOLOR_ARGB(255, 000, 000, 255) 
#define BLACK			D3DCOLOR_ARGB(150, 000, 000, 000)
#define GRAY			D3DCOLOR_ARGB(255, 125, 125, 125)
#define MBLACK			D3DCOLOR_ARGB(100, 000, 000, 205)
#define PURPLE			D3DCOLOR_ARGB(255, 125, 000, 255) 
#define GREY			D3DCOLOR_ARGB(255, 128, 128, 128) 
#define YELLOW			D3DCOLOR_ARGB(255, 255, 255, 000) 
#define ORANGE			D3DCOLOR_ARGB(255, 255, 125, 000)
#define PINK			D3DCOLOR_ARGB(255, 255, 050, 250)
#define CYAN			D3DCOLOR_ARGB(255, 000, 255, 255)
#define WHITE2			D3DCOLOR_ARGB(100, 255, 255, 255)
#define RED2			D3DCOLOR_ARGB(100, 255, 000, 000)
#define GREEN2			D3DCOLOR_ARGB(100, 000, 255, 000)
#define BLUE2			D3DCOLOR_ARGB(100, 000, 000, 255) 
#define BLACK2			D3DCOLOR_ARGB(100, 000, 000, 000)
#define PURPLE2			D3DCOLOR_ARGB(100, 125, 000, 255) 
#define GREY2			D3DCOLOR_ARGB(100, 128, 128, 128) 
#define YELLOW2			D3DCOLOR_ARGB(100, 255, 255, 000) 
#define ORANGE2			D3DCOLOR_ARGB(100, 255, 125, 000)
#define PINK2			D3DCOLOR_ARGB(100, 255, 050, 250)
#define CYAN2			D3DCOLOR_ARGB(100, 000, 255, 255)


CHAR *OnOff[] = { "[Off]", "[On]" };
CHAR *Folder[] = { "[+]", "[-]" };

char* optFOV[7] = { "[90º]", "[30º]", "[60º]", "[360º]", "[120º]", "[150º]", "[180º]" };
char* optKey[6] = { "[LClick]", "[RClick]", "[MClick]", "[Alt]", "[Ctrl]", "[Shift]" };
char *Colors[] = { "[OFF]", "[White]", "[Red]", "[Green]", "[Blue]", "[Black]", "[Gray]", "[mBlack]",
"[Purple]", "[Grey]", "[Yellow]", "[Orange]", "[Pink]", "[Cyan]" };
char *ChamsColors[] = { "[White]", "[Red]", "[Green]", "[Blue]", "[Black]", "[Gray]", "[mBlack]",
"[Purple]", "[Grey]", "[Yellow]", "[Orange]", "[Pink]", "[Cyan]" };

char *Team[] = { "[Off]", "[Zula]", "[Glad]", "[All]" };


INT varBox, varLine, varAimbot, varAimbotKey, varAimbotFov, varDesenharFOV, varChams,
	varChamsGladios, varChamsZula, varWallHack, varWallColor, varCrossHairINT,
	varNoSmoke, varDisableFog, varNamePlayers;

BOOL BFont = FALSE;

unsigned int	uiStride = NULL;

D3DCOLOR d3dColorList[] = { WHITE, RED, GREEN, BLUE, BLACK, GRAY, MBLACK, PURPLE, GREY, YELLOW, ORANGE, PINK, CYAN };// tem 13 cores
LPDIRECT3DTEXTURE9 pWhite, pRed, pGreen, pBlue, pBlack, pGray, pmBlack, pPurple, pGrey, pYellow, pOrange, pPink, pCyan;
LPDIRECT3DTEXTURE9 textureList[] = { pWhite, pRed, pGreen, pBlue, pBlack, pGray, pmBlack, pPurple, pGrey, pYellow, pOrange, pPink, pCyan };

IDirect3DPixelShader9* pShader;
UINT psData;

IDirect3DVertexShader9* vShader;
UINT vsData;
D3DVIEWPORT9 viewport;

INT VFolder[MAX_PATH];
bool bCriado = false;

struct ModelInfo_t{
	D3DXVECTOR3 Position2D;
	D3DXVECTOR3 Position3D;
	int Offset;
	int TEAM;
	int Index;
	float CrosshairDistance;
};
std::vector<ModelInfo_t*>ModelInfo;


void AddModel(IDirect3DDevice9* Device, int Team){
	ModelInfo_t* pModel = new ModelInfo_t;
	pModel->TEAM = Team;
	Device->GetViewport(&viewport);
	D3DXMATRIX projection, view, world;
	D3DXVECTOR3 vScreenCoord(0, 0, (float)0), vWorldLocation(0, 0, 0);

	Device->GetTransform(D3DTS_VIEW, &view);
	Device->GetTransform(D3DTS_PROJECTION, &projection);
	Device->GetTransform(D3DTS_WORLD, &world);

	D3DXVec3Project(&vScreenCoord, &vWorldLocation, &viewport, &projection, &view, &world);
	if(vScreenCoord.z < 1) {
		pModel->Position2D.x = vScreenCoord.x;
		pModel->Position2D.y = vScreenCoord.y;
	}
	ModelInfo.push_back(pModel);
}

int AimFovList[] = { 90, 30, 60, 360, 120, 150, 180};
int AimKeyList[] = { VK_LBUTTON , VK_RBUTTON , VK_MBUTTON , VK_MENU , VK_CONTROL , VK_SHIFT , VK_RBUTTON };

DWORD gametick0 = timeGetTime();
bool inArray(int Index){
	for (int i = 0; i < ModelInfo.size(); i += 1){
		if (ModelInfo[i]->Index == Index)return true;
	}
	return false;
}

int Teste = 1;

void RenderTeste(LPDIRECT3DDEVICE9 pDevice) {
	if(timeGetTime() - gametick0 > 1) {
		if(ModelInfo.size() != NULL) {
			UINT BestTarget = -1;
			DOUBLE fClosestPos = 99999;
			float ScreenCenterX = viewport.Width;
			float ScreenCenterY = viewport.Height;

			for(int i = 0; i < ModelInfo.size(); i++) {
				/*if(varLine) {
					if(ModelInfo[i]->TEAM == 1)
						DrawLine(ScreenCenterX/2, ScreenCenterY, ModelInfo[i]->Position2D.x, ModelInfo[i]->Position2D.y, 1, Blue);
					else
						DrawLine(ScreenCenterX/2, ScreenCenterY, ModelInfo[i]->Position2D.x, ModelInfo[i]->Position2D.y, 1, Red);
				}*/

				if(varBox) {
					if (ModelInfo[i]->TEAM == 1)
						DrawBorder(ModelInfo[i]->Position2D.x, ModelInfo[i]->Position2D.y, 20, 20, 1, Blue, pDevice);
					else
						DrawBorder(ModelInfo[i]->Position2D.x, ModelInfo[i]->Position2D.y, 20, 20, 1, Red, pDevice);
				}

				if(varNamePlayers) {
					if(ModelInfo[i]->TEAM == 1)
						WriteText(ModelInfo[i]->Position2D.x, ModelInfo[i]->Position2D.y - 20, Blue, Team[1]);
					else
						WriteText(ModelInfo[i]->Position2D.x, ModelInfo[i]->Position2D.y - 20, Red, Team[2]);
				}

				if(varAimbot) {
					if (varAimbot != ModelInfo[i]->TEAM && varAimbot != 3) continue;
					float radiusx = AimFovList[varAimbotFov] * (ScreenCenterX / 100);
					float radiusy = AimFovList[varAimbotFov] * (ScreenCenterY / 100);
					ModelInfo[i]->CrosshairDistance = GetDistance(ModelInfo[i]->Position2D.x, ModelInfo[i]->Position2D.y, ScreenCenterX/2, ScreenCenterY/2);
					if(ModelInfo[i]->Position2D.x >= ScreenCenterX/2 - radiusx && ModelInfo[i]->Position2D.x <= ScreenCenterX/2 + radiusx &&
						ModelInfo[i]->Position2D.y >= ScreenCenterY/2 - radiusy && ModelInfo[i]->Position2D.y <= ScreenCenterY/2 + radiusy) {
						if(ModelInfo[i]->CrosshairDistance < fClosestPos) {
							fClosestPos = ModelInfo[i]->CrosshairDistance;
							BestTarget = i;
						}
					}
				}
			}

			if(varAimbot) {
				if(BestTarget != -1) {

					double DistX = ((double)ModelInfo[BestTarget]->Position2D.x - ScreenCenterX / 2);
					double DistY = ((double)ModelInfo[BestTarget]->Position2D.y - ScreenCenterY / 2);

					DistX /= 2;
					DistY /= 2;

					//FillRGB(ModelInfo[BestTarget]->Position2D.x, ModelInfo[BestTarget]->Position2D.y, 3, 3, Orange, pDevice);

					if((GetAsyncKeyState(AimKeyList[varAimbotKey]) & 0x8000))
						mouse_event(MOUSEEVENTF_MOVE, (DWORD)DistX, (DWORD)DistY, NULL, NULL);
				}
			}
			ModelInfo.clear();
		}
		gametick0 = timeGetTime();
	}
}

int PI2 = 6.283185307179586;


HRESULT WINAPI Present(LPDIRECT3DDEVICE9 pDevice, CONST RECT *pSourceRect, CONST RECT *pDestRect, HWND hDestWindowOverride, CONST RGNDATA *pDirtyRegion)
{
	if (!bCriado){
		for (int i = 0; i < ARRAYSIZE(textureList); i++)
			GenerateTexture(pDevice, &textureList[i], d3dColorList[i]);
		if (!PFont){
			D3DXCreateFont(pDevice, 14, 0, 400, 1, 0, 1, 0, 4, 0 | (0 << 4),"FixedSys", &PFont);
			D3DXCreateLine(pDevice, &g_pLine);
			D3DXCreateLine(pDevice, &pLine);
		}
		bCriado = true;
	}
	if (GetAsyncKeyState(VK_INSERT) & 1)
		menu.ShowMenu = !menu.ShowMenu;

	if (menu.ShowMenu && PFont){
		DrawBox(menu.matrixPos[0] - 10, menu.matrixPos[1] - 22, 200, 20, TBlack, Blue, pDevice);//Draw box do título.
		DrawBorder(menu.matrixPos[0] - 10, menu.matrixPos[1] - 22, 200, 20, 1, Blue, pDevice);//Borda da box do título.
		WriteText(menu.matrixPos[0], menu.matrixPos[1] - 20, Yellow, " UN3XP3CT3D D3D v1.0.1");//título do menu.
		DrawBox(menu.matrixPos[0] - 10, menu.matrixPos[1], 200, ((menu.Current * 15) - 5), TBlack, Blue, pDevice);//Draw Box principal.
		DrawBorder(menu.matrixPos[0] - 10, menu.matrixPos[1], 200, ((menu.Current * 15) - 5), 1, Blue, pDevice); //Borda da box principal.
		DrawBorder(menu.matrixPos[0] - 10, ((menu.matrixPos[2] + 2) + (menu.MenuSelection * 15)), 200, 13, 1, Red, pDevice); // Selection
		menu.Current = 1;

		if (GetAsyncKeyState(VK_UP) & 1)
			menu.MenuSelection--;

		if (GetAsyncKeyState(VK_DOWN) & 1)
			menu.MenuSelection++;

		Additem("ESP", 1, VFolder[0], 1, Folder);
		if(VFolder[0]) {
			Additem("Line", 1, varLine, 0, OnOff);
			Additem("Box", 1, varBox, 0, OnOff);
			Additem("NameTeam", 1, varNamePlayers, 0, OnOff);
			Additem("CrossHair", 13, varCrossHairINT, 0, Colors);
			Additem("NoSmoke", 1, varNoSmoke, 0, OnOff);
			Additem("Disable Fog", 1, varDisableFog, 0, OnOff);
		}

		Additem("Aimbot", 1, VFolder[1], 1, Folder);
		if(VFolder[1]) {
			Additem("Aimbot", 3, varAimbot, 0, Team);
			Additem("AimKEY", 5, varAimbotKey, 0, optKey);
			Additem("AimFOV", 5, varAimbotFov, 0, optFOV);
			Additem("DesenharFOV", 1, varDesenharFOV, 0, OnOff);
		}

		Additem("Chams", 1, VFolder[2], 1, Folder);
		if(VFolder[2]) {
			Additem("Chams", 1, varChams, 0, OnOff);
			Additem("Zula", 12, varChamsZula, 0, ChamsColors);
			Additem("Glad", 12, varChamsGladios, 0, ChamsColors);
		}

		if (menu.MenuSelection >= menu.Current)
			menu.MenuSelection = 1;
		else if (menu.MenuSelection <= 0)
			menu.MenuSelection = (menu.Current - 1);
	}

	if(varCrossHairINT) {
		CrossHair(pDevice, d3dColorList[varCrossHairINT - 1]);
	}

	if(varDesenharFOV) {
		POINT pt;
		DWORD x = viewport.Width / 2;
		DWORD y = viewport.Height / 2;
		DrawCircle(x, y, AimFovList[varAimbotFov] * PI2, 8, Green);
	}
	RenderTeste(pDevice);
	return OPresent(pDevice, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}


HRESULT WINAPI Reset(LPDIRECT3DDEVICE9 Device, D3DPRESENT_PARAMETERS *pp){
	if (PFont != NULL){
		PFont->OnLostDevice();
		PFont->OnResetDevice();
	}
	return OReset(Device, pp);
}

bool Model = false;
int UltimoTeam = -1;

HRESULT WINAPI DrawIndexedPrimitive(LPDIRECT3DDEVICE9 pDevice, D3DPRIMITIVETYPE PrimType, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT PrimitiveCount)
{
	LPDIRECT3DVERTEXBUFFER9 Stream_Data;
	UINT Offset = 0;
	if(pDevice->GetStreamSource(0, &Stream_Data, &Offset, &uiStride) == S_OK)Stream_Data->Release();

	if(CHAMS_ZULA)
		AddModel(pDevice, 1);
	if(CHAMS_GLAD)
		AddModel(pDevice, 2);

	if(varChams) {
		if(CHAMS_GLAD) {
				pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
				pDevice->SetTexture(NULL, textureList[varChamsGladios]);
				ODrawIndexedPrimitive(pDevice, PrimType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, PrimitiveCount);
				pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
				pDevice->SetTexture(NULL, textureList[varChamsGladios]);
		}

		if(CHAMS_ZULA) {
			pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
			pDevice->SetTexture(NULL, textureList[varChamsZula]);
			ODrawIndexedPrimitive(pDevice, PrimType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, PrimitiveCount);
			pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
			pDevice->SetTexture(NULL, textureList[varChamsZula]);
		}
	}

	if (uiStride == 64 && PrimitiveCount == 2 && NumVertices == 4)
	{
		if (varNoSmoke) {
			return D3D_OK;
		}
	}

	if (varDisableFog) {
		pDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
	}

	return  ODrawIndexedPrimitive(pDevice, PrimType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, PrimitiveCount);
}


void Hook()
{
	HMODULE D3D9 = 0;

	while(!D3D9) {
		Sleep(100);
		D3D9 = GetModuleHandle("d3d9.dll");
	}
	OPresent = (TPresent)DetourFunction((PBYTE)GetDeviceAddress(17), (PBYTE)Present);
	OReset = (TReset)DetourFunction((PBYTE)GetDeviceAddress(16), (PBYTE)Reset);
	ODrawIndexedPrimitive = (TDrawIndexedPrimitive)DetourFunction((PBYTE)GetDeviceAddress(82), (PBYTE)DrawIndexedPrimitive);
}



BOOL APIENTRY DllMain(HINSTANCE hDll, DWORD _CallReason, LPVOID lpvReserved) {
	switch (_CallReason) {
	case DLL_PROCESS_ATTACH:
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Hook, 0, 0, 0);
	}
	return 1;
}