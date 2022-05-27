# gps_path_tools
A collection of C++ tools & functions for working with GPS path data, calculating distance, speed and heading etc.

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
  double distance = distance_gc({ 52.98277, -6.03927 }, {52.98344, -6.03646 });
  
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
+ ```distance_gc()``` - Calculates the 'Great Circle' distance between two GPS locations.
+ ```heading_gc()``` - Calculates the initial heading or course given two GPS locations.

# Tests
There are some tests in the test directory, to compile and run them run **make** and then execute **./test_gps_path_tools**
