//
// Created by frp on 2021/7/6.
//

#ifndef TEST01_HEAP_HPP
#define TEST01_HEAP_HPP

#include <memory>
#include <algorithm>

namespace frp {
    /*
      类说明：模仿STL的最小堆/最大堆容器，默认的比较器是std::less<T>也就是最小堆，可以构造自己的比较器实现最大堆
    */
  
    template<class T, class Compare = std::less<T>, class Alloc = std::allocator<T>>
    class Heap {
    public:
        using value_type = T;
        using iterator = value_type *;
        using const_iterator = const value_type *;
        using reference = value_type &;
        using pointer = value_type *;
        using size_type = std::size_t;
        using difference_type = ptrdiff_t;
        using value_compare = Compare;

    protected:
        Alloc mAlloc;
        value_compare mCompare;
        iterator mStart;
        iterator mEnd;
        iterator mEndOfStorage;

        void destroy();


    public:
        Heap() : mCompare(value_compare()), mStart(nullptr), mEnd(nullptr), mEndOfStorage(nullptr) {}
        explicit Heap(const value_compare & compare) : mCompare(compare), mStart(nullptr), mEnd(nullptr), mEndOfStorage(nullptr) {}
//        Heap(size_type n, const T &value, const value_compare & compare = Compare());
        Heap(size_type n, const value_compare & compare = Compare());

        virtual ~Heap() { destroy(); }

        iterator begin() { return mStart; }
        iterator end()	 { return mEnd; }
        const_iterator cbegin() const { return mStart; }    //常量迭代器
        const_iterator cend() const { return mEnd; }

        size_type size()  { return size_type(mEnd - mStart); }  //注意转换成size_t类型
        size_type capacity() { return size_type(mEndOfStorage - mStart); }
        bool empty() { return mStart == mEnd; }
        void swap(Heap &other);

        reference operator[](int pos){
            if(pos >= size() || pos < 0)
                throw std::range_error("out of range ");
            return *(mStart+pos);
        }

        reference top();
        void pop();
        void push(const value_type & value);
    };


    /* 没有必要
    template<class T, class Compare, class Alloc>
    Heap<T, Compare, Alloc>::Heap(Heap::size_type n, const T &value, const Heap::value_compare & compare) : mCompare(compare){
        mStart = mAlloc.allocate(n);
        mEnd = mEndOfStorage = mStart + n;
        std::uninitialized_fill(mStart, mEnd, value);
    }
    */

    template<class T, class Compare, class Alloc>
    Heap<T, Compare, Alloc>::Heap(Heap::size_type n, const Heap::value_compare & compare) : mCompare(compare){
        mStart = mAlloc.allocate(n);
        mEnd = mEndOfStorage = mStart + n;
        std::uninitialized_fill(mStart, mEnd, T());
    }

    template<class T, class Compare, class Alloc>
    void Heap<T, Compare, Alloc>::swap(Heap &other) {
        std::swap(mCompare, other.mCompare);
        std::swap(mStart, other.mStart);
        std::swap(mEnd, other.mEnd);
        std::swap(mEndOfStorage, other.mEndOfStorage);
    }

    template<class T, class Compare, class Alloc>
    typename Heap<T, Compare, Alloc>::reference Heap<T, Compare, Alloc>::top() {
        return *mStart;
    }

    template<class T, class Compare, class Alloc>
    void Heap<T, Compare, Alloc>::pop() {
        if(empty()) return;
        //调用析构
        mAlloc.destroy(mStart);
        std::copy(mEnd-1, mEnd, mStart);
        --mEnd;

        int index = 0;
        int son = 2*index+1;
        while (son < size()){
            if((son + 1) < size() && mCompare(*(mStart+son+1), *(mStart+son)))
                ++son;

            if(mCompare(*(mStart+son), *(mStart+index))){
                std::swap(*(mStart+index), *(mStart+son));
            }
            else{
                break;
            }

            index = son;
            son = 2*index+1;
        }
    }

    template<class T, class Compare, class Alloc>
    void Heap<T, Compare, Alloc>::push(const value_type & value) {
        if(mEnd != mEndOfStorage){
            mAlloc.construct(mEnd, value);
            ++mEnd;
        }
        else{
            const size_type old_size = size();
            const size_type len = old_size > 0 ? 2 * old_size : 1;
            iterator new_start = mAlloc.allocate(len);
            iterator new_end = std::uninitialized_copy(mStart, mEnd, new_start);
            mAlloc.construct(new_end, value);

            destroy();

            mStart = new_start;
            mEnd = new_end+1;
            mEndOfStorage = mStart+len;
        }

        int index = size()-1;
        int parent = (index + 1) / 2 - 1;
        while (parent>=0){
            if(mCompare(*(mStart+index), *(mStart+parent))){
                std::swap(*(mStart+parent), *(mStart+index));
                index = parent;
                parent = (index + 1) / 2 - 1;
            }
            else{
                break;
            }
        }
    }

    template<class T, class Compare, class Alloc>
    void Heap<T, Compare, Alloc>::destroy() {
        if(empty()) return;

        //先调用析构
        auto it = mEnd;
        while (it != mStart)
            mAlloc.destroy(--it);

        mAlloc.deallocate(mStart, mEndOfStorage-mStart);
        mStart = mEnd = mEndOfStorage = nullptr;
    }

} // end namespace frp

#endif //TEST01_HEAP_HPP
