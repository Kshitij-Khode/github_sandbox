#include <iostream>
#include <vector>
#include <dlib/matrix.h>
#include <dlib/statistics.h>
#include <dlib/matrix/matrix_math_functions.h>

using namespace dlib;
using namespace std;

std::vector<string> expn_msg =
{
    "mean_matrix_novectorlib used incorrectly"
};

void exception_handler(int expn_code)
{
    cout << expn_msg[abs(expn_code)+1] << endl;
    exit(expn_code);
}

matrix<double> mean_prediction(matrix<double> mat_a, matrix<double> mat_b)
{
    matrix<double>        mat_mean;
    running_stats<double> rs;

    if (no_of_cols <= 0 || no_of_cols > mat_a.nc()) return mat_mean;

    mat_mean.set_size(1, no_of_cols);
    for (int col = 0; col < no_of_cols; col++)
    {
        for (int row = 0; row < mat_a.nr(); row++) { rs.add(mat_a(row, col)); }
        mat_mean(0, col) = rs.mean();
        rs.clear();
    }

    return mat_mean;
}

matrix<double> mean_matrix_novectorlib(matrix<double> mat_a, int no_of_cols)
{
    matrix<double>        mat_mean;
    running_stats<double> rs;

    if (no_of_cols <= 0 || no_of_cols > mat_a.nc()) return mat_mean;

    mat_mean.set_size(1, no_of_cols);
    for (int col = 0; col < no_of_cols; col++)
    {
        for (int row = 0; row < mat_a.nr(); row++) { rs.add(mat_a(row, col)); }
        mat_mean(0, col) = rs.mean();
        rs.clear();
    }

    return mat_mean;
}

int main()
{
    matrix<double> mat_c1;
    matrix<double> mat_c2;
    matrix<double> mat_mean_c1;
    matrix<double> mat_mean_c2;

    mat_c1.set_size(10,3);
    mat_c2.set_size(10,3);
    mat_mean_c1.set_size(1,3);
    mat_mean_c2.set_size(1,3);

    mat_c1 = -5.01, -8.12, -3.68,
             -5.43, -3.48, -3.54,
              1.08, -5.52,  1.66,
              0.86, -3.78, -4.11,
             -2.67,  0.63,  7.39,
              4.94,  3.29,  2.08,
             -2.51,  2.09, -2.59,
             -2.25, -2.13, -6.94,
              5.56,  2.86, -2.26,
              1.03, -3.33,  4.33;

    mat_c2 = -0.91, -0.18, -0.05,
              1.30, -2.06, -3.53,
             -7.75, -4.54, -0.95,
             -5.47,  0.50,  3.92,
              6.14,  5.72, -4.85,
              3.60,  1.26,  4.36,
              5.37, -4.63, -3.65,
              7.18,  1.46, -6.66,
             -7.39,  1.17,  6.30,
             -7.50, -6.32, -0.31;

    mat_mean_c1 = mean_matrix_novectorlib(mat_c1, 1);
    mat_mean_c2 = mean_matrix_novectorlib(mat_c2, 1);
    if (mat_mean_c1 == NULL) exception_handler(-1);
    if (mat_mean_c2 == NULL) exception_handler(-1);


    // std::vector<int> x(mat_c1.begin(), mat_c1.end());
    // for (std::vector<int>::const_iterator i = x.begin(); i != x.end(); ++i)
    // {
    //     std::cout << *i << ' ';
    // }
}