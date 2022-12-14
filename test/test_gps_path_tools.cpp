
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

static bool value_test(int value, int target) {
        auto pass = value == target;

        if (pass) {
            std::cout << "PASS";
        } else {
            std::cout << "FAIL";
        }
        
        std::cout << ", got: " << value << ", target: " << target << ", delta: " << (target - value )<< std::endl;
        
        return pass;
}

static bool value_test(const std::string& value, const std::string& target) {
        auto pass = value == target;

        if (pass) {
            std::cout << "PASS";
        } else {
            std::cout << "FAIL";
        }
        
        std::cout << ", got: " << value << ", target: " << target << std::endl;
        
        return pass;
}

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

static bool iterator_test(const path::iterator value, path::iterator target, path::iterator begin) {
        auto pass = value == target;

        if (pass) {
            std::cout << "PASS";
        } else {
            std::cout << "FAIL";
        }
        
        std::cout << ", got: " << std::distance(begin, value) << ", target: " << std::distance(begin, value) << std::endl;
        
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

static bool test_ddm_to_dd() {
    auto passed = true;
    
    std::cout << "Testing ddm_to_dd()" << std::endl;
    
    std::vector<test_point2> test_points = {
        {0.0, 0.0, 0.0000001},
        { 5258.9655312801942, 52.9827588546699, 0.0000001},
        { 0.9655312801942, 0.9655312801942 / 60.0, 0.0000001},
        { 1.9655312801942, 1.9655312801942 / 60.0, 0.0000001},
        { 10.9655312801942, 10.9655312801942 / 60.0, 0.0000001},
        { 110.9655312801942, 1 + 10.9655312801942 / 60.0, 0.0000001},
        { 1110.9655312801942, 11 + 10.9655312801942 / 60.0, 0.0000001},
        { 11110.9655312801942, 111 + 10.9655312801942 / 60.0, 0.0000001},
        { 11100.9655312801942, 111 + 0.9655312801942 / 60.0, 0.0000001},
        { 7730.5, 77.508333, 0.00001},
    };
    
    for (const auto& test_point: test_points) {
        auto out = ddm_to_dd(test_point.a);
        passed &= value_test(out, test_point.target, test_point.allowable_delta);
    }
    
    return passed;
}

static bool test_dd_to_ddm() {
    auto passed = true;
    
    std::cout << "Testing dd_to_ddm()" << std::endl;
    
    std::vector<test_point2> test_points = {
        {0.0, 0.0, 0.0000001},
        { 52.9827588546699, 5258.9655312801942, 0.0000001},
        { 0.9655312801942 / 60.0, 0.9655312801942, 0.0000001},
        { 1.9655312801942 / 60.0, 1.9655312801942, 0.0000001},
        { 10.9655312801942 / 60.0, 10.9655312801942, 0.0000001},
        { 1 + 10.9655312801942 / 60.0, 110.9655312801942, 0.0000001},
        { 11 + 10.9655312801942 / 60.0, 1110.9655312801942, 0.0000001},
        { 111 + 10.9655312801942 / 60.0, 11110.9655312801942, 0.0000001},
        { 111 + 0.9655312801942 / 60.0, 11100.9655312801942, 0.0000001},
        { 77.508333, 7730.5, 0.0001},
    };
    
    for (const auto& test_point: test_points) {
        auto out = dd_to_ddm(test_point.a);
        passed &= value_test(out, test_point.target, test_point.allowable_delta);
    }
    
    return passed;
}

static bool test_str_to_time_utc() {
    auto passed = true;
    
    std::cout << "Testing str_to_time_utc()" << std::endl;
    
    std::vector<std::pair<const char*, const char*>> tests = {
        { "2022-05-07T15:43:15.999999Z", "2022-05-07T15:43:15.999999Z" },
        { "2022-05-07T15:43:15.000000Z", "2022-05-07T15:43:15.000000Z" },
        { "2022-05-07T15:43:15.000001Z", "2022-05-07T15:43:15.000001Z" },
        { "2022-05-07T15:43:15.001Z", "2022-05-07T15:43:15.001000Z" },
        { "2022-05-07T15:43:15Z", "2022-05-07T15:43:15.000000Z" },
        { "2022-05-07T15:43:15.100015", "2022-05-07T15:43:15.100015Z" },
    };
    
    for (const auto& test : tests) {
        auto time = str_to_time_utc(test.first);
        auto out = time_to_str_utc(time);
        
        passed &= value_test(out, test.second);   
    }
    
       
    return passed;
}



static bool test_distance_vec() {
    auto passed = true;
    
    std::cout << "Testing distance_vec()" << std::endl;
    
    std::vector<test_locations> test_points = {
        { { 52.9827588546699, -6.040081945988319 }, { 52.9827588546699, -6.040081945988319 }, 0, 1 },
        { { 52.9827588546699, -6.040081945988319 }, { 53.057744464984495, -6.040085910508501}, 8338, 10 },
        { { 52.98277, -6.03927 }, {52.98344, -6.03646}, 202, 10 },
        { { 52.983, -6.03843 }, {52.98277, -6.03927}, 62, 10 },
        { {52.98277, -6.03927}, { 52.983, -6.03843 }, 62, 10 },
        { {-73.08967, -72.2826}, { -74.84766, -61.60389 }, 381000, 500 },
//            { { 54.044476 -7.378181 } , {54.044479 -7.378173 }, 3, 1},
    };
    
    for (const auto& test_point: test_points) {
        auto out = distance_vec(test_point.a, test_point.b);
        passed &= value_test(out, test_point.target, test_point.allowable_delta);
    }
    
    return passed;
}

static bool test_distance() {
    auto passed = true;
    
    std::cout << "Testing distance()" << std::endl;
    
    std::vector<test_locations> test_points = {
        { { 52.9827588546699, -6.040081945988319 }, { 52.9827588546699, -6.040081945988319 }, 0, 1 },
        { { 52.9827588546699, -6.040081945988319 }, { 53.057744464984495, -6.040085910508501}, 8338, 10 },
        { { 52.98277, -6.03927 }, {52.98344, -6.03646}, 202, 10 },
        { { 52.983, -6.03843 }, {52.98277, -6.03927}, 62, 10 },
        { {52.98277, -6.03927}, { 52.983, -6.03843 }, 62, 10 },
        { {-73.08967, -72.2826}, { -74.84766, -61.60389 }, 381000, 500 },
//            { { 54.044476 -7.378181 } , {54.044479 -7.378173 }, 3, 1},
    };
    
    for (const auto& test_point: test_points) {
        auto out = distance(test_point.a, test_point.b);
        passed &= value_test(out, test_point.target, test_point.allowable_delta);
    }
    
    return passed;
}


static bool test_heading() {
    auto passed = true;

    std::cout << "Testing heading()" << std::endl;

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
        auto h = heading(test_point.a, test_point.b);
        passed &= value_test(h, test_point.target, test_point.allowable_delta);
    }
}

