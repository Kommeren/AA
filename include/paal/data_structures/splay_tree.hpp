/**
 * @file splay_tree.hpp
 * @brief 
 * @author unknown
 * @version 1.0
 * @date 2013-07-24
 */
#ifndef SPLAY_TREE_HPP
#define SPLAY_TREE_HPP

#include <boost/utility.hpp>
#include <boost/iterator.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <algorithm>
#include <unordered_map>

namespace paal {
namespace data_structures {
namespace splay_tree {

  /**
   * @param node root of a subtree
   * @returns size of subtree
   **/
  template<typename N> inline std::size_t node_size(N node) {
    return (node == NULL) ? 0 : node->size();
  }
  
  template<typename N> inline N * copy_node(N * node) {
    return (node == NULL) ? NULL : new N(*node);
  }

  /**
   * Node of a SplayTree.
   *
   * Left/right relaxation should be understood as follows.
   * Meaning of left/right field changes iff xor of all bits on the path to the
   * root is 1. This enables us to reverse entire subtree in constant time (by
   * flipping bit in the root). Normalization is needed to determine which child is
   * the real left/right. */
  template<typename V> class Node {
    public:
      typedef V value_type;
      typedef Node<value_type> node_type;

      /** @param val stored value */
      explicit Node(const value_type &val) : val_(val), left_(NULL), 
        right_(NULL), parent_(NULL), reversed_(false), size_(1) {
      }
      
      Node(const Node & n) : val_(n.val_), left_(copy_node(n.left_)), 
        right_(copy_node(n.right_)), parent_(NULL), reversed_(n.reversed_), 
        size_(n.size_) {
            if(right_) {
                right_->parent_ = this;
            }
            if(left_) {
                left_->parent_ = this;
            }
      }

      /** @returns parent node */
      node_type *parent() {
        return parent_;
      }

      /** @brief detaches this node from parent */
      void make_root() {
        parent_ = NULL;
      }

      /** @returns true left child pointer */
      node_type *left() {
        normalize();
        return left_;
      }

      /**
       * @brief sets true left child pointer
       * @param node new child
       **/
      void set_left(node_type *node) {
        normalize();
        set_left_internal(node);
        update_size();
      }

      /**
       * @brief sets left child pointer (no relaxation)
       * @param node new child
       **/
      void set_left_internal(node_type *node) {
        left_ = node;
        if (node != NULL) {
          node->parent_ = this;
        }
      }

      /** @returns true right child pointer */
      node_type *right() {
        normalize();
        return right_;
      }

      /**
       * @brief sets true right child pointer
       * @param node new child
       **/
      void set_right(node_type *node) {
        normalize();
        set_right_internal(node);
        update_size();
      }

      /**
       * @brief sets right child pointer (no relaxation)
       * @param node new child
       **/
      void set_right_internal(node_type *node) {
        right_ = node;
        if (node != NULL) {
          node->parent_ = this;
        }
      }

      /** @brief recomputes subtree size from sizes of children's subtrees */
      void update_size() {
        size_ = 1 + ((left_ != NULL) ? left_->size_ : 0)
                + ((right_ != NULL) ? right_->size_ : 0);
      }

      /** @returns next in same tree according to infix order 
       * WARNING, we assume that path from root to the this node is normalized*/
      node_type *next() {
        node_type *node = right();
        if (node != NULL) {
          return node->subtree_min();
        } else {
          node_type *last = NULL;
          node = this;
          for (;;) {
            last = node;
            node = node->parent();
            if (node == NULL) {
              return NULL;
            } else if (node->left() == last) {
              return node;
            }
          }
        }
      }

      /** @returns previous in same tree according to infix order */
      node_type *prev() {
        node_type *node = left();
        if (node != NULL) {
          return node->subtree_max();
        } else {
          node_type *last = NULL;
          node = this;
          for (;;) {
            last = node;
            node = node->parent();
            if (node == NULL) {
              return NULL;
            } else if (node->right() == last) {
              return node;
            }
          }
        }
      }

      /** @returns first node in subtree according to infix order */
      node_type *subtree_min() {
        node_type *node = this;
        normalize();
        while (node->left() != NULL) {
          node = node->left();
          node->normalize();
        }
        return node;
      }

      /** @returns last node in subtree according to infix order */
      node_type *subtree_max() {
        node_type *node = this;
        normalize();
        while (node->right() != NULL) {
          node = node->right();
          node->normalize();
        }
        return node;
      }

      /** @returns size of subtree */
      std::size_t size() {
        return size_;
      }

      /** @brief lazily reverses order in subtree */
      void subtree_reverse() {
        reversed_ ^= 1;
      }

      /** @brief locally relaxes tree */
      void normalize() {
        if (reversed_) {
          std::swap(left_, right_);
          if (left_ != NULL) {
            left_->subtree_reverse();
          }
          if (right_ != NULL) {
            right_->subtree_reverse();
          }
          reversed_ = false;
        }
      }

      /** @brief relaxes all nodes on path from root to this */
      void normalize_root_path() {
        node_type *node = parent();
        if (node != NULL) {
          node->normalize_root_path();
        }
        normalize();
      }

      value_type val_;

    private:
      static const bool kDefLeft = 0;
      node_type *left_ = NULL, *right_ = NULL;
      node_type *parent_;
      bool reversed_;
      std::size_t size_;
  };
  
