#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <thread>
#include <chrono>
#include <future>
#include <ctime>
#include <string>
#include <algorithm>

using namespace std;

template <class T>
class Matrix {
public:
    int rows;
    int columns;
    vector<vector<T>> matrix;

    Matrix() {
            rows = 0;
            columns = 0;
            matrix = NULL;
    }

    Matrix(int ROWS, int COLUMNS, vector<vector<T>>& VECTOR)
            : rows(ROWS), columns(COLUMNS), matrix(VECTOR) {
            if (ROWS <= 0 || COLUMNS <= 0 || VECTOR.size() != rows || (rows > 0 && VECTOR[0].size() != columns)) {
                cerr << "Rows and columns can't be less or equal than zero" << endl;
                throw invalid_argument("");
        }
    }

    Matrix(int ROWS, int COLUMNS) : rows(ROWS), columns(COLUMNS) {
        matrix.resize(rows, vector<T>(columns));
    }


    Matrix(const Matrix& other) : rows(other.rows), columns(other.columns), matrix(other.matrix) {}

    Matrix(const char *fileName);

    ~Matrix() = default;

    void setValue(int row, int column, T value) {
        matrix[row][column] = value;
    }

    void readFromConsole();

    void readFromConsoleWithSize(int, int);

    void printToConsole() const;

    static Matrix<T> getZeroMatrix(int ROWS, int COLUMNS) {
        Matrix<T> result(ROWS, COLUMNS);
        for (int i = 0; i < ROWS; ++i) {
            for (int j = 0; j < COLUMNS; ++j) {
                result.matrix[i][j] = T();
            }
        }
        return result;
    }

    static Matrix<T> getIdentityMatrix(int ROWS, int COLUMNS);

    void readFromFile(const char *);

    void writeToFile(const char *);

    Matrix &Multiply(T);

    Matrix& operator=(const Matrix &m) {
        rows = m.rows;
        columns = m.columns;
        matrix = m.matrix;
        return *this;
    }

    Matrix operator* (const Matrix & other) {
        // столбцы 1-ой и строки 2-ой
        if (columns == other.rows)
        {
            Matrix result(rows, other.columns);

            std::vector<std::thread> threads;
            for (int r = 0; r < rows; r++) {
                threads.emplace_back([this, &other, r, &result] {
                    for (int c_result = 0; c_result < other.columns; c_result++)
                    {
                        for (int c = 0; c < columns; c++)
                        {
                            result.matrix[r][c_result] += matrix[r][c] * other.matrix[c][c_result];
                        }
                    }
                });
            }

            // Join all threads
            for (auto& thread : threads) {
                thread.join();
            }

            return result;

        }
        else
        {
            cerr << "ERROR WITH MULTIPLYING" << endl;
        }

        // вряд ли до сюда дойдет, но пусть будет
        return Matrix();
    }

    Matrix operator*(const T& b) {
        Matrix result(rows, columns);

        std::vector<std::thread> threads;
        for (int i = 0; i < rows; i++) {
            threads.emplace_back([this, b, i, &result] {
                for (int j = 0; j < columns; j++) {
                    result.matrix[i][j] = b * matrix[i][j];
                }
            });
        }

        // Join all threads
        for (auto& thread : threads) {
            thread.join();
        }

        return result;
    }


    Matrix operator+(const Matrix & other) {
        if (rows != other.rows || columns != other.columns)
        {
            cerr << "ERROR WITH ADDITION" << endl;
            throw invalid_argument("");
        }

        // столбцы равны столбцам, строки - строкам

        Matrix result(rows, columns);

        std::vector<std::thread> threads;

        for (int r = 0; r < rows; r++)
        {
            threads.emplace_back([this, &other, r, &result] {
                for (int c = 0; c < columns; c++) {
                    result.matrix[r][c] = matrix[r][c] + other.matrix[r][c];
                }
            });
        }

        // Join all threads
        for (auto& thread : threads) {
            thread.join();
        }

        return result;
    }

