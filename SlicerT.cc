#define SLICERT_CC

template <typename M>
SlicerT<M>::SlicerT(M m)
{
  mesh_ = m;
}


template <typename M>
std::vector<std::vector<typename M::Point> >
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
    float h = 0.0;
    float layer_height = 0.1;
    int iters = diff / layer_height;

    std::vector<std::vector<Point> > toolpath;

    Eigen::MatrixXd A(3,3);

    for(int i = 0; i < iters; i++)
    {

      std::vector< Point > layer;

      Eigen::Vector3d p0(7,  0, h);
      Eigen::Vector3d p1(10, 4, h);
      Eigen::Vector3d p2(6,  3, h);

      
      EdgeIter eIt(mesh_.edges_begin());
      EdgeIter eEnd(mesh_.edges_end());
      for (eIt; eIt!=eEnd; ++eIt) 
      {
        // do something with *e_it, e_it->, or *e_it
        HalfedgeHandle heh = mesh_.halfedge_handle(*eIt,0);
        VertexHandle vh0 = mesh_.from_vertex_handle(heh);
        VertexHandle vh1 = mesh_.to_vertex_handle(heh);

        std::cout << "Edge: " <<  *eIt << " From " << mesh_.point(vh0) << " To " << mesh_.point(vh1) << "\n";

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
          layer.push_back(Point(intersection[0], intersection[1], intersection[2]));
        }
      }
      toolpath.push_back(layer);
      h += layer_height;

    }

    return toolpath;
}
