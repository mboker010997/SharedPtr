#pragma once

#include <iostream>
#include <memory>

#include <exe/wpd.hpp>

template <typename T>
class SharedPtr {

    template <typename U, typename Alloc, typename... Args>
    friend SharedPtr<U> allocateShared(const Alloc&, Args&&...);

    template <typename U, typename... Args>
    friend SharedPtr<U> makeShared(Args&&...);

    template <typename U>
    friend class SharedPtr;

    template <typename U>
    friend class WeakPtr;

private:
    struct BaseControlBlock {
        size_t shared_count = 0;
        size_t weak_count = 0;

        virtual T* get_ptr() = 0;

        virtual void delete_ptr() = 0;

        virtual void destroy_block() = 0;

        virtual ~BaseControlBlock() = default;
    };

    template <typename U, typename Deleter = std::default_delete<U>, typename Alloc = std::allocator<U>>
    struct ControlBlockSimple : BaseControlBlock {
        using AllocTraits = std::allocator_traits<Alloc>;
        using BlockAllocType = typename AllocTraits::template rebind_alloc<ControlBlockSimple<U, Deleter, Alloc>>;
        using BlockAllocTraits = std::allocator_traits<BlockAllocType>;

        T* ptr;
        Deleter deleter;
        BlockAllocType block_alloc;

        ControlBlockSimple(T* ptr, Deleter deleter, BlockAllocType block_alloc)
                : ptr(ptr), deleter(deleter), block_alloc(block_alloc) {}

        T* get_ptr() override {
            return ptr;
        }

        void delete_ptr() override {
            deleter(ptr);
        }

        void destroy_block() override {
            BlockAllocType temp = block_alloc;
            deleter.~Deleter();
            block_alloc.~BlockAllocType();
            BlockAllocTraits::deallocate(temp, this, 1);
        }

        ~ControlBlockSimple() = default;
    };

    template <typename U, typename Alloc = std::allocator<U>>
    struct ControlBlockMakeShared : BaseControlBlock {
        using AllocTraits = std::allocator_traits<Alloc>;
        using BlockAllocType = typename AllocTraits::template rebind_alloc<ControlBlockMakeShared<U, Alloc>>;
        using BlockAllocTraits = std::allocator_traits<BlockAllocType>;

        T object;
        Alloc alloc;
        BlockAllocType block_alloc;

        template <typename... Args>
        ControlBlockMakeShared(Alloc alloc, BlockAllocType block_alloc, Args&&... args)
                : object(std::forward<Args>(args)...), alloc(alloc), block_alloc(block_alloc) {}

        T* get_ptr() override {
            return &object;
        }

        void delete_ptr() override {
            AllocTraits::destroy(alloc, &object);
        }

        void destroy_block() override {
            BlockAllocType temp = block_alloc;
            alloc.~Alloc();
            block_alloc.~BlockAllocType();
            BlockAllocTraits::deallocate(temp, this, 1);
        }

        ~ControlBlockMakeShared() = default;
    };

    BaseControlBlock* cb = nullptr;

    struct MakeSharedConstructorTag {};

    void plus_counter() {
        if (cb) {
            cb->shared_count++;
        }
    }

    SharedPtr(MakeSharedConstructorTag, BaseControlBlock* cb) : cb(cb) {
        plus_counter();
    }

public:
    template <typename U, typename Deleter = std::default_delete<U>, typename Alloc = std::allocator<U>>
    SharedPtr(U* ptr, Deleter deleter = Deleter(), const Alloc& alloc = Alloc());

    SharedPtr(const SharedPtr& shar);

    SharedPtr& operator=(const SharedPtr& shar);

    SharedPtr(SharedPtr&& shar);

    SharedPtr& operator=(SharedPtr&& shar);

    template <typename U>
    SharedPtr(const SharedPtr<U>& shar);

    void swap(SharedPtr& shar);

    template <typename U>
    SharedPtr& operator=(const SharedPtr<U>& shar);

    template <typename U>
    SharedPtr(SharedPtr<U>&& shar);

    template <typename U>
    SharedPtr& operator=(SharedPtr<U>&& shar);

    SharedPtr() = default;

    template <typename U>
    SharedPtr(const WeakPtr<U>& weak);

    ~SharedPtr();

    void reset();

    template <typename U>
    void reset(U* pointer);

    size_t use_count() const;

    T* get() const;

    T* operator->() const;

    T& operator*() const;
};