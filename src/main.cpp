#include <uWS.h>
#include <json.hpp>
#include <PID.hpp>
#include <twiddle.hpp>


#include <iostream>
#include <limits>
#include <math.h>

// for convenience
using json = nlohmann::json;

// For converting back and forth between radians and degrees.
constexpr double pi() { return M_PI; }

double deg2rad(double x) { return x * pi() / 180; }

double rad2deg(double x) { return x * 180 / pi(); }

// Checks if the SocketIO event has JSON data.
// If there is data the JSON object in string format will be returned,
// else the empty string "" will be returned.
std::string hasData(std::string s) {
    auto found_null = s.find("null");
    auto b1 = s.find_first_of("[");
    auto b2 = s.find_last_of("]");
    if (found_null != std::string::npos) {
        return "";
    } else if (b1 != std::string::npos && b2 != std::string::npos) {
        return s.substr(b1, b2 - b1 + 1);
    }
    return "";
}

void reset(uWS::WebSocket<uWS::SERVER> &ws, PID &pid) {
    std::cout << "Reset simulator" << std::endl;
    std::string msg = "42[\"reset\",{}]";
    ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
}

int run(const std::array<double, 3> &p, double &errorOut, int maxRuns = -1) {
    std::cout << "==== Start run with params: " << p[0] << " " << p[1] << " " << p[2] << std::endl;
    uWS::Hub h;

    PID pid;

    bool fresh = false;

    long runNum = 0;
    long thresh = maxRuns / 2;

    h.onMessage([&](uWS::WebSocket<uWS::SERVER> ws, char *data, size_t length,
                    uWS::OpCode opCode) {
        // "42" at the start of the message means there's a websocket message event.
        // The 4 signifies a websocket message
        // The 2 signifies a websocket event
        if (length > 2 && data[0] == '4' && data[1] == '2') {

            // Start fresh
            if (!fresh) {
                // Reset sim
                reset(ws, pid);

                // Reset PID
                pid.init(p[0], p[1], p[2]);

                fresh = true;
                return;
            }

            auto s = hasData(std::string(data).substr(0, length));
            if (s != "") {
                auto j = json::parse(s);
                std::string event = j[0].get<std::string>();
                if (event == "telemetry") {
                    runNum++;

                    // j[1] is the data JSON object
                    double cte = std::stod(j[1]["cte"].get<std::string>());
                    double speed = std::stod(j[1]["speed"].get<std::string>());
                    double angle = std::stod(j[1]["steering_angle"].get<std::string>());

                    // Steering
                    pid.updateError(cte);
                    double steer_value = pid.totalError();

                    // Speed:
                    // - only start actively breaking over 20 mph to avoid going backwards (speed is always positive)
                    // - reduce throttle with greater steering angles over 20 mph to avoid
                    //   losing control
                    double throttle = std::max<double>(speed > 20 ? -1 : 0,
                                                       speed > 20 ? 1 - (speed / 60.0 * std::abs(steer_value) * 5) : 1);

                    // DEBUG
                    if (maxRuns < 0) {
                        std::cout << "CTE: " << cte << " Steering Value: " << steer_value << std::endl;
                    }

                    json msgJson;
                    msgJson["steering_angle"] = steer_value;
                    msgJson["throttle"] = throttle;
                    auto msg = "42[\"steer\"," + msgJson.dump() + "]";

                    // DEBUG
                    if (maxRuns < 0) {
                        std::cout << msg << std::endl;
                    }

                    ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);

                    // Report error if twiddling
                    if (maxRuns > 0 && runNum > thresh) {
                        errorOut += cte * cte;
                    }

                    // End if max runs set and exceeded
                    if (maxRuns > 0 && runNum >= maxRuns) {
                        //End run
                        std::cout << "End of run" << std::endl;
                        h.getDefaultGroup<uWS::SERVER>().close();
                    }
                }
            } else {
                // Manual driving
                std::string msg = "42[\"manual\",{}]";
                ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
            }
        }
    });

    // We don't need this since we're not using HTTP but if it's removed the program
    // doesn't compile :-(
    h.onHttpRequest([](uWS::HttpResponse *res, uWS::HttpRequest req, char *data, size_t, size_t) {
        const std::string s = "<h1>Hello world!</h1>";
        res->end(s.data(), s.length());
    });

    int port = 4567;
    if (h.listen(port)) {
        std::cout << "Listening to port " << port << std::endl;
    } else {
        std::cerr << "Failed to listen to port" << std::endl;
        return -1;
    }
    h.run();
    return 0;
}

int main(int argc, char **argv) {
    const bool runTwiddle = argc > 1;

    double totalError = 0;

    if (runTwiddle) {
        std::cout << "Running twiddle" << std::endl << std::endl;

        // Tweakable; number of steps per run
        int stepsPerRun = 1000;

        auto bestParams = twiddle<3>({.2, .004, 3}, // Initialize with values from course
                                     [&](std::array<double, 3> p) {
                                         totalError = 0;
                                         if (run(p, totalError, stepsPerRun) < 0) {
                                             //Failed
                                             std::cerr << "Run failed" << std::endl;
                                             exit(1);
                                             return std::numeric_limits<double>::max();
                                         } else {
                                             std::cout << "Total error: " << totalError << std::endl;
                                             return totalError;
                                         }
                                     },
                                     0.2 // Tweakable; tolerance
        );

        std::cout << std::endl << "=========" << std::endl;
        std::cout << "Best params: " << bestParams[0] << " " << bestParams[1] << " " << bestParams[2] << std::endl;
    } else {
        // Manually tuned parameters
//        run({.1, .001, 3}, totalError);

        // Twiddle optimized
        run({0.45988, 0.00323188, 14.2266}, totalError);
    }
    std::cout << std::endl << "=========" << std::endl;
    std::cout << std::endl << "Total error: " << totalError << std::endl;
}
