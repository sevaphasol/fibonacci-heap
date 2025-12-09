#pragma once

#include <climits>
#include <cmath>
#include <stdexcept>
#include <vector>

namespace fhp {

class FHeap {
  private:
    struct Node
    {
        int   key;
        Node* parent;
        Node* child;
        Node* left;
        Node* right;
        int   degree;
        bool  mark;

        Node( int k )
            : key( k ),
              parent( nullptr ),
              child( nullptr ),
              left( this ),
              right( this ),
              degree( 0 ),
              mark( false )
        {
        }
    };

    Node*  min_;
    size_t size_;

  public:
    class Handle {
        friend class FHeap;
        Node* ptr_;
        explicit Handle( Node* p ) : ptr_( p ) {}

      public:
        Handle() : ptr_( nullptr ) {}
    };

    FHeap() : min_( nullptr ), size_( 0 ) {}

    Handle
    Insert( int key )
    {
        Node* node = new Node( key );
        if ( min_ == nullptr )
        {
            min_ = node;
        } else
        {
            mergeRootLists( min_, node );
            if ( node->key < min_->key )
            {
                min_ = node;
            }
        }
        ++size_;
        return Handle( node );
    }

    int
    GetMin() const
    {
        if ( min_ == nullptr )
        {
            throw std::runtime_error( "Heap is empty" );
        }
        return min_->key;
    }

    void
    Merge( FHeap& other )
    {
        if ( other.min_ == nullptr )
        {
            return;
        }
        if ( min_ == nullptr )
        {
            min_  = other.min_;
            size_ = other.size_;
        } else
        {
            mergeRootLists( min_, other.min_ );
            if ( other.min_->key < min_->key )
            {
                min_ = other.min_;
            }
            size_ += other.size_;
        }
        other.min_  = nullptr;
        other.size_ = 0;
    }

    int
    ExtractMin()
    {
        if ( min_ == nullptr )
        {
            throw std::runtime_error( "Heap is empty" );
        }

        Node* z   = min_;
        int   key = z->key;

        if ( z->child != nullptr )
        {
            Node* x     = z->child;
            Node* start = x;
            do
            {
                x->parent = nullptr;
                x         = x->right;
            } while ( x != start );
            mergeRootLists( min_, z->child );
        }

        z->left->right = z->right;
        z->right->left = z->left;

        if ( z == z->right )
        {
            min_ = nullptr;
        } else
        {
            min_ = z->right;
            consolidate();
        }

        delete z;
        --size_;
        return key;
    }

    void
    DecreaseKey( const Handle& h, int newKey )
    {
        /* clang-format off

		   assigning instead of substracting, because boost::heap::fibonacci_heap does that
		   see https://www.boost.org/doc/libs/master/doc/html/doxygen/classboost_1_1heap_1_1fibonacci__heap.html

		   clang-format on
        */

        if ( h.ptr_ == nullptr )
        {
            throw std::invalid_argument( "Invalid handle" );
        }
        Node* x = h.ptr_;
        if ( newKey > x->key )
        {
            throw std::invalid_argument( "New key is greater than current key" );
        }
        x->key  = newKey;
        Node* y = x->parent;
        if ( y != nullptr && x->key < y->key )
        {
            cut( x, y );
            cascadingCut( y );
        }
        if ( min_ == nullptr || x->key < min_->key )
        {
            min_ = x;
        }
    }

    void
    Erase( const Handle& h )
    {
        DecreaseKey( h, INT_MIN );
        ExtractMin();
    }

    size_t
    GetSize() const
    {
        return size_;
    }

  private:
    static void
    mergeRootLists( Node* a, Node* b )
    {
        if ( a == nullptr || b == nullptr )
        {
            return;
        }
        Node* a_left = a->left;
        Node* b_left = b->left;

        a->left       = b_left;
        b_left->right = a;

        b->left       = a_left;
        a_left->right = b;
    }

    void
    consolidate()
    {
        if ( min_ == nullptr )
        {
            return;
        }

        size_t max_degree =
            static_cast<size_t>( std::floor( std::log2( static_cast<double>( size_ ) ) + 1 ) );
        std::vector<Node*> tree_ptrs( max_degree + 1, nullptr );

        std::vector<Node*> roots;
        Node*              start = min_;
        Node*              curr  = start;
        do
        {
            roots.push_back( curr );
            curr = curr->right;
        } while ( curr != start );

        for ( Node* x : roots )
        {
            x->left  = x;
            x->right = x;
        }

        for ( Node* x : roots )
        {
            int d = x->degree;
            while ( tree_ptrs[d] != nullptr )
            {
                Node* y = tree_ptrs[d];
                if ( x->key > y->key )
                {
                    std::swap( x, y );
                }
                y->parent = x;
                y->mark   = false;
                if ( x->child == nullptr )
                {
                    x->child = y;
                    y->left  = y;
                    y->right = y;
                } else
                {
                    mergeRootLists( x->child, y );
                }
                x->degree++;
                tree_ptrs[d] = nullptr;
                ++d;
                if ( d >= static_cast<int>( tree_ptrs.size() ) )
                {
                    tree_ptrs.resize( d + 1, nullptr );
                }
            }
            tree_ptrs[d] = x;
        }

        min_ = nullptr;
        for ( Node* node : tree_ptrs )
        {
            if ( node != nullptr )
            {
                node->left   = node;
                node->right  = node;
                node->parent = nullptr;
                node->mark   = false;
                if ( min_ == nullptr )
                {
                    min_ = node;
                } else
                {
                    mergeRootLists( min_, node );
                    if ( node->key < min_->key )
                    {
                        min_ = node;
                    }
                }
            }
        }
    }

    void
    cut( Node* x, Node* y )
    {
        if ( x->right == x )
        {
            y->child = nullptr;
        } else
        {
            if ( y->child == x )
            {
                y->child = x->right;
            }
            x->left->right = x->right;
            x->right->left = x->left;
        }
        y->degree--;

        x->parent = nullptr;
        x->mark   = false;
        x->left   = x;
        x->right  = x;

        if ( min_ == nullptr )
        {
            min_ = x;
        } else
        {
            mergeRootLists( min_, x );
            if ( x->key < min_->key )
            {
                min_ = x;
            }
        }
    }

    void
    cascadingCut( Node* y )
    {
        Node* z = y->parent;
        if ( z != nullptr )
        {
            if ( !y->mark )
            {
                y->mark = true;
            } else
            {
                cut( y, z );
                cascadingCut( z );
            }
        }
    }
};

} // namespace fhp
