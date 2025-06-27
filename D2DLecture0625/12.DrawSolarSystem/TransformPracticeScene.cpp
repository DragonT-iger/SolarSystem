#include "pch.h"
#include "InputManager.h"
#include "D2DTransform.h"
#include "SolarSystemRenderer.h"
#include "TransformPracticeScene.h"


using TestRenderer = myspace::D2DRenderer;
using Vec2 = MYHelper::Vector2F;



TransformPracticeScene::~TransformPracticeScene()
{
    for (auto& box : m_BoxObjects) {
        delete box;
    }
}

void TransformPracticeScene::SetUp(HWND hWnd)
{
    m_hWnd = hWnd;

    SetWindowText(m_hWnd, 
    L"가상의 태양계를 만들어 주세요. 물리 법칙은 무시 합니다. ^^;;");


    //L"[이동키] 카메라, [ASDW] [스페이스]고양이 이동과 회전, [L-Btn] 생성, [R-Btn] 선택, [F1] 클리어");

    std::cout << "태양은 자전을 해야 합니다." << std::endl;
    std::cout << "행성들은 자전을 하며 동시에 태양의 자전에 영향을 받아 공전하는 것처럼 보입니다."<< std::endl;
    std::cout << "달은 자전을 하면서 동시에 지구의 자전에 영향을 받아 공전하는 것처럼 보입니다." << std::endl;
    std::cout << "회전 속도는 자유롭게 설정하세요." << std::endl;

   
    RECT rc;
    if (::GetClientRect(hWnd, &rc))
    {
        float w = static_cast<float>(rc.right - rc.left);
        float h = static_cast<float>(rc.bottom - rc.top);

        m_UnityCamera.SetScreenSize(w, h);
    }

    SolarSystemRenderer::Instance().CreateBitmapFromFile(L"../Resource/cat.png", *m_BitmapPtr.GetAddressOf());

    SolarSystemRenderer::Instance().CreateBitmapFromFile(L"../Resource/Earth.png", *m_EarthBitmapPtr.GetAddressOf());
    
	SolarSystemRenderer::Instance().CreateBitmapFromFile(L"../Resource/Sun.png", *m_SunBitmapPtr.GetAddressOf());
    
	SolarSystemRenderer::Instance().CreateBitmapFromFile(L"../Resource/Moon.png", *m_MoonBitmapPtr.GetAddressOf());
	SolarSystemRenderer::Instance().CreateBitmapFromFile(L"../Resource/Jupiter.png", *m_JupiterBitmapPtr.GetAddressOf());




    // 행성 위성등등 추가 로컬 좌표 기준 , 피봇은 중앙

    AddPlanet({ 0, 0 } , m_SunBitmapPtr); // 태양
    AddPlanet({ 200 ,200 } , m_EarthBitmapPtr); // 지구
	AddPlanet({ -200, 200 }, m_JupiterBitmapPtr); // 목성


    // 행성을 추가하고 위성을 추가해야됨 지금 로직은

    AddSatellite({ 200, 100 }, 2 , m_MoonBitmapPtr); // 달
	AddSatellite({ -200, 50 }, 3, m_MoonBitmapPtr); // 목성의 위성
}

void TransformPracticeScene::Tick(float deltaTime)
{

    ProcessKeyboardEvents();

	std::vector<float> planetSpeed = { 0.2f, 0.3f, 0.1f ,0 , 0.2f}; // 행성 회전 속도
	int index = 0;

    for (auto& box : m_BoxObjects) {
        box->Update(deltaTime);
        box->GetTransform()->Rotate(planetSpeed[index]);
		index++;
    }

    // 카메라 업데이트

    MAT3X2F cameraTM = m_UnityCamera.GetViewMatrix();

    MAT3X2F renderTM = MYTM::MakeRenderMatrix(true); // 카메라 위치 렌더링 매트릭스

    MAT3X2F finalTM = renderTM * cameraTM;

    // 렌더링

    static myspace::D2DRenderer& globalRenderer = SolarSystemRenderer::Instance();

    wchar_t buffer[128] = L"";
    MYTM::MakeMatrixToString(cameraTM, buffer, 128);

    globalRenderer.RenderBegin();

    globalRenderer.SetTransform(finalTM);

    // 카메라 위치 표시
    globalRenderer.DrawRectangle(-10.f, 10.f, 10.f, -10.f, D2D1::ColorF::Red);
    globalRenderer.DrawCircle(0.f, 0.f, 5.f, D2D1::ColorF::Red);
    //globalRenderer.DrawMessage(buffer, 10.f, 10.f, 100.f, 100.f, D2D1::ColorF::Red);

    for (auto& box : m_BoxObjects)
    {
        box->Draw(globalRenderer, cameraTM);
    }

    globalRenderer.RenderEnd();
}

