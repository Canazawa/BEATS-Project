#include "../SauceFile/Origin/COrigin.h"
#include "Mesh/CD3DXMESH_ANIM.h"
#include "Text/D3D11_TEXT.h"
#include "Sound/CDSPro81.h"
#include "../Effect/Effect.h"
#include "Sprite/D3D11_SPRITE.h"
#include "Sprite/TD_Graphics.h"
#include "BMSFileLoad/CBmsPro.h"

// �Q�[���X�e�[�g
enum GAMESTATE
{
	GS_INITLOAD = 0,	// ������&���[�h(��x�̂�)
	GS_TITLE,			// �^�C�g��
	GS_SELECT,			// �ȑI��
	GS_MAINGAME,		// ���C���Q�[��
	GS_RESULT,			// ���U���g
	GS_CREDIT,			// �N���W�b�g
	GS_DECIDE,			// �y�Ȍ���
	GS_READY,			// ���C���Q�[���X�^�[�g���o
	GS_FINISH,			// ���C���Q�[���I��艉�o
	GS_END				// �S�ẴQ�[�������I��
};

// �^�C�g���Z���N�g
enum TITLESELECT
{
	TS_STRAT,			// �y�ȑI����
	TS_CREDIT,			// �N���W�b�g��
	TS_EXIT,			// �Q�[���I��
};

// �T�E���h
enum SOUND
{
	// 0�`9��BGM
	S_GAMEMUSIC = 0,
	BGM_TITLE,
	BGM_SELECT,
	BGM_RESULT,
	BGM_DECIDE,
	BGM_NONE,

	// 10�`19��SE
	SE_START = 10,
	SE_SELECT,
	SE_DECIDE,
	SE_GREAT,
	SE_GOOD,
	SE_EXNOTE,
	SE_HOLDNOTE,

};

enum MAIN_TEX
{
	MGUI_HEADER = 0,
	MGUI_GAGE,
	MGUI_SCORE,
	MGUI_JUDGE,
	MGUI_FRAME,
};

// BMS�f�[�^���
typedef struct _BMSGETPATH
{
	char mBmsFilePath[256];		// ���ʃf�[�^�̃p�X
	char mBmsMusicPath[256];	// �y�ȃf�[�^�̃p�X
} BMSGETPATH, BMSGETPATH;



class CGame
{
public:
// �֐�
	CGame();			// �R���X�g���N�^
	virtual ~CGame();	// �f�X�g���N�^
	
	void Run();			// �Q�[�����C�����[�`��
	void Destroy();		// ���

// �ϐ�
	HRESULT InitWindow(HINSTANCE, INT, INT, INT, INT, LPCWSTR);
	HRESULT Init();
	LRESULT MsgProc(HWND, UINT, WPARAM, LPARAM);

	HWND m_hWnd;
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext *m_pDeviceContext;
	IDXGISwapChain* m_pSwapChain;
	ID3D11RenderTargetView* m_pBackBuffer_TexRTV;
	ID3D11DepthStencilView* m_pBackBuffer_DSTexDSV;
	ID3D11Texture2D* m_pBackBuffer_DSTex;

private:
// �֐�
	void InitLoad();										// ������&���[�h(��x�̂�)
	void TitleRun();										// �^�C�g���p���s���[�v�֐�
	void SelectRun();									// �ȑI��p���s���[�v�֐�
	void MainGameRun();							// ���C���Q�[���p���s���[�v�֐�
	void ResultRun();									// ���U���g�p���s���[�v�֐�

	void BmsFileLoad();								// BMS�t�@�C�����[�h�֐�
	void BmsFileClear();								// BMS�t�@�C�������֐�

	void BmsHeaderLoad();							// BMS�t�@�C���̃w�b�_�[���̂݃��[�h
	void BmsHeaderClear();							// BMS�t�@�C���̃w�b�_�[���̂݃N���A
	void GetBMSFilePath();						// BMS�t�@�C���p�X�擾�֐�

	void MusicDecide();									// �y�Ȍ���V�[�����K
	void Ready();											// ���C���Q�[���X�^�[�g���K
	void Finish();											// ���C���Q�[���I��艉�K

	void AllClear();										// ���[�h�ς݃f�[�^�S����

