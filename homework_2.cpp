#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;

template<typename T>
class Matrix
{
public:
    int rows;
    int columns;
    T** matrix;

    Matrix() {
        rows = 0;
        columns = 0;
        matrix = NULL;
    }

    Matrix(int ROWS, int COLUMNS) {
        matrix = NULL;
        if (ROWS > 0 && COLUMNS > 0)
        {
            rows = ROWS;
            columns = COLUMNS;

            matrix = new T*[rows];
            for (int r = 0; r < rows; r++)
            {
                matrix[r] = new T[columns];
                for (int c = 0; c < columns; c++)
                    matrix[r][c] = 0;
            }
        }
    }

    Matrix(const Matrix &other) {
        rows = other.rows;
        columns = other.columns;
        matrix = new T *[other.rows];
        for (int r = 0; other.rows > r; r++) {
            matrix[r] = new T[other.columns];

            for (int c = 0; c < other.columns; c++)
                matrix[r][c] = other.matrix[r][c];
        }
    }

    ~Matrix() {
        for (int r = 0; r < rows; r++)
        {
            delete matrix[r];
        }
        delete matrix;
        matrix = NULL;
    }

    void readFromConsole() const {
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < columns; j++) {
                cout << "Print element on this position (row - " << i + 1 << " column - " << j + 1 << "): "<< endl;
                cin >> matrix[i][j];
            }
        }
    }

    void printToConsole() const {
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < columns; j++) {
                cout << matrix[i][j] << " ";
            }
            cout << endl;
        }
        cout << endl;
    };

    void readFromFile(const char* fileName) const {
        ifstream inputFile(fileName);
        if (!inputFile) {
            cerr << "Error opening file." << endl;
            return;
        }
        int strf, stlf;
        inputFile >> strf >> stlf;
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < columns; j++) {
                inputFile >> matrix[i][j];
            }
        }
        inputFile.close();
    }

    void writeToFile(const char* fileName) const {
        ofstream outputFile(fileName);
        if (!outputFile) {
            cerr << "Error opening file." << endl;
            return;
        }
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < columns; j++) {
                outputFile << matrix[i][j] << " ";
            }
            outputFile << endl;
        }
        outputFile.close();
    }

    Matrix& Multiply(const double value) {
        for (int r = 0; r < rows; r++)
            for (int c = 0; c < columns; c++)
                matrix[r][c] *= value;
        return *this;
    }


    friend Matrix operator*(const Matrix &a, const Matrix &b) {
        // столбцы 1-ой и строки 2-ой
        if (a.columns == b.rows)
        {
            Matrix result(a.rows, b.columns);
            for (int r = 0; r < a.rows; r++)
                for (int c_result = 0; c_result < b.columns; c_result++)
                    for (int c = 0; c < a.columns; c++)
                        result.matrix[r][c_result] += a.matrix[r][c] * b.matrix[c][c_result];
            return result;
        }
        else
            cerr << "ERROR WITH MULTIPLYING" << endl;

        // вряд ли до сюда дойдет, но пусть будет
        return Matrix();
    }

    friend Matrix operator* (const Matrix& a, const double b)
    {
        Matrix res = a;
        res.Multiply(b);
        return res;
    }

    friend Matrix operator* (const double b, const Matrix& a)
    {
        Matrix res = a;
        res.Multiply(b);
        return res;
    }

    friend Matrix operator+(const Matrix& a, const Matrix& b)
    {
        // столбцы равны столбцам, строки - строкам
        if (a.rows == b.rows && a.columns == b.columns)
        {
            Matrix res(a.rows, a.columns);

            for (int r = 0; r < a.rows; r++)
                for (int c = 0; c < a.columns; c++)
                    res.matrix[r][c] = a.matrix[r][c] + b.matrix[r][c];
            return res;
        }
        else
            cerr << "ERROR WITH ADDITION" << endl;

        // вряд ли до сюда дойдет, но пусть будет
        return Matrix();
    }

    static double determinant(const Matrix& a) {
        if (a.rows != a.columns) {
            cerr << "MATRIX SHOULD BE SQUARED" << endl;
        }

        if (a.rows == 1) {
            return a.matrix[0][0];
        }

        double det = 0;
        int sign = 1;


        for (int c = 0; c < a.columns; c++) {
            Matrix mnr(a.rows - 1, a.columns - 1);
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
        }

        return det;
    }

    static void firstType(Matrix& a, int row1, int row2) {
        // проверяем, что номера строк находятся в допустимом диапазоне
        if (row1 < 0 || row1 > a.rows || row2 < 0 || row2 > a.rows) {
            cerr << "ERROR ..." << endl;
        }

        // свап ????
        T* temp = a.matrix[row1];
        a.matrix[row1] = a.matrix[row2];
        a.matrix[row2] = temp;
    }

    static void secondType(Matrix& a, int row, T alpha) {
        // проверяем, что номер строки находится в допустимом диапазоне
        if (row > a.rows || row < 0) {
            cerr << "ERROR ... x2" << endl;
        }
        // умножаем каждый элемент строки на число
        for (int c = 0; c < a.columns; c++) {
            a.matrix[row][c] *= alpha;
        }
    }

    static void thirdType(Matrix& a, int row_m, int row_c, T alpha)
    {
        // проверяем, что номера строк находятся в допустимом диапазоне
        if (row_m < 0 || row_m > a.rows || row_c < 0 || row_c > a.rows) {
            cerr << "ERROR ... x3" << endl;
        }
        // прибавляем к одной строке другую, умноженную на число
        for (int c = 0; c < a.columns; c++)
        {
            a.matrix[row_c][c] += (a.matrix[row_m][c] * alpha);
        }
    }

    friend Matrix operator-(const Matrix& a, const Matrix& b) {
        // столбцы равны столбцам, строки - строкам
        if (a.rows == b.rows && a.columns == b.columns)
        {
            Matrix res(a.rows, a.columns);

            for (int r = 0; r < a.rows; r++)
                for (int c = 0; c < a.columns; c++)
                    res.matrix[r][c] = a.matrix[r][c] - b.matrix[r][c];
            return res;
        }
        else
            cerr << "ERROR WITH SUBSTRACTION" << endl;

        // вряд ли до сюда дойдет, но пусть будет
        return Matrix();
    }

    friend bool operator==(const Matrix &a, const Matrix &b) {
        if (a.rows != b.rows || a.columns != b.columns)
        {
            return false;
        }
        else
        {
            bool flag = true;
            for (int r = 0; r < a.rows; r++)
                for (int c = 0; c < a.columns; c++)
                    if (a.matrix[r][c] != b.matrix[r][c])
                        flag = false;
            return flag;
        }
    }

    friend bool operator!=(const Matrix &a, const Matrix &b) {
        if (a.rows != b.rows || a.columns != b.columns)
        {
            return true;
        }
        else
        {
            bool flag = false;
            for (int r = 0; r < a.rows; r++)
                for (int c = 0; c < a.columns; c++)
                    if (a.matrix[r][c] != b.matrix[r][c])
                        flag = true;
            return flag;
        }
    }

    friend Matrix operator!(const Matrix& a)
    {
        if (a.rows != a.columns)
        {
            cerr << "MATRIX SHOULD BE SQUARED" << endl;
        }
        Matrix result(a.rows, a.columns);
        double det = determinant(a);
        if (det == 0)
        {
            cerr << "MATRIX IS NOT INVERSE" << endl;
        }

        Matrix another(a.rows, a.columns);

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

                double cof = pow(-1, r + c) * determinant(minor);
                another.matrix[r][c] = cof;
            }
        }

        Matrix adjoint(a.rows, a.columns);
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
};


