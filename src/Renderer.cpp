#include "Renderer.h"

#include <cstring>

#include "Foundation/NSTypes.hpp"
#include "Metal/MTLResource.hpp"

Renderer::Renderer(MTL::Device* pDevice) : _pDevice(pDevice->retain()) {
    _pCommandQueue = _pDevice->newCommandQueue();
    buildShaders();
    buildBuffers();
}

Renderer::~Renderer() {
    _pVertexPositionsBuffer->release();
    _pVertexColorsBuffer->release();
    _pPSO->release();
    _pCommandQueue->release();
    _pDevice->release();
}

const char* readFileToBuffer(const char* filePath) {
    std::ifstream file;
    file.open(filePath);

    file.seekg(0, std::ios::end);
    std::size_t length = file.tellg();
    file.seekg(0, std::ios::beg);

    char* buffer = new char[length + 1];

    file.read(buffer, length);
    buffer[length] = '\0';

    return buffer;
}

void Renderer::buildShaders() {
    using NS::StringEncoding::UTF8StringEncoding;
    const char* shaderSource = readFileToBuffer("../shaders/triangle.metal");

    NS::Error* pError = nullptr;

    MTL::Library* pLibrary =
        _pDevice->newLibrary(NS::String::string(shaderSource, UTF8StringEncoding), nullptr, &pError);
    if (!pLibrary) {
        std::cerr << pError->localizedDescription()->utf8String();
        assert(false);
    }

    MTL::Function* pVertexFunction = pLibrary->newFunction(NS::String::string("vertexMain", UTF8StringEncoding));
    MTL::Function* pFragmentFunction = pLibrary->newFunction(NS::String::string("fragmentMain", UTF8StringEncoding));

    MTL::RenderPipelineDescriptor* pDesc = MTL::RenderPipelineDescriptor::alloc()->init();
    pDesc->setVertexFunction(pVertexFunction);
    pDesc->setFragmentFunction(pFragmentFunction);
    pDesc->colorAttachments()->object(0)->setPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB);

    _pPSO = _pDevice->newRenderPipelineState(pDesc, &pError);
    if (!_pPSO) {
        std::cerr << pError->localizedDescription()->utf8String();
        assert(false);
    }

    pVertexFunction->release();
    pFragmentFunction->release();
    pDesc->release();
    pLibrary->release();

    delete shaderSource;
}

void Renderer::buildBuffers() {
    const size_t NumVertices = 3;

    simd::float3 positions[NumVertices] = {
        {-0.8f, -0.8f, 0.0f},
        { 0.0f, +0.8f, 0.0f},
        {+0.8f, -0.8f, 0.0f}
    };

    simd::float3 colors[NumVertices] = {
        {0.0f, 0.0f, 1.0f},
        {1.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f}
    };

    const size_t positionsDataSize = NumVertices * sizeof(simd::float3);
    const size_t colorsDataSize = NumVertices * sizeof(simd::float3);

    MTL::Buffer* pVertexPositionsBuffer = _pDevice->newBuffer(positionsDataSize, MTL::ResourceStorageModeManaged);
    MTL::Buffer* pVertexColorsBuffer = _pDevice->newBuffer(colorsDataSize, MTL::ResourceStorageModeManaged);

    _pVertexPositionsBuffer = pVertexPositionsBuffer;
    _pVertexColorsBuffer = pVertexColorsBuffer;

    memcpy(_pVertexPositionsBuffer->contents(), positions, positionsDataSize);
    memcpy(_pVertexColorsBuffer->contents(), colors, colorsDataSize);

    _pVertexPositionsBuffer->didModifyRange(NS::Range::Make(0, _pVertexPositionsBuffer->length()));
    _pVertexColorsBuffer->didModifyRange(NS::Range::Make(0, _pVertexColorsBuffer->length()));
}

void Renderer::draw(MTK::View* pView) {
    NS::AutoreleasePool* pPool = NS::AutoreleasePool::alloc()->init();

    MTL::CommandBuffer* pCmd = _pCommandQueue->commandBuffer();
    MTL::RenderPassDescriptor* pRpd = pView->currentRenderPassDescriptor();
    MTL::RenderCommandEncoder* pEnc = pCmd->renderCommandEncoder(pRpd);

    pEnc->setRenderPipelineState(_pPSO);
    pEnc->setVertexBuffer(_pVertexPositionsBuffer, 0, 0);
    pEnc->setVertexBuffer(_pVertexColorsBuffer, 0, 1);
    pEnc->drawPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, NS::UInteger(0), NS::UInteger(3));

    pEnc->endEncoding();
    pCmd->presentDrawable(pView->currentDrawable());
    pCmd->commit();

    pPool->release();
}
