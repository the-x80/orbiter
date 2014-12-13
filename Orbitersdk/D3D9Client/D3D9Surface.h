// ==============================================================
// Part of the ORBITER VISUALISATION PROJECT (OVP)
// Dual licensed under GPL v3 and LGPL v3
// Copyright (C) 2012 - 2014 Jarmo Nikkanen
// ==============================================================

#ifndef __D3DSURFACE_H
#define __D3DSURFACE_H

#include "OrbiterAPI.h"
#include "D3D9Client.h"
#include "D3D9Pad.h"
#include "GDIPad.h"
#include <d3d9.h>
#include <d3dx9.h>


#define D3D9S_TEXTURE	0x1
#define D3D9S_RENDER	0x2
#define D3D9S_DYNAMIC	0x4
#define D3D9S_LOCK		0x8

// Every SURFHANDLE in the client is a pointer into the D3D9ClientSurface class

class D3D9ClientSurface {

	friend class D3D9Client;
	friend class D3D9Pad;
	friend class GDIPad;

public:
						// Initialize global (shared) resources
	static void			D3D9TechInit(class D3D9Client *gc, LPDIRECT3DDEVICE9 pDev, const char *folder);
	static void			GlobalExit();

						// Create empty surface. Must use Create methods to make a valid surface
						D3D9ClientSurface(LPDIRECT3DDEVICE9 pDevice, const char* name = "???");

						// Destroy the class and release the texture (pTex) if exists. Value of Reference counter doesn't matter.
						~D3D9ClientSurface();

	void				MakeBackBuffer(LPDIRECT3DSURFACE9);
	void				MakeTextureEx(UINT Width, UINT Height, DWORD Usage=D3DUSAGE_DYNAMIC|D3DUSAGE_AUTOGENMIPMAP, D3DFORMAT Format=D3DFMT_X8R8G8B8, D3DPOOL pool=D3DPOOL_DEFAULT);
	void				MakeRenderTargetEx(UINT Width, UINT Height, bool bLock=false, D3DFORMAT Format=D3DFMT_X8R8G8B8);
	void				MakeDepthStencil();

	bool				GetDesc(D3DSURFACE_DESC *);

	bool				LoadSurface(const char *fname, DWORD flags);
	bool				LoadTexture(const char *fname);
	void				SaveSurface(const char *fname);

	DWORD				GetMipMaps();
	DWORD				GetWidth();
	DWORD				GetHeight();
	DWORD				GetSizeInBytes();

	void				SetAttribs(DWORD attrib) { Attrib = attrib; }

	void				IncRef();	// Increase surface reference counter
	bool				Release();	// Decrease the counter
	int					RefCount() { return Refs; }

	const char *		GetName() const { return name; }
	void				SetName(const char *);


	bool				IsCompressed();
	bool				IsGDISurface();
	bool				IsBackBuffer();
	bool				IsTexture() const { return (pTex!=NULL); }
	bool				IsRenderTarget();
	bool				Is3DRenderTarget();
	bool				IsPowerOfTwo() const;
	bool				IsSystemMem() { return (desc.Pool==D3DPOOL_SYSTEMMEM); }

	LPDIRECT3DSURFACE9	GetDepthStencil();
	LPDIRECT3DSURFACE9	GetSurface();
	LPDIRECT3DTEXTURE9	GetTextureHard();
	LPDIRECT3DTEXTURE9	GetNormalMap();
	LPDIRECT3DTEXTURE9	GetEmissionMap();
	LPDIRECT3DTEXTURE9	GetSpecularMap();
	LPDIRECT3DTEXTURE9	GetReflectionMap();
	LPDIRECT3DTEXTURE9	GetTexture();
	LPDIRECT3DDEVICE9	GetDevice() { return pDevice; }
	int					GetSketchPadMode() { return SketchPad; }


	void				SetColorKey(DWORD ck);			// Enable and set color key
	void				DisableColorKey();				// Disable color key.

	HDC					GetDC();
	void				ReleaseDC(HDC);
	HDC					GetDC_Hack();

