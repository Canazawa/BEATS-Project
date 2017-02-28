#include "CGame.h"
#include <stdio.h>

#define DEBUGMODE 
#include "DEBUG/DEBUG.H"

D3DXMATRIX CGame::mView;
D3DXMATRIX CGame::mProj;

//コンストラクタ
CGame::CGame()
{
	GameState = GS_INITLOAD; // 初期化&ロードをステートの最初とする
	Hi_SPEED = 1.0f;
	SelectCnt = 0;
	PERFECT = 0;
	GREAT = 0;
	GOOD = 0;
	BAD = 0;
	Combo = 0;
	MaxCombo = 0;
	Gage_y = 0.0f;
	Now_Score = 0;
	Max_Score = 0;
	SpriteColor = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
	Flashing = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
	ChangeSceneColor = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 0.0f);
	AddFlash = 0.01f;
	TitleCurPos = 370;
	ChangeCnt = 0;
	SelectMixWin = 0.0f;
	SceneChangeFlg = false;
	OnePlayBGM = true;
	GameStartFlg = false;
	ResultEffectFlg = true;
	ZeroMemory(&iStartNum, sizeof(iStartNum));
	ZeroMemory(&mBFP, sizeof(mBFP));
	m_pText = new D3D11_TEXT;
	m_pTextSizeS = new D3D11_TEXT;
	m_pMusicWindow = new TD_Graphics;
	m_pSceneChangeTex = new TD_Graphics;
	for (int i = 0; i < 4; i++) { m_pMenuBG[i] = new TD_Graphics; }
	for (int i = 0; i < 5; i++) { m_pMainGameUI[i] = new TD_Graphics; }
	for (int i = 0; i < 8; i++)
	{
		if (i < 4)
		{
			m_pJudgeMark[i] = new CD3DXMESH_ANIM;
		}
		m_pLine[i] = new CD3DXMESH_ANIM;
	}
	for (int i = 0; i < 2; i++) { m_pGageType[i] = new TD_Graphics; }
	for (int i = 0; i < 2; i++) { m_pResult[i] = new TD_Graphics; }
	m_pTitleSlect = new TD_Graphics;
	m_pTutorial = new TD_Graphics;

	m_pLane = new CD3DXMESH_ANIM;
	m_pDome = new CD3DXMESH_ANIM;
	m_pDomeClear = new CD3DXMESH_ANIM;
	m_pBar = new CD3DXMESH_ANIM;
	m_pNote = new CD3DXMESH_ANIM;
	m_pExNote = new CD3DXMESH_ANIM;
	m_pHoldLongNote = new CD3DXMESH_ANIM;

	AllClear();
}

//デストラクタ
CGame::~CGame()
{
	AllClear();
}

