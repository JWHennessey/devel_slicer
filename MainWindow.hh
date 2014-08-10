#ifndef MAINWINDOW_HH
#define MAINWINDOW_HH

#include <QtGui>
#include <QMainWindow>
#include <QSlider>
#include <QButtonGroup>
#include <QPushButton>
#include <QGLWidget>

#include "MeshViewerWidget.hh"
#include "ControlsWidget.hh"

class MainWindow : public QMainWindow
{

public:
   MainWindow();
   void setOptions(const OpenMesh::IO::Options& opts) { meshViewer.setOptions(opts); }
   void open_mesh_gui(QString fname) { meshViewer.open_mesh_gui(fname); }
   void open_texture_gui(QString fname) { meshViewer.open_texture_gui(fname); }
private:
  MeshViewerWidget meshViewer;
  QDockWidget *dock;
  void createDock();
};




#endif
