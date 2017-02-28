#include "CGame.h"
#include <stdio.h>

#define DEBUGMODE 
#include "DEBUG/DEBUG.H"

D3DXMATRIX CGame::mView;
D3DXMATRIX CGame::mProj;

//�R���X�g���N�^
CGame::CGame()
{
	GameState = GS_INITLOAD; // ������&���[�h���X�e�[�g�̍ŏ��Ƃ���
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

//�f�X�g���N�^
CGame::~CGame()
{
	AllClear();
}

// ������&���[�h�֐�(��x�̂�)
void CGame::InitLoad()
{
	DEBUG("������&���[�h!!\n");
	ZeroMemory(&bOnKey, sizeof(bOnKey));
	ZeroMemory(&bOnHold, sizeof(bOnHold));

	ShowCursor(FALSE); //�}�E�X�J�[�\����\��

	Hi_SPEED = 4.0f; // �X�N���[���X�s�[�h

	for (int i = 0; i < 4; i++)
	{
		bOnKey[i] = false;
		bOnHold[i] = false;
	}

	//�X�v���C�g
	D3D11_SPRITE::InitShader(m_pDeviceContext, WINDOW_WIDTH, WINDOW_HEIGHT);
	//UI
	TD_Graphics::InitShader(m_pDeviceContext);

	/////////////////////////////
	// �����񃌃��_�����O�̏�����
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
	// �摜���[�h
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
	
	// ���U���g�N���A�A�t�F�C���h�t�H���g
	m_pResult[0]->Init(L"Texture/Clear_Font.png", D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2(256, 128),
		D3DXVECTOR4(1.0, 1.0, 1.0, 1.0), GrapRect(0.0f, 1.0f, 0.0f, 1.0f));
	m_pResult[1]->Init(L"Texture/Failed_Font.png", D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2(256, 128),
		D3DXVECTOR4(1.0, 1.0, 1.0, 1.0), GrapRect(0.0f, 1.0f, 0.0f, 1.0f));

	// ���C���Q�[��UI
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

	// �Q�[�W�̃`���[�W��
	m_pGageType[0]->Init(L"Texture/gage_less.png", D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2(64, 600),
		D3DXVECTOR4(1.0, 1.0, 1.0, 1.0), GrapRect(0.0f, 1.0f, 0.0f, 1.0f));
	m_pGageType[1]->Init(L"Texture/gage_clear.png", D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2(64, 600),
		D3DXVECTOR4(1.0, 1.0, 1.0, 1.0), GrapRect(0.0f, 1.0f, 0.0f, 1.0f));

	// �y�Ȍ��莞�̃V�[�����B���摜
	m_pSceneChangeTex->Init(L"Texture/SceneChange.png", D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2(WINDOW_WIDTH, WINDOW_HEIGHT),
		D3DXVECTOR4(1.0, 1.0, 1.0, 1.0), GrapRect(0.0f, WINDOW_HEIGHT, 0.0f, WINDOW_WIDTH));

	///////////////
	// ���f�����[�h
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

	// ���C�������f��Y���W�̊�Ƃ���
	for (int i = 0; i < 8; i++)
	{
		if (i < 4) // 0�`3��ʏ�̃W���b�W�}�[�N�A���C��
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
		else // 4�`7�������ꂽ��Ԃ̃W���b�W�}�[�N�A���C��
		{
			if (FAILED((m_pLine[i]->Init(m_hWnd, m_pDevice, m_pDeviceContext, "Model/PushLine.x"))))
			{
				return;
			}
		}
	}
	
	// �G�t�F�N�g�̏�����
	Effect::getInstance().EffectInit(m_hWnd, m_pDevice, m_pDeviceContext);

	// �T�E���h���[�h
	ds.AddSound(BGM_TITLE, "Sound/BGM/title.wav");		// �^�C�g��BGM
	ds.AddSound(BGM_DECIDE, "Sound/BGM/decide.wav");	// ����BGM
	ds.AddSound(BGM_RESULT, "Sound/BGM/result.wav");	// ���U���gBGM

	ds.AddSound(SE_START, "Sound/SE/title_enter.wav");	// �^�C�g���X�^�[�gSE
	ds.AddSound(SE_SELECT, "Sound/SE/select.wav");		// �Z���N�gSE
	ds.AddSound(SE_DECIDE, "Sound/SE/sel_enter.ogg");	// �y�Ȍ���SE

	ds.AddSound(SE_GREAT, "Sound/SE/GREAT.wav");		// �m�[�g�O���[�gSE
	ds.AddSound(SE_GOOD, "Sound/SE/GOOD.wav");			// �m�[�g�O�b�hSE
	ds.AddSound(SE_EXNOTE, "Sound/SE/ExNote.wav");		// �G�N�X�g���m�[�gSE

	for (int i = 0; i < 4; i++) { ds.AddSound(SE_HOLDNOTE + i, "Sound/SE/HoldNote.wav"); } // �z�[���h�m�[�gSE

	// �y�ȃf�[�^�̃p�X�擾
	GetBMSFilePath(); // �y�Ȃ̃t�@�C���p�X���擾

	TitleSate = TS_STRAT;

	// �V�[���J��
	SceneMove(GS_TITLE, 1.0f, 0.0f, D3DXVECTOR3(0.0f, 0.0f, 0.0f), BGM_TITLE, BGM_NONE);

}

// �^�C�g���p���s���[�v
void CGame::TitleRun()
{
	// �ȑI���V�[����(START)
	if (GetKeyState(KYE_DECIDE) & 0x80 || GetKeyState(VK_RETURN) & 0x80) // �u]�v�L�[
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
				DEBUG("�ȑI���X�e�[�g�ړ�\n");
				BmsHeaderLoad(); // �ŏ��̊y�Ȃ̃w�b�_�[�������[�h
				SceneChangeFlg = true;
				break;

			case TS_EXIT:
				GameState = GS_END; // �Q�[���I��
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

	// �^�C�g���w�i�\��
	m_pMenuBG[0]->Render(D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2(WINDOW_WIDTH, WINDOW_HEIGHT), SpriteColor, true);
}

// �ȑI��p���s���[�v
void CGame::SelectRun()
{
	///////////
	// �`�揈��
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

	// �ȍ~�_�~�[
	for (int i = 0; i < 3; i++) { m_pMusicWindow->Render(D3DXVECTOR2(700.0f + (i * 110.0f), 120.0f - (i * 70.0f)), D3DXVECTOR2(0.75f, 0.79f), SpriteColor, true); }
	m_pMusicWindow->Render(D3DXVECTOR2(700, 445), D3DXVECTOR2(0.75f, 0.79f), SpriteColor, true);
	m_pMusicWindow->Render(D3DXVECTOR2(810, 515), D3DXVECTOR2(0.75f, 0.79f), SpriteColor, true);

	m_pMenuBG[1]->Render(D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2(WINDOW_WIDTH, WINDOW_HEIGHT), SpriteColor, true);

	///////////
	// ���͏���
	///////////

	// �y�Ȍ���
	if (GetKeyState(VK_RETURN) & 0x80) // �u]�v�L�[
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

	// �y�ȑI��(�����ꂽ�u�Ԏ��̊y�Ȃ̃w�b�_�[�������[�h)
	if (GetKeyState(KYE_UP) & 0x80 || GetKeyState(VK_UP) & 0x80)
	{
		if (!bOnKey[1])
		{
			bOnKey[1] = true;
			SelectCnt++;
			if (SelectCnt > MaxMusicCnt)
				SelectCnt = 0;			// �y�ȃZ���N�g���[�v
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
				SelectCnt = MaxMusicCnt;// �y�ȃZ���N�g���[�v
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

	static int count; //�n�C�X�s�[�h�L�[�J�E���g
	if (count > 0)
		count--; // �L�[�J�E���g��--

	// �X�N���[���X�s�[�h
	if (GetKeyState(VK_RIGHT) & 0x80 && count == 0) // ���L�[
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
	if (GetKeyState(VK_LEFT) & 0x80 && count == 0) // ���L�[
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

// �y�Ȍ��艉�o�֐�
void CGame::MusicDecide()
{
	const int cnt = 240;	// 6�b���
	ChangeCnt++;			// ���Ԃ��J�E���g

	m_pSceneChangeTex->Render(D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2(WINDOW_WIDTH, WINDOW_HEIGHT), ChangeSceneColor, true); // �t�F�[�h�摜

	if (ChangeCnt < cnt / 2) // �J�E���g�����������Ȃ�t�F�[�h�C��
	{
		ds.Play(BGM_DECIDE, FALSE); // BGM�Đ�
		ChangeSceneColor.w += 0.04f; // �t�F�[�h�A�E�g�̑���
	}
	if (ChangeCnt > cnt / 2) // �J�E���g�������𒴂�����t�F�[�h�A�E�g
	{
		// ���肵���y�Ȃ̃w�b�_�[�\��
		m_pText->Render(title, 490, 450, SpriteColor.w);
		m_pText->Render(artist, 490, 490, SpriteColor.w);
		m_pText->Render(genre, 750, 700, SpriteColor.w);
		m_pText->Render(playlevel, 860, 750, SpriteColor.w);
		m_pText->Render(bpm, 1100, 750, SpriteColor.w);

		m_pMenuBG[3]->Render(D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2(WINDOW_WIDTH, WINDOW_HEIGHT), SpriteColor, true); // �w�i�A�y�ȕ\��

		ChangeSceneColor.w -= 0.04f; // �t�F�[�h�C���̑���
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
	const int cnt = 300;	// 4�b���
	ChangeCnt++;			// ���Ԃ��J�E���g
	MainGameRun();

	// �`���[�g���A���摜�\��
	m_pTutorial->Render(D3DXVECTOR2(735, 590), D3DXVECTOR2(0.87f, 1.6f), SpriteColor, true);

	// ���߃o�[���Z�b�g
	ZeroMemory(&iStartNum, sizeof(iStartNum));
	bms.Restart();

	if (cnt < ChangeCnt)
	{
		ChangeCnt = 0;
		
		// �G�t�F�N�g�Đ�
		Effect::getInstance().Effect_Play("READY", D3DXVECTOR3(0.0f, -0.3f, 0.0f)); // �G�t�F�N�g�Đ�

		// �}�V���̎��g�����擾
		LARGE_INTEGER freq;
		QueryPerformanceFrequency(&freq);
		llGlobalFreq = freq.QuadPart;

		// ���݂̎��Ԃ��J�n���ԂƂ���
		LARGE_INTEGER li;
		QueryPerformanceCounter(&li);
		llStartTime = li.QuadPart;

		ds.Stop(S_GAMEMUSIC);
		ds.Reset(S_GAMEMUSIC);
		
		// �X�R�A�̏�����
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

// ���C���Q�[���I�����o
void CGame::Finish()
{
	const int cnt = 120;	// 2�b���
	ChangeCnt++;

	if (ResultEffectFlg == true)
	{
		if (Gage_y > -0.7f)
			Effect::getInstance().Effect_Play("FAILED_RESULT", D3DXVECTOR3(0.0f, 1.7f, 0.0f)); // �G�t�F�N�g�Đ�
		else
		{
			if(BAD == 0)
				Effect::getInstance().Effect_Play("PERFECT_RESULT", D3DXVECTOR3(0.0f, 1.5f, 0.0f)); // �G�t�F�N�g�Đ�
			else
				Effect::getInstance().Effect_Play("CLEAR_RESULT", D3DXVECTOR3(0.0f, 1.7f, 0.0f)); // �G�t�F�N�g�Đ�
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

// ���C���Q�[���p���s���[�v
void CGame::MainGameRun()
{
	static const int index[4] = { 0,1,2,3 };								// �C���f�b�N�X���X�g
	static const float obj_x[4] = { -1.03f,-0.34f,0.34f,1.03f };			// �m�[�c�̕\��X���W
	static const float efc_x[4] = { -0.65f,-0.22f,0.22f,0.65f };			// �G�t�F�N�g�̕\��X���W
	static int cnt[4];														// �m�[�c���������u�ԕ\������v�b�V�����C���̃J�E���g
	static float JudgeMarkSize[4];											// �W���b�W�}�[�N�̃T�C�Y
	static const D3DXVECTOR3 JudgeEffectPos(0.0f, 1.0f, 1.0f);				// ���蕶���̈ʒu
	float One_Beat = now_count % 2400;								// �ꔏ�̐��l
	
	static const int GoodAddScore = 600;
	static const int GreatAddScore = 1400;
	static const int PerfectAddScore = 2000;
	static const int ExAddScore = 3000;

	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	dElapsedTime = (double)(li.QuadPart - llStartTime) / llGlobalFreq;

	// �o�߂������Ԃ���i��BMS�J�E���g�l���Z�o
	now_count = bms.GetCountFromTime(dElapsedTime);

	// �y�Ȃ�0���ߖڂ𒴂������_�ōĐ�����
	if (now_count > BMS_RESOLUTION && OnePlayBGM == true)
	{
		OnePlayBGM = false;
		ds.Play(S_GAMEMUSIC, FALSE);
	}

	if (GameStartFlg == false)
	{
		// �Q�[���Ŏg�p���鐔�l�̏�����
		PERFECT = 0;
		GREAT = 0;
		GOOD = 0;
		BAD = 0;
		Combo = 0;
		MaxCombo = 0;
		now_count = 0.0;
	}

	// R�L�[����������Z���N�g��
	if(GetKeyState('R') & 0x80)
		SceneChangeFlg = true;

	if (SceneChangeFlg == true)
	{
		SceneMove(GS_SELECT, 0.05f, 30.0f, D3DXVECTOR3(0.0f, 0.0f, 0.0f), S_GAMEMUSIC, S_GAMEMUSIC);
	}

	// BMS�J�E���^���Ȃ̍ő�J�E���g-4���߂𒴂����烊�U���g��
	if (bms.GetMaxCount() - BMS_RESOLUTION * 4 <= now_count)
	{
		// ���߃o�[���Z�b�g
		ZeroMemory(&iStartNum, sizeof(iStartNum));
		bms.Restart();

		now_count = 0.0; // �J�E���g�X�g�b�v
		Finish();		 // �Q�[���I�����o�֐�
	}

	if (SpriteColor.w < 1.0f)
	{
		SpriteColor.w += 0.02f;
	}
	

	///////////
	// ���͏���
	///////////

	static int count; //�n�C�X�s�[�h�L�[�J�E���g
	if (count > 0)
		count--; // �L�[�J�E���g��--

	// �X�N���[���X�s�[�h
	if (GetKeyState(VK_RIGHT) & 0x80 && count == 0) // ���L�[
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
	if (GetKeyState(VK_LEFT) & 0x80 && count == 0) // ���L�[
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

	// ���z���̓n�[�h�E�F�A(�����ꂽ�u�Ԃ���TRUE�ƂȂ�z��)
	BOOL press[4]; // 4Key
	ZeroMemory(&press, sizeof(press));
	// ���z���̓n�[�h�E�F�A(�����ꂽ��TRUE�ƂȂ�z��)
	BOOL keep[4];	// 4Key
	ZeroMemory(&keep, sizeof(keep));

	// ���Ղ̏���
	static const int KEYID[4] =
	{	// �L�[�̃��X�g
		KYE_DOWN,			// ch11�Ɋ��蓖�Ă�L�[
		KYE_UP,				// ch12�Ɋ��蓖�Ă�L�[
		KYE_CANCEL,			// ch13�Ɋ��蓖�Ă�L�[
		KYE_DECIDE,			// ch14�Ɋ��蓖�Ă�L�[
	};

	for (int i = 0; i < 4; i++)
	{
		if ((GetKeyState(KEYID[i]) & 0x80))
		{
			// �L�[�{�[�h�̓��͂��������ꍇ
			if (!bOnKey[i])
			{
				// �܂�������Ă��Ȃ���Ή����ꂽ�u�ԂƂ���
				press[i] = TRUE;
				bOnKey[i] = TRUE;
			}
			// �L�[�������Ă�ԂƂ���
			bOnHold[i] = TRUE;
			keep[i] = TRUE;
			JudgeMarkSize[i] = 1.4f;
		}
		else
		{
			// ������Ă��Ȃ���΃t���O�����Z�b�g
			bOnKey[i] = FALSE;
			bOnHold[i] = FALSE;

			// �W���b�W�}�[�N���e���|�ɍ��킹�ĕω�
			if (One_Beat > 0 && One_Beat < 300)
				JudgeMarkSize[i] = 1.0f;
			else
				JudgeMarkSize[i] = 1.1f;
			// �f�o�b�O�A�I�[�g
			/*press[i] = true;
			keep[i] = true;*/
		}
	}

	


	for (int i = 0; i < 4; i++)
	{
		// �z�[���h�m�[�g�̓r���ŗ�������SE������
		if (bOnHold[i] == FALSE)
		{
			Effect::getInstance().Effect_Stop("HoldNote" + i); // �G�t�F�N�g��~
			ds.Stop(SE_HOLDNOTE + i);
			ds.Reset(SE_HOLDNOTE + i);
		}
	}

	// ���͔���
	const LONG HOLD_RANGE		= BMS_RESOLUTION / 64;		// �z�[���h�p���肷�钆�S����͈̔�(�O�㍇�킹��32��������)
	const LONG PERFECT_RANGE	= BMS_RESOLUTION / 36;		// PERFECT�Ɣ��肷�钆�S����͈̔�(�O�㍇�킹��18��������)
	const LONG GREAT_RANGE		= BMS_RESOLUTION / 16;		// GREAT�Ɣ��肷�钆�S����͈̔�(�O�㍇�킹��8��������)
	const LONG GOOD_RANGE		= BMS_RESOLUTION / 12;		// GOOD�Ɣ��肷�钆�S����͈̔�(�O�㍇�킹��6��������)

	for (int j = 0; j < 4; j++)
	{
		// ����Ώۂ̃`�����l���̃m�[�g���`�F�b�N
		for (int i = iStartNum[j + 0x11 + 0x20]; i < bms.GetObjeNum(0x11 + j); i++)
		{
			LPBMSDATA Note = (LPBMSDATA)bms.GetObje(0x11 + j, i);
			if (Note->bFlag)
			{
				// �܂�������̃m�[�g�Ȃ�
				if (Note->lTime < (now_count - GOOD_RANGE))
				{
					// �ǔ�����߂�����S�Č����������Ƃ���
					Note->bFlag = FALSE; // �I�u�W�F������

					//// �z�[���h�m�[�g�̓r���ŗ�������SE������
					//if (Note->iType == HoldFinish && Note->bFlag == FALSE)
					//{
					//	Effect::getInstance().Effect_Stop("HoldNote" + j); // �G�t�F�N�g��~
					//	ds.Stop(SE_HOLDNOTE);
					//	ds.Reset(SE_HOLDNOTE);
					//}
					Effect::getInstance().Effect_Play("BAD", D3DXVECTOR3(efc_x[j], 0.5f, -0.1f)); // �G�t�F�N�g�Đ�
					Effect::getInstance().Effect_Play("BAD_Font", JudgeEffectPos); // �G�t�F�N�g�Đ�

					BAD++;
					Gage_y += AddGage_y;
					Combo = 0;
					// ����I�u�W�F�����̎�����ɕύX
					iStartNum[j + 0x11 + 0x20] = i + 1;
					// ���̃I�u�W�F���`�F�b�N
					continue;
				}

				// �m�[�g������O�Ȃ甲����
				if ((now_count + GOOD_RANGE) <= Note->lTime)
					break;

				// �m�[�g��������Ȃ�L�[�������ꂽ�u�Ԃ����`�F�b�N
				if (press[j])
				{
					// �L�[���������u�ԂȂ琸�x����
					LONG sub = abs(now_count - Note->lTime);	// �m�[�g�Ƃ̍����Βl�Ŏ擾
					int jadge = 0;								// ����l(0=POOR�A1=BAD�A2=GOOD�A3=GREAT�A4=PKGREAT�Ȃ�)
					
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
							Effect::getInstance().Effect_Play("ExNote", D3DXVECTOR3(efc_x[j], 0.5f, -0.1f)); // �G�t�F�N�g�Đ�
							Effect::getInstance().Effect_Play("PERFECT_Font", JudgeEffectPos); // �G�t�F�N�g�Đ�
							ds.Reset(SE_EXNOTE);
							ds.Play(SE_EXNOTE);
						}
						if (Note->iType == Normal)
						{
							Max_Score += GreatAddScore;
							Effect::getInstance().Effect_Play("PERFECT", D3DXVECTOR3(efc_x[j], 0.5f, -0.1f)); // �G�t�F�N�g�Đ�
							Effect::getInstance().Effect_Play("PERFECT_Font", JudgeEffectPos); // �G�t�F�N�g�Đ�
							ds.Reset(SE_GREAT);
							ds.Play(SE_GREAT);
						}
						if (Note->iType == HoldStart)
						{
							Max_Score += GreatAddScore;
							Effect::getInstance().Effect_Play("HoldNote" + j, D3DXVECTOR3(efc_x[j], 0.5f, -0.1f)); // �G�t�F�N�g�Đ�
							Effect::getInstance().Effect_Play("PERFECT_Font", JudgeEffectPos); // �G�t�F�N�g�Đ�
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
							Effect::getInstance().Effect_Play("ExNote", D3DXVECTOR3(efc_x[j], 0.5f, -0.1f)); // �G�t�F�N�g�Đ�
							Effect::getInstance().Effect_Play("PERFECT_Font", JudgeEffectPos); // �G�t�F�N�g�Đ�
							ds.Reset(SE_EXNOTE);
							ds.Play(SE_EXNOTE);
						}
						if (Note->iType == Normal)
						{
							GREAT++;
							Max_Score += GreatAddScore;
							Effect::getInstance().Effect_Play("GREAT", D3DXVECTOR3(efc_x[j], 0.5f, -0.1f)); // �G�t�F�N�g�Đ�
							Effect::getInstance().Effect_Play("GREAT_Font", JudgeEffectPos); // �G�t�F�N�g�Đ�
							ds.Reset(SE_GREAT);
							ds.Play(SE_GREAT);
						}
						if (Note->iType == HoldStart)
						{
							GREAT++;
							Max_Score += GreatAddScore;
							Effect::getInstance().Effect_Play("HoldNote" + j, D3DXVECTOR3(efc_x[j], 0.5f, -0.1f)); // �G�t�F�N�g�Đ�
							Effect::getInstance().Effect_Play("PERFECT_Font", JudgeEffectPos); // �G�t�F�N�g�Đ�
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
							Effect::getInstance().Effect_Play("ExNote", D3DXVECTOR3(efc_x[j], 0.5f, -0.1f)); // �G�t�F�N�g�Đ�
							Effect::getInstance().Effect_Play("PERFECT_Font", JudgeEffectPos); // �G�t�F�N�g�Đ�
							ds.Reset(SE_EXNOTE);
							ds.Play(SE_EXNOTE);
						}
						if (Note->iType == Normal)
						{
							GOOD++;
							Max_Score += GoodAddScore;
							Effect::getInstance().Effect_Play("GOOD", D3DXVECTOR3(efc_x[j], 0.5f, -0.1f)); // �G�t�F�N�g�Đ�
							Effect::getInstance().Effect_Play("GOOD_Font", JudgeEffectPos); // �G�t�F�N�g�Đ�
							ds.Reset(SE_GOOD);
							ds.Play(SE_GOOD);
						}
						if (Note->iType == HoldStart)
						{
							GOOD++;
							Max_Score += GoodAddScore;
							Effect::getInstance().Effect_Play("HoldNote" + j, D3DXVECTOR3(efc_x[j], 0.5f, -0.1f)); // �G�t�F�N�g�Đ�
							Effect::getInstance().Effect_Play("PERFECT_Font", JudgeEffectPos); // �G�t�F�N�g�Đ�
							ds.Reset(SE_HOLDNOTE + j);
							ds.Play(SE_HOLDNOTE + j, TRUE);
						}
						cnt[j] = 10;
					}

					if (jadge >= 1) // ����̃m�[�c����
					{
						// GOOD�ȏ�Ȃ�I�u�W�F������
						Note->bFlag = FALSE;	// �m�[�c������

						// ����m�[�c�����̎�����ɕύX
						iStartNum[j + 0x11 + 0x20] = i + 1;
						break;
					}
				}

				// �z�[���h�m�[�g��������Ȃ�L�[�������Ă�Ԃ����`�F�b�N
				if (keep[j] && Note->iType == HoldFinish)
				{
					LONG sub = abs(now_count - Note->lTime);	// �m�[�g�Ƃ̍����Βl�Ŏ擾
					bool jadge = false;							// �����Ă�Ԃɔ���l�ɓ�������
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
							Effect::getInstance().Effect_Stop("HoldNote" + j); // �G�t�F�N�g��~
							Effect::getInstance().Effect_Play("PERFECT", D3DXVECTOR3(efc_x[j], 0.5f, -0.1f)); // �G�t�F�N�g�Đ�
							Effect::getInstance().Effect_Play("PERFECT_Font", JudgeEffectPos); // �G�t�F�N�g�Đ�
							ds.Reset(SE_EXNOTE);
							ds.Play(SE_EXNOTE);

							ds.Stop(SE_HOLDNOTE + j);
						}
					}
					if (jadge == true) // ����̃m�[�c����
					{
						// GOOD�ȏ�Ȃ�I�u�W�F������
						Note->bFlag = FALSE; // �m�[�c������

						// ����m�[�c�����̎�����ɕύX
						iStartNum[j + 0x11 + 0x20] = i + 1;
						break;
					}
				}
			}
		}
	}

	///////////
	// �`�揈��
	///////////

	// �t�H���g�\��
	char ViewFont[50];
	bms.GetBmsHeader(title, artist, genre, playlevel, bpm, score); // �y�ȏ��擾
	// �y�ȏ��\��
	m_pTextSizeS->Render(title, 1220, 110, SpriteColor.w);
	m_pTextSizeS->Render(playlevel, 1495, 182, SpriteColor.w);
	m_pTextSizeS->Render(bpm, 1630, 182, SpriteColor.w);

	// �X�N���[���X�s�[�h�\��
	sprintf_s(ViewFont, "%.1f", Hi_SPEED);
	m_pTextSizeS->Render(ViewFont, 1600, 230, SpriteColor.w);

	// ����\��
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
	m_pText->Render(ViewFont, 120, 300, SpriteColor.w);	// �X�R�A(999��9999�܂�)

	Now_Score += 1111;

	if (Now_Score > Max_Score)
		Now_Score = Max_Score;

	// UI�\��
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
	// ���f��
	/////////

	// ���[�����f��
	m_pLane->Render(D3DXVECTOR3(0.0f, -0.05f, 0.0f), D3DXVECTOR3(0, 0, 0), D3DXVECTOR3(1, 1, 1));
	
	for (int i = 0; i < 8; i++)
	{
		if (i < 4)
		{
			// �W���b�W�}�[�N���f��
			m_pJudgeMark[i]->Render(D3DXVECTOR3(obj_x[i], 0.02f, 0.0f), D3DXVECTOR3(0, 0, 0), D3DXVECTOR3(JudgeMarkSize[i], 1, JudgeMarkSize[i]));

			// ���C�����f��
			m_pLine[i]->Render(D3DXVECTOR3(obj_x[i], 0.0f, 0.0f), D3DXVECTOR3(0, 0, 0), D3DXVECTOR3(1, 1, 1));
		}
		if (3 < i && 0 < cnt[i - 4])
		{
			// ���C�����f��
			m_pLine[i]->Render(D3DXVECTOR3(obj_x[i - 4], 0.01f, 0.0f), D3DXVECTOR3(0, 0, 0), D3DXVECTOR3(1, 1, 1));
			cnt[i - 4]--;
		}
	}

	// �m�[�c
	for (int j = 0; j < 4; j++)
	{
		for (int i = iStartNum[0x11 + index[j]]; i < bms.GetObjeNum(0x11 + index[j]); i++)
		{
			LPBMSDATA Note = bms.GetObje(0x11 + index[j], i);
			float off_z = (float)((double)(Note->lTime - now_count) / BMS_RESOLUTION * (Hi_SPEED * 10));
			float scal_z = (float)(Note->fScal * Hi_SPEED / 4.0);
			// ���胉�C����艺�Ȃ�����\���͂����A���񂩂炻�̎��̃I�u�W�F����Q�Ƃ���
			if (off_z < -21.0f || Note->bFlag == FALSE)
			{
				iStartNum[index[j] + 0x11] = i + 1;
				continue;
			}
			// ��ʂ̏���O�Ȃ�΂��̐�͑S�ĕ`��X�L�b�v
			if (off_z > 80 + (float)Note->fScal * Hi_SPEED / 4.0f * 2.5f)
				break;
			// ��ʓ��Ȃ�`��
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
						// �z�[���h�m�[�g�̃X�P�[����0�ȉ��Ȃ��SE���~�߂�
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

	
	// �h�[��
	for (int i = iStartNum[0]; i < bms.GetBarNum(); i++)
	{
		LPBMSBAR bar = bms.GetBar(i);
		float off_z = (float)((double)(bar->lTime - now_count) / BMS_RESOLUTION * 30);
		// ���胉�C����艺�Ȃ�����\���͂����A���񂩂炻�̎��̏��߂���Q�Ƃ���
		if (off_z < 0.0f)
		{
			iStartNum[0] = i + 1;
			continue;
		}
		// ��ʂ̏���O�Ȃ�΂��̐�͑S�ĕ`��X�L�b�v
		if (off_z > 200)
			break;
		// ��ʓ��Ȃ�`��
		if (Gage_y > -0.7f)
			m_pDome->Render(D3DXVECTOR3(0.0f, 1.8f, off_z), D3DXVECTOR3(0, 0, 0), D3DXVECTOR3(1.5, 1.5, 1.5));
		else
			m_pDomeClear->Render(D3DXVECTOR3(0.0f, 1.8f, off_z), D3DXVECTOR3(0, 0, 0), D3DXVECTOR3(1.5, 1.5, 1.5));

	}

	// ���߃o�[
	for (int i = iStartNum[0]; i < bms.GetBarNum(); i++)
	{
		LPBMSBAR bar = bms.GetBar(i);
		float off_z = (float)((double)(bar->lTime - now_count) / BMS_RESOLUTION * (Hi_SPEED * 10));
		// ���胉�C����艺�Ȃ�����\���͂����A���񂩂炻�̎��̏��߂���Q�Ƃ���
		if (off_z < -0.0f)
		{
			iStartNum[0] = i + 1;
			continue;
		}
		// ��ʂ̏���O�Ȃ�΂��̐�͑S�ĕ`��X�L�b�v
		if (off_z > 40 + 2)
			break;
		// ��ʓ��Ȃ�`��
		m_pBar->Render(D3DXVECTOR3(0, 0, off_z), D3DXVECTOR3(0, 0, 0), D3DXVECTOR3(1.0f, 1.0f, 5.0f));
	}

	// �G�t�F�N�g
	Effect::getInstance().Draw(); // �G�t�F�N�g�`��
}

// ���U���g�p���s���[�v
void CGame::ResultRun()
{
	// DEBUG("���U���g�X�e�[�g���s��\n");

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
	

	// �ȑI���V�[���֖߂�
	if (GetKeyState(KYE_DECIDE) & 0x80 || GetKeyState(VK_RETURN) & 0x80)
	{
		if (!bOnKey[3])
		{
			bOnKey[3] = true;
			SceneChangeFlg = true;
			//bms.Save(mBFP[SelectCnt].mBmsFilePath, Now_Score);// �X�R�A���L�^
			BmsFileClear();		// BMS�t�@�C������
			BmsHeaderLoad();	// �ŏ��̊y�Ȃ̃w�b�_�[�������[�h
		}
	}
	else
	{
		bOnKey[3] = false;
	}
	
	if (SceneChangeFlg == true)
		SceneMove(GS_SELECT, 0.01f, 40.0f, D3DXVECTOR3(0.0f, 0.0f, 0.0f), S_GAMEMUSIC, BGM_RESULT);


	// �t�H���g�\��
	char ViewFont[50];

	m_pText->Render(title, 250, 148, SpriteColor.w);
	m_pText->Render(artist, 250, 187, SpriteColor.w);

	m_pText->Render("SCORE", 980, 450, SpriteColor.w);
	sprintf_s(ViewFont, "%07d", Now_Score);
	m_pText->Render(ViewFont, 1040, 500, SpriteColor.w); // �X�R�A(999��9999�܂�)

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

	// �摜�\��
	if (Gage_y > -0.7f)		// Failed�\��
		m_pResult[1]->Render(D3DXVECTOR2(240, 300), D3DXVECTOR2(2.0f, 1.5f), SpriteColor, true);
	else					// Clear�\��
		m_pResult[0]->Render(D3DXVECTOR2(230, 285), D3DXVECTOR2(2.0f, 1.5f), SpriteColor, true);

	// �w�i�\��
	m_pMenuBG[2]->Render(D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2(WINDOW_WIDTH, WINDOW_HEIGHT), SpriteColor, true);

}

// BMS�t�@�C�����[�h�֐�
void CGame::BmsFileLoad()
{
	//DEBUG("BMS�t�@�C�����[�h\n");
	// �t�@�C�����[�h�֐�
	if (!bms.Load(mBFP[SelectCnt].mBmsFilePath))
	{
		DEBUG("BMSFile Load Error\n");
	}
	
	// �Q�[�W�̍ő�l���擾
	MaxGage = bms.GetMaxNoteCnt() * 3; // BAD-1,GOOD+1,GREAT+1,PERFECT+3,�Ƃ���̂ōő� * 3�Ƃ���
	AddGage_y = (float)1 / MaxGage * 3;
	Gage_y = 0.0f; // �Q�[�W�̏�����
	Now_Score = 0; // �X�R�A�̏�����
	Max_Score = 0; // �X�R�A�̏�����
	GameStartFlg = false;
	SceneMove(GS_READY, 0.05f, 300.0f, D3DXVECTOR3(0.0f, 0.0f, 0.0f), BGM_NONE, S_GAMEMUSIC);
}

// BMS�t�@�C�������֐�
void CGame::BmsFileClear()
{
	DEBUG("BMS�t�@�C������\n");
	// BMS�t�@�C�������֐�
	if (!bms.Clear())
	{
		DEBUG("BMSFile Clear Error\n");
	}
	//
}

// BMS�t�@�C���̃w�b�_�[���̂݃��[�h
void CGame::BmsHeaderLoad()
{
	//DEBUG("BMS�t�@�C���w�b�_�[��񃍁[�h\n");
	// BMS�t�@�C���w�b�_�[��񃍁[�h
	bms.LoadHeader(mBFP[SelectCnt].mBmsFilePath);
}

// BMS�t�@�C���̃w�b�_�[���̂ݏ���
void CGame::BmsHeaderClear()
{
	DEBUG("BMS�t�@�C���w�b�_�[������\n");
	// BMS�t�@�C���w�b�_�[������
	//
}

// BMS�t�@�C���p�X�擾
void CGame::GetBMSFilePath()
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	WCHAR* FilePath;
	char BmsName[100]; // �u****.bms�v����p�ϐ�
	size_t wLen = 0;
	int cnt = 0; // �J��Ԃ��J�E���g 

	hFind = FindFirstFile(L"BMSFile/*.bms", &FindFileData);

	while (true)
	{
		if (hFind == INVALID_HANDLE_VALUE)
		{
			return;
		}
		else
		{
			strcpy_s(mBFP[cnt].mBmsFilePath, "BMSFile/"); // �p�X���

			FilePath = FindFileData.cFileName;
			// setlocale(LC_ALL, "japanese");
			wcstombs_s(&wLen, BmsName, 100, FilePath, _TRUNCATE); // WCHAR����char�֕ϊ�����
			strcat_s(mBFP[cnt].mBmsFilePath, BmsName); //�uBMSFile/�v�Ɓu****.bms�v�̕����񌋍�

			// �y�ȃf�[�^�̃t�@�C���p�X���
			
			for (int i = 0; i < 256; i++)
			{
				// �u.�v���u_�v�Ȃ�Δ�����
				if (mBFP[cnt].mBmsFilePath[i] == '.'|| mBFP[cnt].mBmsFilePath[i] == '_')
				{
					strcat_s(mBFP[cnt].mBmsMusicPath, ".wav");
					break;
				}
				// �y�ȗp�t�@�C���p�X���R�s�[
				mBFP[cnt].mBmsMusicPath[i] = mBFP[cnt].mBmsFilePath[i];
			}

			// ����****.bms�t�@�C����ǂݍ���
			if (!FindNextFile(hFind, &FindFileData))
			{
				FindClose(hFind);
				MaxMusicCnt = cnt; // �ő�y�Ȑ�����
				SelectCnt = 0;
				// �ŏ��ɗ����y�Ȃ̃p�X����
				ds.AddSound(S_GAMEMUSIC, mBFP[SelectCnt].mBmsMusicPath);
				return;
			}
			cnt++;
		}
	}
}

// ���[�h�ς݃f�[�^�S�J��
void CGame::AllClear()
{
	DEBUG("���[�h�ς݃f�[�^�S����\n");
	ds.Clear();
}

// �V�[���ړ����K�֐�
void CGame::SceneMove(GAMESTATE NextScene, float FadeTime, float StartMoveTime, D3DXVECTOR3 FadeColor, SOUND Play, SOUND Stop)
{
	SpriteColor.w -= FadeTime; // ����
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
//���b�Z�[�W���[�v�ƃA�v���P�[�V���������̓����
////////////////////////////////////////////////
void CGame::Run()
{
	// �Q�[�����C�����[�v
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
				DEBUG("�ʏ�I��\n");
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//��ʃN���A�i���ۂ͒P�F�ŉ�ʂ�h��Ԃ������j
		float ClearColor[4] = { 0,0,0,1 };// �N���A�F�쐬�@RGBA�̏�
		m_pDeviceContext->ClearRenderTargetView(m_pBackBuffer_TexRTV, ClearColor);//��ʃN���A
		m_pDeviceContext->ClearDepthStencilView(m_pBackBuffer_DSTexDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);//�[�x�o�b�t�@�N���A
		
		
		//�J��������
		// �R�l�̎��_�����@�r���[�g�����X�t�H�[�� �J�������L�����̌��ɔz�u���邾��																							//�J�������
		D3DXVECTOR3 vEyePt(0.0f, 2.5f, -5.0f);		//�J�����i���_�j�ʒu
		D3DXVECTOR3 vLookatPt(0.0f, 0.5f, 3.0f);	//�����ʒu
		D3DXVECTOR3 vUpVec(0.0f, 0.0f, 1.5f);		//����ʒu
		D3DXMatrixLookAtLH(&mView, &vEyePt, &vLookatPt, &vUpVec);
		// �v���W�F�N�V�����g�����X�t�H�[���i�ˉe�ϊ��j
		D3DXMatrixPerspectiveFovLH(&mProj, D3DX_PI / 4, (FLOAT)WINDOW_WIDTH / (FLOAT)WINDOW_HEIGHT, 0.1f, 1000.0f);

		switch (GameState)
		{
		case GS_INITLOAD:
			// ������&���[�h
			InitLoad();
			break;

		case GS_TITLE:
			// �^�C�g��
			TitleRun();
			break;

		case GS_SELECT:
			// �ȑI��
			SelectRun();
			break;

		case GS_MAINGAME:
			// ���C���Q�[��
			MainGameRun();
			break;

		case GS_DECIDE:
			// �y�Ȍ��艉�o
			MusicDecide();
			break;

		case GS_READY:
			// �Q�[���X�^�[�g���o
			Ready();
			break;

		case GS_FINISH:
			// �Q�[���I��艉�o
			Finish();
			break;

		case GS_RESULT:
			// ���U���g
			ResultRun();
			break;

		case GS_END:
			// �S�ẴQ�[������
			bLoop = FALSE;
			DEBUG("�ʏ�I��\n");
			break;

		default:
			// ����`�̃Q�[���X�e�[�g
			bLoop = FALSE;
			DEBUG("�ُ�I��\n");
			break;
		}
		// Sleep(0); // ������~

		// �J����
		CD3DXMESH_ANIM::SetCamera(mView, mProj, D3DXVECTOR3(0.0f, 0.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 1.5f));
		D3D11_SPRITE::SetCamera(mView, mProj);
		TD_Graphics::SetCamera(mView, mProj);

		// �G�t�F�N�g�̒����_
		Effect::getInstance().SetCamera(vEyePt, vLookatPt);

		// �G�t�F�N�g�`��X�V
		Effect::getInstance().Update("GREAT");

		// �t���X�N���[��
		/*static bool fullsc = true;
		if (fullsc)
		{
			BOOL	f;
			m_pSwapChain->GetFullscreenState(&f, 0);
			if (!f)
				m_pSwapChain->SetFullscreenState(true, 0);
			fullsc = false;
		}*/

		// ��ʍX�V�i�o�b�N�o�b�t�@���t�����g�o�b�t�@�Ɂj
		m_pSwapChain->Present(1, 0);
	}
	// �A�v���P�[�V�����̏I��
}