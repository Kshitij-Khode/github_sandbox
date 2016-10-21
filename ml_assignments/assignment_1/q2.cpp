#include <iostream>
#include <vector>
#include <cmath>
#include <dlib/matrix.h>
#include <dlib/statistics.h>
#include <dlib/matrix/matrix_la_abstract.h>

using namespace dlib;
using namespace std;

std::vector<string> expn_msg =
{
    "avg_sq_error_novectorlib used incorrectly"
};

void exception_handler(int expn_code)
{
    cout << expn_msg[abs(expn_code)+1] << endl;
    exit(expn_code);
}

double avg_sq_error_novectorlib(matrix<double> mat_a, matrix<double> mat_b)
{
    if (mat_a.nr() != 1 || (mat_a.nc() != mat_b.nc())) return -1;

    matrix<double> mat_sq_err;
    mat_sq_err.set_size(1, mat_a.nc());
    mat_sq_err = mat_a - mat_b;
    mat_sq_err = scale_columns(mat_sq_err, mat_sq_err);
    double avg_sq_error = 0;
    for(int column = 0; column < mat_sq_err.nc(); column++)
    {
        avg_sq_error += mat_sq_err(0,column);
    }
    return avg_sq_error / mat_sq_err.nc();
}

int main()
{
    matrix<double> mat_t;
    matrix<double> mat_s;
    matrix<double> mat_mconst;
    matrix<double> mat_rterm;
    matrix<double> mat_rterm0;
    matrix<double> mat_rterm1;
    matrix<double> mat_rterm0_eigen;
    matrix<double> mat_rterm1_eigen;
    matrix<double> mat_temp;
    matrix<double> mat_temp_eigen;
    double         avg_sq_error;

    mat_t.set_size(6,1);
    mat_t = 5,
            10,
            15,
            20,
            25,
            30;

    mat_temp.set_size(6,3);
    mat_temp = join_rows(join_rows(ones_matrix<double>(6,1), mat_t), scale_rows(mat_t, mat_t));
    mat_t.set_size(6,3);
    mat_t    = mat_temp;

    mat_s.set_size(6,1);
    mat_s = 722,
            1073,
            1178,
            1177,
            781,
            102;

    mat_mconst.set_size(1,3);
    mat_mconst = trans(mat_s) * inv(trans(mat_t));

    cout << "Solving equation 2(a):" << endl << "    Coefficients: " << mat_mconst << endl;

    mat_mconst.set_size(3,1);
    mat_mconst = inv(trans(mat_t) * mat_t) * trans(mat_t) * mat_s;

    avg_sq_error = avg_sq_error_novectorlib(trans(mat_s), trans(mat_mconst)*trans(mat_t));
    if (avg_sq_error < 0) exception_handler(-1);

    cout << "Answer 2(a,c):" << endl
         << "    Coefficients: "
         << trans(mat_mconst)
         << "    R: "
         << avg_sq_error << endl << endl;

    mat_mconst.set_size(3,1);
    mat_rterm0.set_size(6,6);
    mat_rterm1.set_size(6,6);
    mat_temp  .set_size(6,6);
    mat_rterm .set_size(6,6);
    mat_rterm0 = trans(mat_t)*mat_t;
    mat_rterm1 = 0.5*identity_matrix<double>(3);

    mat_temp   = mat_rterm0 + mat_rterm1;
    mat_rterm  = inv(mat_temp);

    mat_mconst = mat_rterm
               * trans(mat_t)
               * mat_s;

    avg_sq_error  = avg_sq_error_novectorlib(trans(mat_s), trans(mat_mconst)*trans(mat_t));
    if (avg_sq_error < 0) exception_handler(-1);

    cout << "Answer 2(b,c):" << endl
         << "    Coefficients: "
         << trans(mat_mconst)
         << "    R: "
         << avg_sq_error << endl << endl;

    mat_temp_eigen  .set_size(3,1);
    mat_rterm0_eigen.set_size(3,1);
    mat_rterm1_eigen.set_size(3,1);
    mat_temp_eigen   = real_eigenvalues(mat_temp);
    mat_rterm0_eigen = real_eigenvalues(mat_rterm0);
    mat_rterm1_eigen = real_eigenvalues(mat_rterm1);

    cout << "Answer 2(d): Since all eigen values of the following matrices are non-zero, all of them are full rank"
         << endl << "Eigen values of Xt*X - lambda*I:"  << endl
         << mat_rterm0_eigen << "Eigen values of XtX:" << endl
         << mat_rterm1_eigen << "Eigen values of lambda*I:" << endl
         << mat_temp_eigen << endl;

    mat_rterm0.set_size(2,2);
    mat_rterm1.set_size(2,2);
    mat_temp  .set_size(2,2);
    mat_rterm0 =  1, -1,
                 -1,  1;
    mat_rterm1 = 0.5*identity_matrix<double>(2);
    mat_temp   = mat_rterm0 + mat_rterm1;

    mat_temp_eigen   = real_eigenvalues(mat_temp);
    mat_rterm0_eigen = real_eigenvalues(mat_rterm0);
    mat_rterm1_eigen = real_eigenvalues(mat_rterm1);

    cout << "Answer 2(d): Now Xt*X = [1 -1; -1 1] is not full-rank (non-invertible) since some of it's eigen values are"
         << "0. However Xt*X - lambda*I is full-rank (invertible) because all of it's eigen values are non-zero."
         << endl << "Eigen values of Xt*X - lambda*I:"  << endl
         << mat_temp_eigen << "Eigen values of XtX:" << endl
         << mat_rterm0_eigen << "Eigen values of lambda*I:" << endl
         << mat_rterm1_eigen << endl;
}