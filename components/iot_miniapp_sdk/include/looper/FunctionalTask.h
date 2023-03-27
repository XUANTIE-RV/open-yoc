/*
 * Copyright (C) 2019-2021 Alibaba Group Holding Limited
 */
#ifndef ___JQUICK_FUNCTIONAL_TASK_H___
#define ___JQUICK_FUNCTIONAL_TASK_H___

#include "looper/Task.h"
#include <functional>

namespace JQuick
{
namespace std
{
class FunctionalTask : public Task
{
public:

    FunctionalTask(::std::function<void()> func) :
            _func(func)
    {
    }

    void run()
    {
        if (_func) {
            _func();
        }
    };
private:
    ::std::function<void()> _func;
};
}  // namespace std

}  // namespace JQuick

#endif
