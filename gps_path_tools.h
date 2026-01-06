
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


#include <iterator>
#include <math.h>
#include <chrono>
#include <time.h>
#include <vector>
#include <fstream>
#include <regex>
#include <iostream>
#include <iomanip>
 
namespace gps_path_tools {

//
//-------------- Constants -------------- 
//

static constexpr double geoid_radius_m = 6371009;   // The mean radius of the Geoid in metres

//
//-------------- Types -------------- 
//

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

typedef std::chrono::time_point<std::chrono::system_clock> path_time;

// Represents a point on a path or sequence of GPS locations.
struct path_point {
    location loc;
    path_time timestamp;
    int sequence;
};

// represents a value at some point along 
// the path such as heading or speed etc.
struct path_value {
    double value;
    path_time timestamp;
};

typedef std::vector<path_point> path;

// Holds some path summary data, use
// generate_path_summary() to create a summary.
struct path_summary {

    // The number of points in the path
    size_t points;

    // Start & End times,
    // i.e. times of first and last points
    std::string start_time;
    std::string end_time;

    // Path duration in seconds (end_time - start_time)
    double duration_s;

    // Path piece-wise length/distance in metres
    double distance_m;

    // Mean speed along path in km/h
    double mean_speed_kph;
};


//
//-------------- Conversions -------------- 
//

inline std::string to_string(const location& l) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(6)
        << "(" << l.lat << ", " << l.lon << ")";
        
    return oss.str();
}

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
   
#ifndef timegm
// use _mkgmtime() on windows
#define timegm _mkgmtime
#endif

#ifndef _mkgmtime

// Algorithm: http://howardhinnant.github.io/date_algorithms.html
namespace internal {
    
    inline int days_from_epoch(int y, int m, int d)
    {
        y -= m <= 2;
        int era = y / 400;
        int yoe = y - era * 400;                                   // [0, 399]
        int doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;  // [0, 365]
        int doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;           // [0, 146096]
        return era * 146097 + doe - 719468;
    }

    // It  does not modify broken-down time
    inline time_t timegm(struct tm const* t)     
    {
        int year = t->tm_year + 1900;
        int month = t->tm_mon;          // 0-11
        if (month > 11)
        {
            year += month / 12;
            month %= 12;
        }
        else if (month < 0)
        {
            int years_diff = (11 - month) / 12;
            year -= years_diff;
            month += 12 * years_diff;
        }
        int days_since_epoch = days_from_epoch(year, month + 1, t->tm_mday);

        return 60 * (60 * (24L * days_since_epoch + t->tm_hour) + t->tm_min) + t->tm_sec;
    }
}

#endif
// Convert from kilometres per hour to metres per second
inline double kph_to_mps(const double kph) {
    return kph / 3.6;
}

inline path_time str_to_time_utc(const std::string& time_str) {
    std::stringstream in(time_str);

    std::tm tm{};
    
    // e.g. 2022-05-07T15:43:15.000Z
    in >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
    
    time_t t = internal::timegm(&tm);

    //std::cout << t << std::endl;
      
    auto time = std::chrono::system_clock::from_time_t(t);

    double us = 0.0;
    
    sscanf(time_str.c_str() + 19, "%lf", &us);
    
    time += std::chrono::microseconds((unsigned int)((us + 0.0000005) * 1E6));
    
    return time;
}

inline std::string time_to_str_utc(const path_time time) {
	// convert to time_t which will represent the number of
	// seconds since the UNIX epoch, UTC 00:00:00 Thursday, 1st. January 1970
	auto epoch_seconds = std::chrono::system_clock::to_time_t(time);
	// Format this as date time to seconds resolution
	// e.g. 2016-08-30T08:18:51
	std::stringstream stream;
	stream << std::put_time(gmtime(&epoch_seconds), "%Y-%m-%dT%H:%M:%S");
	// If we now convert back to a time_point we will get the time truncated
	// to whole seconds 
	auto truncated = std::chrono::system_clock::from_time_t(epoch_seconds);
	// Now we subtract this seconds count from the original time to
	// get the number of extra microseconds..
	auto delta_us = std::chrono::duration_cast<std::chrono::microseconds>(time - truncated).count();
	// And append this to the output stream as fractional seconds
	// e.g. 2016-08-30T08:18:51.867479
	stream << "." << std::fixed << std::setw(6) << std::setfill('0') << delta_us << "Z";
	return stream.str();
}

//
// Converts the duration between two path times to a string
// in the format HH:MM:SS
//
inline std::string duration_to_str(const path_time t1, const path_time t2) {
	
	auto delta = t2 - t1;
	auto secs = std::chrono::duration_cast<std::chrono::seconds>(delta).count();

    int hours = static_cast<int>(secs / 3600);
    secs %= 3600;
    int minutes = static_cast<int>(secs / 60);
    int seconds = static_cast<int>(secs % 60);	
	
	std::stringstream stream;
	stream << hours << ":" << std::setw(2) << std::setfill('0') << minutes << ":" << std::setw(2) << std::setfill('0') << seconds;

	return stream.str();
}

