#pragma warning( disable : 4996 )
#include "CBmsPro.h"
#include <stdio.h>

#define DEBUGMODE
#include "../DEBUG/DEBUG.H"

#define SAFE_FREE(x)		{ if(x) { free(x); x=NULL; } }

////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////
CBmsPro::CBmsPro( void )
{
	ZeroMemory( &mBH,sizeof(mBH) );
	ZeroMemory( &pBmsData,sizeof(pBmsData) );
	ZeroMemory( &iBmsData,sizeof(iBmsData) );
	ZeroMemory( &mWavFile,sizeof(mWavFile) );
	ZeroMemory( &mBmpFile,sizeof(mBmpFile) );
	ZeroMemory( &mBmsBar,sizeof(mBmsBar) );
	ZeroMemory( &mLastError,sizeof(mLastError) );

	// デフォルト値セット
	Clear();
}

////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////
CBmsPro::~CBmsPro()
{
	Clear();
}

////////////////////////////////////////////
// データの初期化
////////////////////////////////////////////
BOOL CBmsPro::Clear( void )
{
	int i;

	// ヘッダ初期化
	ZeroMemory( &mBH,sizeof(mBH) );
	mBH.lPlayer	= 1;
	mBH.fBpm	= 130;
	for (i = 0; i < BMS_MAXBUFFER; i++)
	{
		mBH.fBpmIndex[i] = 120.0f;
	}

	// 実データ初期化
	for (i = 0; i < BMS_MAXBUFFER; i++)
	{
		SAFE_FREE( pBmsData[i] );		// BMSデータ領域をクリア
		iBmsData[i] = 0;				// データの数をクリア
	}

	// ファイル名
	ZeroMemory( &mWavFile,sizeof(mWavFile) );
	ZeroMemory( &mBmpFile,sizeof(mBmpFile) );

	// 小節の長さを1.0で初期化
	ZeroMemory(&mBmsBar, sizeof(mBmsBar));
	for (int i = 0; i < 1001; i++)
	{
		mBmsBar[i].fScale = 1.0f;
	}

	// 上書きするBMSデータを初期化
	mUpdataBmsData.clear();

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////
// ヘッダ情報だけを取り出す
////////////////////////////////////////////////////////////////////////////////////////
BOOL CBmsPro::LoadHeader( const char *file )
{
	Clear();

	FILE *fp;
	fp = fopen( file,"r" );
	if( !fp ) {
		DEBUG( "ファイルが見つからない : %s\n",file );
		sprintf( mLastError,"[%s] ファイルが見つからない",file );
		return FALSE;
	}

	char buf[1024];
	char tmp[4];
	int num;
	int ch;
	int line;

	while(1)
	{
		// 1行を読みこむ
		ZeroMemory( buf,1024 );
		fgets( buf,1024,fp );
		if( buf[0]==NULL && feof(fp) )	// ファイルの終端なら検索終わり
			break;

		// コマンド以外なら飛ばす
		if( buf[0]!='#' )
			continue;

		// 最後の改行を消去
		if( buf[strlen(buf)-1]=='\n' )
			buf[strlen(buf)-1] = NULL;

		// コマンドの解析
		int cmd = GetCommand( buf );

		// 不明なコマンドならスキップ
		if( cmd<=-2 )
		{
			DEBUG( "不明なコマンド : %s\n",buf );
			continue;
		}

		// パラメータの分割
		char str[1024];
		ZeroMemory( str,1024 );
		if (!GetCommandString(buf, str))
		{
			// 文字列の取得が失敗なら
			fclose(fp);
			sprintf( mLastError,"[%s] パラメータの分割エラー",buf );
			return FALSE;
		}

		// パラメータの代入
		switch( cmd )
		{
		case 0:		// PLAYER
			mBH.lPlayer = atoi( str );
			//DEBUG( "PLAYER       [%d]\n",mBH.lPlayer );
			break;
		case 1:		// GENRE
			strcpy( mBH.mGenre,str );
			//DEBUG( "ジャンル     [%s]\n",mBH.mGenre );
			break;
		case 2:		// TITLE
			strcpy( mBH.mTitle,str );
			//DEBUG("タイトル     [%s]\n", mBH.mTitle);
			break;
		case 3:		// ARTIST
			strcpy( mBH.mArtist,str );
			//DEBUG( "アーティスト [%s]\n",mBH.mArtist );
			break;
		case 4:		// BPM
			if (buf[4] == ' ' || buf[4] == 0x09)
			{
				// 基本コマンドなら
				strcpy(mBH.mBpm, str);
				mBH.fBpm = (float)atof( str );
				AddData(BMS_TEMPO, 0, (LONG)mBH.fBpm, 0);
				//DEBUG("初期BPM      [%.2f]\n", mBH.fBpm);
			}
			else
			{
				// 拡張コマンドなら
				ZeroMemory( tmp,sizeof(tmp) );
				tmp[0] = buf[4];
				tmp[1] = buf[5];
				tmp[2] = NULL;
				ch = strtol( tmp,NULL,16 );	// 16進数
				mBH.fBpmIndex[ch] = (float)atof( str );
				DEBUG( "インデックスBPM : 0x%02X [%f]\n",ch, mBH.fBpmIndex[ch] );
			}
			break;
		case 5:		// MIDIFILE
			strcpy( mBH.mMidifile,str );
			break;
		case 6:		// PLAYLEVEL
			strcpy(mBH.mPlayLevel, str);
			mBH.lPlaylevel = atoi( str );
			break;
		case 7:		// RANK
			mBH.lRank	= atoi( str );
			break;
		case 8:		// VOLWAV
			mBH.lWavVol	= atoi( str );
			break;
		case 9:		// TOTAL
			mBH.lTotal	= atoi( str );
			break;
		case 10:	// StageFile
			strcpy( mBH.mStagePic,str );
			break;
		case 11:	// WAV
			ZeroMemory( tmp,sizeof(tmp) );
			tmp[0] = buf[4];
			tmp[1] = buf[5];
			num = strtol(tmp, NULL, 16);			// 16進数
			strcpy( mWavFile[num],str );
			DEBUG( "WAV%02X : %s\n",num,mWavFile[num] );
			break;
		case 12:	// BMP
			ZeroMemory( tmp,sizeof(tmp) );
			tmp[0] = buf[4];
			tmp[1] = buf[5];
			num = strtol( tmp,NULL,16 );			// 16進数
			strcpy( mBmpFile[num],str );
			DEBUG( "BMP%02X : %s\n",num,mBmpFile[num] );
			break;
		case 13:	// SCORE
			ZeroMemory(tmp, sizeof(tmp));
			strcpy(mBH.mScore, str);
			break;
		default:
			// 小節番号の取得
			ZeroMemory( tmp,sizeof(tmp) );
			memcpy( tmp,buf+1,3 );
			line = atoi( tmp );			// 10進数
			// チャンネル番号の取得
			ZeroMemory( tmp,sizeof(tmp) );
			tmp[0] = buf[4];
			tmp[1] = buf[5];
			ch = strtol(tmp, NULL, 16);		// 16進数
			if (ch == BMS_STRETCH)
			{
				// 小節の倍率変更命令の場合
				mBmsBar[line].fScale = (float)atof( str );
				DEBUG("小節倍率変更 : %f倍\n", mBmsBar[line].fScale);
			}
			// 小節番号の最大値を記憶する
			if (mBH.lEndBar < line)
				mBH.lEndBar = line;
			break;
		}
	}

	// 最後の小節内にもデータが存在するため、その次の小節を終端小節とする
	mBH.lEndBar++;

	// 小節倍率データを元に全ての小節情報を算出
	LONG cnt = 0;	// 現在の小節の開始カウント値
	int i;
	for (i = 0; i <= mBH.lEndBar; i++)
	{
		// 小節リストを加算
		mBmsBar[i].lTime	= cnt;												// 現在の小節の開始カウントを記録
		mBmsBar[i].lLength	= (LONG)(BMS_RESOLUTION * mBmsBar[i].fScale);		// 倍率からこの小節の長さカウント値を算出
		
		// この小節のカウント数を加算して次の小節の開始カウントとする
		cnt += mBmsBar[i].lLength;
	}

	// 最大カウントを保存
	mBH.lMaxCount = cnt;

	fclose( fp );

	return TRUE;
}

////////////////////////////////////////////
// データロード
////////////////////////////////////////////
BOOL CBmsPro::Load( const char *file )
{
	// ヘッダ＆小節倍率の読み込み
	if( !LoadHeader(file) )
	{
		DEBUG( "ヘッダ読み取りエラー\n" );
		return FALSE;
	}
	//DEBUG( "HEADER OK\n" );
	// 実データの読み込み
	if( !LoadBmsData(file) )
	{
		DEBUG("データ読み込みエラー\n");
		return FALSE;
	}
	//DEBUG("DATA OK\n");
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////
// BMSファイルにセーブ
////////////////////////////////////////////////////////////////////////////////////////
BOOL CBmsPro::Save(const char *file, int HiScore)
{
	char* s0 = "#SCORE";
	FILE *fp;
	bool CheckFlag = false;//文字列が合致しているか

	fp = fopen(file, "wb");
	if (!fp)
	{
		sprintf(mLastError, "[%s] ファイル書き込みエラー", file);
		return FALSE;
	}

	//// ヘッダの書き出し





	fprintf(fp, "#SCORE %d\n", HiScore); // スコア記録


	fclose(fp);
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////
// 指定チャンネルのデータを昇順に並び替える
////////////////////////////////////////////////////////////////////////////////////////
BOOL CBmsPro::Sort( int ch )
{
	if( ch<0 || ch>BMS_MAXBUFFER-1 )
		return FALSE;

	// 昇順に並び替える
	int i,j;
	for (i = 0; i < iBmsData[ch] - 1; i++)
	{
		for (j = i + 1; j < iBmsData[ch]; j++)
		{
			if (pBmsData[ch][i].lTime > pBmsData[ch][j].lTime)
			{
				// 構造体を入れ替える
				BMSDATA dmy		= pBmsData[ch][i];		// ダミーに保存
				pBmsData[ch][i] = pBmsData[ch][j];		// iにjを入れる
				pBmsData[ch][j] = dmy;					// jにダミーを入れる
			}
		}
	}
	return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////
// リスタート用にオブジェのフラグを元に戻す
////////////////////////////////////////////////////////////////////////////////////////
BOOL CBmsPro::Restart( void )
{
	int i,j;
	for (j = 0; j < BMS_MAXBUFFER; j++)
	{
		for (i = 0; i < iBmsData[j]; i++)
		{
			pBmsData[j][i].bFlag = TRUE;
		}
	}
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////
// 時間からBMSカウント値を計算
////////////////////////////////////////////////////////////////////////////////////////
LONG CBmsPro::GetCountFromTime( double sec )
{
	LONG cnt = 0;			// BMSカウント
	double t = 0;			// BMS上の時間
	double bpm = 130;
	
	if (iBmsData[BMS_TEMPO] > 0)
	{
		bpm = pBmsData[BMS_TEMPO][0].fData;		// 初期BPM
	}

	if (sec < 0)
		return 0;

//	DEBUG( "■指定時間 (%.2f秒)\n",sec );

	// 指定時間を越えるまでタイムを加算
	int i;
	for (i = 0; i < iBmsData[BMS_TEMPO]; i++)
	{
		// １つ前の時間と新しい時間との経過時間から秒を算出
		double add = (double)( pBmsData[BMS_TEMPO][i].lTime - cnt ) / ( bpm / 60 ) / (BMS_RESOLUTION / 4);
//		DEBUG( "  [%d] 経過時間 %f秒\n",i,t+add );

		// 現在のテンポ値で時間が過ぎたら抜ける
		if (t + add > sec)
		{
//			DEBUG( "                  └時間が過ぎた\n" );
			break;
		}

		t += add;										// 経過時間を加算
		bpm = (double)pBmsData[BMS_TEMPO][i].fData;		// 次のBPMをセット
		cnt = pBmsData[BMS_TEMPO][i].lTime;				// 計算済みのカウントをセット
	}

//	DEBUG( "  BPM %f\n",bpm );
//	DEBUG( "  CNT %I64d\n",cnt );

	// 指定時間と1つ前までの時間の差分
	double sub = sec - t;
//	DEBUG( "  差分 %f秒\n",sub );

	// 差分からBMSカウント数を算出
	LONG cnt2 = (LONG)(sub * (BMS_RESOLUTION / 4) * ( bpm / 60 ));

	// BMSカウント値に加算
	cnt += cnt2;
//	DEBUG( "  結果のカウント値 %I64d\n",cnt );

	return cnt;
}






////////////////////////////////////////////////////////////////////////////////////////
// 10進数を桁付きの16進数文字へ変換
////////////////////////////////////////////////////////////////////////////////////////
BOOL CBmsPro::itoa1016(int num, char *dst, int keta)
{
	if (num < 0)
		return FALSE;

	itoa(num, dst, 16);

	// 小文字チェック
	int i;
	for (i = 0; i < (int)strlen(dst); i++)
	{
		if (dst[i] >= 'a' && dst[i] <= 'z')
		{
			dst[i] -= 0x20;
		}
	}

	// 桁数指定の場合
	if (keta >= 0)
	{
		// 桁数が足りない場合
		if ((int)strlen(dst) < keta)
		{
			char dmy[1024];
			ZeroMemory(dmy, sizeof(dmy));
			for (i = 0; i < keta - (int)strlen(dst); i++)
			{
				dmy[i] = '0';			// 先頭に足りない分の0を追加
			}
			strcpy(&dmy[i], dst);		// その次に算出された文字列をつなげる
			strcpy(dst, dmy);			// 出力結果にダミー文字列をコピー		
		}
	}

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////
// 1つのデータを追加
////////////////////////////////////////////////////////////////////////////////////////
BOOL CBmsPro::AddData(int ch, LONG cnt, LONG data, float scale)
{
	// チャンネル番号をチェック
	if (ch < 0 || ch > 255)
		return FALSE;

	// 小節長変更コマンドなら何もしない
	if (ch == BMS_STRETCH)
		return FALSE;

	// データが無ければ何もしない
	if (data == 0)
		return TRUE;

	switch( ch )
	{
	case BMS_BPMINDEX:
		// BPMのインデックス指定(新)
		iBmsData[BMS_TEMPO]++;
		pBmsData[BMS_TEMPO] = (LPBMSDATA)realloc( pBmsData[BMS_TEMPO],iBmsData[BMS_TEMPO]*sizeof(BMSDATA) );
		ZeroMemory( &pBmsData[BMS_TEMPO][ iBmsData[BMS_TEMPO]-1 ],sizeof(BMSDATA) );	// 追加した配列をクリア
		pBmsData[BMS_TEMPO][iBmsData[BMS_TEMPO]-1].bFlag = TRUE;
		pBmsData[BMS_TEMPO][iBmsData[BMS_TEMPO]-1].lTime = cnt;
		pBmsData[BMS_TEMPO][iBmsData[BMS_TEMPO]-1].lData = (LONG)mBH.fBpmIndex[data];	// テンポリストに入っているテンポ値を登録(LONG型にも保存)
		pBmsData[BMS_TEMPO][iBmsData[BMS_TEMPO]-1].fData = mBH.fBpmIndex[data];			// テンポリストに入っているテンポ値を登録
		break;
	default:
		// データを追加
		iBmsData[ch]++;
		pBmsData[ch] = (LPBMSDATA)realloc( pBmsData[ch],iBmsData[ch]*sizeof(BMSDATA) );
		ZeroMemory( &pBmsData[ch][ iBmsData[ch]-1 ],sizeof(BMSDATA) );					// 追加した配列をクリア
		pBmsData[ch][iBmsData[ch]-1].bFlag = TRUE;
		pBmsData[ch][iBmsData[ch]-1].lTime = cnt;
		pBmsData[ch][iBmsData[ch]-1].lData = data;
		pBmsData[ch][iBmsData[ch]-1].fData = (float)data;								// float型にも保存
		
		mBH.iMaxNoteCnt++;																// ノーツの最大値をカウント

		// ノートタイプ設定
		switch (pBmsData[ch][iBmsData[ch] - 1].lData)
		{
		case (LONG)0x12: // エクストラノート
			pBmsData[ch][iBmsData[ch] - 1].iType = Extra;
			break;
		case (LONG)0x13: // ホールドスタート
			pBmsData[ch][iBmsData[ch] - 1].iType = HoldStart;
			break;
		case (LONG)0x14: // ホールドストップ
			pBmsData[ch][iBmsData[ch] - 1].iType = HoldFinish;
			pBmsData[ch][iBmsData[ch] - 1].fScal = scale;								// ホールドノートの長さを代入
			break;
		default:		 // 対応数値以外はノーマルノート
			pBmsData[ch][iBmsData[ch] - 1].iType = Normal;
			break;
		}
		break;
	}

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////
// コマンド番号を返す
// 戻り値
//		0以上 : コマンド番号
//		-1    : オブジェ配置データ
//		-2    : 不明なコマンド
////////////////////////////////////////////////////////////////////////////////////////
int CBmsPro::GetCommand( const char *s )
{
	static const char *command[14] =
	{
		"PLAYER",
		"GENRE",
		"TITLE",
		"ARTIST",
		"BPM",
		"MIDIFILE",
		"PLAYLEVEL",
		"RANK",
		"VOLWAV",
		"TOTAL",
		"StageFile",
		"WAV",
		"BMP",
		"SCORE",
	};

	// 検索ルーチン
	int i;
	for (i = 0; i < 14; i++)
	{
		if( strnicmp(s+1,command[i],strlen(command[i])) == 0)
			return i;	// コマンドならその番号を返す
	}

	// 先頭が'#nnncc'形式か
	BOOL obj = TRUE;
	for (i = 0; i < 5; i++)
	{
		if (s[i + 1]<'0' || s[i + 1]>'9')
		{
			obj = FALSE;
			break;
		}
	}

	// オブジェ配置なら -1
	if( obj ) {
		return -1;
	}

	// 処理不可能文字列なら
	return -2;
}

////////////////////////////////////////////////////////////////////////////////////////
// コマンドの文字列部分を返す（'\n'は削除 ':'も区切りとして処理）
////////////////////////////////////////////////////////////////////////////////////////
BOOL CBmsPro::GetCommandString( const char *src,char *dst )
{
	int i = 0;
	int j = 0;

	// まずソースデータからデータ部分までのポインタを算出
	while(1)
	{
		if( src[i]==' ' || src[i]==0x09 || src[i]==':' ) {
			i++;
			break;
		}
		if( src[i]=='\n' || src[i]==NULL ) {
			return FALSE;
		}
		i++;
	}

	// 終端までをコピー
	while(1)
	{
		if( src[i]=='\n' || src[i]==NULL )
			break;
		dst[j] = src[i];
		i++;
		j++;
	}
	dst[j] = NULL;
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////
// BMSデータの読み込み
////////////////////////////////////////////////////////////////////////////////////////
BOOL CBmsPro::LoadBmsData( const char *file )
{
	int i;
	char data[1024];
    int cmd;			// コマンド番号
	int line;			// 現在の小節番号
	int ch;				// 現在のチャンネル番号
    int len;			// 文字列の長さ
	bool nowhold[4];	// ホールドスタートに入っているか
	float sclcnt[4][8];	// ホールドノートのZ軸をカウント
	int j[4];
	int BMSDataStrCnt = 0;	// BMSデータの行をカウント

	FILE *fp;
	fp = fopen( file,"r" );
	if (!fp)
	{
		sprintf( mLastError,"[%s] ファイルオープンエラー",file );
		return FALSE;
	}

	ZeroMemory(sclcnt, 4);
	for (int i = 0; i < 4; i++)
	{
		nowhold[i] = false;
		for (int j = 0; j < 8; j++) { sclcnt[i][j] = 0.0f; }
	}

	char buf[1024];
	while(1)
	{
		// 1行を読みこむ
		ZeroMemory( buf,1024 );
		fgets( buf,1024,fp );
		if (buf[0] == NULL && feof(fp))	// ファイルの終端なら検索終わり
			break;

		// BMSデータを保存
		mUpdataBmsData.push_back(buf);
		//strcpy(mUpdataBmsData[BMSDataStrCnt], buf);
		BMSDataStrCnt++;

//		DEBUG( "%s",buf );

		// コマンド以外なら次の行へ
		if( buf[0]!='#' )
			continue;

		// 最後の改行を消去
		if( buf[strlen(buf)-1]=='\n' )
			buf[strlen(buf)-1] = NULL;

		// コマンドの種類を取得
		cmd = GetCommand( buf );

		// オブジェ以外は全て無視
		if( cmd!=-1 )
			continue;

		// パラメータ文字列を取得
		ZeroMemory( data,1024 );
		if (!GetCommandString(buf, data))
		{
			DEBUG( "パラメータ文字列取得エラー\n" );
			fclose(fp);
			sprintf( mLastError,"[%s] パラメータ文字列取得エラー",buf );
			return FALSE;
		}
		
		// データであれば解析

		// チャンネル番号の取得
		char tmp[4];						// 汎用バッファ
		ZeroMemory(&tmp, sizeof(tmp));
		tmp[0] = buf[4];					// チャンネル番号
		tmp[1] = buf[5];					// #001xx:******* のxx部分
		ch = strtol(tmp, NULL, 16);			// 16進数

		// 小節の倍率変更命令の場合はキャンセル
		if (ch == BMS_STRETCH)
			continue;

		// 小節番号を取得
		ZeroMemory(&tmp, sizeof(tmp));
		memcpy(tmp, buf + 1, 3);	// 小節部分を取得
		line = atoi(tmp);			// 数字化

		// データが存在するかチェック
		if (strlen(data) < 1)
		{
			DEBUG( "データが定義されていない箇所が存在\n%s",buf );
			continue;
		}

		// データが偶数かチェック
		if (strlen(data) % 2 == 1)
		{
			DEBUG( "データが偶数個ではない [%s]\n",buf );
			fclose( fp );
			sprintf( mLastError,"[%s] データが偶数個ではない",buf );
			return FALSE;
		}

		// データ数
		len = (int)strlen( data ) / 2;

		if (nowhold[ch - 17] == false)
			j[ch - 17] = 0; // ホールドを最初のスケールにリセット

		// ホールド用に変更
		for (int i = 0; i < data[i]; i += 2)
		{
			// 一つのデータを代入
			ZeroMemory(&tmp, sizeof(tmp));
			tmp[0] = data[i];
			tmp[1] = data[i + 1];

			// 小節にまたがってホールドの終わりが来た場合
			if (tmp[0] == '1' && tmp[1] == '4' || nowhold[ch - 17] == true)
			{
				int srh = 0;
				while (1)
				{
					i += 2;
					// 次のデータがホールド終わりか文字列以上なら抜ける
					if (data[srh] == '1' && data[srh + 1] == '4' || data[i-2] < i)
					{
						j[ch - 17]++; // 次のホールドの長さに変更
						i+2; // 次の数値をみるため+2
						nowhold[ch - 17] = false; // ホールドの終わりなのでフラグを折る
						break;
					}
					srh += 2; // ホールドの終わりの前にあるデータをカウントする
					sclcnt[ch - 17][j[ch - 17]]++; // ホールドノートのZ軸をカウント
				}
			}
			
			// ホールドスタートならば
			if (tmp[0] == '1' && tmp[1] == '3')
			{
				while (1)
				{
					i += 2;
					sclcnt[ch - 17][j[ch - 17]]++; // ホールドノートのZ軸をカウント
					// 次のデータがホールド終わりか文字列以上なら抜ける
					if (data[i] == '1' && data[i + 1] == '4')
					{
						j[ch - 17]++; // 次のホールドの長さに変更
						break;
					}
					if (data[i] < i)
					{
						nowhold[ch - 17] = true; // ホールドの始めの中にいる
						break;
					}
				}
			}
		}

		// 現在の小節のカウント値から1音符分のカウント値を算出
		LONG tick = mBmsBar[line].lLength / len;

		if(nowhold[ch - 17] == false)
			j[ch - 17] = 0; // ホールドを最初のスケールにリセット

		// 実データを追加
		ZeroMemory(&tmp, sizeof(tmp));
		for (i = 0; i < len; i++)
		{
			tmp[0] = data[i * 2];
			tmp[1] = data[i * 2 + 1];
			int data;			// 16進数
			data = strtol(tmp, NULL, 16);
			if (data > 0)
			{
				// データが存在する場合
				AddData(ch, mBmsBar[line].lTime + (tick * i), data, sclcnt[ch - 17][j[ch - 17]]);
			}
			if (tmp[0] == '1' && tmp[1] == '4') // ホールドノートの終わりなら次のホールドノートの長さに変更
				j[ch - 17]++;
		}

		// ホールドの長さを初期化
		for (int i = 0; i < len; i++)
		{
			// 一つのデータを代入
			ZeroMemory(&tmp, sizeof(tmp));
			tmp[0] = data[i * 2];
			tmp[1] = data[i * 2 + 1];

			// ホールドタイプならば
			if (tmp[0] == '1' && tmp[1] == '4' && nowhold[ch - 17] == false)
			{
				for (int i = 0; i < 8; i++) { sclcnt[ch - 17][i] = 0.0f; }// ホールドノートのZ軸をリセット
			}
		}

	}

	fclose( fp );

	// ソート
	for (i = 0; i < BMS_MAXBUFFER; i++)
		Sort( i );

/*	for( i=0;i<iBmsData[BMS_TEMPO];i++ ) {
		char s[256];
		sprintf( s,"[%08d] %f",(int)pBmsData[BMS_TEMPO][i].lTime,pBmsData[BMS_TEMPO][i].fData );
		DEBUG( "%s\n",s )
	}/**/

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////
// データを最適化して返す
////////////////////////////////////////////////////////////////////////////////////////
BOOL CBmsPro::LineCompact( const char *src,char *dst )
{
	int i,j,k;
	char buf[1024];		// ワークバッファ
	strcpy( buf,src );	// 元データをコピー
	*dst = NULL;		// 出力先初期化

	// 010000020000030000040000010000020000030000040000010000020000030000040000010000020000030000040000
	//   ^^  ^^  ^^  ^^  ^^  ^^  ^^  ^^  ^^  ^^  ^^  ^^  ^^  ^^  ^^  ^^  ^^  ^^  ^^  ^^  ^^  ^^  ^^  ^^  i=1  (i+1)=2で割り切れる文字数か→○
	//   ^^^^  ^^^^  ^^^^  ^^^^  ^^^^  ^^^^  ^^^^  ^^^^  ^^^^  ^^^^  ^^^^  ^^^^  ^^^^  ^^^^  ^^^^  ^^^^  i=2  (i+1)=3で割り切れる文字数か→○
	//   ^^^^^^  ^^^^^^  ^^^^^^  ^^^^^^  ^^^^^^  ^^^^^^  ^^^^^^  ^^^^^^  ^^^^^^  ^^^^^^  ^^^^^^  ^^^^^^  i=3  (i+1)=4で割り切れる文字数か→○
	//   ^^^^^^^^  ^^^^^^^^  ^^^^^^^^  ^^^^^^^^  ^^^^^^^^  ^^^^^^^^  ^^^^^^^^  ^^^^^^^^  ^^^^^^^^  ^^^^  i=4  (i+1)=5で割り切れる文字数か→×
	//   ^^^^^^^^^^  ^^^^^^^^^^  ^^^^^^^^^^  ^^^^^^^^^^  ^^^^^^^^^^  ^^^^^^^^^^  ^^^^^^^^^^  ^^^^^^^^^^  i=5  (i+1)=6で割り切れる文字数か→○
	//   ^^^^^^^^^^^^  ^^^^^^^^^^^^  ^^^^^^^^^^^^  ^^^^^^^^^^^^  ^^^^^^^^^^^^  ^^^^^^^^^^^^  ^^^^^^^^^^  i=6  (i+1)=7で割り切れる文字数か→×
	//                                              ：
	//   ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^      i=45 (i+1)=46で割り切れる文字数か→×
	//   ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^    i=46 (i+1)=47で割り切れる文字数か→×
	//   ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^  i=47 (i+1)=48で割り切れる文字数か→○
	while(1)
	{

		// データ数を算出
		int l = (int)strlen(buf) / 2;

		// 空文字ならデータ無し
		if (l == 0)
		{
			return FALSE;
		}

		// 1個のデータなら
		if (l == 1)
		{
			if (buf[0] == '0' && buf[1] == '0')
			{
				// 00ならデータ無し
				return FALSE;
			}
			// それ以外なら確定
			break;
		}


		// 2個以上なら最適化処理
		BOOL div = FALSE;					// 分割出来たか
		for (i = 1; i < l; i++)
		{

			// 分割する文字数
			int fetch_size = 1 + i;

			// 割り切れるか
			if ((l%fetch_size) != 0)
				continue;					// 割り切れないならスキップ

			// 分割文字数内の2番目以降に00以外が含まれるか
			BOOL zero = TRUE;		// 00以外が存在するか
			for( j=0;j<l;j+=fetch_size ) {
				for( k=1;k<fetch_size;k++ ) {
					int p = (j + k) * 2;
					if( buf[p]!='0' || buf[p+1]!='0' ) {
						// 00以外が存在するなら
						zero = FALSE;
						j = l;				// 上位ループも終わらせる
						break;
					}
				}
			}
			
			// 00のみなら現座時の分割サイズで分割
			if( zero ) {
				k = 1;
				for( j=0+fetch_size;j<l;j+=fetch_size ) {			// 1文字目は常に確定なので2つ目からコピー
					int src = j * 2;
					int dst = k * 2;
					buf[k*2+0] = buf[j*2+0];
					buf[k*2+1] = buf[j*2+1];
					// 次のコピー先へ
					k++;
				}
				buf[k*2] = NULL;
				l = k;
				div = TRUE;

//				DEBUG( "分割後 : %s\n",buf );
				break;
			}
		}

		// これ以上分割出来なかったら終わる
		if( !div )
			break;
	}

	// 結果を返す
	strcpy( dst,buf );

	return TRUE;
}

void CBmsPro::GetBmsHeader(char* title, char* artist, char* genre, char* playlevle, char* bpm, char* score)
{
	strcpy(title, mBH.mTitle);
	strcpy(artist, mBH.mArtist);
	strcpy(genre, mBH.mGenre);
	strcpy(playlevle, mBH.mPlayLevel);
	strcpy(bpm, mBH.mBpm);
	strcpy(score, mBH.mScore);
}