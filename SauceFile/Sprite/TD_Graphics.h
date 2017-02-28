#pragma once
#include "../Origin/COrigin.h"
#include "D3D11_SPRITE.h"
//#include "../MAIN.h"

struct SIMPLESHADER_CONSTANT_BUFFER
{
	D3DXMATRIX mWVP;
	D3DXVECTOR4 vColor;
	D3DXVECTOR4 fAlpha;
};
//
//
struct SimpleVertex
{
	D3DXVECTOR3 Pos; //位置
	D3DXVECTOR2 vTex; //テクスチャー座標
};
//
//
struct GrapRect
{
	GrapRect(float top, float bottom, float left, float right){
		m_top = top;
		m_bottom = bottom;
		m_left = left;
		m_right = right;
	}
	~GrapRect(){}

	float m_top;
	float m_bottom;
	float m_left;
	float m_right;
};
//
//
class TD_Graphics
{
public:
	TD_Graphics();
	~TD_Graphics();

	static HRESULT InitShader(ID3D11DeviceContext* pContext);
	static void SetCamera(D3DXMATRIX view, D3DXMATRIX proj);


	// @param (textname)	textureパス
	// @param (drawpos)		描画座標
	// @param (texsize)     画像サイズ
	// @param (vColor)		色
	// @param (_Rect)		レクト
	HRESULT Init(LPCWSTR texname,/* int texnum, */D3DXVECTOR2 drawpos, D3DXVECTOR2 texsize, D3DXVECTOR4 vColor, GrapRect _Rect);

	HRESULT AnimInit(D3DXVECTOR4 vColor, bool flg_alpha, WCHAR *filepass, D3DXVECTOR2 size, int animSpeed);

	// @param (pos)		表示座標
	// @param (scale)	拡大倍率
	// @param (color)	色
	// @param (flg)		モデルとの前後関係(真ならモデルより前)
	void	Render(D3DXVECTOR2 pos, D3DXVECTOR2 scale, D3DXVECTOR4 color, bool flg);

	void SetAlfa(float alpha);

private:
	static ID3D11Device*				m_pDevice;
	static ID3D11DeviceContext*			m_pDeviceContext;
	static ID3D11SamplerState*			m_pSampleLinear;
	static ID3D11VertexShader*			m_pVertexShader;
	static ID3D11PixelShader*			m_pPixelShader;
	static ID3D11InputLayout*			m_pVertexLayout;
	static ID3D11Buffer*				m_pConstantBuffer;
	static ID3D11BlendState*			m_pBlendState;
	static D3D11_BLEND_DESC				dd;
	static D3DXMATRIX					m_mView;
	static D3DXMATRIX					m_mProj;

	ID3D11Buffer*				m_pVertexBuffer;
	ID3D11ShaderResourceView*	m_pAsciiTexture;
	ID3D11Buffer*				m_RectBuffer;
	float						m_fScale;				//25pixelを基準 25pixel=1.0f
	float						m_fAlpha;
	int							m_iAnimSpeed;
	int							m_iAnimCount;
	D3DXVECTOR4					m_vColor;
	D3DXVECTOR2					m_Size;

	static void	StopBlendState() {
		dd.RenderTarget[0].BlendEnable = false;
		m_pDevice->CreateBlendState(&dd, &m_pBlendState);
	}
	static void StartBlendState() {
		dd.RenderTarget[0].BlendEnable = true;
		m_pDevice->CreateBlendState(&dd, &m_pBlendState);
	}
};