    Matrix operator-(const Matrix & other) {
        if (rows != other.rows || columns != other.columns)
        {
            cerr << "ERROR WITH ADDITION" << endl;
            throw invalid_argument("");
        }

        // столбцы равны столбцам, строки - строкам

        Matrix result(rows, columns);

        std::vector<std::thread> threads;

        for (int r = 0; r < rows; r++)
        {
            threads.emplace_back([this, &other, r, &result] {
                for (int c = 0; c < columns; c++) {
                    result.matrix[r][c] = matrix[r][c] - other.matrix[r][c];
                }
            });
        }

        // Join all threads
        for (auto& thread : threads) {
            thread.join();
        }

        return result;
    }

    bool operator==(const Matrix &other) {
        if (rows != other.rows || columns != other.columns)
        {
            return false;
        }

        bool flag = true;
        for (int r = 0; r < rows; r++)
        {
            for (int c = 0; c < columns; c++)
            {
                if (matrix[r][c] != other.matrix[r][c])
                {
                    flag = false;
                    return flag;
                }
            }
        }
        return flag;
    }

    bool operator!=(const Matrix &other) {
        return !(*this == other);;
    }

    friend Matrix operator!(const Matrix &);

    Matrix BlockAdd(const Matrix &other, int blockSize) {
        if (rows != other.rows || columns != other.columns) {
            cerr << "ERROR WITH ADDITION" << endl;
            throw invalid_argument("Matrices must be of the same size for addition.");
        }

        Matrix result(rows, columns);

        vector<future<void>> futures;

        for (int r = 0; r < rows; r += blockSize) {
            for (int c = 0; c < columns; c += blockSize) {
                futures.push_back(async(launch::async, [r, c, blockSize, this, &other, &result] {
                    for (int R = r; R < min(r + blockSize, rows); R++) {
                        for (int C = c; C < min(c + blockSize, columns); C++) {
                            result.matrix[R][C] = matrix[R][C] + other.matrix[R][C];
                        }
                    }
                }));
            }
        }

        for (auto &future : futures) {
            future.get();
        }

        return result;
    }

    Matrix BlockSub(const Matrix &other, int blockSize) {
        if (rows != other.rows || columns != other.columns) {
            cerr << "ERROR WITH ADDITION" << endl;
            throw invalid_argument("Matrices must be of the same size for addition.");
        }

        Matrix result(rows, columns);

        vector<future<void>> futures;

        for (int r = 0; r < rows; r += blockSize) {
            for (int c = 0; c < columns; c += blockSize) {
                futures.push_back(async(launch::async, [r, c, blockSize, this, &other, &result] {
                    for (int R = r; R < min(r + blockSize, rows); R++) {
                        for (int C = c; C < min(c + blockSize, columns); C++) {
                            result.matrix[R][C] = matrix[R][C] - other.matrix[R][C];
                        }
                    }
                }));
            }
        }

        for (auto &future : futures) {
            future.get();
        }

        return result;
    }

    Matrix BlockMult(const Matrix & other, int BlockSize) {
        // столбцы 1-ой и строки 2-ой
        if (columns == other.rows) {
            Matrix result(rows, other.columns);

            vector<future<void>> futures;
            for (int r = 0; r < rows; r += BlockSize) {
                for (int c = 0; c < columns; c++) {
                    futures.push_back(async(launch::async, [r, c, BlockSize, this, &other, &result] {
                        for (int R = r; R < min(r + BlockSize, rows); R++) {
                            for (int C = c; C < min(c + BlockSize, columns); C++) {
                                result.matrix[R][C] += matrix[R][C] * other.matrix[R][C];
                            }
                        }
                    }));
                }
            }
            for (auto &future: futures) {
                future.get();
            }

            return result;
        } else {
            cerr << "ERROR WITH MULTIPLYING" << endl;
        }
    }

