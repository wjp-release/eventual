//
// platform indenpendent (because it depends on c++ std lib only) 
// fixed-sized RAII threadpool
//
// Created by jipeng on 5/26/18.
//

#pragma once


#include <mutex>
#include <condition_variable>
#include <functional>
#include <queue>
#include <thread>

namespace eventual {

class threadpool {
public:
    using func=std::function<void()>;
    explicit threadpool(size_t nr_thread) : meta_(std::make_shared<meta>()) {
        for (size_t i = 0; i < nr_thread; i++) {
            std::thread([this]{
                std::unique_lock<std::mutex> lk(meta_->mtx_);
                for (;;) {
                    if (!meta_->tasks_.empty()) {
                        auto current = meta_->tasks_.front();
                        meta_->tasks_.pop();
                        lk.unlock();
                        current();
                        lk.lock();
                    } else if (meta_->is_shutdown_) {
                        break;
                    } else {
                        meta_->cond_.wait(lk);
                    }
                }
            }).detach();
        }
    }
    threadpool() = delete;
    threadpool(threadpool &&) = default;
    ~threadpool() {
        if ((bool) meta_) {
            {
                std::lock_guard<std::mutex> lk(meta_->mtx_);
                meta_->is_shutdown_ = true;
            }
            meta_->cond_.notify_all();
        }
    }
    void run(func task) {
        {
            std::lock_guard<std::mutex> lk(meta_->mtx_);
            meta_->tasks_.emplace(task);
        }
        meta_->cond_.notify_one();
    }
private:
    struct meta {
        std::mutex mtx_;
        std::condition_variable cond_;
        bool is_shutdown_ = false;
        std::queue<func> tasks_;
    };
    std::shared_ptr<meta> meta_;
};
}

