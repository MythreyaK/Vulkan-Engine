#include "Pipeline.hpp"
#include "Shader/Shader.hpp"

#include <iostream>
#include <string>

namespace Engine::Render {

    Pipeline::Pipeline(const vk::Device& renderDevice, const vk::RenderPass& renderPass, const vk::Extent2D& swapExtents) {

        namespace ERSHD = Engine::Render::Shader;

        const auto vertCode { ERSHD::CreateShaderModule(renderDevice, "vert.spv") };
        const auto fragCode { ERSHD::CreateShaderModule(renderDevice, "frag.spv") };

        const auto inputAssembly{ vk::PipelineInputAssemblyStateCreateInfo()
            .setTopology(vk::PrimitiveTopology::eTriangleList)
            .setPrimitiveRestartEnable(false)
        };

        const auto vertShaderStage{ vk::PipelineShaderStageCreateInfo()
            .setStage(vk::ShaderStageFlagBits::eVertex)
            .setModule(vertCode.get())
            .setPName("main")
        };

        const auto fragShaderStage{ vk::PipelineShaderStageCreateInfo()
            .setStage(vk::ShaderStageFlagBits::eFragment)
            .setModule(fragCode.get())
            .setPName("main")
        };

        vk::PipelineShaderStageCreateInfo shaderStages[]{vertShaderStage, fragShaderStage};

        const auto vertexInputInfo{ vk::PipelineVertexInputStateCreateInfo()
            .setVertexBindingDescriptionCount(0)
            .setPVertexBindingDescriptions(nullptr)
            .setVertexAttributeDescriptionCount(0)
            .setPVertexAttributeDescriptions(nullptr)
            // TODO: Check 'instancing'
        };

        const auto viewport{ vk::Viewport()
            .setX(0)
            .setY(0)
            .setHeight(swapExtents.height)
            .setWidth(swapExtents.width)
            .setMinDepth(0.0f)
            .setMaxDepth(1.0f)
        };

        const auto scissor{ vk::Rect2D()
            .setOffset({0, 0})
            .setExtent(swapExtents)
        };

        const auto viewportState{vk::PipelineViewportStateCreateInfo()
            .setScissorCount(1)
            .setPScissors(&scissor)
            .setViewportCount(1)
            .setPViewports(&viewport)
        };

        const auto rasterizer{ vk::PipelineRasterizationStateCreateInfo()
            .setLineWidth(1.0f)
            .setDepthClampEnable(false)
            .setRasterizerDiscardEnable(false)
            .setPolygonMode(vk::PolygonMode::eFill)
            .setCullMode(vk::CullModeFlagBits::eBack)
            .setFrontFace(vk::FrontFace::eClockwise)
            .setDepthBiasEnable(false)
        };

        const auto multiSample { vk::PipelineMultisampleStateCreateInfo()
            .setRasterizationSamples(vk::SampleCountFlagBits::e1)
            .setSampleShadingEnable(false)
        };

        const auto colorBlendAttachment{ vk::PipelineColorBlendAttachmentState()
            .setColorWriteMask(
                vk::ColorComponentFlagBits::eR |
                vk::ColorComponentFlagBits::eG |
                vk::ColorComponentFlagBits::eB |
                vk::ColorComponentFlagBits::eA )
            .setBlendEnable(false)
        };

        const auto colorBlendState{ vk::PipelineColorBlendStateCreateInfo()
            .setLogicOpEnable(false)
            .setLogicOp(vk::LogicOp::eCopy)
            .setAttachmentCount(1)
            .setPAttachments(&colorBlendAttachment)
            .setBlendConstants({0.0f, 0.0f, 0.0f, 0.0f})
        };

        const auto pipelineLayoutCreateInfo { vk::PipelineLayoutCreateInfo() };

        pipelineLayout = renderDevice.createPipelineLayoutUnique(pipelineLayoutCreateInfo);

        const auto graphicsPipelineCreateInfo { vk::GraphicsPipelineCreateInfo()
            .setPInputAssemblyState(&inputAssembly)
            .setStageCount(2)
            .setPStages(shaderStages)
            .setPVertexInputState(&vertexInputInfo)
            .setPMultisampleState(&multiSample)
            .setPViewportState(&viewportState)
            .setLayout(pipelineLayout.get())
            .setPColorBlendState(&colorBlendState)
            .setPRasterizationState(&rasterizer)
            .setRenderPass(renderPass)
            .setSubpass(0)
        };

        graphicsPipeline = renderDevice.createGraphicsPipelineUnique(nullptr, graphicsPipelineCreateInfo);
    }

}