  /** @brief splay policy */
  enum SplayImplEnum {
    /** splaying goes from root, resulting tree is less balanced */
    kTopDownUnbalanced,
    /** splaying goes from root */
    kTopDown,
    /** splaying goes from node to become root */
    kBottomUp
  };


  template <typename T, SplayImplEnum s> class SplayTree;

  /**
   * SplayTree elements iterator.
   *
   * Traversing order is determined by template argument.
   **/
  template<typename V, SplayImplEnum splay_impl = kTopDownUnbalanced, bool IsForward = true> class Iterator
    : public boost::iterator_facade <
    Iterator<V, splay_impl, IsForward>,
    Node<V>*,
    boost::bidirectional_traversal_tag,
      V& > {
      typedef SplayTree<V, splay_impl> ST;
    public:
      typedef V value_type;
      typedef Node<value_type> node_type;

      /** @brief iterator after last element */
      Iterator() : current_(NULL), rotation_cnt_(0), splay_(NULL) {
      }

      /**
       * @brief iterator to element in given node
       * @param node node storing element pointed by iterator
       * @param splay pointer to the splay tree
       **/
      explicit Iterator(node_type *node, const ST * splay) : current_(node), rotation_cnt_(0), splay_(splay) {
      }

      /**
       * @brief copy constructor
       * @param other iterator to be copied
       **/
      Iterator(const Iterator &other) :
        current_(other.current_), rotation_cnt_(0), splay_(other.splay_) {
      }

    private:
      friend class boost::iterator_core_access;
      friend class SplayTree<V, kTopDownUnbalanced>;
      friend class SplayTree<V, kTopDown>;
      friend class SplayTree<V, kBottomUp>;

      void normalize() {
        if(rotation_cnt_ != splay_->getRotationCnt()) {
            current_->normalize_root_path();
            rotation_cnt_ = splay_->getRotationCnt();
        }
      }

      /** @brief increments iterator */
      void increment() {
        normalize();
        if (IsForward) {
          current_ = current_->next();
        } else {
          current_ = current_->prev();
        }
      }

      /** @brief decrements iterator */
      void decrement() {
        normalize();
        if (IsForward) {
          current_ = current_->prev();
        } else {
          current_ = current_->next();
        }
      }

      /**
       * @param other iterator to be compared with
       * @returns true iff iterators point to the same node
       **/
      bool equal(const Iterator &other) const {
        return this->current_ == other.current_;
      }

      /** @returns reference to pointed element */
      value_type& dereference() const {
        return current_->val_;
      }

      /** pointed node */
      node_type* current_;
      std::size_t rotation_cnt_;
      const ST * splay_;
  };

