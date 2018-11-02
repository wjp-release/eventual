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
    zero_copy_value& operator= (zero_copy_value&&d) noexcept : meta(d.meta){
        std::cout<<"swap!\n";
        meta.swap(d.meta);
        return *this;
    }
    // reference counter ++
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
    //
    template<class T>
    zero_copy_value& operator=(const T& value)
    {
        *this = std::move(zero_copy_value(value));
        return *this;
    }
private:
    // this ctor is only used to create a copy
    zero_copy_value(std::shared_ptr<meta_t> meta):meta(meta){}
    class meta_t{
    public:
        virtual std::string type_name()=0;
        virtual std::shared_ptr<meta_t> copy()=0;
    };
    template <class T>
    class meta_any_t : public meta_t{
    public:
        meta_any_t(const T& value): value(value) {}
        override std::string type_name(){
            return ""; //typeid(T)
        }
        override std::shared_ptr<meta_t> copy(){
            return std::make_shared<meta_any_t>(value);
        }
    private:
        T value;
    };
    std::shared_ptr<meta_t> meta;
};

}

