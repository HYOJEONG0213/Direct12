#pragma once

class SwapChain;
class DescriptorHeap;

class CommandQueue
{
public:
	~CommandQueue();
	void Init(ComPtr<ID3D12Device> device,shared_ptr<SwapChain> swapChain);
	void WaitSync();

	void RenderBegin(const D3D12_VIEWPORT* vp,const D3D12_RECT* rect);
	void RenderEnd();

	//외부에서 커맨드 리스트를 꺼내 쓸 수 있게
	ComPtr <ID3D12CommandQueue> GetCmdQueue() {
		return _cmdQueue;
	}

private:
	// CommandQueue : DX12에 등장
	// 외주를 요청할 때, 하나씩 요청하면 비효율적
	// [외주 목록]에 일감을 차곡차곡 기록했다가 한 방에 요청하는 것
	ComPtr<ID3D12CommandQueue>			_cmdQueue;
	ComPtr<ID3D12CommandAllocator>		_cmdAlloc;	//일 할당 메모리를 할당
	ComPtr<ID3D12GraphicsCommandList>	_cmdList;


	// Fence : 울타리(?) : 외주 작업이 끝날때까지 대기하겠다! 
	// CPU / GPU 동기화를 위한 간단한 도구
	ComPtr<ID3D12Fence>					_fence;
	uint32								_fenceValue = 0;	//해당 번호까지 기다려
	HANDLE								_fenceEvent = INVALID_HANDLE_VALUE;



	shared_ptr<SwapChain>		_swapChain;
	shared_ptr<DescriptorHeap>	_descHeap;

};

