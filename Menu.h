#include "Headers.h"
#define PI 3.14159265

#define Red       D3DCOLOR_ARGB(255, 255, 000, 000)
#define Green     D3DCOLOR_ARGB(255, 127, 255, 000)
#define Orange    D3DCOLOR_ARGB(255, 255, 140, 000)
#define Blue      D3DCOLOR_ARGB(255, 000, 000, 255)
#define Yellow    D3DCOLOR_ARGB(255, 255, 255,  51)
#define Black     D3DCOLOR_ARGB(255, 000, 000, 000)
#define Grey      D3DCOLOR_ARGB(255, 112, 112, 112)
#define Gold      D3DCOLOR_ARGB(255, 255, 215, 000)
#define Pink      D3DCOLOR_ARGB(255, 255, 192, 203)
#define Purple    D3DCOLOR_ARGB(255, 128, 000, 128)
#define White     D3DCOLOR_ARGB(255, 255, 255, 249)
#define Cyan      D3DCOLOR_ARGB(255, 000, 255, 255)
#define Magenta   D3DCOLOR_ARGB(255, 255, 000, 255)
#define TBlack    D3DCOLOR_ARGB(200, 000, 000, 000)

LPD3DXLINE		g_pLine = NULL;
ID3DXLine	*pLine;

struct CMenu
{
	INT MenuSelection,
		matrixPos[ 3 ],
		Current;

	BOOL ShowMenu;

	CMenu( VOID )
	{
		ShowMenu = 1;
		Current = 1;

		matrixPos[ 0 ] = 30; //x1
		matrixPos[ 1 ] = 70; //y1
		matrixPos[ 2 ] = 60; //y2
		matrixPos[ 3 ] = 00; //x2
	}
}
menu;

LPD3DXFONT PFont;

VOID WriteText( INT x, INT y, D3DCOLOR Color, CHAR *String )
{
	RECT Rect;
	SetRect( &Rect, x, y, x, y );
	PFont->DrawTextA( 0, String, -1, &Rect, DT_LEFT | DT_NOCLIP, Color );
}

void DrawLine(float x, float y, float x2, float y2, float width, DWORD color)
{
	D3DXVECTOR2 vLine[2];
	vLine[0].x = x;
	vLine[0].y = y;
	vLine[1].x = x2;
	vLine[1].y = y2;
	g_pLine->SetWidth(width);
	g_pLine->SetAntialias(false);
	g_pLine->SetGLLines(true);
	g_pLine->Begin();
	g_pLine->Draw(vLine, 2, color);
	g_pLine->End();
}

VOID DrawRect( LPDIRECT3DDEVICE9 pDevice, INT x, INT y, INT h, INT w, DWORD Color )
{
	D3DRECT rec;
	rec.x1 = x;
	rec.x2 = x + w;
	rec.y1 = y;
	rec.y2 = y + h;

	pDevice->Clear( 1, &rec, D3DCLEAR_TARGET, Color, 0, 0 );
}

VOID FillRGB( INT x, INT y, INT w, INT h, D3DCOLOR Color, LPDIRECT3DDEVICE9 pDevice )
{
	D3DRECT rec = { x, y, x + w, y + h };
	pDevice->Clear( 1, &rec, D3DCLEAR_TARGET, Color, 0, 0 );
}

VOID DrawBorder( INT x, INT y, INT w, INT h, INT px, D3DCOLOR BorderColor, LPDIRECT3DDEVICE9 pDevice )
{
	FillRGB(x,(y + h - px), w, px, BorderColor, pDevice);
	FillRGB(x, y, px, h, BorderColor, pDevice);
	FillRGB(x, y, w, px, BorderColor, pDevice);
	FillRGB(x + w - px, y, px, h, BorderColor, pDevice);
}

namespace Vertex
{
	struct D3DTLVERTEX
	{
		float x, y, z, u, v;
		float rhw;
		DWORD m_Color;
	};

	D3DTLVERTEX CreateD3DTLVERTEX(float X, float Y, float Z, float RHW, DWORD color, float U, float V) { D3DTLVERTEX v = { X, Y, Z, RHW, color }; return v; }
};

void DrawCircle(int X, int Y, int radius, int numSides, DWORD Color)
{

	D3DXVECTOR2 Line[128];
	float Step = PI * 2.0 / numSides;
	int Count = 0;
	for (float a = 0; a < PI*2.0; a += Step)
	{
		float X1 = radius * cos(a) + X;
		float Y1 = radius * sin(a) + Y;
		float X2 = radius * cos(a + Step) + X;
		float Y2 = radius * sin(a + Step) + Y;
		Line[Count].x = X1;
		Line[Count].y = Y1;
		Line[Count + 1].x = X2;
		Line[Count + 1].y = Y2;
		Count += 2;
	}
	pLine->Begin();
	pLine->Draw(Line, Count, Color);
	pLine->End();
}


