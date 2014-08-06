#ifndef SLICERT_HH
#define SLICERT_HH

#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/IO/Options.hh>
#include <OpenMesh/Core/Mesh/Attributes.hh>


template <typename M>
class SlicerT
{
public:
  typedef M        Mesh;
  typedef typename Mesh::Point Point;
  SlicerT<M>(M m);
  std::vector<std::vector<typename M::Point> > getToolpath();
private:
  Mesh mesh_;

};


#if defined(OM_INCLUDE_TEMPLATES) && !defined(SLICERT_CC)
#  define OPENMESH_MESHVIEWERWIDGET_TEMPLATES
#  include "SlicerT.cc"
#endif
//=============================================================================
#endif 
