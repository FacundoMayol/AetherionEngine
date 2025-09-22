#pragma once

#include <array>
#include <variant>

#include "aetherion/util/flags.hpp"

namespace aetherion {
    // --- Structs ---

    using ColorClearValue
        = std::variant<std::array<float, 4>, std::array<int32_t, 4>, std::array<uint32_t, 4> >;

    struct DepthStencilValue {
        float depth;
        uint32_t stencil;
    };

    using ClearValue = std::variant<ColorClearValue, DepthStencilValue>;

    // --- Queues ---

    enum class GPUQueueType : FlagType {
        None = 0,
        Graphics = 1 << 0,
        Compute = 1 << 1,
        Transfer = 1 << 2,
        Present = 1 << 3
    };
    DECLARE_FLAG_ENUM(GPUQueueType)

    constexpr GPUQueueTypeFlags ALL_QUEUE_TYPES = GPUQueueType::Graphics | GPUQueueType::Compute
                                                  | GPUQueueType::Transfer | GPUQueueType::Present;

    enum class QueuePresentResultCode {
        Success,
        Suboptimal,
        OutOfDate,
        SurfaceLost,
        DeviceLost,
        Error
    };

    // --- Present ---

    enum class PresentMode {
        Immediate,
        Mailbox,
        Fifo,
        FifoRelaxed,
        SharedDemandRefresh,
        SharedContinuousRefresh
    };

    // --- Swapchain ---

    enum class SwapchainAcquireResultCode {
        Success,
        Suboptimal,
        OutOfDate,
        SurfaceLost,
        DeviceLost,
        Error
    };

    using SampleMask = uint32_t;

    // --- Memory ---

    enum class MemoryUsage { PreferGpu, PreferCpu, Auto };

    enum class AllocationAccessType : FlagType {
        None = 0,
        RandomAccess = 1 << 0,
        SequentialAccess = 1 << 1
    };
    DECLARE_FLAG_ENUM(AllocationAccessType)

    // --- Command & Pipeline ---

    enum class CommandPoolBehavior : FlagType {
        None = 0,
        Transient = 1 << 0,
        ResetCommandBuffer = 1 << 1
    };
    DECLARE_FLAG_ENUM(CommandPoolBehavior)

    enum class CommandBufferUsage : FlagType {
        None = 0,
        OneTimeSubmit = 1 << 0,
        MultipleSubmit = 1 << 1,
        Secondary = 1 << 2
    };
    DECLARE_FLAG_ENUM(CommandBufferUsage)

    enum class CommandBufferLevel { Primary, Secondary };

    enum class PipelineBindPoint { Graphics, Compute };

    enum class PipelineStage : FlagType {
        None = 0,
        TopOfPipe = 1 << 0,
        DrawIndirect = 1 << 1,
        VertexInput = 1 << 2,
        VertexShader = 1 << 3,
        TessellationControlShader = 1 << 4,
        TessellationEvaluationShader = 1 << 5,
        GeometryShader = 1 << 6,
        FragmentShader = 1 << 7,
        EarlyFragmentTests = 1 << 8,
        LateFragmentTests = 1 << 9,
        ColorAttachmentOutput = 1 << 10,
        ComputeShader = 1 << 11,
        Transfer = 1 << 12,
        BottomOfPipe = 1 << 13,
        Host = 1 << 14,
        AllGraphics = 1 << 15,
        AllCommands = 1 << 16
    };
    DECLARE_FLAG_ENUM(PipelineStage)

    enum class AccessType : FlagType {
        None = 0,
        IndirectCommandRead = 1 << 0,
        IndexRead = 1 << 1,
        VertexAttributeRead = 1 << 2,
        UniformRead = 1 << 3,
        InputAttachmentRead = 1 << 4,
        ShaderRead = 1 << 5,
        ShaderWrite = 1 << 6,
        ColorAttachmentRead = 1 << 7,
        ColorAttachmentWrite = 1 << 8,
        DepthStencilAttachmentRead = 1 << 9,
        DepthStencilAttachmentWrite = 1 << 10,
        TransferRead = 1 << 11,
        TransferWrite = 1 << 12,
        HostRead = 1 << 13,
        HostWrite = 1 << 14,
        MemoryRead = 1 << 15,
        MemoryWrite = 1 << 16
    };
    DECLARE_FLAG_ENUM(AccessType);

    // --- Shader ---

    enum class ShaderLanguage { GLSL, HLSL, SPIRV };

