#include <iostream>
#include <queue>

class DoubleSidedLinkedList {
private:
    struct Node {
        int _val;
        Node* next;
        Node* prev;

        Node(int val): _val(val) {};
    };

    int _csize = 0;
    int _msize;
    Node* left;
    Node* right;

public:
    DoubleSidedLinkedList(int size): _msize(size) {
        left = new Node(-1);
        right = new Node(-1);
        left->next = right;
        right->prev = left;
    };

    bool enqueue(int val) {
        if (_csize == _msize) return false;

        Node* nn = new Node(val);
        Node* orp = right->prev;
        orp->next = nn;
        nn->prev = orp;
        nn->next = right;
        right->prev = nn;

        _csize++;
        return true;
    }
    bool dequeue() {
        if (_csize == 0) return false;
        
        Node* dn = left->next;
        left->next = dn->next;
        dn->next->prev = left;
        delete dn;

        _csize--;
        return true;
    }

    int front() {
        return left->next->_val;
    }
    int rear() {
        return right->prev->_val;
    }
    bool isEmpty() {
        return _csize == 0;
    }
    bool isFull() {
        return _csize == _msize;
    }
};

int main() {
    DoubleSidedLinkedList cq(3);
    cq.enqueue(1);
    cq.enqueue(2);
    cq.enqueue(3);
    std::cout << cq.front() << std::endl;
    std::cout << cq.rear() << std::endl;

    return 0;
}
