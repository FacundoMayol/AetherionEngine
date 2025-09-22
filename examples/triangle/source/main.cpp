#include <fmt/core.h>

#include <AetherionEngine/game.hpp>
#include <AetherionEngine/rendering/backend/render_defs.hpp>
#include <AetherionEngine/rendering/backend/render_pipeline.hpp>

using namespace AetherionEngine;

class Triangle : public Game {
  public:
    Triangle(const WindowInfo& primaryWindowInfo) : Game(primaryWindowInfo) {
        m_primarySurface = m_driver->createRenderSurface(m_primaryWindow);

        auto vertexShaderData = std::make_shared<Data>("build/triangle/triangle.vert.spv");
        m_vertexShader = m_driver->createShader(std::make_shared<ShaderData>(vertexShaderData),
                                                ShaderStage::Vertex);

        auto fragmentShaderData = std::make_shared<Data>("build/triangle/triangle.frag.spv");
        m_fragmentShader = m_driver->createShader(std::make_shared<ShaderData>(fragmentShaderData),
                                                  ShaderStage::Fragment);

        std::shared_ptr<GPUPipelineLayout> layout = m_driver->createGPUPipelineLayout({{}, {}});

        m_pipeline = m_driver->createGraphicsPipeline(
            {
                .layout = layout,
                .vertexShader = m_vertexShader,
                .fragmentShader = m_fragmentShader,
                .inputStateDescription = {},
                .assemblyStateDescription = {
                    .primitiveType = PrimitiveType::Triangles,
                    .enablePrimitiveRestart = false,
                },
                .rasterizationStateDescription = {
                    .polygonMode = PolygonMode::Fill,
                    .cullMode = CullMode::None,
                    .frontFace = FrontFace::Clockwise,
                    .enableDepthClamp = false,
                    .enableDepthBias = false,
                    .depthBiasConstantFactor = 0.0f,
                    .depthBiasClamp = 0.0f,
                    .depthBiasSlopeFactor = 0.0f,
                    .lineWidth = 1.0f
                },
                .multisampleStateDescription = {
                    .sampleCount = SampleCount::Count1,
                    .enableSampleShading = false,
                    .minSampleShading = 1.0f,
                    .sampleMasks = {}
                },
                .depthStencilStateDescription = {
                    .depthFormat = ImageFormat::D32Sfloat,
                    .stencilFormat = ImageFormat::Undefined,
                    .enableDepthTest = false,
                    .enableDepthWrite = false,
                    .depthCompareOp = CompareOp::LessOrEqual,
                    .enableDepthBoundsTest = false,
                    .minDepthBounds = 0.0f,
                    .maxDepthBounds = 1.0f,
                    .enableStencilTest = false,
                },
                .colorBlendStateDescription = {
                    .colorAttachments = {
                        {
                            .format = m_primarySurface->getColorFormat(),
                            .enableBlending = true,
                            .srcColorBlendFactor = BlendFactor::SrcAlpha,
                            .dstColorBlendFactor = BlendFactor::OneMinusSrcAlpha,
                            .colorBlendOp = BlendOp::Add,
                            .srcAlphaBlendFactor = BlendFactor::One,
                            .dstAlphaBlendFactor = BlendFactor::Zero,
                            .alphaBlendOp = BlendOp::Add,
                            .colorWriteMask = ColorComponent::R | ColorComponent::G | ColorComponent::B | ColorComponent::A,
                        }
                    },
                    .enableLogicOp = false,
                    .logicOp = BlendingLogicOp::Copy,
                    .blendConstants = {0.0f, 0.0f, 0.0f, 0.0f}
                }
            }
        );
    }

    virtual void onInitialization() override {}

    virtual void onIteration() override {
        m_frameCount++;
        // Use HSV to RGB for a smooth, slow color cycle along the hue
        float t = m_frameCount * 0.0001f;  // Slow gradient
        float hue = std::fmod(t, 1.0f);    // Hue cycles from 0 to 1
        float s = 0.7f, v = 0.8f;

        // HSV to RGB conversion
        float c = v * s;
        float x = c * (1 - std::fabs(std::fmod(hue * 6.0f, 2.0f) - 1));
        float m = v - c;
        float r, g, b;
        if (hue < 1.0f / 6.0f) {
            r = c;
            g = x;
            b = 0;
        } else if (hue < 2.0f / 6.0f) {
            r = x;
            g = c;
            b = 0;
        } else if (hue < 3.0f / 6.0f) {
            r = 0;
            g = c;
            b = x;
        } else if (hue < 4.0f / 6.0f) {
            r = 0;
            g = x;
            b = c;
        } else if (hue < 5.0f / 6.0f) {
            r = x;
            g = 0;
            b = c;
        } else {
            r = c;
            g = 0;
            b = x;
        }

        ColorValue clearColor{r + m, g + m, b + m, 1.0f};
        std::shared_ptr<Image> drawImage = m_primarySurface->getDrawImage();
        std::shared_ptr<ImageView> drawImageView = m_primarySurface->getDrawImageView();
        std::shared_ptr<Image> depthImage = m_primarySurface->getDepthImage();
        std::shared_ptr<ImageView> depthImageView = m_primarySurface->getDepthImageView();
        Extent2D drawExtent = m_primarySurface->getDrawExtent();
        m_primarySurface->drawFrame([&](RenderCommandList& commandList) {
            commandList.beginRendering(
                {.renderArea
                 = {0, 0, static_cast<int>(drawExtent.width), static_cast<int>(drawExtent.height)},
                 .layerCount = 1,
                 .colorAttachments = {{.imageView = drawImageView,
                                       .loadOp = AttachmentLoadOp::Clear,
                                       .storeOp = AttachmentStoreOp::Store,
                                       .clearColor = clearColor}},
                 .depthAttachment = AttachmentDescription{.imageView = depthImageView,
                                                          .loadOp = AttachmentLoadOp::Clear,
                                                          .storeOp = AttachmentStoreOp::Store,
                                                          .clearDepth = 0.0f}});
            commandList.setViewport(0, 0, drawExtent.width, drawExtent.height);
            commandList.setScissor(0, 0, drawExtent.width, drawExtent.height);
            commandList.bindPipeline(m_pipeline);
            commandList.draw(3, 1, 0, 0);
            commandList.endRendering();
        });
    }

    virtual void onShutdown() override {
        // Cleanup code here
    }

    ~Triangle() {
        // Cleanup code here
    }

  private:
    std::shared_ptr<RenderSurface> m_primarySurface;
    std::shared_ptr<Shader> m_vertexShader;
    std::shared_ptr<Shader> m_fragmentShader;
    std::shared_ptr<GPUPipeline> m_pipeline;

    uint32_t m_frameCount = 0;
};

int main() {
    Triangle game({.title = "Triangle", .extent = {800, 600}});

    try {
        game.run();
    } catch (const std::exception& e) {
        fmt::println("An error occurred: {}", e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}