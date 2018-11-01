# eventual

1. the primary purpose of lib eventual is to aggregate asynchronous callbacks in a flattened chain 
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