// 初期化&ロード関数(一度のみ)
void CGame::InitLoad()
{
	DEBUG("初期化&ロード!!\n");
	ZeroMemory(&bOnKey, sizeof(bOnKey));
	ZeroMemory(&bOnHold, sizeof(bOnHold));

	ShowCursor(FALSE); //マウスカーソル非表示

	Hi_SPEED = 4.0f; // スクロールスピード

	for (int i = 0; i < 4; i++)
	{
		bOnKey[i] = false;
		bOnHold[i] = false;
	}

	//スプライト
	D3D11_SPRITE::InitShader(m_pDeviceContext, WINDOW_WIDTH, WINDOW_HEIGHT);
	//UI
	TD_Graphics::InitShader(m_pDeviceContext);

	/////////////////////////////
	// 文字列レンダリングの初期化
	/////////////////////////////
	D3DXVECTOR4 TextColor(0.9f, 0.6f, 0.0f, 1.0f);
	D3DXVECTOR4 TextColorSizeS(0.0f, 0.0f, 0.0f, 1.0f);
	if (FAILED(m_pText->Init(m_pDeviceContext, WINDOW_WIDTH, WINDOW_HEIGHT, 1, TextColor)))
	{
		return;
	}
	if (FAILED(m_pTextSizeS->Init(m_pDeviceContext, WINDOW_WIDTH, WINDOW_HEIGHT, 0.5, TextColorSizeS)))
	{
		return;
	}

	/////////////
	// 画像ロード
	/////////////
	m_pMusicWindow->Init(L"Texture/MusicWindowUI.png", D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2(1340, 570),
		D3DXVECTOR4(1.0, 1.0, 1.0, 1.0), GrapRect(0.0f, 1.0f, 0.0f, 1.0f));

	m_pMenuBG[0]->Init(L"Texture/TITLE_BG_notCre.png", D3DXVECTOR2(0.0f,0.0f), D3DXVECTOR2(WINDOW_WIDTH, WINDOW_HEIGHT), 
		D3DXVECTOR4(1.0, 1.0, 1.0, 1.0), GrapRect(0.0f, WINDOW_HEIGHT, 0.0f, WINDOW_WIDTH));
	m_pMenuBG[1]->Init(L"Texture/SELECT_BG.png", D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2(WINDOW_WIDTH, WINDOW_HEIGHT),
		D3DXVECTOR4(1.0, 1.0, 1.0, 1.0), GrapRect(0.0f, WINDOW_HEIGHT, 0.0f, WINDOW_WIDTH));
	m_pMenuBG[2]->Init(L"Texture/RESULT_BG.png", D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2(WINDOW_WIDTH, WINDOW_HEIGHT),
		D3DXVECTOR4(1.0, 1.0, 1.0, 1.0), GrapRect(0.0f, WINDOW_HEIGHT, 0.0f, WINDOW_WIDTH));
	m_pMenuBG[3]->Init(L"Texture/DECIDE_BG.png", D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2(WINDOW_WIDTH, WINDOW_HEIGHT),
		D3DXVECTOR4(1.0, 1.0, 1.0, 1.0), GrapRect(0.0f, WINDOW_HEIGHT, 0.0f, WINDOW_WIDTH));
	m_pTitleSlect->Init(L"Texture/title_arrow.png", D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2(192, 64),
		D3DXVECTOR4(1.0, 1.0, 1.0, 1.0), GrapRect(0.0f, 1.0f, 0.0f, 1.0f));
	
	// リザルトクリア、フェイルドフォント
	m_pResult[0]->Init(L"Texture/Clear_Font.png", D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2(256, 128),
		D3DXVECTOR4(1.0, 1.0, 1.0, 1.0), GrapRect(0.0f, 1.0f, 0.0f, 1.0f));
	m_pResult[1]->Init(L"Texture/Failed_Font.png", D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2(256, 128),
		D3DXVECTOR4(1.0, 1.0, 1.0, 1.0), GrapRect(0.0f, 1.0f, 0.0f, 1.0f));

	// メインゲームUI
	m_pMainGameUI[MGUI_GAGE]->Init(L"Texture/gage.png", D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2(48, 400),
		D3DXVECTOR4(1.0, 1.0, 1.0, 1.0), GrapRect(0.0f, 1.0f, 0.0f, 1.0f));
	m_pMainGameUI[MGUI_HEADER]->Init(L"Texture/GameHeader.png", D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2(512, 128),
		D3DXVECTOR4(1.0, 1.0, 1.0, 1.0), GrapRect(0.0f, 1.0f, 0.0f, 1.0f));
	m_pMainGameUI[MGUI_SCORE]->Init(L"Texture/Score.png", D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2(348, 94),
		D3DXVECTOR4(1.0, 1.0, 1.0, 1.0), GrapRect(0.0f, 1.0f, 0.0f, 1.0f));
	m_pMainGameUI[MGUI_JUDGE]->Init(L"Texture/Judge.png", D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2(272, 192),
		D3DXVECTOR4(1.0, 1.0, 1.0, 1.0), GrapRect(0.0f, 1.0f, 0.0f, 1.0f));
	m_pMainGameUI[MGUI_FRAME]->Init(L"Texture/GameFream.png", D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2(WINDOW_WIDTH, WINDOW_HEIGHT),
		D3DXVECTOR4(1.0, 1.0, 1.0, 1.0), GrapRect(0.0f, WINDOW_HEIGHT, 0.0f, WINDOW_WIDTH));
	m_pTutorial->Init(L"Texture/Tutorial.png", D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2(512, 128),
		D3DXVECTOR4(1.0, 1.0, 1.0, 1.0), GrapRect(0.0f, 1.0f, 0.0f, 1.0f));

	// ゲージのチャージ量
	m_pGageType[0]->Init(L"Texture/gage_less.png", D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2(64, 600),
		D3DXVECTOR4(1.0, 1.0, 1.0, 1.0), GrapRect(0.0f, 1.0f, 0.0f, 1.0f));
	m_pGageType[1]->Init(L"Texture/gage_clear.png", D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2(64, 600),
		D3DXVECTOR4(1.0, 1.0, 1.0, 1.0), GrapRect(0.0f, 1.0f, 0.0f, 1.0f));

	// 楽曲決定時のシーンを隠す画像
	m_pSceneChangeTex->Init(L"Texture/SceneChange.png", D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2(WINDOW_WIDTH, WINDOW_HEIGHT),
		D3DXVECTOR4(1.0, 1.0, 1.0, 1.0), GrapRect(0.0f, WINDOW_HEIGHT, 0.0f, WINDOW_WIDTH));

	///////////////
	// モデルロード
	///////////////
	if (FAILED((m_pLane->Init(m_hWnd, m_pDevice, m_pDeviceContext, "Model/Lane.x"))))
	{
		return;
	}
	

	if (FAILED((m_pDome->Init(m_hWnd, m_pDevice, m_pDeviceContext, "Model/Dome.x"))))
	{
		return;
	}
	if (FAILED((m_pDomeClear->Init(m_hWnd, m_pDevice, m_pDeviceContext, "Model/Dome_Clear.x"))))
	{
		return;
	}

	if (FAILED((m_pBar->Init(m_hWnd, m_pDevice, m_pDeviceContext, "Model/Bar.x"))))
	{
		return;
	}

	if (FAILED((m_pNote->Init(m_hWnd, m_pDevice, m_pDeviceContext, "Model/Note.x"))))
	{
		return;
	}

	if (FAILED((m_pExNote->Init(m_hWnd, m_pDevice, m_pDeviceContext, "Model/ExNote.x"))))
	{
		return;
	}

	if (FAILED((m_pHoldLongNote->Init(m_hWnd, m_pDevice, m_pDeviceContext, "Model/HoldLongNote.x"))))
	{
		return;
	}

	// ラインをモデルY座標の基準とする
	for (int i = 0; i < 8; i++)
	{
		if (i < 4) // 0～3を通常のジャッジマーク、ライン
		{
			if (FAILED((m_pLine[i]->Init(m_hWnd, m_pDevice, m_pDeviceContext, "Model/Line.x"))))
			{
				return;
			}
			if (FAILED((m_pJudgeMark[i]->Init(m_hWnd, m_pDevice, m_pDeviceContext, "Model/JudgeMark2.x"))))
			{
				return;
			}
		}
		else // 4～7を押された状態のジャッジマーク、ライン
		{
			if (FAILED((m_pLine[i]->Init(m_hWnd, m_pDevice, m_pDeviceContext, "Model/PushLine.x"))))
			{
				return;
			}
		}
	}
	
	// エフェクトの初期化
	Effect::getInstance().EffectInit(m_hWnd, m_pDevice, m_pDeviceContext);

	// サウンドロード
	ds.AddSound(BGM_TITLE, "Sound/BGM/title.wav");		// タイトルBGM
	ds.AddSound(BGM_DECIDE, "Sound/BGM/decide.wav");	// 決定BGM
	ds.AddSound(BGM_RESULT, "Sound/BGM/result.wav");	// リザルトBGM

	ds.AddSound(SE_START, "Sound/SE/title_enter.wav");	// タイトルスタートSE
	ds.AddSound(SE_SELECT, "Sound/SE/select.wav");		// セレクトSE
	ds.AddSound(SE_DECIDE, "Sound/SE/sel_enter.ogg");	// 楽曲決定SE

	ds.AddSound(SE_GREAT, "Sound/SE/GREAT.wav");		// ノートグレートSE
	ds.AddSound(SE_GOOD, "Sound/SE/GOOD.wav");			// ノートグッドSE
	ds.AddSound(SE_EXNOTE, "Sound/SE/ExNote.wav");		// エクストラノートSE

	for (int i = 0; i < 4; i++) { ds.AddSound(SE_HOLDNOTE + i, "Sound/SE/HoldNote.wav"); } // ホールドノートSE

	// 楽曲データのパス取得
	GetBMSFilePath(); // 楽曲のファイルパスを取得

	TitleSate = TS_STRAT;

	// シーン遷移
	SceneMove(GS_TITLE, 1.0f, 0.0f, D3DXVECTOR3(0.0f, 0.0f, 0.0f), BGM_TITLE, BGM_NONE);

}

// タイトル用実行ループ
void CGame::TitleRun()
{
	// 曲選択シーンへ(START)
	if (GetKeyState(KYE_DECIDE) & 0x80 || GetKeyState(VK_RETURN) & 0x80) // 「]」キー
	{
		if (!bOnKey[3])
		{
			bOnKey[3] = true;
			switch (TitleSate)
			{
			case TS_STRAT:
				ds.Stop(BGM_TITLE);
				ds.Play(SE_START);
				Flashing.w = 0.0f;
				DEBUG("曲選択ステート移動\n");
				BmsHeaderLoad(); // 最初の楽曲のヘッダー情報をロード
				SceneChangeFlg = true;
				break;

			case TS_EXIT:
				GameState = GS_END; // ゲーム終了
				break;
			}
		}
	}
	else
	{
		bOnKey[3] = false;
	}

	if (GetKeyState(VK_DOWN) & 0x80)
	{
		if (!bOnKey[5])
		{
			bOnKey[5] = true;
			if (TitleSate != TS_EXIT)
			{
				ds.Reset(SE_SELECT);
				ds.Play(SE_SELECT);
			}
			TitleSate = TS_EXIT;
		}
	}
	else
	{
		bOnKey[5] = false;
	}

	if (GetKeyState(VK_UP) & 0x80)
	{
		if (!bOnKey[4])
		{
			bOnKey[4] = true;
			if (TitleSate != TS_STRAT)
			{
				ds.Reset(SE_SELECT);
				ds.Play(SE_SELECT);
			}
			TitleSate = TS_STRAT;
		}
	}
	else
	{
		bOnKey[4] = false;
	}

	switch (TitleSate)
	{
	case TS_STRAT:
		TitleCurPos = 580;
		break;
	case TS_EXIT:
		TitleCurPos = 665;
		break;
	}

	if (SceneChangeFlg == true)
		SceneMove(GS_SELECT, 0.01f, 90.0f, D3DXVECTOR3(0.0f, 0.0f, 0.0f), S_GAMEMUSIC, BGM_TITLE);

	if (SpriteColor.w < 1.0f && SceneChangeFlg == false)
	{
		SpriteColor.w += 0.2f;
	}
	if (SpriteColor.w > 1.0f && SceneChangeFlg == false)
	{
		SpriteColor.w = 1.0f;
	}

	Flashing.w += AddFlash;
	
	if (Flashing.w > 1.0f || Flashing.w < 0.6)
		AddFlash *= -1;

	m_pTitleSlect->Render(D3DXVECTOR2(768, TitleCurPos), D3DXVECTOR2(2.0f, 2.0f), Flashing, true);

	// タイトル背景表示
	m_pMenuBG[0]->Render(D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2(WINDOW_WIDTH, WINDOW_HEIGHT), SpriteColor, true);
}

