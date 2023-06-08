#include <iostream>
#include <queue>
#include <functional>
#include <stack>

using namespace std;

template<typename T>
class dtree {

private:

    struct Node {
        T value;
        Node* left;
        Node* right;

        Node(T value) {
            this->value = value;
            left = nullptr;
            right = nullptr;
        }
    };

    class Iterator {
    private:
        std::stack<Node*> nodeStack;

        void pushLeftNodes(Node* node) {
            while (node != nullptr) {
                nodeStack.push(node);
                node = node->left;
            }
        }

    public:
        Iterator(Node* root) {
            pushLeftNodes(root);
        }

        T operator*() const {
            return nodeStack.top()->value;
        }

        Iterator& operator++() {
            Node* node = nodeStack.top();
            nodeStack.pop();
            pushLeftNodes(node->right);
            return *this;
        }

        bool operator!=(const Iterator& other) const {
            return !nodeStack.empty() || !other.nodeStack.empty();
        }
    };

    Node* root;

    Node* subtree(Node* node, std::function<bool(T)> criteria) const {
        if (!node) {
            return nullptr;
        }

        if (criteria(node->value)) {
            Node* new_node = new Node(node->value);
            new_node->left = subtree(node->left, criteria);
            new_node->right = subtree(node->right, criteria);
            return new_node;
        }

        Node* left = subtree(node->left, criteria);
        Node* right = subtree(node->right, criteria);

        if (left && right) {
            Node* new_node = new Node(node->value);
            new_node->left = left;
            new_node->right = right;
            return new_node;
        }
        else {
            return left ? left : right;
        }
    }

    Node* add(Node* node, T value) {
        if (!node) {
            return new Node(value);
        }

        if (value < node->value) {
            node->left = add(node->left, value);
        }
        else if (value > node->value) {
            node->right = add(node->right, value);
        }

        return node;
    }

    Node* find_nearest(Node* node, T value) {
        Node* nearest = node;
        Node* curr = node;

        // Пока текущий узел не станет nullptr
        while (curr) {

            // Если расстояние от значения текущего узла до заданного значения меньше, чем расстояние от ближайшего до заданного значения
            // то ближайшим узлом становится текущий узел
            if (abs(curr->value - value) < abs(nearest->value - value)) {
                nearest = curr;
            }

            // Если заданное значение меньше значения текущего узла, идем в левое поддерево
            if (value < curr->value) {
                curr = curr->left;
            }

            // Если заданное значение больше значения текущего узла, идем в правое поддерево
            else if (value > curr->value) {
                curr = curr->right;
            }

            // Если значение текущего узла равно заданному, выходим из цикла
            else {
                break;
            }
        }

        return nearest;
    }

    Node* del_nearest(Node* node, T value) {
        if (!node) {
            return nullptr;
        }

        // Если значение, которое мы ищем, меньше значения узла, идем в левое поддерево
        if (value < node->value) {
            node->left = del_nearest(node->left, value);
        }
        // Если значение больше значения узла, идем в правое поддерево
        else if (value > node->value) {
            node->right = del_nearest(node->right, value);
        }
        // Если нашли узел с нужным значением
        else {
            if (!node->left && !node->right) { // Если у узла нет потомков
                delete node;
                return nullptr;
            }
            else if (!node->left) { // Если у узла есть только правый потомок
                Node* right = node->right;
                delete node;
                return right;
            }
            else if (!node->right) { // Если у узла есть только левый потомок
                Node* left = node->left;
                delete node;
                return left;
            }
            else { // Если у узла есть оба потомка
                Node* nearest = find_nearest(node, value); // Находим ближайший по значению к заданному объект
                node->value = nearest->value; // Заменяем значение удаляемого узла на значение ближайшего узла
                node->right = del_nearest(node->right, nearest->value); // Удаляем ближайший узел из правого поддерева
            }
        }

        return node;
    }

public:

    // Конструкторы
    dtree() {
        root = nullptr;
    }

    ~dtree() {
        delete root;
    }

    dtree(const dtree& other) {
        root = subtree(other.root, [](T) {return true;});
    }

    Iterator begin() const {
        return Iterator(root);
    }

    Iterator end() const {
        return Iterator(nullptr);
    }

    // Методы 
    dtree<T> subtree(std::function<bool(T)> criteria) const {
        dtree<T> new_tree;
        new_tree.root = subtree(root, criteria);
        return new_tree;
    }

    void add(T value) {
        root = add(root, value);
    }

    void del(T value) {
        root = del_nearest(root, value);
        cout << "Number: " << root << " has been deleted" << endl;
    }

    bool contains(T value) const {
        Node* curr = root;
        while (curr) {
            if (value < curr->value) {
                curr = curr->left;
            }
            else if (value > curr->value) {
                curr = curr->right;
            }
            else {
                return true;
            }
        }
        return false;
    }

    // Операторы
    dtree<T>& operator=(const dtree& other) {
        if (this != &other) {
            delete root;
            root = subtree(other.root, [](T) {return true;});
        }
        return *this;
    }

    dtree<T> operator+(const dtree& other) const {
        dtree<T> newTree(*this); // Create a copy of the current tree
        // Iterate over the other tree using its begin and end iterators
        for (const auto& value : other) {
            newTree.add(value); // Add each value from the other tree to the new tree
        }
        return newTree;
    }

    dtree<T> operator*(const dtree& other) const {
        dtree<T> newTree;
        // Iterate over the current tree using its begin and end iterators
        for (const auto& value : *this) {
            if (other.contains(value)) {
                newTree.add(value); // Add the common value to the new tree
            }
        }
        return newTree;
    }

};

// Функция критерий для int
bool is_greater_than_10(int value) {
    return value > 10;
}

int main() {

    dtree<int> int_tree;
    for (int i = 0; i < 20; i++) {
        int_tree.add(i);
    }

    dtree<int> int_tree1;
    for (int i = 5; i < 30; i++) {
        int_tree1.add(i);
    }

    dtree<int> int_tree2;
    for (int i = 30; i < 40; i++) {
        int_tree2.add(i);
    }

    dtree<int> result_tree;

    cout << "Criteria subtree" << endl;
    result_tree = int_tree.subtree(is_greater_than_10);
    for (const auto& value : result_tree) {
        cout << value << " ";
    }
    cout << endl;

    cout << "Delete 20" << endl;
    result_tree.del(20);
    for (const auto& value : result_tree) {
        cout << value << " ";
    }
    cout << endl;

    cout << "Operator+" << endl;
    result_tree = int_tree + int_tree2;
    for (const auto& value : result_tree) {
        cout << value << " ";
    }
    cout << endl;

    cout << "Operator*" << endl;
    result_tree = int_tree * int_tree1;
    for (const auto& value : result_tree) {
        cout << value << " ";
    }
    cout << endl;
}
