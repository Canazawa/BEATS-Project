#pragma once

//ヘッダーファイルのインクルード
#include <stdio.h>
#include <Windows.h>
#include <d3dx9.h>
#include <d3d10_1.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dCompiler.h>
#include "../Sprite/D3D11_SPRITE.h"
#include "../Text/D3D11_TEXT.h"
//#include <XAudio2.h>

#include <Effekseer.h>
#include <EffekseerRendererDX11.h>
//#include <EffekseerSoundXAudio2.h>

//#include "./../../Effect/Compiled/lib/VS2015/Debug/"


#if _DEBUG
#pragma comment(lib, "Effect/Compiled/lib/VS2015/Debug/Effekseer.lib" )
#pragma comment(lib, "Effect/Compiled/lib/VS2015/Debug/EffekseerRendererDX11.lib" )
//#pragma comment(lib, "Effect/Compiled/lib/VS2015/Debug/EffekseerSoundXAudio2.lib" )
#else
#pragma comment(lib, "Effect/Compiled/lib/VS2015/Release/Effekseer.lib" )
#pragma comment(lib, "Effect/Compiled/lib/VS2015/Release/EffekseerRendererDX11.lib" )
//#pragma comment(lib, "../../Effect/Compiled/lib/VS2015/Release/EffekseerSoundXAudio2.lib" )
#endif


//必要なライブラリファイルのロード
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"d3dx10.lib")
#pragma comment(lib,"d3dx11.lib")
#pragma comment(lib,"d3dCompiler.lib")


// マクロ定義をインクルード
#include "Defines.h"