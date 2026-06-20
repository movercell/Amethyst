#pragma once

#include <type_traits>
#include <map>
#include <vector>
#include <list>

namespace Helpers {
    // This was gotten from the internet
    template<class... Ts> struct OverloadedLambda : Ts... { using Ts::operator()...; };

    template <typename T>
    struct is_map : std::false_type {};
    template <typename K, typename V, typename C, typename A>
    struct is_map<std::map<K, V, C, A>> : std::true_type {};

    template <typename T>
    struct is_vector : std::false_type {};
    template <typename T, typename A>
    struct is_vector<std::vector<T, A>> : std::true_type {};

    template <typename T>
    struct is_list : std::false_type {};
    template <typename T, typename A>
    struct is_list<std::list<T, A>> : std::true_type {};
}