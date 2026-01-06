
#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "../gps_path_tools.h"
#include "../gps_path_io.h"

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

static bool value_test(path_time value, path_time target) {
        auto pass = value == target;

        if (pass) {
            std::cout << "PASS";
        } else {
            std::cout << "FAIL";
        }
        
        std::cout << ", got: " << time_to_str_utc(value) << ", target: " << time_to_str_utc(target) << std::endl;
        
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

static std::string make_data_path(const std::string& file_name) {
    return "../test_data/" + file_name;
}

TEST_CASE("test_to_radians") {
    std::vector<test_point2> test_points = {
        {0.0, 0.0, 0.0000001},
        {180.0, M_PI, 0.0000001},
        {360.0, 2 * M_PI, 0.0000001},
        {10.0, 0.174532925, 0.0000001},
        {-10.0, -0.174532925, 0.0000001},
    };
    
    for (const auto& test_point: test_points) {
        auto out = to_radians(test_point.a);
        CHECK(value_test(out, test_point.target, test_point.allowable_delta));
    }

}


TEST_CASE("test_to_radians") {
    std::vector<test_point2> test_points = {
        {0.0, 0.0, 0.0000001},
        {M_PI, 180.0, 0.0000001},
        {2 * M_PI, 360.0, 0.0000001},
        {0.174532925, 10.0, 0.0000001},
        {-0.174532925, -10.0, 0.0000001},
    };
    
    for (const auto& test_point: test_points) {
        auto out = to_degrees(test_point.a);
        CHECK(value_test(out, test_point.target, test_point.allowable_delta));
    }
}

TEST_CASE("test_ddm_to_dd") {
    
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
        CHECK(value_test(out, test_point.target, test_point.allowable_delta));
    }
}

TEST_CASE("test_dd_to_ddm") {
    
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
        CHECK(value_test(out, test_point.target, test_point.allowable_delta));
    }
}

TEST_CASE("test_str_to_time_utc") {
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
        
        CHECK(value_test(out, test.second));
    }
}

TEST_CASE("test_distance_vec") {    
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
        CHECK(value_test(out, test_point.target, test_point.allowable_delta));
    }
}

TEST_CASE("test_distance") {        
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
        CHECK(value_test(out, test_point.target, test_point.allowable_delta));
    }
}

TEST_CASE("test_heading") {
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
        CHECK(value_test(h, test_point.target, test_point.allowable_delta));
    }
}

TEST_CASE("test_path_distance") {
    std::vector<path_point> path;
    
    // Test empty path
    auto out = path_distance(path.begin(), path.end());
    CHECK(value_test(out, 0, 0.0000001));
        
    // Test invalid path with one point
    path = {
        { {52.981935612371615, -6.0326861216896495} },
    };

    out = path_distance(path.begin(), path.end());
    CHECK(value_test(out, 0, 0.0000001));

    path = {
        { { 52.9827588546699, -6.040081945988319 } }, 
        { { 53.057744464984495, -6.040085910508501 } },
        { { 52.9827588546699, -6.040081945988319 } }, 
    };

    out = path_distance(path.begin(), path.end());
    CHECK(value_test(out, 8338 * 2, 0.1));
}

TEST_CASE("test_path_distance_on_gpx") {

    auto path = load_gpx_trk(make_data_path("table_mountain_loop.gpx"));

    auto out = path_distance(path.begin(), path.end());

    CHECK(value_test(out, 16964, 0.5));
}

/*
TEST_CASE("test_path_cumulative_distance") {
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
}
*/

TEST_CASE("test_path_heading") {
    auto path = load_gpx_trk(make_data_path("table_mountain_loop.gpx"));
    CHECK(value_test((int)path.size(), 6115));
    
    auto out = path_heading(path.begin(), path.end());
    CHECK(value_test(out.size(), 6114));    
}


