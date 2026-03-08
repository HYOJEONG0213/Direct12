#pragma once
class ConstantBuffer
{
public:
	ConstantBuffer();
	~ConstantBuffer();

	void Init(uint32 size,uint32 count);

	void Clear();
	void PushData(int32 rootParamIndex,void* buffer,uint32 size);

	D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress(uint32 index);

private:
	void CreateBuffer();


private:
	ComPtr<ID3D12Resource>	_cbvBuffer;		// GPU쪽 버퍼 
	BYTE*					_mappedBuffer = nullptr;	//CPU에서 데이터 밀어넣을때 사용 
	uint32					_elementSize = 0;
	uint32					_elementCount = 0;

	uint32					_currentIndex = 0;

};