// 曲選択用実行ループ
void CGame::SelectRun()
{
	///////////
	// 描画処理
	///////////
	
	if (SpriteColor.w < 1.0f)
	{
		SpriteColor.w += 0.02f;
	}
	else
	{
		SpriteColor.w = 1.0f;
	}

	char ViewFont[50];
	sprintf_s(ViewFont, "%.1f", Hi_SPEED);
	m_pText->Render(ViewFont, 1720, 1030, SpriteColor.w);
	
	bms.GetBmsHeader(title, artist, genre, playlevel, bpm, score);

	m_pText->Render(title, 500, 400 + (int)SelectMixWin, SpriteColor.w);
	m_pText->Render(artist, 500, 500 + (int)SelectMixWin, SpriteColor.w);
	m_pText->Render(genre, 500, 585 + (int)SelectMixWin, SpriteColor.w);
	m_pText->Render(playlevel, 590, 630 + (int)SelectMixWin, SpriteColor.w);
	m_pText->Render(bpm, 860, 630 + (int)SelectMixWin, SpriteColor.w);

	m_pText->Render(score, 770.0f, 750.0f + SelectMixWin, SpriteColor.w);
	
	m_pMusicWindow->Render(D3DXVECTOR2(150, 180 + SelectMixWin), D3DXVECTOR2(1.04f, 1.23f), SpriteColor, true);

	// 以降ダミー
	for (int i = 0; i < 3; i++) { m_pMusicWindow->Render(D3DXVECTOR2(700.0f + (i * 110.0f), 120.0f - (i * 70.0f)), D3DXVECTOR2(0.75f, 0.79f), SpriteColor, true); }
	m_pMusicWindow->Render(D3DXVECTOR2(700, 445), D3DXVECTOR2(0.75f, 0.79f), SpriteColor, true);
	m_pMusicWindow->Render(D3DXVECTOR2(810, 515), D3DXVECTOR2(0.75f, 0.79f), SpriteColor, true);

	m_pMenuBG[1]->Render(D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2(WINDOW_WIDTH, WINDOW_HEIGHT), SpriteColor, true);

	///////////
	// 入力処理
	///////////

	// 楽曲決定
	if (GetKeyState(VK_RETURN) & 0x80) // 「]」キー
	{
		if (!bOnKey[3])
		{
			bOnKey[3] = true;
			SceneChangeFlg = true;
		}
	}
	else
	{
		bOnKey[3] = false;
	}

	// 楽曲選択(押された瞬間次の楽曲のヘッダー情報をロード)
	if (GetKeyState(KYE_UP) & 0x80 || GetKeyState(VK_UP) & 0x80)
	{
		if (!bOnKey[1])
		{
			bOnKey[1] = true;
			SelectCnt++;
			if (SelectCnt > MaxMusicCnt)
				SelectCnt = 0;			// 楽曲セレクトループ
			ds.AddSound(S_GAMEMUSIC, mBFP[SelectCnt].mBmsMusicPath);
			ds.Play(S_GAMEMUSIC);
			ds.Reset(SE_SELECT);
			ds.Play(SE_SELECT, FALSE);
			SelectMixWin = 5.0f;
			BmsHeaderLoad();
		}
	}
	else
	{
		bOnKey[1] = false;
	}

	if (GetKeyState(KYE_DOWN) & 0x80|| GetKeyState(VK_DOWN) & 0x80)
	{
		if (!bOnKey[0])
		{
			bOnKey[0] = true;
			SelectCnt--;
			if (SelectCnt < 0)
				SelectCnt = MaxMusicCnt;// 楽曲セレクトループ
			ds.AddSound(S_GAMEMUSIC, mBFP[SelectCnt].mBmsMusicPath);
			ds.Play(S_GAMEMUSIC);
			ds.Reset(SE_SELECT);
			ds.Play(SE_SELECT, FALSE);
			SelectMixWin = -5.0f;
			BmsHeaderLoad();
		}
	}
	else
	{
		bOnKey[0] = false;
	}

	if (0 <= SelectMixWin)
		SelectMixWin -= 1.0f;
	if (0 >= SelectMixWin)
		SelectMixWin += 1.0f;

	static int count; //ハイスピードキーカウント
	if (count > 0)
		count--; // キーカウントを--

	// スクロールスピード
	if (GetKeyState(VK_RIGHT) & 0x80 && count == 0) // ↑キー
	{
		bOnKey[4] = true;
		Hi_SPEED += 1.0f;
		count = 5;
		if (Hi_SPEED > 100.0f)
			Hi_SPEED = 1.0f;
	}
	else
	{
		bOnKey[4] = false;
	}
	if (GetKeyState(VK_LEFT) & 0x80 && count == 0) // ↓キー
	{
		bOnKey[5] = true;
		Hi_SPEED -= 1.0f;
		count = 5;
		if (Hi_SPEED < 1.0f)
			Hi_SPEED = 100.0f;
	}
	else
	{
		bOnKey[5] = false;
	}

	if (SceneChangeFlg == true)
		SceneMove(GS_DECIDE, 0.02f, 10.0f, D3DXVECTOR3(0.0f, 0.0f, 0.0f), BGM_NONE, S_GAMEMUSIC);

}

// 楽曲決定演出関数
void CGame::MusicDecide()
{
	const int cnt = 240;	// 6秒を基準
	ChangeCnt++;			// 時間をカウント

	m_pSceneChangeTex->Render(D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2(WINDOW_WIDTH, WINDOW_HEIGHT), ChangeSceneColor, true); // フェード画像

	if (ChangeCnt < cnt / 2) // カウントが半分未満ならフェードイン
	{
		ds.Play(BGM_DECIDE, FALSE); // BGM再生
		ChangeSceneColor.w += 0.04f; // フェードアウトの速さ
	}
	if (ChangeCnt > cnt / 2) // カウントが半分を超えたらフェードアウト
	{
		// 決定した楽曲のヘッダー表示
		m_pText->Render(title, 490, 450, SpriteColor.w);
		m_pText->Render(artist, 490, 490, SpriteColor.w);
		m_pText->Render(genre, 750, 700, SpriteColor.w);
		m_pText->Render(playlevel, 860, 750, SpriteColor.w);
		m_pText->Render(bpm, 1100, 750, SpriteColor.w);

		m_pMenuBG[3]->Render(D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2(WINDOW_WIDTH, WINDOW_HEIGHT), SpriteColor, true); // 背景、楽曲表示

		ChangeSceneColor.w -= 0.04f; // フェードインの速さ
	}

	if (ChangeSceneColor.w < 0.0f)
		ChangeSceneColor.w = 0.0f;
	if (ChangeSceneColor.w > 1.0f)
		ChangeSceneColor.w = 1.0f;
	
	if (cnt < ChangeCnt)
	{
		BmsFileLoad();
	}

}

