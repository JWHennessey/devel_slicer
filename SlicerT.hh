#ifndef SLICERT_HH
#define SLICERT_HH

#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/IO/Options.hh>
#include <OpenMesh/Core/Mesh/Attributes.hh>
#include "Eigen/Dense"
#include <unordered_map>
#include <boost/config.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/connected_components.hpp>
#include "PrintheadT.hh"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

using namespace boost;


typedef typename OpenMesh::TriMesh_ArrayKernelT<OpenMesh::DefaultTraits>::Point Point;

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


struct Vertex
{
  OpenMesh::TriMesh_ArrayKernelT<OpenMesh::DefaultTraits>::Point intersection;
  int edgeId;
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
  typedef typename Mesh::FaceVertexIter FaceVertexIter;
  typedef typename Mesh::VertexEdgeIter VertexEdgeIter;
  typedef typename Mesh::EdgeIter EdgeIter;
  typedef typename Mesh::HalfedgeHandle HalfedgeHandle;
  typedef typename Mesh::VertexHandle VertexHandle;
  typedef typename Mesh::FaceHandle FaceHandle;
  typedef typename Mesh::EdgeHandle EdgeHandle;
  SlicerT<M>(M m, double lh, bool cl, bool sample);
  std::vector<std::vector<std::vector<typename M::Point > > > getToolpath();
  std::vector<std::vector<std::vector<typename M::Point > > > getToolpathGraph();
  std::vector<std::vector<std::vector<typename M::Point > > >  getCurvature();
  void writeGcode(double offset);

private:
  Mesh mesh_;
  std::vector<std::vector<std::vector<Point > > > layers;
  std::vector<std::vector<std::vector<Point > > > layersOriginal;
  std::vector<std::vector<std::vector<Point > > > curvature;
  std::vector<std::vector<std::vector<Point > > > gcode;
  const bool completeLoop;
  const double layer_height;
  const bool resample;
  const int wall_thickness;
  const float LINE_WIDTH;
  const float platformCenterX;
  const float platformCenterY;
  std::pair <float, float> zAxisMinMax();
  bool linePlaneIntersection(std::vector<Point> *layer, HalfedgeHandle heh, float h);
  void resampleLayerSection(std::vector<Point>* layerSection, std::vector<Point>* newLayerSection);
  std::vector<typename M::Point> computeLayerSectionCurvature(const std::vector<Point>& layerSection);
  void appendFile(std::ofstream *output, const char* filename);

};




#if defined(OM_INCLUDE_TEMPLATES) && !defined(SLICERT_CC)
#  define OPENMESH_MESHVIEWERWIDGET_TEMPLATES
#  include "SlicerT.cc"
#endif
//=============================================================================
#endif 