	HRESULT				AddQueue(D3D9ClientSurface *src, LPRECT s, LPRECT t);
	HRESULT				FlushQueue();
	void				CopyRect(D3D9ClientSurface *src, LPRECT srcrect, LPRECT tgtrect, UINT ck=0);
	void				SketchRect(SURFHANDLE hSrc, LPRECT s, LPRECT t, float alpha=-1, VECTOR3 *clr=NULL);
	void				SketchRotateRect(SURFHANDLE hSrc, LPRECT s, int tcx, int tcy, int w, int h, float angle, float alpha=-1, VECTOR3 *clr=NULL);

	bool				Fill(LPRECT r, DWORD color);
	bool				Clear(DWORD color);
	
	bool				BindGPU();
	void				ReleaseGPU();
	HRESULT				BeginBlitGroup();
	void				EndBlitGroup();
	int					GetQueueSize();
	bool				ScanNameSubId(const char *n);
	bool				ComputeReflAlpha();

private:

	void				ConvertToRenderTargetTexture();
	void				ConvertToRenderTarget();
	void				CreateSubSurface();
	void				CreateDCSubSurface();
	bool				CreateName(char *out, int len, const char *fname, const char *id);
	void				Decompress();
	DWORD				GetTextureSizeInBytes(LPDIRECT3DTEXTURE9 pT);
	DWORD				GetSizeInBytes(D3DFORMAT Format, DWORD pixels);
	HRESULT				GPUCopyRect(D3D9ClientSurface *src, LPRECT srcrect, LPRECT tgtrect);
	HDC					GetDCHard();
	void				SetupViewPort();
	void				LogSpecs(char *name);
	void				Clear();

	// -------------------------------------------------------------------------------
	char				name[128];
	bool				bCompressed;	// True if the surface is compressed
	bool				bDCOpen;		// DC is Open. This is TRUE between GetDC() and ReleaseDC() calls.
	bool				bHard;			// hDC is acquired using GetDCHard()
	bool				bDCHack;		// DC Hack for SketchPad::GetDC() 
	bool				bSkpGetDCEr;
	bool				bBltGroup;		// BlitGroup operation is active
	bool				bBackBuffer;
	int					Refs;
	DWORD				Type;
	int					Initial;		// Initial creation flags
	int					SketchPad;		// 0=None, 1=GDI, 2=GPU
	int					iBindCount;		// GPU Bind reference counter
	D3DSURFACE_DESC		desc;
	LPDIRECT3DSURFACE9	pStencil;		
	LPDIRECT3DSURFACE9	pSurf;		// This is a pointer to a plain surface or a pointer to the first level in a texture
	LPDIRECT3DTEXTURE9	pTex;		// This is a NULL if Type==D3D9S_PLAIN or Creation==D3D9C_BACKBUF
	LPDIRECT3DSURFACE9	pDCSub;		// Containing a temporary system memory copy of a render target texture
	LPDIRECT3DTEXTURE9	pNormalMap;
	LPDIRECT3DTEXTURE9	pSpecularMap;
	LPDIRECT3DTEXTURE9	pEmissionMap;
	LPDIRECT3DTEXTURE9	pReflectionMap;
	LPDIRECT3DDEVICE9	pDevice;
	D3D9ClientSurface * pSrc_prev;
	D3DXCOLOR			ClrKey;
	DWORD				ColorKey;
	DWORD				Attrib;
	LPD3DXMATRIX		pVP;
	D3DVIEWPORT9 *		pViewPort;
	HFONT				hDefFont;

	ID3DXRenderToSurface *pRTS;

	// Rendering pipeline configuration. Applies to every instance of this class
	//
	static D3D9Client * gc;
	static ID3DXEffect*	FX;
	static D3DXHANDLE	eTech;
	static D3DXHANDLE	eFlush;
	static D3DXHANDLE	eSketch;
	static D3DXHANDLE	eRotate;
	static D3DXHANDLE	eVP;		// Transformation matrix
	static D3DXHANDLE	eColor;		// Color key
	static D3DXHANDLE	eTex0;		// Source Texture
	static D3DXHANDLE	eSize;
	static D3DXHANDLE	eKey;
	static WORD *		Index;
	static GPUBLITVTX * pGPUVtx;
	static WORD			GPUBltIdx;
	static D3D9ClientSurface *pPrevSrc;
};


#endif