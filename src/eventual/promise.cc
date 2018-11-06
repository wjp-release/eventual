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

#include "promise.h"

namespace eventual{

// create a initial promise (pending; expose fulfill/reject operations to user code; runs user cb immediately)
promise_t::promise_t(init_func init) : 
meta(std::make_shared<promise_meta_t>())
{
    if(init==nullptr) throw std::runtime_error("promise init_func should not be nullptr!");
    init( 
        [this](value_t v){ 
            promise_engine::instance().run_reject_asyncrhonously(meta, v);
        },
        [this](reason_t r){ 
            promise_engine::instance().run_reject_asyncrhonously(meta, r);
        }
    );        
}

struct promise_t::then_t{
    then_t( on_fullfilled_func          on_fullfilled, 
            on_rejected_func            on_rejected, 
            std::shared_ptr<promise_t>  p) : 
            on_fullfilled_(on_fullfilled), 
            on_rejected_(on_rejected), 
            promise_(p)
    {}
    on_fullfilled_func                  on_fullfilled_;
    on_rejected_func                    on_rejected_;
    std::shared_ptr<promise_meta_t>     promise_;
    void trigger_on_reject()
    {
        // If onRejected is not a function and promise1 is rejected, promise2 must be rejected with the same reason as promise1.
        if(on_rejected_==nullptr)
        {
            promise_->reject(r);
            continue;
        }
        value_t x;
        // If onRejected throws an exception e, promise2 must be rejected with e as the reason.
        try{
            x = on_rejected_(r);
        }catch(const reason_t& reason){
            promise_->reject(reason);
            continue;
        }catch(const std::exception& e){
            reason_t reason(e.what());
            promise_->reject(reason);
            continue;
        }catch(...){
            promise_->reject(reason_t("unknown reason"));
            continue;
        }
        // If onRejected returns a value x, run the Promise Resolution Procedure [[Resolve]](promise2, x).
        resolve(promise_,x);
    }
    void trigger_on_fulfill()
    {
        // If onFulfilled is not a function and promise1 is fulfilled, promise2 must be fulfilled with the same value as promise1.
        if(on_fullfilled_==nullptr)
        {
            promise_->fulfill(v);
            continue;
        }
        value_t x;
        // If onFulfilled throws an exception e, promise2 must be rejected with e as the reason.
        try{
            x = on_fullfilled_(v); // execute user code
        }catch(const reason_t& reason){
            promise_->reject(reason)
            continue;
        }catch(const std::exception& e){
            reason_t reason(e.what());
            promise_->reject(reason)
            continue;
        }catch(...){
            promise_->reject(reason_t("unknown reason"))
            continue;
        }
        // If onFulfilled returns a value x, run the Promise Resolution Procedure [[Resolve]](promise2, x).
        resolve(promise_,x);
    }
};

// promise resolution procedure
void promise_t::resolve(std::shared_ptr<promise_meta_t> p, value_t value_x)
{
    // If x is a promise, it attempts to make promise adopt the state of x. 
    if( x.has_same_type(typeid(promise_t)) )
    {
        std::shared_ptr<promise_meta_t> x= x.data<promise_t>().meta;
        // If x==promise, reject promise 
        if(x==p){
            p->reject(reason_t("It's illogical for a promise to adopt the state of itself!"));
            return;
        }
        // adopt the state of promise_x
        // If x is pending, promise must remain pending until x is fulfilled or rejected.
        // If/when x is fulfilled, fulfill promise with the same value.
        // If/when x is rejected, reject promise with the same reason.
        if(x->state==fulfilled) p->fulfill(x->value); 
        else if(x->state==rejected) p->reject(x->reason);
        else{
            x->thens.emplace_back(nullptr, nullptr, p);
        }
    }else{
        // Otherwise, it fulfills promise with value_t x.
        p->fulfill(value_x);
    }
}


// this object will survive untill its associated resolution procedure is completed even if its parent promise_t is destroyed by user. 
class promise_t::promise_meta_t{
private:
    state_t                             state = pending;
    std::mutex                          mtx; 
    // please note that these are BFS direct successors of this promise
    // don't confused them with DFS then-chained successors
    std::list<then_t>                   thens;  
    value_t                             value;    // default nullptr
    reason_t                            reason;   // default ""

public:
    // initially pending promise
    promise_meta_t(){} 
    // create a fulfilled promise 
    promise_meta_t(value_t v): state(fulfilled), value(v){}
    // create a rejected promise
    promise_meta_t(reason_t r): state(rejected), reason(r){}
    // library-provided nonblocking operation to fulfill a promise 
    void fulfill(value_t v){
        std::lock_guard<std::mutex> lk(mtx);
        if(state!=pending) return;
        state=fulfilled;
        value=v;
        // trigger all pending callbacks in BFS order asynchronously
        promise_engine::instance().run([this]{
            //If/when promise is fulfilled, all respective onFulfilled callbacks must execute in the order of their originating calls to then.
            for(auto& t : thens ){  // thens does not need mutex protection since this promise has become immutable 
                t.trigger_on_fulfill();
            }
        });
    }
    // library-provided nonblocking operation to reject a promise 
    void reject(reason_t r){
        std::lock_guard<std::mutex> lk(mtx);
        if(state!=pending) return;
        state=rejected;
        reason=r;
        promise_engine::instance().run([this]{
            // If/when promise is rejected, all respective onRejected callbacks must execute in the order of their originating calls to then.
            for(auto& t : thens ){ 
                t.trigger_on_reject();
            }
        });
    }
    // if this promise is being fulfilled/rejected, this function will block until the promise is resolved
    // if this promise has been resolved, it should stop emplacing new then_t-s, callbacks of then calls should be triggered immediately
    std::shared_ptr<promise_meta_t> then(on_fullfilled_func f, on_rejected_func r)
    {
        std::unique_lock<std::mutex> lk(mtx);
        auto promise_=std::make_shared<promise_meta_t>();
        if(state==pending) 
            thens.emplace_back(f,r,promise_);
        else if(state==rejected) 
            promise_engine::instance().run([f,r,promise_]{
                then_t(f,r,promise_).trigger_on_reject();
            });
        else
            promise_engine::instance().run([f,r,promise_]{
                then_t(f,r,promise_).trigger_on_fulfill();
            });
        return promise_;
    }

};


}

