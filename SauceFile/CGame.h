#include "../SauceFile/Origin/COrigin.h"
#include "Mesh/CD3DXMESH_ANIM.h"
#include "Text/D3D11_TEXT.h"
#include "Sound/CDSPro81.h"
#include "../Effect/Effect.h"
#include "Sprite/D3D11_SPRITE.h"
#include "Sprite/TD_Graphics.h"
#include "BMSFileLoad/CBmsPro.h"

// ゲームステート
enum GAMESTATE
{
	GS_INITLOAD = 0,	// 初期化&ロード(一度のみ)
	GS_TITLE,			// タイトル
	GS_SELECT,			// 曲選択
	GS_MAINGAME,		// メインゲーム
	GS_RESULT,			// リザルト
	GS_CREDIT,			// クレジット
	GS_DECIDE,			// 楽曲決定
	GS_READY,			// メインゲームスタート演出
	GS_FINISH,			// メインゲーム終わり演出
	GS_END				// 全てのゲーム処理終了
};

// タイトルセレクト
enum TITLESELECT
{
	TS_STRAT,			// 楽曲選択へ
	TS_CREDIT,			// クレジットへ
	TS_EXIT,			// ゲーム終了
};

// サウンド
enum SOUND
{
	// 0～9をBGM
	S_GAMEMUSIC = 0,
	BGM_TITLE,
	BGM_SELECT,
	BGM_RESULT,
	BGM_DECIDE,
	BGM_NONE,

	// 10～19をSE
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

// BMSデータ情報
typedef struct _BMSGETPATH
{
	char mBmsFilePath[256];		// 譜面データのパス
	char mBmsMusicPath[256];	// 楽曲データのパス
} BMSGETPATH, BMSGETPATH;



class CGame
{
public:
// 関数
	CGame();			// コンストラクタ
	virtual ~CGame();	// デストラクタ
	
	void Run();			// ゲームメインルーチン
	void Destroy();		// 解放

// 変数
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
// 関数
	void InitLoad();										// 初期化&ロード(一度のみ)
	void TitleRun();										// タイトル用実行ループ関数
	void SelectRun();									// 曲選択用実行ループ関数
	void MainGameRun();							// メインゲーム用実行ループ関数
	void ResultRun();									// リザルト用実行ループ関数

	void BmsFileLoad();								// BMSファイルロード関数
	void BmsFileClear();								// BMSファイル消去関数

	void BmsHeaderLoad();							// BMSファイルのヘッダー情報のみロード
	void BmsHeaderClear();							// BMSファイルのヘッダー情報のみクリア
	void GetBMSFilePath();						// BMSファイルパス取得関数

	void MusicDecide();									// 楽曲決定シーン演習
	void Ready();											// メインゲームスタート演習
	void Finish();											// メインゲーム終わり演習

	void AllClear();										// ロード済みデータ全消去

	void SceneMove(GAMESTATE NextScene, float FadeTime, float StartMoveTime, D3DXVECTOR3 FadeColor, SOUND Play, SOUND Stop);
// 変数
	GAMESTATE		GameState;				// ゲームステート
	TITLESELECT		TitleSate;		// タイトル時のステート
	LONGLONG		llStartTime;			// ゲーム開始時の時間(高解像度タイマー)
	LONGLONG		llGlobalFreq;			// マシンの１秒間のカウント数(高解像度タイマー)
	double			dElapsedTime;						// 開始からの経過時間(秒)
	int				iStartNum[256];				// BMS演奏で計算開始する配列番号(処理を軽くするため)
	float			Hi_SPEED;							// 小節間の幅の倍
	char			BMSData[256][256];				// BMSデータ格納[.bms][.wav]
	int				SelectCnt;							// 楽曲ファイル検索回数
	int				MaxMusicCnt;					// 楽曲の最大数
	int				ChangeCnt;							// シーン切り替え時のカウント
	bool			SceneChangeFlg;					// シーンを切り替えるフラグ
	bool			GameStartFlg;
	D3DXVECTOR4		SpriteColor;		// 画像の色
	D3DXVECTOR4		ChangeSceneColor;	// 演出用画像色
	D3DXVECTOR4		Flashing;					// 画像の点滅用
	float			AddFlash;			// 点滅用加減変数
	bool			ResultEffectFlg;	// 曲終了時に表示するエフェクトのフラグ

	float			TitleCurPos;		// タイトル時のカーソルの位置
	float			SelectMixWin;		// 楽曲選択時に曲表示UIを動かす

	LONGLONG		now_count;			// 楽曲のBPMに合わせてスクロールする変数

	BMSGETPATH mBFP[256];				// 楽曲の譜面データと曲のパス格納(256曲まで)

	int MaxGage;						// 楽曲のスコアの最大数
	float AddGage_y;					// 1ノーツ判定時の変動するyのゲージの値
	float Gage_y;						// ゲージの中の画像のyの伸び率

	int Now_Score;						// 現在のスコアをカウントしていく
	int Max_Score;						// Now_Scoreの最大値

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

	bool OnePlayBGM;					// 一度だけ楽曲を流すフラグ

	// モデル
	CD3DXMESH_ANIM* m_pLane;			// レーンモデル
	CD3DXMESH_ANIM* m_pDome;			// ドームモデル
	CD3DXMESH_ANIM* m_pDomeClear;		// クリア時のドームモデル
	CD3DXMESH_ANIM* m_pBar;				// バーモデル
	CD3DXMESH_ANIM* m_pLine[8];			// ラインモデル(4本)*2
	CD3DXMESH_ANIM* m_pNote;			// ノーツモデル
	CD3DXMESH_ANIM* m_pExNote;			// Exノーツモデル
	CD3DXMESH_ANIM* m_pHoldLongNote;	// ホールドノーツモデル
	CD3DXMESH_ANIM* m_pJudgeMark[4];	// 判定マーク

	// テキスト
	D3D11_TEXT* m_pText;				// 文字表示用
	D3D11_TEXT* m_pTextSizeS;

	// テクスチャー
	TD_Graphics* m_pMenuBG[4];			// メニュー背景画像
	TD_Graphics* m_pMusicWindow;		// 楽曲背景画像
	TD_Graphics* m_pMainGameUI[5];		// メインゲームUI
	TD_Graphics* m_pSceneChangeTex;		// シーン切り替え時の画像
	TD_Graphics* m_pGageType[2];		// ゲージ内部の色
	TD_Graphics* m_pTitleSlect;			// タイトル時の選択画像
	TD_Graphics* m_pTutorial;
	TD_Graphics* m_pResult[2];			// リザルトのClear、Failed文字

	// 4キーを使って操作する
	BOOL bOnKey[6];						// キーが押されているか [A]0,[S]1,[:]2,[}]3,[↑]4,[↓]5
	BOOL bOnHold[4];					// キーが押されているか (ホールド用)

	//ライブラリ
	CDSPro81	ds;						// DirectSound管理
	CBmsPro		bms;					// BMSファイル管理

	static D3DXMATRIX mView;
	static D3DXMATRIX mProj;
	D3DXMATRIX mTran;					// 座標用マトリックス
	D3DXMATRIX mRot;					// 回転用マトリックス
	D3DXMATRIX mSca;					// 拡大縮小用マトリックス
};
