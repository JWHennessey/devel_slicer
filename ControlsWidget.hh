#ifndef CONTROLSWIDGET_HH
#define CONTROLSWIDGET_HH

#include <QVBoxLayout>
#include <QPushButton>
#include <QGLWidget>
#include <QButtonGroup>
#include <QLabel>
#include <QSlider>
#include <QDoubleSpinBox>
#include <QRadioButton>
#include <QGroupBox>
#include <QCheckBox>
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
  void completeLoopToggle();
  void toggleRadio1();
  void toggleRadio2();
  void toggleRadio3();
  void toggleRadio4();
  void resampleToggle();

private:
  MeshViewerWidget* meshViewer;
  QDoubleSpinBox* layerHeightSpinBox;
  QDoubleSpinBox* offsetSpinBox;
  QPushButton* sliceBtn;
  QPushButton* sliceBtnAlt;
  QPushButton* rotateXBtn;
  QPushButton* rotateYBtn;
  QPushButton* rotateZBtn;
  QCheckBox* completeLoopBtn;
  QCheckBox* resampleBtn;
  QSlider* layerSlider;
  QSlider* lineSlider;
  void addActions();
  void addControls();
  QRadioButton *radio1;
  QRadioButton *radio2;
  QRadioButton *radio3;
  QRadioButton *radio4;
  QGroupBox *groupBox;


};

#endif
