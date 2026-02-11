//
// Created by nickberryman on 1/12/25.
//

module;
#include <type_traits>
#include "Logger.h"

export module SG_Allocator:LinkedList2;
import :BaseArena;
import :PseudoArena;
import SG_AllocatorConfigs;
import Logger;

// TODO examine alignment
template<typename T, bool forwardLinks, bool reverseLinks> class baseNodeLL2 {}; //Implementation at end of file
export namespace SG_Allocator {
    /**
     * @brief Homogeneous doubly-linked list, with arena allocation support
     *
     * @tparam InsideArenaType Arena type to allocate blocks into (Use a PseudoArena if heap allocation preferred)
     * @tparam T Data type to use for elements
     */
    template<typename InsideArenaType, typename T, typename index_t, bool forwardLinks = true, bool reverseLinks = true, bool recycleNodes = true>
    requires std::is_base_of_v<BaseArena, InsideArenaType> && (forwardLinks || reverseLinks)
    class LinkedList2 : private baseNodeLL2<T, forwardLinks, reverseLinks>{
    public:
        explicit LinkedList2(InsideArenaType& arena_) : factory(arena_) {};
        inline ~LinkedList2() requires (!std::is_base_of_v<PseudoArena, InsideArenaType>) = default;
        inline ~LinkedList2() requires std::is_base_of_v<PseudoArena, InsideArenaType> { clear(); };

        void* node_fromFront(const index_t& index) requires forwardLinks;
        static void* node_after(void* const& node) requires forwardLinks {return static_cast<Node*>(node)->next;}
        template<typename... ConstructorArgs> void* construct_front(ConstructorArgs&& ... args) requires forwardLinks;
        template<typename... ConstructorArgs> void* construct_afterNode(void* const& parent, ConstructorArgs&& ... args) requires forwardLinks;
        template<typename... ConstructorArgs> void* construct_fromFront(const index_t& index, ConstructorArgs&& ... args) requires forwardLinks;
        void remove_front() requires forwardLinks;
        void remove_nodeAfter(void* const& node) requires forwardLinks;
        void remove_fromFront(const int& index) requires forwardLinks;
        T& get_fromFront(const index_t& index) requires forwardLinks;

        void* node_fromBack(const index_t& index) requires reverseLinks;
        static void* node_before(void* const& node) requires reverseLinks {return static_cast<Node*>(node)->previous;}
        template<typename... ConstructorArgs> void* construct_back(ConstructorArgs&& ... args) requires reverseLinks;
        template<typename... ConstructorArgs> void* construct_beforeNode(void* const& parent, ConstructorArgs&& ... args) requires reverseLinks;
        template<typename... ConstructorArgs> void* construct_fromBack(const index_t& index, ConstructorArgs&& ... args) requires reverseLinks;
        void remove_back() requires reverseLinks;
        void remove_nodeBefore(void* const& node) requires reverseLinks;
        void remove_fromBack(const int& index) requires reverseLinks;
        T& get_fromBack(const index_t& index) requires reverseLinks;


        void remove_node(void* const& node) requires (reverseLinks && forwardLinks);
        static inline T& get_atNode(void* const& node) { return static_cast<Node*>(node)->value; }
        void clear();
        [[nodiscard]] inline const index_t& length() const {return _length;};

    private:
        typedef baseNodeLL2<T, forwardLinks, reverseLinks> base;
        typedef base::Node Node;

        class NodeFactory {
        public:
            explicit NodeFactory(InsideArenaType& arena) requires recycleNodes: impl(arena), a(arena) {};
            explicit NodeFactory(InsideArenaType& arena) requires (!recycleNodes): a(arena) {};
            void deleteNode(Node* const& toDelete);
            void forceDeleteNode(Node* const& toDelete);
            template<typename... ConstructorArgs> Node* provideNode(Node* const& a, Node* const& b, ConstructorArgs&& ... args);
            void clear(){if constexpr (recycleNodes) impl.clear();}
        private:
            typedef LinkedList2<InsideArenaType, Node*, index_t ,true, false, false> impl_t;
            struct empty_ {};

