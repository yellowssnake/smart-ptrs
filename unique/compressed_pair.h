#pragma once

#include <type_traits>
#include <typeinfo>
#include <memory>
template <typename F, typename S, typename = void>
class CompressedPair {
public:
    CompressedPair() = default;
    CompressedPair(auto&& first, auto&& second) : first_(first), second_(second) {
    }
    CompressedPair(F&& first, auto&& second) : first_(std::move(first)), second_(second) {
    }
    CompressedPair(auto&& first, S&& second) : first_(first), second_(std::move(second)) {
    }
    CompressedPair(F&& first, S&& second) : first_(std::move(first)), second_(std::move(second)) {
    }

    F& GetFirst() {
        return first_;
    }
    const F& GetFirst() const {
        return first_;
    }

    S& GetSecond() {
        return second_;
    };

    const S& GetSecond() const {
        return second_;
    };

private:
    F first_{};
    S second_{};
};
template <typename F, typename S>
class CompressedPair<F, S,
                     std::enable_if_t<std::is_class<F>::value && !std::is_final<F>::value &&
                                      std::is_class<S>::value && !std::is_final<S>::value &&
                                      !std::is_same<F, S>::value>> : F,
                                                                     S {
public:
    CompressedPair() = default;
    CompressedPair(auto&& first, auto&& second) : F(first), S(second) {
    }
    CompressedPair(F&& first, auto&& second) : F(std::move(first)), S(second) {
    }
    CompressedPair(auto&& first, S&& second) : F(first), S(std::move(second)) {
    }
    CompressedPair(F&& first, S&& second) : F(std::move(first)), S(std::move(second)) {
    }

    F& GetFirst() {
        return reinterpret_cast<F&>(*this);
    }
    const F& GetFirst() const {
        return reinterpret_cast<const F&>(*this);
    }
    S& GetSecond() {
        return reinterpret_cast<S&>(*this);
    };

    const S& GetSecond() const {
        return reinterpret_cast<const S&>(*this);
    };
};
template <typename F, typename S>
class CompressedPair<F, S,
                     std::enable_if_t<std::is_class<F>::value && !std::is_final<F>::value &&
                                      (!std::is_class<S>::value || std::is_final<S>::value)>> : F {
public:
    CompressedPair() = default;
    CompressedPair(auto&& first, auto&& second) : F(first), second_(second) {
    }
    CompressedPair(F&& first, auto&& second) : F(std::move(first)), second_(second) {
    }
    CompressedPair(auto&& first, S&& second) : F(first), second_(std::move(second)) {
    }
    CompressedPair(F&& first, S&& second) : F(std::move(first)), second_(std::move(second)) {
    }

    F& GetFirst() {
        return reinterpret_cast<F&>(*this);
    }
    const F& GetFirst() const {
        return reinterpret_cast<const F&>(*this);
    }
    S& GetSecond() {
        return second_;
    };
    const S& GetSecond() const {
        return second_;
    };

private:
    S second_{};
};
template <typename F, typename S>
class CompressedPair<F, S,
                     std::enable_if_t<std::is_class<S>::value && !std::is_final<S>::value &&
                                      (!std::is_class<F>::value || std::is_final<F>::value)>> : S {
public:
    CompressedPair() = default;
    CompressedPair(auto&& first, auto&& second) : first_(first), S(second) {
    }
    CompressedPair(F&& first, auto&& second) : first_(std::move(first)), S(second) {
    }
    CompressedPair(auto&& first, S&& second) : first_(first), S(std::move(second)) {
    }
    CompressedPair(F&& first, S&& second) : first_(std::move(first)), S(std::move(second)) {
    }

    F& GetFirst() {
        return first_;
    }
    const F& GetFirst() const {
        return first_;
    }
    S& GetSecond() {
        return reinterpret_cast<S&>(*this);
    };
    const S& GetSecond() const {
        return reinterpret_cast<const S&>(*this);
    };

private:
    F first_{};
};