void CGame::Ready()
{
	const int cnt = 300;	// 4秒を基準
	ChangeCnt++;			// 時間をカウント
	MainGameRun();

	// チュートリアル画像表示
	m_pTutorial->Render(D3DXVECTOR2(735, 590), D3DXVECTOR2(0.87f, 1.6f), SpriteColor, true);

	// 小節バーリセット
	ZeroMemory(&iStartNum, sizeof(iStartNum));
	bms.Restart();

	if (cnt < ChangeCnt)
	{
		ChangeCnt = 0;
		
		// エフェクト再生
		Effect::getInstance().Effect_Play("READY", D3DXVECTOR3(0.0f, -0.3f, 0.0f)); // エフェクト再生

		// マシンの周波数を取得
		LARGE_INTEGER freq;
		QueryPerformanceFrequency(&freq);
		llGlobalFreq = freq.QuadPart;

		// 現在の時間を開始時間とする
		LARGE_INTEGER li;
		QueryPerformanceCounter(&li);
		llStartTime = li.QuadPart;

		ds.Stop(S_GAMEMUSIC);
		ds.Reset(S_GAMEMUSIC);
		
		// スコアの初期化
		PERFECT = 0;
		GREAT = 0;
		GOOD = 0;
		BAD = 0;
		Combo = 0;
		MaxCombo = 0;
		now_count = 0.0;
		GameStartFlg = true;
		SceneChangeFlg = true;
		OnePlayBGM = true;

		ResultEffectFlg = true;

		SceneMove(GS_MAINGAME, 0.0f, 0.0f, D3DXVECTOR3(0.0f, 0.0f, 0.0f), BGM_NONE, BGM_NONE);
	}
}

// メインゲーム終了演出
void CGame::Finish()
{
	const int cnt = 120;	// 2秒を基準
	ChangeCnt++;

	if (ResultEffectFlg == true)
	{
		if (Gage_y > -0.7f)
			Effect::getInstance().Effect_Play("FAILED_RESULT", D3DXVECTOR3(0.0f, 1.7f, 0.0f)); // エフェクト再生
		else
		{
			if(BAD == 0)
				Effect::getInstance().Effect_Play("PERFECT_RESULT", D3DXVECTOR3(0.0f, 1.5f, 0.0f)); // エフェクト再生
			else
				Effect::getInstance().Effect_Play("CLEAR_RESULT", D3DXVECTOR3(0.0f, 1.7f, 0.0f)); // エフェクト再生
		}
	}

	ResultEffectFlg = false;

	if (SpriteColor.w < 1.0f)
	{
		SpriteColor.w += 0.02f;
	}
	else
	{
		SpriteColor.w = 1.0f;
	}

	if (cnt < ChangeCnt)
	{
		SceneMove(GS_RESULT, 0.05f, 300.0f, D3DXVECTOR3(0.0f, 0.0f, 0.0f), BGM_RESULT, S_GAMEMUSIC);
	}
}