TEST_CASE("test_find_closest_path_point_dist") {
    std::vector<path_point> path;

    location target {52.988201, -6.413192};
    
    // Test empty path
    auto out = find_closest_path_point_dist(path.begin(), path.end(), target);
    CHECK(iterator_test(out, path.end(), path.begin()));
    
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
    CHECK(iterator_test(out, path.begin() + 0, path.begin()));

    target = {52.988189, -6.413176};
    out = find_closest_path_point_dist(path.begin(), path.end(), target);
    CHECK(iterator_test(out, path.begin() + 3, path.begin()));

    target = {52.988179, -6.413166};
    out = find_closest_path_point_dist(path.begin(), path.end(), target);
    CHECK(iterator_test(out, path.begin() + 3, path.begin()));

    // last pos
    target = { 52.988119, -6.413208 };
    out = find_closest_path_point_dist(path.begin(), path.end(), target);
    CHECK(iterator_test(out, path.begin() + 6, path.begin()));
}


TEST_CASE("test_find_stationary_points") {
    auto path = load_gpx_trk(make_data_path("table_mountain_loop.gpx"));
    
    auto it = find_stationary_points(path.begin(), path.end(), 10, 2 * 60);
    
    std::cout << "from: " << time_to_str_utc(it[0]->timestamp) << std::endl;
    std::cout << "to: " << time_to_str_utc(it[1]->timestamp) << std::endl;
    
    it = find_stationary_points(it[1], path.end(), 10, 2 * 60);
    
    std::cout << "from: " << time_to_str_utc(it[0]->timestamp) << std::endl;
    std::cout << "to: " << time_to_str_utc(it[1]->timestamp) << std::endl;
}

TEST_CASE("test_save_gpx_trk") {
    auto path = load_gpx_trk(make_data_path("table_mountain_loop.gpx"));
    CHECK(value_test((int)path.size(), 6115));
    
    auto ok = save_gpx_trk(make_data_path("test.gpx"), path.begin(), path.end());
    CHECK(ok);
    
    auto path1 = load_gpx_trk(make_data_path("test.gpx"));
    CHECK(value_test((int)path1.size(), 6115));
}


TEST_CASE("test_cardinal_direction") {

    CHECK(value_test(cardinal_direction(1.0), "N"));
    CHECK(value_test(cardinal_direction(45.0), "NE"));
    CHECK(value_test(cardinal_direction(90.0), "E"));
    CHECK(value_test(cardinal_direction(134.0), "SE"));
    CHECK(value_test(cardinal_direction(180.0), "S"));
    CHECK(value_test(cardinal_direction(215.0), "SW"));
    CHECK(value_test(cardinal_direction(269.0), "W"));
    CHECK(value_test(cardinal_direction(316.0), "NW"));
    CHECK(value_test(cardinal_direction(324.0), "NW"));
    CHECK(value_test(cardinal_direction(350.0), "N"));
    CHECK(value_test(cardinal_direction(359.0), "N"));
    CHECK(value_test(cardinal_direction(360.0), "N"));
    CHECK(value_test(cardinal_direction(-20.0), "N"));
}

TEST_CASE("test_load_gpx_trk") {
    auto path = load_gpx_trk(make_data_path("table_mountain_loop.gpx"));

    CHECK(value_test((int)path.size(), 6115));
    
    auto& first = path.front();
    
    CHECK(value_test(first.loc.lat, 52.988201, 0.000001));
    CHECK(value_test(first.loc.lon, -6.413192, 0.000001));
    CHECK(value_test(first.timestamp, str_to_time_utc("2022-05-07T10:20:06.433Z")));
    CHECK(value_test(first.loc.ele, 165.56749, 0.000001));

    auto& last = path.back();
    CHECK(value_test(last.loc.lat, 52.988079, 0.000001));
    CHECK(value_test(last.loc.lon, -6.413215, 0.000001));
    CHECK(value_test(last.timestamp, str_to_time_utc("2022-05-07T15:43:38.000Z")));
    CHECK(value_test(last.loc.ele, 170.38306, 0.000001));


    auto out = path_distance(path.begin(), path.end());
    CHECK(value_test(out, 16964, 0.5));  // 16.9Km
}

