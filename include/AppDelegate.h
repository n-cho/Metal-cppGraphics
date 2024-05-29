#include <AppKit/AppKit.hpp>
#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <MetalKit/MetalKit.hpp>
#include <QuartzCore/QuartzCore.hpp>

#include "MTKViewDelegate.h"

class AppDelegate : public NS::ApplicationDelegate {
    public:
        ~AppDelegate();

        NS::Menu* createMenuBar();

        virtual void applicationWillFinishLaunching(NS::Notification* pNotification) override;
        virtual void applicationDidFinishLaunching(NS::Notification* pNotification) override;
        virtual bool applicationShouldTerminateAfterLastWindowClosed(
            NS::Application* pSender) override;

    private:
        NS::Window* _pWindow;
        MTK::View* _pMtkView;
        MTL::Device* _pDevice;
        MTKViewDelegate* _pViewDelegate = nullptr;
};
