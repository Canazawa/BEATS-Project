#include "TD_Graphics.h"

ID3D11Device* TD_Graphics::m_pDevice = nullptr;
ID3D11DeviceContext* TD_Graphics::m_pDeviceContext = nullptr;
ID3D11SamplerState* TD_Graphics::m_pSampleLinear = nullptr;
ID3D11VertexShader* TD_Graphics::m_pVertexShader = nullptr;
ID3D11PixelShader* TD_Graphics::m_pPixelShader = nullptr;
ID3D11InputLayout* TD_Graphics::m_pVertexLayout = nullptr;
ID3D11Buffer* TD_Graphics::m_pConstantBuffer = nullptr;
ID3D11BlendState* TD_Graphics::m_pBlendState;
D3D11_BLEND_DESC TD_Graphics::dd;
//ID3D11Buffer* TD_Graphics::m_pVertexBuffer/*[100]*/;
D3DXMATRIX TD_Graphics::m_mView;
D3DXMATRIX TD_Graphics::m_mProj;

//=======================================================
//	コンストラクタ
//=======================================================
TD_Graphics::TD_Graphics()
{
	ZeroMemory(this, sizeof(TD_Graphics));
	m_fScale = 1.0f;
}

//=======================================================
//	デストラクタ
//=======================================================
TD_Graphics::~TD_Graphics()
{

	SAFE_RELEASE(m_pAsciiTexture);
}

HRESULT TD_Graphics::Init(LPCWSTR textname, /*int texnum, */D3DXVECTOR2 drawpos, D3DXVECTOR2 texsize, D3DXVECTOR4 vColor, GrapRect _Rect)
{
	m_fAlpha = vColor.w;
	m_vColor = vColor;

	float left = drawpos.x, top = drawpos.y, right = texsize.x + left, bottom = texsize.y + top;
	m_Size = texsize;
	GrapRect rect = GrapRect(0.0f, 0.0f, 1.0f / m_Size.x, 1.0f / m_Size.y);
	//SimpleVertex vertices[] =
	//{
	//	D3DXVECTOR3(left, bottom, 0), D3DXVECTOR2(_Rect.m_left, _Rect.m_bottom),//頂点1,
	//	D3DXVECTOR3(left, top, 0), D3DXVECTOR2(_Rect.m_left, _Rect.m_top),//頂点2
	//	D3DXVECTOR3(right, bottom, 0), D3DXVECTOR2(_Rect.m_right, _Rect.m_bottom), //頂点3
	//	D3DXVECTOR3(right, top, 0), D3DXVECTOR2(_Rect.m_right, _Rect.m_top), //頂点4
	//};
	SimpleVertex vertices[] =
	{
		D3DXVECTOR3(0, texsize.y, 0), D3DXVECTOR2(_Rect.m_left, _Rect.m_bottom),//頂点1,
		D3DXVECTOR3(0, 0, 0), D3DXVECTOR2(_Rect.m_left, _Rect.m_top),//頂点2
		D3DXVECTOR3(texsize.x,texsize.y, 0), D3DXVECTOR2(_Rect.m_right, _Rect.m_bottom), //頂点3
		D3DXVECTOR3(texsize.x, 0, 0), D3DXVECTOR2(_Rect.m_right, _Rect.m_top), //頂点4
	};


	//SimpleVertex vertices[] =
	//{
	//	D3DXVECTOR3(0, bottom, 0), D3DXVECTOR2(rect.m_left, rect.m_bottom),//頂点1,
	//	D3DXVECTOR3(0, 0, 0), D3DXVECTOR2(rect.m_left, rect.m_top),//頂点2
	//	D3DXVECTOR3(right, bottom, 0), D3DXVECTOR2(rect.m_right, rect.m_bottom), //頂点3
	//	D3DXVECTOR3(right, 0, 0), D3DXVECTOR2(rect.m_right, rect.m_top), //頂点4
	//};

	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = vertices;
	ID3D11Device* device = m_pDevice;
	if (FAILED(device->CreateBuffer(&bd, &InitData, &m_pVertexBuffer)))
	{
		return E_FAIL;
	}

	LPCWSTR texturename = textname;
	//テクスチャーを作成
	if (FAILED(D3DX11CreateShaderResourceViewFromFile(device, texturename, NULL, NULL, &m_pAsciiTexture, NULL)))
	{		
		return E_FAIL;
	}

	return S_OK;
}

