
#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>

#include "../gps_path_tools.h"

using namespace gps_path_tools;

// we deem the values to be equal if they are within
// .01 of each other.
static bool almost_equal(double val, double target, double delta) {
    return fabs(target - val) < delta;
}

struct test_point2 {
    double a;
    double target;
    double allowable_delta;
};

struct test_locations {
    location a;
    location b;
    double target;
    double allowable_delta;
};

static bool value_test(double value, double target, double allowable_delta) {
        auto pass = almost_equal(value, target, allowable_delta);

        if (pass) {
            std::cout << "PASS";
        } else {
            std::cout << "FAIL";
        }
        
        std::cout << ", got: " << value << ", target: " << target << ", delta: " << (target - value )<< std::endl;
        
        return pass;
}

static bool test_to_radians() {
    auto passed = true;
    
    std::cout << "Testing to_radians()" << std::endl;
    
    std::vector<test_point2> test_points = {
        {0.0, 0.0, 0.0000001},
        {180.0, M_PI, 0.0000001},
        {360.0, 2 * M_PI, 0.0000001},
        {10.0, 0.174532925, 0.0000001},
        {-10.0, -0.174532925, 0.0000001},
    };
    
    for (const auto& test_point: test_points) {
        auto out = to_radians(test_point.a);
        passed &= value_test(out, test_point.target, test_point.allowable_delta);
    }
    
    return passed;
}

static bool test_to_degrees() {
    auto passed = true;
    
    std::cout << "Testing test_to_degrees()" << std::endl;
    
    std::vector<test_point2> test_points = {
        {0.0, 0.0, 0.0000001},
        {M_PI, 180.0, 0.0000001},
        {2 * M_PI, 360.0, 0.0000001},
        {0.174532925, 10.0, 0.0000001},
        {-0.174532925, -10.0, 0.0000001},
    };
    
    for (const auto& test_point: test_points) {
        auto out = to_degrees(test_point.a);
        passed &= value_test(out, test_point.target, test_point.allowable_delta);
    }
    
    return passed;
}

static bool test_distance_gc() {
    auto passed = true;
    
    std::cout << "Testing distance_gc()" << std::endl;
    
    std::vector<test_locations> test_points = {
        { { 52.9827588546699, -6.040081945988319 }, { 52.9827588546699, -6.040081945988319 }, 0, 1 },
        { { 52.9827588546699, -6.040081945988319 }, { 53.057744464984495, -6.040085910508501}, 8338, 10 },
        { { 52.98277, -6.03927 }, {52.98344, -6.03646}, 202, 10 },
        { { 52.983, -6.03843 }, {52.98277, -6.03927}, 62, 10 },
        { {52.98277, -6.03927}, { 52.983, -6.03843 }, 62, 10 },
        { {-73.08967, -72.2826}, { -74.84766, -61.60389 }, 381000, 500 },
    };
    
    for (const auto& test_point: test_points) {
        auto out = distance_gc(test_point.a, test_point.b);
        passed &= value_test(out, test_point.target, test_point.allowable_delta);
    }
    
    return passed;
}

static bool test_heading_gc() {
    auto passed = true;

    // Setup a list of test points
    std::vector<test_locations> test_points{

        { { 52.981935612371615, -6.0326861216896495 }, { 52.98331462882221, -6.034538892416848}, 321.0316, 0.01 },
        { { 52.98331462882221, -6.034538892416848 }, { 52.981935612371615, -6.0326861216896495}, 141.0316, 0.01 },
        { { 52.98279445287178, -6.039364152945033 }, { 52.983451492393165, -6.036505383661567}, 69.11, 0.01  },
        { { 52.983451492393165, -6.036505383661567 }, { 52.98279445287178, -6.039364152945033}, 249.11, 0.01 },
        { { 52.98289, -6.03937 }, { 52.982794, -6.039364 }, 177.85, 0.01 },
        { { -64.4177000, -56.9970700 }, { -64.42482 , -57.01286 }, 223.748, 0.01 },
        { { -64.4177000,-56.9970700 }, { -64.41772 , -56.9578 }, 90.083, 0.01},
    };
    
    // Loop through each test point.
    for (const auto& test_point: test_points) {
        auto heading = heading_gc(test_point.a, test_point.b);
        passed &= value_test(heading, test_point.target, test_point.allowable_delta);
    }
}

int main() {

    auto tests_passed = true;
    
    tests_passed &= test_to_radians();
    tests_passed &= test_to_degrees();
    tests_passed &= test_distance_gc();
    tests_passed += test_heading_gc();
    
    
    return tests_passed ? 0 : 1;
}
