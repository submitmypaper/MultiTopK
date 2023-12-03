#include "MaxHeap.h"
#pragma warning(disable:4996)£»
void MaxHeap::siftUp(int index)
{
    int parent = (index - 1) / 2;
    while (index > 0 && heap[index].value > heap[parent].value)
    {
        swap(heap[index], heap[parent]);
        index = parent;
        parent = (index - 1) / 2;
    }
}

void MaxHeap::siftDown(int index)
{
    int leftChild = 2 * index + 1;
    int rightChild = 2 * index + 2;
    int largest = index;
    if (leftChild < heap.size() && heap[leftChild].value > heap[largest].value) {
        largest = leftChild;
    }

    if (rightChild < heap.size() && heap[rightChild].value > heap[largest].value) {
        largest = rightChild;
    }

    if (largest != index) {
        swap(heap[index], heap[largest]);
        siftDown(largest);
    }
}

void MaxHeap::changeMax(ResultParm obj)
{
    int index = 0;
    int leftChild = 2 * index + 1;
    int rightChild = 2 * index + 2;
    int largest = index;
    heap[index] = obj;
    if (leftChild < heap.size() && heap[leftChild].value > heap[largest].value)
    {
        largest = leftChild;
    }

    if (rightChild < heap.size() && heap[rightChild].value > heap[largest].value)
    {
        largest = rightChild;
    }

    if (largest != index)
    {
        swap(heap[index], heap[largest]);
        siftDown(largest);
    }
}

void MaxHeap::insert(ResultParm obj)
{
    heap.push_back(obj);
    siftUp(heap.size() - 1);
}

void MaxHeap::removeMax()
{
    if (heap.empty()) {
        return;
    }

    swap(heap[0], heap[heap.size() - 1]);
    heap.pop_back();
    siftDown(0);
}

ResultParm MaxHeap::getMax()
{
    if (heap.empty()) {
        return ResultParm();
    }
    return heap[0];
}

bool MaxHeap::empty()
{
    if (heap.empty()) {
        return true;
    }
    else
    {
        return false;
    }
}

void MaxHeap::clear()
{
    heap.clear();
}

int MaxHeap::getSize()
{
    return heap.size();
}