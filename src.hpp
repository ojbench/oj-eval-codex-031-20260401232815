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
    // For list values, we support owning and non-owning (weak) references to avoid self cycles
    std::shared_ptr<std::vector<pylist>> sp_list_; // owning reference when present
    std::weak_ptr<std::vector<pylist>> wp_list_;   // weak reference when aliasing self
    // Track the container that owns this element (if this pylist lives inside a list)
    std::weak_ptr<std::vector<pylist>> owner_;

    // Ensure list_ is allocated when needed
    void ensure_list_storage() {
        if (!sp_list_ && wp_list_.expired()) {
            sp_list_ = std::make_shared<std::vector<pylist>>();
        }
    }

    // Access underlying vector (const)
    const std::vector<pylist>& vec() const {
        if (sp_list_) return *sp_list_;
        auto locked = wp_list_.lock();
        return *locked;
    }
    // Access underlying vector (mutable)
    std::vector<pylist>& vec() {
        if (sp_list_) return *sp_list_;
        auto locked = wp_list_.lock();
        return *locked;
    }

    // Helper for pretty printing with cycle detection
    static void print_impl(std::ostream &os, const pylist &val,
                           std::unordered_set<const void*> &visiting) {
        if (!val.is_list_) {
            os << static_cast<long long>(val.int_val_);
            return;
        }

        const void *id = val.sp_list_ ? static_cast<const void*>(val.sp_list_.get())
                                       : static_cast<const void*>(val.wp_list_.lock().get());
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
        const auto &vec = val.vec();
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i) os << ", ";
            print_impl(os, vec[i], visiting);
        }
        os << "]";
        visiting.erase(id);
    }

public:
    // Constructors
    pylist() : is_list_(true), int_val_(0), sp_list_(std::make_shared<std::vector<pylist>>()), wp_list_() {}
    pylist(int x) : is_list_(false), int_val_(x), sp_list_(nullptr), wp_list_() {}

    // Allow implicit conversion to int when this holds an int
    operator int() const { return static_cast<int>(int_val_); }

    // Assignment from int so that elements can be set to int
    pylist &operator=(int x) {
        is_list_ = false;
        int_val_ = x;
        sp_list_.reset();
        wp_list_.reset();
        return *this;
    }

    // Copy assignment from pylist with self-cycle avoidance when assigning to an element
    pylist &operator=(const pylist &rhs) {
        if (this == &rhs) return *this;
        if (rhs.is_list_) {
            // Detect assigning parent list into its own element => use weak alias
            auto parent = owner_.lock();
            const void *parent_id = parent ? static_cast<const void*>(parent.get()) : nullptr;
            const void *rhs_id = rhs.sp_list_ ? static_cast<const void*>(rhs.sp_list_.get())
                                              : static_cast<const void*>(rhs.wp_list_.lock().get());
            if (parent_id && rhs_id && parent_id == rhs_id) {
                is_list_ = true;
                int_val_ = 0;
                sp_list_.reset();
                // Point to parent as weak alias
                wp_list_ = owner_;
                return *this;
            }
        }
        // Default: shallow copy (pass by reference)
        is_list_ = rhs.is_list_;
        int_val_ = rhs.int_val_;
        sp_list_ = rhs.sp_list_;
        wp_list_ = rhs.wp_list_;
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
        // Break self-owning cycle by storing a weak alias when appending self
        const void* this_id = sp_list_ ? static_cast<const void*>(sp_list_.get())
                                       : static_cast<const void*>(wp_list_.lock().get());
        const void* x_id = nullptr;
        if (x.is_list_) {
            x_id = x.sp_list_ ? static_cast<const void*>(x.sp_list_.get())
                              : static_cast<const void*>(x.wp_list_.lock().get());
        }
        if (x.is_list_ && x_id && this_id && x_id == this_id) {
            // push a weak alias to self
            pylist alias;
            alias.is_list_ = true;
            alias.int_val_ = 0;
            alias.sp_list_.reset();
            alias.wp_list_ = sp_list_ ? std::weak_ptr<std::vector<pylist>>(sp_list_)
                                      : wp_list_;
            vec().push_back(alias);
            // set owner on the newly pushed element
            vec().back().owner_ = sp_list_ ? std::weak_ptr<std::vector<pylist>>(sp_list_)
                                           : wp_list_;
        } else {
            vec().push_back(x);
            // set owner on the newly pushed element
            vec().back().owner_ = sp_list_ ? std::weak_ptr<std::vector<pylist>>(sp_list_)
                                           : wp_list_;
        }
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
        if (vec().empty()) return pylist();
        pylist back = std::move(vec().back());
        vec().pop_back();
        return back;
    }

    // Indexing (O(1))
    pylist &operator[](size_t i) {
        ensure_list_storage();
        // Ensure the element knows its owning container
        pylist &ref = vec()[i];
        ref.owner_ = sp_list_ ? std::weak_ptr<std::vector<pylist>>(sp_list_)
                               : wp_list_;
        return ref;
    }
    const pylist &operator[](size_t i) const {
        return vec()[i];
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