void DrawCircle2(float x, float y, float m_Rad, DWORD m_Color, LPDIRECT3DDEVICE9 m_Device)
{
	const int NUMPOINTS = 360;
//	const float fPI = 3.14159;
	Vertex::D3DTLVERTEX Circle[NUMPOINTS + 1];
	int i;
	float X;
	float Y;
	float Theta;
	float WedgeAngle;
	WedgeAngle = (float)((2 * PI) / NUMPOINTS);
	for (int i = 0; i <= NUMPOINTS; i++)
	{
		Theta = i * WedgeAngle;
		X = (float)(x + m_Rad * cos(Theta));
		Y = (float)(y - m_Rad * sin(Theta));
		Circle[i] = Vertex::CreateD3DTLVERTEX(X, Y, 0.0f, 1.0f, m_Color, 10.0f, 10.0f);
	}
	m_Device->SetFVF(100);
	m_Device->SetTexture(0, NULL);
	m_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	m_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_Device->DrawPrimitiveUP(D3DPT_LINESTRIP, NUMPOINTS, &Circle[0], sizeof(Circle[0]));
}

void Escrever(ID3DXFont *pFont, int x, int y, DWORD Color, const char *Format, ...)
{
	RECT rect = { x, y, x + 500, y + 30 };
	char Buffer[1024] = { '\0' };
	va_list va_alist;
	va_start(va_alist, Format);
	vsprintf_s(Buffer, Format, va_alist);
	va_end(va_alist);
	pFont->DrawTextA(NULL, Buffer, -1, &rect, DT_LEFT | DT_WORDBREAK, Color);
}

VOID DrawBox( INT x, INT y, INT w, INT h, D3DCOLOR Color, D3DCOLOR BorderColor, LPDIRECT3DDEVICE9 pDevice ) 
{
	DrawBorder(x, y, w, h, 2, BorderColor, pDevice);
	
	const DWORD D3D_FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE;

	struct Vertex
	{
		FLOAT x, y, w, h;
		D3DCOLOR Color;
	}
	V[4] = {{(FLOAT) x, (FLOAT)(y + h), 0.0f, 0.0f, Color}, 
			{(FLOAT) x, (FLOAT)y, 0.0f, 0.0f, Color}, 
			{(FLOAT)(x + w), (FLOAT)(y + h), 0.0f, 0.0f, Color}, 
			{(FLOAT)(x + w), (FLOAT)y, 0.0f, 0.0f, Color}
	};

	pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, D3DZB_TRUE );
	pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	pDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );
	pDevice->SetRenderState( D3DRS_FOGENABLE, D3DZB_FALSE );

	pDevice->SetFVF( D3D_FVF );
	pDevice->SetTexture( 0, NULL );
	pDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, V, sizeof(Vertex) );
}

VOID CrossHair( LPDIRECT3DDEVICE9 pDevice, D3DCOLOR Color )
{
	D3DVIEWPORT9 Viewport;
	pDevice->GetViewport( &Viewport );
	DWORD ScreenX = Viewport.Width / 2;
	DWORD ScreenY = Viewport.Height / 2;
	D3DRECT rec1 = { ScreenX - 15, ScreenY, ScreenX + 15, ScreenY + 02 };
	D3DRECT rec2 = { ScreenX, ScreenY - 15, ScreenX + 02, ScreenY + 15 };
	pDevice->Clear( 1, &rec1, D3DCLEAR_TARGET, Color, 0, 0 );
	pDevice->Clear( 1, &rec2, D3DCLEAR_TARGET, Color, 0, 0 );
}


VOID Additem( CHAR *String, INT MaxValue, INT &Variable, INT Folder, CHAR **Option )
{
	if( menu.ShowMenu )
	{
		if( menu.MenuSelection == menu.Current )
		{
			if( GetAsyncKeyState( VK_RIGHT ) &1 )
			{
				if( Variable < MaxValue )
					Variable++;
			}
			else if( GetAsyncKeyState( VK_LEFT ) &1 )
			{
				if( Variable > 0 )
					Variable--;
			}
		}

		if( menu.MenuSelection == menu.Current )
		{
			WriteText( menu.matrixPos[ 0 ] - 3, menu.matrixPos[ 2 ] + ( menu.Current * 15 ), White, String );
			WriteText( menu.matrixPos[ 0 ] + 125, menu.matrixPos[ 2 ] + ( menu.Current * 15), White, Option[ Variable ] );
		}
		else if( Variable > 0 )
		{
			if( Folder == 1 )
			{
				WriteText( menu.matrixPos[ 0 ] - 3, menu.matrixPos[ 2 ] + ( menu.Current * 15 ), Blue, String );
				WriteText( menu.matrixPos[ 0 ] + 125, menu.matrixPos[ 2 ] + ( menu.Current * 15), Blue, Option[ Variable ] );
			}
			else
			{
				WriteText( menu.matrixPos[ 0 ] - 3, menu.matrixPos[ 2 ] + ( menu.Current * 15 ), Yellow, String );
				WriteText( menu.matrixPos[ 0 ] + 125, menu.matrixPos[ 2 ] + ( menu.Current * 15), Yellow, Option[ Variable ] );
			}
		}
		else
		{
			if( Folder == 1 )
			{
				WriteText( menu.matrixPos[ 0 ] - 3, menu.matrixPos[ 2 ] + ( menu.Current * 15 ), Blue, String );
				WriteText( menu.matrixPos[ 0 ] + 125, menu.matrixPos[ 2 ] + ( menu.Current * 15), Blue, Option[ Variable ] );
			}
			else
			{
				WriteText( menu.matrixPos[ 0 ] - 3, menu.matrixPos[ 2 ] + ( menu.Current * 15 ), White, String );
				WriteText( menu.matrixPos[ 0 ] + 125, menu.matrixPos[ 2 ] + ( menu.Current * 15), White, Option[ Variable ] );
			}
		}
		menu.Current++;
	}
}