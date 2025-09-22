#include "vulkan_pipeline.hpp"

#include <fmt/core.h>

#include <stdexcept>
#include <unordered_set>

#include "vulkan_descriptor_set.hpp"
#include "vulkan_device.hpp"
#include "vulkan_render_definitions.hpp"
#include "vulkan_shader.hpp"

namespace aetherion {
    vk::PipelineShaderStageCreateInfo toVkPipelineShaderStageCreateInfo(
        const ShaderModuleStageDescription& description) {
        if (!description.shader) {
            throw std::invalid_argument("Shader in ShaderModuleStageDescription is null.");
        }
        const auto* vkShader = dynamic_cast<const VulkanShader*>(description.shader);

        return vk::PipelineShaderStageCreateInfo()
            .setStage(toVkShaderStageFlag(description.stage))
            .setModule(vkShader->getVkShaderModule())
            .setPName(description.entryPoint.c_str());
        /*.setSpecializationInfo(description.specializationInfo)*/;
    }

    vk::ComputePipelineCreateInfo toVkComputePipelineCreateInfo(
        const IPipelineLayout* layout, const ComputePipelineDescription& description) {
        // Layout
        if (!layout) {
            throw std::invalid_argument("Pipeline layout in ComputePipelineDescription is null.");
        }
        auto vkLayout = dynamic_cast<const VulkanPipelineLayout*>(layout);

        // Shader stage
        auto vkComputeShader = toVkPipelineShaderStageCreateInfo(description.computeShader);

        // Pipeline layout
        vk::PipelineLayout pipelineLayout = vkLayout->getVkPipelineLayout();

        // Pipeline creation
        auto pipelineInfo = vk::ComputePipelineCreateInfo();
        pipelineInfo.setStage(vkComputeShader);
        pipelineInfo.setLayout(pipelineLayout);

        return pipelineInfo;
    }

    vk::GraphicsPipelineCreateInfo toVkGraphicsPipelineCreateInfo(
        const VulkanPipelineLayout& layout, const GraphicsPipelineDescription& description) {
        // Shader stages
        // TODO: Handle more robust use cases (e.g. multiple shader stages, optional stages, etc.).
        // TODO: Missing validation that the provided shaders match the pipeline description.
        if (description.shaders.size() < 2) {
            throw std::runtime_error(
                "Vulkan graphics pipeline requires at least a vertex and fragment shader.");
        }
        std::vector<vk::PipelineShaderStageCreateInfo> vkShaderStages;
        for (const auto& shader : description.shaders) {
            vkShaderStages.push_back(toVkPipelineShaderStageCreateInfo(shader));
        }

        // Dynamic state
        std::vector<vk::DynamicState> dynamicStates
            = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
        vk::PipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.setDynamicStates(dynamicStates);

        // Vertex input state
        vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};
        std::vector<vk::VertexInputBindingDescription> bindingDescriptions;
        for (const auto& binding : description.inputStateDescription.vertexBindings) {
            bindingDescriptions.push_back(
                vk::VertexInputBindingDescription()
                    .setBinding(binding.binding)
                    .setStride(binding.stride)
                    .setInputRate(toVkVertexInputRate(binding.inputRate)));
        }
        vertexInputInfo.setVertexBindingDescriptions(bindingDescriptions);
        std::vector<vk::VertexInputAttributeDescription> attributeDescriptions;
        for (const auto& attribute : description.inputStateDescription.vertexAttributes) {
            attributeDescriptions.push_back(
                vk::VertexInputAttributeDescription()
                    .setLocation(attribute.location)
                    .setBinding(attribute.binding)
                    .setFormat(toVkVertexAttributeFormat(attribute.format))
                    .setOffset(attribute.offset));
        }
        vertexInputInfo.setVertexAttributeDescriptions(attributeDescriptions);

        // Input assembly
        vk::PipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.setTopology(
            toVkPrimitiveTopology(description.assemblyStateDescription.primitiveType));
        inputAssembly.setPrimitiveRestartEnable(
            description.assemblyStateDescription.enablePrimitiveRestart ? vk::True : vk::False);

