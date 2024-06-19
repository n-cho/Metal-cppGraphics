#include "Renderer.h"

#include <simd/vector_types.h>
#include <sys/_types/_u_int32_t.h>

#include <cstring>

#include "Foundation/NSDictionary.hpp"
#include "Foundation/NSError.hpp"
#include "Foundation/NSString.hpp"
#include "Foundation/NSTypes.hpp"
#include "Metal/MTLLibrary.hpp"
#include "Metal/MTLResource.hpp"
#include "Metal/MTLStageInputOutputDescriptor.hpp"
#include "Metal/MTLTexture.hpp"
#include "MetalKit/MTKTextureLoader.hpp"
#include "MetalKit/MetalKitPrivate.hpp"

Renderer::Renderer(MTL::Device *pDevice) : _pDevice(pDevice->retain())
{
    _pCommandQueue = _pDevice->newCommandQueue();
    buildShaders();
    buildTextures();
    buildBuffers();
}

Renderer::~Renderer()
{
    _pTexture->release();
    _pVertexPositionsBuffer->release();
    _pVertexTextureCoordinatesBuffer->release();
    _pVertexIndicesBuffer->release();
    _pPSO->release();
    _pCommandQueue->release();
    _pDevice->release();
}

const char *readFileToCharArray(const char *filepath)
{
    std::ifstream file;
    file.open(filepath);

    file.seekg(0, std::ios::end);
    std::size_t length = file.tellg();
    file.seekg(0, std::ios::beg);

    char *charArray = new char[length + 1];

    file.read(charArray, length);
    charArray[length] = '\0';

    return charArray;
}

void Renderer::buildShaders()
{
    using NS::StringEncoding::UTF8StringEncoding;

    const char *pSource = readFileToCharArray("shaders/square.metal");
    MTL::CompileOptions *pOptions = nullptr;
    NS::Error *pError = nullptr;

    MTL::Library *pLibrary = _pDevice->newLibrary(NS::String::string(pSource, UTF8StringEncoding), pOptions, &pError);
    if (!pLibrary)
    {
        std::cerr << pError->localizedDescription()->utf8String();
        assert(false);
    }

    MTL::Function *pVertexFunction = pLibrary->newFunction(NS::String::string("vertexMain", UTF8StringEncoding));
    MTL::Function *pFragmentFunction = pLibrary->newFunction(NS::String::string("fragmentMain", UTF8StringEncoding));

    MTL::RenderPipelineDescriptor *pDesc = MTL::RenderPipelineDescriptor::alloc()->init();
    pDesc->setVertexFunction(pVertexFunction);
    pDesc->setFragmentFunction(pFragmentFunction);
    pDesc->colorAttachments()->object(0)->setPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB);

    _pPSO = _pDevice->newRenderPipelineState(pDesc, &pError);
    if (!_pPSO)
    {
        std::cerr << pError->localizedDescription()->utf8String();
        assert(false);
    }

    delete[] pSource;

    pVertexFunction->release();
    pFragmentFunction->release();
    pDesc->release();
    _pShaderLibrary = pLibrary;
}

void Renderer::buildTextures()
{
    using NS::StringEncoding::UTF8StringEncoding;

    NS::Error *pError = nullptr;
    MTK::TextureLoader *pTextureLoader = MTK::TextureLoader::alloc()->init(_pDevice);

    MTL::Texture *pTexture = pTextureLoader->newTexture(
        NS::URL::fileURLWithPath(NS::String::string("assets/stone.png", UTF8StringEncoding)), nullptr, &pError);
    if (!pTexture)
    {
        std::cerr << pError->localizedDescription()->utf8String();
        assert(false);
    }

    _pTexture = pTexture;
}

void Renderer::buildBuffers()
{
    const size_t NumVertices = 4;

    simd::float4 positions[NumVertices] = {
        {+0.8f, +0.8f, 0.0f, 1.0f}, {-0.8f, +0.8f, 0.0f, 1.0f}, {-0.8f, -0.8f, 0.0f, 1.0f}, {+0.8f, -0.8f, 0.0f, 1.0f}};

    simd::float2 textureCoordinates[NumVertices] = {{1.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f}};

    simd::ushort1 indices[NumVertices + 2] = {0, 1, 2, 2, 3, 0};

    const size_t positionsDataSize = NumVertices * sizeof(simd::float3);
    const size_t textureCoordinatesDataSize = NumVertices * sizeof(simd::float3);
    const size_t indicesDataSize = (NumVertices + 2) * sizeof(simd::ushort1);

    MTL::Buffer *pVertexPositionsBuffer = _pDevice->newBuffer(positionsDataSize, MTL::ResourceStorageModeManaged);
    MTL::Buffer *pVertexTextureCoordinatesBuffer =
        _pDevice->newBuffer(textureCoordinatesDataSize, MTL::ResourceStorageModeManaged);
    MTL::Buffer *pVertexIndicesBuffer = _pDevice->newBuffer(indicesDataSize, MTL::ResourceStorageModeManaged);

    _pVertexPositionsBuffer = pVertexPositionsBuffer;
    _pVertexTextureCoordinatesBuffer = pVertexTextureCoordinatesBuffer;
    _pVertexIndicesBuffer = pVertexIndicesBuffer;

    memcpy(_pVertexPositionsBuffer->contents(), positions, positionsDataSize);
    memcpy(_pVertexTextureCoordinatesBuffer->contents(), textureCoordinates, textureCoordinatesDataSize);
    memcpy(_pVertexIndicesBuffer->contents(), indices, indicesDataSize);

    _pVertexPositionsBuffer->didModifyRange(NS::Range::Make(0, _pVertexPositionsBuffer->length()));
    _pVertexTextureCoordinatesBuffer->didModifyRange(NS::Range::Make(0, _pVertexTextureCoordinatesBuffer->length()));
    _pVertexIndicesBuffer->didModifyRange(NS::Range::Make(0, _pVertexIndicesBuffer->length()));
}

void Renderer::draw(MTK::View *pView)
{
    NS::AutoreleasePool *pPool = NS::AutoreleasePool::alloc()->init();

    MTL::CommandBuffer *pCmd = _pCommandQueue->commandBuffer();
    MTL::RenderPassDescriptor *pRpd = pView->currentRenderPassDescriptor();
    MTL::RenderCommandEncoder *pEnc = pCmd->renderCommandEncoder(pRpd);

    pEnc->setRenderPipelineState(_pPSO);
    pEnc->setVertexBuffer(_pVertexPositionsBuffer, 0, 0);
    pEnc->setVertexBuffer(_pVertexTextureCoordinatesBuffer, 0, 1);

    pEnc->setFragmentTexture(_pTexture, 0);

    pEnc->drawIndexedPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, NS::UInteger(6),
                                MTL::IndexType::IndexTypeUInt16, _pVertexIndicesBuffer, NS::UInteger(0),
                                NS::UInteger(6));

    pEnc->endEncoding();
    pCmd->presentDrawable(pView->currentDrawable());
    pCmd->commit();

    pPool->release();
}
