//
// Created by nickberryman on 1/12/25.
//

module;
#include <type_traits>
#include "Logger.h"

export module SG_Allocator:LinkedList;
import :BaseArena;
import SG_AllocatorConfigs;
import Logger;


//TODO 
    // See Pathfinding.txt -> summary below
    // clean this file and reduce duplicated code
    // make a 'NodeFactory' inner class that will create & delete nodes -> give it a 'recycle' flag that would allow it to reuse previously constructed nodes to reduce memory fragmentation
    // make templatable for forward, backward, or double-linkage

export namespace SG_Allocator {
    /**
     * @brief Homogeneous  doubly-linked list, with arena allocation support
     *
     * @tparam InsideArenaType Arena type to allocate blocks into (Use a PseudoArena if heap allocation preferred)
     * @tparam T Data type to use for elements
     */
    template<typename InsideArenaType, typename T = char>
    requires std::is_base_of_v<BaseArena, InsideArenaType>
    class LinkedList {
    public:
        LinkedList(InsideArenaType& arena): arena(arena) {};
        inline ~LinkedList() { clear(); };

        template<typename... ConstructorArgs> void* construct_back(ConstructorArgs... args);
        void* push_back(const T& value);
        template<typename... ConstructorArgs> void* construct_front(ConstructorArgs... args);
        void* push_front(const T& value);;
        template<typename... ConstructorArgs> void construct_at(void* parent, ConstructorArgs... args);
        void push_at(const T& value, void* parent);
        template<typename... ConstructorArgs> void construct_at(const arenaSize_t& index, ConstructorArgs... args);
        void push_at(const arenaSize_t& index, const T& value);

        void remove_back();
        void remove_front();
        void remove_at(void* const&  node);
        void remove_at(const arenaSize_t& index);

        inline T& get_at(void* const& node) { return static_cast<Node*>(node)->value; }
        const T& get(const arenaSize_t& index);
        const T& getFromBack(const arenaSize_t& indexFromBack);

        void clear();

        [[nodiscard]] inline const arenaSize_t& length() const {return _length;};

    private:
        struct Node{
            Node(Node* const& prev, Node* const& next):previous(prev),next(next){}
            Node* previous;
            Node* next;
            T value;
        };

        InsideArenaType& arena;
        Node* root = nullptr;
        Node* tail = nullptr;
        arenaSize_t _length = 0;
    };

    template<typename InsideArenaType, typename T> requires std::is_base_of_v<BaseArena, InsideArenaType>
    template<typename ... ConstructorArgs>
    void * LinkedList<InsideArenaType, T>::construct_back(ConstructorArgs... args) {
        if (!tail) tail = root = arena.template allocConstruct<Node>(nullptr, nullptr);
        else {
            tail->next = arena.template allocConstruct<Node>(tail, nullptr);
            tail = tail->next;
        }
        new (&(tail->value)) T(args...);
        _length++;
        return tail;
    }

    template<typename InsideArenaType, typename T> requires std::is_base_of_v<BaseArena, InsideArenaType>
    void * LinkedList<InsideArenaType, T>::push_back(const T &value) {
        if (!tail) tail = root = arena.template allocConstruct<Node>(nullptr, nullptr);
        else {
            tail->next = arena.template allocConstruct<Node>(tail, nullptr);
            tail = tail->next;
        }
        tail->value = value;
        _length++;
        return tail;
    }

    template<typename InsideArenaType, typename T> requires std::is_base_of_v<BaseArena, InsideArenaType>
    template<typename ... ConstructorArgs>
    void * LinkedList<InsideArenaType, T>::construct_front(ConstructorArgs... args) {
        if (!root) tail = root = arena.template allocConstruct<Node>(nullptr);
        else {
            root->previous = arena.template allocConstruct<Node>(nullptr, root);
            root = root->previous;
        }
        new (&(root->value)) T(args...);
        _length++;
        return root;
    }

    template<typename InsideArenaType, typename T> requires std::is_base_of_v<BaseArena, InsideArenaType>
    void * LinkedList<InsideArenaType, T>::push_front(const T &value) {
        if (!root) tail = root = arena.template allocConstruct<Node>(nullptr, nullptr);
        else {
            root->previous = arena.template allocConstruct<Node>(nullptr, root);
            root = root->previous;
        }
        root->value = value;
        _length++;
        return root;
    }

