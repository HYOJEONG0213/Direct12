#pragma once

class UIManager
{
	DECLARE_SINGLE(UIManager);

public:
	void Init(HWND hwnd);
	void Render();
	void Shutdown();

	void AddScore(int score) { _score += score; }

private:
	ComPtr<ID3D12DescriptorHeap> _srvHeap;
	int _score = 0;
};
