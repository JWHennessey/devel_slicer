/*===========================================================================*\
 *                                                                           *
 *                               OpenMesh                                    *
 *      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen      *
 *                           www.openmesh.org                                *
 *                                                                           *
 *---------------------------------------------------------------------------* 
 *  This file is part of OpenMesh.                                           *
 *                                                                           *
 *  OpenMesh is free software: you can redistribute it and/or modify         * 
 *  it under the terms of the GNU Lesser General Public License as           *
 *  published by the Free Software Foundation, either version 3 of           *
 *  the License, or (at your option) any later version with the              *
 *  following exceptions:                                                    *
 *                                                                           *
 *  If other files instantiate templates or use macros                       *
 *  or inline functions from this file, or you compile this file and         *
 *  link it with other files to produce an executable, this file does        *
 *  not by itself cause the resulting executable to be covered by the        *
 *  GNU Lesser General Public License. This exception does not however       *
 *  invalidate any other reasons why the executable file might be            *
 *  covered by the GNU Lesser General Public License.                        *
 *                                                                           *
 *  OpenMesh is distributed in the hope that it will be useful,              *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU Lesser General Public License for more details.                      *
 *                                                                           *
 *  You should have received a copy of the GNU LesserGeneral Public          *
 *  License along with OpenMesh.  If not,                                    *
 *  see <http://www.gnu.org/licenses/>.                                      *
 *                                                                           *
\*===========================================================================*/ 

/*===========================================================================*\
 *                                                                           *             
 *   $Revision: 990 $                                                         *
 *   $Date: 2014-02-05 10:01:07 +0100 (Mi, 05 Feb 2014) $                   *
 *                                                                           *
\*===========================================================================*/

#define OPENMESHAPPS_MESHVIEWERWIDGET_CC

//== INCLUDES =================================================================

#ifdef _MSC_VER
//#  pragma warning(disable: 4267 4311)
#endif

//
#include <iostream>
#include <fstream>
// --------------------
#include <QImage>
#include <QFileInfo>
#include <QKeyEvent>
// --------------------
#include <OpenMesh/Core/Utils/vector_cast.hh>
#include <OpenMesh/Tools/Utils/Timer.hh>
#include <OpenMesh/Apps/Slicer/MeshViewerWidgetT.hh>

using namespace OpenMesh;
using namespace Qt;

#if defined(_MSC_VER)
#  undef min
#  undef max
#endif

using namespace Qt;
//== IMPLEMENTATION ========================================================== 

template <typename M>
void
MeshViewerWidgetT<M>::toggleOutline()
{
  outlinePath = !outlinePath;
}

template <typename M>
void
MeshViewerWidgetT<M>::toggleResample()
{
  resample = !resample;
}

template <typename M>
void
MeshViewerWidgetT<M>::toggleDisplay1()
{
  turnAllDisplayOff();
  displayPath = true;
  displayPoints = true;
  updateGL();
}

template <typename M>
void
MeshViewerWidgetT<M>::toggleDisplay2()
{
  turnAllDisplayOff();
  displayPoints = true;
  updateGL();
}

template <typename M>
void
MeshViewerWidgetT<M>::toggleDisplay3()
{
  turnAllDisplayOff();
  displayPath = true;
  updateGL();
}

template <typename M>
void
MeshViewerWidgetT<M>::toggleDisplay4()
{
  turnAllDisplayOff();
  displayCurvature = true;
  updateGL();
}

template <typename M>
void
MeshViewerWidgetT<M>::toggleDisplay5()
{
  std::cout << "Display 5\n";
  turnAllDisplayOff();
  displayGcode = true;
  updateGL();
}

template <typename M>
void
MeshViewerWidgetT<M>::turnAllDisplayOff()
{
  displayPath = false;
  displayPoints = false;
  displayCurvature = false;
  displayGcode = false;
}


template <typename M>
void
MeshViewerWidgetT<M>::rotate(int deg, Eigen::Vector3f axis)
{
  float angle_in_radian = (deg * M_PI) / 180;
  Eigen::AngleAxis<float> aa(angle_in_radian, axis);
  Eigen::Affine3f t(aa);

  VertexIter vIt(mesh_.vertices_begin());
  VertexIter vEnd(mesh_.vertices_end());
  
  for (vIt; vIt!=vEnd; ++vIt)
  {
    Point p = mesh_.point(*vIt);
    Eigen::Vector3f x(p[0], p[1], p[2]);
    x = t * x;
    Point px(x[0], x[1], x[2]);
    mesh_.point(*vIt) = px;
  }
  updateGL();
}

