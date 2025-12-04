#pragma once

#include <memory>
#include <vector>

template<typename T, typename Allocator = std::allocator<std::weak_ptr<T>>>
class weak_vector {
private:
    std::vector<std::weak_ptr<T>, Allocator> InternalVector;
public:
    weak_vector(decltype(sizeof(0)) size = 20, const Allocator& alloc = Allocator()) : InternalVector(std::vector<std::weak_ptr<T>, Allocator>(size, alloc)) {}
    void push_back(std::weak_ptr<T> value) { InternalVector.push_back(value); }
    std::vector<std::shared_ptr<T>> lock() {
        std::vector<std::shared_ptr<T>> result(InternalVector.size());
        for (int i; i < InternalVector.size(); i++) {
            std::shared_ptr<T> shared = InternalVector[i].lock();
            if (shared) {
                result.push_back(shared);
            } else {
                auto location = InternalVector.begin() + i;
                std::iter_swap(location, InternalVector.back());
                InternalVector.pop_back();
                i--;
            }
        }
        return result;
    }
};