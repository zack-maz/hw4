#ifndef RBBST_H
#define RBBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor and setting
* the color to red since every new node will be red when it is first inserted.
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}











/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/







































template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert(const std::pair<const Key, Value> &new_item);
    virtual void remove(const Key& key);
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    AVLNode<Key, Value>* predecessor(AVLNode<Key, Value>* n);

    void insertFix(AVLNode<Key,Value>* p, AVLNode<Key,Value>* n);
    void removeFix(AVLNode<Key,Value>* n, int diff);

    void rotateLeft(AVLNode<Key,Value>*& n);
    void rotateRight(AVLNode<Key,Value>*& n);


};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert(const std::pair<const Key, Value> &new_item)
{
    AVLNode<Key, Value>* n = new AVLNode<Key, Value>(new_item.first, new_item.second, nullptr);
    n->setBalance(0); //make sure new node doesnt have a balance
    if (this->root_ == nullptr) this->root_ = n; //fil root if empty
    else
    {
        AVLNode<Key, Value>* curr = static_cast<AVLNode<Key,Value>*>(this->root_); //convert root to avlnode
        AVLNode<Key, Value>* p = nullptr;
        while (curr != nullptr) //look through for correct spot to insert
        {
            p = curr; //parent node for insertfix
            if (n->getKey() < curr->getKey()) curr = curr->getLeft();
            else if (n->getKey() > curr->getKey()) curr = curr->getRight();
            else
            {
                curr->setValue(n->getValue());
                return;
            }
        }

        if (p == nullptr) this->root_ = n;
        else
        {
            n->setParent(p); //set parent and children
            if (p->getKey() > n->getKey()) p->setLeft(n);
            else p->setRight(n);

            if (p->getBalance() == -1 || p->getBalance() == 1) p->setBalance(0); //balance parent node because now it has a second child
            else //if parent node has no children
            {
                if (p->getKey() > n->getKey()) p->updateBalance(-1); //node added left
                else p->updateBalance(1); //node added right
                insertFix(p, n); //rotate and fix balances
            }
        }
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key,Value>* p, AVLNode<Key,Value>* n)
{
    if (p == nullptr) return;
    if (p->getParent() == nullptr) return;

    AVLNode<Key,Value>* g = p->getParent(); //grandparent node

    if (g->getLeft() == p) //check if RR or LR
    {
        g->updateBalance(-1); //grandparent has new child on left side so update balance
        if (g->getBalance() == 0) return; //dont need to fix
        else if (g->getBalance() == -1) insertFix(g, p); //great-grandparent may be off balance, so recurse and check
        else if (g->getBalance() == -2) //off balance so rotate
        {
            if (p->getBalance() == -1) //RR rotation
            {
                rotateRight(g);
                g->setBalance(0);
                p->setBalance(0); //reset balances because n, g, c all have 2 or 0 children
            }
            else if (p->getBalance() == 1) //LR rotation
            {
                rotateLeft(p);
                rotateRight(g);

                if (n->getBalance() == -1)
                {
                    p->setBalance(0);
                    g->setBalance(1);
                    //    G 
                    // P     R
                    //L  N
                    //  x

                    //    N 
                    // P     G
                    //L  x     R
                }
                else if (n->getBalance() == 0)
                {
                    p->setBalance(0);
                    g->setBalance(0);
                    //    G 
                    // P     R
                    //L  N     
                    //  y z

                    //    N 
                    // P     G
                    //L y   z R
                }
                else if (n->getBalance() == 1)
                {
                    p->setBalance(-1);
                    g->setBalance(0);
                    //    G 
                    // P     R
                    //L  N
                    //    x

                    //    N 
                    // P     G
                    //L     x  R
                }
                n->setBalance(0);
            }
        }
    }
    else if (g->getRight() == p)//LL AND RL rotation, logic is mirrored from one above
    {
        g->updateBalance(1);
        if (g->getBalance() == 0) return;
        else if (g->getBalance() == 1) insertFix(g, p);
        else if (g->getBalance() == 2)
        {
            if (p->getBalance() == 1)
            {
                rotateLeft(g);
                g->setBalance(0);
                p->setBalance(0);
            }
            else if (p->getBalance() == -1) 
            {
                rotateRight(p);
                rotateLeft(g);

                if (n->getBalance() == 1)
                {
                    p->setBalance(0);
                    g->setBalance(-1);
                }
                else if (n->getBalance() == 0)
                {
                    p->setBalance(0);
                    g->setBalance(0);
                }
                else if (n->getBalance() == -1)
                {
                    p->setBalance(1);
                    g->setBalance(0);
                }
                n->setBalance(0);
            }
        }
    }



}
    





/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::remove(const Key& key)
{

    //FIND NODE TO DELETE
    if (this->root_ == nullptr) return; //if empty, return
    AVLNode<Key, Value>* n = static_cast<AVLNode<Key,Value>*>(this->internalFind(key));
    if (n == nullptr) return; //if node wanted to be deleted, return




    //IF TWO CHILDREN, SWAP WITH PREDECESSOR
    if (n->getRight() != nullptr && n->getLeft() != nullptr)
    {
        AVLNode<Key, Value>*pred = predecessor(n);
        nodeSwap(n, pred);
    }



    //CALCULATE DIFF
    int diff = 0;
    AVLNode<Key, Value>* p = n->getParent();
    if (p != nullptr)
    {
        if (p->getLeft() == n) diff = 1;
        else diff = -1;
    } //diff is change in balance after node is deleted


    //DELETE N AND UPDATE POINTERS
    if (n->getRight() == nullptr && n->getLeft() == nullptr) //NO CHILDREN
    {
        if (n->getKey() == this->root_->getKey()) this->root_ = nullptr;
        else
        { //delete node and sent parents childrento nullptr
            if (p->getLeft() == n) p->setLeft(nullptr);
            else if (p->getRight() == n) p->setRight(nullptr);
        }
    }
    else //ONE CHILD
    {
      Node<Key, Value>* c; //check which child parent node has
      if (n->getLeft() == nullptr) c = n->getRight();
      else if (n->getRight() == nullptr) c = n->getLeft();

      if (p != nullptr) //if not in root
      { //set pointers
        c->setParent(p);
        if (n == p->getLeft()) p->setLeft(c);
        else if (n == p->getRight()) p->setRight(c);
      }
      else
      { //set root to child node if deleted node was root
        c->setParent(nullptr);
        this->root_ = c;
      }
    }
    delete n;

    //REMOVE-FIX
    removeFix(p, diff); //fix balances with rotations through parent
}

template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key,Value>* n, int diff)
{
    //CHECK NULL
    if (n == nullptr) return;

    //CALCULATE NDIFF
    int ndiff = 0;
    AVLNode<Key, Value>* p = n->getParent();
    if (p != nullptr)
    {
        if (p->getLeft() == n) ndiff = 1;
        else ndiff = -1;
    } //calculate new balance of parent node of current node when current node is deleted -> happens recursively

    //LEFT CHECK
    if (diff == -1)
    {
        AVLNode<Key, Value>* c = n->getLeft(); //TALL CHILD
        if (n->getBalance() + diff == -2) //if we need to rotate when node is deleted
        {
            if (c->getBalance() == -1) //if child node has left child
            {
                rotateRight(n);
                n->setBalance(0);
                c->setBalance(0);
                removeFix(p, ndiff); //typical RR rotation

                //    N
                //  C  [deleted]
                // x

                //   C
                // x   N
            }
            else if (c->getBalance() == 0) //zig-zig
            {
                rotateRight(n);
                n->setBalance(-1);
                c->setBalance(1);

                //  N
                // C [deleted]
                //x y

                //   C
                // x    N
                //    y
            }
            else if (c->getBalance() == 1) //zig-zag
            {
                AVLNode<Key, Value>* gc = c->getRight();
                rotateLeft(c);
                rotateRight(n); //LR rotation
                if (gc->getBalance() == 1)
                {
                    n->setBalance(0);
                    c->setBalance(-1);
                    //     N
                    // C     [deleted]
                    //x gc           z  
                    //    y

                    //  gc
                    // C  N
                    //x  y z 
                }
                else if (gc->getBalance() == 0)
                {
                    n->setBalance(0);
                    c->setBalance(0);
                    //     N
                    // C     [deleted]
                    //  gc            

                    //  gc
                    // C  N
                }
                if (gc->getBalance() == -1)
                {
                    n->setBalance(1);
                    c->setBalance(0);
                    //     N
                    // C     [deleted]
                    //x  gc           z  
                    //  y

                    //  gc
                    // C  N
                    //x y  z 
                }
                gc->setBalance(0);
                removeFix(p, ndiff); //check parent of n's balance now
            }
        }
        else if (n->getBalance() + diff == -1) n->setBalance(-1); //initial balance is 0, so now set it to -1
        else if (n->getBalance() + diff == 0) 
        {
            n->setBalance(0); //n now has both children
            removeFix(p, ndiff); //check parent
        }
    }
    else if (diff == 1) //RIGHT CHECK, mirror diff == -1 but with LL and RL
    {
        AVLNode<Key, Value>* c = n->getRight();
        if (n->getBalance() + diff == 2)
        {
            if (c->getBalance() == 1)
            {
                rotateLeft(n);
                n->setBalance(0);
                c->setBalance(0);
                removeFix(p, ndiff);
            }
            else if (c->getBalance() == 0)
            {
                rotateLeft(n);
                n->setBalance(1);
                c->setBalance(-1);
            }
            else if (c->getBalance() == -1)
            {
                AVLNode<Key, Value>* gc = c->getLeft();
                rotateRight(c);
                rotateLeft(n);
                if (gc->getBalance() == -1)
                {
                    n->setBalance(0);
                    c->setBalance(1);
                }
                else if (gc->getBalance() == 0)
                {
                    n->setBalance(0);
                    c->setBalance(0);
                }
                if (gc->getBalance() == 1)
                {
                    n->setBalance(-1);
                    c->setBalance(0);
                }
                gc->setBalance(0);
                removeFix(p, ndiff);
            }
        }
        else if (n->getBalance() + diff == 1) n->setBalance(1);
        else if (n->getBalance() + diff == 0)
        {
            n->setBalance(0);
            removeFix(p, ndiff);
        }
    }
}







