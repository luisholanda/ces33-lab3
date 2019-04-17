#include <iostream>
#include <optional>
#include <thread>
#include "Matrix.hpp"

#include "Operation.hpp"
#include "Channel.hpp"

const size_t MATRIX_SIZE = 100;
const size_t NUMBER_OPS = 10000;
const size_t NUMBER_THREADS = 4;
const size_t CHANNEL_SIZE = 16;

std::optional<Matrix<double>> execute(Operation<Matrix<double>>& op) {
    using Op = Operation<Matrix<double>>;

    if (not (op.kind == Op::Kind::Nop)) {
        if (op.rhs == nullptr) {
            if (op.kind != Op::Kind::Transpose or op.lhs == nullptr) {
                return std::nullopt;
            }
        }
    }

    std::optional<Matrix<double>> result{};
    switch (op.kind) {
        case Op::Kind::Sum:
            return *op.lhs + *op.rhs;
        case Op::Kind::Multiply:
            return *op.lhs * *op.rhs;
        case Op::Kind::Transpose:
            op.lhs->transpose();
            return *op.lhs;
        case Op::Kind::Nop:
            return std::nullopt;
    }
}

Operation<Matrix<double>> random_op(Matrix<double>* const a, Matrix<double>* const b) {
    using Op =  Operation<Matrix<double>>;

    std::default_random_engine generator((std::random_device()()));

    std::uniform_int_distribution distribution(0, 2);

    switch (distribution(generator)) {
        case 0:
            return Op::sum(a, b);
        case 1:
            return Op::multiply(a, b);
        case 2:
            return Op::transpose(a);
    }

    // Unreachable.
    return {};
}

void consumer(Channel<Operation<Matrix<double>>>* chan) {
    Operation<Matrix<double>> op;

    while(chan->recv(op)) {
        std::cout << std::this_thread::get_id() << ": Recv OP " << op.kind << std::endl;
        auto res = execute(op);
        if (res.has_value()) {
            std::cout << std::this_thread::get_id() << ": Finalized OP" << std::endl;
        }
    }

    std::cout << std::this_thread::get_id() << ": Finalized execution" << std::endl;
}

void parallel(Matrix<double>* m, Matrix<double>* n) {
    auto channel = new Channel<Operation<Matrix<double>>>(CHANNEL_SIZE);

    std::vector<std::thread> threads{};
    threads.reserve(NUMBER_THREADS);

    for (size_t t = 0; t != NUMBER_THREADS; t++) {
        std::thread thread(consumer, channel);

        threads.push_back(std::move(thread));
    }

    for (size_t i = 0; i != NUMBER_OPS; i++) {
        auto op = random_op(m, n);

        channel->send(op);
    }

    channel->close();

    for (auto& thread : threads) {
        thread.join();
    }
}

void serial(Matrix<double>* m, Matrix<double>* n) {
    for (size_t i = 0; i != NUMBER_OPS; i++) {
        auto op = random_op(m, n);
        std::cout << "Recv OP " << op.kind << std::endl;
        execute(op);
        std::cout << "Finished OP" << std::endl;
    }
}

int main() {
    auto m = new Matrix(MATRIX_SIZE, MATRIX_SIZE);
    auto n = new Matrix(MATRIX_SIZE, MATRIX_SIZE);

    *m = Matrix<>::random(MATRIX_SIZE, MATRIX_SIZE);
    *n = Matrix<>::random(MATRIX_SIZE, MATRIX_SIZE);

    parallel(m, n);
//    serial(m, n);

    return 0;
}