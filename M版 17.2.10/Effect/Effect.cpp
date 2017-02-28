#include "Effect.h"

map<string, int>				effect_;

::Effekseer::Manager* Effect::g_manager = nullptr;
::EffekseerRenderer::Renderer* Effect::g_renderer = nullptr;

ID3D11Device* Effect::g_device = nullptr;
ID3D11DeviceContext* Effect::g_context = nullptr;


//
// @brief コンストラクタ
Effect::Effect()
{
	ZeroMemory(this, sizeof(Effect));
	for (int i = 0; i < MAX; i++)
	{
		g_handle[i] = -1;
	}

}

//
//	@brief	デストラクタ
Effect::~Effect()
{
	/*for (int i = 0; i < g_effect.size(); i++)
	{
		ES_SAFE_RELEASE(g_effect[0]);
	}*/

	// 先にエフェクト管理用インスタンスを破棄
	g_manager->Destroy();

	// 次に描画用インスタンスを破棄
	g_renderer->Destory();

	ES_SAFE_RELEASE(g_context);
	ES_SAFE_RELEASE(g_device);
}

//
//	@brief	初期化
void Effect::EffectInit(HWND whand, ID3D11Device* device, ID3D11DeviceContext* context)
{
	g_device = device;
	g_context = context;
	HWND g_window_handle = whand;
	static IDXGIDevice1*				g_dxgiDevice = NULL;
	static IDXGIAdapter*				g_adapter = NULL;
	static IDXGIFactory*				g_dxgiFactory = NULL;
	static ID3D11Texture2D*				g_depthBuffer = NULL;

	static IDXGISwapChain* g_swapChain = NULL;
	static ID3D11RenderTargetView* g_renderTargetView = NULL;
	static ID3D11DepthStencilView* g_depthStencilView = NULL;
	static ID3D11Texture2D* g_backBuffer = NULL;
	//////

	// DirectX11の初期化を行う
	if (FAILED(g_device->QueryInterface(__uuidof(IDXGIDevice1), (void**)&g_dxgiDevice)))
	{
		return;
	}

	if (FAILED(g_dxgiDevice->GetAdapter(&g_adapter)))
	{
		return;
	}

	g_adapter->GetParent(__uuidof(IDXGIFactory), (void**)&g_dxgiFactory);
	if (g_dxgiFactory == NULL)
	{
		return;
	}

	DXGI_SWAP_CHAIN_DESC hDXGISwapChainDesc;
	hDXGISwapChainDesc.BufferDesc.Width = WINDOW_WIDTH;
	hDXGISwapChainDesc.BufferDesc.Height = WINDOW_HEIGHT;
	hDXGISwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	hDXGISwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	hDXGISwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	hDXGISwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	hDXGISwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	hDXGISwapChainDesc.SampleDesc.Count = 1;
	hDXGISwapChainDesc.SampleDesc.Quality = 0;
	hDXGISwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	hDXGISwapChainDesc.BufferCount = 1;
	hDXGISwapChainDesc.OutputWindow = (HWND)g_window_handle;
	hDXGISwapChainDesc.Windowed = TRUE;
	hDXGISwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	hDXGISwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	if (FAILED(g_dxgiFactory->CreateSwapChain(g_device, &hDXGISwapChainDesc, &g_swapChain)))
	{
		return;
	}

	if (FAILED(g_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&g_backBuffer)))
	{
		return;
	}

	if (FAILED(g_device->CreateRenderTargetView(g_backBuffer, NULL, &g_renderTargetView)))
	{
		return;
	}

	D3D11_TEXTURE2D_DESC hTexture2dDesc;
	hTexture2dDesc.Width = hDXGISwapChainDesc.BufferDesc.Width;
	hTexture2dDesc.Height = hDXGISwapChainDesc.BufferDesc.Height;
	hTexture2dDesc.MipLevels = 1;
	hTexture2dDesc.ArraySize = 1;
	hTexture2dDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	hTexture2dDesc.SampleDesc = hDXGISwapChainDesc.SampleDesc;
	hTexture2dDesc.Usage = D3D11_USAGE_DEFAULT;
	hTexture2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	hTexture2dDesc.CPUAccessFlags = 0;
	hTexture2dDesc.MiscFlags = 0;
	if (FAILED(g_device->CreateTexture2D(&hTexture2dDesc, NULL, &g_depthBuffer)))
	{
		return;
	}

	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width = (float)WINDOW_WIDTH;
	vp.Height = (float)WINDOW_HEIGHT;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	g_context->RSSetViewports(1, &vp);


	//読み込み
	Set();
}

