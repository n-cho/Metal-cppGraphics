#include "Renderer.h"

#include <simd/vector_types.h>

#include <cstring>

#include "Foundation/NSError.hpp"
#include "Foundation/NSString.hpp"
#include "Foundation/NSTypes.hpp"
#include "Metal/MTLLibrary.hpp"
#include "Metal/MTLResource.hpp"
#include "Metal/MTLStageInputOutputDescriptor.hpp"

Renderer::Renderer(MTL::Device* pDevice) : _pDevice(pDevice->retain()) {
    _pCommandQueue = _pDevice->newCommandQueue();
    buildShaders();
    buildBuffers();
}

Renderer::~Renderer() {
    _pVertexPositionsBuffer->release();
    _pVertexColorsBuffer->release();
    _pVertexIndicesBuffer->release();
    _pPSO->release();
    _pCommandQueue->release();
    _pDevice->release();
}

void Renderer::buildShaders() {
    using NS::StringEncoding::UTF8StringEncoding;

    // Store error information.
    NS::Error* pError = nullptr;

    // Read shader code from file.
    std::ifstream file;
    file.open("shaders/square.metal");

    file.seekg(0, std::ios::end);
    std::size_t length = file.tellg();
    file.seekg(0, std::ios::beg);

    char* shaderCode = new char[length + 1];

    file.read(shaderCode, length);
    shaderCode[length] = '\0';

    // Store shader functions in MTL::Library.
    MTL::Library* pLibrary =
        _pDevice->newLibrary(NS::String::string(shaderCode, UTF8StringEncoding), nullptr, &pError);
    if (!pLibrary) {
        std::cerr << pError->localizedDescription()->utf8String();
        assert(false);
    }

    MTL::Function* pVertexFunction =
        pLibrary->newFunction(NS::String::string("vertexMain", UTF8StringEncoding));
    MTL::Function* pFragmentFunction =
        pLibrary->newFunction(NS::String::string("fragmentMain", UTF8StringEncoding));

    // Store MTL::RenderPipelineState arguments in MTL::RenderPipelineDescriptor.
    MTL::RenderPipelineDescriptor* pDesc = MTL::RenderPipelineDescriptor::alloc()->init();
    pDesc->setVertexFunction(pVertexFunction);
    pDesc->setFragmentFunction(pFragmentFunction);
    pDesc->colorAttachments()->object(0)->setPixelFormat(
        MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB);

    // Generate new MTL::RenderPipelineState.
    _pPSO = _pDevice->newRenderPipelineState(pDesc, &pError);
    if (!_pPSO) {
        std::cerr << pError->localizedDescription()->utf8String();
        assert(false);
    }

    delete[] shaderCode;

    pVertexFunction->release();
    pFragmentFunction->release();
    pDesc->release();
    pLibrary->release();
}

void Renderer::buildBuffers() {
    const size_t NumVertices = 4;

    simd::float4 positions[NumVertices] = {{+0.8f, +0.8f, 0.0f, 1.0f},
                                           {-0.8f, +0.8f, 0.0f, 1.0f},
                                           {-0.8f, -0.8f, 0.0f, 1.0f},
                                           {+0.8f, -0.8f, 0.0f, 1.0f}};

    simd::float4 colors[NumVertices] = {{0.0f, 0.0f, 1.0f, 1.0f},
                                        {1.0f, 0.0f, 0.0f, 1.0f},
                                        {0.0f, 1.0f, 0.0f, 1.0f},
                                        {0.0f, 0.0f, 1.0f, 1.0f}};

    simd::ushort1 indices[NumVertices + 2] = {0, 1, 2, 2, 3, 0};

    const size_t positionsDataSize = NumVertices * sizeof(simd::float3);
    const size_t colorsDataSize = NumVertices * sizeof(simd::float3);
    const size_t indicesDataSize = (NumVertices + 2) * sizeof(simd::ushort1);

    MTL::Buffer* pVertexPositionsBuffer =
        _pDevice->newBuffer(positionsDataSize, MTL::ResourceStorageModeManaged);
    MTL::Buffer* pVertexColorsBuffer =
        _pDevice->newBuffer(colorsDataSize, MTL::ResourceStorageModeManaged);
    MTL::Buffer* pVertexIndicesBuffer =
        _pDevice->newBuffer(indicesDataSize, MTL::ResourceStorageModeManaged);

    _pVertexPositionsBuffer = pVertexPositionsBuffer;
    _pVertexColorsBuffer = pVertexColorsBuffer;
    _pVertexIndicesBuffer = pVertexIndicesBuffer;

    memcpy(_pVertexPositionsBuffer->contents(), positions, positionsDataSize);
    memcpy(_pVertexColorsBuffer->contents(), colors, colorsDataSize);
    memcpy(_pVertexIndicesBuffer->contents(), indices, indicesDataSize);

    _pVertexPositionsBuffer->didModifyRange(NS::Range::Make(0, _pVertexPositionsBuffer->length()));
    _pVertexColorsBuffer->didModifyRange(NS::Range::Make(0, _pVertexColorsBuffer->length()));
    _pVertexIndicesBuffer->didModifyRange(NS::Range::Make(0, _pVertexIndicesBuffer->length()));
}

void Renderer::draw(MTK::View* pView) {
    NS::AutoreleasePool* pPool = NS::AutoreleasePool::alloc()->init();

    MTL::CommandBuffer* pCmd = _pCommandQueue->commandBuffer();
    MTL::RenderPassDescriptor* pRpd = pView->currentRenderPassDescriptor();
    MTL::RenderCommandEncoder* pEnc = pCmd->renderCommandEncoder(pRpd);

    pEnc->setRenderPipelineState(_pPSO);
    pEnc->setVertexBuffer(_pVertexPositionsBuffer, 0, 0);
    pEnc->setVertexBuffer(_pVertexColorsBuffer, 0, 1);
    // pEnc->drawPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, NS::UInteger(0),
    //                      NS::UInteger(3));
    pEnc->drawIndexedPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, NS::UInteger(6),
                                MTL::IndexType::IndexTypeUInt16, _pVertexIndicesBuffer,
                                NS::UInteger(0), NS::UInteger(6));

    pEnc->endEncoding();
    pCmd->presentDrawable(pView->currentDrawable());
    pCmd->commit();

    pPool->release();
}
