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
    L"������ �¾�踦 ����� �ּ���. ���� ��Ģ�� ���� �մϴ�. ^^;;");


    //L"[�̵�Ű] ī�޶�, [ASDW] [�����̽�]����� �̵��� ȸ��, [L-Btn] ����, [R-Btn] ����, [F1] Ŭ����");

    std::cout << "�¾��� ������ �ؾ� �մϴ�." << std::endl;
    std::cout << "�༺���� ������ �ϸ� ���ÿ� �¾��� ������ ������ �޾� �����ϴ� ��ó�� ���Դϴ�."<< std::endl;
    std::cout << "���� ������ �ϸ鼭 ���ÿ� ������ ������ ������ �޾� �����ϴ� ��ó�� ���Դϴ�." << std::endl;
    std::cout << "ȸ�� �ӵ��� �����Ӱ� �����ϼ���." << std::endl;

   
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




    // �༺ ������� �߰� ���� ��ǥ ���� , �Ǻ��� �߾�

    AddPlanet({ 0, 0 } , m_SunBitmapPtr); // �¾�
    AddPlanet({ 200 ,200 } , m_EarthBitmapPtr); // ����
	AddPlanet({ -200, 200 }, m_JupiterBitmapPtr); // ��


    // �༺�� �߰��ϰ� ������ �߰��ؾߵ� ���� ������

    AddSatellite({ 200, 100 }, 2 , m_MoonBitmapPtr); // ��
	AddSatellite({ -200, 50 }, 3, m_MoonBitmapPtr); // ���� ����
}

void TransformPracticeScene::Tick(float deltaTime)
{

    ProcessKeyboardEvents();

	std::vector<float> planetSpeed = { 0.2f, 0.3f, 0.1f ,0 , 0.2f}; // �༺ ȸ�� �ӵ�
	int index = 0;

    for (auto& box : m_BoxObjects) {
        box->Update(deltaTime);
        box->GetTransform()->Rotate(planetSpeed[index]);
		index++;
    }

    // ī�޶� ������Ʈ

    MAT3X2F cameraTM = m_UnityCamera.GetViewMatrix();

    MAT3X2F renderTM = MYTM::MakeRenderMatrix(true); // ī�޶� ��ġ ������ ��Ʈ����

    MAT3X2F finalTM = renderTM * cameraTM;

    // ������

    static myspace::D2DRenderer& globalRenderer = SolarSystemRenderer::Instance();

    wchar_t buffer[128] = L"";
    MYTM::MakeMatrixToString(cameraTM, buffer, 128);

    globalRenderer.RenderBegin();

    globalRenderer.SetTransform(finalTM);

    // ī�޶� ��ġ ǥ��
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

    // ī�޶� �̵� ó��, 
    static const std::vector<std::pair<int, Vec2>> kCameraMoves = {
      { VK_RIGHT, {  1.f,  0.f } },
      { VK_LEFT,  { -1.f,  0.f } },
      { VK_UP,    {  0.f,  1.f } },
      { VK_DOWN,  {  0.f, -1.f } },
    };

    // C++17���ʹ� structured binding�� ����Ͽ� �� �����ϰ� ǥ���� �� �ֽ��ϴ�.
    for (auto& [vk, dir] : kCameraMoves)
    {
        if (InputManager::Instance().GetKeyDown(vk))
        {
            m_UnityCamera.Move(dir.x, dir.y);
        }
    }

    // ù��° ���õ� �ڽ��� �̵�
    static const std::vector<std::pair<int, Vec2>> kBoxMoves = {
      { 'D', {  1.f,  0.f } }, // DŰ�� ������ �̵�
      { 'A', { -1.f,  0.f } }, // AŰ�� ���� �̵�
      { 'W', {  0.f,  1.f } }, // WŰ�� ���� �̵�
      { 'S', {  0.f, -1.f } }, // SŰ�� �Ʒ��� �̵�
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
        std::cout << "�ε����� 0���� ���� ���� AddSatellite�Լ��� �Է��� �� �����ϴ�" << std::endl;
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
    std::cout << "�׼� OR �༺�� ã���� �����ϴ�" << std::endl;
}
