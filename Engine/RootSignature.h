#pragma once

// 계약서 , 결재 
// 어떤일을 할지 서명한다!

class RootSignature
{
public:
	void Init();

	ComPtr<ID3D12RootSignature>	GetRootSignature() {
		return _signature;
	}


private:
	void CreateSamplerDesc();
	void CreateRootSignature();

private:
	ComPtr<ID3D12RootSignature> _signature;
	D3D12_STATIC_SAMPLER_DESC _samplerDesc;
};

