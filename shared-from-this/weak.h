#pragma once

#include "sw_fwd.h"  // Forward declaration
// https://en.cppreference.com/w/cpp/memory/weak_ptr
template <typename T>
class WeakPtr {
    friend SharedPtr<T>;
    friend EnableSharedFromThis<T>;

public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors

    WeakPtr() {
        owned_ = nullptr;
        cblock_ = nullptr;
    };

    WeakPtr(const WeakPtr& other) {
        owned_ = other.Get();
        cblock_ = other.GetBlock();
        if (cblock_ != nullptr) {
            cblock_->IncCounterW();
        }
    };
    WeakPtr(WeakPtr&& other) {
        owned_ = other.Get();
        cblock_ = other.GetBlock();
        other.Earthing();
    };
    template <typename Y>
    WeakPtr(const WeakPtr<Y>& other) {
        owned_ = other.Get();
        cblock_ = other.GetBlock();
        if (cblock_ != nullptr) {
            cblock_->IncCounterW();
        }
    }

    // Demote `SharedPtr`
    // #2 from https://en.cppreference.com/w/cpp/memory/weak_ptr/weak_ptr
    WeakPtr(const SharedPtr<T>& other) {
        owned_ = other.Get();
        cblock_ = other.GetBlock();
        if (cblock_ != nullptr) {
            cblock_->IncCounterW();
        }
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-s

    WeakPtr& operator=(const WeakPtr& other) {
        if (cblock_ == other.cblock_) {
            return *this;
        }
        WeakDeleter();
        owned_ = other.Get();
        cblock_ = other.GetBlock();
        if (cblock_ != nullptr) {
            cblock_->IncCounterW();
        }
        return *this;
    };
    WeakPtr& operator=(WeakPtr&& other) {
        if (cblock_ == other.cblock_) {
            return *this;
        }
        WeakDeleter();
        owned_ = other.Get();
        cblock_ = other.GetBlock();
        other.Earthing();
        return *this;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Destructor

    ~WeakPtr() {
        WeakDeleter();
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    void Reset() {
        WeakDeleter();
    };
    void Swap(WeakPtr& other) {
        std::swap(owned_, other.owned_);
        std::swap(cblock_, other.cblock_);
    };
    T* Get() const {
        if (!Expired()) {
            return owned_;
        }
        return nullptr;
    }
    CB* GetBlock() const {
        if (cblock_ != nullptr) {
            return cblock_;
        }
        return nullptr;
    }
    void Earthing() {
        owned_ = nullptr;
        cblock_ = nullptr;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Observers

    size_t UseCount() const {
        if (!Expired()) {
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
    bool Expired() const {
        return (cblock_ == nullptr or cblock_->Curc() == 0);
    };
    void WeakDeleter() {
        if (cblock_ != nullptr) {
            owned_ = nullptr;
            if (UseCount() == 0 and UseCountW() == 1) {
                CB* base = cblock_;
                cblock_ = nullptr;
                delete base;
            } else {
                cblock_->DecCounterW();
                cblock_ = nullptr;
            }
        }
    }
    SharedPtr<T> Lock() const {
        if (!Expired()) {
            cblock_->IncCounter();
            return (SharedPtr<T>(cblock_, owned_));
        } else {
            return SharedPtr<T>();
        }
    };

    T* owned_ = nullptr;
    CB* cblock_ = nullptr;
};
