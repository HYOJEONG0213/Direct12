<div align="center">

# 다이렉트12 

DirectX 12 기반 자체 엔진으로 개발중인 3D 수박게임 모작

## 개발 환경
Visual Studio 2022 / C++ / DirectX 12 / FBX SDK / ImGui

## 조작법
마우스 이동, 클릭 : 조준 및 행성 발사  
Z, C : 접시 중심 카메라 회전

## 현재까지 개발 사항
### 엔진 (클론코딩)
[C++과 언리얼로 만드는 MMORPG 게임 개발 시리즈 Part2: 게임 수학과 DirectX12](https://www.inflearn.com/course/%EC%96%B8%EB%A6%AC%EC%96%BC-3d-mmorpg-2)

1. Constant Buffer, RootSignature, Index Buffer 
2. Texture Mapping 
3. Depth Stencil View
4. Input, Timer
5. Material 
6. Component
7. SceneManager
8. Camera
9. Resource Manager(Resources)
10. Lighting
11. Normal Mapping
12. Skybox
13. Frustum Culling 
14. 직교 투영
15. Render Target
16. Compute Shader
17. Particle System
18. Instancing
19. Shadow Mapping
20. Tessellation
21. Terrain
22. Picking
23. 애니메이션 

### 엔진(직접 제작)
1. fx 기반 텍스처 Load, Save 기능
2. PhysicsManager (구-구/구-평면 충돌, 반발/마찰 처리)
3. Rigidbody
4. Collider 
5. UI Manager (ImGui 연동)
6. 접시 중심 궤도 카메라

### 클라이언트
1. 오브젝트 배치
2. 공발사 로직 구현 (포물선 운동)
3. 인게임 UI 제작 (점수판, 다음 행성 미리보기, 행성 순서표)
4. 같은 행성 충돌 시 합치기 (PlanetMerger)
5. 공 궤적 조준선 UI + 인스턴싱 적용 (드로우콜 200개 → 1개 최적화)

## 남은 개발 사항들
1. 메인화면, 게임화면 씬 전환
2. 게임오버 판정 및 점수 저장
3. 게임화면 버그 수정 및 퀄리티 올리기 작업
