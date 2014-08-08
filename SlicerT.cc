#define SLICERT_CC

template <typename M>
SlicerT<M>::SlicerT(M m)
{
  mesh_ = m;
}


template <typename M>
std::vector<std::vector<typename M::Point > >
SlicerT<M>::getToolpath()
{

    VertexIter vIt(mesh_.vertices_begin());
    VertexIter vEnd(mesh_.vertices_end());
    VertexIter zMinVIt, zMaxVIt;
    float zMin, zMax;
    zMin, zMax = mesh_.point(*vIt)[2];
    zMinVIt, zMaxVIt = vIt;

    for (size_t count=0; vIt!=vEnd; ++vIt, ++count)
    {
      float z = mesh_.point(*vIt)[2];
      if(z < zMin)
      {
        zMin = z;
        zMinVIt = vIt;
      }
      else if(z > zMax)
      {
        zMax = z;
        zMaxVIt = vIt;
      }
    }

    float diff = zMax - zMin;
    float h = zMin;
    float layer_height = 0.1;
    int iters = diff / layer_height;

    std::vector<std::vector<Point > > layers;

    Eigen::MatrixXd A(3,3);

    for(int i = 0; i < iters; i++)
    {
      Eigen::Vector3d p0(7,  0, h);
      Eigen::Vector3d p1(10, 4, h);
      Eigen::Vector3d p2(6,  3, h);
      Point startPoint;
      bool polygon_not_closed = true;
      std::vector<Point> layer;

      FaceIter fIt(mesh_.faces_begin());
      FaceIter fEnd(mesh_.faces_end());
      FaceHandle currentFh;
      FaceHandle previousFh;
      bool foundFirstEdge = false;
      for (fIt; fIt!=fEnd; ++fIt)
      {
        FaceEdgeIter feIt = mesh_.fe_iter(*fIt);
        for(; feIt.is_valid(); ++feIt)
        {
          // Check if there is intersection with place on halfedge
          HalfedgeHandle heh = mesh_.halfedge_handle(*feIt,0);
          VertexHandle vh0 = mesh_.from_vertex_handle(heh);
          VertexHandle vh1 = mesh_.to_vertex_handle(heh);

          ////std::cout << "Edge: " <<  *eIt << " From " << mesh_.point(vh0) << " To " << mesh_.point(vh1) << "\n";

          //// POINT PLANE INTERSECTION
          Eigen::Vector3d la(mesh_.point(vh0)[0], mesh_.point(vh0)[1], mesh_.point(vh0)[2]);
          Eigen::Vector3d lb(mesh_.point(vh1)[0], mesh_.point(vh1)[1], mesh_.point(vh1)[2]);

          ////Ax = b
          Eigen::Vector3d b = la - p0;
          A.col(0) = la - lb;
          A.col(1) = p1 - p0;
          A.col(2) = p2 - p0;

          Eigen::Vector3d x = A.inverse() * b;

          if((x[0] >= 0.0) && (x[0] <= 1.0))
          {
            Eigen::Vector3d intersection = la + (lb - la) * x[0];
            Point p = Point(intersection[0], intersection[1], intersection[2]);
            //layer.push_back(IntersectionT<M>(p, vh0, vh1));
            startPoint = p;
            layer.push_back(p);
            currentFh = *fIt;
            foundFirstEdge = true;
          }

          if(foundFirstEdge) break;
        }
        if(foundFirstEdge) break;
      }

      std::vector<FaceHandle> seenFaces;
      seenFaces.push_back(currentFh);
      while(polygon_not_closed)
      {
        if(currentFh == previousFh) break;
        previousFh = currentFh;
        FaceFaceIter ffIt = mesh_.ff_iter(currentFh);
        for(; ffIt.is_valid(); ++ffIt)
        {
          if(std::find(seenFaces.begin(), seenFaces.end(), *ffIt)!=seenFaces.end())
            continue;
          seenFaces.push_back(*ffIt);
          FaceEdgeIter feIt = mesh_.fe_iter(*ffIt);
          bool foundEdge = false;
          for(; feIt.is_valid(); ++feIt)
          {
            // Check if there is intersection with place on halfedge
            HalfedgeHandle heh = mesh_.halfedge_handle(*feIt,0);
            VertexHandle vh0 = mesh_.from_vertex_handle(heh);
            VertexHandle vh1 = mesh_.to_vertex_handle(heh);

            //std::cout << "Edge: " <<  *eIt << " From " << mesh_.point(vh0) << " To " << mesh_.point(vh1) << "\n";

            // POINT PLANE INTERSECTION
            Eigen::Vector3d la(mesh_.point(vh0)[0], mesh_.point(vh0)[1], mesh_.point(vh0)[2]);
            Eigen::Vector3d lb(mesh_.point(vh1)[0], mesh_.point(vh1)[1], mesh_.point(vh1)[2]);

            //Ax = b
            Eigen::Vector3d b = la - p0;
            A.col(0) = la - lb;
            A.col(1) = p1 - p0;
            A.col(2) = p2 - p0;

            Eigen::Vector3d x = A.inverse() * b;

            if((x[0] >= 0.0) && (x[0] <= 1.0))
            {
              Eigen::Vector3d intersection = la + (lb - la) * x[0];
              Point p = Point(intersection[0], intersection[1], intersection[2]);
              //layer.push_back(IntersectionT<M>(p, vh0, vh1));
              layer.push_back(p);
              currentFh = *ffIt;
              foundEdge = true;
            }
            if(foundEdge) break;

          }
        }
      }
      std::cout << layer.size() << "\n";
      layers.push_back(layer);
      h += layer_height;
    }

    return layers;
}
