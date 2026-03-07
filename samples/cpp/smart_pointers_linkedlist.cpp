#include <iostream>

struct Node {
    int value;
    std::shared_ptr<Node> next;
    Node(int val) : value(val), next(nullptr) {}
};

class LinkedList {
public:
    std::shared_ptr<Node> head;
    int count = 0;

    LinkedList() : head(nullptr) {}

    void push_front(int val) {
        std::cout << "Before push_front head, head.get(): " << &head << ", " << head.get() << "[" << (head ? std::to_string(head->value) : "null") << "]" << std::endl;
        std::shared_ptr<Node> newNode = std::make_shared<Node>(val);
        newNode->next = head;
        head = newNode;
        std::cout << "After push_front head, head.get(): " << &head << ", " << head.get() << "[" << (head ? std::to_string(head->value) : "null") << "]" << std::endl;
        count++;
    }

    int length() {
        return count;
    }

    void print() {
        std::cout << head.get() << "[" << head->value << "]";
        std::shared_ptr<Node> current = head->next;
        while (current != nullptr) {
            std::cout << " -> " << current.get() << "[" << current->value << "]";
            current = current->next;
        }

    }
};

int main() {
    LinkedList l1;
    std::cout << "Created l1 linked list." << std::endl;
    l1.push_front(10);
    l1.push_front(20);
    l1.push_front(30);
    LinkedList l2 = l1;
    l2.push_front(30);
    std::cout << "Linked l1 list: ";
    l1.print();
    std::cout << std::endl;
    std::cout << "Linked l2 list: ";
    l2.print();
    std::cout << std::endl;
    return 0;
}
