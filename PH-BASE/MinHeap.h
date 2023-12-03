#include <vector>
#include <algorithm>
#include "TStream.h"


class MinHeap {
public:
    vector<Object> heap;

    void heapifyUp(int index);
    void heapifyDown(int index);
    void insert(const Object& element);
    void removeMin();
    Object getMin() const;
    size_t size() const;
    bool isEmpty() const;
    void clear();
    void sortAndUpdateHeap(TreeNode* lastnode);

};


