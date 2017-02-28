#pragma once

#include "../Origin/Defines.h"
#include <Windows.h>
#include <fstream>
#include <vector>

using namespace std;

// BMSヘッダ情報
typedef struct _BMSHEADER
{
	long		lPlayer;					// プレイモード
	char		mGenre[256];				// データのジャンル
	char		mTitle[256];				// データのタイトル
	char		mArtist[256];				// データの製作者
	char		mBpm[256];
	char		mPlayLevel[256];
	char		mScore[8];					// 楽曲のスコア


	float		fBpm;						// 初期テンポ(初期値は130)
	char		mMidifile[MAX_PATH];		// バックグラウンドで流すMIDIファイル
	long		lPlaylevel;					// ゲームの難易度
	long		lRank;						// 判定ランク
	long		lWavVol;					// 音量を元の何%にするか
	long		lTotal;						// ゲージの増量
	char		mStagePic[MAX_PATH];		// 曲開始字に表示する画像
	float		fBpmIndex[BMS_MAXBUFFER];	// テンポインデックス(初期値は120)
	int			iMaxNoteCnt;				// 最大ノート数(ゲージ画像の長さ、スコア上限に使用)

	long		lEndBar;					// 終了小節
	long		lMaxCount;					// 最大のカウント数
} BMSHEADER, *LPBMSHEADER;

// ノートタイプ宣言
enum NOTETYPE
{
	Normal = 0,		// ノーマルノート
	Extra,			// エクストラノート
	HoldStart,		// ホールドノート始め 
	HoldFinish,		// ホールドノート終わり
};


// BMSデータ情報
typedef struct _BMSDATA
{
	LONG		lTime;						// このデータの開始位置(BMSカウント値)
	LONG		lData;						// 鳴らすデータ(0x01～0xFF)
	float		fScal;						// ホールドノートのZ軸の長さ
	float		fData;						// 小数値データ(テンポ用)
	NOTETYPE	iType;						// ノートのタイプ
	BOOL		bFlag;						// アプリが使用出来る任意の変数(ここでは判定に利用)
} BMSDATA,*LPBMSDATA;


// 小節情報
typedef struct _BMSBAR
{
	float		fScale;						// この小節の長さ倍率
	LONG		lTime;						// この小節の開始位置(BMSカウント値)
	LONG		lLength;					// この小節の長さ(BMSカウント値)
} BMSBAR, *LPBMSBAR;



////////////////////////////////////////////////////////////////////////////////////
// BMSクラス
////////////////////////////////////////////////////////////////////////////////////
class CBmsPro
{
protected:
	BMSHEADER	mBH;										// BMSヘッダ情報

	LPBMSDATA	pBmsData[BMS_MAXBUFFER];					// 実データ
	int			iBmsData[BMS_MAXBUFFER];					// 実データのそれぞれの数

	char		mWavFile[BMS_MAXBUFFER][MAX_PATH];			// WAVのファイル名
	char		mBmpFile[BMS_MAXBUFFER][MAX_PATH];			// BMPのファイル名

	BMSBAR		mBmsBar[1000+1];							// 小節データ(999小節時に1000番目も小節バーとして使うため+1しておく)

	char		mLastError[1024];							// エラーが発生した場合の文字列

	vector<string> mUpdataBmsData;							// BMSデータ上書き用変数

	//char		UpdataBmsData[1024][1024];					// BMSファイル上書き変数
public:
	CBmsPro();												// コンストラクタ
	virtual ~CBmsPro();										// デストラクタ

	// 基本メソッド
	BOOL Clear( void );										// データの初期化
	BOOL LoadHeader( const char *file );					// BMSヘッダ情報だけを取り出す
	BOOL Load( const char *file );							// BMSファイルのロード
	BOOL Save( const char *file ,int HiScore );				// BMSファイルにセーブ
	BOOL Sort( int ch );									// 指定チャンネルのデータを昇順に並び替える

	BOOL Restart( void );									// リスタート用にオブジェのフラグを元に戻す
	LONG GetCountFromTime( double sec );					// 時間からBMSカウント値を計算

	void GetBmsHeader(char* title, char* artist, char* genre, char* playlevle, char* bpm, char* score);
public:
	// 内部データのアクセス
	inline const char*			GetLastError( void )		{ return mLastError; }				// 最後のエラー文字列

	// ゲーム必須メソッド
	inline LONG					GetMaxCount( void )			{ return mBH.lMaxCount; }			// ゲーム内の最大のカウント値
	inline int					GetBarNum( void )			{ return mBH.lEndBar+1; }			// 小節バーの個数(最後の小節も含むため+1する)
	inline const LPBMSBAR		GetBar( int num )			{ return &mBmsBar[num]; }			// 小節バーのデータ
	inline int					GetObjeNum( int ch )		{ return iBmsData[ch]; }			// 指定チャネルのデータ数を返す
	inline const LPBMSDATA		GetObje( int ch,int num )	{ return &pBmsData[ch][num]; }		// チャネルと配列番号でデータを取得する
	inline const LPBMSHEADER	GetHeader( void )			{ return &mBH; }					// ヘッダ情報を返す
	//inline const char*			GetBmpFile( int num )		{ return mBmpFile[num]; }			// 使用しているBMPファイル名
	//inline const char*			GetWavFile( int num )		{ return mWavFile[num]; }			// 使用しているWAVファイル名
	inline int					GetMaxNoteCnt( void )		{ return mBH.iMaxNoteCnt; }			// ノートの最大値を返す

private:
	// 非公開関数
	BOOL itoa1016( int num,char *dst,int keta=-1 );			// 10進数を桁付きの16進数文字へ変換
	BOOL AddData( int ch,LONG cnt,LONG data, float scale);	// 1つのデータを追加（ソートはされない）
	int	 GetCommand( const char *s );						// コマンド番号を返す
	BOOL GetCommandString( const char *src,char *dst );		// パラメータ文字列を取得
	BOOL LoadBmsData( const char *file );					// BMSデータの読み込み
	BOOL LineCompact( const char *src,char *dst );			// データを最適化して返す
};