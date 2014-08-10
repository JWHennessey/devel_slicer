#ifndef MAINWINDOW_CC
#define MAINWINDOW_CC

#include "MainWindow.hh"


MainWindow::MainWindow()
{
  createDock();
  setCentralWidget(&meshViewer);
  meshViewer.enable_strips();

}

void MainWindow::createDock()
{
  dock = new QDockWidget(tr("Controls"), this);
  dock->setAllowedAreas(Qt::LeftDockWidgetArea);
  dock->setFeatures(QDockWidget::NoDockWidgetFeatures);
  
  ControlsWidget* controls = new ControlsWidget(&meshViewer);
  dock->setWidget(controls);

  addDockWidget(Qt::LeftDockWidgetArea, dock);


}



#endif