template <typename M>
void
MeshViewerWidgetT<M>::rotateX(int deg)
{
  rotate(deg, Eigen::Vector3f(1,0,0));
}

template <typename M>
void
MeshViewerWidgetT<M>::rotateY(int deg)
{
  rotate(deg, Eigen::Vector3f(0,1,0));
}

template <typename M>
void
MeshViewerWidgetT<M>::rotateZ(int deg)
{
  rotate(deg, Eigen::Vector3f(0,0,1));
}


template <typename M>
int
MeshViewerWidgetT<M>::getLayerCount()
{
  return toolpath.size();
}


template <typename M>
int
MeshViewerWidgetT<M>::getLineNumber()
{
  lineNumber = 0;
  for(size_t i = 0; i< toolpath[layerHeight-1].size(); i++)
    lineNumber += toolpath[layerHeight-1][i].size();
  return lineNumber;
}


template <typename M>
void
MeshViewerWidgetT<M>::completeLoopToggle(bool value)
{
  completeLoop = value;
  std::cout << "completeLoopToggle" << std::endl;
}


template <typename M>
void
MeshViewerWidgetT<M>::setLayerHeight(int value)
{
  layerHeight = value;
  if(layerHeight >= 1)
    lineNumber = getLineNumber();
  updateGL();
}


template <typename M>
void
MeshViewerWidgetT<M>::setLineNumber(int value)
{
  lineNumber = value;
  updateGL();
}


template <typename M>
void 
MeshViewerWidgetT<M>::slice_mesh_alt(double layerHeight, double offset, int wallthickness)
{
   SlicerT<M> slicer = SlicerT<M>(mesh_, layerHeight, resample, wallthickness, outlinePath);
   toolpath = slicer.getToolpathGraph();
   layerHeight = toolpath.size();
   lineNumber = getLineNumber();
   QAction *a = findAction("Gcode");
   a->setChecked(true);
   slotDrawMode(a);
   curvature = slicer.getCurvature();
   slicer.writeGcode(offset);
   //draw_mode_ = 0;
   //draw_openmesh("Gcode");
}


template <typename M>
void 
MeshViewerWidgetT<M>::slice_mesh(double layerHeight, double offset, int wallthickness)
{
   SlicerT<M> slicer = SlicerT<M>(mesh_, layerHeight, resample, wallthickness, outlinePath);
   toolpath = slicer.getToolpath();
   layerHeight = toolpath.size();
   lineNumber = getLineNumber();
   QAction *a = findAction("Gcode");
   a->setChecked(true);
   slotDrawMode(a);
   curvature = slicer.getCurvature();
   slicer.writeGcode(offset);

   //draw_mode_ = 0;
   //draw_openmesh("Gcode");
}

template <typename M>
QDialog
*MeshViewerWidgetT<M>::createDialog(const QString &windowTitle) const
{
  QDialog *dialog = new QDialog(0, Qt::CustomizeWindowHint | Qt::WindowTitleHint);
  
  dialog->setWindowOpacity(0.8);
  dialog->setWindowTitle(windowTitle);
  dialog->setLayout(new QVBoxLayout);
  
  return dialog;
}

template <typename M>
MeshViewerWidgetT<M>::MeshViewerWidgetT(QWidget* _parent)
    : QGLViewerWidget(_parent),
    completeLoop(false),
      displayPath(true),
      displayPoints(true),
      displayCurvature(false),
      outlinePath(true),
      resample(false),
      f_strips_(false), 
      tex_id_(0),
      tex_mode_(GL_MODULATE),
      strips_(mesh_),
      use_color_(true),
      show_vnormals_(false),
      show_fnormals_(false)

{
    add_draw_mode("Points");
    add_draw_mode("Hidden-Line");
#if defined(OM_USE_OSG) && OM_USE_OSG
    add_draw_mode("OpenSG Indices");
#endif

    std::cout << "MeshViewWidgetT Init \n";
    //QWidget *controls = createDialog(tr("Controls"));

    //QWidget *widgets[] = { controls };

    //for (uint i = 0; i < sizeof(widgets) / sizeof(*widgets); ++i) {
      //QGraphicsProxyWidget *proxy = new QGraphicsProxyWidget(0, Qt::Dialog);
      //proxy->setWidget(widgets[i]);
      //addItem(proxy);
    //}
}


