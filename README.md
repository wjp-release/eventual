# eventual

### promise_t

1. The eventual::promise_t does conform to Promises/A+ standard. 

2. But its main purpose is to help users write async code on their own rather than merely use existing async code in the current thread like what a promise does in Javascript.

3. how to use: static link or compile from source

4. drawbacks of promises as a proxy object for async programming: 
    
    4.1 promises are not cancellable nor sleepable

    4.2 value_t introduces implicit couplings

```cpp
    promise_t p1{[](promise_t::fulfill_func fulfill, promise_t::reject_func reject){
      std::cout<<"try to get a connection\n";
      this_thread::sleep_for(chrono::seconds(1));
      int res=u(e);
      if(res<1){
        reject(reason_t("connection failed!"));
      }else{
        fulfill(value_t(res));
      }
    }};
    auto p2=p1.then(
      [](value_t v){
        std::cout<<"now we get a connection! query something!\n";
        this_thread::sleep_for(chrono::seconds(1));
        int res=u(e);
        if(res<1){
          throw reason_t("query failed!");
        }else{
          return value_t(res);
        }
      }, 
      [](reason_t r){
        std::cout<<"fail to connect! try it again!\n";
        int res=u(e);
        if(res<1){
          throw reason_t("connect failed!");
        }else{
          std::cout<<"now we get a connection! query something!\n";
          int query_res=u(e);
          if(query_res<1){
            throw reason_t("query failed!");
          }else{
            return value_t(query_res);
          }
        }
      }
    );
    std::cout<<"then defined, do something else\n";
    auto p3=p2.then(
      [](value_t v){
        std::cout<<"good, result="<<v.data<int>()<<std::endl;
        return v;
      }, 
      [](reason_t r){
        std::cout<<"should handle different reasons here but fk it\n";
        return value_t(-1);
      }
    );
```


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
