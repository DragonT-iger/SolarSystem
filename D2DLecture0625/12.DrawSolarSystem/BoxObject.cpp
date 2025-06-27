#include "pch.h"
#include "BoxObject.h"
#include "SolarSystemRenderer.h"
#include <iostream>

int BoxObject::s_id = 0;

BoxObject::BoxObject(Microsoft::WRL::ComPtr<ID2D1Bitmap1>& bitmap)
    : m_BitmapPtr(bitmap)
{
    ++s_id;
    m_name += std::to_wstring(s_id);
    m_renderTM = MYTM::MakeRenderMatrix(true);
    D2D1_SIZE_F size = { m_rect.right - m_rect.left, m_rect.bottom - m_rect.top };
    m_transform.SetPivotPreset(D2DTM::PivotPreset::Center, size);
}

BoxObject::~BoxObject() = default;

void BoxObject::Update(float deltaTime)
{
    if (m_isSelfRotation)
        m_transform.Rotate(deltaTime * 36.f);
}

void BoxObject::Draw(myspace::D2DRenderer& testRender, D2D1::Matrix3x2F viewTM)
{
    static D2D1_RECT_F s_rect = D2D1::RectF(0.f, 0.f, 100.f, 100.f);
    D2D1::Matrix3x2F worldTM = m_transform.GetWorldMatrix();
    D2D1::Matrix3x2F finalTM = m_renderTM * worldTM * viewTM;

    D2D1::ColorF boxColor = D2D1::ColorF::LightGray;
    if (m_isLeader) boxColor = D2D1::ColorF::Red;
    else if (m_isSelected) boxColor = D2D1::ColorF::HotPink;

    testRender.SetTransform(finalTM);
    testRender.DrawRectangle(s_rect.left, s_rect.top, s_rect.right, s_rect.bottom, boxColor);

    D2D1_RECT_F dest = D2D1::RectF(s_rect.left, s_rect.top, s_rect.right, s_rect.bottom);
    testRender.DrawBitmap(m_BitmapPtr.Get(), dest);
    testRender.DrawMessage(m_name.c_str(), s_rect.left, s_rect.top, 200, 50, D2D1::ColorF::Black);
}

void BoxObject::SetPosition(const MYHelper::Vector2F& pos)
{
    m_transform.SetPosition(pos);
}

void BoxObject::Move(const MYHelper::Vector2F& offset)
{
    m_transform.Translate(offset);
}

void BoxObject::Rotate(float angle)
{
    m_transform.Rotate(angle);
}

void BoxObject::ToggleSelected()
{
    m_isSelected = !m_isSelected;
}

bool BoxObject::IsSelected() const
{
    return m_isSelected;
}

void BoxObject::ToggleSelfRotation()
{
    m_isSelfRotation = !m_isSelfRotation;
}

bool BoxObject::IsHitTest(D2D1_POINT_2F worldPoint, D2D1::Matrix3x2F viewTM)
{
    D2D1::Matrix3x2F worldTM = m_transform.GetWorldMatrix();
    D2D1::Matrix3x2F finalTM = m_renderTM * worldTM * viewTM;
    finalTM.Invert();

    D2D1_POINT_2F localPt = finalTM.TransformPoint(worldPoint);
    std::cout << "BoxObject::IsHitTest: localPt = ("
        << localPt.x << ", " << localPt.y << ")" << std::endl;
    std::cout << "BoxObject::IsHitTest: m_rect = ("
        << m_rect.left << ", " << m_rect.top << ", "
        << m_rect.right << ", " << m_rect.bottom << ")" << std::endl;

    return MYTM::IsPointInRect(localPt, m_rect);
}

D2DTM::Transform* BoxObject::GetTransform()
{
    return &m_transform;
}

void BoxObject::SetParent(BoxObject* parent)
{
    assert(parent);
    if (m_transform.GetParent())
        m_transform.DetachFromParent();
    m_transform.SetParent(parent->GetTransform());
}

void BoxObject::DetachFromParent()
{
    m_transform.DetachFromParent();
}

void BoxObject::SetLeader(bool isLeader)
{
    m_isLeader = isLeader;
}

D2D1_RECT_F BoxObject::GetRect()
{
    return m_rect;
}
