#if !defined( PRINTHEADT_HH )
#define PRINTHEADT_HH

#include <vector>
#include <sstream>
#include <string>
#include <math.h>

template <typename M>
class PrintheadT
{
public:
  PrintheadT();
  void extrudeXYAxisTo(float x, float y);
  void extrudeXYZAxisTo(float x, float y, float z);
  void moveAlongXYAxis(float x, float y);
  void extrudeAlongXYAxis(float x, float y);
  void moveZAxis();
  const std::vector<std::string> getCommands();

private:
  float _x;
  float _y;
  float _z;
  float _e;
  //float E_COEF;
  const float LAYER_HEIGHT;
  const float LINE_WIDTH;
  std::vector<std::string> _commands;

};


#if !defined(PRINTHEAD_CC)
#  define OPENMESH_MESHVIEWERWIDGET_TEMPLATES
#  include "PrintheadT.cc"
#endif


#endif
