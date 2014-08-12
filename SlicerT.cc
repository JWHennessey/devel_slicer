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
        std::cout << layerSize << " / " << intersections.size() << "\n";

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
              currentFh = *fIt;
              foundFirstEdge = true;
              //layerSize++;
            }
            if(foundFirstEdge) break;
          }
          if(foundFirstEdge) break;
        }

        
        //Find all of the other edges at this height
        while(true)
        {
          if(currentFh == previousFh) break;

          previousFh = currentFh;
          seenFaces.push_back(currentFh);
          //FaceFaceIter ffIt = mesh_.ff_iter(currentFh);
          FaceEdgeIter feIt = mesh_.fe_iter(currentFh);
          for(; feIt.is_valid(); ++feIt)
          {
              if(std::find(seenEdges.begin(), seenEdges.end(), *feIt)!=seenEdges.end())
                continue;
              seenEdges.push_back(*feIt);
              HalfedgeHandle heh = mesh_.halfedge_handle(*feIt,0);
              if(linePlaneIntersection(&layerSection, heh, h))
              {
                layerSize++;
                //HalfedgeHandle opposite_heh = mesh_.opposite_halfedge_handle(heh);
                std::cout << "Before currentFh \n";
                currentFh = mesh_.opposite_face_handle(heh);
                std::cout << "Updated currentFh \n";
                break;
              }
           }

          //if(currentFh == previousFh)
          //{
            //FaceFaceIter ffIt = mesh_.ff_iter(currentFh);
            //for(; ffIt.is_valid(); ++ffIt)
            //{
              //if(std::find(seenFaces.begin(), seenFaces.end(), *ffIt)!=seenFaces.end())
                //continue;
              
              //seenFaces.push_back(*ffIt);
              //currentFh = *ffIt;
              //break;
            //}
          //}

          //for(; ffIt.is_valid(); ++ffIt)
          //{
            //if(std::find(seenFaces.begin(), seenFaces.end(), *ffIt)!=seenFaces.end())
              //continue;
            //seenFaces.push_back(*ffIt);
            //std::vector<Point> candidates;
            //FaceEdgeIter feIt = mesh_.fe_iter(*ffIt);
            //for(; feIt.is_valid(); ++feIt)
            //{
              //if(std::find(seenEdges.begin(), seenEdges.end(), *feIt)!=seenEdges.end())
                //continue;
              //HalfedgeHandle heh = mesh_.halfedge_handle(*feIt,0);
              //if(linePlaneIntersection(&candidates, heh, h))
              //{
                //currentFh = *ffIt;
                //seenEdges.push_back(*feIt);
                //layerSize++;
                ////break;
              //}
            //}
          //}
        }
        layer.push_back(layerSection);
      }
      layers.push_back(layer);
      h += layer_height;
      std::cout << "Layer " << layers.size() << " vertex no. " <<  layer.size() << "\n";
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
