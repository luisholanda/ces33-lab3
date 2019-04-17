#include <iostream>
#include <fstream>
#include <optional>
#include <thread>
#include <ctime>
#include <cmath>
#include <string>

#include "Matrix.hpp"

#include "Operation.hpp"
#include "Channel.hpp"

const size_t MATRIX_SIZE = 100;
const size_t NUMBER_OPS = 1000;
const size_t NUMBER_SAMPLES = 1000;
const size_t THREADS_NUMBER_SAMPLES = 500;

const size_t MAX_THREADS = 24;
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
            return op.lhs->transpose();
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
        // std::cout << std::this_thread::get_id() << ": Recv OP " << op.kind << std::endl;
        auto res = execute(op);
        if (res.has_value()) {
            // std::cout << std::this_thread::get_id() << ": Finalized OP" << std::endl;
        }
    }

    // std::cout << std::this_thread::get_id() << ": Finalized execution" << std::endl;
}

clock_t now() {
    return std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
}

clock_t parallel(Matrix<double>* m, Matrix<double>* n, size_t numThreads = NUMBER_THREADS) {
    clock_t timer;
    auto channel = new Channel<Operation<Matrix<double>>>(CHANNEL_SIZE);

    std::vector<std::thread> threads{};
    threads.reserve(NUMBER_THREADS);

    for (size_t t = 0; t != numThreads; t++) {
        std::thread thread(consumer, channel);

        threads.push_back(std::move(thread));
    }

    timer = now();
    for (size_t i = 0; i != NUMBER_OPS; i++) {
        auto op = random_op(m, n);

        channel->send(op);
    }

    channel->close();

    for (auto& thread : threads) {
        thread.join();
    }

    return now() - timer;
}

clock_t serial(Matrix<double>* m, Matrix<double>* n) {
    clock_t timer;
    timer = now();
    for (size_t i = 0; i != NUMBER_OPS; i++) {
        auto op = random_op(m, n);
        // std::cout << "Recv OP [kind = " << op.kind << "] num " << i << " " << NUMBER_OPS << " " << (i < NUMBER_OPS) << std::endl;
        execute(op);
        // std::cout << "Finished OP" << std::endl;
    }
    return now() - timer;
}

void dump(const std::vector<double> data, std::string filename) {
    std::ofstream of(filename);

    for (const auto& elem : data) {
        of << elem << std::endl;
    }

    of.close();
}

void testThreads(Matrix<double>* m, Matrix<double>* n) {
    std::vector<double> meantimeByThread;
    for (int threads = 17; threads <= MAX_THREADS; threads++) {
        double mean = 0.0, elapsed;
        for (size_t sample = 0; sample < THREADS_NUMBER_SAMPLES; sample++) {
            elapsed = parallel(m, n, threads);
            mean = (sample * mean + elapsed)/ (sample + 1);
            std::cout << "Iteration " << sample + 1 << ": mean = " << mean << "s" << std::endl;
        }
        meantimeByThread.push_back(mean);
    }
    dump(meantimeByThread, "threads.txt");
}

void testCmpSyncAsync(Matrix<double>* m, Matrix<double>* n) {
    double elapsed, mean = 0.0, deviation = -1.0;
    std::vector<double> samples;
    for (size_t sample = 0; sample < NUMBER_SAMPLES; sample++) {
        
        elapsed = parallel(m, n);
        // elapsed = serial(m, n);

        samples.push_back(elapsed);
        mean = (sample * mean + elapsed)/ (sample + 1);
        
        std::cout << "Iteration " << sample + 1 << ": mean = " << mean << "s" << std::endl;
    }

    if (samples.size() > 1) {
        deviation = 0.0;
        for (const auto& sample : samples) {
        deviation += (sample - mean) * (sample - mean);
        }
        deviation /= ((int)samples.size() - 1);
    }
    
    std::cout << "Final deviation = " << deviation << " s^2" << std::endl;
    // dump(samples, "serial.txt");
    dump(samples, "parallel.txt");
}

int main() {
    clock_t program_timer = now();
    auto m = new Matrix<>(MATRIX_SIZE, MATRIX_SIZE);
    auto n = new Matrix<>(MATRIX_SIZE, MATRIX_SIZE);

    *m = Matrix<>::random(MATRIX_SIZE, MATRIX_SIZE);
    *n = Matrix<>::random(MATRIX_SIZE, MATRIX_SIZE);

    // testThreads(m, n);
    testCmpSyncAsync(m, n);

    std::cout << "Program duration: " << (now() - program_timer) << std::endl;
    return 0;
}