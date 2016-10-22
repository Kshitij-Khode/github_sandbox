#include <iostream>
#include <vector>
#include <dlib/matrix.h>
#include <dlib/statistics.h>
#include <dlib/matrix/matrix_math_functions.h>
#include <dlib/geometry/vector.h>
#include <dlib/matrix/matrix_la_abstract.h>

using namespace dlib;
using namespace std;

std::vector<string> expn_msg =
{
    "mean_matrix_novectorlib used incorrectly",
    "mean_prediction used incorrectly"
};

void exception_handler(int expn_code)
{
    cout << expn_msg[abs(expn_code)-1] << endl;
    exit(expn_code);
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

matrix<double> mean_prediction
(
    matrix<double> mat_a,
    matrix<double> mat_b,
    matrix<double> mat_c,
    uint no_of_cols
)
{
    matrix<double> mat_mean_a;
    matrix<double> mat_mean_b;
    matrix<double> mat_predict;

    if
    (
        (mat_a.nr() != mat_b.nr()) ||
        (mat_b.nr() != mat_c.nr()) ||
        (mat_c.nr() != mat_a.nr()) ||
        (mat_a.nc() != 3)          ||
        (no_of_cols <= 0)          ||
        (mat_a.nc() < no_of_cols)  ||
        (mat_b.nc() < no_of_cols)  ||
        (mat_c.nc() < no_of_cols)
    )
    {
        return mat_predict;
    }

    mat_mean_a = mean_matrix_novectorlib(mat_a, no_of_cols);
    mat_mean_b = mean_matrix_novectorlib(mat_b, no_of_cols);
    if (mat_mean_a.nc() != mat_c.nc())
    {
        mat_mean_a = join_rows(mat_mean_a, zeros_matrix<double>(mat_mean_a.nc()-mat_c.nc(), 1));
    }
    if (mat_mean_b.nc() != mat_c.nc())
    {
        mat_mean_b = join_rows(mat_mean_b, zeros_matrix<double>(mat_mean_b.nc()-mat_c.nc(), 1));
    }

    mat_predict.set_size(mat_a.nr(), 1);
    for (uint row = 0; row < mat_a.nr(); row++)
    {
        dlib::vector<double> vect_temp_a, vect_temp_b;
        vect_temp_a = rowm(mat_c, row) - rowm(mat_mean_a, row);
        vect_temp_b = rowm(mat_c, row) - rowm(mat_mean_b, row);
        mat_predict(row,0) = vect_temp_a.length_squared() < vect_temp_b.length_squared() ? 1: 2;
    }

    return mat_predict;
}

int main()
{
    matrix<double> mat_c1;
    matrix<double> mat_c2;
    matrix<double> mat_train_c1;
    matrix<double> mat_train_c2;
    uint           col_c;
    double         training_error;

    mat_c1.set_size(10,3);
    mat_c2.set_size(10,3);

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

    col_c = 1;
    training_error = 0.0;
    mat_train_c1 = mean_prediction(mat_c1, mat_c2, mat_c1, col_c);
    mat_train_c2 = mean_prediction(mat_c1, mat_c2, mat_c2, col_c);

    for (int row = 0; row < mat_train_c1.nr(); row++)
    {
        training_error += mat_train_c1(mat_train_c1.nc()-1, row) == 1 ? 1: 0;
        training_error += mat_train_c2(mat_train_c2.nc()-1, row) == 2 ? 1: 0;
    }
    training_error /= mat_train_c1.nr()*2;

    cout << "Answer 1(a):" << endl << "    Training error: " << training_error << endl;

    col_c = 2;
    training_error = 0.0;
    mat_train_c1 = mean_prediction(mat_c1, mat_c2, mat_c1, col_c);
    mat_train_c2 = mean_prediction(mat_c1, mat_c2, mat_c2, col_c);

    for (int row = 0; row < mat_train_c1.nr(); row++)
    {
        training_error += mat_train_c1(mat_train_c1.nc()-1, row) == 1 ? 1: 0;
        training_error += mat_train_c2(mat_train_c2.nc()-1, row) == 2 ? 1: 0;
    }
    training_error /= mat_train_c1.nr()*2;

    cout << "Answer 1(b):" << endl << "    Training error: " << training_error << endl;

    col_c = 3;
    training_error = 0.0;
    mat_train_c1 = mean_prediction(mat_c1, mat_c2, mat_c1, col_c);
    mat_train_c2 = mean_prediction(mat_c1, mat_c2, mat_c2, col_c);

    for (int row = 0; row < mat_train_c1.nr(); row++)
    {
        training_error += mat_train_c1(mat_train_c1.nc()-1, row) == 1 ? 1: 0;
        training_error += mat_train_c2(mat_train_c2.nc()-1, row) == 2 ? 1: 0;
    }
    training_error /= mat_train_c1.nr()*2;

    cout << "Answer 1(c):" << endl << "    Training error: " << training_error << endl;
}