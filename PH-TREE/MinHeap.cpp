#include "MinHeap.h"


bool compvaldesc(const Object& a, const Object& b) {
    return a.value > b.value;
}

void MinHeap::heapifyUp(int index) {
    while (index > 0) {
        int parentIndex = (index - 1) / 2;
        if (heap[index].value < heap[parentIndex].value) {
            std::swap(heap[index], heap[parentIndex]);
            index = parentIndex;
        }
        else {
            break;
        }
    }
}

void MinHeap::heapifyDown(int index) {
    int leftChild = 2 * index + 1;
    int rightChild = 2 * index + 2;
    int smallest = index;

    if (leftChild < heap.size() && heap[leftChild].value < heap[smallest].value) {
        smallest = leftChild;
    }

    if (rightChild < heap.size() && heap[rightChild].value < heap[smallest].value) {
        smallest = rightChild;
    }

    if (smallest != index) {
        std::swap(heap[index], heap[smallest]);
        heapifyDown(smallest);
    }
}

void MinHeap::sortAndUpdateHeap(TreeNode* lastnode) {
    if (heap.empty())return;

    sort(heap.begin(), heap.end(), compvaldesc);
    for (auto it = heap.begin(); it != heap.end(); it++)
    {
        lastnode->nodelist.push_back(*it);
    }
}

void MinHeap::insert(const Object& element) {
    heap.push_back(element);
    heapifyUp(heap.size() - 1);
}

void MinHeap::removeMin() {
    if (!heap.empty()) {
        heap[0] = heap.back();
        heap.pop_back();
        heapifyDown(0);
    }
}

Object MinHeap::getMin() const {
    if (!heap.empty()) {
        return heap[0];
    }
}

size_t MinHeap::size() const {
    return heap.size();
}

bool MinHeap::isEmpty() const {
    return heap.empty();
}

void MinHeap::clear() { heap.clear(); }
