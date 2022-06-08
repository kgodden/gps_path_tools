
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
#include <chrono>
#include <vector>
#include <fstream>
#include <regex>
 
namespace gps_path_tools {

//
//-------------- Constants -------------- 
//

static constexpr double geoid_radius_m = 6371009;   // The mean radius of the Geoid in metres

//
//-------------- Types -------------- 
//

typedef std::chrono::time_point<std::chrono::system_clock> path_time;

struct location {
    // Note that it is assumed that latitude and
    // longitude are always provided as decimal
    // degrees.
    
    double lat;
    double lon;
};

// Represents a point on a path or sequence of GPS locations.
struct path_point {
    location loc;
    path_time timestamp;
};

typedef std::vector<path_point> path;

//
//-------------- Conversions -------------- 
//

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

// Convert from metres per second to kilometres
// per hour.
inline double mps_to_kph(const double mps) {
    return mps * 3.6;
}
    
// Convert from kilometres per hour to metres per second
inline double kph_to_mps(const double kph) {
    return kph / 3.6;
}

// Calculates the haversine of the passed angle
inline double hav(const double theta) {
    const double s = sin(theta / 2.0);
    return s * s;
}

// Calculates the inverse haversine of h
inline double ahav(const double h) {
    return 2.0 * asin(sqrt(h));
}

// Converts a decimal degrees value 
// into decimal degrees and decimal minutes.
inline double dd_to_ddm(const double dd) {
    double whole_degrees = floor(dd);
    double minutes = (dd - whole_degrees) * 60.0;
    return whole_degrees * 100.0 + minutes;
}


//
//-------------- Path Part Functions -------------- 
//

//
// Calculates great circle distance in metres
//  
inline double distance_vec(const location& l1, const location& l2) {
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

inline double distance(const location& l1, const location& l2) {
    // Calculates the haversine distance between the two points
    // see these pages for a nice background to haversine:
    // https://plus.maths.org/content/lost-lovely-haversine
    // https://www.movable-type.co.uk/scripts/latlong.html
    
    // Convert degrees to radians
    double lat1 = to_radians(l1.lat);
    double lon1 = to_radians(l1.lon);
    double lat2 = to_radians(l2.lat);
    double lon2 = to_radians(l2.lon);

    double dlat = lat2 - lat1;
    double dlon = lon2 - lon1;
    
    return geoid_radius_m * ahav(hav(dlat) + cos(lat1) * cos(lat2) * hav(dlon));
}

// Calculates speed in metres/s
//
inline double speed(const location& l1, const location& l2, const double seconds) {
    const auto dist = distance(l1, l2);
    return dist / seconds;
}

inline double heading(const location& l1, const location& l2) {
    // Convert degrees to radians
    double lat1 = to_radians(l1.lat);
    double lon1 = to_radians(l1.lon);
    double lat2 = to_radians(l2.lat);
    double lon2 = to_radians(l2.lon);   
        
    double dlon = lon2 - lon1;
    double X = cos(lat2) * sin(dlon);
    double Y = cos(lat1) * sin(lat2) - sin(lat1) * cos(lat2) * cos(dlon);
    
    double h = atan2(X,Y);
    
    // We want heading in degrees, not radians.
    h = to_degrees(h);
  
    // We want a uniform heading of >=0 and <360
    if (h < 0)
        h = 360.0 + h;
    
    return h;
}

//
//-------------- Path Functions -------------- 
//

inline double path_distance(const path::iterator start_it, const path::iterator end_it) {
    
    double dist = 0.0;
    
    // If the sequence is empty, return 0
    if (start_it == end_it)
        return dist;

    // iterator that is one past second last point.
    auto end = std::prev(end_it);
    
    // If the sequence only has one location,
    // return 0
    if (end == start_it)
        return dist;
    
    // Loop through from start to the second last
    // point accumulating the distances between
    // the point pairs.
    for (auto i = start_it; i != end; ++i) {
        const auto d = distance(i->loc, std::next(i)->loc); 

        if (!isnan(d))
            dist += d;
    }
    
    return dist;
}    

path::iterator find_closest_path_point_time(const path::iterator start_it, const path::iterator end_it, path_time target_timestamp) {

    // Check for empty/bad range
    if (start_it == end_it)
        return end_it;

    auto closest = start_it;
    auto smallest_time_delta = start_it->timestamp - target_timestamp;
    
    for (auto i = start_it; i != end_it; ++i) {
        const auto delta = start_it->timestamp - target_timestamp;
        
        if (delta < smallest_time_delta) {
            smallest_time_delta = delta;
            closest = i;
        }
    }
    
    return closest;
}

path::iterator find_closest_path_point_dist(const path::iterator start_it, const path::iterator end_it, const location& target) {
    
    // Check for empty/bad range
    if (start_it == end_it)
        return end_it;
    
    auto closest = start_it;
    auto smallest_delta = distance(start_it->loc, target);
    
    for (auto i = start_it; i != end_it; ++i) {
        const auto delta = distance(i->loc, target);;
        
        if (delta < smallest_delta) {
            smallest_delta = delta;
            closest = i;
        }
    }
    
    return closest;
}

path load_gpx_qd(const std::string filename) {
    path out;
    
    std::ifstream fs(filename);
    
    if (!fs)
        return out;

    std::string line;

    try {
    static const std::regex regexp(R"(lat\s*\=\s*\"([-+0-9\.]+)\" lon\s*=\s*\"([-+0-9\.]+)\")"); 
    while (getline(fs, line)) { 
        auto p = line.find("lat");
        
        if (p != std::string::npos) {
            std::smatch match;

            if (std::regex_search(line, match, regexp)) {
                double lat = atof(match[1].str().c_str());
                double lon = atof(match[2].str().c_str());
                out.push_back({ {lat, lon }});
            }
        }
    }

    } catch (const std::regex_error& ex) {
        printf("ex: %s %d\n", ex.what(), ex.code()); 
    }
    
    return out;
}
}   // namespace