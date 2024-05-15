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

class MyMTKViewDelegate : public MTK::ViewDelegate {
    public:
        MyMTKViewDelegate(MTL::Device* pDevice);
        virtual ~MyMTKViewDelegate() override;
        virtual void drawInMTKView(MTK::View* pView) override;

    private:
        Renderer* _pRenderer;
};

class MyAppDelegate : public NS::ApplicationDelegate {
    public:
        ~MyAppDelegate();

        NS::Menu* createMenuBar();

        virtual void applicationWillFinishLaunching(NS::Notification* pNotification) override;
        virtual void applicationDidFinishLaunching(NS::Notification* pNotification) override;
        virtual bool applicationShouldTerminateAfterLastWindowClosed(NS::Application* pSender) override;

    private:
        NS::Window* _pWindow;
        MTK::View* _pMtkView;
        MTL::Device* _pDevice;
        MyMTKViewDelegate* _pViewDelegate = nullptr;
};

int main(int argc, char* argv[]) {
    NS::AutoreleasePool* pAutoreleasePool = NS::AutoreleasePool::alloc()->init();

    MyAppDelegate del;

    NS::Application* pSharedApplication = NS::Application::sharedApplication();
    pSharedApplication->setDelegate(&del);
    pSharedApplication->run();

    pAutoreleasePool->release();

    return 0;
}

MyAppDelegate::~MyAppDelegate() {
    _pMtkView->release();
    _pWindow->release();
    _pDevice->release();
    delete _pViewDelegate;
}

NS::Menu* MyAppDelegate::createMenuBar() {
    using NS::StringEncoding::UTF8StringEncoding;

    NS::Menu* pMainMenu = NS::Menu::alloc()->init();
    NS::MenuItem* pAppMenuItem = NS::MenuItem::alloc()->init();
    NS::Menu* pAppMenu = NS::Menu::alloc()->init(NS::String::string("Appname", UTF8StringEncoding));

    NS::String* appName = NS::RunningApplication::currentApplication()->localizedName();
    NS::String* quitItemName = NS::String::string("Quit ", UTF8StringEncoding)->stringByAppendingString(appName);

    SEL quitCallback = NS::MenuItem::registerActionCallback("appQuit", [](void*, SEL, const NS::Object* pSender) {
        auto pApp = NS::Application::sharedApplication();
        pApp->terminate(pSender);
    });

    NS::MenuItem* pAppQuitItem =
        pAppMenu->addItem(quitItemName, quitCallback, NS::String::string("q", UTF8StringEncoding));
    pAppQuitItem->setKeyEquivalentModifierMask(NS::EventModifierFlagCommand);
    pAppMenu->setSubmenu(pAppMenu);

    NS::MenuItem* pWindowMenuItem = NS::MenuItem::alloc()->init();
    NS::Menu* pWindowMenu = NS::Menu::alloc()->init(NS::String::string("Window", UTF8StringEncoding));
}
