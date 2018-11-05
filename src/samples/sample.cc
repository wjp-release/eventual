#include "todo.hpp"
#include "zero_copy_value.hpp"
#include <iostream>
#include <chrono>
#include <thread>
#include <random>

using namespace eventual;
using namespace std;

// dummy service routines that simulate background threads or threadpools that actually complete the async tasks

void dummy_service_routine(todo td)
{
    this_thread::sleep_for(chrono::milliseconds(200));
    td.resolve();
    td.wake();
}

uniform_int_distribution<unsigned> u(0,9);
default_random_engine e;

void dummy_service_routine_sometimes_reject(todo td)
{
    this_thread::sleep_for(chrono::milliseconds(400));
    if(u(e)<6){
        td.resolve(); 
    }else{
        td.reject(); // 40%
    }
    td.wake();
}
void dummy_error_handling(todo td)
{
    this_thread::sleep_for(chrono::milliseconds(100));
    td.finish();
    td.wake();
}
void ftp_conn_service(todo td)
{
    dummy_service_routine(td);
}
void downloading_service(todo td)
{
    dummy_service_routine(td);
}
void uncompressing_service(todo td)
{
    dummy_service_routine(td);
}
void decoding_service(todo td)
{
    dummy_service_routine_sometimes_reject(td);
}
void fwrite_service(todo td)
{
    dummy_service_routine_sometimes_reject(td);
}

// style 1:
//      todo{job0}(job1)(job2)(job3)(job4,job3_err)[job4_err]();
// 1. define jobs first
// 2. chain functions with parentheses! {..}(..)(..)(..)[..]
// 3. use an empty () to kick off the whole callback chain
// {} captures init function for todo constructor
// () captures both on_resolve & on_reject
// [] captures on_reject only
void test1()
{
    auto job0=[](todo* self){
        cout<<"job0 connect to ftp server \n";
        ftp_conn_service(*self);
    };
    auto job1=[](todo* self){
        cout<<"job1 download file todo.zip \n";
        downloading_service(*self);
    };
    auto job2=[](todo* self){
        cout<<"job2 unzip file todo.zip \n";
        uncompressing_service(*self);
    };
    auto job3=[](todo* self){
        cout<<"job3 decipher file todo.data \n";
        decoding_service(*self);
    };
    auto job4=[](todo* self){
        cout<<"job4 write file todo.new \n";
        fwrite_service(*self);
    };
    auto job3_err=[](todo* self){
        cout<<"triggered by job3 decoding_service failure! \n";
        cout<<"quit now\n";
        dummy_error_handling(*self);
    };
    auto job4_err=[](todo* self){
        cout<<"triggered by job4 fwrite_service failure! \n";
        cout<<"quit now\n";
        dummy_error_handling(*self);
    };
    todo{job0}(job1)(job2)(job3)(job4,job3_err)[job4_err]();
}

// style 2:
// use methods rather than parenthese
void test2()
{
    todo td([](todo* self){ //func ===  std::function<void(todo*)> 
        cout<<"job0 connect to ftp server \n";
        ftp_conn_service(*self);
    });
    td.then([](todo* self){
        cout<<"job1 download file todo.zip \n";
        downloading_service(*self);
    }).then([](todo* self){  //then(func,func) === operator(func,func); then(nullptr, func) === operator[func]
        cout<<"job2 unzip file todo.zip \n";
        uncompressing_service(*self);
    }).then([](todo* self){
        cout<<"job3 decipher file todo.data \n";
        decoding_service(*self);
    }).then([](todo* self){
        cout<<"job4 write file todo.new \n";
        fwrite_service(*self);
        },  [](todo* self){
        cout<<"triggered by job3 decoding_service failure! \n";
        cout<<"quit now\n";
        dummy_error_handling(*self);
    }).then(nullptr, [](todo* self){
        cout<<"triggered by job4 fwrite_service failure! \n";
        cout<<"quit now\n";
        dummy_error_handling(*self);
    });
    td.run(); // run() === operator()
}

// style 3:
// all in one closure
void test3()
{
    todo
    {
        [](todo* self){ 
            cout<<"job0 connect to ftp server \n";
            ftp_conn_service(*self);
        }
    }
    (
        [](todo* self){
            cout<<"job1 download file todo.zip \n";
            downloading_service(*self);
        }
    )
    (
        [](todo* self){
            cout<<"job2 unzip file todo.zip \n";
            uncompressing_service(*self);
        }
    )
    (
        [](todo* self){
            cout<<"job3 decipher file todo.data \n";
            decoding_service(*self);
        }
    )
    (
        [](todo* self){
            cout<<"job4 write file todo.new \n";
            fwrite_service(*self);
        },
        [](todo* self){
            cout<<"triggered by job3 decoding_service failure! \n";
            cout<<"quit now\n";
            dummy_error_handling(*self);
        }
    )
    ( //c++ does not allow consequtive left square brackets here, so we have to use () with first arg set to nullptr
        nullptr, 
        [](todo* self){
            cout<<"triggered by job4 fwrite_service failure! \n";
            cout<<"quit now\n";
            dummy_error_handling(*self);
        }
    )();
}

void test_zero_copy_value()
{
    zero_copy_value a;
    cout<<"a empty: "<<a.empty()<<endl; //1
    cout<<"a typename: "<<a.type()<<endl; //Dn

    zero_copy_value b(123);
    cout<<"b typename: "<<b.type()<<endl; //i
    cout<<"b data as int: "<<b.data<int>()<<endl; //123

    zero_copy_value c(123.456);
    cout<<"c typename: "<<c.type()<<endl; //d
    cout<<"c data as double "<<c.data<double>()<<endl;
    
    zero_copy_value d = c; // ctor ref++ 
    zero_copy_value e = c.copy(); // make a copy
    cout<<"c==d: "<<(c==d) <<endl;
    cout<<"e==c: "<<(c==e) <<endl;
    cout<<"e typename: "<<e.type()<<endl; //d
    cout<<"e data as double "<<e.data<double>()<<endl;

    zero_copy_value f = 100; // construct from a specific type
    cout<<"f typename: "<<f.type()<<endl; //i
//    cout<<"f data as double "<<f.data<double>()<<endl; //throw exception (should be int)

    f=e; //operator= lvalue ref
    cout<<"f typename: "<<f.type()<<endl; //i
    cout<<"f data as double "<<e.data<double>()<<endl;
    f=e.copy(); // operator= rvalue!
    cout<<"f typename: "<<f.type()<<endl; //i
    cout<<"f data as double "<<e.data<double>()<<endl;

}

int main()
{
    test3();

    cin.get();
    return 0;
}