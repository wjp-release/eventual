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
#include <list>

namespace eventual{

struct promise_t::then_t{
    then_t( on_fullfilled_func          on_fullfilled, 
            on_rejected_func            on_rejected, 
            std::shared_ptr<promise_meta_t> p) : 
            on_fullfilled_(on_fullfilled), 
            on_rejected_(on_rejected), 
            promise_(p)
    {}
    on_fullfilled_func                  on_fullfilled_;
    on_rejected_func                    on_rejected_;
    std::shared_ptr<promise_meta_t>     promise_;
};

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

    void fulfill(value_t v){
        std::lock_guard<std::mutex> lk(mtx);
        if(state!=pending) return;
        state=fulfilled;
        value=v;
        for(auto& t : thens ){   
            promise_engine::instance().run([t,v]{
                trigger_on_fulfill(t.promise_, t.on_fullfilled_,v);
            });
        }
    }

    void reject(reason_t r){
        std::lock_guard<std::mutex> lk(mtx);
        if(state!=pending) return;
        state=rejected;
        reason=r;
        for(auto& t : thens ){   
            promise_engine::instance().run([t,r]{
                trigger_on_reject(t.promise_, t.on_rejected_, r);
            });
        }
    }

    std::shared_ptr<promise_meta_t> then(on_fullfilled_func f, on_rejected_func r)
    {
        std::unique_lock<std::mutex> lk(mtx);
        auto promise_=std::make_shared<promise_meta_t>();
        if(state==pending) 
            thens.emplace_back(f,r,promise_);
        else if(state==rejected) 
            promise_engine::instance().run([r,promise_,cur_r=reason]{
                trigger_on_reject(promise_, r, cur_r);
            });
        else
            promise_engine::instance().run([f,promise_,cur_v=value]{
                trigger_on_fulfill(promise_, f, cur_v);
            });
        return promise_;
    }

    void adopted_by(std::shared_ptr<promise_meta_t> p)
    {
        std::lock_guard<std::mutex> lk(mtx);
        if(state==fulfilled) p->fulfill(value); 
        else if(state==rejected) p->reject(reason);
        else{
            thens.emplace_back(nullptr, nullptr, p);
        }
    }
};


// promise resolution procedure: If x is a promise, it attempts to make promise adopt the state of x. 
void promise_t::resolve(std::shared_ptr<promise_meta_t> p, value_t value_x)
{
    if( value_x.has_same_type(typeid(promise_t)) )
    {
        std::shared_ptr<promise_meta_t> x= value_x.data<promise_t>().meta;
        if(x==p){
            p->reject(reason_t("It's illogical for a promise to adopt the state of itself!"));
            return;
        }
        x->adopted_by(p);//will block if x is now being fulfilled/rejected
    }else{
        p->fulfill(value_x);
    }
}

promise_t promise_t::then(on_fullfilled_func f, on_rejected_func r)
{
    return promise_t(meta->then(f,r));
}


// create a initial promise 
promise_t::promise_t(init_func init) : 
meta(std::make_shared<promise_meta_t>())
{
    if(init==nullptr) throw std::runtime_error("promise init_func should not be nullptr!");
    init( 
        [this](value_t v){ 
            meta->fulfill(v);
        },
        [this](reason_t r){ 
            meta->reject(r);
        }
    );        
}

void promise_t::trigger_on_reject(std::shared_ptr<promise_meta_t> promise_, on_rejected_func on_rejected_, reason_t r)
{
    if(on_rejected_==nullptr)
    {
        promise_->reject(r);
        return;
    }
    value_t x;
    try{
        x = on_rejected_(r);
    }catch(const reason_t& reason){
        promise_->reject(reason);
        return;
    }catch(const std::exception& e){
        reason_t reason(e.what());
        promise_->reject(reason);
        return;
    }catch(...){
        promise_->reject(reason_t("unknown reason"));
        return;
    }
    resolve(promise_,x);
}


void promise_t::trigger_on_fulfill(std::shared_ptr<promise_meta_t> promise_, on_fullfilled_func on_fullfilled_, value_t v)
{
    if(on_fullfilled_==nullptr)
    {
        promise_->fulfill(v);
        return;
    }
    value_t x;
    try{
        x = on_fullfilled_(v); // execute user code
    }catch(const reason_t& reason){
        promise_->reject(reason);
        return;
    }catch(const std::exception& e){
        reason_t reason(e.what());
        promise_->reject(reason);
        return;
    }catch(...){
        promise_->reject(reason_t("unknown reason"));
        return;
    }
    resolve(promise_,x);
}

promise_t promise_t::create_fulfilled_promise(value_t v)
{
    return std::make_shared<promise_meta_t>(v);
}

promise_t promise_t::create_rejected_promise(reason_t r)
{
    return std::make_shared<promise_meta_t>(r);
}

}

