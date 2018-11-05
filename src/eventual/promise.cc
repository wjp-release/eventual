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
};

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
    // unlike promise_t, ctors of promise_meta_t have no side-effect but member initialization
    promise_meta_t(){} // initially pending promise
    // create a fulfilled promise 
    promise_meta_t(value_t v): state(fulfilled), value(v){}
    // create a rejected promise
    promise_meta_t(reason_t r): state(rejected), reason(r){}

    // If x is pending, promise must remain pending until x is fulfilled or rejected.
    // If/when x is fulfilled, fulfill promise with the same value.
    // If/when x is rejected, reject promise with the same reason.
    void adopt(std::shared_ptr<promise_meta_t> x)
    {
        
    }
    
    // this function should not be called directly;
    // 
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



};


}