template <typename M>
bool 
MeshViewerWidgetT<M>::open_mesh(const char* _filename, IO::Options _opt)
{
  // load mesh
  // calculate normals
  // set scene center and radius   
  
  mesh_.request_face_normals();
  mesh_.request_face_colors();
  mesh_.request_vertex_normals();
  mesh_.request_vertex_colors();
  mesh_.request_vertex_texcoords2D();
  
  std::cout << "Loading from file '" << _filename << "'\n";
  if ( IO::read_mesh(mesh_, _filename, _opt ))
  {
    // store read option
    opt_ = _opt;
    
    // update face and vertex normals     
    if ( ! opt_.check( IO::Options::FaceNormal ) )
      mesh_.update_face_normals();
    else
      std::cout << "File provides face normals\n";
    
    if ( ! opt_.check( IO::Options::VertexNormal ) )
      mesh_.update_vertex_normals();
    else
      std::cout << "File provides vertex normals\n";


    // check for possible color information
    if ( opt_.check( IO::Options::VertexColor ) )
    {
      std::cout << "File provides vertex colors\n";
      add_draw_mode("Colored Vertices");
    }
    else
      mesh_.release_vertex_colors();

    if ( _opt.check( IO::Options::FaceColor ) )
    {
      std::cout << "File provides face colors\n";
      add_draw_mode("Solid Colored Faces");
      add_draw_mode("Smooth Colored Faces");
    }
    else
      mesh_.release_face_colors();

    if ( _opt.check( IO::Options::VertexTexCoord ) )
      std::cout << "File provides texture coordinates\n";


    // bounding box
    typename Mesh::ConstVertexIter vIt(mesh_.vertices_begin());
    typename Mesh::ConstVertexIter vEnd(mesh_.vertices_end());      
    
    typedef typename Mesh::Point Point;
    using OpenMesh::Vec3f;
    
    Vec3f bbMin, bbMax;
    
    bbMin = bbMax = OpenMesh::vector_cast<Vec3f>(mesh_.point(*vIt));
    
    for (size_t count=0; vIt!=vEnd; ++vIt, ++count)
    {
      bbMin.minimize( OpenMesh::vector_cast<Vec3f>(mesh_.point(*vIt)));
      bbMax.maximize( OpenMesh::vector_cast<Vec3f>(mesh_.point(*vIt)));
    }
    
    
    // set center and radius
    set_scene_pos( (bbMin+bbMax)*0.5, (bbMin-bbMax).norm()*0.5 );
    
    // for normal display
    normal_scale_ = (bbMax-bbMin).min()*0.05f;
    
    // info
    std::clog << mesh_.n_vertices() << " vertices, "
	      << mesh_.n_edges()    << " edge, "
	      << mesh_.n_faces()    << " faces\n";
    
    // base point for displaying face normals
    {
      OpenMesh::Utils::Timer t;
      t.start();
      mesh_.add_property( fp_normal_base_ );
      typename M::FaceIter f_it = mesh_.faces_begin();
      typename M::FaceVertexIter fv_it;
      for (;f_it != mesh_.faces_end(); ++f_it)
      {
        typename Mesh::Point v(0,0,0);
        for( fv_it=mesh_.fv_iter(*f_it); fv_it.is_valid(); ++fv_it)
          v += OpenMesh::vector_cast<typename Mesh::Normal>(mesh_.point(*fv_it));
        v *= 1.0f/3.0f;
        mesh_.property( fp_normal_base_, *f_it ) = v;
      }
      t.stop();
      std::clog << "Computed base point for displaying face normals [" 
                << t.as_string() << "]" << std::endl;
    }

    //      
    {
      std::clog << "Computing strips.." << std::flush;
      OpenMesh::Utils::Timer t;
      t.start();
      compute_strips();
      t.stop();
      std::clog << "done [" << strips_.n_strips() 
		<< " strips created in " << t.as_string() << "]\n";
    }
    
    //    
#if defined(WIN32)
    updateGL();
#endif

    setWindowTitle(QFileInfo(_filename).fileName());

    // loading done
    return true;
  }
  return false;
}


