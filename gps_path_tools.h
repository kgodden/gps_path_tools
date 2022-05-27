
#pragma once

//   Copyright 2022 Kevin Godden
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.


#include <math.h>


namespace gps_path_tools {

static constexpr double geoid_radius_m = 6378100;   // The radius of the Geoid in metres

struct location {
    // Note that it is assumed that latitude and
    // longitude are always provided as decimal
    // degrees.
    
    double lat;
    double lon;
};

inline double to_radians(const double theta) {
    return (theta * M_PI) / 180.0;
}

inline double to_degrees(const double theta) {
    return (theta * 180.0) / M_PI;
}

// Converts a decimal degrees and minutes value 
// into decimal degrees.
inline double ddm_to_dd(const double ddm) {
    
    double degrees = floor(ddm / 100.0);
    double minutes = ddm - degrees * 100.0;
    double decimal_degrees = degrees + minutes / 60.0;
    
    return decimal_degrees;
}

//
//
inline double distance_gc(const location& l1, const location& l2) {
    // Convert degrees to radians
    double lat1 = to_radians(l1.lat);
    double lon1 = to_radians(l1.lon);
    double lat2 = to_radians(l2.lat);
    double lon2 = to_radians(l2.lon);

    // Note that the radius of the geoid has been taken
    // out of the calculations of rho, z and cos_theta etc.
    // as it cancels out.
    
    // P
    double rho1 = cos(lat1);
    double z1 = sin(lat1);
    double x1 = rho1 * cos(lon1);
    double y1 = rho1 * sin(lon1);
    
    // Q
    double rho2 = cos(lat2);
    double z2 = sin(lat2);
    double x2 = rho2 * cos(lon2);
    double y2 = rho2 * sin(lon2);
    
    // Dot product
    double dot = (x1 * x2 + y1 * y2 + z1 * z2);
    double cos_theta = dot;
    double theta = acos(cos_theta);
    
    // Distance in Metres
    return geoid_radius_m * theta;
}

inline double heading_gc(const location& l1, const location& l2) {
    // Convert degrees to radians
    double lat1 = to_radians(l1.lat);
    double lon1 = to_radians(l1.lon);
    double lat2 = to_radians(l2.lat);
    double lon2 = to_radians(l2.lon);   
        
    double dlon = lon2 - lon1;
    double X = cos(lat2) * sin(dlon);
    double Y = cos(lat1) * sin(lat2) - sin(lat1) * cos(lat2) * cos(dlon);
    
    double heading = atan2(X,Y);
    
    // We want heading in degrees, not radians.
    heading = to_degrees(heading);
  
    // We want a uniform heading of >=0 and <360
    if (heading < 0)
        heading = 360.0 + heading;
    
    return heading;
}

}   // namespace