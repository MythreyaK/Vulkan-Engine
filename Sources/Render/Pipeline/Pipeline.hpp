#ifndef RENDER_PIPELINE_HPP

#define RENDER_PIPELINE_HPP

#include "VKinclude/VKinclude.hpp"


namespace Engine::Render {

    class Pipeline {

    private:
        vk::UniquePipelineLayout    pipelineLayout;
        vk::UniquePipeline          graphicsPipeline;

    public:
        Pipeline() = default;
        Pipeline(const Pipeline&) = delete;
        Pipeline(Pipeline&&) = default;
        Pipeline& operator=(const Pipeline&) = delete;
        Pipeline& operator=(Pipeline&&) = default;
        Pipeline(const vk::Device& renderDevice, const vk::RenderPass&, const vk::Extent2D& swapExtents);
        vk::Pipeline          GetPipeline() { return graphicsPipeline.get(); }
        vk::PipelineLayout    GetPipelineLayout() { return pipelineLayout.get(); }
    };

}

#endif // !RENDER_PIPELINE_HPP
