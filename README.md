# gps_path_tools
GPS Path Tools code - A collection of C++ tools & functions for working with GPS path data, calculating distance, speed and heading etc.

**Warning - Work In Progress**

This is intended to be a fairly simple collection of functions to help working with GPS coordinates, positions and paths, its main aims are:

+ Provide some useful & simple tools for working with GPS positions and Paths for solving everyday problems.
+ Not to use any fancy/confusing C++ methods or tricks - use simple types and prioritise code readibility over code 'sophistication'
+ Have no dependencies other than the standard C++ lib.
+ Use via a single include of a header file.

To use these tools, just include **gps_path_tools.h**, and use the functions in the **gps_path_tools** namespace, for example:

```cpp
#include "gps_path_tools.h"

using namespace gps_path_tools;
  
int main() {
  // calculate the great circle distance between two lat/lon locations
  double dist = distance({ 52.98277, -6.03927 }, {52.98344, -6.03646 });
  
  // etc...
}
```

# Tools
This is currently a work in progress, the currently implemented functions are:

+ ```to_radians()``` - Converts from degrees to radians.
+ ```to_degrees()``` - Converts from radians to degrees.
+ ```ddm_to_dd()``` - Converts from Decimal Degrees and Decimal Minutes (ddm) to Decimal Degrees (dd).
+ ```dd_to_ddm()``` - Converts from Decimal Degrees (dd) to Decimal Degrees and Decimal Minutes (ddm).
+ ```mps_to_kph()``` - Converts from Meters per second (m/s) to Kilometers per hour (km/h).
+ ```kph_to_mps()``` - Converts from Kilometers per hour (km/h) to Meters per second (m/s).
+ ```str_to_time_utc()``` - Converts a UTC date/time string with optional fractional seconds in ISO8601 format (e.g. 2022-05-07T15:43:15.000001Z) to a time value (std::time_point). 
+ ```time_to_str_utc()``` - Converts a UTC time (std::time_point) to a string in ISO8601 format with fractional seconds. 
+ ```duration_to_str()``` - Converts the duration between two path time points to a string in the format HH:MM:SS. 
+ ```duration_to_seconds()``` - Converts the duration between two path time points to a number of seconds. 
+ ```time_to_us()``` - Converts a path time point to a number of microseconds in the Unix Epoch. 
+ ```cardinal_direction()``` - Estimates the 'closest' cardinal direction to the passed bearing and returns it as a string ("N", "SW" etc.)
+ ```hav()``` - Calculates the Haversine of an angle in radians.
+ ```ahav()``` - Calculates the inverse Haversine.
+ ```distance()``` - Calculates the Haversine distance between two GPS locations in meters.
+ ```distance_vec()``` - Calculates the 'Great Circle' distance between two GPS locations using vector normals.
+ ```path_distance()``` - Calculates the piecewise haversine distance of a path made up of a sequence of GPS locations in meters.
+ ```heading_gc()``` - Calculates the initial heading or course given two GPS locations.
+ ```path_heading()``` - Calculates the nominal heading between each path location and the next location in the given path.
+ ```path_speed()``` - Calculates the mean speed between the pairs of locations in the given path.
+ ```path_distance()``` - Sums up the location to location distances on the given path.
+ ```find_closest_path_point_dist()``` - Finds the closest path point to the provided target location based on distance.
+ ```find_closest_path_point_time()``` - Finds the closest path point to the provided target time based on time.
+ ```find_stationary_points()``` - Finds the first region within a path where progress halted, i.e. where the traveller 'stopped'.
+ ```load_gpx_trk()``` - Loads a sequence of GPS locations in a GPX file into a GPS path - Quick and Dirty, very limited/work in progress.
+ ```save_gpx_trk()``` - Saves a path to a GPX file as a <trk>.
+ ```smooth()``` - Smooths a vector of path values, for example speeds or distances etc.
+ ```first_forward_difference()``` - Calculates the First Forward Difference of a vector of path values to obtain its numerical derivative.
+ ```first_central_difference()``` - Calculates the First Central Difference of a vector of path values to obtain its numerical derivative.
+ ```print_path_summary()``` - Generates and prints to STDIO a summary of the GPS path including Start Time, End Time, Durationm Distance etc.

# Using from your project

To use gps_path_tools from your project just include the headerfiles:

For the GPS path functions include: **gps_path_tools.h**
For Reading and writing GPS files include: **gps_path_io.h**