    Matrix BlockMultiN (T number, int BlockSize) {
        Matrix result(rows, columns);
        vector<future<void>> futures;
        for (int i = 0; i < rows; i += BlockSize) {
            for (int j = 0; j < columns; j += BlockSize) {
                futures.emplace_back(async(launch::async, [number, i, j, this, &result, BlockSize] {
                    for (int indexI = i; indexI < min(i+BlockSize, rows); indexI++) {
                        for (int indexJ = j; indexJ < min(j+BlockSize, columns); indexJ++) {
                            result.matrix[indexI][indexJ] = number * matrix[indexI][indexJ];
                        }
                    }
                }));
            }
        }
        for (auto &future : futures) {
            future.get();
        }
        return result;
    }

    T BlockDeterminant(int BlockSize) //
    {
        if (rows != columns)
        {
            cerr << "MATRIX SHOULD BE SQUARED" << endl;
        }

        if (rows == 1)
        {
            return matrix[0][0];
        }

        T det = 0;
        int sign = 1;

        vector<future<void>> futures;
        for (int c = 0; c < columns; c += BlockSize) {
            futures.push_back(async(launch::async, [&, BlockSize, c] {
                for (int C = c; C < min(c + BlockSize, columns); C++) {
                    Matrix mnr(rows - 1, columns - 1);
                    for (int r = 1; r < rows; r++) {
                        int k = 0;
                        for (int l = 0; l < columns; l++) {
                            if (l != C) {
                                mnr.matrix[r - 1][k] = matrix[r][l];
                                k++;
                            }
                        }
                    }
                    // Умножаем элемент первой строки на определитель подматрицы
                    det += sign * matrix[0][C] * determinant(mnr);
                    sign = -sign;
                }
            }));
        }

        for (auto &future : futures) {
            future.get();
        }

        return det;
    }

    Matrix Inverse(int BlockSize) {
        if (rows != columns) {
            cerr << "MATRIX SHOULD BE SQUARED" << endl;
            throw invalid_argument("MATRIX SHOULD BE SQUARED");
        }

        Matrix result(rows, columns);

        T det = this->BlockDeterminant(BlockSize);
        if (det == 0) {
            cerr << "MATRIX IS NOT INVERSE" << endl;
            throw invalid_argument("MATRIX IS NOT INVERSE");
        }

        Matrix another(rows, columns);

        vector<future<void>> futures;

        for (int r = 0; r < rows; r++) {
            futures.emplace_back(async(launch::async, [this, r, BlockSize, &another] {
                for (int c = 0; c < columns; c++) {
                    Matrix minor(rows - 1, columns - 1);
                    int mi = 0;
                    int mj = 0;
                    for (int i = 0; i < rows; i++) {
                        if (i != r) {
                            for (int j = 0; j < columns; j++) {
                                if (j != c) {
                                    minor.matrix[mi][mj] = this->matrix[i][j];
                                    mj++;
                                }
                            }
                            mi++;
                            mj = 0;
                        }
                    }

                    T cof = pow(-1, r + c) * minor.BlockDeterminant(BlockSize);
                    another.matrix[c][r] = cof;
                }
            }));
        }

        for (auto &future : futures) {
            future.get();
        }

        Matrix adjoint(rows, columns);

        // Транспонирование матрицы 'another' в 'adjoint' не требует асинхронности
        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < columns; c++) {
                adjoint.matrix[c][r] = another.matrix[r][c];
            }
        }

        // Деление каждого элемента матрицы 'adjoint' на определитель 'det'
        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < columns; c++) {
                result.matrix[r][c] = adjoint.matrix[r][c] / det;
            }
        }

        return result;
    }
};

template <class T>
Matrix<T>::Matrix(const char* fileName)
{
    ifstream inputFile(fileName);
    if (!inputFile)
    {
        cerr << "Error opening file." << endl;
        return;
    }

    int strf, stlf;
    inputFile >> strf >> stlf;
    for (int i = 0; i < strf; i++)
    {
        for (int j = 0; j < stlf; j++)
        {
            inputFile >> matrix[i][j];
        }
    }
    inputFile.close();
}

