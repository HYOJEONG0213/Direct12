#include "pch.h"
#include "Mesh.h"
#include "Engine.h"
#include "Material.h"

void Mesh::Init(const vector<Vertex> &vertexBuffer, const vector<uint32> &indexbuffer)
{
	CreateVertexBuffer(vertexBuffer);
	CreateIndexBuffer(indexbuffer);
}

// CommandQueue.cpp의 RenderBegin과 RenderEnd 사이에 호출
void Mesh::Render()
{
	// 커맨드리스트를 이용해서
	// 정점들이 어떻게 연결되어있는지 설정
	CMD_LIST->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// vertexBufferView 연결
	CMD_LIST->IASetVertexBuffers(0, 1, &_vertexBufferView); // Slot: (0~15)
	CMD_LIST->IASetIndexBuffer(&_indexBufferView);

	// 1. Buffer에다가 데이터 셋팅
	// 2. TableDescHeap에다가 CBV 전달
	// 3. 모두 세팅이 끝났으면 TableDescHeap 커밋
	CONST_BUFFER(CONSTANT_BUFFER_TYPE::TRANSFORM)->PushData(&_transform, sizeof(_transform));

	_mat->Update();

	GEngine->GetTableDescHeap()->CommitTable();

	// 그리라고 예약하기 (버텍스버전)
	// CMD_LIST->DrawInstanced(_vertexCount,1,0,0);
	CMD_LIST->DrawIndexedInstanced(_indexCount, 1, 0, 0, 0);
}

// 벡터에 vertex 받기 (위치, 컬러 정보)
void Mesh::CreateVertexBuffer(const vector<Vertex> &buffer)
{
	_vertexCount = static_cast<uint32>(buffer.size());
	uint32 bufferSize = _vertexCount * sizeof(Vertex);

	// 정점을 GPU 메모리에 복사하기 위해 해당 공간 할당받음
	// 어떤 타입? 얼마나 공간 필요해?
	D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC	  desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

	// vertexBuffer 생성 (GPU 메모리 공간에 할당되어있음)
	DEVICE->CreateCommittedResource(&heapProperty, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ,
									nullptr, IID_PPV_ARGS(&_vertexBuffer));

	// vertexDataBuffer를 통해 vertexBuffer(실제 GPU 공간)에
	// 데이터 복사할 수 있게 연결
	// Copy the triangle data to the vertex buffer.
	void		 *vertexDataBuffer = nullptr;
	CD3DX12_RANGE readRange(0, 0); // We do not intend to read from this resource on the CPU.

	// 공간 매핑 (뚜껑 열고, 닫고)
	_vertexBuffer->Map(0, &readRange, &vertexDataBuffer);
	// 실제 GPU 공간에 데이터 복사
	::memcpy(vertexDataBuffer, &buffer[0], bufferSize);
	_vertexBuffer->Unmap(0, nullptr);

	// Initialize the vertex buffer view.
	// 각각의 뷰 정보
	_vertexBufferView.BufferLocation = _vertexBuffer->GetGPUVirtualAddress();
	_vertexBufferView.StrideInBytes = sizeof(Vertex); // 정점 1개 크기
	_vertexBufferView.SizeInBytes = bufferSize;		  // 버퍼의 크기
}

// 다 똑같은데 Vertex Buffer View->index buffer view로 수정
// sizeof(Vertex)대신 sizeof(uint32)
void Mesh::CreateIndexBuffer(const vector<uint32> &buffer)
{
	_indexCount = static_cast<uint32>(buffer.size());
	uint32 bufferSize = _indexCount * sizeof(uint32);

	D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC	  desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

	DEVICE->CreateCommittedResource(&heapProperty, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ,
									nullptr, IID_PPV_ARGS(&_indexBuffer));

	void		 *indexDataBuffer = nullptr;
	CD3DX12_RANGE readRange(0, 0); // We do not intend to read from this resource on the CPU.
	_indexBuffer->Map(0, &readRange, &indexDataBuffer);
	::memcpy(indexDataBuffer, &buffer[0], bufferSize);
	_indexBuffer->Unmap(0, nullptr);

	_indexBufferView.BufferLocation = _indexBuffer->GetGPUVirtualAddress();
	_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	_indexBufferView.SizeInBytes = bufferSize;
}
