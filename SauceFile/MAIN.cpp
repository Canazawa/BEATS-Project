#include "../SauceFile/CGame.h"
#include "../resource.h"

#define DEBUGMODE
#include "../SauceFile/DEBUG/DEBUG.H"

//�O���[�o���ϐ�
CGame* g_pMain = NULL;

//�֐��v���g�^�C�v�̐錾
LRESULT CALLBACK WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);

//�A�v���P�[�V�����̃G���g���[�֐�
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, INT)
{
	// �f�o�b�O���C�u����������
	INITDEBUG();
	CLEARDEBUG();
	g_pMain = new CGame;
	if (g_pMain != NULL)
	{
		if (SUCCEEDED(g_pMain->InitWindow(hInstance, 0, 0, WINDOW_WIDTH,
			WINDOW_HEIGHT, APP_NAME)))
		{
			if (SUCCEEDED(g_pMain->Init()))
			{
				g_pMain->Run();
			}
		}
		//�A�v���I��
		g_pMain->Destroy();
		delete g_pMain;
	}
	return 0;
}

//OS���猩���E�B���h�E�v���V�[�W���[�i���ۂ̏�����MAIN�N���X�̃v���V�[�W���[�ŏ����j
LRESULT CALLBACK WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	return g_pMain->MsgProc(hWnd, uMsg, wParam, lParam);
}

//�E�B���h�E�쐬
HRESULT CGame::InitWindow(HINSTANCE hInstance,
		INT iX,INT iY,INT iWidth,INT iHeight,LPCWSTR WindowName)
 {
#define MY_ICON_ MAKEINTRESOURCE (IDI_ICON1)

	 // �E�B���h�E�̒�`
	WNDCLASSEX  wc;
	ZeroMemory(&wc,sizeof(wc));
	wc.cbSize = sizeof(wc);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(hInstance, MY_ICON_);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
	wc.lpszClassName = WindowName;
	wc.hIconSm = LoadIcon(hInstance, MY_ICON_);
	RegisterClassEx(&wc);

	// �E�B���h�E�̍쐬
	m_hWnd=CreateWindow(WindowName,WindowName,WS_OVERLAPPEDWINDOW,
		0,0,iWidth,iHeight,0,0,hInstance,0);
	 if(!m_hWnd) 
	 {
		 return E_FAIL;
	 }
	 // �E�C���h�E�̕\��
	 ShowWindow(m_hWnd,SW_SHOW);
	 UpdateWindow(m_hWnd);

	 CoInitialize(NULL);

	 // DirectSound����
	 if (!ds.Create(m_hWnd))
	 {
		 DEBUG("DirectAudio create error\n");
		 CoUninitialize();//�����Ɏ��s���Ă�����폜
		 return E_FAIL;
	 }
	
	 return S_OK;
 }

//�E�B���h�E�v���V�[�W���[
 LRESULT CGame::MsgProc(HWND hWnd,UINT iMsg,WPARAM wParam,LPARAM lParam)
{
	switch(iMsg)
	{
		case WM_KEYDOWN:
		switch((char)wParam)
		{
			case VK_ESCAPE://ESC�L�[�ŏC��
			PostQuitMessage(0);
			break;
		}
		break;
		case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc (hWnd, iMsg, wParam, lParam);
 }

 //������
HRESULT CGame::Init()
{
	// �f�o�C�X�ƃX���b�v�`�F�[���̍쐬
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory( &sd, sizeof(sd) );
	sd.BufferCount = 1;
	sd.BufferDesc.Width = WINDOW_WIDTH;
	sd.BufferDesc.Height = WINDOW_HEIGHT;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = m_hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	D3D_FEATURE_LEVEL pFeatureLevels = D3D_FEATURE_LEVEL_11_0; 
	D3D_FEATURE_LEVEL* pFeatureLevel = NULL;

    if( FAILED( D3D11CreateDeviceAndSwapChain(NULL,D3D_DRIVER_TYPE_HARDWARE,NULL, 
		0,&pFeatureLevels,1,D3D11_SDK_VERSION,&sd,&m_pSwapChain,&m_pDevice,
		pFeatureLevel,&m_pDeviceContext )) )
	{
		return FALSE;
	}
	//�e��e�N�X�`���[�ƁA����ɕt�т���e��r���[���쐬

	//�o�b�N�o�b�t�@�[�e�N�X�`���[���擾�i���ɂ���̂ō쐬�ł͂Ȃ��j
	ID3D11Texture2D *pBackBuffer_Tex;
	m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer_Tex);
	//���̃e�N�X�`���[�ɑ΂������_�[�^�[�Q�b�g�r���[(RTV)���쐬
	m_pDevice->CreateRenderTargetView(pBackBuffer_Tex, NULL, &m_pBackBuffer_TexRTV);
	SAFE_RELEASE(pBackBuffer_Tex);	

	//�f�v�X�X�e���V���r���[�p�̃e�N�X�`���[���쐬
	D3D11_TEXTURE2D_DESC descDepth;
	descDepth.Width = WINDOW_WIDTH;
	descDepth.Height = WINDOW_HEIGHT;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	m_pDevice->CreateTexture2D( &descDepth, NULL, &m_pBackBuffer_DSTex );
	//���̃e�N�X�`���[�ɑ΂��f�v�X�X�e���V���r���[(DSV)���쐬
	m_pDevice->CreateDepthStencilView( m_pBackBuffer_DSTex, NULL, &m_pBackBuffer_DSTexDSV);

	//�����_�[�^�[�Q�b�g�r���[�Ɛ[�x�X�e���V���r���[���p�C�v���C���Ƀo�C���h
	m_pDeviceContext->OMSetRenderTargets(1, &m_pBackBuffer_TexRTV,m_pBackBuffer_DSTexDSV);
	//�r���[�|�[�g�̐ݒ�
	D3D11_VIEWPORT vp;
	vp.Width = WINDOW_WIDTH;
	vp.Height = WINDOW_HEIGHT;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	m_pDeviceContext->RSSetViewports( 1, &vp );
	//���X�^���C�Y�ݒ�
	D3D11_RASTERIZER_DESC rdc;
	ZeroMemory(&rdc,sizeof(rdc));
	rdc.CullMode=D3D11_CULL_NONE;
	rdc.FillMode=D3D11_FILL_SOLID;
	ID3D11RasterizerState* pIr=NULL;
	m_pDevice->CreateRasterizerState(&rdc,&pIr);
	m_pDeviceContext->RSSetState(pIr);
	SAFE_RELEASE(pIr);

	return S_OK;
}

// ���
void CGame::Destroy()
{
	delete m_pText;
	delete m_pTextSizeS;

	// ���f���f���[�g
	SAFE_DELETE(m_pLane);
	SAFE_DELETE(m_pDome);
	SAFE_DELETE(m_pBar);
	SAFE_DELETE(m_pNote);
	SAFE_DELETE(m_pExNote);
	SAFE_DELETE(m_pHoldLongNote);

	for (int i = 0; i < 8; i++)
	{
		if (i < 4)
		{
			SAFE_DELETE(m_pJudgeMark[i]);
		}
		SAFE_DELETE(m_pLine[i]);
	}

	SAFE_RELEASE(m_pSwapChain);
	SAFE_RELEASE(m_pBackBuffer_TexRTV);
	SAFE_RELEASE(m_pBackBuffer_DSTexDSV);
	SAFE_RELEASE(m_pBackBuffer_DSTex);
	SAFE_RELEASE(m_pDevice);
}
