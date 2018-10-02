#pragma once

#include <array>
#include <iostream>
#include <numeric>
#include <vector>

/**
 *
 * @tparam N the number of parameters to tune
 * @tparam RunFn A function to start a run
 * @param dp the parameter intialization
 * @param run the function to call to start a run, must return a double for the total error
 * @param tol the tolerance. Make this tolerance bigger if you are timing out!
 * @return the best parameters
 */
template<int N, class RunFn>
std::array<double, N> twiddle(std::array<double, N> dp, RunFn &&run, float tol = 0.2) {
    // Setup initial parameter array
    std::array<double, N> p;
    std::fill(p.begin(), p.end(), 0.);

    // Initial run
    double best_err = run(p);

    int it = 0;
    while (std::accumulate(dp.begin(), dp.end(), 0.0) > tol) {
        std::cout << "Iteration " << it << " best error = " << best_err << std::endl;
        for (int i = 0; i < N; i++) {
            p[i] += dp[i];

            double err = run(p);

            if (err < best_err) {
                best_err = err;
                dp[i] *= 1.1;
            } else {
                p[i] -= 2 * dp[i];

                err = run(p);

                if (err < best_err) {
                    best_err = err;
                    dp[i] *= 1.1;
                } else {
                    p[i] += dp[i];
                    dp[i] *= 0.9;
                }
            }

        }
        it += 1;
    }
    return p;
}