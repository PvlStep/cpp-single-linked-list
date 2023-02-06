#pragma once

#include <cassert>
#include <cstddef>
#include <iterator>
#include <string>
#include <utility>
#include <iostream>

template <typename Type>
class SingleLinkedList {
    
    // Узел списка
    struct Node {
        Node() = default;
        Node(const Type& val, Node* next)
            : value(val)
            , next_node(next) {
        }
        Type value;
        Node* next_node = nullptr;
    };

    template <typename ValueType>
    class BasicIterator {
        
        friend class SingleLinkedList;

        explicit BasicIterator(Node* node): node_(node) {            
        }

    public:
        // Объявленные ниже типы сообщают стандартной библиотеке о свойствах этого итератора

        // Категория итератора — forward iterator
        // (итератор, который поддерживает операции инкремента и многократное разыменование)
        using iterator_category = std::forward_iterator_tag;
        // Тип элементов, по которым перемещается итератор
        using value_type = Type;
        // Тип, используемый для хранения смещения между итераторами
        using difference_type = std::ptrdiff_t;
        // Тип указателя на итерируемое значение
        using pointer = ValueType*;
        // Тип ссылки на итерируемое значение
        using reference = ValueType&;

        BasicIterator() = default;

        // Конвертирующий конструктор/конструктор копирования
        // При ValueType, совпадающем с Type, играет роль копирующего конструктора
        // При ValueType, совпадающем с const Type, играет роль конвертирующего конструктора
        BasicIterator(const BasicIterator<Type>& other) noexcept {
            this->node_ = other.node_; 
        }

        // Чтобы компилятор не выдавал предупреждение об отсутствии оператора = при наличии
        // пользовательского конструктора копирования, явно объявим оператор = и
        // попросим компилятор сгенерировать его за нас
        BasicIterator& operator=(const BasicIterator& rhs) = default;

        // Оператор сравнения итераторов (в роли второго аргумента выступает константный итератор)
        [[nodiscard]] bool operator==(const BasicIterator<const Type>& rhs) const noexcept {
            return node_ == rhs.node_;
        }

        [[nodiscard]] bool operator!=(const BasicIterator<const Type>& rhs) const noexcept {
            return !(node_ == rhs.node_);
        }

        [[nodiscard]] bool operator==(const BasicIterator<Type>& rhs) const noexcept {
            return node_ == rhs.node_;
        }

        [[nodiscard]] bool operator!=(const BasicIterator<Type>& rhs) const noexcept {
            return !(node_ == rhs.node_);
        }

        BasicIterator& operator++() noexcept {
            assert(node_ != nullptr);
            node_ = node_->next_node;
            return *this;
        }

        BasicIterator operator++(int) noexcept {
            auto old_value(*this); 
            ++(*this); 
            return old_value;
        }
        
        [[nodiscard]] reference operator*() const noexcept {
            assert(node_ != nullptr);
            return node_->value;
        }

        [[nodiscard]] pointer operator->() const noexcept {
            assert(node_ != nullptr);
            return &(node_->value);
        }


    private:
        Node* node_ = nullptr;
    };

    

public:
    using value_type = Type;
    using reference = value_type&;
    using const_reference = const value_type&;

    // Итератор, допускающий изменение элементов списка
    using Iterator = BasicIterator<Type>;
    // Константный итератор, предоставляющий доступ для чтения к элементам списка
    using ConstIterator = BasicIterator<const Type>;

    SingleLinkedList()
    :size_(0)
    {}
    
    ~SingleLinkedList() {
        Clear();
    }


    SingleLinkedList(std::initializer_list<Type> values): head_(), size_(0) {
        this->CopyAndReverse(values);
    }

    SingleLinkedList(const SingleLinkedList& other) {
        SingleLinkedList temp;
        temp.CopyAndReverse(other);
        swap(temp);
    }
    

     SingleLinkedList& operator=(const SingleLinkedList& rhs) {
        SingleLinkedList temp;
        temp.CopyAndReverse(rhs);
        swap(temp);
        return *this;
    }

     [[nodiscard]] Iterator begin() noexcept {
         return Iterator{ head_.next_node };
     }