static bool test_path_distance() {
    auto passed = true;
    
    std::cout << "Testing path_distance()" << std::endl;
    std::vector<path_point> path;
    
    // Test empty path
    auto out = path_distance(path.begin(), path.end());
    passed &= value_test(out, 0, 0.0000001);
        
    // Test invalid path with one point
    path = {
        { {52.981935612371615, -6.0326861216896495} },
    };

    out = path_distance(path.begin(), path.end());
    passed &= value_test(out, 0, 0.0000001);

    path = {
        { { 52.9827588546699, -6.040081945988319 } }, 
        { { 53.057744464984495, -6.040085910508501 } },
        { { 52.9827588546699, -6.040081945988319 } }, 
    };

    out = path_distance(path.begin(), path.end());
    passed &= value_test(out, 8338 * 2, 0.1);
    
    return passed;
}

static bool test_path_distance_on_gpx() {
    auto passed = true;
    
    std::cout << "Testing test_path_distance_on_gpx()" << std::endl;
    auto path = load_gpx_qd("table_mountain_loop.gpx");

    auto out = path_distance(path.begin(), path.end());

    passed &= value_test(out, 8338 * 2, 0.1);
    
    return passed;
}

static bool test_path_cumulative_distance() {
    auto passed = true;
    
    std::cout << "Testing path_cumulative_distance()" << std::endl;
    auto path = load_gpx_qd("table_mountain_loop.gpx");

    auto cdist = path_cumulative_distance(path.begin(), path.end());
    auto speed = path_speed(path.begin(), path.end());
    speed = smooth(speed.begin(), speed.end());
    speed = smooth(speed.begin(), speed.end());
    
    std::ofstream file("./it.csv");
    
    int i = 0;
    
    for (const auto& d : cdist) {
        file << time_to_str_utc(d.timestamp) << "," << time_to_us(d.timestamp) << ", " << d.value << "," << speed[i++].value << std::endl;
    }
    
    return passed;
}


static bool test_path_heading() {
    auto passed = true;

    std::cout << "Testing path_heading()" << std::endl;

    auto path = load_gpx_qd("table_mountain_loop.gpx");
    passed &= value_test((int)path.size(), 6115);
    
    auto out = path_heading(path.begin(), path.end());
    passed &= value_test(out.size(), 6114);
    
    out = smooth(out.begin(), out.end());
    out = smooth(out.begin(), out.end());
    out = smooth(out.begin(), out.end());
    out = smooth(out.begin(), out.end());
    
    for (const auto i : out) {
       // std::cout << i << std::endl;
    }
    
    //out = first_central_difference(out.begin(), out.end());

    for (const auto i : out) {
      //  std::cout << i << std::endl;
    }
    
    
    return passed;
}


