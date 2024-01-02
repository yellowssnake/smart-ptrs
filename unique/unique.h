#pragma once

#include "compressed_pair.h"

#include <cstddef>  // std::nullptr_t
template <typename T>
struct Slug {
    Slug() = default;
    template <typename O>
    Slug(Slug<O>&& other) {
        *this = Slug<T>{};
    };
    void operator()(T* ptr) {
        delete ptr;
    }
};

// Specialization for arrays
template <typename T>
struct Slug<T[]> {
    Slug() = default;
    template <typename O>
    Slug(Slug<O>&& other) {
        *this = Slug<T>{};
    };
    void operator()(T* ptr) {
        delete[] ptr;
    }
};
// Primary template
template <typename T, typename Deleter = Slug<T>>
class UniquePtr {
public:
    explicit UniquePtr(T* ptr = nullptr) {
        cmp_.GetFirst() = ptr;
    };
    UniquePtr(T* ptr, Deleter deleter) : cmp_(ptr, std::move(deleter)){};

    UniquePtr(UniquePtr&& other) noexcept {
        if (this != &other) {
            cmp_.GetSecond()(cmp_.GetFirst());
            cmp_.GetFirst() = other.Get();
            cmp_.GetSecond() = std::move(other.GetDeleter());
            other.cmp_.GetFirst() = nullptr;
        }
    };
    UniquePtr(UniquePtr& other) = delete;
    template <class T2, class Del2>
    UniquePtr(UniquePtr<T2, Del2>&& right) noexcept {
        cmp_.GetSecond()(cmp_.GetFirst());
        cmp_.GetFirst() = std::move(right.Get());
        cmp_.GetSecond() = std::move(right.GetDeleter());
        right.Earthing();
    }

    UniquePtr(const UniquePtr& right) = delete;
    UniquePtr& operator=(const UniquePtr& right) = delete;

    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (this == &other) {
            return *this;
        } else {
            std::swap(cmp_.GetFirst(), other.cmp_.GetFirst());
            std::swap(cmp_.GetSecond(), other.cmp_.GetSecond());
            T* pt = other.Get();
            other.cmp_.GetFirst() = nullptr;
            other.cmp_.GetSecond()(pt);
            return *this;
        }
    };
    UniquePtr& operator=(std::nullptr_t) {
        cmp_.GetSecond()(this->Get());
        cmp_.GetFirst() = nullptr;
        return *this;
    };
    void Earthing() {
        cmp_.GetFirst() = nullptr;
    }

    ~UniquePtr() {
        if (cmp_.GetFirst() != nullptr) {
            T* pt = this->Release();
            cmp_.GetSecond()(pt);
        }
    };

    T* Release() {
        T* bib = this->Get();
        cmp_.GetFirst() = nullptr;
        return bib;
    };
    void Reset(T* ptr = nullptr) {
        T* pt = cmp_.GetFirst();
        cmp_.GetFirst() = ptr;
        cmp_.GetSecond()(pt);
    };
    void Swap(UniquePtr& other) {
        std::swap(other.cmp_.GetFirst(), cmp_.GetFirst());
        std::swap(other.cmp_.GetSecond(), cmp_.GetSecond());
    };

    T* Get() const {
        return cmp_.GetFirst();
    };
    Deleter& GetDeleter() {
        return cmp_.GetSecond();
    };
    const Deleter& GetDeleter() const {
        return cmp_.GetSecond();
    };
    explicit operator bool() const {
        return (this->Get() != nullptr);
    };

    std::add_lvalue_reference_t<T> operator*() const {
        return *reinterpret_cast<T*>(cmp_.GetFirst());
    };
    T* operator->() const {
        return cmp_.GetFirst();
    };

private:
    CompressedPair<T*, Deleter> cmp_;
};
// Specialization for arrays
template <typename T, typename Deleter>
class UniquePtr<T[], Deleter> {
public:
    explicit UniquePtr(T* ptr = nullptr) {
        cmp_.GetFirst() = ptr;
    };
    UniquePtr(T* ptr, Deleter deleter) : cmp_(ptr, std::move(deleter)){};

    UniquePtr(UniquePtr&& other) noexcept {
        if (this != &other) {
            cmp_.GetFirst() = other.Get();
            cmp_.GetSecond() = std::move(other.GetDeleter());
            other.cmp_.GetFirst() = nullptr;
        }
    };
    UniquePtr(UniquePtr& other) = delete;
    template <class T2, class Del2>
    UniquePtr(UniquePtr<T2, Del2>&& right) noexcept {
        cmp_.GetSecond()(cmp_.GetFirst());
        cmp_.GetFirst() = std::move(right.Get());
        cmp_.GetSecond() = std::move(right.GetDeleter());
        right.Earthing();
    }
    UniquePtr(const UniquePtr& right) = delete;
    UniquePtr& operator=(const UniquePtr& right) = delete;

    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (this == &other) {
            return *this;
        } else {
            std::swap(cmp_.GetFirst(), other.cmp_.GetFirst());
            std::swap(cmp_.GetSecond(), other.cmp_.GetSecond());
            T* pt = other.Get();
            other.cmp_.GetFirst() = nullptr;
            other.cmp_.GetSecond()(pt);
            return *this;
        }
    };
    UniquePtr& operator=(std::nullptr_t) {
        cmp_.GetSecond()(this->Get());
        cmp_.GetFirst() = nullptr;
        return *this;
    };

    ~UniquePtr() {
        if (cmp_.GetFirst() != nullptr) {
            T* pt = this->Release();
            cmp_.GetSecond()(pt);
        }
    };
    void Earthing() {
        cmp_.GetFirst() = nullptr;
    }

    T* Release() {
        T* bib = this->Get();
        cmp_.GetFirst() = nullptr;
        return bib;
    };
    void Reset(T* ptr = nullptr) {
        T* pt = cmp_.GetFirst();
        cmp_.GetFirst() = ptr;
        cmp_.GetSecond()(pt);
    };
    void Swap(UniquePtr& other) {
        std::swap(other.cmp_.GetFirst(), cmp_.GetFirst());
        std::swap(other.cmp_.GetSecond(), cmp_.GetSecond());
    };
    T& operator[](size_t szt) {
        return *(this->Get() + szt);
    }

    T* Get() const {
        return cmp_.GetFirst();
    };
    Deleter& GetDeleter() {
        return cmp_.GetSecond();
    };
    const Deleter& GetDeleter() const {
        return cmp_.GetSecond();
    };
    explicit operator bool() const {
        return (this->Get() != nullptr);
    };

    std::add_lvalue_reference_t<T> operator*() const {
        return *reinterpret_cast<T*>(cmp_.GetFirst());
    };
    T* operator->() const {
        return cmp_.GetFirst();
    };

private:
    CompressedPair<T*, Deleter> cmp_;
};
