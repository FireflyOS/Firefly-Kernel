#pragma once

#include <cstdint>
#include <frg/manual_box.hpp>
#include <frg/small_vector.hpp>

#include "firefly/memory-manager/allocator.hpp"

struct Stack {
    struct stack {
        uint64_t rbp;
        uint64_t rsp;
    };

    Stack() = default;
    virtual ~Stack() {
    }

    [[gnu::artificial]] virtual inline void add(stack stk) = 0;
    [[gnu::artificial]] virtual inline stack get(int index = 0) const = 0;

protected:
    void operator delete([[maybe_unused]] void *ptr) {
    }
};

class kernelStack : Stack {
public:
    inline void add(stack stk) override {
        stacks.push_back(stk);
    }

    inline stack get(int index = 0) const override {
        return stacks.front();
    }

    inline auto operator[](int index) const {
        return stacks[index];
    }

    inline int howMany() const {
        return stacks.size();
    };

private:
    frg::small_vector<stack, 16, Allocator> stacks;
};

class userStack : Stack {
public:
    inline void add(stack stk) override {
        stacks.push_back(stk);
    }

    inline stack get(int index = 0) const override {
        return stacks.front();
    }

    inline auto operator[](int index) const {
        return stacks[index];
    }

    inline int howMany() const {
        return stacks.size();
    };

private:
    frg::small_vector<stack, 4, Allocator> stacks;
};

extern constinit frg::manual_box<kernelStack> kStack;