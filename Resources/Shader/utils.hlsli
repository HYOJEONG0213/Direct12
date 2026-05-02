#ifndef _UTILS_HLSLI_
#define _UTILS_HLSLI_

#include "params.hlsli"

LightColor CalculateLightColor(int lightIndex, float3 viewNormal, float3 viewPos)
{
	LightColor color = (LightColor) 0.f;

	float3 viewLightDir = (float3) 0.f;

	float diffuseRatio = 0.f;
	float specularRatio = 0.f;
	float distanceRatio = 1.f;

	if (g_light[lightIndex].lightType == 0)
	{
        // Directional Light
		// 월드기준 빛의 방향을 뷰 기준으로 변환
		viewLightDir = normalize(mul(float4(g_light[lightIndex].direction.xyz, 0.f), g_matView).xyz);	// = L(빛의 방향)
		diffuseRatio = saturate(dot(-viewLightDir, viewNormal)); // L과 N내적후 cos값 나온뒤 saturate로 0~1사이로 보정
	}
	else if (g_light[lightIndex].lightType == 1)
	{
        // Point Light : 광원의 위치가 중요하다! 최대거리 필요! 
		float3 viewLightPos = mul(float4(g_light[lightIndex].position.xyz, 1.f), g_matView).xyz;	// 물체의 뷰포지션
		viewLightDir = normalize(viewPos - viewLightPos);
		diffuseRatio = saturate(dot(-viewLightDir, viewNormal));

		// 멀어지면 약해져야함
		float dist = distance(viewPos, viewLightPos);
		if (g_light[lightIndex].range == 0.f)
			distanceRatio = 0.f;
		else
			distanceRatio = saturate(1.f - pow(dist / g_light[lightIndex].range, 100));
	}
	else
	{
        // Spot Light : d 체크, 각도 체크 필요 
		float3 viewLightPos = mul(float4(g_light[lightIndex].position.xyz, 1.f), g_matView).xyz;
		viewLightDir = normalize(viewPos - viewLightPos); // 빛에서 픽셀로 향하는 방향(정규화)
		diffuseRatio = saturate(dot(-viewLightDir, viewNormal));	

		if (g_light[lightIndex].range == 0.f)	//빛x
			distanceRatio = 0.f;
		else
		{
			float halfAngle = g_light[lightIndex].angle / 2;

			float3 viewLightVec = viewPos - viewLightPos; // 빛에서 픽셀로 향하는 방향벡터(크기 포함) 
			//빛이 나가는 방향 뷰좌표로 변환, 방향벡터이므로 w=0
			float3 viewCenterLightDir = normalize(mul(float4(g_light[lightIndex].direction.xyz, 0.f), g_matView).xyz);	

			float centerDist = dot(viewLightVec, viewCenterLightDir);
			distanceRatio = saturate(1.f - centerDist / g_light[lightIndex].range);

			float lightAngle = acos(dot(normalize(viewLightVec), viewCenterLightDir));

			if (centerDist < 0.f || centerDist > g_light[lightIndex].range) // 최대 거리를 벗어났는지
				distanceRatio = 0.f;
			else if (lightAngle > halfAngle) // 최대 시야각을 벗어났는지, cos이라 마이너스가 되어도 같다!
				distanceRatio = 0.f;
			else // 거리에 따라 적절히 세기를 조절
				distanceRatio = saturate(1.f - pow(centerDist / g_light[lightIndex].range, 2));
		}
	}

	// R구하기 위해 2배해주는거 그림!
	float3 reflectionDir = normalize(viewLightDir + 2 * (saturate(dot(-viewLightDir, viewNormal)) * viewNormal));	
	float3 eyeDir = normalize(viewPos); // 뷰스페이스 = 카메라 기준으로 한 로컬 좌표계 = 카메라 위치 (0,0,0)
	specularRatio = saturate(dot(-eyeDir, reflectionDir)); //기존 eyeDir는 카메라에서 픽셀로 향하는 방향이므로 -eyeDir로 바꿔줘야함
	specularRatio = pow(specularRatio, 2); // cos값에 대한 감쇠, 2는 임의로 정한 값. 높을수록 반짝이는 정도가 커짐

	//cos값에 빛의 색상 곱해서 diffuse 계산, 거리에 따른 감쇠도 적용
	color.diffuse = g_light[lightIndex].color.diffuse * diffuseRatio * distanceRatio; 
	color.ambient = g_light[lightIndex].color.ambient * distanceRatio;
	color.specular = g_light[lightIndex].color.specular * specularRatio * distanceRatio;

	return color;
}



#endif