//-----------------------------------------------------------------------------

template <typename M>
bool MeshViewerWidgetT<M>::open_texture( const char *_filename )
{
   QImage texsrc;
   QString fname = _filename;

   if (texsrc.load( fname ))
   {      
     return set_texture( texsrc );
   }
   return false;
}


//-----------------------------------------------------------------------------

template <typename M>
bool MeshViewerWidgetT<M>::set_texture( QImage& _texsrc )
{
  if ( !opt_.vertex_has_texcoord() )
    return false;
   
  {
    // adjust texture size: 2^k * 2^l
    int tex_w, w( _texsrc.width()  );
    int tex_h, h( _texsrc.height() );

    for (tex_w=1; tex_w <= w; tex_w <<= 1) {};
    for (tex_h=1; tex_h <= h; tex_h <<= 1) {};
    tex_w >>= 1;
    tex_h >>= 1;
    _texsrc = _texsrc.scaled( tex_w, tex_h, Qt::IgnoreAspectRatio, Qt::SmoothTransformation );
  }

  QImage texture( QGLWidget::convertToGLFormat ( _texsrc ) );
  
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glPixelStorei(GL_UNPACK_SKIP_ROWS,   0);
  glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
  glPixelStorei(GL_UNPACK_ALIGNMENT,   1);
  glPixelStorei(GL_PACK_ROW_LENGTH,    0);
  glPixelStorei(GL_PACK_SKIP_ROWS,     0);
  glPixelStorei(GL_PACK_SKIP_PIXELS,   0);
  glPixelStorei(GL_PACK_ALIGNMENT,     1);    
  
  if ( tex_id_ > 0 )
  {
    glDeleteTextures(1, &tex_id_);
  }
  glGenTextures(1, &tex_id_);
  glBindTexture(GL_TEXTURE_2D, tex_id_);
    
  // glTexGenfv( GL_S, GL_SPHERE_MAP, 0 );
  // glTexGenfv( GL_T, GL_SPHERE_MAP, 0 );
    
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);      
  
  glTexImage2D(GL_TEXTURE_2D,       // target
	       0,                   // level
	       GL_RGBA,             // internal format
	       texture.width(),     // width  (2^n)
	       texture.height(),    // height (2^m)
	       0,                   // border
	       GL_RGBA,             // format
	       GL_UNSIGNED_BYTE,    // type
	       texture.bits() );    // pointer to pixels

  std::cout << "Texture loaded\n";
  return true;
}


//-----------------------------------------------------------------------------

template <typename M>
void
MeshViewerWidgetT<M>::draw_openmesh(const std::string& _draw_mode)
{
  typename Mesh::ConstFaceIter    fIt(mesh_.faces_begin()), 
                                  fEnd(mesh_.faces_end());

  typename Mesh::ConstFaceVertexIter fvIt;


   //std::cout << "draw_openmesh " << _draw_mode << "\n";


  
#if defined(OM_USE_OSG) && OM_USE_OSG
  if (_draw_mode == "OpenSG Indices") // --------------------------------------
  {
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, mesh_.points());

    glEnableClientState(GL_NORMAL_ARRAY);
    glNormalPointer(GL_FLOAT, 0, mesh_.vertex_normals());

    if ( tex_id_ && mesh_.has_vertex_texcoords2D() )
    {
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      glTexCoordPointer(2, GL_FLOAT, 0, mesh_.texcoords2D());
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, tex_id_);
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, tex_mode_);
    }

    glDrawElements(GL_TRIANGLES, 
		   mesh_.osg_indices()->size(), 
		   GL_UNSIGNED_INT, 
		   &mesh_.osg_indices()->getField()[0] );

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  }
  else
