#ifndef SLICERT_HH
#define SLICERT_HH

#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/IO/Options.hh>
#include <OpenMesh/Core/Mesh/Attributes.hh>
#include "Eigen/Dense"


template <typename M>
class IntersectionT
{
public:
  IntersectionT<M>(
      typename M::Point p, 
      typename M::VertexHandle f, 
      typename M::VertexHandle t)
  {
    intersection = p;
    from = f;
    to = to;
  }
private:
  typename M::Point intersection;
  typename M::VertexHandle from;
  typename M::VertexHandle to;
  typename M::Point getIntersection(){ return intersection; };
  typename M::VertexHandle getFrom(){ return from; };
  typename M::VertexHandle getTo(){ return to; };

};


template <typename M>
class SlicerT
{
public:
  typedef M        Mesh;
  typedef typename Mesh::Point Point;
  typedef typename Mesh::VertexIter VertexIter;
  typedef typename Mesh::FaceIter FaceIter;
  typedef typename Mesh::FaceEdgeIter FaceEdgeIter;
  typedef typename Mesh::FaceFaceIter FaceFaceIter;
  typedef typename Mesh::EdgeIter EdgeIter;
  typedef typename Mesh::HalfedgeHandle HalfedgeHandle;
  typedef typename Mesh::VertexHandle VertexHandle;
  typedef typename Mesh::FaceHandle FaceHandle;
  typedef typename Mesh::EdgeHandle EdgeHandle;
  
  SlicerT<M>(M m);
  std::vector<std::vector<typename M::Point > > getToolpath();

private:
  Mesh mesh_;
  std::vector<std::vector<Point > > layers;
  
  std::pair <float, float> zAxisMinMax();
  bool linePlaneIntersection(std::vector<Point> *layer, HalfedgeHandle heh, float h);

};




#if defined(OM_INCLUDE_TEMPLATES) && !defined(SLICERT_CC)
#  define OPENMESH_MESHVIEWERWIDGET_TEMPLATES
#  include "SlicerT.cc"
#endif
//=============================================================================
#endif 
