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

  offsetSpinBox = new QDoubleSpinBox();
  offsetSpinBox->setPrefix("Offset:  ");
  offsetSpinBox->setRange(0.0, 1.0);
  offsetSpinBox->setSingleStep(0.01);
  offsetSpinBox->setValue(0.0);
  vlay->addWidget(offsetSpinBox);


  completeLoopBtn = new QCheckBox("Complete Loop", this);
  vlay->addWidget(completeLoopBtn);

  resampleBtn = new QCheckBox("Resample", this);
  vlay->addWidget(resampleBtn);
  
  sliceBtn = new QPushButton("Slice Mesh");
  vlay->addWidget(sliceBtn);

  sliceBtnAlt = new QPushButton("Slice Mesh Alt");
  vlay->addWidget(sliceBtnAlt);

  layerSlider = new QSlider();
  layerSlider->setMaximumHeight(200);
  layerSlider->setHidden(true);
  vlay->addWidget(layerSlider);



  groupBox = new QGroupBox(tr("Render Style"));

  radio1 = new QRadioButton(tr("Path and Points"));
  radio2 = new QRadioButton(tr("Points Only"));
  radio3 = new QRadioButton(tr("Path Only"));
  radio4 = new QRadioButton(tr("Curvature"));
  radio1->setChecked(true);
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->addWidget(radio1);
  vbox->addWidget(radio2);
  vbox->addWidget(radio3);
  vbox->addWidget(radio4);
  vbox->addStretch(1);
  groupBox->setLayout(vbox);
  vlay->addWidget(groupBox);
  groupBox->setHidden(true);

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
  connect(completeLoopBtn, SIGNAL(clicked()),this, SLOT(completeLoopToggle()));
  connect(resampleBtn, SIGNAL(clicked()),this, SLOT(resampleToggle()));
  connect(rotateXBtn, SIGNAL(clicked()),this, SLOT(rotateX()));
  connect(rotateYBtn, SIGNAL(clicked()),this, SLOT(rotateY()));
  connect(rotateZBtn, SIGNAL(clicked()),this, SLOT(rotateZ()));
  connect(layerSlider, SIGNAL(valueChanged(int)),
             this, SLOT(setLayerHeight(int)));
  connect(radio1, SIGNAL(clicked()),this, SLOT(toggleRadio1()));
  connect(radio2, SIGNAL(clicked()),this, SLOT(toggleRadio2()));
  connect(radio3, SIGNAL(clicked()),this, SLOT(toggleRadio3()));
  connect(radio4, SIGNAL(clicked()),this, SLOT(toggleRadio4()));
}


void ControlsWidget::resampleToggle()
{
  meshViewer->toggleResample();
}

void ControlsWidget::toggleRadio1()
{
  meshViewer->toggleDisplay1();
}

void ControlsWidget::toggleRadio2()
{
  meshViewer->toggleDisplay2();

}
void ControlsWidget::toggleRadio3()
{
  meshViewer->toggleDisplay3();

}
void ControlsWidget::toggleRadio4()
{
  meshViewer->toggleDisplay4();
}
void ControlsWidget::completeLoopToggle()
{
  meshViewer->completeLoopToggle(completeLoopBtn->isChecked());
}

void ControlsWidget::slice()
{
  double layerHeight = layerHeightSpinBox->value();
  double offset = offsetSpinBox->value();
  meshViewer->slice_mesh(layerHeight, offset);

  int count = meshViewer->getLayerCount();
  layerSlider->setRange(0, count);
  layerSlider->setSliderPosition(count);
  layerSlider->setHidden(false);
  groupBox->setHidden(false);
}


void ControlsWidget::sliceAlt()
{
  double layerHeight = layerHeightSpinBox->value();
  double offset = offsetSpinBox->value();
  meshViewer->slice_mesh_alt(layerHeight, offset);

  int count = meshViewer->getLayerCount();
  layerSlider->setRange(0, count);
  layerSlider->setSliderPosition(count);
  layerSlider->setHidden(false);
  groupBox->setHidden(false);
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