#endif

  if (_draw_mode == "Wireframe") // -------------------------------------------
  {
     glBegin(GL_TRIANGLES);
     for (; fIt!=fEnd; ++fIt)
     {
        fvIt = mesh_.cfv_iter(*fIt);
        glVertex3fv( &mesh_.point(*fvIt)[0] );
        ++fvIt;
        glVertex3fv( &mesh_.point(*fvIt)[0] );
        ++fvIt;
        glVertex3fv( &mesh_.point(*fvIt)[0] );
     }
     glEnd();
  }
  
  else if (_draw_mode == "Solid Flat") // -------------------------------------
  {
    glBegin(GL_TRIANGLES);
    for (; fIt!=fEnd; ++fIt)
    {
      glNormal3fv( &mesh_.normal(*fIt)[0] );
      
      fvIt = mesh_.cfv_iter(*fIt);
      glVertex3fv( &mesh_.point(*fvIt)[0] );
      ++fvIt;
      glVertex3fv( &mesh_.point(*fvIt)[0] );
      ++fvIt;
      glVertex3fv( &mesh_.point(*fvIt)[0] );
    }
    glEnd();
    
  }


  else if (_draw_mode == "Solid Smooth") // -----------------------------------
  {
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, mesh_.points());

    glEnableClientState(GL_NORMAL_ARRAY);
    glNormalPointer(GL_FLOAT, 0, mesh_.vertex_normals());

    if ( tex_id_ && mesh_.has_vertex_texcoords2D() )
    {
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      glTexCoordPointer(2, GL_FLOAT, 0, mesh_.texcoords2D());
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, tex_id_);
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, tex_mode_);
    }
  else if (_draw_mode == "Wireframe")
  {
    glDisable(GL_LIGHTING);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    draw_openmesh(_draw_mode);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }
    glBegin(GL_TRIANGLES);
    for (; fIt!=fEnd; ++fIt)
    {
      fvIt = mesh_.cfv_iter(*fIt);
      glArrayElement(fvIt->idx());
      ++fvIt;
      glArrayElement(fvIt->idx());
      ++fvIt;
      glArrayElement(fvIt->idx());
    }
    glEnd();
    
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    if ( tex_id_ && mesh_.has_vertex_texcoords2D() )
    {
      glDisable(GL_TEXTURE_2D);
    }
  }  

  else if (_draw_mode == "Colored Vertices") // --------------------------------
  {
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, mesh_.points());

    glEnableClientState(GL_NORMAL_ARRAY);
    glNormalPointer(GL_FLOAT, 0, mesh_.vertex_normals());

    if ( mesh_.has_vertex_colors() )
    {
      glEnableClientState( GL_COLOR_ARRAY );
      glColorPointer(3, GL_UNSIGNED_BYTE, 0,mesh_.vertex_colors());
    }

    glBegin(GL_TRIANGLES);
    for (; fIt!=fEnd; ++fIt)
    {
      fvIt = mesh_.cfv_iter(*fIt);
      glArrayElement(fvIt->idx());
      ++fvIt;
      glArrayElement(fvIt->idx());
      ++fvIt;
      glArrayElement(fvIt->idx());
    }
    glEnd();
    
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
  }  


  else if (_draw_mode == "Solid Colored Faces") // -----------------------------
  {
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, mesh_.points());

    glEnableClientState(GL_NORMAL_ARRAY);
    glNormalPointer(GL_FLOAT, 0, mesh_.vertex_normals());

    glBegin(GL_TRIANGLES);
    for (; fIt!=fEnd; ++fIt)
    {
      glColor( *fIt );

      fvIt = mesh_.cfv_iter(*fIt);
      glArrayElement(fvIt->idx());
      ++fvIt;
      glArrayElement(fvIt->idx());
      ++fvIt;
      glArrayElement(fvIt->idx());
    }
    glEnd();
    
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
  }  


  else if (_draw_mode == "Smooth Colored Faces") // ---------------------------
  {
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, mesh_.points());

    glEnableClientState(GL_NORMAL_ARRAY);
    glNormalPointer(GL_FLOAT, 0, mesh_.vertex_normals());

    glBegin(GL_TRIANGLES);
    for (; fIt!=fEnd; ++fIt)
    {
      glMaterial( *fIt );

      fvIt = mesh_.cfv_iter(*fIt);
      glArrayElement(fvIt->idx());
      ++fvIt;
      glArrayElement(fvIt->idx());
      ++fvIt;
      glArrayElement(fvIt->idx());
    }
    glEnd();
    
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
  }  


  else if ( _draw_mode == "Strips'n VertexArrays" ) // ------------------------
  {
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, mesh_.points());

    glEnableClientState(GL_NORMAL_ARRAY);
    glNormalPointer(GL_FLOAT, 0, mesh_.vertex_normals());

    if ( tex_id_ && mesh_.has_vertex_texcoords2D() )
    {
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      glTexCoordPointer(2, GL_FLOAT, 0, mesh_.texcoords2D());
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, tex_id_);
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, tex_mode_);
    }

    typename MyStripifier::StripsIterator strip_it = strips_.begin();
    typename MyStripifier::StripsIterator strip_last = strips_.end();

    // Draw all strips
    for (; strip_it!=strip_last; ++strip_it)
    {
      glDrawElements(GL_TRIANGLE_STRIP, 
          static_cast<GLsizei>(strip_it->size()), GL_UNSIGNED_INT, &(*strip_it)[0] );
    }

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  }


  else if (_draw_mode == "Show Strips" && strips_.is_valid() ) // -------------
  {
    typename MyStripifier::StripsIterator strip_it = strips_.begin();
    typename MyStripifier::StripsIterator strip_last = strips_.end();

    float cmax  = 256.0f;
    int   range = 220;
    int   base  = (int)cmax-range;
    int   drcol  = 13;
    int   dgcol  = 31;
    int   dbcol  = 17;
    
    int rcol=0, gcol=dgcol, bcol=dbcol+dbcol;
         
    // Draw all strips
    for (; strip_it!=strip_last; ++strip_it)
    {
      typename MyStripifier::IndexIterator idx_it   = strip_it->begin();
      typename MyStripifier::IndexIterator idx_last = strip_it->end();
      
      rcol = (rcol+drcol) % range;
      gcol = (gcol+dgcol) % range;
      bcol = (bcol+dbcol) % range;
      
      glBegin(GL_TRIANGLE_STRIP);
      glColor3f((rcol+base)/cmax, (gcol+base)/cmax, (bcol+base)/cmax);
      for ( ;idx_it != idx_last; ++idx_it )
        glVertex3fv(&mesh_.point( OM_TYPENAME Mesh::VertexHandle(*idx_it))[0]);
      glEnd();
    }
    glColor3f(1.0, 1.0, 1.0);
  }


  else if( _draw_mode == "Points" ) // -----------------------------------------
  {
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, mesh_.points());

    if (mesh_.has_vertex_colors() && use_color_)
    {
      glEnableClientState(GL_COLOR_ARRAY);
      glColorPointer(3, GL_UNSIGNED_BYTE, 0, mesh_.vertex_colors());
    }

    glDrawArrays( GL_POINTS, 0, static_cast<GLsizei>(mesh_.n_vertices()) );
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
  }

  //glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
  //glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  // MY CODE
  else if( _draw_mode == "Gcode" ) // -----------------------------------------
  {

    if(!toolpath.size())
      std::cout << "You need to slice mesh\n";
    else
    {

      if(displayPoints)
      {
        glEnableClientState(GL_VERTEX_ARRAY);
        glColor3f(1.0, 0.0, 0.0);
        for(int j=0; j < layerHeight; j++)
        {
          for(size_t i=0; i < toolpath[j].size(); i++)
          {  
            glVertexPointer(3, GL_FLOAT, 0, &toolpath[j][i][0]);
            glPointSize(2.0f);
            glDrawArrays( GL_POINTS, 0, static_cast<GLsizei>(toolpath[j][i].size()) );
          }
        }
        glDisableClientState(GL_VERTEX_ARRAY);
      }
      if(displayPath)
      {
        glLineWidth(1.0); 
        glColor3f(1.0, 1.0, 1.0);
        glBegin(GL_LINES);

        //Render the top most layer acouding to the line number
        //if(layerHeight > 0)
        //{
          //int k = 0;
          //int nextLayerThreshold = toolpath[layerHeight-1][k].size();
          //for(int j=1; j < lineNumber; j++)
          //{
            //if(j == nextLayerThreshold)
            //{
              //k++;
              //nextLayerThreshold = toolpath[layerHeight-1][k].size();
            //}
            //glVertex3f(toolpath[layerHeight - 1][k][j-1][0], toolpath[layerHeight - 1][k][j-1][1], toolpath[layerHeight - 1][k][j-1][2]);
            //glVertex3f(toolpath[layerHeight - 1][k][j][0], toolpath[layerHeight - 1][k][j][1], toolpath[layerHeight - 1][k][j][2]);
          //}
        //}

        //std::cout << "G-code render\n";
        // if you have command slider for(int i=0; i < (layerHeight - 1); i++)
        for(int i=0; i < layerHeight; i++)
        {
          for(size_t k=0; k < toolpath[i].size(); k++)
          {
            //std::cout << "Inside K Loop \n";
            for(size_t j=1; j < toolpath[i][k].size(); j++)
            {
              //std::cout << "Inside J Loop \n";
              glVertex3f(toolpath[i][k][j-1][0], toolpath[i][k][j-1][1], toolpath[i][k][j-1][2]);
              glVertex3f(toolpath[i][k][j][0], toolpath[i][k][j][1], toolpath[i][k][j][2]);
            }
          }
        }
      }
      if(displayCurvature)
      {
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        for(int j=0; j < layerHeight; j++)
        {
          for(size_t i=0; i < toolpath[j].size(); i++)
          {  
            glColorPointer(3, GL_FLOAT, 0, &curvature[j][i][0]);
            glVertexPointer(3, GL_FLOAT, 0, &toolpath[j][i][0]);
            glPointSize(2.0f);
            glDrawArrays( GL_POINTS, 0, static_cast<GLsizei>(toolpath[j][i].size()) );
          }
        }
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);
        
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //glBegin(GL_POINTS);
        //glPointSize(2.0f);
        //for(int j=0; j < layerHeight; j++)
        //{
          //for(size_t i=0; i < toolpath[j].size(); i++)
          //{  
            //for(size_t k = 0; k < toolpath[j][i].size(); k++)
            //{
              ////if(curvature[j][i][k] > 0)
              ////{
                //std::cout << curvature[j][i][k] << std::endl;
                //glColor3f((float)curvature[j][i][k], 1.0f - (float)curvature[j][i][k], 0.0f);
                //glVertex3f(toolpath[j][i][k][0], toolpath[j][i][k][1], toolpath[j][i][k][2]);
              ////}
            //}
          //}
        //}
        glEnd();
      }
      glEnd();
    }
  }
}


