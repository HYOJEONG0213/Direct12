// 셰이더 : 일감, 명령어. 외주를 어떤 일을 시킬지 단계별로 묘사함 

cbuffer TRANSFORM_PARAMS : register(b0)
{
	row_major matrix matWVP;
};

cbuffer MATERIAL_PARAMS : register(b1)
{
	int int_0;
	int int_1;
	int int_2;
	int int_3;
	int int_4;
	float float_0;
	float float_1;
	float float_2;
	float float_3;
	float float_4;
};

Texture2D tex_0 : register(t0);
Texture2D tex_1 : register(t1);
Texture2D tex_2 : register(t2);
Texture2D tex_3 : register(t3);
Texture2D tex_4 : register(t4);

// rasterizer 단계에서 uv값 정해준 뒤 픽셸 세이더에서 원하는 색상 골라주는 정책
SamplerState sam_0 : register(s0);

struct VS_IN
{
	float3 pos : POSITION;
	float4 color : COLOR;
	float2 uv : TEXCOORD;
};

struct VS_OUT
{
	float4 pos : SV_Position;
	float4 color : COLOR;
	float2 uv : TEXCOORD;
};

VS_OUT VS_Main(VS_IN input)
{
	VS_OUT output = (VS_OUT) 0;

	// Matrix WVP 적용 : 
	output.pos = mul(float4(input.pos, 1.f), matWVP);
	
	output.color = input.color;
	output.uv = input.uv;

	return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
	float4 color = tex_0.Sample(sam_0, input.uv);
	return color;
}