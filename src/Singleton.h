#pragma once

template<class T> class Singleton
{
public:
    static T& getInstance() 
    {
        static T instance;
        return instance;
    };

public:
    Singleton(T const&) = delete;
    void operator=(T const&) = delete;

    Singleton() {};
};
