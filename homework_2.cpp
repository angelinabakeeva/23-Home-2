#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;

template <class T>
class Matrix
{
public:
    int rows;
    int columns;
    T** matrix;

    Matrix();
    Matrix(int, int);
    Matrix(const Matrix&);
    Matrix(const char* fileName);
    ~Matrix();

    void readFromConsole();
    void readFromConsoleWithSize(int, int);
    void printToConsole() const;
    static Matrix<T> getZeroMatrix(int ROWS, int COLUMNS);
    static Matrix<T> getIdentityMatrix(int ROWS, int COLUMNS);
    void readFromFile(const char*);
    void writeToFile(const char*);

    Matrix& Multiply(const T);

    friend Matrix operator* (const Matrix&, const Matrix&);
    friend Matrix operator* (const Matrix&, const T);
    friend Matrix operator* (const T, const Matrix&);

    friend Matrix operator+ (const Matrix&, const Matrix&);
    friend Matrix operator- (const Matrix&, const Matrix&);

    friend bool operator== (const Matrix&, const Matrix&);
    friend bool operator!= (const Matrix&, const Matrix&);

    friend Matrix operator! (const Matrix&);

};

template <class T>
Matrix<T>::Matrix()
{
    rows = 0;
    columns = 0;
    matrix = NULL;
}

template <class T>
Matrix<T>::Matrix(int ROWS, int COLUMNS)
{
    if (ROWS <= 0 || COLUMNS <= 0)
    {
        cerr << "Rows and columns can't be less or equal than zero" << endl;
        throw invalid_argument("");
    }

    matrix = new T * [ROWS];
    rows = ROWS;
    columns = COLUMNS;
    for (int r = 0; r < ROWS; r++)
    {
        matrix[r] = new T[COLUMNS];
        for (int c = 0; c < COLUMNS; c++)
        {
            matrix[r][c] = 0;
        }
    }
}

