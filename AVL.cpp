#include "AVL.h"

BinarySearchTree::Node::Node(Key key, Value value, 
    Node *parent, Node *left, Node *right): keyValuePair({key, value}), parent(parent), left(left), right(right) {}
BinarySearchTree::Node::~Node()
{
    clear();
}
BinarySearchTree::Node::Node(const Node &other): keyValuePair(other.keyValuePair), parent(nullptr), left(nullptr), right(nullptr), height(other.height)
{
    if (other.left)
    {
        left = new Node(*other.left);
        left -> parent = this;
    }
    if (other.right)
    {
        right = new Node(*other.right);
        right -> parent = this;
    }
}
bool BinarySearchTree::Node::operator==(const Node &other) const
{
    return keyValuePair == other.keyValuePair;
}
int BinarySearchTree::Node::balanceFactor()
{
    int lHeight = left ? static_cast<int>(left -> height) : 0;
    int rHeight = right ? static_cast<int>(right -> height) : 0;
    return lHeight - rHeight;
}
void BinarySearchTree::Node::fixHeight()
{
    unsigned lHeight = left ? left -> height : 0;
    unsigned rHeight = right ? right -> height : 0;
    height = std::max(lHeight, rHeight) + 1;
}
BinarySearchTree::Node *BinarySearchTree::rightRotate(Node *node)
{
    Node *new_node = node -> left;
    node -> left = new_node -> right;
    if (node -> left)
    {
        node -> left -> parent = node;
    }
    new_node -> right = node;
    new_node -> parent = node -> parent;
    node -> parent = new_node;
    node -> fixHeight();
    new_node -> fixHeight();
    return new_node;
}
BinarySearchTree::Node *BinarySearchTree::leftRotate(Node *node)
{
    Node *new_node = node -> right;
    node -> right = new_node -> left;
    if (node -> right)
    {
        node -> right -> parent = node;
    }
    new_node -> left = node;
    new_node -> parent = node -> parent;
    node -> parent = new_node;
    node -> fixHeight();
    new_node -> fixHeight();
    return new_node;
}
BinarySearchTree::Node *BinarySearchTree::balance(Node *node)
{
    node -> fixHeight();
    if (node -> balanceFactor() > 1)
    {
        if (node -> left -> balanceFactor() < 0)
        {
            node -> left = leftRotate(node -> left);
        }
        return rightRotate(node);
    }
    if (node -> balanceFactor() < -1)
    {
        if (node -> right -> balanceFactor() > 0)
        {
            node -> right = rightRotate(node -> right);
        }
        return leftRotate(node);
    }
    return node;
}
void BinarySearchTree::Node::clear()
{
    if (left)
    {
        left -> clear();
        delete left;
        left = nullptr;
    }
    if (right)
    {
        right -> clear();
        delete right;
        right = nullptr;
    }
}
void BinarySearchTree::Node::output_node_tree() const
{
    if (left)
    {
        left -> output_node_tree();
    }
    if (keyValuePair.first != UINT32_MAX)
    {
        std::cout << "(" << keyValuePair.first << ", " << keyValuePair.second << ")\n";
    }
    if (right)
    {
        right -> output_node_tree();
    }
}
BinarySearchTree::Node *BinarySearchTree::insertNode(Node *node, Node *parent, const Key &key, const Value &value)
{
    if (!node)
    {
        return new Node(key, value, parent);
    }
    if (key < node -> keyValuePair.first)
    {
        node -> left = insertNode(node -> left, node, key, value);
    }
    else
    {
        node -> right = insertNode(node -> right, node, key, value);
    }
    return balance(node);
}
void BinarySearchTree::insert(const Key &key, const Value &value)
{
    if (key == UINT32_MAX)
    {
        throw std::invalid_argument("key UINT32_MAX is reserved");
    }
    if (!_root)
    {
        _root = new Node(UINT32_MAX, 0);
    }
    _root = insertNode(_root, nullptr, key, value);
    _size++;
}
BinarySearchTree::Node *BinarySearchTree::eraseMin(Node *node)
{
    if (!node -> left)
    {
        Node *right = node -> right;
        if (right)
        {
            right -> parent = node -> parent;
        }
        node -> right = nullptr;
        delete node;
        return right;
    }
    node -> left = eraseMin(node -> left);
    if (node -> left)
    {
        node -> left -> parent = node;
    }
    return balance(node);
}
BinarySearchTree::Node *BinarySearchTree::eraseNode(Node *node, const Key &key)
{
    if (key < node -> keyValuePair.first)
    {
        node -> left = eraseNode(node -> left, key);
        if (node -> left)
        {
            node -> left -> parent = node;
        }
    }
    else if (key > node -> keyValuePair.first)
    {
        node -> right = eraseNode(node -> right, key);
        if (node -> right)
        {
            node -> right -> parent = node;
        }
    }
    else if (!node -> left || !node -> right)
    {
        Node *child = node -> left ? node -> left : node -> right;
        if (child)
        {
            child -> parent = node -> parent;
        }
        node -> left = nullptr;
        node -> right = nullptr;
        delete node;
        return child;
    }
    else
    {
        Node *minNode = node -> right;
        while (minNode -> left)
        {
            minNode = minNode -> left;
        }
        node -> keyValuePair = minNode -> keyValuePair;
        node -> right = eraseMin(node -> right);
        if (node -> right)
        {
            node -> right -> parent = node;
        }
    }
    return balance(node);
}
void BinarySearchTree::erase(const Key &key)
{
    if (key == UINT32_MAX)
    {
        return;
    }
    while (find(key) != end())
    {
        _root = eraseNode(_root, key);
        _root -> parent = nullptr;
        _size--;
    }
    if (_size == 0)
    {
        delete _root;
        _root = nullptr;
    }
}
BinarySearchTree::BinarySearchTree(const BinarySearchTree &other): _size(other._size), _root(nullptr)
{
    if (other._root)
    {
        _root = new Node(*other._root);
    }
}
BinarySearchTree& BinarySearchTree::operator=(const BinarySearchTree &other)
{
    if (this != &other)
    {
        delete _root;
        _root = nullptr;
        if (other._root)
        {
            _root = new Node(*other._root);
        }
        else
        {
            _root = nullptr;
        }
        _size = other._size;
    }
    return *this;
}
BinarySearchTree::BinarySearchTree(BinarySearchTree &&other) noexcept
{
    *this = std::move(other);
}
BinarySearchTree& BinarySearchTree::operator=(BinarySearchTree &&other) noexcept
{
    if (this == &other)
    {
        return *this;
    }
    if (_root)
    {
        _root -> clear();
        delete _root;
    }
    _size = other._size;
    _root = other._root;
    other._size = 0;
    other._root = nullptr;
    return *this;
}
BinarySearchTree::~BinarySearchTree()
{
    if (_root)
    {
        _root -> clear();
        delete _root;
    }
}
BinarySearchTree::Iterator::Iterator(Node *node): _node(node) {}
std::pair<Key, Value>& BinarySearchTree::Iterator::operator*()
{
    return _node -> keyValuePair;
}
const std::pair<Key, Value>& BinarySearchTree::Iterator::operator*() const
{
    return _node -> keyValuePair;
}
std::pair<Key, Value>* BinarySearchTree::Iterator::operator->()
{
    return &(_node -> keyValuePair);
}
const std::pair<Key, Value>* BinarySearchTree::Iterator::operator->() const
{
    return &(_node -> keyValuePair);
}
BinarySearchTree::Iterator BinarySearchTree::Iterator::operator++()
{
    if (!_node)
    {
        return *this;
    }
    if (_node -> right)
    {
        _node = _node -> right;
        while (_node -> left)
        {
            _node = _node -> left;
        }
    }
    else
    {
        Node *_parent = _node -> parent;
        while (_parent && _parent -> right == _node)
        {
            _node = _parent;
            _parent = _parent -> parent;
        }
        _node = _parent;
    }
    return *this;
}
BinarySearchTree::Iterator BinarySearchTree::Iterator::operator++(int)
{
    Iterator tmp = *this;
    ++(*this);
    return tmp;
}
BinarySearchTree::Iterator BinarySearchTree::Iterator::operator--()
{
    if (!_node)
    {
        return *this;
    }
    if (_node -> left)
    {
        _node = _node -> left;
        while (_node -> right)
        {
            _node = _node -> right;
        }
    } 
    else
    {
        Node* parent = _node -> parent;
        while (parent && _node == parent -> left)
        {
            _node = parent;
            parent = parent -> parent;
        }
        _node = parent;
    }
    return *this;
}
BinarySearchTree::Iterator BinarySearchTree::Iterator::operator--(int)
{
    Iterator tmp = *this;
    --(*this);
    return tmp;
}
bool BinarySearchTree::Iterator::operator==(const Iterator &other) const
{
    return _node == other._node;
}
bool BinarySearchTree::Iterator::operator!=(const Iterator &other) const
{
    return _node != other._node;
}
BinarySearchTree::ConstIterator::ConstIterator(const Node *node): _node(node) {}
const std::pair<Key, Value>& BinarySearchTree::ConstIterator::operator*() const
{
    return _node -> keyValuePair;
}
const std::pair<Key, Value>* BinarySearchTree::ConstIterator::operator->() const
{
    return &(_node -> keyValuePair);
}
BinarySearchTree::ConstIterator BinarySearchTree::ConstIterator::operator++()
{
    if (!_node)
    {
        return *this;
    }
    if (_node -> right)
    {
        _node = _node -> right;
        while (_node -> left)
        {
            _node = _node -> left;
        }
    }
    else
    {
        Node *_parent = _node -> parent;
        while (_parent && _parent -> right == _node)
        {
            _node = _parent;
            _parent = _parent -> parent;
        }
        _node = _parent;
    }
    return *this;
}
BinarySearchTree::ConstIterator BinarySearchTree::ConstIterator::operator++(int)
{
    ConstIterator tmp = *this;
    ++(*this);
    return tmp;
}
BinarySearchTree::ConstIterator BinarySearchTree::ConstIterator::operator--()
{
    if (!_node)
    {
        return *this;
    }
    if (_node -> left)
    {
        _node = _node -> left;
        while (_node -> right)
        {
            _node = _node -> right;
        }
    }
    else
    {
        Node* parent = _node -> parent;
        while (parent && _node == parent -> left)
        {
            _node = parent;
            parent = parent -> parent;
        }
        _node = parent;
    }
    return *this;
}
BinarySearchTree::ConstIterator BinarySearchTree::ConstIterator::operator--(int)
{
    ConstIterator tmp = *this;
    --(*this);
    return tmp;
}
bool BinarySearchTree::ConstIterator::operator==(const ConstIterator &other) const
{
    return _node == other._node;
}
bool BinarySearchTree::ConstIterator::operator!=(const ConstIterator &other) const
{
    return _node != other._node;
}
BinarySearchTree::ConstIterator BinarySearchTree::find(const Key &key) const
{
    Node *current = _root;
    Node *candidate = nullptr;
    while (current)
    {
        if (current -> keyValuePair.first < key)
        {
            current = current -> right;
        }
        else
        {
            candidate = current;
            current = current -> left;
        }
    }
    if (candidate && candidate -> keyValuePair.first == key)
    {
        return ConstIterator(candidate);
    }
    return cend();
}
BinarySearchTree::Iterator BinarySearchTree::find(const Key &key)
{
    Node *current = _root;
    Node *candidate = nullptr;
    while (current)
    {
        if (current -> keyValuePair.first < key)
        {
            current = current -> right;
        }
        else
        {
            candidate = current;
            current = current -> left;
        }
    }
    if (candidate && candidate -> keyValuePair.first == key)
    {
        return Iterator(candidate);
    }
    return end();
}
std::pair<BinarySearchTree::Iterator, BinarySearchTree::Iterator> BinarySearchTree::equalRange(const Key &key)
{
    Iterator itfirst = find(key);
    if (itfirst == end())
    {
        return std::make_pair(itfirst, end());
    }
    Iterator itsecond = itfirst;
    while (itsecond != end() && (*itsecond).first == key)
    {
        itsecond++;
    }
    return std::make_pair(itfirst, itsecond);
}
std::pair<BinarySearchTree::ConstIterator, BinarySearchTree::ConstIterator> BinarySearchTree::equalRange(const Key &key) const
{
    ConstIterator itfirst = find(key);
    if (itfirst == cend())
    {
        return std::make_pair(itfirst, cend());
    }
    ConstIterator itsecond = itfirst;
    while (itsecond != cend() && (*itsecond).first == key)
    {
        itsecond++;
    }
    return std::make_pair(itfirst, itsecond);
}
BinarySearchTree::ConstIterator BinarySearchTree::max() const
{
    ConstIterator it = cend();
    if (_root)
    {
        --it;
    }
    return it;
}
BinarySearchTree::ConstIterator BinarySearchTree::min() const
{
    if (_root)
    {
        Node *current = _root;
        while (current -> left)
        {
            current = current -> left;
        }
        return ConstIterator(current);
    }
    return cend();
}
BinarySearchTree::ConstIterator BinarySearchTree::min(const Key &key) const
{   
    auto er = equalRange(key);
    auto min = er.first;
    for (auto it = er.first; it != er.second; ++it)
    {
        if (min -> second > it -> second)
        {
            min = it;
        }
    }
    return ConstIterator(min);
}
BinarySearchTree::ConstIterator BinarySearchTree::max(const Key &key) const
{
    auto er = equalRange(key);
    auto max = er.first;
    for (auto it = er.first; it != er.second; ++it)
    {
        if (max -> second < it -> second)
        {
            max = it;
        }
    }
    return ConstIterator(max);
}
BinarySearchTree::Iterator BinarySearchTree::begin()
{
    Node *current = _root;
    if (current)
    {
        while (current -> left)
        {
            current = current -> left;
        }
        return Iterator(current);
    }
    return end();
}
BinarySearchTree::Iterator BinarySearchTree::end()
{
    if (!_root)
    {
        return Iterator(nullptr);
    }
    Node *current = _root;
    while (current -> right)
    {
        current = current -> right;
    }
    return Iterator(current);
}
BinarySearchTree::ConstIterator BinarySearchTree::cbegin() const
{
    Node *current = _root;
    if (current)
    {
        while (current -> left)
        {
            current = current -> left;
        }
        return ConstIterator(current);
    }
    return cend();
}
BinarySearchTree::ConstIterator BinarySearchTree::cend() const
{
    if (!_root)
    {
        return ConstIterator(nullptr);
    }
    Node *current = _root;
    while (current -> right)
    {
        current = current -> right;
    }
    return ConstIterator(current);
}
size_t BinarySearchTree::size() const
{
    return _size;
}
void BinarySearchTree::output_tree()
{
    if (_root)
    {
        _root -> output_node_tree();
    }
}
size_t BinarySearchTree::max_height() const
{
    if (!_root)
    {
        return 0;
    }
    return _root -> height;
}
