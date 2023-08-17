#pragma once
#include <chrono>
#include <string>
#include <iostream>

class ScopeTimer {
public:
    ScopeTimer(const std::string& describe, std::chrono::system_clock::duration* duration = nullptr, bool enable = true)
        : _describe(describe), 
          _duration(duration),  
          _enable(enable) {
        if ((_describe.empty()
                || nullptr == _duration)
                && !_enable) {
            return;
        }   

        _time_begin = std::chrono::system_clock::now();
    }   

    ScopeTimer(const ScopeTimer&) = delete;
    ScopeTimer& operator=(const ScopeTimer&) = delete;

    ~ScopeTimer() {
        if ((_describe.empty()
                || nullptr == _duration)
                && !_enable) {
            return;
        }   

        std::chrono::system_clock::duration duration = std::chrono::system_clock::now() - _time_begin;

        if (!_describe.empty()) {
            std::cout << _describe << " cost time: " << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms" << std::endl;
        }   

        if (nullptr != _duration) {
            *_duration = duration;
        }   
    }   

private:
    std::string _describe;
    std::chrono::system_clock::duration* _duration;
    bool _enable;
    std::chrono::system_clock::time_point _time_begin;
};