/**
 * jfp 2019
 * mieti - uminho
 */
#ifndef VAL_FACE_GEOFACE_FACTORY_HPP
#define VAL_FACE_GEOFACE_FACTORY_HPP

#include "NFD/daemon/face/face.hpp"
#include <memory>

namespace ns3 {
namespace ndn {
namespace val {
class ValForwarder;
namespace face {

class GeofaceFactory
{
public:
    GeofaceFactory(ValForwarder& valFwd);
    ~GeofaceFactory();

    std::shared_ptr<nfd::face::Face>
    makeGeoface();

private:
    ValForwarder& m_valFwd;    
};
/*
namespace ns3 {
namespace ndn {
namespace val {
namespace face {

class GeofaceFactory
{
public:
    GeofaceFactory();
    ~GeofaceFactory();

    std::shared_ptr<nfd::face::Face>
    makeGeoface();

private:
      
};*/



    
} // namespace face
} // namespace val
} // namespace ndn
} // namespace ns3

#endif