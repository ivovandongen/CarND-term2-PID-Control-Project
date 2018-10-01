#include "PID.hpp"

#include <iostream>

PID::PID() = default;

PID::~PID() = default;

void PID::init(double Kp_, double Ki_, double Kd_) {
    // Set the params
    Kp = Kp_;
    Ki = Ki_;
    Kd = Kd_;

    // Initialize the errors
    p_error = i_error = d_error = 0;
}

void PID::updateError(double cte) {
    d_error = cte - p_error;
    p_error = cte;
    i_error += cte;
}

double PID::totalError() {
//    std::cout << "P: " << -Kp * p_error << std::endl;
//    std::cout << "D: " << -Kd * d_error << std::endl;
//    std::cout << "I: " << -Ki * i_error << std::endl;
    return -Kp * p_error - Kd * d_error - Ki * i_error;
}