// メインゲーム用実行ループ
void CGame::MainGameRun()
{
	static const int index[4] = { 0,1,2,3 };								// インデックスリスト
	static const float obj_x[4] = { -1.03f,-0.34f,0.34f,1.03f };			// ノーツの表示X座標
	static const float efc_x[4] = { -0.65f,-0.22f,0.22f,0.65f };			// エフェクトの表示X座標
	static int cnt[4];														// ノーツを消した瞬間表示するプッシュラインのカウント
	static float JudgeMarkSize[4];											// ジャッジマークのサイズ
	static const D3DXVECTOR3 JudgeEffectPos(0.0f, 1.0f, 1.0f);				// 判定文字の位置
	float One_Beat = now_count % 2400;								// 一拍の数値
	
	static const int GoodAddScore = 600;
	static const int GreatAddScore = 1400;
	static const int PerfectAddScore = 2000;
	static const int ExAddScore = 3000;

	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	dElapsedTime = (double)(li.QuadPart - llStartTime) / llGlobalFreq;

	// 経過した時間から進んだBMSカウント値を算出
	now_count = bms.GetCountFromTime(dElapsedTime);

	// 楽曲を0小節目を超えた時点で再生する
	if (now_count > BMS_RESOLUTION && OnePlayBGM == true)
	{
		OnePlayBGM = false;
		ds.Play(S_GAMEMUSIC, FALSE);
	}

	if (GameStartFlg == false)
	{
		// ゲームで使用する数値の初期化
		PERFECT = 0;
		GREAT = 0;
		GOOD = 0;
		BAD = 0;
		Combo = 0;
		MaxCombo = 0;
		now_count = 0.0;
	}

	// Rキーを押したらセレクトへ
	if(GetKeyState('R') & 0x80)
		SceneChangeFlg = true;

	if (SceneChangeFlg == true)
	{
		SceneMove(GS_SELECT, 0.05f, 30.0f, D3DXVECTOR3(0.0f, 0.0f, 0.0f), S_GAMEMUSIC, S_GAMEMUSIC);
	}

	// BMSカウンタが曲の最大カウント-4小節を超えたらリザルトへ
	if (bms.GetMaxCount() - BMS_RESOLUTION * 4 <= now_count)
	{
		// 小節バーリセット
		ZeroMemory(&iStartNum, sizeof(iStartNum));
		bms.Restart();

		now_count = 0.0; // カウントストップ
		Finish();		 // ゲーム終了演出関数
	}

	if (SpriteColor.w < 1.0f)
	{
		SpriteColor.w += 0.02f;
	}
	

	///////////
	// 入力処理
	///////////

	static int count; //ハイスピードキーカウント
	if (count > 0)
		count--; // キーカウントを--

	// スクロールスピード
	if (GetKeyState(VK_RIGHT) & 0x80 && count == 0) // ↑キー
	{
		bOnKey[4] = true;
		Hi_SPEED += 1.0f;
		count = 5;
		if (Hi_SPEED > 100.0f)
			Hi_SPEED = 1.0f;
	}
	else
	{
		bOnKey[4] = false;
	}
	if (GetKeyState(VK_LEFT) & 0x80 && count == 0) // ↓キー
	{
		bOnKey[5] = true;
		Hi_SPEED -= 1.0f;
		count = 5;
		if (Hi_SPEED < 1.0f)
			Hi_SPEED = 100.0f;
	}
	else
	{
		bOnKey[5] = false;
	}

	// 仮想入力ハードウェア(押された瞬間だけTRUEとなる配列)
	BOOL press[4]; // 4Key
	ZeroMemory(&press, sizeof(press));
	// 仮想入力ハードウェア(押された間TRUEとなる配列)
	BOOL keep[4];	// 4Key
	ZeroMemory(&keep, sizeof(keep));

	// 鍵盤の処理
	static const int KEYID[4] =
	{	// キーのリスト
		KYE_DOWN,			// ch11に割り当てるキー
		KYE_UP,				// ch12に割り当てるキー
		KYE_CANCEL,			// ch13に割り当てるキー
		KYE_DECIDE,			// ch14に割り当てるキー
	};

	for (int i = 0; i < 4; i++)
	{
		if ((GetKeyState(KEYID[i]) & 0x80))
		{
			// キーボードの入力があった場合
			if (!bOnKey[i])
			{
				// まだ押されていなければ押された瞬間とする
				press[i] = TRUE;
				bOnKey[i] = TRUE;
			}
			// キーを押してる間とする
			bOnHold[i] = TRUE;
			keep[i] = TRUE;
			JudgeMarkSize[i] = 1.4f;
		}
		else
		{
			// 押されていなければフラグをリセット
			bOnKey[i] = FALSE;
			bOnHold[i] = FALSE;

			// ジャッジマークをテンポに合わせて変化
			if (One_Beat > 0 && One_Beat < 300)
				JudgeMarkSize[i] = 1.0f;
			else
				JudgeMarkSize[i] = 1.1f;
			// デバッグ、オート
			/*press[i] = true;
			keep[i] = true;*/
		}
	}

	


	for (int i = 0; i < 4; i++)
	{
		// ホールドノートの途中で離したらSEを消す
		if (bOnHold[i] == FALSE)
		{
			Effect::getInstance().Effect_Stop("HoldNote" + i); // エフェクト停止
			ds.Stop(SE_HOLDNOTE + i);
			ds.Reset(SE_HOLDNOTE + i);
		}
	}

	// 入力判定
	const LONG HOLD_RANGE		= BMS_RESOLUTION / 64;		// ホールド用判定する中心からの範囲(前後合わせて32分音符内)
	const LONG PERFECT_RANGE	= BMS_RESOLUTION / 36;		// PERFECTと判定する中心からの範囲(前後合わせて18分音符内)
	const LONG GREAT_RANGE		= BMS_RESOLUTION / 16;		// GREATと判定する中心からの範囲(前後合わせて8分音符内)
	const LONG GOOD_RANGE		= BMS_RESOLUTION / 12;		// GOODと判定する中心からの範囲(前後合わせて6分音符内)

	for (int j = 0; j < 4; j++)
	{
		// 判定対象のチャンネルのノートをチェック
		for (int i = iStartNum[j + 0x11 + 0x20]; i < bms.GetObjeNum(0x11 + j); i++)
		{
			LPBMSDATA Note = (LPBMSDATA)bms.GetObje(0x11 + j, i);
			if (Note->bFlag)
			{
				// まだ未判定のノートなら
				if (Note->lTime < (now_count - GOOD_RANGE))
				{
					// 良判定を過ぎたら全て見逃し扱いとする
					Note->bFlag = FALSE; // オブジェを消す

					//// ホールドノートの途中で離したらSEを消す
					//if (Note->iType == HoldFinish && Note->bFlag == FALSE)
					//{
					//	Effect::getInstance().Effect_Stop("HoldNote" + j); // エフェクト停止
					//	ds.Stop(SE_HOLDNOTE);
					//	ds.Reset(SE_HOLDNOTE);
					//}
					Effect::getInstance().Effect_Play("BAD", D3DXVECTOR3(efc_x[j], 0.5f, -0.1f)); // エフェクト再生
					Effect::getInstance().Effect_Play("BAD_Font", JudgeEffectPos); // エフェクト再生

					BAD++;
					Gage_y += AddGage_y;
					Combo = 0;
					// 判定オブジェをその次からに変更
					iStartNum[j + 0x11 + 0x20] = i + 1;
					// 次のオブジェをチェック
					continue;
				}

				// ノートが判定外なら抜ける
				if ((now_count + GOOD_RANGE) <= Note->lTime)
					break;

				// ノートが判定内ならキーが押された瞬間かをチェック
				if (press[j])
				{
					// キーを押した瞬間なら精度判定
					LONG sub = abs(now_count - Note->lTime);	// ノートとの差を絶対値で取得
					int jadge = 0;								// 判定値(0=POOR、1=BAD、2=GOOD、3=GREAT、4=PKGREATなど)
					
					if (sub <= PERFECT_RANGE)
					{
						jadge = 3;
						PERFECT++;
						Gage_y -= AddGage_y * 1.5f;
						Combo++;
						if (Combo > MaxCombo)
							MaxCombo++;
						if (Note->iType == Extra)
						{
							Max_Score += ExAddScore;
							Effect::getInstance().Effect_Play("ExNote", D3DXVECTOR3(efc_x[j], 0.5f, -0.1f)); // エフェクト再生
							Effect::getInstance().Effect_Play("PERFECT_Font", JudgeEffectPos); // エフェクト再生
							ds.Reset(SE_EXNOTE);
							ds.Play(SE_EXNOTE);
						}
						if (Note->iType == Normal)
						{
							Max_Score += GreatAddScore;
							Effect::getInstance().Effect_Play("PERFECT", D3DXVECTOR3(efc_x[j], 0.5f, -0.1f)); // エフェクト再生
							Effect::getInstance().Effect_Play("PERFECT_Font", JudgeEffectPos); // エフェクト再生
							ds.Reset(SE_GREAT);
							ds.Play(SE_GREAT);
						}
						if (Note->iType == HoldStart)
						{
							Max_Score += GreatAddScore;
							Effect::getInstance().Effect_Play("HoldNote" + j, D3DXVECTOR3(efc_x[j], 0.5f, -0.1f)); // エフェクト再生
							Effect::getInstance().Effect_Play("PERFECT_Font", JudgeEffectPos); // エフェクト再生
							ds.Reset(SE_HOLDNOTE + j);
							ds.Play(SE_HOLDNOTE + j, TRUE);
						}
						cnt[j] = 10;
					}
					else if (sub <= GREAT_RANGE)
					{
						jadge = 2;
						Gage_y -= AddGage_y * 0.8;
						Combo++;
						if (Combo > MaxCombo)
							MaxCombo++;
						if (Note->iType == Extra)
						{
							PERFECT++;
							Max_Score += ExAddScore;
							Effect::getInstance().Effect_Play("ExNote", D3DXVECTOR3(efc_x[j], 0.5f, -0.1f)); // エフェクト再生
							Effect::getInstance().Effect_Play("PERFECT_Font", JudgeEffectPos); // エフェクト再生
							ds.Reset(SE_EXNOTE);
							ds.Play(SE_EXNOTE);
						}
						if (Note->iType == Normal)
						{
							GREAT++;
							Max_Score += GreatAddScore;
							Effect::getInstance().Effect_Play("GREAT", D3DXVECTOR3(efc_x[j], 0.5f, -0.1f)); // エフェクト再生
							Effect::getInstance().Effect_Play("GREAT_Font", JudgeEffectPos); // エフェクト再生
							ds.Reset(SE_GREAT);
							ds.Play(SE_GREAT);
						}
						if (Note->iType == HoldStart)
						{
							GREAT++;
							Max_Score += GreatAddScore;
							Effect::getInstance().Effect_Play("HoldNote" + j, D3DXVECTOR3(efc_x[j], 0.5f, -0.1f)); // エフェクト再生
							Effect::getInstance().Effect_Play("PERFECT_Font", JudgeEffectPos); // エフェクト再生
							ds.Reset(SE_HOLDNOTE + j);
							ds.Play(SE_HOLDNOTE + j, TRUE);
						}
						cnt[j] = 10;
					}
					else if (sub <= GOOD_RANGE)
					{
						jadge = 1;
						Gage_y -= AddGage_y * 0.5f;
						Combo++;
						if (Combo > MaxCombo)
							MaxCombo++;
						if (Note->iType == Extra)
						{
							PERFECT++;
							Max_Score += ExAddScore;
							Effect::getInstance().Effect_Play("ExNote", D3DXVECTOR3(efc_x[j], 0.5f, -0.1f)); // エフェクト再生
							Effect::getInstance().Effect_Play("PERFECT_Font", JudgeEffectPos); // エフェクト再生
							ds.Reset(SE_EXNOTE);
							ds.Play(SE_EXNOTE);
						}
						if (Note->iType == Normal)
						{
							GOOD++;
							Max_Score += GoodAddScore;
							Effect::getInstance().Effect_Play("GOOD", D3DXVECTOR3(efc_x[j], 0.5f, -0.1f)); // エフェクト再生
							Effect::getInstance().Effect_Play("GOOD_Font", JudgeEffectPos); // エフェクト再生
							ds.Reset(SE_GOOD);
							ds.Play(SE_GOOD);
						}
						if (Note->iType == HoldStart)
						{
							GOOD++;
							Max_Score += GoodAddScore;
							Effect::getInstance().Effect_Play("HoldNote" + j, D3DXVECTOR3(efc_x[j], 0.5f, -0.1f)); // エフェクト再生
							Effect::getInstance().Effect_Play("PERFECT_Font", JudgeEffectPos); // エフェクト再生
							ds.Reset(SE_HOLDNOTE + j);
							ds.Play(SE_HOLDNOTE + j, TRUE);
						}
						cnt[j] = 10;
					}

					if (jadge >= 1) // 判定のノーツ処理
					{
						// GOOD以上ならオブジェを処理
						Note->bFlag = FALSE;	// ノーツを消す

						// 判定ノーツをその次からに変更
						iStartNum[j + 0x11 + 0x20] = i + 1;
						break;
					}
				}

				// ホールドノートが判定内ならキーが押してる間かをチェック
				if (keep[j] && Note->iType == HoldFinish)
				{
					LONG sub = abs(now_count - Note->lTime);	// ノートとの差を絶対値で取得
					bool jadge = false;							// 押してる間に判定値に入ったか
					if (sub <= HOLD_RANGE)
					{
						jadge = true;
						PERFECT++;
						Combo++;
						Max_Score += PerfectAddScore;
						if (Combo > MaxCombo)
							MaxCombo++;
						if (Note->iType == HoldFinish && bOnHold[j])
						{
							Effect::getInstance().Effect_Stop("HoldNote" + j); // エフェクト停止
							Effect::getInstance().Effect_Play("PERFECT", D3DXVECTOR3(efc_x[j], 0.5f, -0.1f)); // エフェクト再生
							Effect::getInstance().Effect_Play("PERFECT_Font", JudgeEffectPos); // エフェクト再生
							ds.Reset(SE_EXNOTE);
							ds.Play(SE_EXNOTE);

							ds.Stop(SE_HOLDNOTE + j);
						}
					}
					if (jadge == true) // 判定のノーツ処理
					{
						// GOOD以上ならオブジェを処理
						Note->bFlag = FALSE; // ノーツを消す

						// 判定ノーツをその次からに変更
						iStartNum[j + 0x11 + 0x20] = i + 1;
						break;
					}
				}
			}
		}
	}

	///////////
	// 描画処理
	///////////

	// フォント表示
	char ViewFont[50];
	bms.GetBmsHeader(title, artist, genre, playlevel, bpm, score); // 楽曲情報取得
	// 楽曲情報表示
	m_pTextSizeS->Render(title, 1220, 110, SpriteColor.w);
	m_pTextSizeS->Render(playlevel, 1495, 182, SpriteColor.w);
	m_pTextSizeS->Render(bpm, 1630, 182, SpriteColor.w);

	// スクロールスピード表示
	sprintf_s(ViewFont, "%.1f", Hi_SPEED);
	m_pTextSizeS->Render(ViewFont, 1600, 230, SpriteColor.w);

	// 判定表示
	sprintf_s(ViewFont, "%03d", MaxCombo);
	m_pText->Render(ViewFont, 380, 485, SpriteColor.w);	// MAXCOMBO
	sprintf_s(ViewFont, "%03d", PERFECT);
	m_pText->Render(ViewFont, 380, 545, SpriteColor.w);	// PERFECT
	sprintf_s(ViewFont, "%03d", GREAT);
	m_pText->Render(ViewFont, 380, 590, SpriteColor.w);	// GREAT
	sprintf_s(ViewFont, "%03d", GOOD);
	m_pText->Render(ViewFont, 380, 635, SpriteColor.w);	// GOOD
	sprintf_s(ViewFont, "%03d", BAD);
	m_pText->Render(ViewFont, 380, 680, SpriteColor.w);	// BAD

	m_pText->Render("COMBO", 860, 480, SpriteColor.w);
	sprintf_s(ViewFont, "%04d", Combo);
	m_pText->Render(ViewFont, 880, 450, SpriteColor.w);	// COMBO	

	m_pText->Render("SCORE", 90, 240, SpriteColor.w);
	sprintf_s(ViewFont, "%07d", Now_Score);
	m_pText->Render(ViewFont, 120, 300, SpriteColor.w);	// スコア(999万9999まで)

	Now_Score += 1111;

	if (Now_Score > Max_Score)
		Now_Score = Max_Score;

	// UI表示
	m_pMainGameUI[MGUI_GAGE]->Render(D3DXVECTOR2(1560.0f, 330.0f), D3DXVECTOR2(1.5f, 1.5f), SpriteColor, true);
	m_pMainGameUI[MGUI_HEADER]->Render(D3DXVECTOR2(780.0f, 40.0f), D3DXVECTOR2(2.1f, 2.0f), SpriteColor, true);
	m_pMainGameUI[MGUI_SCORE]->Render(D3DXVECTOR2(40.0f, 200.0f), D3DXVECTOR2(2.0f, 2.1f), SpriteColor, true);
	m_pMainGameUI[MGUI_JUDGE]->Render(D3DXVECTOR2(40.0f, 400.0f), D3DXVECTOR2(2.1f, 2.1f), SpriteColor, true);
	m_pMainGameUI[MGUI_FRAME]->Render(D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2(WINDOW_WIDTH, WINDOW_HEIGHT), SpriteColor, true);

	if(Gage_y > -0.7f)
		m_pGageType[0]->Render(D3DXVECTOR2(1560.0f, 930.0f), D3DXVECTOR2(1.125f, Gage_y), SpriteColor, true);
	else
		m_pGageType[1]->Render(D3DXVECTOR2(1560.0f, 930.0f), D3DXVECTOR2(1.125f, Gage_y), SpriteColor, true);


	if (Gage_y < -1.0f)
		Gage_y = -1.0f;
	if (Gage_y > 0.0f)
		Gage_y = 0.0f;

	/////////
	// モデル
	/////////

	// レーンモデル
	m_pLane->Render(D3DXVECTOR3(0.0f, -0.05f, 0.0f), D3DXVECTOR3(0, 0, 0), D3DXVECTOR3(1, 1, 1));
	
	for (int i = 0; i < 8; i++)
	{
		if (i < 4)
		{
			// ジャッジマークモデル
			m_pJudgeMark[i]->Render(D3DXVECTOR3(obj_x[i], 0.02f, 0.0f), D3DXVECTOR3(0, 0, 0), D3DXVECTOR3(JudgeMarkSize[i], 1, JudgeMarkSize[i]));

			// ラインモデル
			m_pLine[i]->Render(D3DXVECTOR3(obj_x[i], 0.0f, 0.0f), D3DXVECTOR3(0, 0, 0), D3DXVECTOR3(1, 1, 1));
		}
		if (3 < i && 0 < cnt[i - 4])
		{
			// ラインモデル
			m_pLine[i]->Render(D3DXVECTOR3(obj_x[i - 4], 0.01f, 0.0f), D3DXVECTOR3(0, 0, 0), D3DXVECTOR3(1, 1, 1));
			cnt[i - 4]--;
		}
	}

	// ノーツ
	for (int j = 0; j < 4; j++)
	{
		for (int i = iStartNum[0x11 + index[j]]; i < bms.GetObjeNum(0x11 + index[j]); i++)
		{
			LPBMSDATA Note = bms.GetObje(0x11 + index[j], i);
			float off_z = (float)((double)(Note->lTime - now_count) / BMS_RESOLUTION * (Hi_SPEED * 10));
			float scal_z = (float)(Note->fScal * Hi_SPEED / 4.0);
			// 判定ラインより下ならもう表示はせず、次回からその次のオブジェから参照する
			if (off_z < -21.0f || Note->bFlag == FALSE)
			{
				iStartNum[index[j] + 0x11] = i + 1;
				continue;
			}
			// 画面の上より外ならばその先は全て描画スキップ
			if (off_z > 80 + (float)Note->fScal * Hi_SPEED / 4.0f * 2.5f)
				break;
			// 画面内なら描画
			if (Note->iType == Extra)
			{
				m_pExNote->Render(D3DXVECTOR3(obj_x[j], 0.0f, off_z), D3DXVECTOR3(0, 0, 0), D3DXVECTOR3(1, 1, 1));
			}
			else if (Note->iType == HoldFinish)
			{
				float Hold_pos = (off_z - (float)(Note->fScal * Hi_SPEED / 4.0f * 2.5f));
				if (Hold_pos < 0)
				{
					scal_z = off_z / 2.5f;
					if (scal_z < 0)
					{
						// ホールドノートのスケールが0以下ならばSEを止める
						ds.Stop(SE_HOLDNOTE + j);

						scal_z = 0;
					}
				}
				m_pHoldLongNote->Render(D3DXVECTOR3(obj_x[j], 0.0f, off_z), D3DXVECTOR3(0, 0, 0), D3DXVECTOR3(1, 1, scal_z));
			}
			else if (Note->iType == Normal)
			{
				m_pNote->Render(D3DXVECTOR3(obj_x[j], 0.0f, off_z), D3DXVECTOR3(0, 0, 0), D3DXVECTOR3(1, 1, 1));
			}
		}
	}

	
	// ドーム
	for (int i = iStartNum[0]; i < bms.GetBarNum(); i++)
	{
		LPBMSBAR bar = bms.GetBar(i);
		float off_z = (float)((double)(bar->lTime - now_count) / BMS_RESOLUTION * 30);
		// 判定ラインより下ならもう表示はせず、次回からその次の小節から参照する
		if (off_z < 0.0f)
		{
			iStartNum[0] = i + 1;
			continue;
		}
		// 画面の上より外ならばその先は全て描画スキップ
		if (off_z > 200)
			break;
		// 画面内なら描画
		if (Gage_y > -0.7f)
			m_pDome->Render(D3DXVECTOR3(0.0f, 1.8f, off_z), D3DXVECTOR3(0, 0, 0), D3DXVECTOR3(1.5, 1.5, 1.5));
		else
			m_pDomeClear->Render(D3DXVECTOR3(0.0f, 1.8f, off_z), D3DXVECTOR3(0, 0, 0), D3DXVECTOR3(1.5, 1.5, 1.5));

	}

	// 小節バー
	for (int i = iStartNum[0]; i < bms.GetBarNum(); i++)
	{
		LPBMSBAR bar = bms.GetBar(i);
		float off_z = (float)((double)(bar->lTime - now_count) / BMS_RESOLUTION * (Hi_SPEED * 10));
		// 判定ラインより下ならもう表示はせず、次回からその次の小節から参照する
		if (off_z < -0.0f)
		{
			iStartNum[0] = i + 1;
			continue;
		}
		// 画面の上より外ならばその先は全て描画スキップ
		if (off_z > 40 + 2)
			break;
		// 画面内なら描画
		m_pBar->Render(D3DXVECTOR3(0, 0, off_z), D3DXVECTOR3(0, 0, 0), D3DXVECTOR3(1.0f, 1.0f, 5.0f));
	}

	// エフェクト
	Effect::getInstance().Draw(); // エフェクト描画
}

