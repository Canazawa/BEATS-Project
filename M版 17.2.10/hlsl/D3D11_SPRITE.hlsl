//グローバル

Texture2D g_texColor: register(t0);//テクスチャーは レジスターt(n)
SamplerState g_samLinear : register(s0);//サンプラーはレジスターs(n)

cbuffer global
{
	matrix g_mWVP; //ワールドから射影までの変換行列
	float4 g_vDiffuse;//ディフューズ色
	float4 g_fAlpha;
};

//構造体
struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD;
};

cbuffer Rectangle
{
	float Rect_w;
	float Rect_h;
	float Rect_l;
	float Rect_t;
};

//
//バーテックスシェーダー
//
VS_OUTPUT VS( float4 Pos : POSITION ,float2 Tex : TEXCOORD )
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.Pos = mul(Pos,g_mWVP);
	output.Tex = Tex;// * float2(Rect_w, Rect_h) + float2(Rect_l, Rect_t);

    return output;
}

//
//ピクセルシェーダー
//
float4 PS( VS_OUTPUT input  ) : SV_Target
{
	float4 color = (g_texColor.Sample(g_samLinear, input.Tex))*g_fAlpha.x;

	clip(color.w - 0.5);

	return color;
}