    enum class ShaderStage : FlagType {
        None = 0,
        Vertex = 1 << 0,
        Fragment = 1 << 1,
        Compute = 1 << 2,
        Geometry = 1 << 3,
        TessellationControl = 1 << 4,
        TessellationEvaluation = 1 << 5
    };
    DECLARE_FLAG_ENUM(ShaderStage)

    // --- Descriptor ---

    enum class DescriptorType {
        Sampler,
        CombinedImageSampler,
        SampledImage,
        StorageImage,
        UniformTexelBuffer,
        StorageTexelBuffer,
        UniformBuffer,
        StorageBuffer,
        UniformBufferDynamic,
        StorageBufferDynamic,
        InputAttachment
    };

    enum class DescriptorPoolBehavior : FlagType { None = 0, FreeIndividualSets = 1 << 0 };
    DECLARE_FLAG_ENUM(DescriptorPoolBehavior)

    // --- Resources ---

    enum class SharingMode { Exclusive, Concurrent };

    enum class Format {
        Undefined,
        R8Unorm,
        R8Uint,
        R8Srgb,
        R16Unorm,
        R16Uint,
        R16Sfloat,
        R32Uint,
        R32Sfloat,
        R8G8Unorm,
        R8G8Uint,
        R8G8Srgb,
        R16G16Unorm,
        R16G16Uint,
        R16G16Sfloat,
        R32G32Uint,
        R32G32Sfloat,
        R8G8B8A8Unorm,
        R8G8B8A8Uint,
        R8G8B8A8Srgb,
        B8G8R8A8Unorm,
        B8G8R8A8Srgb,
        R16G16B16A16Sfloat,
        R32G32B32A32Sfloat,
        D16Unorm,
        D32Sfloat,
        D24UnormS8Uint
    };

    // --- Buffer ---

    enum class GPUBufferUsage : FlagType {
        None = 0,
        TransferSrc = 1 << 0,
        TransferDst = 1 << 1,
        UniformTexel = 1 << 2,
        StorageTexel = 1 << 3,
        Uniform = 1 << 4,
        Storage = 1 << 5,
        Vertex = 1 << 6,
        Index = 1 << 7,
        Indirect = 1 << 8
    };
    DECLARE_FLAG_ENUM(GPUBufferUsage)

    enum class GPUBufferType { Vertex, Index, Uniform, Storage };

    constexpr size_t WHOLE_BUFFER_SIZE = 0;

    // --- Image ---

    enum class GPUImageLayout {
        Undefined,
        General,
        ColorAttachmentOptimal,
        DepthStencilAttachmentOptimal,
        DepthStencilReadOnlyOptimal,
        ShaderReadOnlyOptimal,
        TransferSrcOptimal,
        TransferDstOptimal,
        Preinitialized,
        PresentSource
    };

    enum class GPUImageType { Tex1d, Tex2d, Tex3d };

    enum class GPUImageViewType {
        Tex1d,
        Tex1dArray,
        Tex2d,
        Tex2dArray,
        TexCube,
        TexCubeArray,
        Tex3d
    };

    enum class ColorSpace {
        SrgbNonlinear,
        DisplayP3Nonlinear,
        ExtendedSrgbLinear,
        ExtendedSrgbNonlinear,
        Bt709Linear,
        Bt709Nonlinear,
        Bt2020Linear,
        DciP3Nonlinear,
        Hdr10St2084,
        Unknown
    };

    enum class ComponentSwizzle { Identity, Zero, One, R, G, B, A };

    enum class GPUImageUsage : FlagType {
        None = 0,
        Sampled = 1 << 0,
        Storage = 1 << 1,
        TransferSrc = 1 << 2,
        TransferDst = 1 << 3,
        ColorAttachment = 1 << 4,
        DepthStencilAttachment = 1 << 5,
        InputAttachment = 1 << 6,
        TransientAttachment = 1 << 7
    };
    DECLARE_FLAG_ENUM(GPUImageUsage)

    enum class GPUImageTiling { Optimal, Linear };

    enum class GPUImageAspect : FlagType {
        None = 0,
        Color = 1 << 0,
        Depth = 1 << 1,
        Stencil = 1 << 2,
        Metadata = 1 << 3
    };
    DECLARE_FLAG_ENUM(GPUImageAspect)

    enum class CubeFace { PositiveX, NegativeX, PositiveY, NegativeY, PositiveZ, NegativeZ };

