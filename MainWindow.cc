#ifndef MAINWINDOW_CC
#define MAINWINDOW_CC

#include "MainWindow.hh"


MainWindow::MainWindow()
{
  //createDock();
}


void MainWindow::setCentralWidget(MeshViewerWidget* widget)
{
  QMainWindow::setCentralWidget(widget);
  meshViewer = widget;
}

void MainWindow::createDock()
{
  QDockWidget *dock = new QDockWidget(tr("Controls"), this);
  dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

  ControlsWidget* controls = new ControlsWidget(meshViewer);
  dock->setWidget(controls);
  addDockWidget(Qt::RightDockWidgetArea, dock);


  //QButtonGroup* btnGroup = new QButtonGroup(this);
  //QPushButton* sliceBtn = new QPushButton(this);
  //sliceBtn->setText("Slice");
  //connect(sliceBtn, SIGNAL(clicked()), this, SLOT(sliceButtonSlot()));
  //btnGroup->addButton(sliceBtn);
  //dock->setWidget(sliceBtn);
  //addDockWidget(Qt::RightDockWidgetArea, dock);

  //dock = new QDockWidget(tr("Height"), this);

  //QSlider* layerHeight = new QSlider(Qt::Horizontal);
  //dock->setWidget(layerHeight);
  //addDockWidget(Qt::RightDockWidgetArea, dock);
}



#endif

