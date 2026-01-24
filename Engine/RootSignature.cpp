#include "pch.h"
#include "RootSignature.h"

void RootSignature::Init(ComPtr<ID3D12Device> device)
{
	//기본상태의 서명이 들어간다
	D3D12_ROOT_SIGNATURE_DESC sigDesc = CD3DX12_ROOT_SIGNATURE_DESC(D3D12_DEFAULT);
	//입력 조립기 단계
	sigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT; 

	ComPtr<ID3DBlob> blobSignature;
	ComPtr<ID3DBlob> blobError;
	::D3D12SerializeRootSignature(&sigDesc,D3D_ROOT_SIGNATURE_VERSION_1,&blobSignature,&blobError);
	device->CreateRootSignature(0,blobSignature->GetBufferPointer(),blobSignature->GetBufferSize(),IID_PPV_ARGS(&_signature));
}