//
// Converts the duration between two path times to a number of seconds.
//
inline double duration_to_seconds(const path_time t1, const path_time t2) {
	auto delta = t2 - t1;
	return std::chrono::duration_cast<std::chrono::seconds>(delta).count();
}

//
// Converts a path time point to the number of microseconds in the Unix epoch.
//
inline long long time_to_us(const path_time time) {
    return std::chrono::duration_cast<std::chrono::microseconds>(time.time_since_epoch()).count();
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

// Estimates the cardinal direction from the passed bearing
// Heading to be passed in degrees
inline const char* cardinal_direction(double bearing) {

    static const struct {
        
        double angle;
        const char* name;
        
    } cardinals[] = {
        {0.0, "N"},
        {45.0, "NE"},
        {90.0, "E"},
        {135.0, "SE"},
        {190.0, "S"},
        {225.0, "SW"},
        {270.0, "W"},
        {315.0, "NW"},
        {360.0, "N"},
    };

    bearing = fmod(bearing, 360.0);
    
    if (bearing < 0.0)
        bearing += 360;
    
    int closest_index = 0;
    int closest_delta = fabs(cardinals[0].angle - bearing);
    
    for (int i = 0; i != 9; ++i) {
        int delta = fabs(cardinals[i].angle - bearing);
        
        if (delta < closest_delta) {
                closest_delta = delta;
                closest_index = i;
        }
    }
    
    return cardinals[closest_index].name;
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

inline double path_distance(const path::const_iterator start_it, const path::const_iterator end_it) {
    
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

//
// Calculates the nominal heading between each path location and the next location in the given path.
//
inline std::vector<path_value> path_heading(const path::iterator start_it, const path::iterator end_it) {
        
    if (std::distance(start_it, end_it) < 2)
        return {};
    

    // iterator that is one past second last point.
    auto end = std::prev(end_it);
        
    std::vector<path_value> out;
    out.reserve(std::distance(start_it, end));
    
    // Loop through from start to the second last
    // point accumulating the heading between
    // the point pairs.
    for (auto i = start_it; i != end; ++i) {
        const auto h = heading(i->loc, std::next(i)->loc); 

        out.push_back({ h, i->timestamp });
    }
    
    return out;
}    

// 
// Calculates the mean speed between the pairs of locations in the given path.
//
inline std::vector<path_value> path_speed(const path::const_iterator start_it, const path::const_iterator end_it) {
        
    if (std::distance(start_it, end_it) < 2)
        return {};
    

    // iterator that is one past second last point.
    auto end = std::prev(end_it);
        
    std::vector<path_value> out;
    out.reserve(std::distance(start_it, end));
    
    // Loop through from start to the second last
    // point accumulating the speed between
    // the point pairs.
    for (auto i = start_it; i != end; ++i) {
        const auto h = speed(i->loc, std::next(i)->loc, (double)std::chrono::duration_cast<std::chrono::microseconds>(std::next(i)->timestamp - i->timestamp).count() / 1E6); 

        out.push_back({ h, i->timestamp });
    }
    
    return out;
}

//
// Sums up the location to location distances on the given path.
//
inline std::vector<path_value> path_cumulative_distance(const path::const_iterator start_it, const path::const_iterator end_it) {
        
    if (std::distance(start_it, end_it) < 2)
        return {};
    

    // iterator that is one past second last point.
    auto end = std::prev(end_it);
        
    std::vector<path_value> out;
    out.reserve(std::distance(start_it, end));
    
    double dist = 0.0;
    
    // Loop through from start to the second last
    // point accumulating the heading between
    // the point pairs.
    for (auto i = start_it; i != end; ++i) {
        const auto d = distance(i->loc, std::next(i)->loc); 
        dist += d;
        
        out.push_back({ dist, std::next(i)->timestamp });
    }
    
    return out;
}    

//
// Finds the closest path point to the provided target location based on time.
//
inline path::iterator find_closest_path_point_time(const path::iterator start_it, const path::iterator end_it, path_time target_timestamp) {

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

//
// Finds the closest path point to the provided target location based on distance.
//
inline path::iterator find_closest_path_point_dist(const path::iterator start_it, const path::iterator end_it, const location& target) {
    
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

//
// Finds the first region within a path where progress halted, i.e. where the traveller 'stopped'.
//
inline std::vector<path::const_iterator> find_stationary_points(const path::const_iterator start_it, const path::const_iterator end_it, const int radius_m, const int time_s) {
    // Find the points where successive distance travelled values
    // does not go further than radius_m
    
    auto start = start_it;
    int stationary_count = 0;
    bool time_ok = false;
    
    for (auto i = start_it; i != std::prev(end_it); ++i) {
        const auto next = std::next(i);
        const auto delta = distance(start->loc, next->loc);
        
        if (delta < radius_m) {
                        
            if (i->timestamp - start->timestamp >  std::chrono::seconds(time_s)) {
                time_ok = true;
            }
        } else {

            if (time_ok) {
                return { start, i };
            }
            
            start = i;
        }
    }
    
    // none found
    return { end_it, end_it };
}

//
//-------------- Helper Functions -------------- 
//

inline std::vector<path_value> smooth(const std::vector<path_value>::const_iterator start_it, const std::vector<path_value>::const_iterator end_it) {
    if (std::distance(start_it, end_it) < 3)
        return {};
        
    std::vector<path_value> out;
    out.reserve(std::distance(start_it, end_it) - 2);
    
    for (auto i = start_it; i != std::prev(end_it); ++i) {
            // Triangular filter
            // Use [ 1, 2, 1 ] kernel
            const double val = (std::prev(i)->value + 2.0 * i->value + std::next(i)->value) / 4.0;
            out.push_back({ val , i->timestamp });
    }

    return out;
}

inline std::vector<path_value> first_forward_difference(const std::vector<path_value>::const_iterator start_it, const std::vector<path_value>::const_iterator end_it) {
    if (std::distance(start_it, end_it) < 3)
        return {};
        
    std::vector<path_value> out;
    out.reserve(std::distance(start_it, end_it) - 2);
    
    for (auto i = start_it; i != std::prev(end_it); ++i) {
            const double val = std::next(i)->value - std::prev(i)->value; 
            out.push_back({ val, i->timestamp });
    }

    return out;
}

inline std::vector<path_value> first_central_difference(const std::vector<path_value>::iterator start_it, const std::vector<path_value>::iterator end_it) {
    if (std::distance(start_it, end_it) < 3)
        return {};
        
    std::vector<path_value> out;
    out.reserve(std::distance(start_it, end_it) - 2);
    
    for (auto i = std::next(start_it); i != std::prev(end_it); ++i) {
            const double val = (std::next(i)->value - std::prev(i)->value) / 2.0;
            out.push_back( { val, i->timestamp });
    }

    return out;
}

inline path_summary generate_path_summary(const path::const_iterator start_it, const path::const_iterator end_it) {
    path_summary summary{};

    summary.points = std::distance(start_it, end_it);
	
	if (summary.points < 2) {
		return summary;
	}
	
	summary.start_time = time_to_str_utc(start_it->timestamp);
	summary.end_time = time_to_str_utc(end_it->timestamp); 
	summary.duration_s = duration_to_seconds(start_it->timestamp, (end_it- 1)->timestamp); 
	summary.distance_m = path_distance(start_it, end_it);	
	auto seconds = duration_to_seconds(start_it->timestamp, (end_it - 1)->timestamp);
	auto speed = seconds > 0 ? summary.distance_m / seconds : 0.0;
	
	summary.mean_speed_kph = mps_to_kph(speed);	

    return summary;
}


//
//-------------- I/O Functions -------------- 
//

//
// Prints out a summary of the given path to stdio.
//
inline void print_path_summary(const path& in) {

    const auto summary = generate_path_summary(in.begin(), in.end());

    std::cout << "Number of points: " << summary.points << std::endl;
	
	if (summary.points < 2) {
		return;
	}
		
	std::cout << "Start Time: " << summary.start_time << std::endl; 
	std::cout << "End Time: " << summary.end_time << std::endl; 
	std::cout << "Duration: " << summary.duration_s << "s" << std::endl; 
		
	std::cout << "Distance: " << summary.distance_m << "m" << std::endl;
	std::cout << "Mean Speed: " << std::fixed << std::setprecision(1) << summary.mean_speed_kph << "kph" << std::endl;
	
	auto spoints = find_stationary_points(in.begin(), in.end(), 15.0, 3 * 60);
	
	std::cout << "Stationary points " << spoints.size() << std::endl;
	
	for (const auto p : spoints) {
		std::cout << "Time: " << time_to_str_utc(p->timestamp) << std::endl;
	}
}

// Quick and Dirty (qd) write path segment to .gpx

// Quick and dirty load of path from csv file.
//
inline path load_csv_qd(const std::string filename) {
    path out;
    
    std::cout << "Loading " << filename << std::endl;
    
    std::ifstream fs(filename);
    
    if (!fs)
        return out;

    std::cout << "loaded" << std::endl;

    std::string line;

    location loc{};
    
    try {
        while (getline(fs, line)) { 
            // 1658184839700,27797,51.872949,-8.582667,40.6,80.57,4,0.00,0.00,0.000,-1,0,0
            //std::cout << line << std::endl;
            long long ts;
            long long tsd;
            double lat;
            double lon;
            
            if (sscanf(line.c_str(), "%lld,%lld,%lf,%lf", &ts, &tsd, &lat, &lon) == 4) {
                loc = { lat, lon };
                out.push_back({ loc });
            }
            
        }

    } catch (const std::regex_error& ex) {
        printf("ex: %s %d\n", ex.what(), ex.code()); 
    }
    
    return out;
}


}   // namespace