#pragma once

#include <exception>
#include <algorithm>
class BadWeakPtr : public std::exception {};

template <typename T>
class SharedPtr;

template <typename T>
class WeakPtr;

class ESFT;
template <typename T>
class EnableSharedFromThis;
class CB {
public:
    virtual void IncCounter() = 0;

    virtual void IncCounterW() = 0;

    virtual void DecCounter() = 0;

    virtual void DecCounterW() = 0;

    virtual size_t Curc() = 0;

    virtual size_t Curcw() = 0;

    virtual ~CB(){};
};
template <typename Y>
class CB1 : public CB {
    friend SharedPtr<Y>;
    friend WeakPtr<Y>;

public:
    CB1(Y* other) {
        buff_ = other;
        bib_ = 1;
        bibw_ = 0;
    }
    void IncCounter() override {
        ++bib_;
    }
    void IncCounterW() override {
        ++bibw_;
    }
    void DecCounter() override {
        --bib_;
        if (bib_ == 0) {
            delete buff_;
        }
    }
    void DecCounterW() override {
        --bibw_;
    }
    size_t Curc() override {
        return bib_;
    }
    size_t Curcw() override {
        return bibw_;
    }
    Y* GetW() {
        return buff_;
    };
    ~CB1() override{};

private:
    Y* buff_;
    size_t bib_;
    size_t bibw_;
};
template <typename Y>
class CB2 : public CB {
    friend SharedPtr<Y>;
    friend WeakPtr<Y>;

public:
    template <typename... Args>
    CB2(Args&&... other) {
        ::new (&buff_[0]) Y(std::forward<Args>(other)...);
        ::new (&buff_[1]) size_t(1);
        ::new (&buff_[2]) size_t(0);
    }
    void IncCounter() override {
        ++*(std::launder(reinterpret_cast<size_t*>(&buff_[1])));
    }

    void IncCounterW() override {
        ++*(std::launder(reinterpret_cast<size_t*>(&buff_[2])));
    }
    void DecCounter() override {
        --*(std::launder(reinterpret_cast<size_t*>(&buff_[1])));
        if (Curc() == 0) {
            reinterpret_cast<Y*>(&buff_[0])->~Y();
        }
    }
    void DecCounterW() override {
        --*(std::launder(reinterpret_cast<size_t*>(&buff_[2])));
    }
    Y* GetW() {
        return reinterpret_cast<Y*>(&buff_[0]);
    };
    size_t Curcw() override {
        return *(std::launder(reinterpret_cast<size_t*>(&buff_[2])));
    }
    size_t Curc() override {
        return *(std::launder(reinterpret_cast<size_t*>(&buff_[1])));
    }
    ~CB2() override {
        reinterpret_cast<size_t*>(&buff_[2])->~size_t();
        reinterpret_cast<size_t*>(&buff_[1])->~size_t();
    };

private:
    std::aligned_storage_t<std::max(sizeof(Y), sizeof(size_t)),
                           std::max(alignof(Y), alignof(size_t))>
        buff_[3];
};
