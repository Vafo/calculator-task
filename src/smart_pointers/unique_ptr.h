#ifndef UTIL_UNIQUE_PTR_H
#define UTIL_UNIQUE_PTR_H

#include <memory>
#include <cassert>

#include "util.h"

namespace postfix::util {

template<typename T, typename Deleter = std::default_delete<T>>
class unique_ptr {
public:
    unique_ptr(): ptr(nullptr) { }

    unique_ptr(const unique_ptr& other) = delete;

    template<typename D, typename D_Deleter>
    unique_ptr(unique_ptr<D, D_Deleter>&& other): ptr(NULL) {
        static_assert(std::is_base_of<T, D>::value);
        reset(other.release());
    }

    template<typename D>
    explicit unique_ptr(D *in_ptr): ptr(in_ptr) {
        static_assert(std::is_base_of<T, D>::value);
    }

    unique_ptr& operator=(const unique_ptr& other) = delete;

    template<typename D, typename D_Deleter>
    unique_ptr& operator=(unique_ptr<D, D_Deleter>&& other) {
        static_assert(std::is_base_of<T, D>::value);
        
        reset(other.release());
        return *this;
    }

    ~unique_ptr() {
        if(ptr != NULL) {
            check_if_deletable(ptr);

            Deleter deleter;
            deleter(ptr);
        }
    }

    void swap(unique_ptr& other) {
        using std::swap;

        swap(ptr, other.ptr);
    }

    T& operator*() {
        assert(ptr != NULL);
        return *ptr;
    }

    const T& operator*() const {
        assert(ptr != NULL);
        return *ptr;
    }

    T* operator->() {
        assert(ptr != NULL);
        return ptr;
    }

    const T* operator->() const {
        assert(ptr != NULL);
        return ptr;
    }

    T* release() {
        T *tmp = ptr;
        ptr = NULL;
        return tmp;
    }

    void reset(T *new_ptr) {
        if(ptr != NULL) {
            Deleter deleter;
            deleter(ptr);
        }
        ptr = new_ptr;
    }

    T* get() {
        return ptr;
    }

    const T* get() const {
        return ptr;
    }

private:
    T *ptr;

};

template<
    typename T,
    typename Deleter = std::default_delete<T>,
    class... Args>
unique_ptr<T, Deleter> make_unique(Args&... args) {
    return unique_ptr<T, Deleter>( new T(args...) );
}

} // namespace postfix::util

#endif