template <class T>
void Matrix<T>::readFromConsole()
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < columns; j++)
        {

            cout << "Print element on this position (row - " << i + 1 << " column - " << j + 1 << "): " << endl;
            cin >> matrix[i][j];
        }
    }
}

template <class T>
void Matrix<T>::readFromConsoleWithSize(int ROWS, int COLUMNS)
{
    rows = ROWS;
    columns = COLUMNS;
    matrix.resize(rows, std::vector<T>(columns));

    std::cout << "Введите элементы матрицы:" << std::endl;
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < columns; c++) {
            cout << "Print element on this position (row - " << r + 1 << " column - " << c + 1 << "): " << endl;
            std::cin >> matrix[r][c];
        }
    }
}

template<class T>
ostream& operator<<(ostream& output, const Matrix<T>& matrix)
{
    for (int i = 0; i < matrix.rows; i++)
    {
        for (int j = 0; j < matrix.columns; j++)
        {
            output << matrix.matrix[i][j] << " ";
        }
        output << std::endl;
    }
    return output;
}

template <class T>
void Matrix<T>::printToConsole() const
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < columns; j++)
        {
            cout << matrix[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;
}

template <class T>
void Matrix<T>::readFromFile(const char* fileName)
{
    ifstream inputFile(fileName);
    if (!inputFile)
    {
        cerr << "Error opening file." << endl;
        return;
    }
    int strf, stlf;
    inputFile >> strf >> stlf;
    for (int i = 0; i < strf; i++)
    {
        for (int j = 0; j < stlf; j++)
        {
            inputFile >> matrix[i][j];
        }
    }
    inputFile.close();
}

template <class T>
void Matrix<T>::writeToFile(const char* fileName)
{
    ofstream outputFile(fileName);
    if (!outputFile)
    {
        cerr << "Error opening file." << endl;
        return;
    }
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < columns; j++)
        {
            outputFile << matrix[i][j] << " ";
        }
        outputFile << endl;
    }
    outputFile.close();
}

template <class T>
Matrix<T>& Matrix<T>::Multiply(T value)
{
    for (int r = 0; r < rows; r++)
    {
        for (int c = 0; c < columns; c++)
        {
            matrix[r][c] *= value;
        }
    }
    return *this;
}

template <class T>
void firstType(Matrix<T>& a, int row1, int row2)
{
    // проверяем, что номера строк находятся в допустимом диапазоне
    if (row1 < 0 || row1 > a.rows || row2 < 0 || row2 > a.rows)
    {
        cerr << "ERROR ..." << endl;
    }

    // свап ????

    swap(a.matrix[row1], a.matrix[row2]);

    /*T* temp = a.matrix[row1];
    a.matrix[row1] = a.matrix[row2];
    a.matrix[row2] = temp;*/
}

template <class T>
void secondType(Matrix<T>& a, int row, T alpha)
{
    // проверяем, что номер строки находится в допустимом диапазоне
    if (row > a.rows || row < 0)
    {
        cerr << "ERROR ... x2" << endl;
    }
    // умножаем каждый элемент строки на число
    for (int c = 0; c < a.columns; c++)
    {
        a.matrix[row][c] *= alpha;
    }
}

template <class T>
void thirdType(Matrix<T>& a, int row_m, int row_c, T alpha)
{
    // проверяем, что номера строк находятся в допустимом диапазоне
    if (row_m < 0 || row_m > a.rows || row_c < 0 || row_c > a.rows)
    {
        cerr << "ERROR ... x3" << endl;
    }
    // прибавляем к одной строке другую, умноженную на число
    for (int c = 0; c < a.columns; c++)
    {
        a.matrix[row_c][c] += (a.matrix[row_m][c] * alpha);
    }
}