int main() {
    int strc, stlc;
    cout << "Print the number of rows: " << endl;
    cin >> strc;
    cout << "Print the number of columns: " << endl;
    cin >> stlc;
    Matrix<int> A(strc, stlc);
    A.readFromConsole();
    A.printToConsole();
    /*int strf, stlf;
    ifstream inputFile("example.txt");
    inputFile >> strf >> stlf;
    Matrix B(strf, stlf);
    B.readFromFile("example.txt");
    B.writeToFile("test.txt");
    Matrix C = A * B;
    cout << "A * B = \n";
    C.printToConsole();
    double scalar1;
    cout << "Print the scalar for multiplication (scalar will be on the right side): \n";
    cin >> scalar1;
    Matrix D = A * scalar1;
    cout << "A * " << scalar1 << " = \n";
    D.printToConsole();
    double scalar2;
    cout << "Print the scalar for multiplication (scalar will be on the left side): \n";
    cin >> scalar2;
    Matrix E = scalar2 * B;
    cout << scalar2 << " * B "<< " = \n";
    E.printToConsole();
    Matrix S = A + B;
    cout << "The addition of two matrices: \n";
    S.printToConsole();
    Matrix R = A - B;
    cout << "The subtraction of two matrices: \n";
    R.printToConsole();
    if (A == B)
    {
        cout << "1. Equal" << endl;
    }
    else
    {
        cout << "1. Different" << endl;
    }

    if (A != B)
    {
        cout << "2. Different" << endl;
    }
    else
    {
        cout << "2. Equal" << endl;
    }
    int row_s1, row_s2;
    cout << "Print indexes of swapping rows: " << endl;
    cin >> row_s1 >> row_s2;
    firstType(A, row_s1 - 1, row_s2 - 1);
    A.printToConsole();
    int row_chng;
    double number;
    cout << "Print index of changing row: " << endl;
    cin >> row_chng;
    cout << "Print the alpha (the number by which we multiply): " << endl;
    cin >> number;
    secondType(A, row_chng - 1, number);
    A.printToConsole();
    int row_mlted;
    cout << "Print index of changing row: " << endl;
    cin >> row_mlted;
    int row_mlting;
    cout << "Print index of multiplying row: " << endl;
    cin >> row_mlting;
    double cipher;
    cout << "Print the cipher (the number by which we multiply): " << endl;
    cin >> cipher;
    A.printToConsole();
    thirdType(A, row_mlting - 1, row_mlted - 1, cipher);
    A.printToConsole();
    */
    Matrix Inv = !A;
    Inv.printToConsole();
    return 0;
}