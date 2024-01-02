#pragma once

#include <cstddef>  // for std::nullptr_t
#include <utility>  // for std::exchange / std::swap
template <typename T>
concept operation = requires(T* pointer) {
    pointer->DecRef(pointer);
};
class SimpleCounter {
public:
    size_t IncRef() {
        ++count_;
        return count_;
    };
    size_t DecRef() {
        --count_;
        return count_;
    };
    size_t RefCount() const {
        return count_;
    };

private:
    size_t count_ = 0;
};

struct DefaultDelete {
    template <typename T>
    static void Destroy(T* object) {
        delete object;
    }
};

template <typename Derived, typename Counter, typename Deleter>
class RefCounted {
public:
    // Increase reference counter.
    void IncRef() {
        counter_.IncRef();
    };
    RefCounted operator=(RefCounted a) {
        ;
        return *this;
    }

    // Decrease reference counter.
    // Destroy object using Deleter when the last instance dies.
    template <typename T>
    void DecRef(T* pt) {
        counter_.DecRef();
        if (RefCount() == 0) {
            Deleter a;
            a.Destroy(pt);
        }
    };

    // Get current counter value (the number of strong references).
    size_t RefCount() const {
        return counter_.RefCount();
    };

private:
    Counter counter_;
};

template <typename Derived, typename D = DefaultDelete>
using SimpleRefCounted = RefCounted<Derived, SimpleCounter, D>;

template <typename T>
class IntrusivePtr {
    template <typename Y>
    friend class IntrusivePtr;
    friend SimpleRefCounted<T>;

public:
    // Constructors
    IntrusivePtr() {
        ptr_ = nullptr;
    };
    IntrusivePtr(std::nullptr_t) {
        ptr_ = nullptr;
    };
    IntrusivePtr(T* ptr) {
        ptr_ = ptr;
        ptr_->IncRef();
    };

    template <typename Y>
    IntrusivePtr(const IntrusivePtr<Y>& other) {
        ptr_ = other.ptr_;
        if (ptr_ != nullptr) {
            ptr_->IncRef();
        }
    };

    template <typename Y>
    IntrusivePtr(IntrusivePtr<Y>&& other) {
        ptr_ = other.ptr_;
        other.ptr_ = nullptr;
    };

    IntrusivePtr(const IntrusivePtr& other) {
        ptr_ = other.ptr_;
        if (ptr_ != nullptr) {
            ptr_->IncRef();
        }
    };
    IntrusivePtr(IntrusivePtr&& other) {
        ptr_ = other.ptr_;
        other.ptr_ = nullptr;
    };

    // `operator=`-s
    IntrusivePtr& operator=(const IntrusivePtr& other) {
        if (ptr_ == other.ptr_) {
            return *this;
        }
        IntrusiveDeleter();
        ptr_ = other.ptr_;
        if (ptr_ != nullptr) {
            ptr_->IncRef();
        }
        return *this;
    };
    IntrusivePtr& operator=(IntrusivePtr&& other) {
        if (ptr_ == other.ptr_) {
            return *this;
        }
        IntrusiveDeleter();
        ptr_ = other.ptr_;
        other.ptr_ = nullptr;
        return *this;
    };

    // Destructor
    ~IntrusivePtr() {
        IntrusiveDeleter();
    };
    void IntrusiveDeleter() {
        if (ptr_ != nullptr) {
            if constexpr (operation<T>) {
                ptr_->DecRef(ptr_);
            }
            if constexpr (!operation<T>) {
                ptr_->DecRef();
            }
        }
    }

    // Modifiers
    void Reset() {
        IntrusiveDeleter();
        ptr_ = nullptr;
    };
    void Reset(T* ptr) {
        if (ptr_ == ptr) {
            return;
        }
        IntrusiveDeleter();
        ptr_ = ptr;
        ptr_->IncRef();
    };
    void Swap(IntrusivePtr& other) {
        std::swap(ptr_, other.ptr_);
    };

    // Observers
    T* Get() const {
        return ptr_;
    };
    T& operator*() const {
        return (*ptr_);
    };
    T* operator->() const {
        return ptr_;
    };
    size_t UseCount() const {
        if (ptr_ == nullptr) {
            return 0;
        }
        return ptr_->RefCount();
    };
    explicit operator bool() const {
        return (ptr_ != nullptr and ptr_->RefCount() != 0);
    };

private:
    T* ptr_;
};

template <typename T, typename... Args>
IntrusivePtr<T> MakeIntrusive(Args&&... args) {
    return IntrusivePtr<T>(new T(std::forward<Args>(args)...));
};