template <class T>
Matrix<T>::Matrix(const Matrix& other)
{
    rows = other.rows;
    columns = other.columns;
    matrix = new T* [other.rows];
    for (int r = 0; other.rows > r; r++) {
        matrix[r] = new T[other.columns];

        for (int c = 0; c < other.columns; c++)
        {
            matrix[r][c] = other.matrix[r][c];
        }
    }
}

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
Matrix<T>::~Matrix()
{
    for (int r = 0; r < rows; r++)
    {
        delete matrix[r];
    }
    delete matrix; //delete[]
    matrix = NULL;
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
    matrix = new T*[rows];
    for (int r = 0; rows > r; r++)
    {
        matrix[r] = new T[columns];
        for (int c = 0; c < columns; c++)
        {
            cout << "Print element on this position (row - " << r + 1 << " column - " << c + 1 << "): " << endl;
            cin >> matrix[r][c];
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
Matrix<T>& Matrix<T>::Multiply(const T value)
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
Matrix<T> operator*(const Matrix<T>& a, const Matrix<T>& b)
{
    // столбцы 1-ой и строки 2-ой
    if (a.columns == b.rows)
    {
        Matrix<T> result(a.rows, b.columns);
        for (int r = 0; r < a.rows; r++)
        {
            for (int c_result = 0; c_result < b.columns; c_result++)
            {
                for (int c = 0; c < a.columns; c++)
                {
                    result.matrix[r][c_result] += a.matrix[r][c] * b.matrix[c][c_result];
                }
            }
        }
        return result;
    }
    else
    {
        cerr << "ERROR WITH MULTIPLYING" << endl;
    }

    // вряд ли до сюда дойдет, но пусть будет
    return Matrix<T>();
}

template <class T>
Matrix<T> operator* (const Matrix<T>& a, const T b)
{
    Matrix<T> res = a;
    res.Multiply(b);
    return res;
}

template <class T>
Matrix<T> operator* (const T b, const Matrix<T>& a)
{
    Matrix<T> res = a;
    res.Multiply(b);
    return res;
}

template <class T>
Matrix<T> operator+(const Matrix<T>& a, const Matrix<T>& b)
{
    if (a.rows != b.rows || a.columns != b.columns)
    {
        cerr << "ERROR WITH ADDITION" << endl;
        throw invalid_argument("");
    }

    // столбцы равны столбцам, строки - строкам

    Matrix<T> res(a.rows, a.columns);

    for (int r = 0; r < a.rows; r++)
    {
        for (int c = 0; c < a.columns; c++)
        {
            res.matrix[r][c] = a.matrix[r][c] + b.matrix[r][c];
        }
    }
    return res;

    // вряд ли до сюда дойдет, но пусть будет
    //return Matrix<T>();!!!
}

template <class T>
Matrix<T> operator-(const Matrix<T>& a, const Matrix<T>& b)
{
    // столбцы равны столбцам, строки - строкам
    if (a.rows == b.rows && a.columns == b.columns)
    {
        Matrix<T> res(a.rows, a.columns);

        for (int r = 0; r < a.rows; r++)
        {
            for (int c = 0; c < a.columns; c++)
            {
                res.matrix[r][c] = a.matrix[r][c] - b.matrix[r][c];
            }
        }
        return res;
    }
    else
    {
        cerr << "ERROR WITH SUBSTRACTION" << endl;
    }

    // вряд ли до сюда дойдет, но пусть будет
    return Matrix<T>();
}

template <class T>
bool operator==(const Matrix<T>& a, const Matrix<T>& b)
{
    if (a.rows != b.rows || a.columns != b.columns)
    {
        return false;
    }

    bool flag = true;
    for (int r = 0; r < a.rows; r++)
    {
        for (int c = 0; c < a.columns; c++)
        {
            if (a.matrix[r][c] != b.matrix[r][c])
            {
                flag = false;
                return flag;
            }
        }
    }
    return flag;
}

template <class T>
bool operator!=(const Matrix<T>& a, const Matrix<T>& b)
{
    if (a.rows != b.rows || a.columns != b.columns)
    {
        return true;
    }

    bool flag = false;
    for (int r = 0; r < a.rows; r++)
    {
        for (int c = 0; c < a.columns; c++)
        {
            if (a.matrix[r][c] != b.matrix[r][c])
            {
                flag = true;
                return flag;
            }
        }
    }
    return flag;
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

    for (int c = 0; c < a.columns; c++)
    {
        Matrix<T> mnr(a.rows - 1, a.columns - 1);
        for (int r = 1; r < a.rows; r++)
        {
            int k = 0;
            for (int l = 0; l < a.columns; l++)
            {
                if (l != c)
                {
                    mnr.matrix[r - 1][k] = a.matrix[r][l];
                    k++;
                }
            }
        }
        // Умножаем элемент первой строки на определитель подматрицы
        det += sign * a.matrix[0][c] * determinant(mnr);
        sign = -sign;
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

    for (int r = 0; r < a.rows; r++)
    {
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
    }

    Matrix<T> adjoint(a.rows, a.columns);
    for (int r = 0; r < a.rows; r++)
    {
        for (int c = 0; c < a.columns; c++)
        {
            adjoint.matrix[c][r] = another.matrix[r][c];
        }
    }

    for (int r = 0; r < a.rows; r++)
    {
        for (int c = 0; c < a.columns; c++)
        {
            result.matrix[r][c] = adjoint.matrix[r][c] / det;
        }
    }

    return result;
}

template <class T>
Matrix<T> Matrix<T>::getZeroMatrix(int ROWS, int COLUMNS) //Matrix<T>::Matrix(int ROWS, int COLUMNS)
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
            matrix[r][c] = 0;
        }
    }
    return matrix;
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
    int rows, columns;
    cout << "Print the number of rows: " << endl;
    cin >> rows;
    cout << "Print the number of columns: " << endl;
    cin >> columns;
    return 0;
}