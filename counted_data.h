#ifndef _COUNTED_DATA_H_
#define _COUNTED_DATA_H_

#include <iostream>
#include <vector>

template <typename T> class MyData
{
public:
    MyData() : d(T()) {};
    MyData(const T& i) : d (i) {} ;
    const MyData& operator=(const MyData& rhs) {N_a()++; this->d = rhs.d; return *this;}
    T d;

    static int& N_c() {static int t = 0; return t;} // number of comparison
    static int& N_a() {static int t = 0; return t;} // number of assignment
    static void clear_stat() { N_c() = N_a() = 0;}
    static void print_stat() { std::cout <<    "num of compare  = " << N_c()
                                         << "\t num of assign   = " << N_a() << std::endl; }
};

template <typename T> const bool operator==(const MyData<T>& lhs, const MyData<T>& rhs)
{
    return lhs.d == rhs.d;
}
template <typename T> std::ostream& operator<<(std::ostream& os, const MyData<T>& v)
{
    os << v.d;
    return os;
}

template <typename T> bool operator<(const MyData<T>& lhs, const MyData<T>& rhs)
{
    MyData<T>::N_c()++;
    return lhs.d < rhs.d;
}


template <typename T> std::ostream& operator<<(std::ostream& os, const std::vector<T>& v)
{
    for(auto it = v.begin(); it != v.end(); ++it)
        os << *it << "\t";
    return os;
}

#endif
