#pragma once

#include "promise.h"
#include <chrono>
#include <thread>
#include <iostream>
#include <random>
#include "promise.h"

using namespace eventual;
using namespace std;

uniform_int_distribution<unsigned> u(0,9);
default_random_engine e;

static void promise1()
{
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
    cin.get();
}

static void promise2()
{

}