HRESULT TD_Graphics::AnimInit(D3DXVECTOR4 vColor, bool flg_alpha, WCHAR *filepass, D3DXVECTOR2 size, int animSpeed)
{
	m_fAlpha = vColor.w;
	m_vColor = vColor;
	m_Size = size;
	m_iAnimSpeed = animSpeed;

	////デバイスとコンテキストをコピー
	//m_pDeviceContext = pContext;
	//m_pDeviceContext->GetDevice(&m_pDevice);
	////window サイズ
	//m_dwWindowWidth = width;
	//m_dwWindowHeight = height;

	//画像表示位置指定　デフォルトは全域
	D3DXVECTOR4 rect = D3DXVECTOR4(0.0f, 0.0f, 1.0f / m_Size.x, 1.0f / m_Size.y);
	TextVertex vertices[] =
	{
		D3DXVECTOR3(0, TEX_DIMENSION, 0), D3DXVECTOR2(rect.x, rect.w),//頂点1,
		D3DXVECTOR3(0, 0, 0), D3DXVECTOR2(rect.x, rect.y),//頂点2
		D3DXVECTOR3(TEX_DIMENSION, TEX_DIMENSION, 0), D3DXVECTOR2(rect.z, rect.w), //頂点3
		D3DXVECTOR3(TEX_DIMENSION, 0, 0), D3DXVECTOR2(rect.z, rect.y), //頂点4
	};

	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(TextVertex) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = vertices;
	if (FAILED(m_pDevice->CreateBuffer(&bd, &InitData, &m_pVertexBuffer)))
	{
		return E_FAIL;
	}

	//テクスチャー用サンプラー作成
	D3D11_SAMPLER_DESC SamDesc;
	ZeroMemory(&SamDesc, sizeof(D3D11_SAMPLER_DESC));
	SamDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	SamDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	m_pDevice->CreateSamplerState(&SamDesc, &m_pSampleLinear);

	//テクスチャーを作成
	ID3D11ShaderResourceView* temp;
	if (FAILED(D3DX11CreateShaderResourceViewFromFile(m_pDevice, filepass, NULL, NULL, &temp, NULL)))
	{
		return E_FAIL;
	}



	//#pragma region Shader
	//
	//	//hlslファイル読み込み ブロブ作成　ブロブとはシェーダーの塊みたいなもの。XXシェーダーとして特徴を持たない。後で各種シェーダーに成り得る。
	//	ID3DBlob *pCompiledShader = NULL;
	//	ID3DBlob *pErrors = NULL;
	//	//ブロブからバーテックスシェーダー作成
	//	if (FAILED(D3DX11CompileFromFile(L"./UI/D3D11_SPRITE.hlsl", NULL, NULL, "VS", "vs_5_0", 0, 0, NULL, &pCompiledShader, &pErrors, NULL)))
	//	{
	//		MessageBox(0, L"hlsl読み込み失敗", NULL, MB_OK);
	//		return E_FAIL;
	//	}
	//	SAFE_RELEASE(pErrors);
	//
	//	if (FAILED(m_pDevice->CreateVertexShader(pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), NULL, &m_pVertexShader)))
	//	{
	//		SAFE_RELEASE(pCompiledShader);
	//		MessageBox(0, L"バーテックスシェーダー作成失敗", NULL, MB_OK);
	//		return E_FAIL;
	//	}
	//	//頂点インプットレイアウトを定義	
	//	D3D11_INPUT_ELEMENT_DESC layout[] =
	//	{
	//		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//	};
	//	UINT numElements = sizeof(layout) / sizeof(layout[0]);
	//
	//	//頂点インプットレイアウトを作成
	//	if (FAILED(m_pDevice->CreateInputLayout(layout, numElements, pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), &m_pVertexLayout)))
	//		return FALSE;
	//	//ブロブからピクセルシェーダー作成
	//	if (FAILED(D3DX11CompileFromFile(L"./UI/D3D11_SPRITE.hlsl", NULL, NULL, "PS", "ps_5_0", 0, 0, NULL, &pCompiledShader, &pErrors, NULL)))
	//	{
	//		MessageBox(0, L"hlsl読み込み失敗", NULL, MB_OK);
	//		return E_FAIL;
	//	}
	//	SAFE_RELEASE(pErrors);
	//	if (FAILED(m_pDevice->CreatePixelShader(pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), NULL, &m_pPixelShader)))
	//	{
	//		SAFE_RELEASE(pCompiledShader);
	//		MessageBox(0, L"ピクセルシェーダー作成失敗", NULL, MB_OK);
	//		return E_FAIL;
	//	}
	//	SAFE_RELEASE(pCompiledShader);
	//	//コンスタントバッファー作成　ここでは変換行列渡し用
	//	D3D11_BUFFER_DESC cb;
	//	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	//	cb.ByteWidth = sizeof(SPRITE_CONSTANT_BUFFER);
	//	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	//	cb.MiscFlags = 0;
	//	cb.StructureByteStride = 0;
	//	cb.Usage = D3D11_USAGE_DYNAMIC;
	//
	//	if (FAILED(m_pDevice->CreateBuffer(&cb, NULL, &m_pConstantBuffer)))
	//	{
	//		return E_FAIL;
	//	}
	//
	//	if (flg_alpha)
	//	{
	//		//抜け色設定
	//		D3D11_BLEND_DESC bd2;
	//		ZeroMemory(&bd2, sizeof(D3D11_BLEND_DESC));
	//		bd2.IndependentBlendEnable = false;
	//		bd2.AlphaToCoverageEnable = false;
	//		bd2.RenderTarget[0].BlendEnable = true;
	//		bd2.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	//		bd2.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	//		bd2.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	//		bd2.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	//		bd2.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	//		bd2.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	//		bd2.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	//
	//		if (FAILED(m_pDevice->CreateBlendState(&bd2, &m_pBlendState)))
	//		{
	//			return E_FAIL;
	//		}
	//	}
	//#pragma endregion
}

