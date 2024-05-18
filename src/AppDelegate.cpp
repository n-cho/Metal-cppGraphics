#include "AppDelegate.h"

AppDelegate::~AppDelegate() {
    _pMtkView->release();
    _pWindow->release();
    _pDevice->release();
    delete _pViewDelegate;
}

NS::Menu* AppDelegate::createMenuBar() {
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
    pAppMenuItem->setSubmenu(pAppMenu);

    NS::MenuItem* pWindowMenuItem = NS::MenuItem::alloc()->init();
    NS::Menu* pWindowMenu = NS::Menu::alloc()->init(NS::String::string("Window", UTF8StringEncoding));

    SEL closeWindowCallback = NS::MenuItem::registerActionCallback("windowClose", [](void*, SEL, const NS::Object*) {
        auto pApp = NS::Application::sharedApplication();
        pApp->windows()->object<NS::Window>(0)->close();
    });
    NS::MenuItem* pCloseWindowItem = pWindowMenu->addItem(
        NS::String::string("Close Window", UTF8StringEncoding),
        closeWindowCallback,
        NS::String::string("w", UTF8StringEncoding));
    pCloseWindowItem->setKeyEquivalentModifierMask(NS::EventModifierFlagCommand);

    pWindowMenuItem->setSubmenu(pWindowMenu);

    pMainMenu->addItem(pAppMenuItem);
    pMainMenu->addItem(pWindowMenuItem);

    pAppMenu->release();
    pAppMenuItem->release();
    pWindowMenu->release();
    pWindowMenuItem->release();

    return pMainMenu->autorelease();
}

void AppDelegate::applicationWillFinishLaunching(NS::Notification* pNotification) {
    NS::Menu* pMenu = createMenuBar();
    NS::Application* pApp = reinterpret_cast<NS::Application*>(pNotification->object());
    pApp->setMainMenu(pMenu);
    pApp->setActivationPolicy(NS::ActivationPolicy::ActivationPolicyRegular);
}

void AppDelegate::applicationDidFinishLaunching(NS::Notification* pNotification) {
    CGRect frame = (CGRect){
        {100.0, 100.0},
        {640.0, 640.0}
    };

    _pWindow = NS::Window::alloc()->init(
        frame, NS::WindowStyleMaskClosable | NS::WindowStyleMaskTitled, NS::BackingStoreBuffered, false);

    _pDevice = MTL::CreateSystemDefaultDevice();

    _pMtkView = MTK::View::alloc()->init(frame, _pDevice);
    _pMtkView->setColorPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB);
    _pMtkView->setClearColor(MTL::ClearColor::Make(0.0, 0.0, 0.0, 1.0));

    _pViewDelegate = new MTKViewDelegate(_pDevice);
    _pMtkView->setDelegate(_pViewDelegate);

    _pWindow->setContentView(_pMtkView);
    _pWindow->setTitle(NS::String::string("Graphics", NS::StringEncoding::UTF8StringEncoding));

    _pWindow->makeKeyAndOrderFront(nullptr);

    NS::Application* pApp = reinterpret_cast<NS::Application*>(pNotification->object());
    pApp->activateIgnoringOtherApps(true);
}

bool AppDelegate::applicationShouldTerminateAfterLastWindowClosed(NS::Application* pSender) { return true; }