            std::conditional_t<recycleNodes, impl_t, empty_> impl;
            InsideArenaType& a;

            template<typename... ConstructorArgs> Node* constructNode(Node* const& previous_, Node* const& next_, ConstructorArgs&& ... args) requires (forwardLinks && reverseLinks) { return a.template allocConstruct<Node>(previous_, next_, args...); };
            template<typename... ConstructorArgs> Node* constructNode(Node* const& parent_, ConstructorArgs&& ... args) requires (forwardLinks || reverseLinks && !(forwardLinks && reverseLinks)) { return a.template allocConstruct<Node>(parent_, args...); };
        } factory;
        index_t _length = 0;
    };




    /** Implementation **/\
    #define SG_LL_removeFrontBack(from, direction, d2, checkDirection, clr) \
        if constexpr (clr) if (_length == 0) {return;}\
        LOGGER_ASSERT_EXCEPT(_length > 0) \
        Node* delNode = from.direction; \
        from.direction = delNode->direction; \
        if constexpr (checkDirection && !clr) from.direction->d2 = &(from); \
        if constexpr (!clr) factory.deleteNode(delNode); \
        else factory.forceDeleteNode(delNode); \
        --_length;

    /**
     * Remove last node from list. Calls default destructor on its value - manually destruct first using get_atNode(node_fromBack(0)) if you want non-default destruction
     */
    template<typename InsideArenaType, typename T, typename index_t, bool forwardLinks, bool reverseLinks, bool recycleNodes> requires std::is_base_of_v<BaseArena, InsideArenaType> && (forwardLinks || reverseLinks)
    void LinkedList2<InsideArenaType, T, index_t, forwardLinks, reverseLinks, recycleNodes>::remove_back() requires reverseLinks{ SG_LL_removeFrontBack((base::tail), previous, next, forwardLinks, false) }

    /**
     * Remove first node from list. Calls default destructor on its value - manually destruct first using get_atNode(node_fromFront(0)) if you want non-default destruction
     */
    template<typename InsideArenaType, typename T, typename index_t, bool forwardLinks, bool reverseLinks, bool recycleNodes> requires std::is_base_of_v<BaseArena, InsideArenaType> && (forwardLinks || reverseLinks)
    void LinkedList2<InsideArenaType, T, index_t, forwardLinks, reverseLinks, recycleNodes>::remove_front() requires forwardLinks{ SG_LL_removeFrontBack((base::root), next, previous, reverseLinks, false) }


    #define SG_LL_pushFrontBack(from, direction, before, after) \
        from.direction = factory.provideNode(before, after, args...); \
        ++_length; \
        return from.direction;

    /**
     * Create a new node at the back of the list, and construct its value in-place (Use a copy constructor if you want it to copy)
     * @param args (Optional) list of arguments to construct value within new node
     * @return Address of new node
     */
    template<typename InsideArenaType, typename T, typename index_t, bool forwardLinks, bool reverseLinks, bool recycleNodes> requires std::is_base_of_v<BaseArena, InsideArenaType> && (forwardLinks || reverseLinks) template<typename ... ConstructorArgs>
    void * LinkedList2<InsideArenaType, T, index_t, forwardLinks, reverseLinks, recycleNodes>::construct_back(ConstructorArgs&& ... args) requires reverseLinks {
        //SG_LL_pushFrontBack(base::tail, previous, base::tail.previous, &(base::tail))
        return construct_beforeNode(static_cast<void *>(&(base::tail)), args...);
    }

    /**
     * Create a new node at the front of the list, and construct its value in-place (Use a copy constructor if you want it to copy)
     * @param args (Optional) list of arguments to construct value within new node
     * @return Address of new node
     */
    template<typename InsideArenaType, typename T, typename index_t, bool forwardLinks, bool reverseLinks, bool recycleNodes> requires std::is_base_of_v<BaseArena, InsideArenaType> && (forwardLinks || reverseLinks) template<typename ... ConstructorArgs>
    void * LinkedList2<InsideArenaType, T, index_t, forwardLinks, reverseLinks, recycleNodes>::construct_front(ConstructorArgs&& ... args) requires forwardLinks {
        //SG_LL_pushFrontBack(base::root, next, &(base::root), base::root.next)
        return construct_afterNode(static_cast<void *>(&(base::root)), args...);
    }



