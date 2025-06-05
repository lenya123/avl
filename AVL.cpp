#include "AVL.h"

BinarySearchTree::Node::Node(Key key, Value value, 
    Node *parent, Node *left, Node *right): keyValuePair({key, value}), parent(parent), left(left), right(right) {}
BinarySearchTree::Node::~Node()
{
    clear();
}
BinarySearchTree::Node::Node(const Node &other): keyValuePair(other.keyValuePair), parent(nullptr), right(nullptr), left(nullptr)
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
    if (!left && right)
    {
        return 0 - right -> height;
    }
    else if (!right && left)
    {
        return left -> height;
    }
    else if (!left && !right)
    {
        return 0;
    }
    return left -> height - right -> height;
}
void BinarySearchTree::Node::fixHeight()
{
    unsigned lHeight = left ? left -> height : 0;
    unsigned rHeight = right ? right -> height : 0;
    height = std::max(lHeight, rHeight) + 1;
}
BinarySearchTree::Node *BinarySearchTree::rightRotate(Node *node)
{
    if (!node || !node -> left)
    {
        return node;
    }
    Node *new_node = node -> left;
    node -> left = new_node -> right;
    if (new_node -> right)
    {
        new_node -> right -> parent = node;
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
    if (!node || !node -> right)
    {
        return node;
    }
    Node *new_node = node -> right;
    node -> right = new_node -> left;
    if (new_node -> left)
    {
        new_node -> left -> parent = node;
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
    if (!node || node->keyValuePair.first == UINT32_MAX)
    {
        return node;
    }
    node -> fixHeight();
    if (node -> balanceFactor() == 2)
    {
        if (node -> left && node -> left -> balanceFactor() < 0)
        {
            node -> left = leftRotate(node -> left);
        }
        return rightRotate(node);
    }
    if (node -> balanceFactor() == -2)
    {
        if (node -> right && node -> right -> balanceFactor() > 0)
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
    std::cout << "(" << keyValuePair.first << ", " << keyValuePair.second << ")\n";
    if (right && right -> keyValuePair.first != UINT32_MAX)
    {
        right -> output_node_tree();
    }
}
void BinarySearchTree::Node::insert(const Key &key, const Value &value)
{
    if (key < keyValuePair.first)
    {
        if (!left)
        {
            left = new Node(key, value, this);
        }
        else
        {
            left -> insert(key, value);
        }
    }
    else
    {
        if (!right)
        {
            right = new Node(key, value, this);
        }
        else
        {
            right -> insert(key, value);
        }
    }
    fixHeight();
}
void BinarySearchTree::insert(const Key &key, const Value &value)
{
    if (!_root)
    {
        _root = new Node(key, value);
        _root -> right = new Node(UINT32_MAX, 0);
        _root -> right -> parent = _root;
        _root -> right -> right = nullptr;
        _root -> right -> left = nullptr;
        _size = 1;
    }
    else
    {
        auto it = max();
        Key maximum = (*it).first;
        if (key >= maximum)
        {
            _root -> erase(UINT32_MAX);
            _root -> insert(key, value);
            _root = balance(_root);
            _root -> insert(UINT32_MAX, 0);
        }
        else
        {
            _root -> insert(key, value);
            _root = balance(_root);
        }
        _size++;
    }
}
void BinarySearchTree::Node::erase(const Key &key)
{
    if (key < keyValuePair.first)
    {
        if (left)
        {
            left -> erase(key);
            left = EraseAdvance(left);
            if (left)
            {
                left -> parent = this;
            }
            fixHeight();
            left = balance(left);
            if (left)
            {
                left -> parent = this;
            }
        }
    }
    else if (key > keyValuePair.first)
    {
        if (right)
        {
            right -> erase(key);
            right = EraseAdvance(right);
            if (right)
            {
                right -> parent = this;
            }
            fixHeight();
            right = balance(right);
            if (right)
            {
                right -> parent = this;
            }
        }
    }
    else
    {
        if (left && right)
        {
            Node *minNode = right;
            while (minNode -> left)
            {
                minNode = minNode -> left;
            }
            keyValuePair = minNode -> keyValuePair;
            right -> erase(keyValuePair.first);
            right = EraseAdvance(right);
            if (right)
            {
                right -> parent = this;
            }
            fixHeight();
            right = balance(right);
            if (right)
            {
                right -> parent = this;
            }
        }
        else
        {
            return;
        }
    }
    fixHeight();
}

void BinarySearchTree::erase(const Key &key)
{
    if (!_root) 
    {
        return;
    }
    while (find(key) != end())
    {
        if (_root -> keyValuePair.first == key)
        {
            _root = Node::EraseAdvance(_root);
            if (_root)
            {
                _root -> parent = nullptr;
                _root = balance(_root);
            }
        }
        else
        {
            _root -> erase(key);
            _root = balance(_root);
        }
        _size--;
    }
} 
BinarySearchTree::Node *BinarySearchTree::Node::EraseAdvance(Node *target)
{
    if (!target -> left && !target -> right)
    {
        delete target;
        return nullptr;
    }
    else if (!target -> left)
    {
        Node *temp = target -> right;
        temp -> parent = target -> parent;
        target -> left = nullptr;
        target -> right = nullptr;
        delete target;
        return temp;
    }
    else if (!target -> right)
    {
        Node *temp = target -> left;
        temp -> parent = target -> parent;
        target -> left = nullptr;
        target -> right = nullptr;
        delete target;
        return temp;
    }
    else
    {
        Node *rt_subtree = target -> right;
        while (rt_subtree -> left)
        {
            rt_subtree = rt_subtree -> left;
        }
        target -> keyValuePair = rt_subtree -> keyValuePair;
        rt_subtree -> parent -> right = EraseAdvance(rt_subtree -> parent -> right);
        return target;
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
        return Iterator(nullptr);
    }
    Node *temp = _node;
    while (temp -> parent)
    {
        temp = temp -> parent;
    }
    while (temp -> right)
    {
        temp = temp -> right;
    }
    if (temp -> parent == _node)
    {
        _node = temp;
        return Iterator(_node);
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
        return Iterator(nullptr);
    }
    if (_node -> keyValuePair.first == UINT32_MAX)
    {
        _node = _node -> parent;
        return Iterator(_node);
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
        return ConstIterator(nullptr);
    }
    const Node *temp = _node;
    while (temp -> parent)
    {
        temp = temp -> parent;
    }
    while (temp -> right)
    {
        temp = temp -> right;
    }
    if (temp -> parent == _node)
    {
        _node = temp;
        return ConstIterator(_node);
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
        return ConstIterator(nullptr);
    }
    if (_node -> keyValuePair.first == UINT32_MAX)
    {
        _node = _node -> parent;
        return ConstIterator(_node);
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
    Node *firstmatch = nullptr;
    while (current)
    {
        if (key < current -> keyValuePair.first)
        {
            current = current -> left;
        }
        else if (key > current  ->keyValuePair.first)
        {
            current = current -> right;
        }
        else
        {
            firstmatch = current;
            while (current -> left && current -> left -> right && current -> left -> right -> keyValuePair.first == key)
            {
                current = current -> left -> right;
                firstmatch = current;
            }
            return ConstIterator(firstmatch);
        }
    }
    return cend();
}
BinarySearchTree::Iterator BinarySearchTree::find(const Key &key)
{
    Node *current = _root;
    Node *firstmatch = nullptr;
    while (current)
    {
        if (key < current -> keyValuePair.first)
        {
            current = current -> left;
        }
        else if (current->keyValuePair.first < key)
        {
            current = current -> right;
        }
        else
        {
            firstmatch = current;
            while (current -> left && current -> left -> right && current -> left -> right -> keyValuePair.first == key)
            {
                current = current -> left -> right;
                firstmatch = current;
            }
            return Iterator(firstmatch);
        }
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
    if (_root)
    {
        Node *current = _root;
        while (current -> right)
        {
            current = current -> right;
        }
        return ConstIterator(current -> parent);
    }
    return cend();
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
    while (current -> right && current -> right -> keyValuePair.first != UINT32_MAX)
    {
        current = current -> right;
    }
    return Iterator(current -> right);
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
    while (current -> right && current -> right -> keyValuePair.first != UINT32_MAX)
    {
        current = current -> right;
    }
    return ConstIterator(current -> right);
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
