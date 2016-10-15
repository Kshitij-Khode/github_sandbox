#include <iostream>
#include <vector>
#include <cmath>
#include <dlib/matrix.h>
#include <dlib/statistics.h>

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
    matrix<double> mat_temp;

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

    // Print Answer
    cout << "Solving equation 2(a):" << endl << "    Coefficients: " << mat_mconst;

    mat_mconst.set_size(3,1);
    mat_mconst = inv(trans(mat_t) * mat_t) * trans(mat_t) * mat_s;

    double avg_sq_error = avg_sq_error_novectorlib(trans(mat_s), trans(mat_mconst)*trans(mat_t));
    if (avg_sq_error < 0) exception_handler(-1);

    // Print Answer and format answer for readability
    cout << "Answer 2(a,c):" << endl
         << "    Coefficients: "
         << trans(mat_mconst)
         << "    R: "
         << avg_sq_error << endl;

    double lambda = 0.5;
    mat_mconst    =  inv(trans(mat_t)*mat_t + lambda*identity_matrix<double>(6))
                  *  trans(mat_t)
                  *  mat_s;

    avg_sq_error  = avg_sq_error_novectorlib(trans(mat_s), trans(mat_mconst)*trans(mat_t));
    if (avg_sq_error < 0) exception_handler(-1);

    // Print Answer and format answer for readability
    cout << "Answer 2(b,c):" << endl
         << "    Coefficients: "
         << trans(mat_mconst)
         << "    R: "
         << avg_sq_error << endl;
}