#pragma once
#include <new>

template<typename T>
class Arena {
    T* memory;
    T* free;
    T* end;

public:
    static Arena<T>* current;
    Arena<T>* parent;

    Arena(std::size_t n) : parent(current) {
        memory = static_cast<T*>(::operator new(n*sizeof(T)));
        free = memory;
        end = memory + n;
        current = this;
    }

    ~Arena() {
        current = parent;
        ::operator delete(memory);
    }

    T* alloc() {
        if (free >= end)
            throw std::bad_alloc();
        return free++;
    }
};

template<typename T>
Arena<T>* Arena<T>::current = nullptr;