        // Viewport and scissor
        // NOTE: Handled dynamically in the command buffer.
        vk::PipelineViewportStateCreateInfo viewportState{};
        viewportState.setViewportCount(1);
        viewportState.setScissorCount(1);

        // Rasterizer
        vk::PipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.setDepthClampEnable(
            description.rasterizationStateDescription.enableDepthClamp ? vk::True : vk::False);
        rasterizer.setPolygonMode(
            toVkPolygonMode(description.rasterizationStateDescription.polygonMode));
        rasterizer.setCullMode(toVkCullMode(description.rasterizationStateDescription.cullMode));
        rasterizer.setFrontFace(toVkFrontFace(description.rasterizationStateDescription.frontFace));
        rasterizer.setDepthBiasEnable(
            description.rasterizationStateDescription.enableDepthBias ? vk::True : vk::False);
        rasterizer.setDepthBiasConstantFactor(
            description.rasterizationStateDescription.depthBiasConstantFactor);
        rasterizer.setDepthBiasClamp(description.rasterizationStateDescription.depthBiasClamp);
        rasterizer.setDepthBiasSlopeFactor(
            description.rasterizationStateDescription.depthBiasSlopeFactor);
        rasterizer.setLineWidth(description.rasterizationStateDescription.lineWidth);

        // Multisampling
        vk::PipelineMultisampleStateCreateInfo multisampling{};
        multisampling.setSampleShadingEnable(
            description.multisampleStateDescription.enableSampleShading ? vk::True : vk::False);
        multisampling.setRasterizationSamples(
            toVkSampleCount(description.multisampleStateDescription.sampleCount));
        multisampling.setMinSampleShading(description.multisampleStateDescription.minSampleShading);
        multisampling.setPSampleMask(description.multisampleStateDescription.sampleMasks
                                         .data());  // TODO: Check if this is correct.

        // Depth stencil
        vk::PipelineDepthStencilStateCreateInfo depthStencil{};
        depthStencil.setDepthTestEnable(
            description.depthStencilStateDescription.enableDepthTest ? vk::True : vk::False);
        depthStencil.setDepthWriteEnable(
            description.depthStencilStateDescription.enableDepthWrite ? vk::True : vk::False);
        depthStencil.setDepthCompareOp(
            toVkCompareOp(description.depthStencilStateDescription.depthCompareOp));
        depthStencil.setDepthBoundsTestEnable(
            description.depthStencilStateDescription.enableDepthBoundsTest ? vk::True : vk::False);
        depthStencil.setStencilTestEnable(
            description.depthStencilStateDescription.enableStencilTest ? vk::True : vk::False);
        // Missing front and back values for stencil.
        depthStencil.setMinDepthBounds(description.depthStencilStateDescription.minDepthBounds);
        depthStencil.setMaxDepthBounds(description.depthStencilStateDescription.maxDepthBounds);

        // Color blend
        vk::PipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.setLogicOpEnable(
            description.colorBlendStateDescription.enableLogicOp ? vk::True : vk::False);
        colorBlending.setLogicOp(
            static_cast<vk::LogicOp>(description.colorBlendStateDescription.logicOp));
        std::vector<vk::PipelineColorBlendAttachmentState> blendAttachments;
        for (const auto& attachment : description.colorBlendStateDescription.colorAttachments) {
            vk::PipelineColorBlendAttachmentState blendAttachment{};
            blendAttachment.setBlendEnable(attachment.enableBlending ? vk::True : vk::False);
            blendAttachment.setSrcColorBlendFactor(toVkBlendFactor(attachment.srcColorBlendFactor));
            blendAttachment.setDstColorBlendFactor(toVkBlendFactor(attachment.dstColorBlendFactor));
            blendAttachment.setColorBlendOp(toVkBlendOp(attachment.colorBlendOp));
            blendAttachment.setSrcAlphaBlendFactor(toVkBlendFactor(attachment.srcAlphaBlendFactor));
            blendAttachment.setDstAlphaBlendFactor(toVkBlendFactor(attachment.dstAlphaBlendFactor));
            blendAttachment.setAlphaBlendOp(toVkBlendOp(attachment.alphaBlendOp));
            blendAttachment.setColorWriteMask(toVkColorComponentFlags(attachment.colorWriteMask));
            blendAttachments.push_back(blendAttachment);
        }
        colorBlending.setAttachments(blendAttachments);
        colorBlending.setBlendConstants(description.colorBlendStateDescription.blendConstants);