// リザルト用実行ループ
void CGame::ResultRun()
{
	// DEBUG("リザルトステート実行中\n");

	if (SceneChangeFlg == false)
	{
		if (SpriteColor.w < 1.0f)
		{
			SpriteColor.w += 0.02f;
		}
		else
		{
			SpriteColor.w = 1.0f;
		}
	}
	

	// 曲選択シーンへ戻る
	if (GetKeyState(KYE_DECIDE) & 0x80 || GetKeyState(VK_RETURN) & 0x80)
	{
		if (!bOnKey[3])
		{
			bOnKey[3] = true;
			SceneChangeFlg = true;
			//bms.Save(mBFP[SelectCnt].mBmsFilePath, Now_Score);// スコアを記録
			BmsFileClear();		// BMSファイル消去
			BmsHeaderLoad();	// 最初の楽曲のヘッダー情報をロード
		}
	}
	else
	{
		bOnKey[3] = false;
	}
	
	if (SceneChangeFlg == true)
		SceneMove(GS_SELECT, 0.01f, 40.0f, D3DXVECTOR3(0.0f, 0.0f, 0.0f), S_GAMEMUSIC, BGM_RESULT);


	// フォント表示
	char ViewFont[50];

	m_pText->Render(title, 250, 148, SpriteColor.w);
	m_pText->Render(artist, 250, 187, SpriteColor.w);

	m_pText->Render("SCORE", 980, 450, SpriteColor.w);
	sprintf_s(ViewFont, "%07d", Now_Score);
	m_pText->Render(ViewFont, 1040, 500, SpriteColor.w); // スコア(999万9999まで)

	sprintf_s(ViewFont, "MAXCOMBO:%d", MaxCombo);
	m_pText->Render(ViewFont, 940, 670, SpriteColor.w);	// MAXCOMBO

	sprintf_s(ViewFont, "PERFECT :%d", PERFECT);
	m_pText->Render(ViewFont, 940, 720, SpriteColor.w);
	sprintf_s(ViewFont, "GREAT   :%d", GREAT);
	m_pText->Render(ViewFont, 940, 760, SpriteColor.w);
	sprintf_s(ViewFont, "GOOD    :%d", GOOD);
	m_pText->Render(ViewFont, 940, 800, SpriteColor.w);
	sprintf_s(ViewFont, "BAD     :%d", BAD);
	m_pText->Render(ViewFont, 940, 840, SpriteColor.w);

	// 画像表示
	if (Gage_y > -0.7f)		// Failed表示
		m_pResult[1]->Render(D3DXVECTOR2(240, 300), D3DXVECTOR2(2.0f, 1.5f), SpriteColor, true);
	else					// Clear表示
		m_pResult[0]->Render(D3DXVECTOR2(230, 285), D3DXVECTOR2(2.0f, 1.5f), SpriteColor, true);

	// 背景表示
	m_pMenuBG[2]->Render(D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2(WINDOW_WIDTH, WINDOW_HEIGHT), SpriteColor, true);

}

