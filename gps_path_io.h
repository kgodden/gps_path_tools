#pragma once
#include <string>
#include "gps_path_tools.h"
#include <cstring>
#include <fstream>
#include <regex>

namespace gps_path_tools {

#define prn(arg)  do { std::cout << arg << std::endl; } while (false)

enum class stream_state {
    in_boring,
    in_element_start,
    in_element_end,
};

enum class gpx_state {
    scanning,
    have_opening_tag,
    have_closing_tag,
};

/*
static std::string trimw(const std::string_view s) {
    static const char* whitespace = " \t\n\r\f\v";

    size_t start = 0;
    size_t end = s.size();

    while (start < end && std::strchr(whitespace, s[start])) ++start;
    while (end > start && std::strchr(whitespace, s[end - 1])) --end;

    return std::string(s.substr(start, end - start));
}
*/

path load_gpx_trk(const std::string& filename) {

    std::ifstream fs(filename);

    if (!fs) {
        return {};
    }

    auto sstate = stream_state::in_boring;
    auto gstate = gpx_state::scanning;

    std::string buf;
    buf.reserve(128);

    std::string opening;
    opening.reserve(128);
    
    std::string closing;
    closing.reserve(64);
 
    std::string element;
    element.reserve(128);

    path gpx_path;
    char last_c{};
    char c{};
    bool in_trkpt{};

    double lat{};
    double lon{};
    double ele{};
    int seq{};

    path_time time{};

    while (!fs.eof()) {

        last_c = c;
        fs.get(c);

        switch (sstate) {
            case stream_state::in_boring:
                if (last_c == '<') {
                    if (c != '/') {
                        buf = last_c;
                        sstate = stream_state::in_element_start;
                    } else {
                        if (!buf.empty()) {
                            element = buf.substr(0, buf.size() - 1);
                        } else {
                            element.clear();
                        }

                        buf = last_c;
                        sstate = stream_state::in_element_end;
                    }
                }

                buf += c;

                break;
            case stream_state::in_element_start:
                buf += c;

                if (c == '>') {
                    sstate = stream_state::in_boring;

                    opening = buf;
                    buf.clear();

                    gstate = gpx_state::have_opening_tag;
                }
                break;
            case stream_state::in_element_end:
                buf += c;
                if (c == '>') {
                    sstate = stream_state::in_boring;
                    gstate = gpx_state::have_closing_tag;
                    closing = buf;
                    buf.clear();
                }
                break;
        }

        switch (gstate) {
            case gpx_state::scanning:
                break;
            case gpx_state::have_opening_tag:
                // This rfind() works like starts_with()
                if (opening.rfind("<trkp", 0) == 0) {
                    in_trkpt = true;
                    std::sscanf(opening.c_str(), "<trkpt lat=\"%lf\" lon=\"%lf\"", &lat, &lon);
                }
                break;
            case gpx_state::have_closing_tag:
                if (in_trkpt) {
                    if (closing.rfind("</ti", 0) == 0) {
                        time = str_to_time_utc(element);
                    }
                    else if (closing.rfind("</el", 0) == 0) {
                        ele = atof(element.c_str());
                    }
                }
                
                if (closing.rfind("</trkp", 0) == 0) {
                    in_trkpt = false;
                    gpx_path.emplace_back(path_point{{ lat, lon, ele }, time, seq++});
                }
                break;
        }

        gstate = gpx_state::scanning;
    }

    return gpx_path;
}

inline bool save_gpx_trk(const std::string filename, const path::iterator start_it, const path::iterator end_it) {

    if (start_it == end_it)
        return false;
    
    static const std::string_view header = R"(<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
                <gpx version="1.1" xmlns="http://www.topografix.com/GPX/1/1" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="http://www.topografix.com/GPX/1/1 http://www.topografix.com/GPX/1/1/gpx.xsd" xmlns:oa="http://www.outdooractive.com/GPX/Extensions/1">
                  <metadata>
                  </metadata>
                  <trk>
                    <trkseg>
                )";
    
    static const std::string_view footer = R"(</trkseg>
                            </trk>
                            </gpx>)";
    
    std::ofstream out(filename);
    
    if (!out) {
        return false;
    }

    out << header;
    out.precision(10);
    
    /*
    Like: 
    <trkpt lat="52.988222" lon="-6.413189">
        <ele>166.27321</ele>
        <time>2022-05-07T10:20:08.000Z</time>
    </trkpt>    
    */
    
    for (auto i = start_it; i != end_it; ++i) {
        const auto lat = i->loc.lat;
        const auto lon = i->loc.lon;
        const auto time = time_to_str_utc(i->timestamp);

        out << "<trkpt lat=\"" << lat << "\"" << " lon=\"" << lon << "\">" << std::endl;
        out << "<ele>" << i->loc.ele << "</ele>" << std::endl;
        out << "<time>" << time << "</time>" << std::endl;
        out << "</trkpt>";
        out << std::endl;
    }

    out << footer;

    return true;
}

} // namespace