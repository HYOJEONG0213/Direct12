#pragma once
class DepthStencilBuffer
{
public:
	// D32 : depth 용도로 32비트 활용한다1
	// D45 : depth 용도로 24비트, stencil은 8비트 활용한다
	void Init(const WindowInfo &window, DXGI_FORMAT dsvFormat = DXGI_FORMAT_D32_FLOAT);

	D3D12_CPU_DESCRIPTOR_HANDLE GetDSVCpuHandle() { return _dsvHandle; }
	DXGI_FORMAT					GetDSVFormat() { return _dsvFormat; }

private:
	// Depth Stencil View
	ComPtr<ID3D12Resource>		 _dsvBuffer;
	ComPtr<ID3D12DescriptorHeap> _dsvHeap;
	D3D12_CPU_DESCRIPTOR_HANDLE	 _dsvHandle = {};
	DXGI_FORMAT					 _dsvFormat = {};
};
