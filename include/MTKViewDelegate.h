#include <AppKit/AppKit.hpp>
#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <MetalKit/MetalKit.hpp>
#include <QuartzCore/QuartzCore.hpp>

#include "Renderer.h"

class MTKViewDelegate : public MTK::ViewDelegate {
    public:
        MTKViewDelegate(MTL::Device* pDevice);
        virtual ~MTKViewDelegate() override;
        virtual void drawInMTKView(MTK::View* pView) override;

    private:
        Renderer* _pRenderer;
};