template <class T>
T determinant(const Matrix<T>& a) //
{
    if (a.rows != a.columns)
    {
        cerr << "MATRIX SHOULD BE SQUARED" << endl;
    }

    if (a.rows == 1)
    {
        return a.matrix[0][0];
    }

    T det = 0;
    int sign = 1;

    vector<thread> threads;
    for (int c = 0; c < a.columns; c++) {
        threads.emplace_back([&a, &det, c, &sign] {
            Matrix<T> mnr(a.rows - 1, a.columns - 1);
            for (int r = 1; r < a.rows; r++) {
                int k = 0;
                for (int l = 0; l < a.columns; l++) {
                    if (l != c) {
                        mnr.matrix[r - 1][k] = a.matrix[r][l];
                        k++;
                    }
                }
            }
            // Умножаем элемент первой строки на определитель подматрицы
            det += sign * a.matrix[0][c] * determinant(mnr);
            sign = -sign;
        });
    }

    for (auto &thread : threads) {
        thread.join();
    }

    return det;
}

template <class T>
Matrix<T> operator! (const Matrix<T>& a)
{
    if (a.rows != a.columns)
    {
        cerr << "MATRIX SHOULD BE SQUARED" << endl;
        throw invalid_argument("MATRIX SHOULD BE SQUARED");
    }

    Matrix<T> result(a.rows, a.columns);

    T det = determinant(a);
    if (det == 0)
    {
        cerr << "MATRIX IS NOT INVERSE" << endl;
        throw invalid_argument("MATRIX IS NOT INVERSE");
    }

    Matrix<T> another(a.rows, a.columns);

    vector<thread> threads;

    for (int r = 0; r < a.rows; r++)
    {
        threads.emplace_back([&, r](){
            for (int c = 0; c < a.columns; c++)
            {
                Matrix minor(a.rows - 1, a.columns - 1);
                int mi = 0;
                int mj = 0;
                for (int i = 0; i < a.rows; i++)
                {
                    if (i != r)
                    {
                        for (int j = 0; j < a.columns; j++)
                        {
                            if (j != c)
                            {
                                minor.matrix[mi][mj] = a.matrix[i][j];
                                mj++;
                            }
                        }
                        mi++;
                        mj = 0;
                    }
                }

                T cof = pow(-1, r + c) * determinant(minor);
                another.matrix[r][c] = cof;
            }
        });
    }

    for (auto& t : threads)
    {
        t.join();
    }

    Matrix<T> adjoint(a.rows, a.columns);
    for (int r = 0; r < a.rows; r++)
    {
        // Если же нам нужно внутри лямбда-выражения оперировать непосредственно внешними локальными переменными, то их следует передавать либо по ссылке,
        // либо через указатели. Начнем с захвата внешних переменных по ссылкам. Для этого в квадратных скобках прописывается символ ссылки ‘&’
        threads.emplace_back([&, r](){
            for (int c = 0; c < a.columns; c++)
            {
                adjoint.matrix[c][r] = another.matrix[r][c];
            }
        });
    }

    for (auto& t : threads)
    {
        t.join();
    }

    for (int r = 0; r < a.rows; r++)
    {
        threads.emplace_back([&, r](){
            for (int c = 0; c < a.columns; c++)
            {
                result.matrix[r][c] = adjoint.matrix[r][c] / det;
            }
        });
    }

    for (auto& t : threads)
    {
        t.join();
    }

    return result;
}

