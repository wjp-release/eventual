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

#include <functional>
#include <list>
#include <memory>
#include "zero_copy_value.hpp"
#include <stdexcept>
#include <mutex>

namespace eventual{

// Promises/A+ reforged in modern C++
// see https://promisesaplus.com/

class promise;

using value_t = zero_copy_value;
using reason_t = std::string;

/*
    If x==promise, reject promise with a reason says that "promises cannot adopt the state of themselves!".
    If x is a promise, it attempts to make promise adopt the state of x. Otherwise, it fulfills promise with value_t x.
*/

// promise resolution procedure. 
extern void resolve(std::shared_ptr<promise>, value_t);

// promise resolution procedure. 
void resolve(std::shared_ptr<promise> promise_, value_t x)
{
    std::cout<<x.type()<<std::endl;

}

class promise{
public:
    using func = std::function<void()>;
    using on_fullfilled_func = std::function<value_t(value_t)>;
    using on_rejected_func = std::function<value_t(reason_t)>;
    using fulfill_func = std::function<void(value_t)>;
    using reject_func = std::function<void(reason_t)>;
    using init_func = std::function<void(fulfill_func, reject_func)>; 
    // all promises are created with shared_ptrs 
    static std::shared_ptr<promise> create_promise(init_func init)
    {
        return std::make_shared<promise>(init);
    }
    // return a promise that is fulfilled with given value
    static std::shared_ptr<promise> create_fulfilled_promise(value_t v)
    {
        return std::make_shared<promise>(v);
    }
    // return a promise that is rejected with given reason
    static std::shared_ptr<promise> create_rejected_promise(reason_t r)
    {
        return std::make_shared<promise>(r);
    }
protected:
    // return a promise that is pending (called by then()) 
    static std::shared_ptr<promise> create_promise()
    {
        return std::make_shared<promise>();
    }
    // create a pending promise without init func (created by then())
    promise(){}
    // create a fulfilled promise 
    promise(value_t v): value(v), state(fulfilled){}
    // create a rejected promise
    promise(reason_t r): reason(r), state(rejected){}
    // create a pending promise: supply init function with this promise object's fulfill/reject methods and execute it on construction  
    promise(init_func init){
        if(init==nullptr) throw std::runtime_error("promise init_func should not be nullptr!");
        init( 
            [this](value_t v){
                fulfill(v);
            },
            [this](reason_t r){
                reject(r);
            }
        );        
    }
    void fulfill(value_t v){
        std::lock_guard<std::mutex> lk(mtx);
        if(state!=pending) return;
        state=fulfilled;
        value=v;
        //If/when promise is fulfilled, all respective onFulfilled callbacks must execute in the order of their originating calls to then.
        for(auto& t : thens ){
            // If onFulfilled is not a function and promise1 is fulfilled, promise2 must be fulfilled with the same value as promise1.
            if(t.on_fullfilled_==nullptr)
            {
                t.promise_->fulfill(v);
            }
            value_t x;
            // If onFulfilled throws an exception e, promise2 must be rejected with e as the reason.
            try{
                x = t.on_fullfilled_(v);
            }catch(const reason_t& reason){
                t.promise_->reject(reason);
            }catch(const std::exception& e){
                reason_t reason(e.what());
                t.promise_->reject(reason);
            }catch(...){
                t.promise_->reject(reason_t("unknown reason"));
            }
            // If onFulfilled returns a value x, run the Promise Resolution Procedure [[Resolve]](promise2, x).
            resolve(t.promise_, x);
        }
    }

    void reject(reason_t r){
        std::lock_guard<std::mutex> lk(mtx);
        if(state!=pending) return;
        state=rejected;
        reason=r;
        // If/when promise is rejected, all respective onRejected callbacks must execute in the order of their originating calls to then.
        for(auto& t : thens ){
            // If onRejected is not a function and promise1 is rejected, promise2 must be rejected with the same reason as promise1.
            if(t.on_rejected_==nullptr)
            {
                t.promise_->reject(reason);
            }
            value_t x;
            // If onRejected throws an exception e, promise2 must be rejected with e as the reason.
            try{
                x = t.on_rejected_(reason);
            }catch(const reason_t& reason){
                t.promise_->reject(reason);
            }catch(const std::exception& e){
                reason_t reason(e.what());
                t.promise_->reject(reason);
            }catch(...){
                t.promise_->reject(reason_t("unknown reason"));
            }
            // If onRejected returns a value x, run the Promise Resolution Procedure [[Resolve]](promise2, x).
            resolve(t.promise_, x);
        }

    }
    // will block if fulfill/reject is running
    std::shared_ptr<promise> then(on_fullfilled_func f, on_rejected_func r)
    {
        std::lock_guard<std::mutex> lk(mtx);
        auto promise_=create_promise();
        thens.emplace_back(f,r,promise_);
        return promise_;
    }

private:
    struct then_t{
        then_t(on_fullfilled_func on_fullfilled, on_rejected_func on_rejected, std::shared_ptr<promise> p) : on_fullfilled_(on_fullfilled), on_rejected_(on_rejected), promise_(p){}
        on_fullfilled_func on_fullfilled_;
        on_rejected_func on_rejected_;
        std::shared_ptr<promise> promise_;
    };
    enum state_t : int{
        pending = 0,
        fulfilled = 1,
        rejected =2
    };
    state_t state = pending;
    std::mutex mtx; 
    // please note that elements in thens are not those in a then chain
    // they were multiple independent then() calls 
    std::list<then_t> thens;  
    value_t value;    // default empty, holds nullptr
    reason_t reason;   // default empty, ""
};

}