static bool test_find_closest_path_point_dist() {
    auto passed = true;

    std::cout << "Testing find_closest_path_point_dist()" << std::endl;
    std::vector<path_point> path;

    location target {52.988201, -6.413192};
    
    // Test empty path
    auto out = find_closest_path_point_dist(path.begin(), path.end(), target);
    passed &= iterator_test(out, path.end(), path.begin());
    
    path = {
        { { 52.988201, -6.413192 } }, 
        { { 52.988222, -6.413189 } }, 
        { { 52.98821, -6.413156 } }, 
        { { 52.988189, -6.413176 } }, 
        { { 52.988171, -6.413184 } }, 
        { { 52.988148, -6.413189 } }, 
        { { 52.988109, -6.413218 } }, 
    };

    out = find_closest_path_point_dist(path.begin(), path.end(), target);
    passed &= iterator_test(out, path.begin() + 0, path.begin());

    target = {52.988189, -6.413176};
    out = find_closest_path_point_dist(path.begin(), path.end(), target);
    passed &= iterator_test(out, path.begin() + 3, path.begin());

    target = {52.988179, -6.413166};
    out = find_closest_path_point_dist(path.begin(), path.end(), target);
    passed &= iterator_test(out, path.begin() + 3, path.begin());

    // last pos
    target = { 52.988119, -6.413208 };
    out = find_closest_path_point_dist(path.begin(), path.end(), target);
    passed &= iterator_test(out, path.begin() + 6, path.begin());

}

static test_find_stationary_points() {
    auto passed = true;

    std::cout << "Testing test_find_stationary_points()" << std::endl;

    auto path = load_gpx_qd("table_mountain_loop.gpx");
    
    auto it = find_stationary_points(path.begin(), path.end(), 10, 2 * 60);
    
    std::cout << "from: " << time_to_str_utc(it[0]->timestamp) << std::endl;
    std::cout << "to: " << time_to_str_utc(it[1]->timestamp) << std::endl;
    
    it = find_stationary_points(it[1], path.end(), 10, 2 * 60);
    
    std::cout << "from: " << time_to_str_utc(it[0]->timestamp) << std::endl;
    std::cout << "to: " << time_to_str_utc(it[1]->timestamp) << std::endl;

}



static bool test_load_gpx_qd() {
    auto passed = true;

    std::cout << "Testing load_gpx_qd()" << std::endl;

    auto path = load_gpx_qd("table_mountain_loop.gpx");
    passed &= value_test((int)path.size(), 6115);
    
    auto out = path_distance(path.begin(), path.end());
    passed &= value_test(out, 16964, 1.0);  // 16.9Km
        
    return passed;
}

static bool test_save_gpx_qd() {
    auto passed = true;

    std::cout << "Testing save_gpx_qd()" << std::endl;

    auto path = load_gpx_qd("table_mountain_loop.gpx");

    save_gpx_qd("test.gpx", path.begin(), path.end());
        
    return passed;
}

static bool test_load_csv_qd() {
    auto passed = true;

    std::cout << "Testing load_csv_qd()" << std::endl;

    auto path = load_csv_qd("pass4_gps_track_log_24_08_2022.csv");

    save_gpx_qd("test_gps_log.gpx", path.begin()+60, path.end());
        
    return passed;
}


static bool test_cardinal_direction() {
    auto passed = true;

    std::cout << "Testing cardinal_direction()" << std::endl;

    passed &= value_test(cardinal_direction(1.0), "N");
    passed &= value_test(cardinal_direction(45.0), "NE");
    passed &= value_test(cardinal_direction(90.0), "E");
    passed &= value_test(cardinal_direction(134.0), "SE");
    passed &= value_test(cardinal_direction(180.0), "S");
    passed &= value_test(cardinal_direction(215.0), "SW");
    passed &= value_test(cardinal_direction(269.0), "W");
    passed &= value_test(cardinal_direction(316.0), "NW");
    passed &= value_test(cardinal_direction(324.0), "NW");
    passed &= value_test(cardinal_direction(350.0), "N");
    passed &= value_test(cardinal_direction(359.0), "N");
    passed &= value_test(cardinal_direction(360.0), "N");
    passed &= value_test(cardinal_direction(-20.0), "N");
            
    return passed;
}


int main() {

    auto tests_passed = true;
    
    tests_passed &= test_to_radians();
    tests_passed &= test_to_degrees();
    tests_passed &= test_ddm_to_dd();
    tests_passed &= test_dd_to_ddm();
    tests_passed &= test_str_to_time_utc();
    tests_passed &= test_distance_vec();
    tests_passed += test_heading();
    tests_passed += test_distance();
    tests_passed += test_path_distance();
    tests_passed += test_path_distance_on_gpx();
    tests_passed += test_path_cumulative_distance();
    tests_passed += test_path_heading();
    tests_passed += test_find_closest_path_point_dist();
    tests_passed += test_cardinal_direction();
    tests_passed += test_load_gpx_qd();
    tests_passed += test_save_gpx_qd();
    tests_passed += test_load_csv_qd();
    tests_passed += test_find_stationary_points();

    return tests_passed ? 0 : 1;
}