  /**
   * Splay trees with logarithmic reversing of any subsequence.
   *
   * All tree operations are amortized logarithmic time in size of tree,
   * each element is indexed by number of smaller elements than this element.
   * Note that lookups are also amortized logarithmic in size of tree. Order of
   * elements is induced from infix ordering of nodes storing these elements.
   **/
  template<typename T, enum SplayImplEnum SplayImpl = kTopDownUnbalanced>
  class SplayTree {
    public:
      typedef T value_type;
      typedef Node<value_type> node_type;
      typedef Iterator<value_type, SplayImpl, true> iterator;
      typedef const Iterator<value_type, SplayImpl, true> const_iterator;
      typedef Iterator<value_type, SplayImpl, false> reverse_iterator;
      typedef const Iterator<value_type, SplayImpl, false> const_reverse_iterator;

      SplayTree() = default;

      /**
       * @brief constructs tree from elements between two iterators
       * @param b iterator to first element
       * @param e iterator to element after last
       **/
      template<typename I> SplayTree(const I b, const I e) {
        root_ = build_tree(b, e);
      }
      
      SplayTree(SplayTree && splay)  {
          *this = std::move(splay);
      }

      SplayTree& operator=(SplayTree && splay) {
        rotation_cnt_ = splay.rotation_cnt_;
        root_         = splay.root_;
        tTonode_      = std::move(splay.tTonode_);
        splay.root_   = NULL;
        splay.rotation_cnt_ = 0;
        return *this;
      }

      SplayTree& operator=(SplayTree & splay) {
          SplayTree sp(splay);
          *this = std::move(sp);
          return *this;
      }
      
      SplayTree(const SplayTree & splay) : root_(copy_node(splay.root_)) {
          auto i = begin();
          auto e = end();
          for(;i != e; ++i) {
              tTonode_.insert(std::make_pair(*i, i.current_));
          }
      }

      /**
       * @brief creates tree from elements in std::vector
       * @param array vector container
       **/
      template<typename A> explicit SplayTree(const A &array) {
        root_ = build_tree(std::begin(array), std::end(array));
      }

      ~SplayTree() {
        dispose_tree(root_);
      }

      /** @returns forward iterator to first element in container */
      iterator begin() const {
        return (root_ == NULL) ? end() : iterator(root_->subtree_min(), this);
      }

      /** @returns forward iterator to element after last in container */
      iterator end() const {
        return iterator();
      }

      /** @returns reverse iterator to last element in container */
      reverse_iterator rbegin() {
        return (root_ == NULL) ? rend()
               : reverse_iterator(root_->subtree_max(), this);
      }

      /** @returns reverse iterator to element before first in container */
      reverse_iterator rend() {
        return reverse_iterator();
      }

      /** @returns number of elements in tree */
      std::size_t size() const {
        return (root_ == NULL) ? 0 : root_->size();
      }

      /** @returns true iff tree contains no elements */
      bool empty() {
        return (root_ == NULL);
      }

      /** @param i index of referenced element */
      value_type& operator[](std::size_t i) const {
        return find(i)->val_;
      }
      
      /** @param t referenced element */
      std::size_t getIdx(const T & t) const {
        node_type *node = tTonode_.at(t);
        if(node == NULL) {
            return -1;
        }
        node->normalize_root_path();
        
        std::size_t i = node_size(node->left());
        while(node != root_) {
            if(node->parent()->left() == node) {
                node = node->parent();
            } else {
                node = node->parent();
                i += node_size(node->left()) + 1;
            }
        }
        return i;
      }

      std::size_t getRotationCnt() const {
          return rotation_cnt_;
      }

      /**
       * @brief splays tree according to splay policy
       * @param i index of element to become root
       **/
      node_type *splay(std::size_t i) const {
        switch (SplayImpl) {
          case kTopDownUnbalanced:
          case kTopDown:
            root_ = splay_down(i);
            return root_;
          case kBottomUp:
            splay_internal(find(i));
            return root_;
        }
      }

