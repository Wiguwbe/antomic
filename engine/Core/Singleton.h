#pragma once

namespace Engine {
    template <class T>
    class Singleton
    {
    public:
        static T &GetInstance()
        {
            static T instance;
            return instance;
        };

    public:
        Singleton(T const &) = delete;
        void operator=(T const &) = delete;

        Singleton(){};
    };
}