HRESULT TD_Graphics::InitShader(ID3D11DeviceContext * pContext)
{
	m_pDeviceContext = pContext;
	m_pDeviceContext->GetDevice(&m_pDevice);

	//テクスチャー用サンプラー作成
	D3D11_SAMPLER_DESC SamDesc;
	ZeroMemory(&SamDesc, sizeof(D3D11_SAMPLER_DESC));
	SamDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	SamDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	m_pDevice->CreateSamplerState(&SamDesc, &m_pSampleLinear);

	//hlslファイル読み込み ブロブ作成　ブロブとはシェーダーの塊みたいなもの。XXシェーダーとして特徴を持たない。後で各種シェーダーに成り得る。
	ID3DBlob *pCompiledShader = NULL;
	ID3DBlob *pErrors = NULL;
	//ブロブからバーテックスシェーダー作成
	if (FAILED(D3DX11CompileFromFile(L"hlsl/D3D11_SPRITE.hlsl", NULL, NULL, "VS", "vs_5_0", 0, 0, NULL, &pCompiledShader, &pErrors, NULL)))
	{
		MessageBox(0, L"hlsl読み込み失敗", NULL, MB_OK);
		return E_FAIL;
	}
	SAFE_RELEASE(pErrors);

	if (FAILED(m_pDevice->CreateVertexShader(pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), NULL, &m_pVertexShader)))
	{
		SAFE_RELEASE(pCompiledShader);
		MessageBox(0, L"バーテックスシェーダー作成失敗", NULL, MB_OK);
		return E_FAIL;
	}
	//頂点インプットレイアウトを定義
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = sizeof(layout) / sizeof(layout[0]);

	//頂点インプットレイアウトを作成
	if (FAILED(m_pDevice->CreateInputLayout(layout, numElements, pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), &m_pVertexLayout)))
		return FALSE;
	//ブロブからピクセルシェーダー作成
	if (FAILED(D3DX11CompileFromFile(L"hlsl/D3D11_SPRITE.hlsl", NULL, NULL, "PS", "ps_5_0", 0, 0, NULL, &pCompiledShader, &pErrors, NULL)))
	{
		MessageBox(0, L"hlsl読み込み失敗", NULL, MB_OK);
		return E_FAIL;
	}
	SAFE_RELEASE(pErrors);
	if (FAILED(m_pDevice->CreatePixelShader(pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), NULL, &m_pPixelShader)))
	{
		SAFE_RELEASE(pCompiledShader);
		MessageBox(0, L"ピクセルシェーダー作成失敗", NULL, MB_OK);
		return E_FAIL;
	}
	SAFE_RELEASE(pCompiledShader);
	//コンスタントバッファー作成　ここでは変換行列渡し用
	D3D11_BUFFER_DESC cb;
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	//cb.ByteWidth = sizeof(SIMPLESHADER_CONSTANT_BUFFER);
	cb.ByteWidth = sizeof(SPRITE_CONSTANT_BUFFER);
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags = 0;
	cb.StructureByteStride = 0;
	cb.Usage = D3D11_USAGE_DYNAMIC;

	if (FAILED(m_pDevice->CreateBuffer(&cb, NULL, &m_pConstantBuffer)))
	{
		return E_FAIL;
	}

	//抜け色設定
	D3D11_BLEND_DESC dd;
	ZeroMemory(&dd, sizeof(D3D11_BLEND_DESC));
	dd.IndependentBlendEnable = false;
	dd.AlphaToCoverageEnable = true;
	dd.RenderTarget[0].BlendEnable = true;
	dd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	dd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	dd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	dd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	dd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	dd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	dd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	if (FAILED(m_pDevice->CreateBlendState(&dd, &m_pBlendState)))
	{
		return E_FAIL;
	}

	return S_OK;
}

