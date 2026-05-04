#ifndef _SKYBOX_HLSLI_
#define _SKYBOX_HLSLI_

#include "params.hlsli"

struct VS_IN
{
	float3 localPos : POSITION;	//로컬 좌표계!
	float2 uv : TEXCOORD;
};

struct VS_OUT
{
	float4 pos : SV_Position;
	float2 uv : TEXCOORD;
};

VS_OUT VS_Main(VS_IN input)
{
	VS_OUT output = (VS_OUT) 0;

	// 1. view space 변환 (World 변환 생략) 
	// w 성분을 0으로 설정해 카메라 이동 무시, Rotation만 적용 
	float4 viewPos = mul(float4(input.localPos, 0), g_matView);	
	
	// 2. Clip 스페이스 변환 (view->projection) 
	float4 clipSpacePos = mul(viewPos, g_matProjection);	
	
	// 3. 깊이 강제 조작 ((x, y, z, w) -> (x, y, w, w)로)
	// 래스터라이저 동차 나누기 과정에서 깊이는 z/w 로 계산됌 
    // w/w=1이기 때문에 항상 깊이가 1로 유지된다 = 최대 깊이 = 화면 가장 뒤쪽에 그려짐 
	output.pos = clipSpacePos.xyww;	
	
	// 4. uv 좌표 전달  
	output.uv = input.uv;

	return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
	float4 color = g_tex_0.Sample(g_sam_0, input.uv);
	return color;
}

#endif