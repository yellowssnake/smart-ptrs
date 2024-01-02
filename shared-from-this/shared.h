#pragma once

#include "sw_fwd.h"  // Forward declaration

#include <cstddef>  // std::nullptr_t
#include <algorithm>
#include <utility>
#include <iostream>
class ESFT {
public:
    ~ESFT(){};
};
template <typename T>
class EnableSharedFromThis : public ESFT {
    friend SharedPtr<T>;
    friend WeakPtr<T>;

public:
    SharedPtr<T> SharedFromThis() {
        return SharedPtr(weak_this_);
    };
    SharedPtr<const T> SharedFromThis() const {
        return SharedPtr(weak_this_);
    };

    WeakPtr<T> WeakFromThis() noexcept {
        return WeakPtr<T>(weak_this_);
    };

    WeakPtr<const T> WeakFromThis() const noexcept {
        return WeakPtr<const T>(weak_this_);
    };
    ~EnableSharedFromThis(){};

    WeakPtr<T> weak_this_;
};
// https://en.cppreference.com/w/cpp/memory/shared_ptr
template <typename T>
class SharedPtr {
    friend WeakPtr<T>;
    friend EnableSharedFromThis<T>;

public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors
    SharedPtr() {
        owned_ = nullptr;
        cblock_ = nullptr;
    };
    SharedPtr(CB* ptr, T* point) {
        cblock_ = ptr;
        owned_ = point;
        if constexpr (std::is_base_of_v<ESFT, T>) {
            point->weak_this_.owned_ = point;
            point->weak_this_.cblock_ = ptr;
            if (cblock_->Curcw() == 0) {
                cblock_->IncCounterW();
            }
        }
    }
    SharedPtr(std::nullptr_t) {
        owned_ = nullptr;
        cblock_ = nullptr;
    };
    explicit SharedPtr(T* ptr) {
        owned_ = ptr;
        cblock_ = new CB1(ptr);
        if constexpr (std::is_base_of_v<ESFT, T>) {
            ptr->weak_this_.owned_ = owned_;
            ptr->weak_this_.cblock_ = cblock_;
            if (cblock_->Curcw() == 0) {
                cblock_->IncCounterW();
            }
        }
    };
    template <typename Y>
    SharedPtr(Y* ptr) {
        owned_ = ptr;
        cblock_ = new CB1(ptr);
        if constexpr (std::is_base_of_v<ESFT, Y>) {
            ptr->weak_this_.owned_ = owned_;
            ptr->weak_this_.cblock_ = cblock_;
            if (cblock_->Curcw() == 0) {
                cblock_->IncCounterW();
            }
        }
    };

    SharedPtr(const SharedPtr& other) {
        owned_ = other.Get();
        cblock_ = other.GetBlock();
        if (cblock_ != nullptr) {
            cblock_->IncCounter();
        }
    };
    template <typename U>
    SharedPtr(const SharedPtr<U>& other) {
        owned_ = other.Get();
        cblock_ = other.GetBlock();
        if (cblock_ != nullptr) {
            cblock_->IncCounter();
        }
    };
    SharedPtr(SharedPtr&& other) noexcept {
        owned_ = other.Get();
        cblock_ = other.GetBlock();
        other.Earthing();
    };
    template <typename U>
    SharedPtr(SharedPtr<U>&& other) noexcept {
        owned_ = other.Get();
        cblock_ = other.GetBlock();
        other.Earthing();
    };

    // Aliasing constructor
    // #8 from https://en.cppreference.com/w/cpp/memory/shared_ptr/shared_ptr
    template <typename Y>
    SharedPtr(const SharedPtr<Y>& other, T* ptr) {
        owned_ = ptr;
        cblock_ = other.GetBlock();
        cblock_->IncCounter();
    };

