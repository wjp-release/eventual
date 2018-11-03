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

#include <iostream>

namespace eventual{

class zero_copy_value{
public:
    // empty is a valid state
    zero_copy_value() : meta(nullptr){}
    // none of these ctors & operators would actually make a copy
    zero_copy_value(const zero_copy_value& d) : meta(d.meta){}
    // rvalue to be swapped as they would be obsolete anyway
    zero_copy_value& operator= (zero_copy_value&&d) noexcept{
        std::cout<<"swap!\n";
        meta.swap(d.meta);
        return *this;
    }
    // increment ref count without making actual copy 
    zero_copy_value& operator= (const zero_copy_value& d) noexcept{
        std::cout<<"ref++\n";
        meta=d.meta;
        return *this;
    }
    // do we refer to the same meta object?
    bool operator== (const zero_copy_value& d) noexcept{
        return meta.get()==d.meta.get();
    }
    bool operator!= (const zero_copy_value& d) const noexcept{
        return !(*this==d);
    }
    bool empty() const noexcept{
        return meta==nullptr;
    }
    // swap values between two lvalues
    zero_copy_value& swap(zero_copy_value& d)
    {
        meta.swap(d.meta);
        return this;
    }
    // copy must be called in a very explicit way
    zero_copy_value& copy()  //return a reference to another copy of *this
    {
        return zero_copy_value(meta->copy());
    } 
    // construct from any type
    template<class T>
    zero_copy_value(const T& value) : meta(std::make_shared<meta_any_t<T>>(value))
    {} 
    // assign any type
    template<class T>
    zero_copy_value& operator=(const T& value)
    {
        // reuse move ctor and template ctor won't introduce overhead
        *this = std::move(zero_copy_value(value));
        // meta=std::make_shared<meta_any_t<T>>(value);
        return *this;
    }
    // return typeid type name
    std::string type() 
    {
        if(meta==nullptr) return std::string(typid(nullptr).name());
        return meta->type_name();
    }

    // template<class T>
    // T data()
    // {
    //     if(type()==typeid(T).name()){
    //         //no need for dynamic_cast, since we have checked type of T matches meta's 
    //         auto meta_of_T=static_cast<meta_any_t<T>>(meta); 
    //         return meta_of_T->data();
    //     }
    //     else{
    //         throw std::runtime_error("T is not the type holded by this zero_copy_value");
    //     }
    // }
private:
    class meta_t{
    public:
        virtual std::string type_name()=0;
        virtual std::shared_ptr<meta_t> copy()=0;
    };

    // this ctor is only used to create a copy
    zero_copy_value(std::shared_ptr<meta_t> meta):meta(meta){}

    template <class T>
    class meta_any_t : public meta_t{
    public:
        meta_any_t(const T& value): value(value) {}
        std::string type_name() override{
            return std::string(typeid(T).name()); 
        }
        std::shared_ptr<meta_t> copy() override{
            return std::make_shared<meta_any_t<T>>(value);
        }
        T data() noexcept const{
            return value;
        }
    private:
        T value;
    };
    std::shared_ptr<meta_t> meta;
};

}

