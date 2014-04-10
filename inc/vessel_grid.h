#ifndef VESSEL_GRID_H_
#define VESSEL_GRID_H_

#include "main.h"

class Cell;
class GridManager;

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
  Vector3i vStart;
  Vector3i vSize;
};

class VesselGrid {
public:
  enum VesselGridType {
    VGT_NORMAL,
    VGT_CYCLED
  };

  VesselGrid() : m_pVGInfo(new VesselGridInfo) {}
  virtual ~VesselGrid() {}

  void setGridManager(GridManager* pGridManager) { m_pGridManager = pGridManager; }
  GridManager* getGridManager() { return m_pGridManager; }

  // this is a slice of 3D, so it must be 2D, that's why Ny, Nz
  // they must be "mod 2 = 0"
  // also i need vAreastep, meaning this is normal areastep
  VesselGridInfo* getVesselGridInfo() { return m_pVGInfo.get(); }

  void SetVesselGridType(VesselGridType eType) { m_eType = eType; }
  void CreateAndLinkVessel();
  void PrintLinkage(sep::Axis aAxis);

  // access method for saving grid data
  const std::vector< std::vector<Cell*> >& GetPrintVector() const { return m_vPrintVector; }
  const Vector2i& GetPrintVectorSize() const { return m_vPrintVectorSize; }

  // access method for linking last colomn
  Cell* GetBorderCell(int iIndexY) { return m_vCells[m_vCells.size() - m_pVGInfo->iNy + iIndexY].get(); }

  // action methods:
  void computeType(sep::Axis aAxis);
  void computeHalf(sep::Axis aAxis);
  void computeValue(sep::Axis aAxis);
  void computeIntegral(unsigned int gi0, unsigned int gi1);
  void computeMacroData();
protected:
  GridManager* m_pGridManager;

  VesselGridType m_eType;

  std::shared_ptr<VesselGridInfo> m_pVGInfo;
  std::vector< std::shared_ptr<Cell> > m_vCells;
  std::vector< std::vector<Cell*> > m_vPrintVector;
  Vector2i m_vPrintVectorSize;

  virtual void create_normal_vessel() = 0;
  virtual void create_cycled_vessel() = 0;

  virtual void link_normal_vessel() = 0;
  virtual void link_cycled_vessel() = 0;

  virtual void generate_normal_print_vector() = 0;
  virtual void generate_cycled_print_vector() = 0;

  // helpers
  int compute_binary_summ(int value);
  // value = 2^n; retrun n;
  int compute_binary_log(int value);
  int compute_binary_pow(int value, int n);
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

  virtual void generate_normal_print_vector();
  virtual void generate_cycled_print_vector();
};

class RightVesselGrid : public VesselGrid {
public:
  RightVesselGrid() {}
  virtual ~RightVesselGrid() {}

protected:
  virtual void create_normal_vessel();
  virtual void create_cycled_vessel();

  virtual void link_normal_vessel();
  virtual void link_cycled_vessel();

  virtual void generate_normal_print_vector();
  virtual void generate_cycled_print_vector();
};

#endif // VESSEL_GRID_H_