void TransformPracticeScene::OnResize(int width, int height)
{ 
    m_UnityCamera.SetScreenSize(width, height);
}

void TransformPracticeScene::ProcessKeyboardEvents()
{

    // 카메라 이동 처리, 
    static const std::vector<std::pair<int, Vec2>> kCameraMoves = {
      { VK_RIGHT, {  1.f,  0.f } },
      { VK_LEFT,  { -1.f,  0.f } },
      { VK_UP,    {  0.f,  1.f } },
      { VK_DOWN,  {  0.f, -1.f } },
    };

    // C++17부터는 structured binding을 사용하여 더 간결하게 표현할 수 있습니다.
    for (auto& [vk, dir] : kCameraMoves)
    {
        if (InputManager::Instance().GetKeyDown(vk))
        {
            m_UnityCamera.Move(dir.x, dir.y);
        }
    }

    // 첫번째 선택된 박스를 이동
    static const std::vector<std::pair<int, Vec2>> kBoxMoves = {
      { 'D', {  1.f,  0.f } }, // D키로 오른쪽 이동
      { 'A', { -1.f,  0.f } }, // A키로 왼쪽 이동
      { 'W', {  0.f,  1.f } }, // W키로 위로 이동
      { 'S', {  0.f, -1.f } }, // S키로 아래로 이동
    };

    for (auto& [vk, dir] : kBoxMoves)
    {
        if (InputManager::Instance().GetKeyDown(vk))
        {
            if (!m_BoxObjects.empty()) {
                m_BoxObjects.front()->Move(dir);
            }
        }
    }
}


void TransformPracticeScene::AddPlanet(D2D1_POINT_2F localPos, ComPtr<ID2D1Bitmap1> bitmap)
{
    BoxObject* pNewBox = new BoxObject(bitmap);
    D2D1_RECT_F rect = pNewBox->GetRect();


    pNewBox->GetTransform()->SetPivotPreset(D2DTM::PivotPreset::Center, { rect.right - rect.left , rect.bottom - rect.top });

    if (m_BoxObjects.size() == 0) {
        

        pNewBox->SetPosition(Vec2(localPos.x , localPos.y));

        m_BoxObjects.push_back(pNewBox);
    }
    else {

        pNewBox->SetPosition(Vec2(localPos.x, localPos.y));
        pNewBox->GetTransform()->SetParent((*m_BoxObjects.front()).GetTransform());
        m_BoxObjects.push_back(pNewBox);
    }
}

void TransformPracticeScene::AddSatellite(D2D1_POINT_2F localPos, int index, ComPtr<ID2D1Bitmap1> bitmap)
{
    

    if (index <= 0) {
        std::cout << "인덱스가 0보다 작은 값을 AddSatellite함수에 입력할 수 없습니다" << std::endl;
        return;
    }

    for (auto it = m_BoxObjects.begin(); it != m_BoxObjects.end(); it++) {

        index--;

        if (index == 0) {
            BoxObject* pNewBox = new BoxObject(bitmap);
            D2D1_RECT_F rect = pNewBox->GetRect();
            pNewBox->GetTransform()->SetPivotPreset(D2DTM::PivotPreset::Center, { rect.right - rect.left , rect.bottom - rect.top });
            pNewBox->SetPosition(Vec2(localPos.x, localPos.y));
            pNewBox->GetTransform()->SetParent((*it)->GetTransform());
            m_BoxObjects.push_back(pNewBox);
            return;
        }
    }
    std::cout << "항성 OR 행성을 찾을수 없습니다" << std::endl;
}