      /**
       * @brief splits sequence, modified this contains elements {0, ..., i}
       * @param i index of last element of this after modification
       * @returns tree containing elements {i+1, ...}
       **/
      SplayTree<value_type, SplayImpl> split_higher(std::size_t i) {
        splay(i);
        node_type *new_root = root_->right();
        if (new_root != NULL) {
          new_root->make_root();
          root_->set_right(NULL);
        }
        return SplayTree<value_type, SplayImpl>(new_root);
      }

      /**
       * @brief splits sequence, modified this contains elements {i, ...}
       * @param i index of first element of this after modification
       * @returns tree containing elements {0, ..., i-1}
       **/
      SplayTree<value_type, SplayImpl> split_lower(std::size_t i) {
        splay(i);
        node_type *new_root = root_->left();
        if (new_root != NULL) {
          new_root->make_root();
          root_->set_left(NULL);
        }
        return SplayTree<value_type, SplayImpl>(new_root);
      }

      /**
       * @brief merges given tree to the right of the biggest element of this
       * @param other tree to be merged
       **/
      template<enum SplayImplEnum S>
      void merge_right(SplayTree<value_type, S> &other) {
        if (other.root_ == NULL) {
          return;
        }
        splay(root_->size() - 1);
        assert(root_->right() == NULL);
        root_->set_right(other.root_);
        other.root_ = NULL;
      }

      /**
       * @brief merges given tree to the left of the smallest element of this
       * @param other tree to be merged
       **/
      template<enum SplayImplEnum S>
      void merge_left(SplayTree<value_type, S> &other) {
        if (other.root_ == NULL) {
          return;
        }
        splay(0);
        assert(root_->left() == NULL);
        root_->set_left(other.root_);
        other.root_ = NULL;
      }

      /**
       * @brief reverses subsequence of elements with indices in {i, ..., j}
       * @param i index of first element of subsequence
       * @param j index of last element of subsequence
       **/
      void reverse(std::size_t i, std::size_t j) {
        assert(i <= j);
        // split lower
        SplayTree<value_type, SplayImpl> ltree = split_lower(i);
        // split higher
        SplayTree<value_type, SplayImpl> rtree = split_higher(j - i);
        // reverse
        root_->subtree_reverse();
        // merge
        merge_left(ltree);
        merge_right(rtree);
      }

    private:
      /** @brief creates tree with given node as a root */
      explicit SplayTree(node_type *root) : root_(root) {}

      /**
       * @brief splays given node to tree root
       * @param node node of a tree to be moved to root
       **/
      void splay_internal(node_type *const node) const {
        node_type *const parent = node->parent();
        if (node == root_) {
          return;
        } else if (parent == root_) {
          if (node == parent->left()) {
            rotate_right(parent);
          } else {
            rotate_left(parent);
          }
        } else {
          node_type *const grand = parent->parent();
          if (node == parent->left() && parent == grand->left()) {
            rotate_right(grand);
            rotate_right(parent);
          } else if (node == parent->right() && parent == grand->right()) {
            rotate_left(grand);
            rotate_left(parent);
          } else if (node == parent->right() && parent == grand->left()) {
            rotate_left(parent);
            rotate_right(grand);
          } else if (node == parent->left() && parent == grand->right()) {
            rotate_right(parent);
            rotate_left(grand);
          }
        }
        splay_internal(node);
      }

