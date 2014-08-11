#define SLICERT_CC

template <typename M>
SlicerT<M>::SlicerT(M m)
{
  mesh_ = m;
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
    float layer_height = 0.1;
    int iters = diff / layer_height;

    h += 0.1;
    for(int i = 0; i < iters; i++)
    {
      std::vector<std::vector<Point> > layer;
      std::vector<Point> intersections;
      std::vector<EdgeHandle> seenEdges;
      std::vector<FaceHandle> seenFaces;
      size_t layerSize = 0;

      EdgeIter eIt(mesh_.edges_begin());
      EdgeIter eEnd(mesh_.edges_end());
      for(eIt; eIt!=eEnd; eIt++)
      {
        HalfedgeHandle heh = mesh_.halfedge_handle(*eIt,0);
        linePlaneIntersection(&intersections, heh, h);
      }

      FaceIter fIt(mesh_.faces_begin());
      FaceIter fEnd(mesh_.faces_end());
      FaceHandle currentFh;
      FaceHandle previousFh;
      //Find the first edge at this height
      while(layerSize < intersections.size())
      {
        std::vector<Point> layerSection;
        //std::cout << layers.size() << " / " << intersections.size() << "\n";
        bool foundFirstEdge = false;

        for (fIt; fIt!=fEnd; ++fIt)
        {
          FaceEdgeIter feIt = mesh_.fe_iter(*fIt);
          for(; feIt.is_valid(); ++feIt)
          {
            if(std::find(seenEdges.begin(), seenEdges.end(), *feIt)!=seenEdges.end())
              continue;

            HalfedgeHandle heh = mesh_.halfedge_handle(*feIt,0);
            if(linePlaneIntersection(&layerSection, heh, h))
            {
              seenEdges.push_back(*feIt);
              currentFh = *fIt;
              foundFirstEdge = true;
              layerSize++;
            }
            if(foundFirstEdge) break;
          }
          if(foundFirstEdge) break;
        }

        seenFaces.push_back(currentFh);
        //Find all of the other edges at this height
        while(true)
        {
          if(currentFh == previousFh) break;

          // If I remove this I get segmentation fault ?!?!??!
          std::cout << "";

          previousFh = currentFh;
          FaceFaceIter ffIt = mesh_.ff_iter(currentFh);
          for(; ffIt.is_valid(); ++ffIt)
          {
            if(std::find(seenFaces.begin(), seenFaces.end(), *ffIt)!=seenFaces.end())
              continue;
            seenFaces.push_back(*ffIt);
            FaceEdgeIter feIt = mesh_.fe_iter(*ffIt);
            for(; feIt.is_valid(); ++feIt)
            {
              if(std::find(seenEdges.begin(), seenEdges.end(), *feIt)!=seenEdges.end())
                continue;
              seenEdges.push_back(*feIt);
              HalfedgeHandle heh = mesh_.halfedge_handle(*feIt,0);
              if(linePlaneIntersection(&layerSection, heh, h))
              {
                currentFh = *ffIt;
                layerSize++;
              }
            }
          }
        }
        layer.push_back(layerSection);
      }
      layers.push_back(layer);
      h += layer_height;
      std::cout << "Layer " << layers.size() << " vertex no. " <<  layer.size() << "\n";
    }

    return layers;
}