    #define SG_LL_getFromFrontBack(from, direction) \
        LOGGER_ASSERT_EXCEPT(index < _length); \
        LOGGER_ASSERT_EXCEPT(index >= 0); \
        Node* n = from.direction; \
        for (index_t _ = 0; _ < index; _++) n = n->direction; \
        return get_atNode(n);

    /**
     *
     * @param index Index of node to get the value of (from front)
     * @return Mutable reference to node value
     */
    template<typename InsideArenaType, typename T, typename index_t, bool forwardLinks, bool reverseLinks, bool recycleNodes> requires std::is_base_of_v<BaseArena, InsideArenaType> && (forwardLinks || reverseLinks)
    T & LinkedList2<InsideArenaType, T, index_t, forwardLinks, reverseLinks, recycleNodes>::get_fromFront(const index_t &index) requires forwardLinks { SG_LL_getFromFrontBack((base::root), next); }

    /**
     *
     * @param index Index of node to get the value of (from back)
     * @return Mutable reference to node value
     */
    template<typename InsideArenaType, typename T, typename index_t, bool forwardLinks, bool reverseLinks, bool recycleNodes> requires std::is_base_of_v<BaseArena, InsideArenaType> && (forwardLinks || reverseLinks)
    T & LinkedList2<InsideArenaType, T, index_t, forwardLinks, reverseLinks, recycleNodes>::get_fromBack(const index_t &index) requires reverseLinks { SG_LL_getFromFrontBack(base::tail, previous); }


    template<typename InsideArenaType, typename T, typename index_t, bool forwardLinks, bool reverseLinks, bool recycleNodes> requires std::is_base_of_v<BaseArena, InsideArenaType> && (forwardLinks || reverseLinks)
    void LinkedList2<InsideArenaType, T, index_t, forwardLinks, reverseLinks, recycleNodes>::NodeFactory::deleteNode(Node* const& toDelete) {
        if constexpr (recycleNodes) impl.construct_front(toDelete);
        else a.softDelete(toDelete);
    }

    template<typename InsideArenaType, typename T, typename index_t, bool forwardLinks, bool reverseLinks, bool recycleNodes> requires std::is_base_of_v<BaseArena, InsideArenaType> && (forwardLinks || reverseLinks)
    void LinkedList2<InsideArenaType, T, index_t, forwardLinks, reverseLinks, recycleNodes>::NodeFactory::forceDeleteNode(Node* const& toDelete) { a.softDelete(toDelete); }

    template<typename InsideArenaType, typename T, typename index_t, bool forwardLinks, bool reverseLinks, bool recycleNodes> requires std::is_base_of_v<BaseArena, InsideArenaType> && (forwardLinks || reverseLinks)template<typename ... ConstructorArgs> typename LinkedList2<InsideArenaType, T, index_t, forwardLinks, reverseLinks, recycleNodes>::
    Node * LinkedList2<InsideArenaType, T, index_t, forwardLinks, reverseLinks, recycleNodes>::NodeFactory::provideNode(Node * const &a, Node * const &b,ConstructorArgs&& ... args) {
        if constexpr (recycleNodes) {
            if (impl.length() > 0) {
                Node* out = impl.get_fromFront(0);
                if constexpr (reverseLinks && forwardLinks) new (out) Node(a, b, args...);
                else if constexpr (reverseLinks || forwardLinks) new (out) Node(a, args...);
                impl.remove_front();
                return out;
            }
        }
        if constexpr (reverseLinks && forwardLinks) return constructNode(a,b,args...);
        else if constexpr (reverseLinks) return constructNode(a,args...);
        else if constexpr (forwardLinks) return constructNode(b,args...);
        //return nullptr; //Should never actually get to this
    }


