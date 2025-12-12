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

Note that when compiling wou may need to define **_USE_MATH_DEFINES** depending on your compiler (-D_USE_MATH_DEFINES)

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
+ ```load_gpx_qd()``` - Loads a sequence of GPS locations in a GPX file into a GPS path - Quick and Dirty, very limited/work in progress.
+ ```smooth()``` - Smooths a vector of path values, for example speeds or distances etc.
+ ```first_forward_difference()``` - Calculates the First Forward Difference of a vector of path values to obtain its numerical derivative.
+ ```first_central_difference()``` - Calculates the First Central Difference of a vector of path values to obtain its numerical derivative.
+ ```print_path_summary()``` - Generates and prints to STDIO a summary of the GPS path including Start Time, End Time, Durationm Distance etc.


# Types

+ location - a POD struct that holds latitude and longitude.

```cpp
struct location {
    // Note that it is assumed that latitude and
    // longitude are always provided as decimal
    // degrees.
    
    double lat;
    double lon;
};
```

+ path_point - represents a single location on a path of GPS points.

```cpp
// Represents a point on a path or sequence of GPS locations.
struct path_point {
    location loc;
    std::chrono::time_point<std::chrono::system_clock> timestamp;
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

# Tests
There are some tests in the test directory, to compile and run them run **make** and then execute **./test_gps_path_tools**
