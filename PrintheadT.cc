#if !defined( PRINTHEADT_CC )
#define PRINTHEADT_CC

template <typename M>
PrintheadT<M>::PrintheadT() :
  _x(0.0),
  _y(0.0),
  _z(0.0),
  _e(10.0),
  LAYER_HEIGHT(0.1),
  LINE_WIDTH(0.3)
{
  std::stringstream ss;
  ss << "G92 E0 \n";
  std::string s = ss.str();
  _commands.push_back(s);
}

template <typename M> 
void 
PrintheadT<M>::extrudeXYAxisTo(float x, float y)
{
  float d = sqrt(pow((_x - x), 2) + pow((_y - y), 2));
  _x = x;
  _y = y;
  _e += (d * LAYER_HEIGHT * LINE_WIDTH);
  std::stringstream ss;
  ss << "G1 F2000 X" << _x << " Y" << _y << " E" << _e << "\n";
  std::string s = ss.str();
  _commands.push_back(s);
}


template <typename M> 
void 
PrintheadT<M>::moveXYAxisTo(float x, float y)
{
  float d = sqrt(pow((_x - x), 2) + pow((_y - y), 2));
  _x = x;
  _y = y;
  std::stringstream ss;
  ss << "G1 F2000 X" << _x << " Y" << _y << "\n";
  std::string s = ss.str();
  _commands.push_back(s);
}

template <typename M> 
void 
PrintheadT<M>::extrudeXYZAxisTo(float x, float y, float z)
{

  float d = sqrt(pow((_x - x), 2) + pow((_y - y), 2));
  _x = x;
  _y = y;
  _z = z;
  _z = z;
  // extrusionPerMM = double(layerThickness) / 1000.0; this is really just 0.1
  // extrusionAmount += extrusionPerMM * double(lineWidth) / 1000.0 * vSizeMM(diff);
  // _e += 
  _e += (d * LAYER_HEIGHT * LINE_WIDTH);
  std::stringstream ss;
  ss << "G1 F2000 X" << _x << " Y" << _y << " Z" << _z  << " E" << _e << "\n";
  std::string s = ss.str();
  _commands.push_back(s);
}


template <typename M> 
void 
PrintheadT<M>::moveAlongXYAxis(float x, float y)
{
  _x += x;
  _y += y;
  std::stringstream ss;
  ss << "G1 F2000 X" << _x << " Y" << _y << "\n";
  std::string s = ss.str();
  _commands.push_back(s);
}

template <typename M> 
void 
PrintheadT<M>::extrudeAlongXYAxis(float x, float y)
{
  float d = sqrt(pow((_x - x), 2) + pow((_y - y), 2));
  _x += x;
  _y += y;
  _e += (d * LAYER_HEIGHT * LINE_WIDTH);
  std::stringstream ss;
  ss << "G1 F2000 X" << _x << " Y" << _y << " E" << _e << "\n";
  std::string s = ss.str();
  _commands.push_back(s);
}

template <typename M> 
void 
PrintheadT<M>::moveZAxis()
{
  _z += LAYER_HEIGHT;
  std::stringstream ss;
  ss << ";NEWLAYER\nG1 F1200 X" << _x << " Y" << _y << " Z" << _z << "\n";
  std::string s = ss.str();
  _commands.push_back(s);
}

template <typename M>
const std::vector<std::string>
PrintheadT<M>::getCommands()
{
  return _commands;
}


#endif
