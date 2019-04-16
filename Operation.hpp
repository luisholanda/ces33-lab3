//
// Created by Luis Holanda on 2019-04-16.
//

#ifndef LAB3_OPERATION_HPP
#define LAB3_OPERATION_HPP


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
    void* const lhs = nullptr;
    /// Right-hand of the operation.
    /// In a `Kind::Transpose` operation, this can be ignored.
    void* const rhs = nullptr;

    static Operation sum(void* lhs, void* rhs) noexcept;
    static Operation multiply(void* lhs, void* rhs) noexcept;
    static Operation transpose(void* matrix) noexcept;
};

inline Operation Operation::sum(void *const lhs, void *const rhs) noexcept {
    return Operation {
        .kind = Operation::Kind::Sum,
        .lhs = lhs,
        .rhs = rhs
    };
}

inline Operation Operation::multiply(void *const lhs, void *const rhs) noexcept {
    return Operation {
        .kind = Operation::Kind::Multiply,
        .lhs = lhs,
        .rhs = rhs
    };
}

inline Operation Operation::transpose(void *const matrix) noexcept {
    return Operation {
        .kind = Operation::Kind::Transpose,
        .lhs = matrix
    };
}

#endif //LAB3_OPERATION_HPP