// BMSファイルロード関数
void CGame::BmsFileLoad()
{
	//DEBUG("BMSファイルロード\n");
	// ファイルロード関数
	if (!bms.Load(mBFP[SelectCnt].mBmsFilePath))
	{
		DEBUG("BMSFile Load Error\n");
	}
	
	// ゲージの最大値を取得
	MaxGage = bms.GetMaxNoteCnt() * 3; // BAD-1,GOOD+1,GREAT+1,PERFECT+3,とするので最大 * 3とする
	AddGage_y = (float)1 / MaxGage * 3;
	Gage_y = 0.0f; // ゲージの初期化
	Now_Score = 0; // スコアの初期化
	Max_Score = 0; // スコアの初期化
	GameStartFlg = false;
	SceneMove(GS_READY, 0.05f, 300.0f, D3DXVECTOR3(0.0f, 0.0f, 0.0f), BGM_NONE, S_GAMEMUSIC);
}

// BMSファイル消去関数
void CGame::BmsFileClear()
{
	DEBUG("BMSファイル消去\n");
	// BMSファイル消去関数
	if (!bms.Clear())
	{
		DEBUG("BMSFile Clear Error\n");
	}
	//
}

// BMSファイルのヘッダー情報のみロード
void CGame::BmsHeaderLoad()
{
	//DEBUG("BMSファイルヘッダー情報ロード\n");
	// BMSファイルヘッダー情報ロード
	bms.LoadHeader(mBFP[SelectCnt].mBmsFilePath);
}