      /**
       * @brief splays node with given index to tree root
       * @param i index of a node to become root
       **/
      node_type *splay_down(std::size_t i) const {
        node_type *parent = root_;
        node_type second_tree(T{});
        node_type *l = &second_tree, *r = &second_tree;

        for (;;) {
          std::size_t left_size = node_size(parent->left());
          if (left_size == i) {
            break;
          } else if (left_size > i) {
            if (SplayImpl != kTopDownUnbalanced) {
              if (node_size(parent->left()->left()) > i) {
                node_type *node = parent->left();
                parent->set_left(node->right());
                node->set_right_internal(parent);
                node->make_root();
                parent = node;
              }
            }
            node_type *node = parent->left();
            node->make_root();
            r->set_left_internal(parent);
            r = parent;

            parent = node;
          } else {
            if (SplayImpl != kTopDownUnbalanced) {
              if (left_size + 1 + node_size(parent->right()->left()) < i) {
                node_type *node = parent->right();
                parent->set_right(node->left());
                node->set_left_internal(parent);
                node->make_root();
                left_size = parent->size();
                parent = node;
              }
            }
            node_type *node = parent->right();
            node->make_root();
            l->set_right_internal(parent);
            l = parent;

            parent = node;
            i -= left_size + 1;
          }
        }

        r->set_left_internal(parent->right());
        l->set_right_internal(parent->left());

        parent->set_left_internal(second_tree.right());
        parent->set_right_internal(second_tree.left());

        parent->make_root();

        while (r != NULL) {
          r->update_size();
          r = r->parent();
        }
        while (l != NULL) {
          l->update_size();
          l = l->parent();
        }

        return parent;
      }

      /**
       * @brief rotates tree right over given node
       * @param parent pivot of rotation
       **/
      void rotate_right(node_type *parent) const {
        node_type *const node = parent->left(),
                         *const grand = parent->parent();
        parent->set_left(node->right());
        if (grand != NULL) {
          if (parent == grand->right()) {
            grand->set_right(node);
          } else {
            grand->set_left(node);
          }
        }
        node->set_right(parent);
        if (parent == root_) {
          root_ = node;
          node->make_root();
        }
      }

      /**
       * @brief rotates tree left over given node
       * @param parent pivot of rotation
       **/
      void rotate_left(node_type *parent) const {
        node_type *const node = parent->right(),
                         *const grand = parent->parent();
        parent->set_right(node->left());
        if (grand != NULL) {
          if (parent == grand->left()) {
            grand->set_left(node);
          } else {
            grand->set_right(node);
          }
        }
        node->set_left(parent);
        if (parent == root_) {
          root_ = node;
          node->make_root();
        }
      }

      /**
       * @brief recursively creates balanced tree from a structure described
       *        by two random access iterators
       * @param b iterator to first element
       * @param e iterator to element after last
       **/
      template<typename I> node_type *build_tree(const I b, const I e) {
        if (b >= e) {
          return NULL;
        }
        std::size_t m = (e - b) / 2;
        node_type *node = new node_type(*(b + m));
        bool ret = tTonode_.insert(std::make_pair(*(b + m), node)).second;
        assert(ret);
        node->set_left(build_tree(b, b + m));
        node->set_right(build_tree(b + m + 1, e));
        return node;
      }

      /**
       * @brief recursively removes subtree
       * @param node pointer to subtree to be removed
       **/
      void dispose_tree(node_type *node) {
        if (node == NULL) {
          return;
        }
        dispose_tree(node->left());
        dispose_tree(node->right());
        delete node;
      }

      /**
       * @brief find n-th element in tree (counting from zero)
       * @param i number of elements smaller than element to be returned
       * @returns pointer to found node or NULL if doesn't exist
       **/
      node_type *find(std::size_t i) const {
        node_type *node = root_;
        for (;;) {
          if (node == NULL) {
            return NULL;
          }
          node_type *left = node->left();
          std::size_t left_size = (left == NULL) ? 0 : left->size();
          if (left_size == i) {
            return node;
          } else if (left_size > i) {
            node = left;
          } else {
            i -= left_size + 1;
            node = node->right();
          }
        }
      }

      /**
       * @brief SplayTree stream output operator
       * @param stream output stream
       * @param tree splay tree
       **/
/*      template<typename S, typename V, enum SplayImplEnum I>
      friend S& operator<<(S &stream, SplayTree<V, I> &tree) {
        tree.root_->print_tree(stream);
        return stream;
      }*/

      /** root node of a tree */
      std::size_t rotation_cnt_ = 0; // to keep iterators consistent with tree
      mutable node_type *root_ = NULL;
      std::unordered_map<T, node_type *> tTonode_;
  };
}
}
}

#endif  // SPLAY_TREE_HPP
