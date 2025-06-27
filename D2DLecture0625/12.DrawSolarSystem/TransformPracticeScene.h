#pragma once
#include "OnlyForTestScene.h"
#include "Camera2D.h"
#include <list>
#include <vector>
#include <wrl/client.h>
#include <d2d1_1.h>
#include "BoxObject.h"


class TransformPracticeScene : public OnlyForTestScene
{
public:
    TransformPracticeScene() = default;
    virtual ~TransformPracticeScene();
    
    void SetUp(HWND hWnd) override;

    void Tick(float deltaTime) override;

    void OnResize(int width, int height) override;

private:

    void ProcessKeyboardEvents();

    void AddPlanet(D2D1_POINT_2F localPos , ComPtr<ID2D1Bitmap1> Bitmap);

    void AddSatellite(D2D1_POINT_2F localPos, int index, ComPtr<ID2D1Bitmap1> bitmap);

    void ClearBoxObjects();

    void SelectBoxObject(D2D1_POINT_2F point);

    void SetBoxSelfRotation();

    void UpdateRelationship();
   
    UnityCamera m_UnityCamera;


    std::vector<BoxObject*> m_BoxObjects;

    ComPtr<ID2D1Bitmap1> m_BitmapPtr;



    ComPtr<ID2D1Bitmap1> m_SunBitmapPtr;
    ComPtr<ID2D1Bitmap1> m_EarthBitmapPtr;
    ComPtr<ID2D1Bitmap1> m_MoonBitmapPtr;
    //¸ñ¼º
	ComPtr<ID2D1Bitmap1> m_JupiterBitmapPtr;

};

