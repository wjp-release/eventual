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
Eventual: Promises/A+-conformant Async Programming Framework
    
    1. Library-provided operations (then, reject, and fulfill) are nonblocking; they will be completed asynchronously in background threads
    
    2. The life cycle of eventual::promise_t is automatically managed by the library.
    
    3. It's safe (and cheap) to pass or store promise_t, value_t and reason_t by value, just like primitive types.
*/

namespace eventual{

using value_t = zero_copy_value;
using reason_t = std::string; // it's efficient as long as you don't dump too much error info; 

class promise_t{
public:
    // promise resolution procedure.
    static void resolve(std::shared_ptr<promise_meta_t> p, value_t x)
    {
        // If x is a promise, it attempts to make promise adopt the state of x. 
        if( x.has_same_type(typeid(promise_t)) )
        {
            promise_t promise_x=x.data<promise_t>();
            // If x==promise, reject promise 
            if(promise_x.meta==p){
                promise_engine::instance().run_reject_asyncrhonously(p, reason_t("It's illogical for a promise to adopt the state of itself!"));
                return;
            }
            // adopt the state of promise_x
            // todo
        }else{
            // Otherwise, it fulfills promise with value_t x.
            promise_engine::instance().run_fulfill_asyncrhonously(p,x);
            return;
        }
    }


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
    promise_t(init_func init) : meta(std::make_shared<promise_meta_t>()){
        if(init==nullptr) throw std::runtime_error("promise init_func should not be nullptr!");
        init( 
            [this](value_t v){ 
                run_fulfill(meta, v);  
            },
            [this](reason_t r){ 
                run_reject(meta, r);
            }
        );        
    }
   
    // return a promise that is pending (called by then()) 
    static promise_t create_promise()
    {
        return std::make_shared<promise>();
    }
    // return a promise that is pending (immediately runs init_func) 
    static promise_t create_promise(init_func init)
    {
        return std::make_shared<promise>(init);
    }
    // return a promise that is fulfilled with given value
    static promise_t create_fulfilled_promise(value_t v)
    {
        return std::make_shared<promise>(v);
    }
    // return a promise that is rejected with given reason
    static promise_t create_rejected_promise(reason_t r)
    {
        return std::make_shared<promise>(r);
    }

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
    struct promise_meta_t; // status of promise
    std::shared_ptr<promise_meta_t> meta;
};

}