    enum class CubeImageFace : size_t {
        PositiveX = 0,
        NegativeX = 1,
        PositiveY = 2,
        NegativeY = 3,
        PositiveZ = 4,
        NegativeZ = 5
    };

    struct RenderSurfaceFormat {
        Format imageFormat;
        ColorSpace colorSpace;
    };

    // --- Sampler ---

    enum class SamplerMipmapMode { Nearest, Linear, Anisotropic };

    enum class SamplerAddressMode {
        Repeat,
        MirroredRepeat,
        ClampToEdge,
        ClampToBorder,
        MirrorClampToEdge
    };

    enum class SamplerBorderColor {
        FloatTransparentBlack,
        FloatOpaqueBlack,
        FloatOpaqueWhite,
        IntTransparentBlack,
        IntOpaqueBlack,
        IntOpaqueWhite
    };

    enum class FilterMode { Nearest, Linear };

    enum class AddressMode {
        Repeat,
        MirroredRepeat,
        ClampToEdge,
        ClampToBorder,
        MirrorClampToEdge
    };

    struct GPUImageRangeDescription {
        uint32_t baseArrayLayer = 0;
        uint32_t layerCount = 1;
        uint32_t baseMipLevel = 0;
        uint32_t mipLevelCount = 1;
    };

    struct GPUImageSubresourceDescription {
        GPUImageAspectFlags aspectMask = GPUImageAspect::Color;
        GPUImageRangeDescription range;
    };

    // --- Render Pass / Attachment ---

    enum class AttachmentLoadOp { Load, Clear, DontCare };

    enum class AttachmentStoreOp { Store, DontCare };

    enum class ResolveMode : FlagType {
        None = 0,
        SampleZero = 1 << 0,
        Average = 1 << 1,
        Min = 1 << 2,
        Max = 1 << 3
    };
    DECLARE_FLAG_ENUM(ResolveMode)

    // --- Blending ---

    enum class BlendFactor {
        Zero,
        One,
        SrcColor,
        OneMinusSrcColor,
        DstColor,
        OneMinusDstColor,
        SrcAlpha,
        OneMinusSrcAlpha,
        DstAlpha,
        OneMinusDstAlpha,
        ConstantColor,
        OneMinusConstantColor,
        ConstantAlpha,
        OneMinusConstantAlpha
    };

    enum class BlendOp { Add, Subtract, ReverseSubtract, Min, Max };

    enum class ColorComponent : FlagType {
        None = 0,
        R = 1 << 0,
        G = 1 << 1,
        B = 1 << 2,
        A = 1 << 3
    };
    DECLARE_FLAG_ENUM(ColorComponent);

    enum class BlendingLogicOp {
        Clear,
        And,
        AndReverse,
        Copy,
        AndInverted,
        NoOp,
        Xor,
        Or,
        Nor,
        Equivalent,
        Invert,
        OrReverse,
        CopyInverted,
        OrInverted,
        Nand,
        Set
    };

    // --- Rasterization ---

    enum class PrimitiveTopology {
        PointList,
        LineList,
        LineStrip,
        LineLoop,
        TriangleList,
        TriangleStrip,
        TriangleFan
    };

    enum class PolygonMode { Fill, Line, Point };

    enum class CullMode { None, Front, Back, FrontAndBack };

    enum class FrontFace { Clockwise, CounterClockwise };

    // --- Depth/Stencil ---

    enum class CompareOp {
        Never,
        Less,
        Equal,
        LessOrEqual,
        Greater,
        NotEqual,
        GreaterOrEqual,
        Always
    };

    enum class StencilOp {
        Keep,
        Zero,
        Replace,
        IncrementAndClamp,
        DecrementAndClamp,
        Invert,
        IncrementAndWrap,
        DecrementAndWrap
    };

    // --- Vertex description ---

    enum class VertexInputRate { Vertex, Instance };

    enum class VertexAttributeFormat {
        Float,
        Float2,
        Float3,
        Float4,
        Int,
        Int2,
        Int3,
        Int4,
        UInt,
        UInt2,
        UInt3,
        UInt4,
        Mat2,
        Mat3,
        Mat4
    };

    enum class IndexType { UInt16, UInt32 };

    // --- Multisampling ---

    enum class SampleCount {
        Count1 = 1,
        Count2 = 2,
        Count4 = 4,
        Count8 = 8,
        Count16 = 16,
        Count32 = 32,
        Count64 = 64
    };
}  // namespace aetherion