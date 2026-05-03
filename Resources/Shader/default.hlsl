// 셰이더 : 일감, 명령어. 외주를 어떤 일을 시킬지 단계별로 묘사함 

#ifndef _DEFAULT_HLSL_
#define _DEFAULT_HLSL_

#include "params.hlsli"
#include "utils.hlsli"


struct VS_IN
{
	float3 pos : POSITION;
	float2 uv : TEXCOORD;
	float3 normal : NORMAL;
};

struct VS_OUT
{
	float4 pos : SV_Position;
	float2 uv : TEXCOORD;
	float3 viewPos : POSITION;
	float3 viewNormal : NORMAL;
};

VS_OUT VS_Main(VS_IN input)
{
	VS_OUT output = (VS_OUT) 0;

	// Matrix WVP 적용 : 
	output.pos = mul(float4(input.pos, 1.f), g_matWVP);
	output.uv = input.uv;
	
	output.viewPos = mul(float4(input.pos, 1.f), g_matWV).xyz;
	// 0으로 적용해야 이동이 적용안됌
	output.viewNormal = normalize(mul(float4(input.normal, 0.f), g_matWV).xyz);

	return output;
}

// 픽셸단위 연산 
float4 PS_Main(VS_OUT input) : SV_Target
{
	float4 color = g_tex_0.Sample(g_sam_0, input.uv);
	//float4 color = float4(1.f, 1.f, 1.f, 1.f);
	
	// 각각에 대한 빛의 색상 계산
	LightColor totalColor = (LightColor) 0.f;

	for (int i = 0; i < g_lightCount; ++i)
	{
		LightColor color = CalculateLightColor(i, input.viewNormal, input.viewPos);
		totalColor.diffuse += color.diffuse;
		totalColor.ambient += color.ambient;
		totalColor.specular += color.specular;
	}

	// 빛에 의해 색상 변화
	color.xyz = (totalColor.diffuse.xyz * color.xyz)
        + totalColor.ambient.xyz * color.xyz
        + totalColor.specular.xyz;
	
	return color;
}

#endif