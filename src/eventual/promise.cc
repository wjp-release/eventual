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
    then_t(on_fullfilled_func on_fullfilled, on_rejected_func on_rejected, std::shared_ptr<promise_t> p) : on_fullfilled_(on_fullfilled), on_rejected_(on_rejected), promise_t_(p){}
    on_fullfilled_func on_fullfilled_;
    on_rejected_func on_rejected_;
    std::shared_ptr<promise_meta_t> promise;
};

struct promise_t::promise_meta_t{
    // create a pending promise without init func (created by then())
    promise_meta_t(){}
    // create a fulfilled promise 
    promise_meta_t(value_t v): state(fulfilled), value(v){}
    // create a rejected promise
    promise_meta_t(reason_t r): state(rejected), reason(r){}
    state_t state = pending;
    std::mutex mtx; 
    // please note that elements in thens are not those in a then chain
    // they were multiple independent then() calls 
    std::list<then_t> thens;  
    value_t value;    // default empty, holds nullptr
    reason_t reason;   // default empty, ""

};


}

