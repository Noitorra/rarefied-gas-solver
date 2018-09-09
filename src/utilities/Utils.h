#ifndef RGS_UTILS_H
#define RGS_UTILS_H

#include <sstream>
#include <vector>
#include <chrono>

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/shared_ptr.hpp>

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

    template<typename T>
    static std::string toString(const std::vector<T>& vector) {
        std::stringstream ss;
        ss << "[";
        for (unsigned int i = 0; i < vector.size(); i++) {
            if (i != 0) {
                ss << ", ";
            }
            ss << vector[i];
        }
        ss << "]";
        return ss.str();
    }

    template<typename Enumeration>
    static inline auto asNumber(Enumeration const value) -> typename std::underlying_type<Enumeration>::type {
        return static_cast<typename std::underlying_type<Enumeration>::type>(value);
    }

    template<typename Enumeration>
    static inline auto asNumberVector(std::vector<Enumeration> const values) -> std::vector<typename std::underlying_type<Enumeration>::type> {
        std::vector<typename std::underlying_type<Enumeration>::type> basicValues;
        for (unsigned int i = 0; i < values.size(); i++) {
            basicValues.push_back(static_cast<typename std::underlying_type<Enumeration>::type>(values[i]));
        }
        return basicValues;
    }

    template<class T>
    static std::string serialize(T object) {
        std::ostringstream os;
        boost::archive::binary_oarchive oa(os);
        setUpOutArchive(oa);
        oa << object;
        return os.str();
    }

    template<class T>
    static void deserialize(const std::string& buffer, T& object) {
        std::istringstream is(buffer);
        boost::archive::binary_iarchive ia(is);
        setUpInArchive(ia);
        ia >> object;
    }

    static std::string getCurrentDateAndTime();

private:
    static void setUpOutArchive(boost::archive::binary_oarchive& oa);
    static void setUpInArchive(boost::archive::binary_iarchive& ia);

};


#endif //RGS_UTILS_H
