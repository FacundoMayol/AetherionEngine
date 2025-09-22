#pragma once

#include <array>
#include <string>
#include <vector>

#include "aetherion/graphics/backend/render_definitions.hpp"
#include "aetherion/graphics/backend/resource.hpp"

namespace aetherion {
    // Forward declarations
    class IDescriptorSetLayout;
    class IPushConstantRange;
    class IShader;

    struct PipelineLayoutDescription {
        std::vector<IDescriptorSetLayout*> descriptorSetLayouts;
        std::vector<IPushConstantRange*> pushConstantRanges;
    };

    struct VertexBindingDescription {
        uint32_t binding;
        uint32_t stride;
        VertexInputRate inputRate;
    };

    struct VertexAttributeDescription {
        uint32_t location;
        uint32_t binding;
        VertexAttributeFormat format;
        uint32_t offset;
    };

    struct PipelineInputStateDescription {
        std::vector<VertexBindingDescription> vertexBindings;
        std::vector<VertexAttributeDescription> vertexAttributes;
    };

    struct PipelineAssemblyStateDescription {
        PrimitiveTopology primitiveType;
        bool enablePrimitiveRestart;
    };

    struct PipelineRasterizationStateDescription {
        PolygonMode polygonMode;
        CullMode cullMode;
        FrontFace frontFace;
        bool enableDepthClamp;
        bool enableDepthBias;
        float depthBiasConstantFactor;
        float depthBiasClamp;
        float depthBiasSlopeFactor;
        float lineWidth;
    };

    struct PipelineMultisampleStateDescription {
        SampleCount sampleCount;
        bool enableSampleShading;
        float minSampleShading;
        std::vector<SampleMask> sampleMasks;
        /*bool enableAlphaToCoverage;
        bool enableAlphaToOne;*/
    };

    struct PipelineDepthStencilStateDescription {
        Format depthFormat;
        Format stencilFormat;
        bool enableDepthTest;
        bool enableDepthWrite;
        CompareOp depthCompareOp;
        bool enableDepthBoundsTest;
        float minDepthBounds;
        float maxDepthBounds;
        bool enableStencilTest;
        // Missing stencil ops
    };

    struct PipelineBlendAttachmentStateDescription {
        Format format;
        bool enableBlending;
        BlendFactor srcColorBlendFactor;
        BlendFactor dstColorBlendFactor;
        BlendOp colorBlendOp;
        BlendFactor srcAlphaBlendFactor;
        BlendFactor dstAlphaBlendFactor;
        BlendOp alphaBlendOp;
        ColorComponentFlags colorWriteMask;
    };

    struct PipelineColorBlendAttachmentStateDescription {
        std::vector<PipelineBlendAttachmentStateDescription> colorAttachments;
        bool enableLogicOp;
        BlendingLogicOp logicOp;
        std::array<float, 4> blendConstants;
    };

    struct ShaderModuleStageDescription {
        ShaderStage stage;
        IShader* shader;
        std::string entryPoint = "main";
    };

    struct ComputePipelineDescription {
        class IPipelineLayout* layout{};
        ShaderModuleStageDescription computeShader;
    };

    struct GraphicsPipelineDescription {
        class IPipelineLayout* layout;
        std::vector<ShaderModuleStageDescription> shaders;
        PipelineInputStateDescription inputStateDescription;
        PipelineAssemblyStateDescription assemblyStateDescription;
        PipelineRasterizationStateDescription rasterizationStateDescription;
        PipelineMultisampleStateDescription multisampleStateDescription;
        PipelineDepthStencilStateDescription depthStencilStateDescription;
        PipelineColorBlendAttachmentStateDescription colorBlendStateDescription;
    };

    class IPipelineLayout : public IResource {
      public:
        ~IPipelineLayout() override = 0;

        IPipelineLayout(const IPipelineLayout&) = delete;
        IPipelineLayout& operator=(const IPipelineLayout&) = delete;

      protected:
        IPipelineLayout() = default;
        IPipelineLayout(IPipelineLayout&&) noexcept = default;
        IPipelineLayout& operator=(IPipelineLayout&&) noexcept = default;
    };

    class IPipeline : public IResource {
      public:
        ~IPipeline() override = 0;

        IPipeline(const IPipeline&) = delete;
        IPipeline& operator=(const IPipeline&) = delete;

        virtual PipelineBindPoint getPipelineType() const = 0;

      protected:
        IPipeline() = default;
        IPipeline(IPipeline&&) noexcept = default;
        IPipeline& operator=(IPipeline&&) noexcept = default;
    };
}  // namespace aetherion