        // Pipeline layout
        vk::PipelineLayout pipelineLayout = layout.getVkPipelineLayout();

        // Rendering information
        std::unordered_set<vk::Format> attachmentFormatsSet;
        for (const auto& attachment : description.colorBlendStateDescription.colorAttachments) {
            attachmentFormatsSet.insert(toVkFormat(attachment.format));
        }
        std::vector<vk::Format> attachmentFormats(attachmentFormatsSet.begin(),
                                                  attachmentFormatsSet.end());
        vk::PipelineRenderingCreateInfo renderingInfo{};
        renderingInfo.setColorAttachmentCount(static_cast<uint32_t>(attachmentFormats.size()));
        renderingInfo.setColorAttachmentFormats(attachmentFormats);
        renderingInfo.setDepthAttachmentFormat(
            toVkFormat(description.depthStencilStateDescription.depthFormat));
        /*renderingInfo.setStencilAttachmentFormat(vulkan_defs::translateImageFormat(
            description.depthStencilStateDescription.stencilFormat));*/

        // Pipeline creation
        // TODO: Handle pipeline caching.

        auto pipelineInfo = vk::GraphicsPipelineCreateInfo();
        pipelineInfo.setStages(vkShaderStages);
        pipelineInfo.setPDynamicState(&dynamicState);
        pipelineInfo.setPViewportState(&viewportState);
        pipelineInfo.setPVertexInputState(&vertexInputInfo);
        pipelineInfo.setPInputAssemblyState(&inputAssembly);
        pipelineInfo.setPRasterizationState(&rasterizer);
        pipelineInfo.setPMultisampleState(&multisampling);
        pipelineInfo.setPDepthStencilState(&depthStencil);
        pipelineInfo.setPColorBlendState(&colorBlending);
        pipelineInfo.setLayout(pipelineLayout);
        pipelineInfo.setPNext(&renderingInfo);

