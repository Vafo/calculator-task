#ifndef SHARED_PTR_H
#define SHARED_PTR_H

#include <cassert>

namespace postfix::util {

template<typename T>
class shared_ptr {
public:
    shared_ptr(T *ptr = NULL): impl(NULL) {
        if(ptr != NULL)
            impl = new shared_ptr_impl(ptr);
    }

    shared_ptr(const T& obj): impl(NULL) {
        // TODO: Reduce to 1 allocator call
        T *ptr = new T(obj);
        impl = new shared_ptr_impl(ptr);
    }

    template<typename D>
    shared_ptr(const D* obj): impl(NULL) {
        static_assert(std::is_base_of<T, D>::value);
        T *ptr = new D(*obj);
        impl = new shared_ptr_impl(ptr);
    }

    shared_ptr(const shared_ptr& other): impl(other.impl) {
        if(impl != NULL) {
            ++impl->ref_count;
        }
    }

    T&
    operator*() {
        assert(impl != NULL);
        return *impl->obj;
    }

    const T&
    operator*() const {
        assert(impl != NULL);
        return *impl->obj;
    }

    T*
    operator->() {
        assert(impl != NULL);
        return impl->obj;
    }

    const T*
    operator->() const {
        assert(impl != NULL);
        return impl->obj;
    }

    shared_ptr&
    operator= (const shared_ptr& other) {
        if(other.impl != NULL)
            ++other.impl->ref_count;

        if(impl != NULL)
            dec_n_check();

        impl = other.impl;
        return *this;
    }

    ~shared_ptr() {
        dec_n_check();
    }

private:
    class shared_ptr_impl {
    public:
        shared_ptr_impl(T *ptr): obj(ptr), ref_count(1) {}

        ~shared_ptr_impl() {
            delete obj;
        }

        T *obj;
        int ref_count;
    };

    shared_ptr_impl* impl;

    void dec_n_check() {
        --impl->ref_count;
        if(impl->ref_count == 0)
            delete impl;
    }
};

template<typename T>
bool operator== (const shared_ptr<T> &a, const shared_ptr<T> &b) {
    return a.impl->obj == b.impl->obj;
}

template<typename T>
bool operator!= (const shared_ptr<T> &a, const shared_ptr<T> &b) {
    return !(a == b);
}

} // namespace util

#endif