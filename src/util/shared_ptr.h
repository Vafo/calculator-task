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

    // allocate copy of obj
    shared_ptr(const T& obj): impl(NULL) {
        // TODO: Reduce to 1 allocator call
        T *ptr = allocator.allocate(1);
        allocator.construct(ptr, obj);
        impl = new shared_ptr_impl(ptr);
    }

    // allocate copy of obj
    // Argument is derived object
    template<typename D>
    shared_ptr(const D& obj): impl(NULL) {
        static_assert(std::is_base_of<T, D>::value);
        std::allocator<D> d_allocator;
        D *ptr = d_allocator.allocate(1);
        d_allocator.construct(ptr, obj);
        impl = new shared_ptr_impl(ptr);
    }

    // allocate copy of obj
    // Argument is pointer to derived object
    template<typename D>
    shared_ptr(D* obj): impl(NULL) {
        static_assert(std::is_base_of<T, D>::value);
        std::allocator<D> d_allocator;
        D *ptr = d_allocator.allocate(1);
        d_allocator.construct(ptr, *obj);
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
            allocator.destroy(obj);
            allocator.deallocate(obj, 1);
        }

        T *obj;
        int ref_count;
        std::allocator<T> allocator;
    };

    shared_ptr_impl* impl;
    std::allocator<T> allocator;

    void dec_n_check() {
        if(impl != NULL) {
            --impl->ref_count;
            if(impl->ref_count == 0)
                delete impl;
        }
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