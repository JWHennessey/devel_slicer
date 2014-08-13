#define SLICERT_CC

template <typename M>
SlicerT<M>::SlicerT(M m, double lh)
{
  mesh_ = m;
  layer_height = lh;
}

template <typename M>
std::pair <float, float>
SlicerT<M>::zAxisMinMax()
{
  VertexIter vIt(mesh_.vertices_begin());
  VertexIter vEnd(mesh_.vertices_end());
  float zMin, zMax = mesh_.point(*vIt)[2];

  for (size_t count=0; vIt!=vEnd; ++vIt, ++count)
  {
    float z = mesh_.point(*vIt)[2];
    if(z < zMin)
    {
      zMin = z;
    }
    else if(z > zMax)
    {
      zMax = z;
    }
  }
  std::pair <float, float> p(zMin, zMax);
  return p;
}

template <typename M>
bool
SlicerT<M>::linePlaneIntersection(std::vector<Point> *layer, HalfedgeHandle heh, float h)
{
    VertexHandle vh0 = mesh_.from_vertex_handle(heh);
    VertexHandle vh1 = mesh_.to_vertex_handle(heh);

    Eigen::Vector3d la(mesh_.point(vh0)[0], mesh_.point(vh0)[1], mesh_.point(vh0)[2]);
    Eigen::Vector3d lb(mesh_.point(vh1)[0], mesh_.point(vh1)[1], mesh_.point(vh1)[2]);

    Eigen::Vector3d p0(7,  0, h);
    Eigen::Vector3d p1(10, 4, h);
    Eigen::Vector3d p2(6,  3, h);

    //Ax = b
    Eigen::Vector3d b = la - p0;
    Eigen::MatrixXd A(3,3);
    A.col(0) = la - lb;
    A.col(1) = p1 - p0;
    A.col(2) = p2 - p0;

    Eigen::Vector3d x = A.inverse() * b;

    if((x[0] >= 0.0) && (x[0] <= 1.0))
    {
      Eigen::Vector3d intersection = la + (lb - la) * x[0];
      Point p = Point(intersection[0], intersection[1], intersection[2]);
      layer->push_back(p);
      return true;
    }
    return false;
}

template <typename M>
std::vector<std::vector<std::vector<typename M::Point > > >
SlicerT<M>::getToolpath()
{

    float zMin, zMax;
    std::pair <float, float> zAxisBoundry = zAxisMinMax();
    zMin = zAxisBoundry.first;
    zMax = zAxisBoundry.second;

    float diff = zMax - zMin;
    float h = zMin;
    int iters = diff / layer_height;

    //h += layer_height;
    for(int i = 0; i < iters; i++)
    {
      std::vector<std::vector<Point> > layer;
      std::vector<Point> intersections;
      std::vector<EdgeHandle> seenEdges;
      std::vector<EdgeHandle> toVisitEdges;
      size_t layerSize = 0;

      EdgeIter eIt(mesh_.edges_begin());
      EdgeIter eEnd(mesh_.edges_end());
      for(eIt; eIt!=eEnd; eIt++)
      {
        HalfedgeHandle heh = mesh_.halfedge_handle(*eIt,0);
        if(linePlaneIntersection(&intersections, heh, h))
        {
          toVisitEdges.push_back(*eIt);
        }
      }

      //delete intersections;

      EdgeHandle currentEh;
      EdgeHandle previousEh;

      //Find the first edge at this height
      while(layerSize < toVisitEdges.size())
      {
        std::vector<Point> layerSection;
        size_t i = 0;
        for(; i < toVisitEdges.size(); i++)
        {
          currentEh = toVisitEdges.at(i);
          if(std::find(seenEdges.begin(), seenEdges.end(), currentEh)!=seenEdges.end())
            continue;
          seenEdges.push_back(currentEh);
          HalfedgeHandle heh = mesh_.halfedge_handle(currentEh,0);
          linePlaneIntersection(&layerSection, heh, h);
          layerSize++;
          break;
        }

        while(true)
        {
          if(currentEh == previousEh) break;

          previousEh = currentEh;

          HalfedgeHandle heh = mesh_.halfedge_handle(currentEh,0);
          std::vector<VertexHandle> vhs;
          vhs.push_back(mesh_.from_vertex_handle(heh));
          vhs.push_back(mesh_.to_vertex_handle(heh));
          bool edgeFound = false;
          for(size_t i = 0; i < vhs.size(); i++)
          {
            VertexEdgeIter veIt = mesh_.ve_iter(vhs.at(i));
            for(; veIt.is_valid(); ++veIt)
            {
              if(std::find(seenEdges.begin(), seenEdges.end(), *veIt)!=seenEdges.end())
                continue;
              seenEdges.push_back(*veIt);
              HalfedgeHandle heh = mesh_.halfedge_handle(*veIt,0);
              if(linePlaneIntersection(&layerSection, heh, h))
              {
                layerSize++;
                currentEh = *veIt;
                edgeFound = true;
              }
              //if(edgeFound) break;
            }
            //if(edgeFound) break;
          }
        }
        layer.push_back(layerSection);
      }
      layers.push_back(layer);
      h += layer_height;
      std::cout << "Layer " << layers.size() << " no. layer parts " <<  layer.size() << "\n";
    }
    return layers;
}


template <typename M>
void
SlicerT<M>::resampleLayerSection(std::vector<Point>* layerSection)
{
  //std::vector<Point> newLayerSection;
  //for(size_t i = 1; i < layerSection->size(); i++)
  //{
    //double distCount = 0.0;
    //size_t pointsAdded = 0;

    //Point a = layerSection->at(i-1);
    //Point b = layerSection->at(i);
    //Point direction = b - a;
    //Eigen::Vector3d aVec(a[0], a[1], a[2]);

    //Eigen::Vector3d d(direction[0], direction[1], direction[2]);
    //int iters = d.norm() / 0.1;
    ////Point c = a + (b - a) * 0.1;
    //newLayerSection.push_back(a);
    //for(int j = 1; j < iters; j++)
    //{
      //Eigen::Vector3d xVec = aVec + (j * d);
      //Point x(xVec[0], xVec[1],xVec[2]);
      //newLayerSection.push_back(x);
    //}
    //newLayerSection.push_back(b);
  //}
  //layerSection = &newLayerSection;
}

