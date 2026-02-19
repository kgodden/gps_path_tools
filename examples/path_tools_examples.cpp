
#include "../gps_path_tools.h"
#include "../gps_path_io.h"

using namespace gps_path_tools;

void haversine_distance() {

    location p1{ 52.9827588546699, -6.040081945988319 };
    location p2{ 53.057744464984495, -6.040085910508501};
    
    auto dist = distance(p1, p2);

    std::cout << "The distance between " << to_string(p1) << " and " << to_string(p2) << " is " << dist << "m" << std::endl;
}

void heading() {
    
    location p1{ 52.981935612371615, -6.0326861216896495 };
    location p2{ 52.98331462882221, -6.034538892416848};
    
    auto bearing = heading(p1, p2);
    
    std::cout << "The bearing between " << to_string(p1) << " and " << to_string(p1) << " is " << bearing << " degrees" << std::endl;
}

void path_distance() {
    
    std::vector<path_point> path = {
        { { 52.9827588546699, -6.040081945988319 } }, 
        { { 53.057744464984495, -6.040085910508501 } },
        { { 52.9827588546699, -6.040081945988319 } }, 
    };

    auto distance = path_distance(path.begin(), path.end());
    
    std::cout << "The piece-wise distance along the path is " << distance << "m" << std::endl;
}

void path_distance_from_gpx() {
    auto path = load_gpx_trk("table_mountain_loop.gpx");

    auto distance = path_distance(path.begin(), path.end());

    std::cout << "The piece-wise distance along the path is " << distance << "m" << std::endl;
}


void closest_path_point() {

    location target{52.988181, -6.413106};
        
    std::vector<path_point> path = {
        { { 52.988201, -6.413192 } }, 
        { { 52.988222, -6.413189 } }, 
        { { 52.98821, -6.413156 } }, 
        { { 52.988189, -6.413176 } }, 
        { { 52.988171, -6.413184 } }, 
        { { 52.988148, -6.413189 } }, 
        { { 52.988109, -6.413218 } }, 
    };
    
    auto closest = find_closest_path_point_dist(path.begin(), path.end(), target);
    auto dist = distance(closest->loc, target);

    std::cout << "The closest point on the path to  " << to_string(target) << " is " << to_string(closest->loc) << ", it is " << dist << "m away" << std::endl; 
}

void cardinal_direction() {

    double bearing = 324.0;
    double back_bearing = 324 + 180.0;
    
    std::cout << "The cardinal direction of bearing " << bearing << "deg. is " << cardinal_direction(bearing) << std::endl;
    std::cout << "The cardinal direction of back-bearing on " << bearing << "deg. is " << cardinal_direction(back_bearing) << std::endl;
}

void find_stationary_points() {

    // Load GPX
    auto path = load_gpx_trk("../examples/table_mountain_loop.gpx");
    
    // Find the first stationary segment on the path, here we are deemed as
    // stationary if we stay within a 10m radius for 2 minutes.  Will return
    // an iterator to the first and last points in the stationary path segment.
    auto [start, end] = find_stationary_points(path.begin(), path.end(), 10, 2 * 60);
    
    if (start != path.end() && end != path.end()) {
        auto time0 = start->timestamp;
        auto time1 = end->timestamp;
        auto location = start->loc;
        
        std::cout << "Stationary near " << to_string(location) << std::endl;
        std::cout << "From: " << time_to_str_utc(time0) << std::endl;
        std::cout << "To: " << time_to_str_utc(time1) << std::endl;
        std::cout << "For a time of " << duration_to_seconds(time0, time1) << " seconds." << std::endl;
    } else {
        std::cout << "No stationary segment found." << std::endl;
    }
}

void find_closest_path_point_time() {

    // Load GPX
    auto path = load_gpx_trk("../examples/table_mountain_loop.gpx");
    for (auto pp : path) {
 //       std::cout << pp.timestamp << std::endl;
    }
}

void find_farthest_point() {
    auto path = load_gpx_trk("../examples/table_mountain_loop.gpx");

    // Find the farthest point from the start of the path across the whole path
    auto farthest = find_farthest_point(path.begin(), path.end(), path.begin()->loc);

    std::cout << "The farthest point from the path start is point #"
            << (farthest - path.begin()) << " at " << to_string(farthest->loc) <<
            ", it is " << distance(path.begin()->loc, farthest->loc) << "m away from the start." << std::endl;
}

// Loads a GPS path from a GPX file, calculates its bounding box,
// prints some details and then adds the box corners to the original path
// and saves it to a GPX file so that it can be viewed
//
void axis_aligned_bounding_box() {
    auto path = load_gpx_trk("../examples/table_mountain_loop.gpx");

    // Get the corners of the path's axis-aligned 
    // bounding box.
    auto [nw, ne, se, sw] = axis_aligned_bounding_box(path.begin(), path.end());

    auto we_length = distance(nw, ne);
    auto ns_length = distance(nw, sw);

    std::cout << "Bounding box corners NW: " << to_string(nw) << ", SE: " << to_string(se) <<
            ", NS length (m): " << ns_length << ", WE length (m): " << we_length << std::endl; 
    
    // Add the corner points onto the end of
    // the path so that we can save it as GPX
    // and load it to view the path and bounding box.
    path.push_back({ nw });
    path.push_back({ ne });
    path.push_back({ se });
    path.push_back({ sw });
    path.push_back({ nw });

    save_gpx_trk("al_bounding_box.gpx", path.begin(), path.end());
}

int main(int, char**) {
    haversine_distance();
    find_stationary_points();
    heading();
    path_distance();
    path_distance_from_gpx();
    closest_path_point();
    cardinal_direction();
    find_closest_path_point_time();
    find_farthest_point();
    axis_aligned_bounding_box();

    return 1;
}