    // Promote `WeakPtr`
    // #11 from https://en.cppreference.com/w/cpp/memory/shared_ptr/shared_ptr
    template <typename Y>
    SharedPtr(const WeakPtr<Y>& other) {
        if (other.Expired()) {
            throw BadWeakPtr();
        }
        owned_ = other.Get();
        cblock_ = other.GetBlock();
        if (cblock_ != nullptr) {
            cblock_->IncCounter();
        }
    };
    explicit SharedPtr(const WeakPtr<T>& other) {
        if (other.Expired()) {
            throw BadWeakPtr();
        }
        owned_ = other.Get();
        cblock_ = other.GetBlock();
        if (cblock_ != nullptr) {
            cblock_->IncCounter();
        }
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-s

    SharedPtr& operator=(const SharedPtr& other) {
        if (this->cblock_ == other.GetBlock()) {
            cblock_->IncCounter();
            return *this;
        }
        Deleter();
        owned_ = other.Get();
        cblock_ = other.GetBlock();
        if (cblock_ != nullptr) {
            cblock_->IncCounter();
        }
        return *this;
    };
    template <typename U>
    SharedPtr& operator=(const SharedPtr<U>& other) {
        if (this->cblock_ == other.GetBlock()) {
            cblock_->IncCounter();
            return *this;
        }
        Deleter();
        owned_ = other.Get();
        cblock_ = other.GetBlock();
        if (cblock_ != nullptr) {
            cblock_->IncCounter();
        }
        return *this;
    };
    template <typename U>
    SharedPtr& operator=(SharedPtr<U>& other) {
        if (this->cblock_ == other.GetBlock()) {
            return *this;
        }
        Deleter();
        owned_ = other.Get();
        cblock_ = other.GetBlock();
        if (cblock_ != nullptr) {
            cblock_->IncCounter();
        }
        return *this;
    };
    template <typename U>
    SharedPtr& operator=(SharedPtr<U>&& other) noexcept {
        if (this->cblock_ == other.GetBlock()) {
            return *this;
        }
        Deleter();
        owned_ = other.Get();
        cblock_ = other.GetBlock();
        other.Earthing();
        return *this;
    };
    SharedPtr& operator=(SharedPtr&& other) noexcept {
        if (this->cblock_ == other.GetBlock()) {
            return *this;
        }
        Deleter();
        owned_ = other.Get();
        cblock_ = other.GetBlock();
        other.Earthing();
        return *this;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Destructor

    ~SharedPtr() {
        Deleter();
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    void Reset() {
        Deleter();
        owned_ = nullptr;
        cblock_ = nullptr;
    };
    void Reset(T* ptr) {
        if (ptr == this->owned_) {
            return;
        }
        Deleter();
        owned_ = ptr;
        cblock_ = new CB1(ptr);
        if constexpr (std::is_base_of_v<ESFT, T>) {
            this->owned_->weak_this_.owned_ = owned_;
            this->owned_->weak_this_.cblock_ = cblock_;
            if (cblock_->Curcw() == 0) {
                cblock_->IncCounterW();
            }
        }
    };
    template <typename U>
    void Reset(U* ptr) {
        if (ptr == this->owned_) {
            return;
        }
        Deleter();
        owned_ = ptr;
        cblock_ = new CB1(ptr);
        if constexpr (std::is_base_of_v<ESFT, T>) {
            this->owned_->weak_this_.owned_ = owned_;
            this->owned_->weak_this_.cblock_ = cblock_;
            if (cblock_->Curcw() == 0) {
                cblock_->IncCounterW();
            }
        }
    };
    void Swap(SharedPtr& other) {
        std::swap(owned_, other.owned_);
        std::swap(cblock_, other.cblock_);
    };
    template <typename U>
    void Swap(SharedPtr<U>& other) {
        std::swap(owned_, static_cast<T*>(other.owned_));
        std::swap(cblock_, other.cblock_);
    };
    void Deleter() {
        if (cblock_ != nullptr) {
            owned_ = nullptr;
            if (UseCount() == 1 and UseCountW() == 0) {
                cblock_->DecCounter();
                CB* base = cblock_;
                cblock_ = nullptr;
                delete base;
            } else if (UseCount() == 1) {
                CB* base = cblock_;
                cblock_ = nullptr;
                base->DecCounter();
            } else {
                cblock_->DecCounter();
                cblock_ = nullptr;
            }
        }
    }

    void Earthing() {
        owned_ = nullptr;
        cblock_ = nullptr;
    }
    CB* GetBlock() const {
        return cblock_;
    }
    T* Get() const {
        return owned_;
    };
    T& operator*() const {
        return *owned_;
    };
    T* operator->() const {
        return owned_;
    };
    size_t UseCount() const {
        if (cblock_ != nullptr) {
            return cblock_->Curc();
        }
        return 0;
    };
    size_t UseCountW() const {
        if (cblock_ != nullptr) {
            return cblock_->Curcw();
        }
        return 0;
    }
    explicit operator bool() const {
        return (owned_ != nullptr);
    };

private:
    T* owned_ = nullptr;
    CB* cblock_ = nullptr;
};

template <typename T, typename U>
inline bool operator==(const SharedPtr<T>& left, const SharedPtr<U>& right) {
    return (left.Get() == right.Get());
}

// Allocate memory only once
template <typename T, typename... Args>
SharedPtr<T> MakeShared(Args&&... args) {
    CB2<T>* pnt = new CB2<T>(std::forward<Args>(args)...);
    return std::move(SharedPtr(pnt, pnt->GetW()));
}

// Look for usage examples in tests
