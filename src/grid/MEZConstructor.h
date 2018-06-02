#ifndef RGS_MEZCONSTRUCTOR_H
#define RGS_MEZCONSTRUCTOR_H

#include "GridConstructor.h"
#include "CellData.h"

class MEZConstructor : public GridConstructor {
public:
    std::vector<GridBox*> createBoxes() override {
        std::vector<GridBox*> boxes;

        GridBox* box = nullptr;

        const double sizeX = 5.0 / 18;
        const double sizeY = 0.4 / 5;

        box = new GridBox(Vector2d(0.0, 0.0), Vector2d(100.0, 100.0), false);
        box->setMainFunction([](Vector2d point, CellData& data) {
            data.params().set(0, 1.0, 1.0, 1.0);
            data.setStep(Vector3d(0.1, 0.1, 0.0));
        });
        box->setLeftBorderFunction([](double point, CellData& data) {
            data.setBoundaryTypes(0, CellData::BoundaryType::DIFFUSE);
            data.boundaryParams().setTemp(0, 1.0);
            data.setStep(Vector3d(0.1, 0.1, 0.0));
        });
        box->setRightBorderFunction([](double point, CellData& data) {
            data.setBoundaryTypes(0, CellData::BoundaryType::DIFFUSE);
            data.boundaryParams().setTemp(0, 1.0);
            data.setStep(Vector3d(0.1, 0.1, 0.0));
        });
        box->setTopBorderFunction([](double point, CellData& data) {
            data.setBoundaryTypes(0, CellData::BoundaryType::DIFFUSE);
            data.boundaryParams().setTemp(0, 1.0);
            data.setStep(Vector3d(0.1, 0.1, 0.0));
        });
        box->setBottomBorderFunction([](double point, CellData& data) {
            data.setBoundaryTypes(0, CellData::BoundaryType::DIFFUSE);
            data.boundaryParams().setTemp(0, 1.0);
            data.setStep(Vector3d(0.1, 0.1, 0.0));
        });
        boxes.push_back(box);

        box = new GridBox(Vector2d(20.0, 10.0), Vector2d(40.0, 20.0), true);
        box->setLeftBorderFunction([](double point, CellData& data) {
            data.setBoundaryTypes(0, CellData::BoundaryType::DIFFUSE);
            data.boundaryParams().setTemp(0, 2.0);
            data.setStep(Vector3d(0.1, 0.1, 0.0));
        });
        box->setRightBorderFunction([](double point, CellData& data) {
            data.setBoundaryTypes(0, CellData::BoundaryType::DIFFUSE);
            data.boundaryParams().setTemp(0, 2.0);
            data.setStep(Vector3d(0.1, 0.1, 0.0));
        });
        box->setTopBorderFunction([](double point, CellData& data) {
            data.setBoundaryTypes(0, CellData::BoundaryType::DIFFUSE);
            data.boundaryParams().setTemp(0, 2.0);
            data.setStep(Vector3d(0.1, 0.1, 0.0));
        });
        box->setBottomBorderFunction([](double point, CellData& data) {
            data.setBoundaryTypes(0, CellData::BoundaryType::DIFFUSE);
            data.boundaryParams().setTemp(0, 2.0);
            data.setStep(Vector3d(0.1, 0.1, 0.0));
        });
        boxes.push_back(box);

        box = new GridBox(Vector2d(40.0, 40.0), Vector2d(40.0, 20.0), true);
        box->setLeftBorderFunction([](double point, CellData& data) {
            data.setBoundaryTypes(0, CellData::BoundaryType::DIFFUSE);
            data.boundaryParams().setTemp(0, 1.0);
            data.setStep(Vector3d(0.1, 0.1, 0.0));
        });
        box->setRightBorderFunction([](double point, CellData& data) {
            data.setBoundaryTypes(0, CellData::BoundaryType::DIFFUSE);
            data.boundaryParams().setTemp(0, 1.0);
            data.setStep(Vector3d(0.1, 0.1, 0.0));
        });
        box->setTopBorderFunction([](double point, CellData& data) {
            data.setBoundaryTypes(0, CellData::BoundaryType::DIFFUSE);
            data.boundaryParams().setTemp(0, 1.0);
            data.setStep(Vector3d(0.1, 0.1, 0.0));
        });
        box->setBottomBorderFunction([](double point, CellData& data) {
            data.setBoundaryTypes(0, CellData::BoundaryType::DIFFUSE);
            data.boundaryParams().setTemp(0, 1.0);
            data.setStep(Vector3d(0.1, 0.1, 0.0));
        });
        boxes.push_back(box);

        box = new GridBox(Vector2d(20.0, 70.0), Vector2d(40.0, 20.0), true);
        box->setLeftBorderFunction([](double point, CellData& data) {
            data.setBoundaryTypes(0, CellData::BoundaryType::DIFFUSE);
            data.boundaryParams().setTemp(0, 2.0);
            data.setStep(Vector3d(0.1, 0.1, 0.0));
        });
        box->setRightBorderFunction([](double point, CellData& data) {
            data.setBoundaryTypes(0, CellData::BoundaryType::DIFFUSE);
            data.boundaryParams().setTemp(0, 2.0);
            data.setStep(Vector3d(0.1, 0.1, 0.0));
        });
        box->setTopBorderFunction([](double point, CellData& data) {
            data.setBoundaryTypes(0, CellData::BoundaryType::DIFFUSE);
            data.boundaryParams().setTemp(0, 2.0);
            data.setStep(Vector3d(0.1, 0.1, 0.0));
        });
        box->setBottomBorderFunction([](double point, CellData& data) {
            data.setBoundaryTypes(0, CellData::BoundaryType::DIFFUSE);
            data.boundaryParams().setTemp(0, 2.0);
            data.setStep(Vector3d(0.1, 0.1, 0.0));
        });
        boxes.push_back(box);

        return boxes;
    }
};

#endif //RGS_MEZCONSTRUCTOR_H