    #define SG_LL_nodeAt(from, direction) \
        LOGGER_ASSERT_EXCEPT(index < _length) \
        LOGGER_ASSERT_EXCEPT(index >= 0) \
        Node* n = from.direction; \
        for (index_t _ = 0; _ < index; _++) n = n->direction; \
        return n;

    /**
     *
     * @param index Index from front of list
     * @return Address of node at that index
     */
    template<typename InsideArenaType, typename T, typename index_t, bool forwardLinks, bool reverseLinks, bool recycleNodes> requires std::is_base_of_v<BaseArena, InsideArenaType> && (forwardLinks || reverseLinks)
    void * LinkedList2<InsideArenaType, T, index_t, forwardLinks, reverseLinks, recycleNodes>::node_fromFront(const index_t &index) requires forwardLinks{ SG_LL_nodeAt(base::root, next)}

    /**
     *
     * @param index Index from front of list
     * @return Address of node at that index
     */
    template<typename InsideArenaType, typename T, typename index_t, bool forwardLinks, bool reverseLinks, bool recycleNodes> requires std::is_base_of_v<BaseArena, InsideArenaType> && (forwardLinks || reverseLinks)
    void * LinkedList2<InsideArenaType, T, index_t, forwardLinks, reverseLinks, recycleNodes>::node_fromBack(const index_t &index) requires reverseLinks { SG_LL_nodeAt(base::tail, previous) }


    #define SG_LL_pushBeforeAfter(d1, d2, d3, d4, checkDirection) \
        Node* child = static_cast<Node *>(parent)->d1; \
        if constexpr (forwardLinks && reverseLinks) static_cast<Node *>(parent)->d1 = factory.provideNode(d2, d3, args...); \
        else if constexpr (forwardLinks) static_cast<Node *>(parent)->d1 = factory.provideNode(nullptr,d3, args...); \
        else if constexpr (reverseLinks) static_cast<Node *>(parent)->d1 = factory.provideNode(d3, nullptr, args...); \
        if constexpr (checkDirection) if (child != nullptr) child->d4 = static_cast<Node *>(parent)->d1; \
        _length++; \
        return static_cast<Node *>(parent)->d1;

    /**
     * Create a new node behind the provided node, and construct its value in-place (Use a copy constructor if you want it to copy)
     * @param parent Address of node to construct the new node behind
     * @param args (Optional) list of arguments to construct value within new node
     * @return Address of new node
     */
    template<typename InsideArenaType, typename T, typename index_t, bool forwardLinks, bool reverseLinks, bool recycleNodes> requires std::is_base_of_v<BaseArena, InsideArenaType> && (forwardLinks || reverseLinks) template<typename ... ConstructorArgs>
    void * LinkedList2<InsideArenaType, T, index_t, forwardLinks, reverseLinks, recycleNodes>::construct_afterNode(void* const& parent, ConstructorArgs&& ... args) requires forwardLinks{
        SG_LL_pushBeforeAfter(next, static_cast<Node *>(parent), child, previous, reverseLinks)
    }

    /**
     * Create a new node in front of the provided node, and construct its value in-place (Use a copy constructor if you want it to copy)
     * @param parent Address of node to construct the new node in front of
     * @param args (Optional) list of arguments to construct value within new node
     * @return Address of new node
     */
    template<typename InsideArenaType, typename T, typename index_t, bool forwardLinks, bool reverseLinks, bool recycleNodes> requires std::is_base_of_v<BaseArena, InsideArenaType> && (forwardLinks || reverseLinks) template<typename ... ConstructorArgs>
    void * LinkedList2<InsideArenaType, T, index_t, forwardLinks, reverseLinks, recycleNodes>::construct_beforeNode(void* const& parent, ConstructorArgs&& ... args) requires reverseLinks{
        SG_LL_pushBeforeAfter(previous, child, static_cast<Node *>(parent), next, forwardLinks)
    }