//
//
void TD_Graphics::Render(D3DXVECTOR2 pos, D3DXVECTOR2 scale, D3DXVECTOR4 color, bool flg)
{
	//トポロジー
	m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//頂点インプットレイアウトをセット
	m_pDeviceContext->IASetInputLayout(m_pVertexLayout);

	//使用するシェーダーの登録
	m_pDeviceContext->VSSetShader(m_pVertexShader, NULL, 0);
	m_pDeviceContext->PSSetShader(m_pPixelShader, NULL, 0);
	//このコンスタントバッファーを使うシェーダーの登録
	m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	m_pDeviceContext->PSSetConstantBuffers(0, 1, &m_pConstantBuffer);

	//テクスチャーをシェーダーに渡す
	m_pDeviceContext->PSSetSamplers(0, 1, &m_pSampleLinear);
	m_pDeviceContext->PSSetShaderResources(0, 1, &m_pAsciiTexture);

	//ワールド変換
	float z = 0;
	if (flg)
	{
		z = -1;
	}
	D3DXMATRIX mWorld, mTran, mScale;
	//D3DXMatrixIdentity(&mWorld);
	D3DXMatrixTranslation(&mTran, pos.x, pos.y, z);
	//スケール変換
	//D3DXMatrixIdentity(&mScale);
	D3DXMatrixScaling(&mScale, scale.x, scale.y, 1);
	mWorld = mScale*mTran;
	//シェーダーのコンスタントバッファーに各種データを渡す	
	D3D11_MAPPED_SUBRESOURCE pData;
	SPRITE_CONSTANT_BUFFER cb;
	m_pDeviceContext->Map(m_pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData);
	//ワールド、カメラ、射影行列を渡す
	D3DXMATRIX m = mWorld*m_mView*m_mProj;
	D3DXMatrixTranspose(&m, &m);
	cb.mWVP = m;
	//カラーを渡す
	cb.vColor = color;
	//透明度を渡す
	cb.fAlpha.x = color.w;

	memcpy_s(pData.pData, pData.RowPitch, (void*)(&cb), sizeof(cb));
	m_pDeviceContext->Unmap(m_pConstantBuffer, 0);
	//バーテックスバッファーをセット
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	m_pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	//抜け色
	UINT ColorKey = 0xffffffff;
	m_pDeviceContext->OMSetBlendState(m_pBlendState, NULL, ColorKey);
	m_pDeviceContext->Draw(4, 0);
}

void TD_Graphics::SetAlfa(float alfa)
{
	m_fAlpha = alfa;
}

void TD_Graphics::SetCamera(D3DXMATRIX view, D3DXMATRIX proj)
{
	m_mView = view;
	m_mProj = proj;

	StartBlendState();//ブレンドを有効に

	// ビュートランスフォーム（視点座標変換）
	D3DXVECTOR3 vEyePt(0.0f, 0.0f, -1); //カメラ（視点）位置
	D3DXVECTOR3 vLookatPt(0.0f, 0.0f, 0.0f);//注視位置
	D3DXVECTOR3 vUpVec(0.0f, 1.0f, 0.0f);//上方位置
	D3DXMatrixLookAtLH(&m_mView, &vEyePt, &vLookatPt, &vUpVec);

	// プロジェクショントランスフォーム（射影変換）
	D3DMATRIX mOtho = {
		2.0f / (float)(WINDOW_WIDTH), 0.0f, 0.0f, 0.0f,
		0.0f, -2.0f / (float)(WINDOW_HEIGHT), 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f, 1.0f
	};
	m_mProj = mOtho;

}