	void SceneMove(GAMESTATE NextScene, float FadeTime, float StartMoveTime, D3DXVECTOR3 FadeColor, SOUND Play, SOUND Stop);
// �ϐ�
	GAMESTATE		GameState;				// �Q�[���X�e�[�g
	TITLESELECT		TitleSate;		// �^�C�g�����̃X�e�[�g
	LONGLONG		llStartTime;			// �Q�[���J�n���̎���(���𑜓x�^�C�}�[)
	LONGLONG		llGlobalFreq;			// �}�V���̂P�b�Ԃ̃J�E���g��(���𑜓x�^�C�}�[)
	double			dElapsedTime;						// �J�n����̌o�ߎ���(�b)
	int				iStartNum[256];				// BMS���t�Ōv�Z�J�n����z��ԍ�(�������y�����邽��)
	float			Hi_SPEED;							// ���ߊԂ̕��̔{
	char			BMSData[256][256];				// BMS�f�[�^�i�[[.bms][.wav]
	int				SelectCnt;							// �y�ȃt�@�C��������
	int				MaxMusicCnt;					// �y�Ȃ̍ő吔
	int				ChangeCnt;							// �V�[���؂�ւ����̃J�E���g
	bool			SceneChangeFlg;					// �V�[����؂�ւ���t���O
	bool			GameStartFlg;
	D3DXVECTOR4		SpriteColor;		// �摜�̐F
	D3DXVECTOR4		ChangeSceneColor;	// ���o�p�摜�F
	D3DXVECTOR4		Flashing;					// �摜�̓_�ŗp
	float			AddFlash;			// �_�ŗp�����ϐ�
	bool			ResultEffectFlg;	// �ȏI�����ɕ\������G�t�F�N�g�̃t���O

	float			TitleCurPos;		// �^�C�g�����̃J�[�\���̈ʒu
	float			SelectMixWin;		// �y�ȑI�����ɋȕ\��UI�𓮂���

	LONGLONG		now_count;			// �y�Ȃ�BPM�ɍ��킹�ăX�N���[������ϐ�

	BMSGETPATH mBFP[256];				// �y�Ȃ̕��ʃf�[�^�ƋȂ̃p�X�i�[(256�Ȃ܂�)

	int MaxGage;						// �y�Ȃ̃X�R�A�̍ő吔
	float AddGage_y;					// 1�m�[�c���莞�̕ϓ�����y�̃Q�[�W�̒l
	float Gage_y;						// �Q�[�W�̒��̉摜��y�̐L�ї�

	int Now_Score;						// ���݂̃X�R�A���J�E���g���Ă���
	int Max_Score;						// Now_Score�̍ő�l

	int PERFECT;
	int GREAT;
	int GOOD;
	int BAD;
	int Combo;
	int MaxCombo;

	char title[256];
	char artist[256];
	char genre[256];
	char playlevel[256];
	char bpm[256];
	char score[8];

	bool OnePlayBGM;					// ��x�����y�Ȃ𗬂��t���O

	// ���f��
	CD3DXMESH_ANIM* m_pLane;			// ���[�����f��
	CD3DXMESH_ANIM* m_pDome;			// �h�[�����f��
	CD3DXMESH_ANIM* m_pDomeClear;		// �N���A���̃h�[�����f��
	CD3DXMESH_ANIM* m_pBar;				// �o�[���f��
	CD3DXMESH_ANIM* m_pLine[8];			// ���C�����f��(4�{)*2
	CD3DXMESH_ANIM* m_pNote;			// �m�[�c���f��
	CD3DXMESH_ANIM* m_pExNote;			// Ex�m�[�c���f��
	CD3DXMESH_ANIM* m_pHoldLongNote;	// �z�[���h�m�[�c���f��
	CD3DXMESH_ANIM* m_pJudgeMark[4];	// ����}�[�N

	// �e�L�X�g
	D3D11_TEXT* m_pText;				// �����\���p
	D3D11_TEXT* m_pTextSizeS;

	// �e�N�X�`���[
	TD_Graphics* m_pMenuBG[4];			// ���j���[�w�i�摜
	TD_Graphics* m_pMusicWindow;		// �y�Ȕw�i�摜
	TD_Graphics* m_pMainGameUI[5];		// ���C���Q�[��UI
	TD_Graphics* m_pSceneChangeTex;		// �V�[���؂�ւ����̉摜
	TD_Graphics* m_pGageType[2];		// �Q�[�W�����̐F
	TD_Graphics* m_pTitleSlect;			// �^�C�g�����̑I���摜
	TD_Graphics* m_pTutorial;
	TD_Graphics* m_pResult[2];			// ���U���g��Clear�AFailed����

	// 4�L�[���g���đ��삷��
	BOOL bOnKey[6];						// �L�[��������Ă��邩 [A]0,[S]1,[:]2,[}]3,[��]4,[��]5
	BOOL bOnHold[4];					// �L�[��������Ă��邩 (�z�[���h�p)

	//���C�u����
	CDSPro81	ds;						// DirectSound�Ǘ�
	CBmsPro		bms;					// BMS�t�@�C���Ǘ�

	static D3DXMATRIX mView;
	static D3DXMATRIX mProj;
	D3DXMATRIX mTran;					// ���W�p�}�g���b�N�X
	D3DXMATRIX mRot;					// ��]�p�}�g���b�N�X
	D3DXMATRIX mSca;					// �g��k���p�}�g���b�N�X
};