template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key,Value>*& n)
{
    //n serves as grandparent
    AVLNode<Key,Value>* c = n->getRight(); //c is right child
    
    AVLNode<Key,Value>* p = n->getParent(); //p is parent of n
    c->setParent(p); //c replaces spot of n, so set its parent to c
    if (p!= nullptr) //if n is not root
    {
        if (p->getLeft() == n) p->setLeft(c);
        else p->setRight(c); //make sure p's correct child is c
    }
    else this->root_ = c; //else c is now the root

    n->setParent(c); //n's new parent is c after rotation
    AVLNode<Key,Value>* fill = c->getLeft(); //c's left child becomes n, so c's old left child needs to be put somewhere else
    n->setRight(fill); //n loses its right child, so put there
    if (fill != nullptr) fill->setParent(n); //set parent pointer
    c->setLeft(n); //child's left child is n after rotation

}
template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key,Value>*& n)
{
    //mirror rotate left
    AVLNode<Key,Value>* c = n->getLeft();

    AVLNode<Key,Value>* p = n->getParent();
    c->setParent(p);
    if (p!= nullptr)
    {
        if (p->getLeft() == n) p->setLeft(c);
        else p->setRight(c);
    }
    else this->root_ = c;

    n->setParent(c);
    AVLNode<Key,Value>* fill = c->getRight();
    n->setLeft(fill);
    if (fill != nullptr) fill->setParent(n);
    c->setRight(n);

}






/*template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::convertNode(Node<Key, Value>*& n)
{
    return static_cast<AVLNode<Key,Value>*>(n);
}*/


template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::predecessor(AVLNode<Key, Value>* n)
{
    if (n->getLeft() != nullptr) //check if predecessor exists below node
    {
      n = n->getLeft();
      while (n->getRight() != nullptr) n = n->getRight();
      return n;
    }
    else //otherwise predecessor is above node
    {
      Node<Key, Value>* prev = n;
      while (n->getParent() != nullptr)
      {
        n = n->getParent();
        if (prev == n->getRight()) return n;
        prev = n;
      }
    }
    return nullptr;
}


template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}




#endif
