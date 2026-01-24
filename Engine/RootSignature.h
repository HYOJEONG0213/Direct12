#pragma once

// 계약서 , 결재 
// 어떤일을 할지 서명한다!

class RootSignature
{
public:
	void Init(ComPtr<ID3D12Device> device);

	ComPtr<ID3D12RootSignature>	GetSignature() {return _signature;}


private:
	ComPtr<ID3D12RootSignature> _signature;

};

