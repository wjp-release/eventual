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

// todo is not a part of promise implementation
// it's just a funny way to organize code  

#include <functional>
#include <list>
#include <memory>
#include <mutex>              
#include <condition_variable> 
#include <atomic>

namespace eventual
{

class todo
{
public:
    using func = std::function<void(todo*)>;
    todo(func init=nullptr) : meta(std::make_shared<meta_t>(init)){}
    enum state_t : int{
        pending = 0,
        resolved = 1,
        rejected = 2,
        finished = 3
    };
    todo(const todo& d) noexcept : meta(d.meta){}

    todo& operator= (todo&& d) noexcept{
        meta.swap(d.meta);
        return *this;
    }

    todo& operator= (const todo& d) noexcept{
        meta=d.meta;
        return *this;
    }

    bool operator== (const todo& d) const noexcept{
        return meta.get()==d.meta.get();
    }

    bool operator!= (const todo& d) const noexcept{
        return !(*this==d);
    }

    todo& operator[](func on_reject){
        return then(nullptr, on_reject);
    }

    todo& operator()(func on_resolve, func on_reject=nullptr){
        return then(on_resolve, on_reject);
    }

    todo& operator()(){
        run();
        return *this;
    }

    todo& then(func on_resolve, func on_reject=nullptr){
        meta->steps.emplace_back(on_resolve, on_reject);
        return *this;
    }
    
    void run(){
        std::unique_lock<std::mutex> lck(meta->mtx);

        if(meta->init!=nullptr){
            set_state(pending);
            meta->init(this);
            while(get_state()==pending) meta->cv.wait(lck);
        }
        for(auto s: meta->steps){
            std::function<void()> run_next[4]={
                [&](){
                    // normally code won't run into this function; you should never wake a todo in a pending state
                    throw std::runtime_error("provider woke this todo object without setting its state properly!");
                },
                [&](){
                    set_state(pending);
                    if(s.on_resolve!=nullptr){
                        s.on_resolve(this);
                        while(get_state()==pending) meta->cv.wait(lck); // waiting till completion
                    }
                    else{ 
                        set_state(finished);
                    }
                },
                [&](){
                    if(s.on_reject!=nullptr){
                        set_state(pending);
                        s.on_reject(this);
                        while(get_state()==pending) meta->cv.wait(lck); 
                    }
                    set_state(finished); // s quit; no matter if on_reject exists
                },
                [&](){
                    // nothing to do here, let's finish the run
                }
            };
            run_next[get_state()]();
            if(get_state()==finished){
                break;
            }
        }
        set_state(finished);
    }
    void resolve() noexcept{
        meta->state=resolved;
    }
    void finish() noexcept{
        meta->state=finished;
    }
    void reject() noexcept{
        meta->state=rejected;
    }
    int get_state() noexcept{
        return meta->state.load();
    }
    void set_state(state_t i) noexcept{
        meta->state=i;
    }
    // this functon is called by provider;
    // provider should set state to rejected/resolved before waking the todo object
    void wake() noexcept{
        meta->cv.notify_all();
    }
private:
    struct step_t{
        step_t(func on_resolve, func on_reject) : on_resolve(on_resolve), on_reject(on_reject){}
        func on_resolve;
        func on_reject;
    };
    struct meta_t{
        meta_t(func init) : state(pending), init(init){}
        std::atomic<int> state;
        std::mutex mtx;
        std::condition_variable cv;
        func init;
        std::list<step_t> steps;
    };
    std::shared_ptr<meta_t> meta;
};



}