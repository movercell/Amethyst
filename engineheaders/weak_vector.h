#pragma once

#include <memory>
#include <vector>

template<typename T, typename Allocator = std::allocator<std::weak_ptr<T>>>
class weak_vector {
private:
    std::vector<std::weak_ptr<T>, Allocator> InternalVector;
public:
    weak_vector(decltype(sizeof(0)) capacity = 20, const Allocator& alloc = Allocator()) : InternalVector(std::vector<std::weak_ptr<T>, Allocator>(alloc)) { InternalVector.reserve(capacity); }
    void push_back(std::weak_ptr<T> value) { InternalVector.push_back(value); }
    std::vector<std::shared_ptr<T>> lock() {
        std::vector<std::shared_ptr<T>> result;
        result.reserve(InternalVector.size());
        for (int i = 0; i < InternalVector.size(); i++) {
            std::shared_ptr<T> shared = InternalVector[i].lock();
            if (shared) {
                result.push_back(shared);
            } else {
                auto location = InternalVector.begin() + i;
                std::iter_swap(location, InternalVector.rbegin());
                InternalVector.pop_back();
                i--;
            }
        }
        return result;
    }
};