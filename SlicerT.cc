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
    std::vector< Point > v1;
    v1.push_back( Point(0,0,0) );
    v1.push_back( Point(5,5,5) );
    std::vector< Point > v2;
    v2.push_back( Point(10,0,0) );
    v2.push_back( Point(10,10,0) );
    v2.push_back( Point(10,10,10) );
    std::vector<std::vector<Point> > toolpath;
    toolpath.push_back(v1);
    toolpath.push_back(v2);
    return toolpath;
}