    /**
     * Create a new node at the provided index, and construct its value in-place (Use a copy constructor if you want it to copy)
     * @param index Index that new node should occupy (from front of list)
     * @param args (Optional) list of arguments to construct value within new node
     * @return Address of new node
     */
    template<typename InsideArenaType, typename T, typename index_t, bool forwardLinks, bool reverseLinks, bool recycleNodes> requires std::is_base_of_v<BaseArena, InsideArenaType> && (forwardLinks || reverseLinks) template<typename ... ConstructorArgs>
    void * LinkedList2<InsideArenaType, T, index_t, forwardLinks, reverseLinks, recycleNodes>::construct_fromFront(const index_t &index, ConstructorArgs&& ... args) requires forwardLinks{
        if constexpr (reverseLinks) { LOGGER_ASSERT_EXCEPT(index < _length); }
        LOGGER_ASSERT_EXCEPT(index > 0);
        return construct_afterNode(node_fromFront(index-1), args...);
    }

    /**
     * Create a new node at the provided index, and construct its value in-place (Use a copy constructor if you want it to copy)
     * @param index Index that new node should occupy (from end of list)
     * @param args (Optional) list of arguments to construct value within new node
     * @return Address of new node
     */
    template<typename InsideArenaType, typename T, typename index_t, bool forwardLinks, bool reverseLinks, bool recycleNodes> requires std::is_base_of_v<BaseArena, InsideArenaType> && (forwardLinks || reverseLinks) template<typename ... ConstructorArgs>
    void * LinkedList2<InsideArenaType, T, index_t, forwardLinks, reverseLinks, recycleNodes>::construct_fromBack(const index_t &index, ConstructorArgs&& ... args) requires reverseLinks{
        if constexpr (forwardLinks) { LOGGER_ASSERT_EXCEPT(index < _length); }
        LOGGER_ASSERT_EXCEPT(index > 0);
        return construct_beforeNode(node_fromBack(index-1), args...);
    }

    /**
     * Remove specified node from list. Calls default destructor on its value - manually destruct first using get_atNode() and get if you want non-default destruction
     * @param node Address of node to remove. Treat as invalid after this call
     */
    template<typename InsideArenaType, typename T, typename index_t, bool forwardLinks, bool reverseLinks, bool recycleNodes> requires std::is_base_of_v<BaseArena, InsideArenaType> && (forwardLinks || reverseLinks)
    void LinkedList2<InsideArenaType, T, index_t, forwardLinks, reverseLinks, recycleNodes>::remove_node(void * const &node) requires (reverseLinks && forwardLinks) {
        static_cast<Node *>(node)->previous->next = static_cast<Node *>(node)->next; \
        static_cast<Node *>(node)->next->previous = static_cast<Node *>(node)->previous; \
        factory.deleteNode(static_cast<Node *>(node)); \
        --_length;
    }


    #define SG_LL_removeBeforeAfter(d1) \
        LOGGER_ASSERT_EXCEPT(static_cast<Node *>(node)->d1 != nullptr) \
        Node* toDel = static_cast<Node *>(node)->d1; \
        static_cast<Node *>(node)->d1 = static_cast<Node *>(node)->d1->d1; \
        factory.deleteNode(static_cast<Node *>(toDel)); \
        _length--;

    /**
     * Remove node following the specified node. Undefined behaviour if called on last node. Calls default destructor on its value - manually destruct first using get_atNode() and get if you want non-default destruction
     * @param node Address of base node. Removes the following node
     */
    template<typename InsideArenaType, typename T, typename index_t, bool forwardLinks, bool reverseLinks, bool recycleNodes> requires std::is_base_of_v<BaseArena, InsideArenaType> && (forwardLinks || reverseLinks)
    void LinkedList2<InsideArenaType, T, index_t, forwardLinks, reverseLinks, recycleNodes>::remove_nodeAfter(void * const &node) requires forwardLinks { SG_LL_removeBeforeAfter(next); }

    /**
     * Remove node in front of the specified node. Undefined behaviour if called on first node. Calls default destructor on its value - manually destruct first using get_atNode() and get if you want non-default destruction
     * @param node Address of base node. Removes the node in front
     */
    template<typename InsideArenaType, typename T, typename index_t, bool forwardLinks, bool reverseLinks, bool recycleNodes> requires std::is_base_of_v<BaseArena, InsideArenaType> && (forwardLinks || reverseLinks)
    void LinkedList2<InsideArenaType, T, index_t, forwardLinks, reverseLinks, recycleNodes>::remove_nodeBefore(void * const &node) requires reverseLinks { SG_LL_removeBeforeAfter(previous); }


