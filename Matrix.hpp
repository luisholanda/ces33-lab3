#ifndef LAB3_MATRIX_HPP
#define LAB3_MATRIX_HPP

#include <iostream>
#include <sstream>
#include <vector>
#include <random>
#include <cassert>

template<typename T = double>
class Matrix {
public:
    using RandomAllowedMatrix = Matrix<typename std::enable_if<std::is_floating_point<T>::value, T>::type>;

    /*!
    *   \brief Default initializer
    */
    Matrix<T>(size_t nrows, size_t ncols) 
        : nrows(nrows)
        , ncols(ncols)
        , data(nrows, RowCont(ncols))
    {}

    Matrix<T>(const Matrix<T>& copy) = default;

    /*!
    *   \brief Creates a matrix with uniformly distributed random numbers in [0, 1] if T is a real type
    */
    static RandomAllowedMatrix
    random(size_t nrows, size_t ncols) {
        Matrix<T> matrix(nrows, ncols);
        std::random_device dev;
        std::default_random_engine generator(dev());
        std::uniform_real_distribution<T> distribution(0.0, 1.0);
        for (auto& row : matrix.data) {
            for (auto& elem : row) {
                elem =  distribution(generator);
            }
        }
        return matrix;
    }

    /*!
    *   \brief Prints matrix do stdout
    */
    void print() {
        std::ostringstream os;

        for (const auto& row : data) {
            for (const auto& elem : row) {
                os << elem << " ";
            }
            os << std::endl;
        }

        std::cout << os.str();
    }

    Matrix<T> transpose() {
        Matrix<T> res(ncols, nrows);

        for (size_t i = 0; i < nrows; i++) {
            for (size_t j = 0; j < ncols; j++) {
                res.data[i][j] = data[j][i];
            }
        }
        return *this;
    }


    Matrix<T> operator + (const Matrix<T>& other) {
        assert (nrows == other.nrows && ncols == other.ncols);
        Matrix<T> result(*this);
        MatrixCont& resultData = result.data;
        const MatrixCont& otherData = other.data;
        for (size_t i = 0; i < other.nrows; i++) {
            for (size_t j = 0; j < other.ncols; j++) {
                resultData[i][j] = resultData[i][j] + otherData[i][j];
            }
        }
        return result;
    }

    Matrix<T>& operator += (const Matrix<T>& other) {
        assert (nrows == other.nrows && ncols == other.ncols);
        MatrixCont& resultData = this->data;
        const MatrixCont& otherData = other.data;
        for (size_t i = 0; i < other.nrows; i++) {
            for (size_t j = 0; j < other.ncols; j++) {
                resultData[i][j] = resultData[i][j] + otherData[i][j];
            }
        }
        return *this;
    }

    Matrix<T> operator * (const Matrix<T>& other) {
        assert (ncols == other.nrows);
        Matrix<T> result(nrows, other.ncols);
        MatrixCont& resultData = result.data;
        const MatrixCont& otherData = other.data;
        for (size_t i = 0; i < nrows; i++) {
            for (size_t j = 0; j < other.ncols; j++) {
                T& resultElem = resultData[i][j];
                for (size_t k = 0; k < ncols; k++) {
                    resultElem = resultElem + data[i][k] * otherData[k][j];
                }
            }
        }
        return result;
    }

     Matrix<T>& operator *= (const Matrix<T>& other) {
        assert (ncols == other.nrows);
        const MatrixCont& otherData = other.data;
        RowCont oldData;
        for (size_t i = 0; i < nrows; i++) {
            oldData = data[i];
            data[i].resize(other.ncols);
            for (size_t j = 0; j < other.ncols; j++) {
                auto resultElem = T();
                for (size_t k = 0; k < ncols; k++) {
                    resultElem = resultElem + oldData[k] * otherData[k][j];
                }
                data[i][j] = resultElem;
            }
        }
        return *this;
    }

    Matrix<T>& operator=(Matrix<T> const& m) = default;
private:
    using RowCont = std::vector<T>;     //!< Container for rows
    using MatrixCont = std::vector<RowCont>; //!< Container for columns

    size_t nrows;   //!< Number of rows
    size_t ncols;   //!< Number of columns
    MatrixCont data;    //!< Data container
};

#endif