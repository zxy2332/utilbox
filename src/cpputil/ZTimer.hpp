#include <thread>
#include <chrono>
#include <functional>
#include <condition_variable>
#include <atomic>

namespace zxy::util{
    class ZTimerPrivate {
    public:
        ZTimerPrivate() {
            CreateThread();
        }
        ZTimerPrivate(std::chrono::milliseconds sleep_time,
        std::function<void()> func):ZTimerPrivate() {
            SetTimeOutFunc(func);
            Start(sleep_time);
        }
        ~ZTimerPrivate() {
            DestroyThread();
        }

        void SetTimeOutFunc(std::function<void()> func) {
            cb_ = func;
        }

        void Start(std::chrono::milliseconds sleep_time) {
            sleep_time_ = sleep_time;
            if (sleep_time_.count() == 0) {
                //休眠时间最少1ms
                sleep_time_ = std::chrono::milliseconds(1);
            }
            NotifyAll();
            running_.store(true);
        }

        void Stop() {
            running_.store(false);
        }
    private:
        void CreateThread() {
            DestroyThread();
            stop_.store(false);
            t_ = std::thread([this]() {
                while (stop_.load() == false) {
                    std::unique_lock<std::mutex> lk_running(running_mx_);
                    cv_running_.wait(lk_running, [this]() {
                        return running_.load() || stop_.load();
                    });
                    std::unique_lock<std::mutex> lk(mx_);
                    //等待一定时间自动退出
                    cv_.wait_for(lk,sleep_time_, [this]() {
                        //stop_为true时必须退出，停止运行时不需要退出，继续运行时重启
                        return stop_.load();
                    });
                    if (stop_.load()) {
                        break;
                    }
                    if (running_.load()) {
                        //调用回调
                        if (cb_) {
                            cb_();
                        }
                    }
                }
            });
        }
        void DestroyThread() {
            stop_.store(true);
            NotifyAll();
            if (t_.joinable()) {
                t_.join();
            }
        }

        void NotifyAll() {
            cv_running_.notify_all();
            cv_.notify_all();
        }
    private:
        std::function<void()> cb_;
        std::chrono::milliseconds sleep_time_ = std::chrono::milliseconds(1000);
        std::thread t_;
        std::atomic<bool> running_;
        std::condition_variable cv_;
        std::condition_variable cv_running_;
        std::mutex mx_;
        std::mutex running_mx_;
        std::atomic<bool> stop_;
    };
}
