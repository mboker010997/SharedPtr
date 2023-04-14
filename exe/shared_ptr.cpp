#include <exe/shared_ptr.hpp>

#include <memory>

template <typename T>
template <typename U, typename Deleter, typename Alloc>
SharedPtr<T>::SharedPtr(U* ptr, Deleter deleter, const Alloc& alloc) {
    using AllocTraits = std::allocator_traits<Alloc>;
    using BlockAllocType = typename AllocTraits::template rebind_alloc<ControlBlockSimple<U, Deleter, Alloc>>;
    using BlockAllocTraits = std::allocator_traits<BlockAllocType>;

    BlockAllocType block_alloc = alloc;
    ControlBlockSimple<U, Deleter, Alloc>* cur = BlockAllocTraits::allocate(block_alloc, 1);
    new (cur) ControlBlockSimple<U, Deleter, Alloc>(ptr, deleter, block_alloc);
    cb = cur;
    plus_counter();
}

template <typename T>
SharedPtr<T>::SharedPtr(const SharedPtr& shar) : cb(reinterpret_cast<BaseControlBlock*>(shar.cb)) {
    plus_counter();
}

template <typename T>
SharedPtr<T>& SharedPtr<T>::operator=(const SharedPtr<T>& shar) {
    SharedPtr<T> copy = shar;
    copy.swap(*this);
    return *this;
}

template <typename T>
SharedPtr<T>::SharedPtr(SharedPtr&& shar) : cb(reinterpret_cast<BaseControlBlock*>(shar.cb)) {
    shar.cb = nullptr;
}

template <typename T>
SharedPtr<T>& SharedPtr<T>::operator=(SharedPtr<T>&& shar) {
    SharedPtr copy = std::move(shar);
    copy.swap(*this);
    return *this;
}

template <typename T>
template <typename U>
SharedPtr<T>::SharedPtr(const SharedPtr<U>& shar)
    : cb(reinterpret_cast<BaseControlBlock*>(shar.cb)) {
    plus_counter();
}

template <typename T>
void SharedPtr<T>::swap(SharedPtr& shar) {
    std::swap(cb, shar.cb);
}

template <typename T>
template <typename U>
SharedPtr<T>& SharedPtr<T>::operator=(const SharedPtr<U>& shar) {
    SharedPtr<T> copy = shar;
    copy.swap(*this);
    return *this;
}

template <typename T>
template <typename U>
SharedPtr<T>::SharedPtr(SharedPtr<U>&& shar) : cb(reinterpret_cast<BaseControlBlock*>(shar.cb)) {
    shar.cb = nullptr;
}

template <typename T>
template <typename U>
SharedPtr<T>& SharedPtr<T>::operator=(SharedPtr<U>&& shar) {
    SharedPtr copy = std::move(shar);
    copy.swap(*this);
    return *this;
}

template <typename T>
template <typename U>
SharedPtr<T>::SharedPtr(const WeakPtr<U>& weak) : cb(reinterpret_cast<BaseControlBlock*>(weak.cb)) {
    plus_counter();
}

template <typename T>
SharedPtr<T>::~SharedPtr() {
    if (!cb || !cb->shared_count) return;
    cb->shared_count--;
    if (!cb->shared_count) {
        cb->delete_ptr();
    }
    if (!cb->shared_count && !cb->weak_count) {
        cb->destroy_block();
    }
}

template <typename T>
void SharedPtr<T>::reset() {
    *this = SharedPtr();
}

template <typename T>
template <typename U>
void SharedPtr<T>::reset(U* pointer) {
    *this = SharedPtr(pointer);
}

template <typename T>
size_t SharedPtr<T>::use_count() const {
    return (cb ? cb->shared_count : 0);
}

template <typename T>
T* SharedPtr<T>::get() const {
    return cb ? cb->get_ptr() : nullptr;
}

template <typename T>
T* SharedPtr<T>::operator->() const {
    return get();
}

template <typename T>
T& SharedPtr<T>::operator*() const {
    return *get();
}