#ifndef SLICERT_HH
#define SLICERT_HH

#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/IO/Options.hh>
#include <OpenMesh/Core/Mesh/Attributes.hh>
#include "Eigen/Dense"

template <typename M>
class SlicerT
{
public:
  typedef M        Mesh;
  typedef typename Mesh::Point Point;
  typedef typename Mesh::VertexIter VertexIter;
  typedef typename Mesh::EdgeIter EdgeIter;
  typedef typename Mesh::HalfedgeHandle HalfedgeHandle;
  typedef typename Mesh::VertexHandle VertexHandle;
  
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
