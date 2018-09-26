#include "SerializationUtils.h"
#include "mesh/Point.h"
#include "mesh/Line.h"
#include "mesh/Triangle.h"
#include "mesh/Quadrangle.h"
#include "mesh/Tetrahedron.h"
#include "mesh/Hexahedron.h"
#include "mesh/Prism.h"

#include <boost/serialization/export.hpp>


void SerializationUtils::setUpOutArchive(boost::archive::binary_oarchive& oa) {
    oa.register_type<Point>();
    oa.register_type<Line>();
    oa.register_type<Triangle>();
    oa.register_type<Quadrangle>();
    oa.register_type<Tetrahedron>();
    oa.register_type<Hexahedron>();
    oa.register_type<Prism>();
}

void SerializationUtils::setUpInArchive(boost::archive::binary_iarchive& ia) {
    ia.register_type<Point>();
    ia.register_type<Line>();
    ia.register_type<Triangle>();
    ia.register_type<Quadrangle>();
    ia.register_type<Tetrahedron>();
    ia.register_type<Hexahedron>();
    ia.register_type<Prism>();
}