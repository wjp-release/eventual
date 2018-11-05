#include "promise.h"

namespace eventual{

threadpool threadpool_;

// promise resolution procedure. 
void resolve(std::shared_ptr<promise> promise_, value_t x)
{
    // If x is a promise, it attempts to make promise adopt the state of x. 
    if( x.has_same_type(typeid(std::shared_ptr<promise>)) )
    {
        std::shared_ptr<promise> promise_x=x.data<std::shared_ptr<promise>>();
        // If x==promise, reject promise 
        if(x_promise==promise_){
            promise_->reject(reason_t("It's illogical for a promise to adopt the state of itself!"));
            return;
        }
        // adopt the state of promise_x
        
    }else{
        // Otherwise, it fulfills promise with value_t x.
        promise_.fulfill(x);
    }

}


}