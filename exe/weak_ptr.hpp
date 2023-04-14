#pragma once

#include <iostream>
#include <memory>

#include "shared_ptr.hpp"

template <typename T>
class WeakPtr {

private:
    template <typename U>
    friend class WeakPtr;

    template <typename U>
    friend class SharedPtr;

    typename SharedPtr<T>::BaseControlBlock* cb = nullptr;

    void plus_counter() {
        if (cb) {
            cb->weak_count++;
        }
    }

public:
    template <typename U>
    WeakPtr(const SharedPtr<U>& shar);

    WeakPtr() = default;

    template <typename U>
    WeakPtr(const WeakPtr<U>& weak);

    void swap(WeakPtr<T>& weak);

    template <typename U>
    WeakPtr& operator=(const WeakPtr<U>& weak);

    template <typename U>
    WeakPtr(WeakPtr<U>&& weak);

    template <typename U>
    WeakPtr& operator=(WeakPtr<U>&& weak);

    WeakPtr(const WeakPtr& weak);

    WeakPtr& operator=(const WeakPtr& weak);

    WeakPtr(WeakPtr&& weak);

    WeakPtr& operator=(WeakPtr&& weak);

    void reset();

    size_t use_count() const;

    bool expired() const;

    SharedPtr<T> lock() const;

    ~WeakPtr();
};

