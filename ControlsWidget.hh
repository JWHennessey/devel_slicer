#include "MeshViewerWidget.hh"

class ControlsWidget : public QWidget
{
  Q_OBJECT
public:
  ControlsWidget(MeshViewerWidget* mv)
  {
    meshViewer = mv;
  }

public slots:
  void slice();
 
private:
  MeshViewerWidget* meshViewer;
};
