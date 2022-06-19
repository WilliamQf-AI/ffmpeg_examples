#ifndef FFMPEG_EXAMPLES_CONSUMER_H
#define FFMPEG_EXAMPLES_CONSUMER_H

#include <thread>
#include <mutex>
#include <atomic>

template<class T>
class Consumer {
public:
    Consumer() = default;
    Consumer(const Consumer&) = delete;
    Consumer& operator=(const Consumer&) = delete;
    virtual ~Consumer() {
        std::lock_guard lock(mtx_);

        running_ = false;
        paused_ = false;
        eof_ = 0x00;
        ready_ = false;

        if (thread_.joinable()) {
            thread_.join();
        }
    }

    virtual int run() = 0;
    virtual int consume(T*, int) = 0;

    virtual void pause() { paused_ = true; }
    virtual void resume() { paused_ = false; }
    virtual void stop() { running_ = false; reset(); }
    virtual void reset() = 0;
    virtual int wait()
    {
        if (thread_.joinable()) {
            thread_.join();
        }
        return 0;
    }

    virtual bool full(int) const = 0;
    virtual int format(int) const = 0;
    virtual bool accepts(int) const = 0;
    virtual void enable(int, bool = true) = 0;

    virtual bool ready() const { return ready_; }
    bool running() const { return running_; }
    bool paused() const { return paused_; }
    virtual bool eof() const { return eof_; }

protected:
    std::atomic<bool> running_{ false };
    std::atomic<bool> paused_{ false };
    std::atomic<uint8_t> eof_{ 0x00 };
    std::atomic<bool> ready_{ false };
    std::thread thread_;
    std::mutex mtx_;
};

#endif // !FFMPEG_EXAMPLES_CONSUMER_H
