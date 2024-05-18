#include <simd/simd.h>

#include <AppKit/AppKit.hpp>
#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <MetalKit/MetalKit.hpp>
#include <QuartzCore/QuartzCore.hpp>
#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>

class Renderer {
    public:
        Renderer(MTL::Device* pDevice);
        ~Renderer();

        void buildShaders();
        void buildBuffers();
        void draw(MTK::View* pView);

    private:
        MTL::Device* _pDevice;
        MTL::CommandQueue* _pCommandQueue;
        MTL::RenderPipelineState* _pPSO;
        MTL::Buffer* _pVertexPositionsBuffer;
        MTL::Buffer* _pVertexColorsBuffer;
};