        return pipelineInfo;
    }

    vk::PushConstantRange toVkPushConstantRange(const IPushConstantRange* range) {
        const auto* vkRange = dynamic_cast<const VulkanPushConstantRange*>(range);
        if (!vkRange) {
            throw std::invalid_argument("Invalid push constant range.");
        }
        return vkRange->getVkPushConstantRange();
    }

    vk::DescriptorSetLayout toVkDescriptorSetLayout(const IDescriptorSetLayout* layout) {
        const auto* vkLayout = dynamic_cast<const VulkanDescriptorSetLayout*>(layout);
        if (!vkLayout) {
            throw std::invalid_argument("Invalid descriptor set layout.");
        }
        return vkLayout->getVkDescriptorSetLayout();
    }

    std::vector<vk::PushConstantRange> toVkPushConstantRanges(
        const std::vector<IPushConstantRange*>& ranges) {
        std::vector<vk::PushConstantRange> vkRanges;
        vkRanges.reserve(ranges.size());

        for (const auto& range : ranges) {
            vkRanges.push_back(toVkPushConstantRange(range));
        }
        return vkRanges;
    }

    std::vector<vk::DescriptorSetLayout> toVkDescriptorSetLayouts(
        const std::vector<IDescriptorSetLayout*>& layouts) {
        std::vector<vk::DescriptorSetLayout> vkLayouts;
        vkLayouts.reserve(layouts.size());

        for (const auto& layout : layouts) {
            vkLayouts.push_back(toVkDescriptorSetLayout(layout));
        }
        return vkLayouts;
    }

    VulkanPipelineLayout::VulkanPipelineLayout(VulkanDevice& device,
                                               const PipelineLayoutDescription& description)
        : device_(device.getVkDevice()) {
        auto vkPushConstantRanges = toVkPushConstantRanges(description.pushConstantRanges);

        auto vkSetLayouts = toVkDescriptorSetLayouts(description.descriptorSetLayouts);

        pipelineLayout_
            = device_.createPipelineLayout(vk::PipelineLayoutCreateInfo()
                                               .setSetLayouts(vkSetLayouts)
                                               .setPushConstantRanges(vkPushConstantRanges)
                                               .setSetLayouts(vkSetLayouts));
    }

    VulkanPipelineLayout::VulkanPipelineLayout(vk::Device device, vk::PipelineLayout pipelineLayout)
        : device_(device), pipelineLayout_(pipelineLayout) {}

    VulkanPipelineLayout::~VulkanPipelineLayout() noexcept { clear(); }

    VulkanPipelineLayout::VulkanPipelineLayout(VulkanPipelineLayout&& other) noexcept
        : IPipelineLayout(std::move(other)),
          device_(other.device_),
          pipelineLayout_(other.pipelineLayout_) {
        other.device_ = nullptr;
        other.pipelineLayout_ = nullptr;
    }

    VulkanPipelineLayout& VulkanPipelineLayout::operator=(VulkanPipelineLayout&& other) noexcept {
        if (this != &other) {
            clear();

            IPipelineLayout::operator=(std::move(other));
            device_ = other.device_;
            pipelineLayout_ = other.pipelineLayout_;

            other.release();
        }
        return *this;
    }

    void VulkanPipelineLayout::clear() noexcept {
        if (pipelineLayout_ && device_) {
            device_.destroyPipelineLayout(pipelineLayout_);
            pipelineLayout_ = nullptr;
        }
        device_ = nullptr;
    }

    void VulkanPipelineLayout::release() noexcept {
        pipelineLayout_ = nullptr;
        device_ = nullptr;
    }

    VulkanPipeline::VulkanPipeline(VulkanDevice& device,
                                   const ComputePipelineDescription& description)
        : device_(device.getVkDevice()) {
        if (!description.layout) {
            throw std::invalid_argument("Pipeline layout in ComputePipelineDescription is null.");
        }
        const auto* vkLayout = dynamic_cast<const VulkanPipelineLayout*>(description.layout);

        auto result = device_.createComputePipeline(
            {}, toVkComputePipelineCreateInfo(vkLayout, description));
        if (result.result != vk::Result::eSuccess) {
            throw std::runtime_error("Failed to create Vulkan compute pipeline.");
        }
        pipeline_ = result.value;

        pipelineType_ = PipelineBindPoint::Compute;
    }

    VulkanPipeline::VulkanPipeline(VulkanDevice& device,
                                   const GraphicsPipelineDescription& description)
        : device_(device.getVkDevice()) {
        if (!description.layout) {
            throw std::invalid_argument("Pipeline layout in GraphicsPipelineDescription is null.");
        }
        const auto* vkLayout = dynamic_cast<const VulkanPipelineLayout*>(description.layout);

        auto result = device_.createGraphicsPipeline(
            {}, toVkGraphicsPipelineCreateInfo(*vkLayout, description));
        if (result.result != vk::Result::eSuccess) {
            throw std::runtime_error("Failed to create Vulkan graphics pipeline.");
        }
        pipeline_ = result.value;

        pipelineType_ = PipelineBindPoint::Graphics;
    }

    VulkanPipeline::VulkanPipeline(vk::Device device, vk::Pipeline pipeline,
                                   PipelineBindPoint pipelineType)
        : device_(device), pipeline_(pipeline), pipelineType_(pipelineType) {}

    VulkanPipeline::~VulkanPipeline() noexcept { clear(); }

    VulkanPipeline::VulkanPipeline(VulkanPipeline&& other) noexcept
        : device_(other.device_), pipeline_(other.pipeline_), pipelineType_(other.pipelineType_) {
        other.device_ = nullptr;
        other.pipeline_ = nullptr;
    }

    VulkanPipeline& VulkanPipeline::operator=(VulkanPipeline&& other) noexcept {
        if (this != &other) {
            clear();

            device_ = other.device_;
            pipeline_ = other.pipeline_;

            other.release();
        }
        return *this;
    }

    void VulkanPipeline::clear() noexcept {
        if (pipeline_ && device_) {
            device_.destroyPipeline(pipeline_);
            pipeline_ = nullptr;
        }
        device_ = nullptr;
    }

    void VulkanPipeline::release() noexcept {
        pipeline_ = nullptr;
        device_ = nullptr;
    }
}  // namespace aetherion