     [[nodiscard]] Iterator end() noexcept {
         return Iterator(nullptr);
     }

     [[nodiscard]] ConstIterator begin() const noexcept {
         return this->cbegin();
     }

     [[nodiscard]] ConstIterator end() const noexcept {
         return this->cend();
     }

     [[nodiscard]] ConstIterator cbegin() const noexcept {
         return ConstIterator{ head_.next_node };
     }

     [[nodiscard]] ConstIterator cend() const noexcept {
         return ConstIterator(nullptr);
     }

     [[nodiscard]] Iterator before_begin() noexcept {
         return Iterator{ &head_ };
     }

     [[nodiscard]] ConstIterator cbefore_begin() const noexcept {
         return ConstIterator{ const_cast<Node*> (&head_) };
     }

     [[nodiscard]] ConstIterator before_begin() const noexcept {
         return ConstIterator{ const_cast<Node*> (&head_) };
     }

  
    size_t GetSize() const noexcept {
        return size_;
    }

     bool IsEmpty() const noexcept {
        return size_ == 0;
    }
    
     void swap(SingleLinkedList& other) noexcept {
         std::swap(other.size_, size_);
         std::swap(other.head_.next_node, head_.next_node);
     }


    void PushFront(const Type& value) {
        head_.next_node = new Node(value, head_.next_node);
        ++size_;
    }

    Iterator InsertAfter(ConstIterator pos, const Type& value) {
        assert(pos.node_ != nullptr);
        if (pos != this->before_begin()) {
            Node* new_node = new Node(value, pos.node_->next_node);
            pos.node_->next_node = new_node;
            ++size_;
            return Iterator(new_node);
        }
        else {
            PushFront(value);
            return Iterator(head_.next_node);
        }
    }

    void PopFront() noexcept {
        if (head_.next_node != nullptr) {
            Node* before_node = head_.next_node;
            head_.next_node = before_node->next_node;
            --size_;
            delete before_node;
        }
    }

    Iterator EraseAfter(ConstIterator pos) noexcept {
        assert(pos.node_ && pos.node_->next_node && size_ > 0);
        Node* before = pos.node_;
        auto after_item = before->next_node->next_node;
        delete before->next_node;
        before->next_node = after_item;
        --this->size_;
        return Iterator(before->next_node);;
    }

    void Clear() noexcept {
        while (head_.next_node) {
            Node* temp = head_.next_node->next_node;
            delete this->head_.next_node;
            head_.next_node = temp;
            
        }
        size_ = 0;
    }

    template <typename ForwList>
    void CopyAndReverse(ForwList& other) {
    /* Комментарий ревьюера: "Внутри класса вам доступно использование приватных методов, в частности здесь можно значительно упростить алгоритм. 
        Прямо внутри этого метода создавайте новые элементы и цепляйте их друг за дружку начиная с head. Это позволит избавиться от лишнего цикла"
        
       - Попытался реализовать такой способ, но он не работает. Буду пытаться разобраться с этой темой подробнее.
       Node* last = nullptr;
        for (auto elem : other) {
            Node* item = new Node(elem);
            if (head_.next_node == nullptr) {
                head_ = *item;
            }
            else {
                last->next_node = item;
            }
            last = item;
            ++size_;
      }*/
      
        SingleLinkedList temp, temp2;
        for (auto elem : other) {
            temp.PushFront(elem);
        }
        for (auto it : temp) {
            this->PushFront(it);
        }
    }

private:
    Node head_;
    size_t size_;
};

 template <typename Type>
 void swap(SingleLinkedList<Type>& lhs, SingleLinkedList<Type>& rhs) noexcept {
     lhs.swap(rhs);
}

 template <typename Type>
bool operator==(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    if (lhs.GetSize() == rhs.GetSize()) {
        return std::equal(lhs.begin(), lhs.end(), rhs.begin());
    }
    return false;
}

template <typename Type>
bool operator!=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
bool operator<(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
bool operator<=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs > rhs);
}

template <typename Type>
bool operator>(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return std::lexicographical_compare(rhs.begin(), rhs.end(), lhs.begin(), lhs.end());
}

template <typename Type>
bool operator>=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs < rhs);
}
