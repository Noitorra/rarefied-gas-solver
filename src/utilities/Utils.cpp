#include "Utils.h"
#include "mesh/Point.h"
#include "mesh/Line.h"
#include "mesh/Triangle.h"
#include "mesh/Quadrangle.h"

#include <boost/serialization/export.hpp>

std::string Utils::getCurrentDateAndTime() {
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    char buffer[100];
    if (std::strftime(buffer, sizeof(buffer), "%Y-%m-%d_%H-%M-%S", std::localtime(&now_time_t)) > 0) {
        return std::string(buffer);
    } else {
        return "unknown";
    }
}

void Utils::setUpOutArchive(boost::archive::binary_oarchive& oa) {
    oa.register_type<Point>();
    oa.register_type<Line>();
    oa.register_type<Triangle>();
    oa.register_type<Quadrangle>();
}

void Utils::setUpInArchive(boost::archive::binary_iarchive& ia) {
    ia.register_type<Point>();
    ia.register_type<Line>();
    ia.register_type<Triangle>();
    ia.register_type<Quadrangle>();
}
