#ifndef CONTROLSWIDGET_CC
#define CONTROLSWIDGET_CC


#include "ControlsWidget.hh"


  //QButtonGroup* btnGroup = new QButtonGroup(this);
  //QPushButton* sliceBtn = new QPushButton(this);
  //sliceBtn->setText("Slice");
  //connect(sliceBtn, SIGNAL(clicked()), this, SLOT(sliceButtonSlot()));
  //btnGroup->addButton(sliceBtn);
  //dock->setWidget(sliceBtn);
  //addDockWidget(Qt::RightDockWidgetArea, dock);

  //dock = new QDockWidget(tr("Height"), this);

  //QSlider* layerSliderHeight = new QSlider(Qt::Horizontal);
  //dock->setWidget(layerSliderHeight);
  //addDockWidget(Qt::RightDockWidgetArea, dock);

ControlsWidget::ControlsWidget(MeshViewerWidget* mv)
{
  meshViewer = mv;
  setMinimumWidth(200);
  addControls();
  }

void ControlsWidget::addControls()
{
  QVBoxLayout *vlay = new QVBoxLayout(this);
  sliceBtn = new QPushButton("Slice Mesh");
  vlay->addWidget(sliceBtn);

  layerSlider = new QSlider();
  layerSlider->setMaximumHeight(200);
  layerSlider->setHidden(true);
  vlay->addWidget(layerSlider);

  lineSlider = new QSlider(Qt::Horizontal);
  lineSlider->setMaximumHeight(200);
  lineSlider->setHidden(true);
  vlay->addWidget(lineSlider);

  addActions();
}

void ControlsWidget::addActions()
{
  connect(sliceBtn, SIGNAL(clicked()),this, SLOT(slice()));
  connect(layerSlider, SIGNAL(valueChanged(int)),
             this, SLOT(setLayerHeight(int)));
  connect(lineSlider, SIGNAL(valueChanged(int)),
             this, SLOT(setLineNumber(int)));
}

void ControlsWidget::slice()
{
  meshViewer->slice_mesh();

  int count = meshViewer->getLayerCount();
  layerSlider->setRange(0, count);
  layerSlider->setSliderPosition(count);
  layerSlider->setHidden(false);

  int lineNo = meshViewer->getLineNumber();
  lineSlider->setRange(1, lineNo);
  lineSlider->setSliderPosition(lineNo);
  lineSlider->setHidden(false);
}

void ControlsWidget::setLayerHeight(int value)
{
  meshViewer->setLayerHeight(value);
}

void ControlsWidget::setLineNumber(int value)
{
  meshViewer->setLineNumber(value);
}

#endif
