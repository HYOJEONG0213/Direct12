#pragma once

 //[기안서]
 //외주를 맡길 때 이런 저런 정보들을 같이 넘겨줘야 하는데,
 //아무 형태로나 요청하면 못 알아먹는다
 //- 각종 리소스를 어떤 용도로 사용하는지 꼼꼼하게 적어서 넘겨주는 용도

 //다렉11까지는 View 라는 용어로 불렸음. 
 //리소스 한번 더 매핑해서 뭐하는얘인지 덧불이는 느낌
 //일종의 배열(같은 얘들끼리 묶어서 관리) 
class DescriptorHeap
{
public:
	void Init(ComPtr<ID3D12Device> device, shared_ptr<class SwapChain> swapChain);
	
	D3D12_CPU_DESCRIPTOR_HANDLE		GetRTV(int32 idx) { return _rtvHandle[idx]; }
	D3D12_CPU_DESCRIPTOR_HANDLE		GetBackBufferView();

private:
	ComPtr<ID3D12DescriptorHeap>	_rtvHeap;
	uint32							_rtvHeapSize = 0;
	D3D12_CPU_DESCRIPTOR_HANDLE		_rtvHandle[SWAP_CHAIN_BUFFER_COUNT];

	shared_ptr<class SwapChain>		_swapChain;
};

