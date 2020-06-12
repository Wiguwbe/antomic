#pragma once
#include <string>

class Application
{

public:
    Application() : Application("Application", 640, 480, false){};
    Application(const char *name) : Application(name, 640, 480, false){};
    Application(const char *name, uint32_t width, uint32_t height, bool fullscreen);

    virtual ~Application();

    virtual bool init();
    virtual void shutdown();
    virtual void update() = 0;

    void run();

private:
    std::string mName;
    uint32_t mWidth;
    uint32_t mHeight;
    bool mFullscreen;
    bool mRunning;
};
