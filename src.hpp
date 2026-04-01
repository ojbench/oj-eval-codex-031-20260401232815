// Implementation of a Python-like list with reference semantics
#ifndef PYLIST_H
#define PYLIST_H

#include <iostream>
#include <vector>
#include <memory>
#include <unordered_set>

class pylist {
private:
    bool is_list_ = true; // default pylist is an empty list
    long long int_val_ = 0; // valid only when is_list_ == false
    std::shared_ptr<std::vector<pylist>> list_; // valid only when is_list_ == true

    // Ensure list_ is allocated when needed
    void ensure_list_storage() {
        if (!list_) {
            list_ = std::make_shared<std::vector<pylist>>();
        }
    }

    // Helper for pretty printing with cycle detection
    static void print_impl(std::ostream &os, const pylist &val,
                           std::unordered_set<const void*> &visiting) {
        if (!val.is_list_) {
            os << static_cast<long long>(val.int_val_);
            return;
        }

        const void *id = val.list_.get();
        if (!id) {
            // Treat null storage as empty list
            os << "[]";
            return;
        }
        if (visiting.find(id) != visiting.end()) {
            os << "[...]";
            return;
        }
        visiting.insert(id);
        os << "[";
        const auto &vec = *val.list_;
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i) os << ", ";
            print_impl(os, vec[i], visiting);
        }
        os << "]";
        visiting.erase(id);
    }

public:
    // Constructors
    pylist() : is_list_(true), int_val_(0), list_(std::make_shared<std::vector<pylist>>()) {}
    pylist(int x) : is_list_(false), int_val_(x), list_(nullptr) {}

    // Allow implicit conversion to int when this holds an int
    operator int() const { return static_cast<int>(int_val_); }

    // Assignment from int so that elements can be set to int
    pylist &operator=(int x) {
        is_list_ = false;
        int_val_ = x;
        list_.reset();
        return *this;
    }

    // Append operations (O(1) amortized)
    void append(const pylist &x) {
        // Assume called on a list as per problem description
        if (!is_list_) {
            // If somehow called on an int, convert to empty list first
            is_list_ = true;
            ensure_list_storage();
        }
        ensure_list_storage();
        list_->push_back(x);
    }

    void append(int x) {
        append(pylist(x));
    }

    // Pop the last element and return it (O(1))
    pylist pop() {
        // Assume valid usage; if empty, return an empty list to avoid UB
        if (!is_list_) {
            return pylist();
        }
        ensure_list_storage();
        if (list_->empty()) return pylist();
        pylist back = std::move(list_->back());
        list_->pop_back();
        return back;
    }

    // Indexing (O(1))
    pylist &operator[](size_t i) {
        ensure_list_storage();
        return (*list_)[i];
    }
    const pylist &operator[](size_t i) const {
        return (*list_)[i];
    }

    // Stream output
    friend std::ostream &operator<<(std::ostream &os, const pylist &ls) {
        if (!ls.is_list_) {
            os << static_cast<long long>(ls.int_val_);
            return os;
        }
        std::unordered_set<const void*> visiting;
        print_impl(os, ls, visiting);
        return os;
    }
};

#endif // PYLIST_H

