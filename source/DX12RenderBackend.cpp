#include "DX12RenderBackend.h"

using namespace Falcon;


void DX12RenderBackend::InitializePipeline() {
	/*	Enable the debug layer.
		Create the device.
		Create the command queue.
		Create the swap chain.
		Create a render target view(RTV) descriptor heap.
		Create frame resources(a render target view for each frame).
		Create a command allocator.*/

}
// 每个场景加载时调用一次
void DX12RenderBackend::InitializeResources() {   
	/*	Create an empty root signature.
		A graphics root signature defines what resources are bound to the graphics pipeline.
		Compile the shaders.
		Create the vertex input layout.
		Create a pipeline state object description, then create the object.
		Create the command list.
		Close the command list.
		Createand load the vertex buffers.
		Create the vertex buffer views.
		Create a fence.
		Create an event handle.
		Wait for the GPU to finish.*/
}



void DX12RenderBackend::DoRender() {

	// 资源准备一次

	for (auto cam : sceneCams) {
		// if onlyAE{}
		// 获取相机参数
		// 渲染
		for (auto obj : RenderQueue) {

			drawxxx;
		}
	}

	auto mainCam = SceneManager::Instance().CurrScene().MainCam();




	/*Populate the command list.
		Reset the command list allocator.
		Reset the command list.
		Set the graphics root signature.
		Sets the graphics root signature to use for the current command list.
		Set the viewportand scissor rectangles.
		Set a resource barrier, indicating the back buffer is to be used as a render target.
		Resource barriers are used to manage resource transitions.
		Record commands into the command list.
		Indicate the back buffer will be used to present after the command list has executed.
		Close the command list to further recording.
		Execute the command list.
		Present the frame.
		Wait for the GPU to finish.*/


	/*
	auto& fb = checked_cast<D3D12FrameBuffer&>(*this->CurFrameBuffer());
	fb.BindBarrier();

	for (uint32_t i = 0; i < so_buffs_.size(); ++i)
	{
		auto& d3dvb = checked_cast<D3D12GraphicsBuffer&>(*so_buffs_[i]);
		d3dvb.UpdateResourceBarrier(d3d_render_cmd_list_.get(), 0, D3D12_RESOURCE_STATE_STREAM_OUT);
	}

	uint32_t const num_vertex_streams = rl.NumVertexStreams();

	for (uint32_t i = 0; i < num_vertex_streams; ++i)
	{
		auto& d3dvb = checked_cast<D3D12GraphicsBuffer&>(*rl.GetVertexStream(i));
		if (!(d3dvb.AccessHint() & (EAH_CPU_Read | EAH_CPU_Write)))
		{
			d3dvb.UpdateResourceBarrier(d3d_render_cmd_list_.get(), 0, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
		}
	}
	if (rl.InstanceStream())
	{
		auto& d3dvb = checked_cast<D3D12GraphicsBuffer&>(*rl.InstanceStream().get());
		if (!(d3dvb.AccessHint() & (EAH_CPU_Read | EAH_CPU_Write)))
		{
			d3dvb.UpdateResourceBarrier(d3d_render_cmd_list_.get(), 0, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
		}
	}

	if (rl.UseIndices())
	{
		auto& ib = checked_cast<D3D12GraphicsBuffer&>(*rl.GetIndexStream());
		if (!(ib.AccessHint() & (EAH_CPU_Read | EAH_CPU_Write)))
		{
			ib.UpdateResourceBarrier(d3d_render_cmd_list_.get(), 0, D3D12_RESOURCE_STATE_INDEX_BUFFER);
		}
	}

	if (rl.GetIndirectArgs())
	{
		auto& arg_buff = checked_cast<D3D12GraphicsBuffer&>(*rl.GetIndirectArgs());
		if (!(arg_buff.AccessHint() & (EAH_CPU_Read | EAH_CPU_Write)))
		{
			arg_buff.UpdateResourceBarrier(d3d_render_cmd_list_.get(), 0, D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT);
		}
	}

	this->FlushResourceBarriers(d3d_render_cmd_list_.get());

	checked_cast<D3D12RenderLayout const&>(rl).Active();

	uint32_t const vertex_count = static_cast<uint32_t>(rl.UseIndices() ? rl.NumIndices() : rl.NumVertices());

	RenderLayout::topology_type tt = rl.TopologyType();
	if (tech.HasTessellation())
	{
		switch (tt)
		{
		case RenderLayout::TT_PointList:
			tt = RenderLayout::TT_1_Ctrl_Pt_PatchList;
			break;

		case RenderLayout::TT_LineList:
			tt = RenderLayout::TT_2_Ctrl_Pt_PatchList;
			break;

		case RenderLayout::TT_TriangleList:
			tt = RenderLayout::TT_3_Ctrl_Pt_PatchList;
			break;

		default:
			break;
		}
	}
	this->IASetPrimitiveTopology(tt);

	uint32_t prim_count;
	switch (tt)
	{
	case RenderLayout::TT_PointList:
		prim_count = vertex_count;
		break;

	case RenderLayout::TT_LineList:
	case RenderLayout::TT_LineList_Adj:
		prim_count = vertex_count / 2;
		break;

	case RenderLayout::TT_LineStrip:
	case RenderLayout::TT_LineStrip_Adj:
		prim_count = vertex_count - 1;
		break;

	case RenderLayout::TT_TriangleList:
	case RenderLayout::TT_TriangleList_Adj:
		prim_count = vertex_count / 3;
		break;

	case RenderLayout::TT_TriangleStrip:
	case RenderLayout::TT_TriangleStrip_Adj:
		prim_count = vertex_count - 2;
		break;

	default:
		if ((tt >= RenderLayout::TT_1_Ctrl_Pt_PatchList)
			&& (tt <= RenderLayout::TT_32_Ctrl_Pt_PatchList))
		{
			prim_count = vertex_count / (tt - RenderLayout::TT_1_Ctrl_Pt_PatchList + 1);
		}
		else
		{
			KFL_UNREACHABLE("Invalid topology type");
		}
		break;
	}

	uint32_t const num_instances = rl.NumInstances() * this->NumRealizedCameraInstances();

	num_primitives_just_rendered_ += num_instances * prim_count;
	num_vertices_just_rendered_ += num_instances * vertex_count;

	uint32_t const num_passes = tech.NumPasses();
	bool const has_tessellation = tech.HasTessellation();
	GraphicsBufferPtr const& indirect_buff = rl.GetIndirectArgs();
	if (indirect_buff)
	{
		auto const& d3d12_indirect_buff = checked_cast<D3D12GraphicsBuffer const&>(*indirect_buff);
		auto* arg_buff = d3d12_indirect_buff.D3DResource();
		uint32_t const arg_buff_offset = d3d12_indirect_buff.D3DResourceOffset();

		if (rl.UseIndices())
		{
			for (uint32_t i = 0; i < num_passes; ++i)
			{
				auto& pass = tech.Pass(i);

				pass.Bind(effect);
				this->UpdateRenderPSO(effect, pass, rl, has_tessellation);
				d3d_render_cmd_list_->ExecuteIndirect(draw_indexed_indirect_signature_.get(), 1,
					arg_buff, arg_buff_offset + rl.IndirectArgsOffset(), nullptr, 0);
				pass.Unbind(effect);
			}
		}
		else
		{
			for (uint32_t i = 0; i < num_passes; ++i)
			{
				auto& pass = tech.Pass(i);

				pass.Bind(effect);
				this->UpdateRenderPSO(effect, pass, rl, has_tessellation);
				d3d_render_cmd_list_->ExecuteIndirect(draw_indirect_signature_.get(), 1,
					arg_buff, arg_buff_offset + rl.IndirectArgsOffset(), nullptr, 0);
				pass.Unbind(effect);
			}
		}
	}
	else
	{
		if (rl.UseIndices())
		{
			uint32_t const num_indices = rl.NumIndices();
			for (uint32_t i = 0; i < num_passes; ++i)
			{
				auto& pass = tech.Pass(i);

				pass.Bind(effect);
				this->UpdateRenderPSO(effect, pass, rl, has_tessellation);
				d3d_render_cmd_list_->DrawIndexedInstanced(num_indices, num_instances, rl.StartIndexLocation(),
					rl.StartVertexLocation(), rl.StartInstanceLocation());
				pass.Unbind(effect);
			}
		}
		else
		{
			uint32_t const num_vertices = rl.NumVertices();
			for (uint32_t i = 0; i < num_passes; ++i)
			{
				auto& pass = tech.Pass(i);

				pass.Bind(effect);
				this->UpdateRenderPSO(effect, pass, rl, has_tessellation);
				d3d_render_cmd_list_->DrawInstanced(num_vertices, num_instances,
					rl.StartVertexLocation(), rl.StartInstanceLocation());
				pass.Unbind(effect);
			}
		}
	}

	num_draws_just_called_ += num_passes;
	*/
}


void DX12RenderBackend::DoDispatch() {
	/*
	uint32_t const num_passes = tech.NumPasses();
		for (uint32_t i = 0; i < num_passes; ++ i)
		{
			auto& pass = tech.Pass(i);

			pass.Bind(effect);
			this->UpdateComputePSO(effect, pass);
			d3d_render_cmd_list_->Dispatch(tgx, tgy, tgz);
			pass.Unbind(effect);
		}

		num_dispatches_just_called_ += num_passes;
	*/
}