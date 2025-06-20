#include <print>

#include "boost/bimap.hpp"

int main() {
    using bm_type = boost::bimap<int, std::string>;
    using bm_value_type = bm_type::value_type;
    bm_type bm;
    bm.insert(bm_value_type(1, "one"));
    bm.insert(bm_value_type(2, "two"));
    for (auto it = bm.begin(); it != bm.end(); ++it) {
        std::println("relations view: left: {}, right: {}", it->left, it->right);
    }
    for (auto it = bm.right.begin(); it != bm.right.end(); ++it) {
        std::println("right view: key: {}, value: {}", it->first, it->second);
    }
    return 0;
}