//-----------------------------------------------------------------------------


template <typename M>
void 
MeshViewerWidgetT<M>::draw_scene(const std::string& _draw_mode)
{
  
  if ( ! mesh_.n_vertices() )
    return;
   

  //std::cout << _draw_mode << "\n";

#if defined(OM_USE_OSG) && OM_USE_OSG
  else if ( _draw_mode == "OpenSG Indices")
  {     
    glEnable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);
    draw_openmesh( _draw_mode );
  }     
  else
#endif
  if ( _draw_mode == "Points" )
  {
    glDisable(GL_LIGHTING);
    draw_openmesh(_draw_mode);
  }
  else if (_draw_mode == "Wireframe")
  {
    glDisable(GL_LIGHTING);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    draw_openmesh(_draw_mode);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }
  
  else if ( _draw_mode == "Hidden-Line" )
  {
    glDisable(GL_LIGHTING);
    glShadeModel(GL_FLAT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glColor4f( 0.0f, 0.0f, 0.0f, 1.0f );
    glDepthRange(0.01, 1.0);
    draw_openmesh( "Wireframe" );
    
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE);
    glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
    glDepthRange( 0.0, 1.0 );
    draw_openmesh( "Wireframe" );
    
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL);          
  }
  
  else if (_draw_mode == "Solid Flat")
  {
    glEnable(GL_LIGHTING);
    glShadeModel(GL_FLAT);
    draw_openmesh(_draw_mode);
  }
  
  else if (_draw_mode == "Solid Smooth"        ||
	   _draw_mode == "Strips'n VertexArrays" )
  {
    glEnable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);
    draw_openmesh(_draw_mode);
  }
  
  else if (_draw_mode == "Show Strips")
  {
    glDisable(GL_LIGHTING);
    draw_openmesh(_draw_mode);
  }
  
  else if (_draw_mode == "Colored Vertices" )
  {
    glDisable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);
    draw_openmesh(_draw_mode);
  }

  else if (_draw_mode == "Solid Colored Faces")
  {
    glDisable(GL_LIGHTING);
    glShadeModel(GL_FLAT);
    draw_openmesh(_draw_mode);
    setDefaultMaterial();
  }
  
  else if (_draw_mode == "Smooth Colored Faces" )
  {
    glEnable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);
    draw_openmesh(_draw_mode);
    setDefaultMaterial();
  }
  else if (_draw_mode == "Gcode")
  {
    glDisable(GL_LIGHTING);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    draw_openmesh(_draw_mode);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }


  if (show_vnormals_)
  {
    typename Mesh::VertexIter vit;
    glDisable(GL_LIGHTING);
    glBegin(GL_LINES);
    glColor3f(1.000f, 0.803f, 0.027f); // orange
    for(vit=mesh_.vertices_begin(); vit!=mesh_.vertices_end(); ++vit)
    {
      glVertex( *vit );
      glVertex( mesh_.point( *vit ) + normal_scale_*mesh_.normal( *vit ) );
    }
    glEnd();
  }

  if (show_fnormals_)
  {
    typename Mesh::FaceIter fit;
    glDisable(GL_LIGHTING);
    glBegin(GL_LINES);
    glColor3f(0.705f, 0.976f, 0.270f); // greenish
    for(fit=mesh_.faces_begin(); fit!=mesh_.faces_end(); ++fit)
    {
      glVertex( mesh_.property(fp_normal_base_, *fit) );
      glVertex( mesh_.property(fp_normal_base_, *fit) +
                normal_scale_*mesh_.normal( *fit ) );
    }
    glEnd();
  }
}


