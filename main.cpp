#include <iostream>
#include <memory>
#include <mutex>
#include <stack>

struct empty_stack:std::exception {
    const char* what () const noexcept {}
};

template <typename T>
class stack_threadsafe {
    private:
    std::stack<T> data;
    mutable std::mutex m;
    public:
    stack_threadsafe() {}
    stack_threadsafe(const stack_threadsafe &other) {
        std::lock_guard(other.m);
        data=other.data;
    }
    stack_threadsafe &operator=(const stack_threadsafe &other)=delete;

    void push(const T &value) {
        std::lock_guard<std::mutex> lock(m);
        data.push(std::move(value));
    }

    std::shared_ptr<T> pop() {
        std::lock_guard<std::mutex> lock(m);
        if (data.empty()) {
            throw empty_stack();
        }
        std::shared_ptr<T> result = std::make_shared<T>(std::move(data.top()));
        data.pop();
        return result;
    }

    void pop(T& value) {
        std::lock_guard<std::mutex> lock(m);
        if (data.empty()) {
            throw empty_stack();
        }
        value = std::move(data.top());
        data.pop();
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(m);
        return data.empty();
    }
};