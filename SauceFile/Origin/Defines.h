#pragma once

// マクロ定義
#define SAFE_DELETE(x) if(x){delete x; x=0;}
#define SAFE_RELEASE(x) if(x){x->Release(); x=NULL;}
#define SAFE_DELETE_ARRAY(p){ if(p){ delete[] (p);(p)=NULL;}}

// ウィンドウ定数定義
#define WINDOW_WIDTH		1920				// ウィンドウ幅
#define WINDOW_HEIGHT		1080				// ウィンドウ高さ
#define APP_NAME			L"BEATs"		// ウィンドウ名

// テキスト定義
#define DIMENSION			48.0f
#define WDIMENSION			40.0f
#define TEX_DIMENSION		512.00f

#define BMS_RESOLUTION		9600			// 1小節のカウント値
#define BMS_MAXBUFFER		(16 * 16)		// 00～FFまでのバッファ数

// BMSチャンネル定義
#define BMS_BACKMUSIC		0x01			// その位置にきたら、自動的に再生されるWAVを指定します
#define BMS_STRETCH			0x02			// その小節の長さを定義したデータ倍します（１０進数、小数ともに使用可）
#define BMS_TEMPO			0x03			// 再生テンポ（ＢＰＭ / １分間の四分音符数）の途中変更（１６進数）
#define BMS_BACKANIME		0x04			// バックグラウンドアニメーション機能
#define BMS_EXTENEDOBJ		0x05			// 落下してくるオブジェを別のキャラクターナンバーのものにすり替える機能
#define BMS_CHGPOORANI		0x06			// POORを出したときに表示される画像を変更
#define BMS_LAYERANIME		0x07			// Ch.04で指定したBGAの上にかぶせるBMPを指定できます
#define BMS_BPMINDEX		0x08			// BPMのインデックス指定(新)

// 操作キー定義
#define KYE_DOWN			0x53			//「S」
#define KYE_UP				0x44			//「D」
#define KYE_CANCEL			0x4B			//「K」
#define KYE_DECIDE			0x4C			//「L」
