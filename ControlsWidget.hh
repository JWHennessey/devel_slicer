#ifndef CONTROLSWIDGET_HH
#define CONTROLSWIDGET_HH

#include <QVBoxLayout>
#include <QPushButton>
#include <QGLWidget>
#include <QButtonGroup>
#include <QLabel>
#include <QSlider>

#include "MeshViewerWidget.hh"

//class MeshViewerWidget;

class ControlsWidget : public QWidget
{
  Q_OBJECT
public:
  ControlsWidget(MeshViewerWidget* mv);
  
public slots:
  void slice();
  void setLayerHeight(int value);
  void setLineNumber(int value);
 
private:
  MeshViewerWidget* meshViewer;
  QPushButton* sliceBtn;
  QSlider* layerSlider;
  QSlider* lineSlider;
  void addActions();
  void addControls();


};

#endif
