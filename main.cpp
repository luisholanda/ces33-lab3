#include <iostream>
#include "Matrix.hpp"

int main() {
    auto m = Matrix<>::random(2, 3), n = Matrix<>::random(2, 3);
    m.print();
    std::cout << std::endl;
    n.print();
    std::cout << std::endl;
    (m + n).print();
    std::cout << std::endl;
    (m += n).print();
    std::cout << std::endl;
    (m.transpose()).print();
    std::cout << std::endl;
    (m * n).print();
    std::cout << std::endl;
    (m *= n).print();
    return 0;
}