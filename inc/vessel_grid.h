#ifndef VESSEL_GRID_H_
#define VESSEL_GRID_H_

#include "main.h"

class Cell;

struct VesselGridInfo {
  VesselGridInfo() {
    dStartConcentration = 0.0;
    dStartTemperature = 0.0;
    vAreastep = Vector3d();
    iNy = 0;
    iNz = 0;
    iAdditionalLenght = 0;
  }

  double dStartConcentration;
  double dStartTemperature;
  Vector3d vAreastep;
  int iNy;
  int iNz;
  int iAdditionalLenght;
};

class VesselGrid {
public:
  enum VesselGridType {
    VGT_NORMAL,
    VGT_CYCLED
  };

  VesselGrid() : m_pVGInfo(new VesselGridInfo) {}
  virtual ~VesselGrid() {}
  // this is a slice of 3D, so it must be 2D, that's why Ny, Nz
  // they must be "mod 2 = 0"
  // also i need vAreastep, meaning this is normal areastep
  VesselGridInfo* getVesselGridInfo() { return m_pVGInfo.get(); }

  void SetVesselGridType(VesselGridType eType) { m_eType = eType; }
  void CreateAndLinkVessel();
  void PrintLinkage(sep::Axis aAxis);

  // access method for saving grid data
  const std::vector<Cell*>& GetPrintVector() const { return m_vPrintVector; }

  // access method for linking last colomn

  // action methods:
  void computeType(sep::Axis aAxis);
  void computeHalf(sep::Axis aAxis);
  void computeValue(sep::Axis aAxis);
  void computeIntegral(unsigned int gi0, unsigned int gi1);
  void computeMacroData();
protected:
  VesselGridType m_eType;

  std::shared_ptr<VesselGridInfo> m_pVGInfo;
  std::vector< std::shared_ptr<Cell> > m_vCells;
  std::vector<Cell*> m_vPrintVector;

  virtual void create_normal_vessel() = 0;
  virtual void create_cycled_vessel() = 0;

  virtual void link_normal_vessel() = 0;
  virtual void link_cycled_vessel() = 0;

  virtual void generate_normal_print_vector(std::vector<Cell*>& vPrintVector) = 0;
  virtual void generate_cycled_print_vector(std::vector<Cell*>& vPrintVector) = 0;

  // helpers
  int compute_binary_summ(int value);
};

class LeftVesselGrid : public VesselGrid {
public:
  LeftVesselGrid() {}
  virtual ~LeftVesselGrid() {}
  
protected:
  virtual void create_normal_vessel();
  virtual void create_cycled_vessel();

  virtual void link_normal_vessel();
  virtual void link_cycled_vessel();

  virtual void generate_normal_print_vector(std::vector<Cell*>& vPrintVector);
  virtual void generate_cycled_print_vector(std::vector<Cell*>& vPrintVector);
};

#endif // VESSEL_GRID_H_