    #define SG_LL_removeFromFrontFromBack(fun) \
        if constexpr (forwardLinks && reverseLinks) remove_node(fun(index)); \
        else if constexpr (forwardLinks) remove_nodeAfter(fun(index-1)); \
        else remove_nodeBefore(fun(index+1));

    /**
     * Remove specified node from list. Calls default destructor on its value - manually destruct first using get_atNode() and get if you want non-default destruction
     * @param index Index of node to remove.
     */
    template<typename InsideArenaType, typename T, typename index_t, bool forwardLinks, bool reverseLinks, bool recycleNodes> requires std::is_base_of_v<BaseArena, InsideArenaType> && (forwardLinks || reverseLinks)
    void LinkedList2<InsideArenaType, T, index_t, forwardLinks, reverseLinks, recycleNodes>::remove_fromFront(const int &index) requires forwardLinks { SG_LL_removeFromFrontFromBack(node_fromFront) }

    /**
     * Remove specified node from list. Calls default destructor on its value - manually destruct first using get_atNode() and get if you want non-default destruction
     * @param index Index of node to remove.
     */
    template<typename InsideArenaType, typename T, typename index_t, bool forwardLinks, bool reverseLinks, bool recycleNodes> requires std::is_base_of_v<BaseArena, InsideArenaType> && (forwardLinks || reverseLinks)
    void LinkedList2<InsideArenaType, T, index_t, forwardLinks, reverseLinks, recycleNodes>::remove_fromBack(const int &index) requires reverseLinks{ SG_LL_removeFromFrontFromBack(node_fromBack) }


    /**
     * Sets list to empty. Destructs allocated memory for PseudoArenas, but does not for other Arenas. For other Arenas, should generally only be called before clearing the arena or rolling back the sublifetime.
     */
    template<typename InsideArenaType, typename T, typename index_t, bool forwardLinks, bool reverseLinks, bool recycleNodes> requires std::is_base_of_v<BaseArena, InsideArenaType> && (forwardLinks || reverseLinks)
    void LinkedList2<InsideArenaType, T, index_t, forwardLinks, reverseLinks, recycleNodes>::clear() {
        if constexpr (!std::is_base_of_v<PseudoArena, InsideArenaType>){
            if constexpr (reverseLinks) while (_length > 0) {SG_LL_removeFrontBack(base::tail, previous, next, forwardLinks, true);}
            else if constexpr (forwardLinks) while (_length > 0) { SG_LL_removeFrontBack(base::root, next, previous, reverseLinks, true); }
        } else _length = 0;
        factory.clear();
    }

}




template<typename T> class baseNodeLL2<T, true, true> {
public:
    baseNodeLL2() {
        root.next = &(tail);
        tail.previous = &(root);
    }
    struct Node{
        template<typename... ConstructorArgs> Node(Node* const& previous_, Node* const& next_, ConstructorArgs&& ... args):previous(previous_),next(next_),value(args...){}
        Node():previous(nullptr),next(nullptr),empty(0){}
        Node* previous;
        Node* next;
        union{char empty; T value;};
    };
    Node root;
    Node tail;
};
template<typename T> class baseNodeLL2<T, true, false> {
public:
    struct Node{
        template<typename... ConstructorArgs> explicit Node(Node* const& next_, ConstructorArgs&& ... args):next(next_),value(args...){}
        Node():next(nullptr),empty(0){}
        Node* next;
        union{char empty; T value;};
    };
    Node root;
};
template<typename T> class baseNodeLL2<T, false, true> {
public:
    struct Node{
        template<typename... ConstructorArgs> explicit Node(Node* const& previous_, ConstructorArgs&& ... args):previous(previous_),value(args...){}
        Node():previous(nullptr),empty(0){}
        Node* previous;
        union{char empty; T value;};
    };
    Node tail;
};