//-----------------------------------------------------------------------------

template <typename M>
void 
MeshViewerWidgetT<M>::enable_strips() 
{ 
  if (!f_strips_)
  {
    f_strips_ = true;  
    add_draw_mode("Strips'n VertexArrays");
    add_draw_mode("Show Strips");
  }
}

//-----------------------------------------------------------------------------

template <typename M>
void 
MeshViewerWidgetT<M>::disable_strips() 
{ 
  if (f_strips_)
  {
    f_strips_ = false; 
    del_draw_mode("Show Strips");
    del_draw_mode("Strip'n VertexArrays");
  }
}


//-----------------------------------------------------------------------------

#define TEXMODE( Mode ) \
   tex_mode_ = Mode; std::cout << "Texture mode set to " << #Mode << std::endl

template <typename M>
void 
MeshViewerWidgetT<M>::keyPressEvent( QKeyEvent* _event)
{
  switch( _event->key() )
  {
    case Key_D:
      if ( mesh_.has_vertex_colors() && (current_draw_mode()=="Points") )
      {
        use_color_ = !use_color_;
        std::cout << "use color: " << (use_color_?"yes\n":"no\n");
        if (!use_color_)
          glColor3f(1.0f, 1.0f, 1.0f);
        updateGL();
      }
      break;

    case Key_N:
      if ( _event->modifiers() & ShiftModifier )
      {
        show_fnormals_ = !show_fnormals_;
        std::cout << "show face normals: " << (show_fnormals_?"yes\n":"no\n");
      }
      else
      {
        show_vnormals_ = !show_vnormals_;
        std::cout << "show vertex normals: " << (show_vnormals_?"yes\n":"no\n");
      }
      updateGL();
      break;

    case Key_I:
      std::cout << "\n# Vertices     : " << mesh_.n_vertices() << std::endl;
      std::cout << "# Edges        : " << mesh_.n_edges()    << std::endl;
      std::cout << "# Faces        : " << mesh_.n_faces()    << std::endl;
      std::cout << "binary  input  : " << opt_.check(opt_.Binary) << std::endl;
      std::cout << "swapped input  : " << opt_.check(opt_.Swap) << std::endl;
      std::cout << "vertex normal  : " 
                << opt_.check(opt_.VertexNormal) << std::endl;
      std::cout << "vertex texcoord: " 
                << opt_.check(opt_.VertexTexCoord) << std::endl;
      std::cout << "vertex color   : " 
                << opt_.check(opt_.VertexColor) << std::endl;
      std::cout << "face normal    : " 
                << opt_.check(opt_.FaceNormal) << std::endl;
      std::cout << "face color     : " 
                << opt_.check(opt_.FaceColor) << std::endl;
      this->QGLViewerWidget::keyPressEvent( _event );
      break;

    case Key_T:
      switch( tex_mode_ )
      {
        case GL_MODULATE: TEXMODE(GL_DECAL); break;
        case GL_DECAL:    TEXMODE(GL_BLEND); break;
        case GL_BLEND:    TEXMODE(GL_REPLACE); break;
        case GL_REPLACE:  TEXMODE(GL_MODULATE); break;
      }
      updateGL();
      break;

    default:
      this->QGLViewerWidget::keyPressEvent( _event );
  }
}

#undef TEXMODE

//=============================================================================