TEST_CASE("test_load_gpx_trk1") {
    auto path = load_gpx_trk(make_data_path("knocknalogha_moot_25.gpx"));
    
    auto& first = path.front();

    std::cout << "First: " << to_string(first.loc) << std::endl;

    CHECK(value_test(first.loc.lat, 52.25126, 0.000001));
    CHECK(value_test(first.loc.lon, -7.96103, 0.000001));
    CHECK(value_test(first.timestamp, str_to_time_utc("2025-11-15T12:10:55.054Z")));
    CHECK(value_test(first.loc.ele, 263.5514879999982, 0.000001));

    auto& last = path.back();
    CHECK(value_test(last.loc.lat, 52.25424, 0.000001));
    CHECK(value_test(last.loc.lon, -7.96033, 0.000001));
    CHECK(value_test(last.timestamp, str_to_time_utc("2025-11-15T17:08:05.052Z")));
    CHECK(value_test(last.loc.ele, 179.0823679999559, 0.000001));
}


TEST_CASE("test_load_csv_qd") {

    auto path = load_csv_qd(make_data_path("pass4_gps_track_log_24_08_2022.csv"));

    CHECK(value_test(path.size(), 2010));

    save_gpx_trk("test_gps_log.gpx", path.begin()+60, path.end());
}

TEST_CASE("test_generate_path_summary") {
    auto path = load_gpx_trk(make_data_path("table_mountain_loop.gpx"));

    auto summary = generate_path_summary(path.begin(), path.end());
    CHECK(value_test(summary.points, 6115));
    CHECK(value_test(summary.start_time, time_to_str_utc(path.begin()->timestamp)));
    CHECK(value_test(summary.end_time, time_to_str_utc(path.end()->timestamp)));
    CHECK(value_test(summary.duration_s, duration_to_seconds(path.front().timestamp, path.back().timestamp)));
    CHECK(value_test(summary.distance_m, path_distance(path.begin(), path.end())));
}

#if 0
static void test_100m() {
	auto path = load_gpx_trk_qd(make_data_path("knocknalogha_moot_25.gpx"));
	std::cout << "Loaded " << path.size() << " points." << std::endl;
	print_path_summary(path);
	
	auto search_end = std::next(path.begin(), path.size() / 3);
	
	auto s1 = find_closest_path_point_dist(path.begin(), search_end, { 52.2494, -7.96006 });
	auto s2 = find_closest_path_point_dist(path.begin(), search_end, { 52.2476, -7.95012 });

/*
	for (auto it = s1; it != s2; ++it) {
		std::cout << time_to_str_utc(s1->timestamp) << " lat: " << it->loc.lat << std::endl;
	}
*/	
	std::cout << "Section start: " << time_to_str_utc(s1->timestamp) << std::endl;
	std::cout << "Section end: " << time_to_str_utc(s2->timestamp) << std::endl;
	std::cout << "Section points: " << (s2 - s1) << std::endl;
	
	save_gpx_trk_qd("./section.gpx", s1, s2);
	
	auto speed = path_speed(s1, s2);

	std::cout << "Speed points: " << speed.size() << std::endl;
	
	std::ofstream file("./knocknalogha.csv");
    
    int i = 0;
    
    for (const auto& s : speed) {
        file << time_to_str_utc(s.timestamp) << "," << s.value << std::endl;
    }
	
	auto p1 = s1 + 8 - 1;
	auto p2 = s1 + 23;
	
	auto dist = path_distance(p1, p2);
	
	std::cout << "Distance 1: " << dist << std::endl;
	
	auto p3 = s1 + 26 - 1;
	auto p4 = s1 + 42 + 1;
	
	auto dist1 = path_distance(p3, p4);
	
	std::cout << "Distance 2: " << dist1 << std::endl;
}
#endif
