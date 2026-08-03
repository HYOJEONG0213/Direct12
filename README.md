<div align="center">

# 다이렉트12 

DirectX 12 기반 자체 엔진으로 개발중인 3D 수박게임 모작

## 개발 환경
Visual Studio 2022 / C++ / DirectX 12 / FBX SDK / ImGui

## 조작법
마우스 이동, 클릭 : 조준 및 행성 발사  
Z, C : 접시 중심 카메라 회전

## 현재까지 개발 사항
### 데모 영상
![demo](docs/demo.gif)

### 엔진 (클론코딩)
[C++과 언리얼로 만드는 MMORPG 게임 개발 시리즈 Part2: 게임 수학과 DirectX12](https://www.inflearn.com/course/%EC%96%B8%EB%A6%AC%EC%96%BC-3d-mmorpg-2)

| 카테고리 | 내용 |
|---|---|
| 렌더링 파이프라인 | Constant Buffer, RootSignature, Index Buffer / Texture Mapping / Depth Stencil View |
| 조명 & 메테리얼 | Material / Lighting / Normal Mapping / Shadow Mapping |
| 씬 & 카메라 | SceneManager / Camera / 직교 투영 / Frustum Culling / Picking |
| 렌더 타겟 & GPU 연산 | Render Target / Compute Shader / Instancing / Tessellation |
| 이펙트 & 지형 | Particle System / Terrain / Skybox |
| 애니메이션 | 스켈레탈 애니메이션 |
| 기타 | Input, Timer / Resource Manager(Resources) / Component |

### 엔진(직접 제작)
| 기능 | 설명 |
|---|---|
| fx 기반 텍스처 Load, Save |  미리 저장, 로드해 런타임 감소 |
| PhysicsManager | 구-구/구-평면 충돌, 반발/마찰 처리 |
| Rigidbody | 중력, 속도 기반 이동 처리 |
| Collider | 구(Sphere) 충돌 판정용 반지름 정보 관리 |
| UI Manager | ImGui 연동 |
| 접시 중심 궤도 카메라 | Z/C 키로 접시를 바라보며 원 궤도 회전 |

### 클라이언트
| 기능 | 설명 |
|---|---|
| 오브젝트 배치 | 행성, 테이블 등 여러 오브젝트 배치 |
| 공 발사 로직 구현 | 포물선 운동 |
| 인게임 UI 제작 | 점수판, 다음 행성 미리보기, 행성 순서표 |
| 같은 행성 충돌 시 합치기 | PlanetMerger |
| 공 궤적 조준선 UI | 인스턴싱 적용 (드로우콜 200개 → 1개 최적화) |

## 남은 개발 사항들
1. 메인화면, 게임화면 씬 전환
2. 게임오버 판정 및 점수 저장
3. 게임화면 버그 수정 및 퀄리티 올리기 작업
