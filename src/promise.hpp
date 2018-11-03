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

namespace eventual{

// Promises/A+ reforged in modern C++
// see https://promisesaplus.com/

/*
Terminlogy:
“promise” is an object or function with a then method whose behavior conforms to this specification.
“thenable” is an object or function that defines a then method.
“value” is any legal JavaScript value (including undefined, a thenable, or a promise).
“exception” is a value that is thrown using the throw statement.
“reason” is a value that indicates why a promise was rejected.

Promise States:
A promise must be in one of three states: pending, fulfilled, or rejected.
When pending, a promise:
    may transition to either the fulfilled or rejected state.
When fulfilled, a promise:
    must not transition to any other state.
    must have a value, which must not change.
When rejected, a promise:
    must not transition to any other state.
    must have a reason, which must not change.

The then Method:
A promise must provide a then method to access its current or eventual value or reason.
A promise’s then method accepts two arguments: 
  promise.then(onFulfilled, onRejected)
Both onFulfilled and onRejected are optional arguments:
    If onFulfilled is not a function, it must be ignored.
    If onRejected is not a function, it must be ignored.
If onFulfilled is a function:
    it must be called after promise is fulfilled, with promise’s value as its first argument.
    it must not be called before promise is fulfilled.
    it must not be called more than once.
If onRejected is a function,
    it must be called after promise is rejected, with promise’s reason as its first argument.
    it must not be called before promise is rejected.
    it must not be called more than once.
onFulfilled or onRejected must not be called until the execution context stack contains only platform code. [3.1].
onFulfilled and onRejected must be called as functions (i.e. with no this value). [3.2]
then may be called multiple times on the same promise.
    If/when promise is fulfilled, all respective onFulfilled callbacks must execute in the order of their originating calls to then.
    If/when promise is rejected, all respective onRejected callbacks must execute in the order of their originating calls to then.
then must return a promise [3.3]. promise2 = promise1.then(onFulfilled, onRejected);
    If either onFulfilled or onRejected returns a value x, run the Promise Resolution Procedure 
      [[Resolve]](promise2, x).
    If either onFulfilled or onRejected throws an exception e, promise2 must be rejected with e as the reason.
    If onFulfilled is not a function and promise1 is fulfilled, promise2 must be fulfilled with the same value as promise1.
    If onRejected is not a function and promise1 is rejected, promise2 must be rejected with the same reason as promise1.

The Promise Resolution Procedure
The promise resolution procedure is an abstract operation taking as input a promise and a value, which we denote as [[Resolve]](promise, x). If x is a thenable, it attempts to make promise adopt the state of x, under the assumption that x behaves at least somewhat like a promise. Otherwise, it fulfills promise with the value x.
This treatment of thenables allows promise implementations to interoperate, as long as they expose a Promises/A+-compliant then method. It also allows Promises/A+ implementations to “assimilate” nonconformant implementations with reasonable then methods.
To run [[Resolve]](promise, x), perform the following steps:
    If promise and x refer to the same object, reject promise with a TypeError as the reason.
    If x is a promise, adopt its state [3.4]:
        If x is pending, promise must remain pending until x is fulfilled or rejected.
        If/when x is fulfilled, fulfill promise with the same value.
        If/when x is rejected, reject promise with the same reason.
    If x is not a promise, fulfill promise with x.
*/




class promise{
public:
    using sys_func = std::function<void()>;
    using cb_func = std::function<void(sys_func, sys_func)>;
    promise(){}
    enum state_t : int{
        pending = 0,
        fulfilled = 1,
        rejected =2
    };

private:

    struct then_t{
        then_t(cb_func on_resolve, cb_func on_reject) : on_resolve(on_resolve), on_reject(on_reject){}
        cb_func on_resolve;
        cb_func on_reject;
    };

    // all these then_t-s are called after this promise fulfilled/rejected in the 'thens' list order
    std::list<then_t> thens; 

};

}

