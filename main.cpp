#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

using namespace std;

class BinaryTree {
public:
    virtual void insert(const string& key, const string& value) = 0;
    virtual void remove(const string& key) = 0;
    virtual string search(const string& key) const = 0;
    virtual int height() const = 0;
};

class DictionaryNode {
public:
    string key;
    string value;
    int height;
    DictionaryNode* left;
    DictionaryNode* right;

    DictionaryNode(const string& k, const string& v) : key(k), value(v), height(1), left(nullptr), right(nullptr) {}
};

class AVLTree : public BinaryTree {
private:
    DictionaryNode* root;

    int getHeight(DictionaryNode* node) const {
        return node ? node->height : 0;
    }

    int getBalance(DictionaryNode* node) const {
        return node ? getHeight(node->left) - getHeight(node->right) : 0;
    }

    DictionaryNode* rotateRight(DictionaryNode* y) {
        DictionaryNode* x = y->left;
        DictionaryNode* T2 = x->right;

        x->right = y;
        y->left = T2;

        y->height = 1 + max(getHeight(y->left), getHeight(y->right));
        x->height = 1 + max(getHeight(x->left), getHeight(x->right));

        return x;
    }

    DictionaryNode* rotateLeft(DictionaryNode* x) {
        DictionaryNode* y = x->right;
        DictionaryNode* T2 = y->left;

        y->left = x;
        x->right = T2;

        x->height = 1 + max(getHeight(x->left), getHeight(x->right));
        y->height = 1 + max(getHeight(y->left), getHeight(y->right));

        return y;
    }

    DictionaryNode* balance(DictionaryNode* node) {
        if (node == nullptr)
            return nullptr;

        node->height = 1 + max(getHeight(node->left), getHeight(node->right));

        int balance = getBalance(node);

        if (balance > 1) {
            if (getBalance(node->left) < 0)
                node->left = rotateLeft(node->left);
            return rotateRight(node);
        }

        if (balance < -1) {
            if (getBalance(node->right) > 0)
                node->right = rotateRight(node->right);
            return rotateLeft(node);
        }

        return node;
    }

    DictionaryNode* insert(DictionaryNode* node, const string& key, const string& value) {
        if (node == nullptr)
            return new DictionaryNode(key, value);

        if (key < node->key)
            node->left = insert(node->left, key, value);
        else if (key > node->key)
            node->right = insert(node->right, key, value);
        else
            node->value = value;

        return balance(node);
    }

    DictionaryNode* findMin(DictionaryNode* node) {
        while (node->left != nullptr)
            node = node->left;
        return node;
    }

    DictionaryNode* remove(DictionaryNode* node, const string& key) {
        if (node == nullptr)
            return nullptr;

        if (key < node->key)
            node->left = remove(node->left, key);
        else if (key > node->key)
            node->right = remove(node->right, key);
        else {
            if (node->left == nullptr || node->right == nullptr) {
                DictionaryNode* temp = node->left ? node->left : node->right;
                if (temp == nullptr) {
                    temp = node;
                    node = nullptr;
                } else {
                    *node = *temp;
                }
                delete temp;
            } else {
                DictionaryNode* temp = findMin(node->right);
                node->key = temp->key;
                node->value = temp->value;
                node->right = remove(node->right, temp->key);
            }
        }

        if (node == nullptr)
            return nullptr;

        return balance(node);
    }

    string search(DictionaryNode* node, const string& key) const {
        if (node == nullptr)
            return "Слово не найдено.";

        if (key < node->key)
            return search(node->left, key);
        else if (key > node->key)
            return search(node->right, key);
        else
            return node->value;
    }

    void print(DictionaryNode* node) const {
        if (node == nullptr)
            return;

        print(node->left);
        cout << node->key << " - " << node->value << endl;
        print(node->right);
    }

public:
    AVLTree() : root(nullptr) {}

    void insert(const string& key, const string& value) override {
        root = insert(root, key, value);
    }

    void remove(const string& key) override {
        root = remove(root, key);
    }

    string search(const string& key) const override {
        return search(root, key);
    }

    int height() const override {
        return getHeight(root);
    }

    void print() const {
        print(root);
    }
    ~AVLTree(){}
};

class Dictionary {
private:
    AVLTree russianTree;
    AVLTree englishTree;

public:
    Dictionary(){}
    ~Dictionary(){}
    void loadFromFile(const string& filename) {
        ifstream file(filename);

        if (!file.is_open()) {
            cerr << "Ошибка открытия файла: " << filename << endl;
            return;
        }

        string russianWord, englishWord;

        while (file >> russianWord >> englishWord) {
            russianTree.insert(russianWord, englishWord);
            englishTree.insert(englishWord, russianWord);
        }

        file.close();
    }

    void interactiveSearch() {
        string input;
        while (true) {
            cout << "Введите слово ('выход' или 'exit' для выхода): ";
            getline(cin, input);

            if ((input == "выход" ) || (input == "exit"))
                break;

            string russianResult = russianTree.search(input);
            if (russianResult != "Слово не найдено.") {
                cout << "Перевод: " << russianResult << endl;
            } else {
                string englishResult = englishTree.search(input);
                if (englishResult != "Слово не найдено.") {
                    cout << "Перевод: " << englishResult << endl;
                } else {
                    cout << "Слово не найдено." << endl;
                }
            }
        }
    }

    int height() const {
        cout << "Высота русского дерева: " << russianTree.height() << endl;
        cout << "Высота анлийского дерева: " << englishTree.height() << endl;
    }

    void printTree() const {
        cout << "Русское дерево:" << endl;
        russianTree.print();

        cout << endl << "Английское дерево:" << endl;
        englishTree.print();
    }
};

int main() {
    Dictionary dictionary;
    dictionary.loadFromFile("data.txt");

    dictionary.height();

    dictionary.interactiveSearch();

    cout << "Посмотреть деревья да/нет > ";
    string input;
    getline(cin, input);
    if (input == "да")
        dictionary.printTree();
    else
        cout << "Спасибо за просмотр!"<< endl;
    return 0;
}
