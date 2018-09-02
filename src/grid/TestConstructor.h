#ifndef RGS_TESTCONSTRUCTOR_H
#define RGS_TESTCONSTRUCTOR_H

#include "GridConstructor.h"
#include "CellData.h"

class TestConstructor : public GridConstructor {
public:
     std::vector<GridBox*> createBoxes() override {
        std::vector<GridBox*> boxes;

        GridBox* box = nullptr;

        const double sizeX = 100.0 / 18;
        const double sizeY = 100.0 / 5;

         box = new GridBox(Vector2d(0.0, 0.0), Vector2d(100.0, 100.0), false);
         box->setMainFunction([](Vector2d point, CellData& data) {
             data.params().set(0, 1.0, 1.0, 1.0);
             data.setVolume(Vector3d(0.1, 0.1, 0.0));
         });
         box->setLeftBorderFunction([](double point, CellData& data) {
             data.setBoundaryTypes(0, CellData::BoundaryType::DIFFUSE);
             data.boundaryParams().setTemp(0, 1.0);
             data.setVolume(Vector3d(0.1, 0.1, 0.0));
         });
         box->setRightBorderFunction([](double point, CellData& data) {
             data.setBoundaryTypes(0, CellData::BoundaryType::DIFFUSE);
             data.boundaryParams().setTemp(0, 1.0);
             data.setVolume(Vector3d(0.1, 0.1, 0.0));
         });
         box->setTopBorderFunction([](double point, CellData& data) {
             data.setBoundaryTypes(0, CellData::BoundaryType::DIFFUSE);
             data.boundaryParams().setTemp(0, 1.0);
             data.setVolume(Vector3d(0.1, 0.1, 0.0));
         });
         box->setBottomBorderFunction([](double point, CellData& data) {
             data.setBoundaryTypes(0, CellData::BoundaryType::DIFFUSE);
             data.boundaryParams().setTemp(0, 1.0);
             data.setVolume(Vector3d(0.1, 0.1, 0.0));
         });
         boxes.push_back(box);

         box = new GridBox(Vector2d(20.0, 10.0), Vector2d(40.0, 20.0), true);
         box->setLeftBorderFunction([](double point, CellData& data) {
             data.setBoundaryTypes(0, CellData::BoundaryType::DIFFUSE);
             data.boundaryParams().setTemp(0, 2.0);
             data.setVolume(Vector3d(0.1, 0.1, 0.0));
         });
         box->setRightBorderFunction([](double point, CellData& data) {
             data.setBoundaryTypes(0, CellData::BoundaryType::DIFFUSE);
             data.boundaryParams().setTemp(0, 2.0);
             data.setVolume(Vector3d(0.1, 0.1, 0.0));
         });
         box->setTopBorderFunction([](double point, CellData& data) {
             data.setBoundaryTypes(0, CellData::BoundaryType::DIFFUSE);
             data.boundaryParams().setTemp(0, 2.0);
             data.setVolume(Vector3d(0.1, 0.1, 0.0));
         });
         box->setBottomBorderFunction([](double point, CellData& data) {
             data.setBoundaryTypes(0, CellData::BoundaryType::DIFFUSE);
             data.boundaryParams().setTemp(0, 2.0);
             data.setVolume(Vector3d(0.1, 0.1, 0.0));
         });
         boxes.push_back(box);

         box = new GridBox(Vector2d(40.0, 40.0), Vector2d(40.0, 20.0), true);
         box->setLeftBorderFunction([](double point, CellData& data) {
             data.setBoundaryTypes(0, CellData::BoundaryType::DIFFUSE);
             data.boundaryParams().setTemp(0, 1.0);
             data.setVolume(Vector3d(0.1, 0.1, 0.0));
         });
         box->setRightBorderFunction([](double point, CellData& data) {
             data.setBoundaryTypes(0, CellData::BoundaryType::DIFFUSE);
             data.boundaryParams().setTemp(0, 1.0);
             data.setVolume(Vector3d(0.1, 0.1, 0.0));
         });
         box->setTopBorderFunction([](double point, CellData& data) {
             data.setBoundaryTypes(0, CellData::BoundaryType::DIFFUSE);
             data.boundaryParams().setTemp(0, 1.0);
             data.setVolume(Vector3d(0.1, 0.1, 0.0));
         });
         box->setBottomBorderFunction([](double point, CellData& data) {
             data.setBoundaryTypes(0, CellData::BoundaryType::DIFFUSE);
             data.boundaryParams().setTemp(0, 1.0);
             data.setVolume(Vector3d(0.1, 0.1, 0.0));
         });
         boxes.push_back(box);

         box = new GridBox(Vector2d(20.0, 70.0), Vector2d(40.0, 20.0), true);
         box->setLeftBorderFunction([](double point, CellData& data) {
             data.setBoundaryTypes(0, CellData::BoundaryType::DIFFUSE);
             data.boundaryParams().setTemp(0, 2.0);
             data.setVolume(Vector3d(0.1, 0.1, 0.0));
         });
         box->setRightBorderFunction([](double point, CellData& data) {
             data.setBoundaryTypes(0, CellData::BoundaryType::DIFFUSE);
             data.boundaryParams().setTemp(0, 2.0);
             data.setVolume(Vector3d(0.1, 0.1, 0.0));
         });
         box->setTopBorderFunction([](double point, CellData& data) {
             data.setBoundaryTypes(0, CellData::BoundaryType::DIFFUSE);
             data.boundaryParams().setTemp(0, 2.0);
             data.setVolume(Vector3d(0.1, 0.1, 0.0));
         });
         box->setBottomBorderFunction([](double point, CellData& data) {
             data.setBoundaryTypes(0, CellData::BoundaryType::DIFFUSE);
             data.boundaryParams().setTemp(0, 2.0);
             data.setVolume(Vector3d(0.1, 0.1, 0.0));
         });
         boxes.push_back(box);

        return boxes;
    }
};

#endif //RGS_TESTCONSTRUCTOR_H
