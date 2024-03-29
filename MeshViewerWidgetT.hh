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


#ifndef OPENMESHAPPS_MESHVIEWERWIDGETT_HH
#define OPENMESHAPPS_MESHVIEWERWIDGETT_HH

#include <math.h>
//== INCLUDES =================================================================
#include <QGraphicsScene>
#include <QVBoxLayout> 
#include <QAction>
#include <string>
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/IO/Options.hh>
#include <OpenMesh/Core/Utils/GenProg.hh>
#include <OpenMesh/Core/Utils/color_cast.hh>
#include <OpenMesh/Core/Mesh/Attributes.hh>
#include <OpenMesh/Tools/Utils/StripifierT.hh>
#include <OpenMesh/Tools/Utils/Timer.hh>
#include <OpenMesh/Apps/Slicer/QGLViewerWidget.hh>
#include <OpenMesh/Apps/Slicer/SlicerT.hh>
#include "Eigen/Dense"
#include "Eigen/Geometry"

//== FORWARDS =================================================================

class QImage;


//== CLASS DEFINITION =========================================================

	      
template <typename M>
class MeshViewerWidgetT : public QGLViewerWidget
{
public:

  typedef M                             Mesh;
  typedef OpenMesh::StripifierT<Mesh>   MyStripifier;
  typedef typename Mesh::Point Point;
  typedef typename Mesh::VertexIter VertexIter;

public:

  /// default constructor
  MeshViewerWidgetT(QWidget* _parent = 0);
  
  /// destructor
  ~MeshViewerWidgetT() {}
  
public:


  virtual void slice_mesh(double layerHeight, double offset, int wallthickness);
  virtual void slice_mesh_alt(double layerHeight, double offset, int wallthickness);
  int getLayerCount();
  int getLineNumber();
  void rotateX(int deg);
  void rotateY(int deg);
  void rotateZ(int deg);
  void completeLoopToggle(bool value);
  void toggleDisplay1();
  void toggleDisplay2();
  void toggleDisplay3();
  void toggleDisplay4();
  void toggleDisplay5();
  void toggleResample();
  void toggleOutline();

  /// open mesh
  virtual bool open_mesh(const char* _filename, OpenMesh::IO::Options _opt);
  
  /// load texture
  virtual bool open_texture( const char *_filename );



  bool set_texture( QImage& _texsrc );
 
  void enable_strips();
  void disable_strips();  
  

  Mesh& mesh() { return mesh_; }
  const Mesh& mesh() const { return mesh_; }
  
protected:
  void turnAllDisplayOff();
  bool completeLoop;
  ////s inherited drawing method
  virtual void draw_scene(const std::string& _draw_mode);
  
protected:
  void rotate(int deg, Eigen::Vector3f axis);

  /// draw the mesh
  virtual void draw_openmesh(const std::string& _drawmode);


  void glVertex( const typename Mesh::VertexHandle _vh )
  { glVertex3fv( &mesh_.point( _vh )[0] ); }

  void glVertex( const typename Mesh::Point& _p )
  { glVertex3fv( &_p[0] ); }
  
  void glNormal( const typename Mesh::VertexHandle _vh )
  { glNormal3fv( &mesh_.normal( _vh )[0] ); }

  void glTexCoord( const typename Mesh::VertexHandle _vh )
  { glTexCoord2fv( &mesh_.texcoord(_vh)[0] ); }
  
  void glColor( const typename Mesh::VertexHandle _vh )
  { glColor3ubv( &mesh_.color(_vh)[0] ); }
  
  // face properties

  void glNormal( const typename Mesh::FaceHandle _fh )
  { glNormal3fv( &mesh_.normal( _fh )[0] ); }

  void glColor( const typename Mesh::FaceHandle _fh )
  { glColor3ubv( &mesh_.color(_fh)[0] ); }

  void glMaterial( const typename Mesh::FaceHandle _fh, 
		   int _f=GL_FRONT_AND_BACK, int _m=GL_DIFFUSE )
  { 
    OpenMesh::Vec3f c=OpenMesh::color_cast<OpenMesh::Vec3f>(mesh_.color(_fh));
    OpenMesh::Vec4f m( c[0], c[1], c[2], 1.0f );

    glMaterialfv(_f, _m, &m[0]); 
  }


protected: // Strip support
  
  void compute_strips(void)
  {
    if (f_strips_)
    {
      strips_.clear();
      strips_.stripify();
    }
  }

protected: // inherited
   
  virtual void keyPressEvent( QKeyEvent* _event);

protected:
  bool                   displayPath;
  bool                   displayPoints;
  bool                   displayCurvature;
  bool                   displayGcode;
  bool                   outlinePath;
  bool                   resample;
  bool                   f_strips_; // enable/disable strip usage
  GLuint                 tex_id_;
  GLint                  tex_mode_;
  OpenMesh::IO::Options  opt_; // mesh file contained texcoords?
  
  Mesh                   mesh_;
  MyStripifier           strips_;
  bool                   use_color_;
  bool                   show_vnormals_;
  bool                   show_fnormals_;
  float                  normal_scale_;
  OpenMesh::FPropHandleT< typename Mesh::Point > fp_normal_base_;

  // My Code
  std::vector<std::vector<std::vector<typename Mesh::Point> > > toolpath;
  std::vector<std::vector<std::vector<typename Mesh::Point> > > curvature;
  QDialog *createDialog(const QString &windowTitle) const;
  int layerHeight;
  int lineNumber;

public:
  void setLayerHeight(int value);
  void setLineNumber(int value);
};


//=============================================================================
#if defined(OM_INCLUDE_TEMPLATES) && !defined(OPENMESHAPPS_MESHVIEWERWIDGET_CC)
#  define OPENMESH_MESHVIEWERWIDGET_TEMPLATES
#  include "MeshViewerWidgetT.cc"
#endif
//=============================================================================
#endif // OPENMESHAPPS_MESHVIEWERWIDGETT_HH defined
//=============================================================================

