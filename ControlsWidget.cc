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
  
  rotateXBtn = new QPushButton("Rotate X (90)");
  vlay->addWidget(rotateXBtn);
  
  rotateYBtn = new QPushButton("Rotate Y (90)");
  vlay->addWidget(rotateYBtn);
  
  rotateZBtn = new QPushButton("Rotate Z (90)");
  vlay->addWidget(rotateZBtn);


  layerHeightSpinBox = new QDoubleSpinBox();
  layerHeightSpinBox->setPrefix("Layer Height:  ");
  layerHeightSpinBox->setRange(0.01, 1.0);
  layerHeightSpinBox->setSingleStep(0.01);
  layerHeightSpinBox->setValue(0.1);
  vlay->addWidget(layerHeightSpinBox);

  sliceBtn = new QPushButton("Slice Mesh");
  vlay->addWidget(sliceBtn);

  sliceBtnAlt = new QPushButton("Slice Mesh Alt");
  vlay->addWidget(sliceBtnAlt);

  layerSlider = new QSlider();
  layerSlider->setMaximumHeight(200);
  layerSlider->setHidden(true);
  vlay->addWidget(layerSlider);

  //lineSlider = new QSlider(Qt::Horizontal);
  //lineSlider->setMaximumHeight(200);
  //lineSlider->setHidden(true);
  //vlay->addWidget(lineSlider);

  addActions();
}

void ControlsWidget::addActions()
{
  connect(sliceBtn, SIGNAL(clicked()),this, SLOT(slice()));
  connect(sliceBtnAlt, SIGNAL(clicked()),this, SLOT(sliceAlt()));
  connect(rotateXBtn, SIGNAL(clicked()),this, SLOT(rotateX()));
  connect(rotateYBtn, SIGNAL(clicked()),this, SLOT(rotateY()));
  connect(rotateZBtn, SIGNAL(clicked()),this, SLOT(rotateZ()));
  connect(layerSlider, SIGNAL(valueChanged(int)),
             this, SLOT(setLayerHeight(int)));
  //connect(lineSlider, SIGNAL(valueChanged(int)),
             //this, SLOT(setLineNumber(int)));
}

void ControlsWidget::slice()
{
  double layerHeight = layerHeightSpinBox->value();
  meshViewer->slice_mesh(layerHeight);

  int count = meshViewer->getLayerCount();
  layerSlider->setRange(0, count);
  layerSlider->setSliderPosition(count);
  layerSlider->setHidden(false);
}


void ControlsWidget::sliceAlt()
{
  double layerHeight = layerHeightSpinBox->value();
  meshViewer->slice_mesh_alt(layerHeight);

  int count = meshViewer->getLayerCount();
  layerSlider->setRange(0, count);
  layerSlider->setSliderPosition(count);
  layerSlider->setHidden(false);
}

void ControlsWidget::setLayerHeight(int value)
{
  meshViewer->setLayerHeight(value);
}

void ControlsWidget::setLineNumber(int value)
{
  meshViewer->setLineNumber(value);
}

void ControlsWidget::rotateX()
{
  meshViewer->rotateX(90);
}

void ControlsWidget::rotateY()
{
  meshViewer->rotateX(90);
}

void ControlsWidget::rotateZ()
{
  meshViewer->rotateZ(90);
}

#endif
