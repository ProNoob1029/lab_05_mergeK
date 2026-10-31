/**
 *  Author: Morar Dragoș Bogdan
 *  Group: 30424
 *  Lab: 5
 *  Subject: Merge K lists
 *  Varying n:
 *      All 3 tests turn out linear, because n is the linear
 *      part of n log k, their only difference is their slope,
 *      given by the log k constant. Higher k gives more
 *      operations.
 *  Varying k:
 *      Varying k gives a curved graph reminiscent of log x,
 *      which confirms that k is the log part of the complexity.
 *      Increasing k high enough eventually adds a diminishing
 *      amount of operations.
 */

#include <iostream>
#include <cstdlib>
#include <ctime>
#include "Profiler.h"

static constexpr int NR_TESTS = 5;

static Profiler profiler("merge_k_lists");

static auto operationName = "demo";
static int dataSize = 0;

struct Node {
    int key;
    Node *next;
};

struct List {
    Node *first;
    Node *last;
};

Node *createNode(int key) {
    auto node = new Node;
    node->key = key;
    node->next = nullptr;
    return node;
}

void initList(List *list) {
    list->first = nullptr;
    list->last = nullptr;
}

void appendKey(List *list, int key) {
    Node *node = createNode(key);
    if (list->first == nullptr) {
        list->first = node;
    } else {
        list->last->next = node;
    }
    list->last = node;
}

void freeList(List *list) {
    Node *current = list->first;
    while (current != nullptr) {
        Node *next = current->next;
        delete current;
        current = next;
    }
    list->first = nullptr;
    list->last = nullptr;
}

void printList(List *list) {
    Node *current = list->first;
    while (current != nullptr) {
        std::cout << current->key << " ";
        current = current->next;
    }
    std::cout << "\n";
}

void generateLists(List lists[], int listCount, int elementCount) {
    auto sizes = new int[listCount];

    for (int i = 0; i < listCount; i++) {
        sizes[i] = 1;
        initList(&lists[i]);
    }
    for (int i = 0; i < elementCount - listCount; i++) {
        sizes[rand() % listCount]++;
    }

    for (int i = 0; i < listCount; i++) {
        int value = rand() % 10;
        for (int position = 0; position < sizes[i]; position++) {
            appendKey(&lists[i], value);
            value += rand() % 10;   //non-decreasing => sorted
        }
    }
    delete[] sizes;
}

void swapNodes(Node **firstSlot, Node **secondSlot) {
    Node *temporary = *firstSlot;
    *firstSlot = *secondSlot;
    *secondSlot = temporary;
    profiler.countOperation(operationName, dataSize, 3);
}

void minHeapify(Node *heap[], int size, int index) {
    while (true) {
        int left = 2 * index;
        int right = 2 * index + 1;
        int minI = index;

        if (left <= size) {
            profiler.countOperation(operationName, dataSize);
            if (heap[left]->key < heap[minI]->key) {
                minI = left;
            }
        }
        if (right <= size) {
            profiler.countOperation(operationName, dataSize);
            if (heap[right]->key < heap[minI]->key) {
                minI = right;
            }
        }
        if (minI == index) {
            break;
        }
        swapNodes(&heap[index], &heap[minI]);
        index = minI;
    }
}

void buildMinHeap(Node *heap[], int size) {
    for (int i = size / 2; i >= 1; i--) {
        minHeapify(heap, size, i);
    }
}

