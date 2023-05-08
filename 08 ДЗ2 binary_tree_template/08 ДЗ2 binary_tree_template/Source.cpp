// ���� ������������ �������, ��������: 

// std::function<bool(T)>
// other.breadth_first_traversal([&](T value) ������������� ������ �������, ��� �������-���������?


#include <iostream>
#include <queue>
#include <functional>

using namespace std;


class NaturalNumber {
private:
    unsigned int value; 

public:

    // ������������
    NaturalNumber() : value(0) {}
    NaturalNumber(unsigned int val) : value(val) {}

    // ������ ������� � ��������
    unsigned int getValue() const { return value; }
    void setValue(unsigned int val) { value = val; }

    bool operator<(const NaturalNumber& other) const {
        return value < other.value;
    }

    bool operator>(const NaturalNumber& other) const {
        return value > other.value;
    }

    NaturalNumber operator+(const NaturalNumber& other) const {
        return NaturalNumber(value + other.value);
    }

    NaturalNumber operator*(const NaturalNumber& other) const {
        return NaturalNumber(value * other.value);
    }

    // ������� ������
    static void nat_visitor(const NaturalNumber& number) {
        std::cout << number.getValue() << std::endl;
    }
};


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

        // ���� ������� ���� �� ������ nullptr
        while (curr) {

            // ���� ���������� �� �������� �������� ���� �� ��������� �������� ������, ��� ���������� �� ���������� �� ��������� ��������
            // �� ��������� ����� ���������� ������� ����
            if (abs(curr->value - value) < abs(nearest->value - value)) {
                nearest = curr;
            }

            // ���� �������� �������� ������ �������� �������� ����, ���� � ����� ���������
            if (value < curr->value) {
                curr = curr->left;
            }

            // ���� �������� �������� ������ �������� �������� ����, ���� � ������ ���������
            else if (value > curr->value) {
                curr = curr->right;
            }

            // ���� �������� �������� ���� ����� ���������, ������� �� �����
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

        // ���� ��������, ������� �� ����, ������ �������� ����, ���� � ����� ���������
        if (value < node->value) {
            node->left = del_nearest(node->left, value);
        }
        // ���� �������� ������ �������� ����, ���� � ������ ���������
        else if (value > node->value) {
            node->right = del_nearest(node->right, value);
        }
        // ���� ����� ���� � ������ ���������
        else {
            if (!node->left && !node->right) { // ���� � ���� ��� ��������
                delete node;
                return nullptr;
            }
            else if (!node->left) { // ���� � ���� ���� ������ ������ �������
                Node* right = node->right;
                delete node;
                return right;
            }
            else if (!node->right) { // ���� � ���� ���� ������ ����� �������
                Node* left = node->left;
                delete node;
                return left;
            }
            else { // ���� � ���� ���� ��� �������
                Node* nearest = find_nearest(node, value); // ������� ��������� �� �������� � ��������� ������
                node->value = nearest->value; // �������� �������� ���������� ���� �� �������� ���������� ����
                node->right = del_nearest(node->right, nearest->value); // ������� ��������� ���� �� ������� ���������
            }
        }

        return node;
    }

public:

    // ������������
    dtree() {
        root = nullptr;
    }

    ~dtree() {
        delete root;
    }

    dtree(const dtree& other) {
        root = subtree(other.root, [](T) {return true;});
    }


    // ������ 
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

    void breadth_first_traversal(std::function<void(T)> visitor) const {
        std::queue<Node*> q;

        if (root) {
            q.push(root);
        }

        while (!q.empty()) {
            Node* node = q.front();
            q.pop();

            visitor(node->value);

            if (node->left) {
                q.push(node->left);
            }

            if (node->right) {
                q.push(node->right);
            }
        }
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

    // ���������
    dtree<T>& operator=(const dtree& other) {
        if (this != &other) {
            delete root;
            root = subtree(other.root, [](T) {return true;});
        }
        return *this;
    }

    dtree operator+(const dtree& other) const {
        dtree result(*this);
        other.breadth_first_traversal([&](T value) {
            result.add(value);
            });
        return result;
    }

    dtree<T> operator*(const dtree<T>& other) const {
        dtree<T> new_tree;
        std::function<bool(T)> criteria = [&](T value) {
            return other.contains(value);
        };
        new_tree.root = subtree(root, criteria);
        return new_tree;
    }



};

// ������� ������ ��� int
void print_value(int value) {
    std::cout << value << " ";
}

// ������� �������� ��� int
bool is_greater_than_10(int value) {
    return value > 10;
}

int main() {

    dtree<NaturalNumber> nat_tree;
    for (int i = 0; i < 20; i++) {
        nat_tree.add(i);
    }
    //nat_tree.breadth_first_traversal(NaturalNumber::nat_visitor);


    dtree<int> int_tree;
    for (int i = 0; i < 20; i++) {
        int_tree.add(i);
    }
    dtree<int> int_subtree;
    int_subtree = int_tree.subtree(is_greater_than_10);
    //int_subtree.breadth_first_traversal(print_value);

    int_subtree.del(20);
    //int_subtree.breadth_first_traversal(print_value);

    dtree<int> result_tree = int_subtree;
    //result_tree.breadth_first_traversal(NaturalNumber::nat_visitor);

    result_tree = int_tree + int_subtree;
    //result_tree.breadth_first_traversal(print_value);

    result_tree = int_tree * int_subtree;
    result_tree.breadth_first_traversal(print_value);
}