    template<typename InsideArenaType, typename T> requires std::is_base_of_v<BaseArena, InsideArenaType>
    template<typename ... ConstructorArgs>
    void LinkedList<InsideArenaType, T>::construct_at(void* parent, ConstructorArgs... args) {
        Node* const& child = static_cast<Node *>(parent)->next;
        static_cast<Node *>(parent)->next = arena.template allocConstruct<Node>(parent, child);
        new (&(static_cast<Node *>(parent)->next->value)) T(args...);
        static_cast<Node *>(parent)->next->previous = static_cast<Node *>(parent);
        static_cast<Node *>(parent)->next->next = child;
        _length++;
    }

    template<typename InsideArenaType, typename T> requires std::is_base_of_v<BaseArena, InsideArenaType>
    void LinkedList<InsideArenaType, T>::push_at(const T &value, void* parent) {
        Node* const& child = static_cast<Node *>(parent)->next;
        static_cast<Node *>(parent)->next = arena.template allocConstruct<Node>(static_cast<Node *>(parent), child);
        //static_cast<Node *>(parent)->next->previous = static_cast<Node *>(parent);
        //static_cast<Node *>(parent)->next->next = child;
        static_cast<Node *>(parent)->next->value = value;
        if (child != nullptr) child->previous = static_cast<Node *>(parent)->next;
        _length++;
    }

    template<typename InsideArenaType, typename T> requires std::is_base_of_v<BaseArena, InsideArenaType>
    template<typename ... ConstructorArgs>
    void LinkedList<InsideArenaType, T>::construct_at(const arenaSize_t &index, ConstructorArgs... args) {
        LOGGER_ASSERT_EXCEPT(index < _length);
        Node* n = root;
        for (arenaSize_t _ = 0; _ < index; _++) n = n->next;
        construct_at(args..., n);
    }

    template<typename InsideArenaType, typename T> requires std::is_base_of_v<BaseArena, InsideArenaType>
    void LinkedList<InsideArenaType, T>::push_at(const arenaSize_t &index, const T &value) {
        LOGGER_ASSERT_EXCEPT(index < _length);
        Node* n = root;
        for (arenaSize_t _ = 0; _ < index; _++) n = n->next;
        push_at(value, n);
    }

    template<typename InsideArenaType, typename T> requires std::is_base_of_v<BaseArena, InsideArenaType>
    void LinkedList<InsideArenaType, T>::remove_back() {
        tail = tail->previous;
        arena.softDelete(tail->next);
        tail->next = nullptr;
        _length--;
    }

    template<typename InsideArenaType, typename T> requires std::is_base_of_v<BaseArena, InsideArenaType>
    void LinkedList<InsideArenaType, T>::remove_front() {
        root = root->next;
        arena.softDelete(root->previous);
        root->previous = nullptr;
        _length--;
    }

    template<typename InsideArenaType, typename T> requires std::is_base_of_v<BaseArena, InsideArenaType>
    void LinkedList<InsideArenaType, T>::remove_at(void* const&node) {
        if (static_cast<Node *>(node)->previous) static_cast<Node *>(node)->previous->next = static_cast<Node *>(node)->next;
        if (static_cast<Node *>(node)->next) static_cast<Node *>(node)->next->previous = static_cast<Node *>(node)->previous;
        arena.softDelete(node);
        _length--;
    }

    template<typename InsideArenaType, typename T> requires std::is_base_of_v<BaseArena, InsideArenaType>
    void LinkedList<InsideArenaType, T>::remove_at(const arenaSize_t &index) {
        LOGGER_ASSERT_EXCEPT(index < _length);
        Node* n = root;
        for (arenaSize_t _ = 0; _ < index; _++) n = n->next;
        remove_at(n);
    }

    template<typename InsideArenaType, typename T> requires std::is_base_of_v<BaseArena, InsideArenaType>
    const T & LinkedList<InsideArenaType, T>::get(const arenaSize_t &index) {
        LOGGER_ASSERT_EXCEPT(index < _length);
        Node* n = root;
        for (arenaSize_t _ = 0; _ < index; _++) n = n->next;
        return get_at(n);
    }

    template<typename InsideArenaType, typename T> requires std::is_base_of_v<BaseArena, InsideArenaType>
    const T & LinkedList<InsideArenaType, T>::getFromBack(const arenaSize_t &indexFromBack) {
        LOGGER_ASSERT_EXCEPT(indexFromBack < _length);
        Node* n = tail;
        for (arenaSize_t _ = 0; _ < indexFromBack; _++) n = n->previous;
        return get_at(n);
    }

    template<typename InsideArenaType, typename T> requires std::is_base_of_v<BaseArena, InsideArenaType>
    void LinkedList<InsideArenaType, T>::clear() {
        Node* n = root;
        while (n != nullptr) {
            Node* lastN = n;
            n = n->next;
            arena.softDelete(lastN);
        }
        _length = 0;
    }
}