void mergeKLists(List *lists, int listCount, List *result) {
    auto heap = new Node *[listCount + 1];   //slot 0 is unused
    int size = 0;

    initList(result);

    for (int i = 0; i < listCount; i++) {
        if (lists[i].first != nullptr) {
            size++;
            heap[size] = lists[i].first;
            profiler.countOperation(operationName, dataSize);
        }
    }
    buildMinHeap(heap, size);

    while (size > 0) {
        Node *minNode = heap[1];
        Node *next = minNode->next;
        profiler.countOperation(operationName, dataSize, 2);

        if (result->first == nullptr) {
            result->first = minNode;
        } else {
            result->last->next = minNode;
        }
        result->last = minNode;
        profiler.countOperation(operationName, dataSize, 2);

        if (next != nullptr) {
            heap[1] = next;
        } else {
            heap[1] = heap[size];
            size--;
        }
        profiler.countOperation(operationName, dataSize);
        minHeapify(heap, size, 1);
    }

    if (result->last != nullptr) {
        result->last->next = nullptr;
    }
    for (int index = 0; index < listCount; index++) {
        lists[index].first = lists[index].last = nullptr;   //nodes now belong to result
    }
    delete[] heap;
}

void demoMergeTwo() {
    constexpr int k = 4;
    constexpr int n = 20;
    List lists[k];
    List result{};

    operationName = "demo";
    dataSize = n;

    std::cout << "=== Demo: generation (k=" << k << ", n=" << n
              << ") and merging of 2 lists ===\n";
    generateLists(lists, k, n);
    for (int index = 0; index < k; index++) {
        std::cout << "List " << index << ": ";
        printList(&lists[index]);
    }

    std::cout << "Merging list 0 and list 1:\n";
    mergeKLists(lists, 2, &result);   //the first 2 lists of the array
    printList(&result);

    freeList(&result);
    for (int index = 2; index < k; index++) {   //lists 0 and 1 were emptied by the merge
        freeList(&lists[index]);
    }
    std::cout << "\n";
}

void demoMergeK() {
    constexpr int k = 4;
    constexpr int n = 20;
    List lists[k];
    List result{};

    operationName = "demo";
    dataSize = n;

    std::cout << "=== Demo: merging k=" << k << " lists (n=" << n << ") ===\n";
    generateLists(lists, k, n);
    for (int index = 0; index < k; index++) {
        std::cout << "List " << index << ": ";
        printList(&lists[index]);
    }

    mergeKLists(lists, k, &result);
    std::cout << "Merged:  ";
    printList(&result);

    freeList(&result);
    std::cout << "\n";
}

void profileMergeVaryN() {
    int listCounts[3] = {5, 10, 100};
    const char *names[3] = {"merge_k5", "merge_k10", "merge_k100"};
    int maxN = 10000;

    for (int n = 100; n <= maxN; n += 100) {
        std::cout << "N: " << n << " / " << maxN << "\n";
        for (int test = 0; test < NR_TESTS; test++) {
            for (int config = 0; config < 3; config++) {
                int listCount = listCounts[config];
                auto lists = new List[listCount];
                List result{};

                operationName = names[config];
                dataSize = n;

                generateLists(lists, listCount, n);
                mergeKLists(lists, listCount, &result);

                freeList(&result);
                delete[] lists;
            }
        }
    }

    for (auto name : names) {
        profiler.divideValues(name, NR_TESTS);
    }
    profiler.createGroup("Merge k lists varying n", names[0], names[1], names[2]);
}

static void profileMergeVaryK() {
    int elementCount = 10000;
    int maxK = 500;
    char opName[] = "merge_n10000";

    for (int k = 10; k <= maxK; k += 10) {
        std::cout << "K: " << k << " / " << maxK << "\n";
        for (int test = 0; test < NR_TESTS; test++) {
            auto lists = new List[k];
            List result{};

            operationName = opName;
            dataSize = k;

            generateLists(lists, k, elementCount);
            mergeKLists(lists, k, &result);

            freeList(&result);
            delete[] lists;
        }
    }

    profiler.divideValues(opName, NR_TESTS);
    profiler.createGroup("Merge k lists varying k", opName);
}

static void profileMerge() {
    profileMergeVaryN();
    profileMergeVaryK();
    profiler.showReport();
}

int main() {
    //seed random with time
    srand(static_cast<unsigned>(time(nullptr)));

    profileMerge();
    demoMergeTwo();
    demoMergeK();

    return 0;
}