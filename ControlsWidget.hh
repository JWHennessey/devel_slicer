#ifndef CONTROLSWIDGET_HH
#define CONTROLSWIDGET_HH

#include <QVBoxLayout>
#include <QPushButton>
#include <QGLWidget>
#include <QButtonGroup>
#include <QLabel>
#include <QSlider>
#include <QDoubleSpinBox>

#include "MeshViewerWidget.hh"

//class MeshViewerWidget;

class ControlsWidget : public QWidget
{
  Q_OBJECT
public:
  ControlsWidget(MeshViewerWidget* mv);
  
public slots:
  void slice();
  void sliceAlt();
  void setLayerHeight(int value);
  void setLineNumber(int value);
  void rotateX();
  void rotateY();
  void rotateZ();

private:
  MeshViewerWidget* meshViewer;
  QDoubleSpinBox* layerHeightSpinBox;
  QPushButton* sliceBtn;
  QPushButton* sliceBtnAlt;
  QPushButton* rotateXBtn;
  QPushButton* rotateYBtn;
  QPushButton* rotateZBtn;
  QSlider* layerSlider;
  QSlider* lineSlider;
  void addActions();
  void addControls();


};

#endif
