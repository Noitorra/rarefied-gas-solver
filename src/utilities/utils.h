#ifndef RGS_UTILS_H
#define RGS_UTILS_H

#include <sstream>
#include <vector>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>

const double BOLTZMANN_CONSTANT = 1.38e-23; // Boltzmann const // TODO: Make more precise

class Utils {
public:
    template<typename T>
    static inline int sgn(T val) {
        return (T(0) < val) - (val < T(0));
    }

    template<typename T>
    static std::string toString(T const &value) {
        std::stringstream ss;
        ss << value;
        return ss.str();
    }

    template<typename Enumeration>
    static inline auto asInteger(Enumeration const value) -> typename std::underlying_type<Enumeration>::type {
        return static_cast<typename std::underlying_type<Enumeration>::type>(value);
    }

    template<class T>
    static const char* serialize(T object) {
        std::ostringstream os;
        boost::archive::text_oarchive oa(os);
        oa << object;
        return os.str().c_str();
    }

    template<class T>
    static void deserialize(const char* buffer, T& object) {
        std::istringstream is(buffer);
        boost::archive::text_iarchive ia(is);
        ia >> object;
    }
};


#endif //RGS_UTILS_H