// BMSファイルのヘッダー情報のみ消去
void CGame::BmsHeaderClear()
{
	DEBUG("BMSファイルヘッダー情報消去\n");
	// BMSファイルヘッダー情報消去
	//
}

// BMSファイルパス取得
void CGame::GetBMSFilePath()
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	WCHAR* FilePath;
	char BmsName[100]; // 「****.bms」代入用変数
	size_t wLen = 0;
	int cnt = 0; // 繰り返しカウント 

	hFind = FindFirstFile(L"BMSFile/*.bms", &FindFileData);

	while (true)
	{
		if (hFind == INVALID_HANDLE_VALUE)
		{
			return;
		}
		else
		{
			strcpy_s(mBFP[cnt].mBmsFilePath, "BMSFile/"); // パス代入

			FilePath = FindFileData.cFileName;
			// setlocale(LC_ALL, "japanese");
			wcstombs_s(&wLen, BmsName, 100, FilePath, _TRUNCATE); // WCHARからcharへ変換処理
			strcat_s(mBFP[cnt].mBmsFilePath, BmsName); //「BMSFile/」と「****.bms」の文字列結合

			// 楽曲データのファイルパス代入
			
			for (int i = 0; i < 256; i++)
			{
				// 「.」か「_」ならば抜ける
				if (mBFP[cnt].mBmsFilePath[i] == '.'|| mBFP[cnt].mBmsFilePath[i] == '_')
				{
					strcat_s(mBFP[cnt].mBmsMusicPath, ".wav");
					break;
				}
				// 楽曲用ファイルパスをコピー
				mBFP[cnt].mBmsMusicPath[i] = mBFP[cnt].mBmsFilePath[i];
			}

			// 次の****.bmsファイルを読み込む
			if (!FindNextFile(hFind, &FindFileData))
			{
				FindClose(hFind);
				MaxMusicCnt = cnt; // 最大楽曲数を代入
				SelectCnt = 0;
				// 最初に流す楽曲のパスを代入
				ds.AddSound(S_GAMEMUSIC, mBFP[SelectCnt].mBmsMusicPath);
				return;
			}
			cnt++;
		}
	}
}

// ロード済みデータ全開放
void CGame::AllClear()
{
	DEBUG("ロード済みデータ全消去\n");
	ds.Clear();
}

// シーン移動演習関数
void CGame::SceneMove(GAMESTATE NextScene, float FadeTime, float StartMoveTime, D3DXVECTOR3 FadeColor, SOUND Play, SOUND Stop)
{
	SpriteColor.w -= FadeTime; // 透明
	ChangeCnt++;

	if (StartMoveTime < ChangeCnt)
	{
		ds.Stop(Stop);
		ds.Reset(Stop);
		ds.Play(Play ,TRUE);
		GameState = NextScene;
		SceneChangeFlg = false;
		ChangeCnt = 0;
	}
}

////////////////////////////////////////////////
//メッセージループとアプリケーション処理の入り口
////////////////////////////////////////////////
void CGame::Run()
{
	// ゲームメインループ
	MSG msg = { 0 };
	bool bLoop = TRUE;
	ZeroMemory(&msg, sizeof(msg));
	while (bLoop)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				bLoop = FALSE;
				DEBUG("通常終了\n");
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//画面クリア（実際は単色で画面を塗りつぶす処理）
		float ClearColor[4] = { 0,0,0,1 };// クリア色作成　RGBAの順
		m_pDeviceContext->ClearRenderTargetView(m_pBackBuffer_TexRTV, ClearColor);//画面クリア
		m_pDeviceContext->ClearDepthStencilView(m_pBackBuffer_DSTexDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);//深度バッファクリア
		
		
		//カメラ処理
		// ３人称視点処理　ビュートランスフォーム カメラをキャラの後ろに配置するだけ																							//カメラ情報
		D3DXVECTOR3 vEyePt(0.0f, 2.5f, -5.0f);		//カメラ（視点）位置
		D3DXVECTOR3 vLookatPt(0.0f, 0.5f, 3.0f);	//注視位置
		D3DXVECTOR3 vUpVec(0.0f, 0.0f, 1.5f);		//上方位置
		D3DXMatrixLookAtLH(&mView, &vEyePt, &vLookatPt, &vUpVec);
		// プロジェクショントランスフォーム（射影変換）
		D3DXMatrixPerspectiveFovLH(&mProj, D3DX_PI / 4, (FLOAT)WINDOW_WIDTH / (FLOAT)WINDOW_HEIGHT, 0.1f, 1000.0f);

		switch (GameState)
		{
		case GS_INITLOAD:
			// 初期化&ロード
			InitLoad();
			break;

		case GS_TITLE:
			// タイトル
			TitleRun();
			break;

		case GS_SELECT:
			// 曲選択
			SelectRun();
			break;

		case GS_MAINGAME:
			// メインゲーム
			MainGameRun();
			break;

		case GS_DECIDE:
			// 楽曲決定演出
			MusicDecide();
			break;

		case GS_READY:
			// ゲームスタート演出
			Ready();
			break;

		case GS_FINISH:
			// ゲーム終わり演出
			Finish();
			break;

		case GS_RESULT:
			// リザルト
			ResultRun();
			break;

		case GS_END:
			// 全てのゲーム処理
			bLoop = FALSE;
			DEBUG("通常終了\n");
			break;

		default:
			// 未定義のゲームステート
			bLoop = FALSE;
			DEBUG("異常終了\n");
			break;
		}
		// Sleep(0); // 処理停止

		// カメラ
		CD3DXMESH_ANIM::SetCamera(mView, mProj, D3DXVECTOR3(0.0f, 0.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 1.5f));
		D3D11_SPRITE::SetCamera(mView, mProj);
		TD_Graphics::SetCamera(mView, mProj);

		// エフェクトの注視点
		Effect::getInstance().SetCamera(vEyePt, vLookatPt);

		// エフェクト描画更新
		Effect::getInstance().Update("GREAT");

		// フルスクリーン
		/*static bool fullsc = true;
		if (fullsc)
		{
			BOOL	f;
			m_pSwapChain->GetFullscreenState(&f, 0);
			if (!f)
				m_pSwapChain->SetFullscreenState(true, 0);
			fullsc = false;
		}*/

		// 画面更新（バックバッファをフロントバッファに）
		m_pSwapChain->Present(1, 0);
	}
	// アプリケーションの終了
}