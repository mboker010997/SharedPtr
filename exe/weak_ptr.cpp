#include <exe/weak_ptr.hpp>


template <typename T>
template <typename U>
WeakPtr<T>::WeakPtr(const SharedPtr<U>& shar) : cb(reinterpret_cast<typename SharedPtr<T>::BaseControlBlock*>(shar.cb)) {
    plus_counter();
}

template <typename T>
template <typename U>
WeakPtr<T>::WeakPtr(const WeakPtr<U>& weak) : cb(reinterpret_cast<typename SharedPtr<T>::BaseControlBlock*>(weak.cb)) {
    plus_counter();
}

template <typename T>
void WeakPtr<T>::swap(WeakPtr<T>& weak) {
    std::swap(cb, weak.cb);
}

template <typename T>
template <typename U>
WeakPtr<T>& WeakPtr<T>::operator=(const WeakPtr<U>& weak) {
    WeakPtr copy = weak;
    copy.swap(*this);
    return *this;
}

template <typename T>
template <typename U>
WeakPtr<T>::WeakPtr(WeakPtr<U>&& weak) : cb(weak.cb) {
    weak.cb = nullptr;
}

template <typename T>
template <typename U>
WeakPtr<T>& WeakPtr<T>::operator=(WeakPtr<U>&& weak) {
    WeakPtr copy = std::move(weak);
    copy.swap(*this);
    return *this;
}

template <typename T>
WeakPtr<T>::WeakPtr(const WeakPtr& weak) : cb(reinterpret_cast<typename SharedPtr<T>::BaseControlBlock*>(weak.cb)) {
    plus_counter();
}

template <typename T>
WeakPtr<T>& WeakPtr<T>::operator=(const WeakPtr& weak) {
    WeakPtr copy = weak;
    copy.swap(*this);
    return *this;
}

template <typename T>
WeakPtr<T>::WeakPtr(WeakPtr&& weak) : cb(weak.cb) {
    weak.cb = nullptr;
}

template <typename T>
WeakPtr<T>& WeakPtr<T>::operator=(WeakPtr&& weak) {
    WeakPtr copy = std::move(weak);
    copy.swap(*this);
    return *this;
}

template <typename T>
void WeakPtr<T>::reset() {
    *this = WeakPtr<T>();
}

template <typename T>
size_t WeakPtr<T>::use_count() const {
    return (cb ? cb->shared_count : 0);
}

template <typename T>
bool WeakPtr<T>::expired() const {
    return use_count() == 0;
}

template <typename T>
SharedPtr<T> WeakPtr<T>::lock() const {
    return expired() ? SharedPtr<T>() : SharedPtr<T>(*this);
}

template <typename T>
WeakPtr<T>::~WeakPtr() {
    if (!cb || !cb->weak_count) return;
    cb->weak_count--;
    if (cb->shared_count + cb->weak_count == 0) cb->destroy_block();
}
