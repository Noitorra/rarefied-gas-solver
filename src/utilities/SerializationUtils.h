#ifndef RGS_SERIALIZATIONUTILS_H
#define RGS_SERIALIZATIONUTILS_H

#include <sstream>

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/map.hpp>

class SerializationUtils {
public:
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

private:
    static void setUpOutArchive(boost::archive::binary_oarchive& oa);
    static void setUpInArchive(boost::archive::binary_iarchive& ia);

};


#endif //RGS_SERIALIZATIONUTILS_H
