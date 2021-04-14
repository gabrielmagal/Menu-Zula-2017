/*********************************************************/
/* CA Base D3D Menu Selection V5
/* Creditos:
/*	Solify's	-> CA Base D3D Menu Selection.
/*  WE11ington  -> Disponibilizou Para Combat Arms Brasil.
/*  luizimloko  -> Menu Selection V5.
/*********************************************************/
#include "Headers.h"

typedef HRESULT( WINAPI *TReset )( LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS *pPresentationParameters );
TReset OReset;

typedef HRESULT( WINAPI *TPresent )( LPDIRECT3DDEVICE9 pDevice, CONST RECT *pSourceRect, CONST RECT *pDestRect, HWND hDestWindowOverride, CONST RGNDATA *pDirtyRegion );
TPresent OPresent;

typedef HRESULT(WINAPI* TDrawIndexedPrimitive)(LPDIRECT3DDEVICE9 pDevice, D3DPRIMITIVETYPE PrimType, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount);
TDrawIndexedPrimitive ODrawIndexedPrimitive;

typedef HRESULT(WINAPI* TSetStreamSource)(LPDIRECT3DDEVICE9 pDevice, UINT StreamNumber, IDirect3DVertexBuffer9* pStreamData, UINT OffsetInBytes, UINT Stride);
TSetStreamSource OSetStreamSource;

typedef HRESULT(WINAPI* tEndScene)(LPDIRECT3DDEVICE9 pDevice);
tEndScene oEndScene;
HRESULT GenerateTexture(LPDIRECT3DDEVICE9 pDevice, IDirect3DTexture9 **ppD3Dtex, DWORD colour32)
{
	if (FAILED(pDevice->CreateTexture(8, 8, 1, 0, D3DFMT_A4R4G4B4, D3DPOOL_MANAGED, ppD3Dtex, NULL)))
		return E_FAIL;

	WORD colour16 = ((WORD)((colour32 >> 28) & 0xF) << 12)
		| (WORD)(((colour32 >> 20) & 0xF) << 8)
		| (WORD)(((colour32 >> 12) & 0xF) << 4)
		| (WORD)(((colour32 >> 4) & 0xF) << 0);

	D3DLOCKED_RECT d3dlr;
	(*ppD3Dtex)->LockRect(0, &d3dlr, 0, 0);
	WORD *pDst16 = (WORD*)d3dlr.pBits;

	for (int xy = 0; xy < 8 * 8; xy++)
		*pDst16++ = colour16;

	(*ppD3Dtex)->UnlockRect(0);

	return S_OK;
}
float GetDistance(float Xx, float Yy, float xX, float yY)
{
	return sqrt((yY - Yy) * (yY - Yy) + (xX - Xx) * (xX - Xx));
}

bool bCheckNum(int rec[], int vert)
{
	int i = 0;

	while (rec[i] != 0)
	{
		if (rec[i] == vert) { return(true); }
		i++;
	}
	return(false);
}

DWORD FindDevice(DWORD Len)
{
     DWORD dwObjBase = 0;
        
     dwObjBase = (DWORD)LoadLibrary("d3d9.dll");
     while (dwObjBase++ < dwObjBase + Len)
     {
         if ( (*(WORD*)(dwObjBase + 0x00)) == 0x06C7
           && (*(WORD*)(dwObjBase + 0x06)) == 0x8689
           && (*(WORD*)(dwObjBase + 0x0C)) == 0x8689
           ) { dwObjBase += 2; break; }
     }
     return( dwObjBase );
}


DWORD GetDeviceAddress(int VTableIndex)
{
    PDWORD VTable;
    *(DWORD*)&VTable = *(DWORD*)FindDevice(0x128000);
    return VTable[VTableIndex];
}

VOID CreateDevice(DWORD *dwVTable)
{
	LPDIRECT3D9 D3D9;
	LPDIRECT3DDEVICE9 pD3DDevice;
	D3D9 = Direct3DCreate9( D3D_SDK_VERSION );
	if( D3D9 == NULL )
		return;
	D3DPRESENT_PARAMETERS pPresentParms;
	ZeroMemory( &pPresentParms, sizeof( pPresentParms ) );
	pPresentParms.Windowed = TRUE;
	pPresentParms.BackBufferFormat = D3DFMT_UNKNOWN;
	pPresentParms.SwapEffect = D3DSWAPEFFECT_DISCARD;
	if( FAILED( D3D9->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, GetDesktopWindow(), D3DCREATE_SOFTWARE_VERTEXPROCESSING, &pPresentParms, &pD3DDevice ) ) )
		return;
	DWORD *dwTable = (DWORD *)pD3DDevice;
	dwTable = (DWORD *) dwTable[ 0 ];
	dwVTable[ 1 ] = dwTable[ 17 ];
	dwVTable[ 2 ] = dwTable[ 82 ];
	dwVTable[ 3 ] = dwTable[100];
	dwVTable[ 4 ] = dwTable[42];
}
