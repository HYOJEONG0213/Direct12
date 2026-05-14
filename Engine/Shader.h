#pragma once
#include "Object.h"

// 일감 기술서 : 외주 인력들이 뭘 해야할지 기술함
// 단계별 기술서

enum class SHADER_TYPE : uint8
{
	DEFERRED,
	FORWARD,
};

enum class RASTERIZER_TYPE : uint8
{
	CULL_NONE,	// 모두 연산
	CULL_FRONT, // 시계방향 무시
	CULL_BACK,	// 반시계방향 무시
	WIREFRAME,	// 외각선만 보임
};

enum class DEPTH_STENCIL_TYPE : uint8 // 깊이테스트
{
	LESS,		// 깊이값이 작을때만 그려주기
	LESS_EQUAL, // 같을때도
	GREATER,
	GREATER_EQUAL,
};

struct ShaderInfo
{
	SHADER_TYPE					  shaderType = SHADER_TYPE::FORWARD;
	RASTERIZER_TYPE				  rasterizerType = RASTERIZER_TYPE::CULL_BACK;
	DEPTH_STENCIL_TYPE			  depthStencilType = DEPTH_STENCIL_TYPE::LESS;
	D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
};

class Shader : public Object
{
public:
	Shader();
	virtual ~Shader();

	void Init(const wstring &path, ShaderInfo info = ShaderInfo());
	void Update();

	SHADER_TYPE GetShaderType() { return _info.shaderType; }

private:
	void CreateShader(const wstring &path, const string &name, const string &version, ComPtr<ID3DBlob> &blob,
					  D3D12_SHADER_BYTECODE &shaderByteCode);
	void CreateVertexShader(const wstring &path, const string &name, const string &version);
	void CreatePixelShader(const wstring &path, const string &name, const string &version);

private:
	ShaderInfo _info;

	ComPtr<ID3DBlob> _vsBlob;
	ComPtr<ID3DBlob> _psBlob;
	ComPtr<ID3DBlob> _errBlob;

	ComPtr<ID3D12PipelineState>		   _pipelineState;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC _pipelineDesc = {};
};
