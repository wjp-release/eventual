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
Eventual: A Promises/A+-conformant Async Programming Library
    
    1. Library-provided reject and fulfill are nonblocking operations; they will be completed asynchronously in background threads.

    2. A promise object is released shortly after being resolved (fulfilled or rejected).  

    3. The life cycle of a promise object is automatically managed by the library. It's safe (actually recommended, if you don't need to append more then-s to it) for user code to tears down a promise immediately after its creation or before its resolution. 
    
    4. It's safe and cheap to pass, store and copy promise_t, value_t and reason_t by value, just like primitive types.
*/

namespace eventual{

using value_t = zero_copy_value;
using reason_t = std::string; // it's efficient as long as you don't dump too much error info; 

class promise_t{
public:
    // promise resolution procedure.
    static void resolve(std::shared_ptr<promise_meta_t> p, value_t x);

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
    
    // create a pending promise without init func (created by then())
    promise_t() : meta(std::make_shared<promise_meta_t>()){}
    // create a fulfilled promise 
    promise_t(value_t v) : meta(std::make_shared<promise_meta_t>(v)){}
    // create a rejected promise
    promise_t(reason_t r) : meta(std::make_shared<promise_meta_t>(v)){}
    // create a initial promise (pending; expose fulfill/reject operations to user code; runs user cb immediately)
    promise_t(init_func);
   

    // assign/copy operations/ctors will increase meta's ref count by 1 rather than make an actual copy/clone of this object
    promise_t(const promise_t d) : meta(d.meta){}
    promise_t& operator= (promise_t&&d) noexcept{
        meta.swap(d.meta);
        return *this;
    }
    promise_t& operator= (const promise_t& d) noexcept{
        meta=d.meta;
        return *this;
    }
    bool operator== (const promise_t& d) noexcept{
        return meta==d.meta;
    }
    bool operator!= (const promise_t& d) const noexcept{
        return !(*this==d);
    }

private:
    enum state_t : int{ 
        pending = 0,
        fulfilled = 1,
        rejected =2
    };
    struct then_t; // tracks then-ed promises and their onfulfill/onreject callbacks
    class promise_meta_t; // implementation details, should only be accessed by library
    std::shared_ptr<promise_meta_t> meta;
};

}

