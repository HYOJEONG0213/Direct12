#include "pch.h"
#include "UIManager.h"
#include "Engine.h"
#include "Device.h"
#include "CommandQueue.h"

void UIManager::Init(HWND hwnd)
{
	// 텍스처(SRV)용 DescriptorHeap 생성
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	desc.NumDescriptors = 1;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	ComPtr<ID3D12Device> device = GEngine->GetDevice()->GetDevice();
	device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&_srvHeap));

	// ImGui 초기화
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(hwnd);

	ImGui_ImplDX12_InitInfo initInfo;
	initInfo.Device = device.Get();
	initInfo.CommandQueue = GEngine->GetGraphicsCmdQueue()->GetGraphicsCmdQueue().Get();
	initInfo.NumFramesInFlight = 2;
	initInfo.RTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	initInfo.DSVFormat = DXGI_FORMAT_UNKNOWN;
	initInfo.SrvDescriptorHeap = _srvHeap.Get();
	initInfo.LegacySingleSrvCpuDescriptor = _srvHeap->GetCPUDescriptorHandleForHeapStart();
	initInfo.LegacySingleSrvGpuDescriptor = _srvHeap->GetGPUDescriptorHandleForHeapStart();
	ImGui_ImplDX12_Init(&initInfo);
}

void UIManager::Render()
{
	// 프레임 시작
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImVec2 disp = ImGui::GetIO().DisplaySize;
	ImVec2 pos(20.f, 540.f / 600.f * disp.y - 50.0f);
	ImVec2 size(140.f / 800.f * disp.x, 60.f / 600.f * disp.y);

	ImGui::SetNextWindowPos(pos, ImGuiCond_Always);
	ImGui::SetNextWindowSize(size, ImGuiCond_Always);
	ImGui::Begin("Score", nullptr,
				 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar |
					 ImGuiWindowFlags_NoBackground);
	ImGui::Text("Score: %d", _score);
	ImGui::End();

	// 프레임 종료 후 Render()
	ImGui::Render();

	// cmd에 그리기 명령 전달
	auto				  cmdList = GEngine->GetGraphicsCmdQueue()->GetGraphicsCmdList();
	ID3D12DescriptorHeap *heaps[] = {_srvHeap.Get()};
	cmdList->SetDescriptorHeaps(1, heaps);
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), cmdList.Get());
}

void UIManager::Shutdown()
{
	ImGui_ImplDX12_Shutdown(); // 연결 해제
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext(); // ImGui 메모리 정리
}
