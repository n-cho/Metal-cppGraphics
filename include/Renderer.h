#include <AppKit/AppKit.hpp>
#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <MetalKit/MetalKit.hpp>
#include <QuartzCore/QuartzCore.hpp>

class Renderer {
    public:
        Renderer(MTL::Device* pDevice);
        ~Renderer();

        void draw(MTK::View* pView);

    private:
        MTL::Device* _pDevice;
        MTL::CommandQueue* _pCommandQueue;
};