Note that when compiling wou may need to define **_USE_MATH_DEFINES** depending on your compiler (-D_USE_MATH_DEFINES)

# Building and Running the Tests

```bash
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake build .

# Run tests
./path_tools_tests.exe

```

The tests use the [doctest](https://github.com/doctest/doctest) test framework.

# Types

+ location - a POD struct that holds latitude and longitude.

```cpp
struct location {
    // Note that it is assumed that latitude and
    // longitude are always provided as decimal
    // degrees.
    
    // Latitude
    double lat;

    // Longitude
    double lon;

    // Elevation
    double ele;
};
```

+ path_point - represents a single location on a path of GPS points.

```cpp
typedef std::chrono::time_point<std::chrono::system_clock> path_time;

// Represents a point on a path or sequence of GPS locations.
struct path_point {
    location loc;
    path_time timestamp;
    int sequence;
};
```

+ path - A vector of path_points that represents a path.

```cpp
typedef std::vector<path_point> path;
```

+ path_value - a numerical value that can be associated with a path point, e.g. speed or distance etc.

```cpp
struct path_value {
    double value;
    path_time timestamp;
};
```

# Examples

## Calculate the Haversine distance between two locations

```cpp
void haversine_distance() {

    location p1{ 52.9827588546699, -6.040081945988319 };
    location p2{ 53.057744464984495, -6.040085910508501};
    
    auto dist = distance(p1, p2);

    std::cout << "The distance between " << to_string(p1) << " and " << to_string(p2) << " is " << dist << "m" << std::endl;
}
```

## Calculate the Heading/Bearing between two locations

```cpp
void heading() {
    
    location p1{ 52.981935612371615, -6.0326861216896495 };
    location p2{ 52.98331462882221, -6.034538892416848};
    
    auto bearing = heading(p1, p2);
    
    std::cout << "The bearing between " << to_string(p1) << " and " << to_string(p1) << " is " << bearing << " degrees";
}
```

## Calculate the piece-wise Haversine distance along a path.

```cpp
void path_distance() {
    
    std::vector<path_point> path = {
        { { 52.9827588546699, -6.040081945988319 } }, 
        { { 53.057744464984495, -6.040085910508501 } },
        { { 52.9827588546699, -6.040081945988319 } }, 
    };

    auto distance = path_distance(path.begin(), path.end());
    
    std::cout << "The piece-wise distance along the path is " << distance << "m" << std::endl;
}
```

## Calculate the distance along a path loaded from a GPX file.

```cpp
void path_distance_from_gpx() {
    auto path = load_gpx_trk("table_mountain_loop.gpx");

    auto distance = path_distance(path.begin(), path.end());

    std::cout << "The piece-wise distance along the path is " << distance << "m" << std::endl;
}
```

## Find the point on a path that is closest to a given location

```cpp
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
```

## Get Cardinal Direction from bearing and back-bearing

```cpp
void cardinal_direction() {

    double bearing = 324.0;
    double back_bearing = 324 + 180.0;
    
    std::cout << "The cardinal direction of bearing " << bearing << "deg. is " << cardinal_direction(bearing) << std::endl;
    std::cout << "The cardinal direction of back-bearing on " << bearing << "deg. is " << cardinal_direction(back_bearing) << std::endl;
}
```

## Find first stationary segment of a path loaded from a GPX file

This function attempts to find the first "stationary segment" in the path loaded from  table_mountain_loop.gpx, here we define "stationary" as not moving outside a 10m radius during a 2 minute interval.

```cpp
void find_stationary_points() {

	// Load GPX
    auto path = load_gpx_trk("table_mountain_loop.gpx");
    
	// Find the first stationary segment on the path, here we are deemed as
	// stationary if we stay within a 10m radius for 2 minutes.  Will return
	// an iterator to the first and last points in the stationary path segment.
    auto stat = find_stationary_points(path.begin(), path.end(), 10, 2 * 60);
    
	if (stat.size() > 0) {
        auto time0 = stat[0]->timestamp;
        auto time1 = stat[1]->timestamp;
        auto location = stat[0]->loc;
        
		std::cout << "Stationary near " << to_string(location) << std::endl;
        std::cout << "From: " << time_to_str_utc(time0) << std::endl;
		std::cout << "To: " << time_to_str_utc(time1) << std::endl;
        std::cout << "For a time of " << duration_to_seconds(time0, time1) << " seconds." << std::endl;
	} else {
		std::cout << "No stationary segment found." << std::endl;
	}
}
```

