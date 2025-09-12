#include <chrono>
#include <functional>
#include <thread>
#include <atomic>
#include <vector>
#include <memory>
#include <mutex>

class Timer {
private:
    std::function<void()> cb;
    std::chrono::milliseconds ms;
    std::atomic<bool> run{false};
    std::atomic<bool> stop{false};
    std::thread t;
    bool rep;
    bool same;
    std::chrono::steady_clock::time_point last;
    
public:
    Timer(std::function<void()> callback, int intervalMs, bool repeat = true, bool sameThread = false) 
        : cb(callback), ms(abs(intervalMs)), rep(intervalMs > 0 ? repeat : false), same(sameThread) {
        // Negative interval = timeout (one-shot)
        if (same) last = std::chrono::steady_clock::now();
    }
    
    void start() {
        if (run.exchange(true)) return;
        
        if (same) return; // Same thread, just mark running
        
        stop = false;
        t = std::thread([this]() {
            do {
                std::this_thread::sleep_for(ms);
                if (!stop && cb) cb();
            } while (rep && !stop);
            run = false;
        });
    }
    
    void tick() {
        if (!same || !run) return;
        
        auto now = std::chrono::steady_clock::now();
        if (now - last >= ms) {
            if (cb) cb();
            last = now;
            if (!rep) run = false;
        }
    }
    
    void kill() {
        stop = true;
        if (t.joinable()) t.join();
        run = false;
    }
    
    bool alive() const { return run; }
    bool isSame() const { return same; }
    
    ~Timer() { kill(); }
};

class Timers { // TimerManager -> TM because we're lazy
private:
    static std::vector<std::unique_ptr<Timer>> timers;
    static std::mutex mtx;
    
public:
    // Regular timer (separate thread)
    template<typename F>
    static void set(F&& f, int ms, bool rep = true) {
        std::lock_guard<std::mutex> lock(mtx);
        auto timer = std::make_unique<Timer>(std::forward<F>(f), ms, rep, false);
        timer->start();
        timers.push_back(std::move(timer));
    }
    
    // Same-thread timer (for OpenGL)
    template<typename F>
    static void single(F&& f, int ms, bool rep = true) {
        std::lock_guard<std::mutex> lock(mtx);
        auto timer = std::make_unique<Timer>(std::forward<F>(f), ms, rep, true);
        timer->start();
        timers.push_back(std::move(timer));
    }
    
    // Timeout (negative ms = one-shot after delay)
    template<typename F>
    static void timeout(F&& f, int ms) {
        set(std::forward<F>(f), -abs(ms), false); // Force negative + no repeat
    }
    
    // OpenGL timeout
    template<typename F>
    static void singleTimeout(F&& f, int ms) {
        single(std::forward<F>(f), -abs(ms), false);
    }
    
    static void tick() {
        std::lock_guard<std::mutex> lock(mtx);
        for (auto& t : timers) {
            if (t->isSame()) t->tick();
        }
    }
    
    static void clean() {
        std::lock_guard<std::mutex> lock(mtx);
        timers.erase(std::remove_if(timers.begin(), timers.end(),
            [](const auto& t) { return !t->alive(); }), timers.end());
    }
    
    static void kill() {
        std::lock_guard<std::mutex> lock(mtx);
        timers.clear();
    }
    
    static size_t count() {
        std::lock_guard<std::mutex> lock(mtx);
        return timers.size();
    }
};

std::vector<std::unique_ptr<Timer>> Timers::timers;
std::mutex Timers::mtx;