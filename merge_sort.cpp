#include <algorithm>
#include <cstdlib>
#include <exception>
#include <ctime>
#include "counted_data.h"

using namespace std;

typedef MyData<int> DataType;


///////////////////////////////////////////////////////////////////////
// In Place Merge Sort, no addtional Buffer is needed
// Algo is described in PRACTICAL IN-PLACE MERGESORT by JYRKI KATAJAINEN

///////////////////////////////////////////////////////////////////////
// merge sub array a1 and a2 into area starting at b3 = (b2 - (e1-b1)) end at e2
// move the original data between b3 and b2 to where a1 was
template <typename T> void merge_two_using_workspace(vector<T>& A,
        const size_t b1, const size_t e1,
        const size_t b2, const size_t e2)
{
    size_t b3 = b2 - (e1 - b1); // starting of destitation index
    size_t i1 = b1, i2 = b2, i3 = b3;
    // TODO avoid swapping in this function
    //const T buf = A[i3]; // create a hole in destination array to avoid swapping
    while(i1 < e1 && i2 < e2)
        if(A[i1] < A[i2])
        {
            //A[i3] = A[i1]
            //A[i1++] = A[++i3];
            swap(A[i3++], A[i1++]);
        }
        else
        {
            //A[i3] = A[i2]
            //A[i2++] = A[++i3];
            swap(A[i3++], A[i2++]);
	}
    while(i1 < e1)
        swap(A[i3++], A[i1++]);
    // since a2 is inplace,  no need to copy left over
}

// swap content of  A[b:e] and A[w:] 
template <typename T> void swap_range(vector<T>& A,
        const size_t b, const size_t e, const size_t w)
{
    auto& W = A; // give A another name
    size_t i1 = b, i2 = w;
    const T buf = A[w]; // make a whole, avoid swapping of each elements
    while(i1 < e)
    {
        A[i2] = A[i1];
        A[i1++] = A[++i2];
    }
    A[i2] = A[i1];
    A[i1] = buf;
}


template <typename T> void merge_sort_using_workspace(vector<T>& A,
        const size_t b, const size_t e, const size_t w)
{
    // workspace has unsorted data, which can be moved around but CANNOT be deleted
    if (b + 1 == e)
        return;
    size_t m = (b + e) / 2;
    merge_sort_using_workspace(A, b, m, w);
    merge_sort_using_workspace(A, m, e, w);
    swap_range(A, b, m, w); // move a1 into workspace
    merge_two_using_workspace(A, w, w + (m - b), m, e);
}

template <typename T> void merge_sort_inplace(vector<T>& A)
{
    size_t n = A.size();
    // sort second half using first half as workspace
    size_t b2 = n  / 2; // begining of sorted array
    merge_sort_using_workspace(A, b2, n, 0);

    while (b2 != 1)
    {
        size_t m = b2 / 2; // devide unsorted into 2
        merge_sort_using_workspace(A, 0, m, m); // sort 1st half using 2n half as workspace
        merge_two_using_workspace(A, 0, m, b2, n); // merge 1st half to sorted large chuck
        b2 -= m;
    }
    while(A[b2] < A[b2-1])
        swap(A[b2], A[b2-1]), b2++;
}
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
// Standard Top Down Merge Sort, using Buffer of half of the array size
template <typename T> void merge_two(vector<T>& A, const size_t b, const size_t m, const size_t e, vector<T>& B)
{
    size_t i1 = 0, i2 = 0, i3 = 0, s = 0;
    // move a1 to buffer
    for(i1 = b, s = 0; i1 < m; ++i1, ++s)
        B[s] =  A[i1];

    // now a1 = B[0:s] , a2 = A[m:e], a3 = A[b:e]
    i1 = 0; // iterator for a1
    i2 = m; // iterator for a2
    i3 = b; // iterator for a
    while(i1 < s && i2 < e) 
        A[i3++] = B[i1] < A[i2] ? B[i1++] : A[i2++];
    while(i1 < s)
        A[i3++] = B[i1++];
    // since a2 is inplace,  no need to copy left over
}


template <typename T> void merge_sort(vector<T>& A, size_t b, size_t e, vector<T>& B)
{
    if (b+1 == e)
        return;
    size_t m = (b + e) / 2;
    merge_sort(A, b, m, B);
    merge_sort(A, m, e, B);
    merge_two(A, b, m, e, B);
}


template <typename T> void merge_sort(vector<T>& A)
{
    vector<T> B(static_cast<size_t>(A.size() / 2));
    merge_sort(A, 0, A.size(), B);
}
///////////////////////////////////////////////////////////////////////


int main()
{
   const int N = 80000;
   //srand(std::time(0));
   std::vector<DataType> A(N);
   for (size_t i = 0; i < A.size(); ++i)
        A[i] = rand() % (2 * N);
        
   auto B = A;
   auto C = A;
   cout << "N = " << N << endl; 
   //cout << "A = " << A << endl;
   
   // baseline using STD algo 
   DataType::clear_stat();
   std::sort(B.begin(), B.end());
   cout << "STL sort" << endl;
   DataType::print_stat();

   DataType::clear_stat();
   merge_sort_inplace(C);
   cout << "My Inplace Merge Sort without Addtional Memory" << endl;
   if(B == C)
       DataType::print_stat();
   else
       cout << "failed" << endl;

   C = A;
   DataType::clear_stat();
   merge_sort(C);
   cout << "My Top Down Merge sort using N/2 buffer" << endl;
   if(B == C)
       DataType::print_stat();
   else
       cout << "failed" << endl;

   vector<int> T;
   for(size_t i = 0; i < 12; ++i)
       T.push_back(i);
   cout << T << endl;
   swap_range(T,0,2, 6);
   cout << T << endl;

   return 0;
}
