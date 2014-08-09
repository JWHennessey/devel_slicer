#ifndef MAINWINDOW_HH
#define MAINWINDOW_HH

#include <QtGui>
#include <QMainWindow>
#include <QSlider>
#include <QButtonGroup>
#include <QPushButton>
#include "MeshViewerWidget.hh"
#include "ControlsWidget.hh"

class MainWindow : public QMainWindow
{

public:
   MainWindow();
   void setCentralWidget(MeshViewerWidget* widget);

private:
  MeshViewerWidget* meshViewer;
  void createDock();
};

#if !defined(MAINWINDOW_CC)
#  define OPENMESH_MESHVIEWERWIDGET_TEMPLATES
#  include "MainWindow.cc"
#endif


#endif
