//
// Created by Luis Holanda on 2019-04-16.
//

#ifndef LAB3_OPERATION_HPP
#define LAB3_OPERATION_HPP


template<typename T>
struct Operation {
    /// All possible operations.
    enum Kind {
        /// Sum two matrices.
        Sum,
        /// Multiply two matrices.
        Multiply,
        /// Transpose a matrix.
        Transpose,
        /// No-operation, default value for `Operation::kind`.
        Nop
    };

    /// What operation will be realized.
    Kind kind = Nop;
    /// Left-hand of the operation.
    T* const lhs = nullptr;
    /// Right-hand of the operation.
    /// In a `Kind::Transpose` operation, this can be ignored.
    T* const rhs = nullptr;

    static Operation sum(T* lhs, T* rhs) noexcept;
    static Operation multiply(T* lhs, T* rhs) noexcept;
    static Operation transpose(T* matrix) noexcept;
};

template<typename T>
inline Operation<T> Operation<T>::sum(T *const lhs, T *const rhs) noexcept {
    return Operation {
        .kind = Operation::Kind::Sum,
        .lhs = lhs,
        .rhs = rhs
    };
}

template<typename T>
inline Operation<T> Operation<T>::multiply(T *const lhs, T *const rhs) noexcept {
    return Operation {
        .kind = Operation::Kind::Multiply,
        .lhs = lhs,
        .rhs = rhs
    };
}

template<typename T>
inline Operation<T> Operation<T>::transpose(T *const matrix) noexcept {
    return Operation {
        .kind = Operation::Kind::Transpose,
        .lhs = matrix
    };
}

#endif //LAB3_OPERATION_HPP