//
//	@brief	描画準備
void Effect::Set()
{
	// 描画用インスタンスの生成
	g_renderer = ::EffekseerRendererDX11::Renderer::Create(g_device, g_context, 2000);

	// エフェクト管理用インスタンスの生成
	g_manager = ::Effekseer::Manager::Create(2000);

	// 描画用インスタンスから描画機能を設定
	g_manager->SetSpriteRenderer(g_renderer->CreateSpriteRenderer());
	g_manager->SetRibbonRenderer(g_renderer->CreateRibbonRenderer());
	g_manager->SetRingRenderer(g_renderer->CreateRingRenderer());
	g_manager->SetTrackRenderer(g_renderer->CreateTrackRenderer());
	g_manager->SetModelRenderer(g_renderer->CreateModelRenderer());

	// 描画用インスタンスからテクスチャの読込機能を設定
	// 独自拡張可能、現在はファイルから読み込んでいる。
	g_manager->SetTextureLoader(g_renderer->CreateTextureLoader());
	g_manager->SetModelLoader(g_renderer->CreateModelLoader());

	// エフェクトの読込
	EffectSet();
	//effect_["smork"] = LoadEffect((const EFK_CHAR*)L"Effekseer/Effects/盾/smork.efk");
	//effect_["smork"] = LoadEffect((const EFK_CHAR*)L"Effekseer/EffectsMaterial/bi-m2.efk");
	
	//g_handle = g_manager->Play(g_effect, 0, 0, 0);
}

//
//	@brief	エフェクト読み込み
void Effect::EffectSet()
{
	//全体
	effect_["PERFECT"] = LoadEffect((const EFK_CHAR*)L"Effect/EffectsMaterial/PERFECT.efk");
	effect_["GREAT"] = LoadEffect((const EFK_CHAR*)L"Effect/EffectsMaterial/GREAT.efk");
	effect_["GOOD"] = LoadEffect((const EFK_CHAR*)L"Effect/EffectsMaterial/GOOD.efk");
	effect_["BAD"] = LoadEffect((const EFK_CHAR*)L"Effect/EffectsMaterial/BAD.efk");
	effect_["ExNote"] = LoadEffect((const EFK_CHAR*)L"Effect/EffectsMaterial/ExNote.efk");

	for (int i = 0; i < 4; i++)
	{
		effect_["HoldNote" + i] = LoadEffect((const EFK_CHAR*)L"Effect/EffectsMaterial/HoldNote.efk");
	}

	effect_["READY"] = LoadEffect((const EFK_CHAR*)L"Effect/EffectsMaterial/Ready.efk");
	effect_["CLEAR_RESULT"] = LoadEffect((const EFK_CHAR*)L"Effect/EffectsMaterial/Clear_result.efk");
	effect_["FAILED_RESULT"] = LoadEffect((const EFK_CHAR*)L"Effect/EffectsMaterial/FAILED_result.efk");
	effect_["PERFECT_RESULT"] = LoadEffect((const EFK_CHAR*)L"Effect/EffectsMaterial/PERFECT_Result.efk");

	effect_["PERFECT_Font"] = LoadEffect((const EFK_CHAR*)L"Effect/EffectsMaterial/PERFECT_Font.efk");
	effect_["GREAT_Font"] = LoadEffect((const EFK_CHAR*)L"Effect/EffectsMaterial/GREAT_Font.efk");
	effect_["GOOD_Font"] = LoadEffect((const EFK_CHAR*)L"Effect/EffectsMaterial/GOOD_Font.efk");
	effect_["BAD_Font"] = LoadEffect((const EFK_CHAR*)L"Effect/EffectsMaterial/BAD_Font.efk");

	effect_["Clear_Mode"] = LoadEffect((const EFK_CHAR*)L"Effect/EffectsMaterial/Clear_Mode.efk");

	//effect_["charge2"] = LoadEffect((const EFK_CHAR*)L"Effekseer/Effects/全体/charge2.efk");

	//剣士

	//盾士
	//effect_["smork"] = LoadEffect((const EFK_CHAR*)L"Effekseer/Effects/盾士/smork.efk");

	//爆弾士
	//effect_["explosion"] = LoadEffect((const EFK_CHAR*)L"Effekseer/Effects/爆弾士/explosion.efk");

	//魔導士
	//effect_["beam1"] = LoadEffect((const EFK_CHAR*)L"Effekseer/Effects/魔導士/beam1.efk");

	//effect_["beam2"] = LoadEffect((const EFK_CHAR*)L"Effekseer/Effects/魔導士/bi-m2.efk");

	//姫

	//
}