template <class T>
Matrix<T> Matrix<T>::getIdentityMatrix(int ROWS, int COLUMNS) //Matrix<T>::Matrix(int ROWS, int COLUMNS)
{
    if (ROWS <= 0 || COLUMNS <= 0)
    {
        cerr << "Rows and columns can't be less or equal than zero" << endl;
        throw invalid_argument("");
    }

    auto matrix = new T * [ROWS];
    for (int r = 0; r < ROWS; r++)
    {
        matrix[r] = new T[COLUMNS];
        for (int c = 0; c < COLUMNS; c++)
        {
            if (r == c)
            {
                matrix[r][c] = 1;
            }
            else
            {
                matrix[r][c] = 0;
            }
        }
    }
    return matrix;
}
int main()
{
    cout << std::thread::hardware_concurrency() << endl;
    srand(static_cast<unsigned int> (time(NULL))); //инициализирует генератор случайных чисел с помощью текущего времени как начального значения.
    int blockSize = 0;
    /*
    for (int i = 0; i <= 1000; i += 50) {
        Matrix<double> A(i, i), B(i, i);
        for (int set_rows = 0; set_rows < i; set_rows++) {
            for (int set_col = 0; set_col < i; set_col++) {
                A.setValue(set_rows, set_col, rand()%1000+1);
                B.setValue(set_rows, set_col, rand()%1000+1);
            }
        }
        auto start = chrono::high_resolution_clock::now();
        Matrix C = A.BlockMult(B, blockSize);
        auto end = chrono::high_resolution_clock::now();
        auto duration = end - start;
        auto iMillis = chrono::duration_cast<chrono::milliseconds>(duration);
        cout << "Size of Matrix: " << i << " * " << i << ". Time: " << iMillis.count() << endl;
        blockSize += 10;
    }

    cout << std::endl;
    cout << "--------------------------------------------------------------------------------------------------------";
    cout << std::endl;

    for (int i = 2; i <= 8; i += 2) {
        blockSize = i/2;
        Matrix<double> A(i, i);
        for (int set_rows = 0; set_rows < i; set_rows++) {
            for (int set_col = 0; set_col < i; set_col++) {
                A.setValue(set_rows, set_col, rand()%1000+1);
            }
        }

        try {
            auto start = chrono::high_resolution_clock::now();
            Matrix C = A.Inverse(blockSize);
            auto end = chrono::high_resolution_clock::now();
            auto duration = end - start;
            auto Millis = chrono::duration_cast<chrono::milliseconds>(duration);
            cout << "Size of Matrix: " << i << " * " << i << ". Time: " << Millis.count() << endl;
        }
        catch (...) {
            cout << "Error!" << endl;
        }
        blockSize += 1;
    }

    cout << std::endl;
    cout << "--------------------------------------------------------------------------------------------------------";
    cout << std::endl;
*/
    for (int thread = 1; thread <= 8; thread++) {
        Matrix<double> A(1000, 1000), B(1000, 1000);
        for (int set_rows = 0; set_rows < 1000; set_rows++) {
            for (int set_col = 0; set_col < 1000; set_col++) {
                A.setValue(set_rows, set_col, rand()%1000+1);
                B.setValue(set_rows, set_col, rand()%1000+1);
            }
        }
        auto start = chrono::high_resolution_clock::now();
        Matrix C = A.BlockMult(B, 1000/thread);
        auto end = chrono::high_resolution_clock::now();
        auto duration = end - start;
        auto iMillis = chrono::duration_cast<chrono::milliseconds>(duration);
        cout << "Number of threads: " << thread << ". Time: " << iMillis.count() << endl;
    }

    cout << std::endl;
    cout << "---------------------------------------------------------------------------------------------------------";
    cout << std::endl;
/*
    for (int thread = 1; thread <= 8; thread++) {
        Matrix<double> A(6, 6);
        for (int set_rows = 0; set_rows < 6; set_rows++) {
            for (int set_col = 0; set_col < 6; set_col++) {
                A.setValue(set_rows, set_col, rand()%1000+1);
            }
        }
        auto start = chrono::high_resolution_clock::now();
        Matrix C = A.Inverse(thread);
        auto end = chrono::high_resolution_clock::now();
        auto duration = end - start;
        auto iMillis = chrono::duration_cast<chrono::milliseconds>(duration);
        cout << "Number of threads: " << thread << ". Time: " << iMillis.count() << endl;
    } */
    return 0;
}