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

#include "engine.hpp"
#include "zero_copy_value.hpp"

/*
Eventual: A Promise-like Async Programming Library
    
    1. Library-provided reject and fulfill are nonblocking operations; they will be completed asynchronously in background threads.

    2. A promise object is released shortly after being resolved (fulfilled or rejected).  

    3. The life cycle of a promise object is automatically managed by the library. It's safe (actually recommended, if you don't need to append more then-s to it) for user code to tears down a promise immediately after its creation or before its resolution. 
    
    4. It's safe and cheap to pass, store and copy promise_t, value_t and reason_t by value, just like scalar types.

    5. then could be called on the same promise multiple times 
       a.then(m).then(n) will ensure n-after-m order
       but a.then(m); a.then(n) will not

    6. Note that user callbacks in eventual::promise_t objects are automatically arranged to run in background threads. This library intends to help users write async code on their own rather than merely call async routines(ajax or timer) in the main thread like what promise does in Javascript.

*/

namespace eventual{

using value_t = zero_copy_value;
using reason_t = std::string; 

class promise_t{
public:

    using func = std::function<void()>;
    // on fullfilled callback: should be provided by user via then
    using on_fullfilled_func = std::function<value_t(value_t)>;
    // on rejected callback: should be provided by user via then
    using on_rejected_func = std::function<value_t(reason_t)>;
    // fulfill operation: provided by library, asynchronous
    using fulfill_func = std::function<void(value_t)>;
    // reject operation: provided by library, asynchronous
    using reject_func = std::function<void(reason_t)>;
    // initial function: should be provided by user via create_promise
    using init_func = std::function<void(fulfill_func, reject_func)>;
    // create a fulfilled promise 
    static promise_t create_fulfilled_promise(value_t v);
    // create a rejected promise
    static promise_t create_rejected_promise(reason_t r);
    // [interface 1] create a initial promise
    promise_t(init_func);
    // [interface 2] what to do next 
    promise_t then(on_fullfilled_func f, on_rejected_func r);

    promise_t(const promise_t& d) : meta(d.meta){}
    promise_t& operator= (promise_t&&d) noexcept{
        meta.swap(d.meta);
        return *this;
    }
    promise_t& operator= (const promise_t& d) noexcept{
        meta=d.meta;
        return *this;
    }
    bool operator== (const promise_t& d) const noexcept{
        return meta==d.meta;
    }
    bool operator!= (const promise_t& d) const noexcept{
        return !(*this==d);
    }

private:
    struct then_t;  
    class promise_meta_t; // impl class

    promise_t(std::shared_ptr<promise_meta_t> m):
    meta(m){}
    enum state_t : int{ 
        pending = 0,
        fulfilled = 1,
        rejected =2
    };
    std::shared_ptr<promise_meta_t> meta;
    // promise resolution procedure.
    static void resolve(std::shared_ptr<promise_meta_t> p, value_t x);
    static void trigger_on_reject(std::shared_ptr<promise_meta_t> promise_, on_rejected_func on_rejected_, reason_t r);
    static void trigger_on_fulfill(std::shared_ptr<promise_meta_t> promise_, on_fullfilled_func on_fullfilled_, value_t v);


};

}