//
//	@brief	エフェクトのロード
int Effect::LoadEffect(const EFK_CHAR* path)
{
	static int no = -1;
	++no;
	g_effect[no] = Effekseer::Effect::Create(g_manager, path);
	return no;
}

//
//	@brief			カメラセット
//	@param(pos)		カメラの視点座標
//	@param(look)	カメラの注視点座標
void Effect::SetCamera(D3DXVECTOR3 pos, D3DXVECTOR3 look)
{
	Effekseer::Vector3D g_position = ::Effekseer::Vector3D(pos.x, pos.y, pos.z);
	const Effekseer::Vector3D g_at = ::Effekseer::Vector3D(look.x, look.y, look.z);
	const Effekseer::Matrix44 m_View = ::Effekseer::Matrix44().LookAtLH(g_position, g_at, ::Effekseer::Vector3D(0, 1, 0));
	const Effekseer::Matrix44 m_Proj = ::Effekseer::Matrix44().PerspectiveFovLH(D3DX_PI / 6, (FLOAT)WINDOW_WIDTH / (FLOAT)WINDOW_HEIGHT, 0.1f, 1000.0f);

	// 投影行列を設定
	g_renderer->SetProjectionMatrix(m_Proj);

	// カメラ行列を設定
	g_renderer->SetCameraMatrix(m_View);

}

//
//	@brief			描画開始
//	@param (pos)	描画座標
void Effect::Effect_Play(string name, D3DXVECTOR3 pos)
{
	g_handle[effect_[name]] = g_manager->Play(g_effect[effect_[name]], pos.x, pos.y, pos.z);
}

//
//	@brief			描画終了
//	@param (name)	描画を終了するエフェクト名
void Effect::Effect_Stop(string name)
{
	g_manager->StopEffect(g_handle[effect_[name]]);
}

//
//	@brief			スケールの設定
//	@param (name)	スケールを設定するエフェクト
//	@param (scale)	拡大率
void Effect::SetScale(string name, float scale)
{
	g_manager->SetScale(g_handle[effect_[name]], scale, scale, scale);
}

//
//	@brief			更新
//	@param(name)	更新するエフェクト名
//	@param(pos)		描画座標
void Effect::Update(string name)
{
	//　座標設定
	//::Effekseer::Vector3D e_pos(pos.x, pos.y, pos.z);
	//　移動更新
	//g_manager->SetLocation(g_handle[effect_[name]], e_pos);
	// エフェクトの更新処理を行う
	g_manager->Update();
}

void Effect::Draw()
{
	// エフェクトの描画開始処理を行う。
	g_renderer->BeginRendering();

	// エフェクトの描画を行う。
	g_manager->Draw();

	// エフェクトの描画終了処理を行う。
	g_renderer->EndRendering();
}
