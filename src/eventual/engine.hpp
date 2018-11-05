/*
 * MIT License
 * 
 * Copyright (c) 2018 jipeng wu
 * <recvfromsockaddr at gmail dot com>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include "threadpool.hpp"

#define NR_THREADS 32

namespace eventual{

class promise_engine
{
public:
    static promise_engine& instance()
    {
        static promise_engine instance;
        return instance;
    }
    void run_fulfill_asyncrhonously(std::shared_ptr<promise_meta_t> p, value_t v)
    {
        threadpool_->run([p, v]{  // ref capture is incorrect
            p->fulfill(v); 
        });
    }
    void run_reject_asyncrhonously(std::shared_ptr<promise_meta_t> p, reason_t r)
    {
        threadpool_->run([p, r]{
            p->reject(r);
        });
    }   
protected:
    promise_engine() : threadpool_(std::make_unique<threadpool>(NR_THREADS))
    {}
private:
    std::unique_ptr<threadpool> threadpool_;
};


}
