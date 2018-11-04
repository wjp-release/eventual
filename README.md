# eventual

Eventual is an asyncrhonous programming framework for modern C++.

### Promises/A+ in C++

1. eventual::promise is an implementation that confirms Promises/A+, 
    
    1.1 with one little exception that eventual::promise does not try to assimilate nonconformant implementations of thenables. note that rule 2.3.3.1, 2.3.3.2, 2.3.3.3 does not apply to eventual::promise (see https://promisesaplus.com/)

    1.2 javascript's value is weakly typed, hence value in Promises/A+ is implemented as a zero_copy_value class that can hold any type in C++.

    1.3 users only have access to std::shared_ptrs of promises; they will be released automatically.

2. dependencies: C++11, zero_copy_value.hpp

3. how to use: static link or compile from source

4. drawbacks of promises: 
    
    4.1 promises are not cancellable nor sleepable

    4.2 value_t introduces implicit couplings that C++ programmer must be very careful with


### Breakable Promises/C++



### zero_copy_value

1. zero_copy_value is a data structure that holds any type. 
2. It also prevents implicit deep copy from happenning when it is assigned, passed or stored in value.
3. it is header-only and platform indenpendent

```cpp
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
```


### todo.hpp 

1. class todo is designed to aggregate asynchronous callbacks in a flattened chain 
2. class todo is NOT promise-like; it is mainly designed for c++ system-level async network programming 
3. it is header-only and platform indenpendent

```cpp
// ... define jobs first
todo{job0}(job1)(job2)(job3)(job4,job3_err)[job4_err]();
```

```cpp
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

```
