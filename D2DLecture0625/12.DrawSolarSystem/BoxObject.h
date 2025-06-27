#pragma once

#include <wrl/client.h>
#include <d2d1.h>
#include <string>
#include <cassert>
#include "D2DTransform.h"


namespace myspace { class D2DRenderer; }

class BoxObject {
public:
    BoxObject(Microsoft::WRL::ComPtr<ID2D1Bitmap1>& bitmap);
    ~BoxObject();

    void Update(float deltaTime);
    void Draw(myspace::D2DRenderer& renderer, D2D1::Matrix3x2F viewTM);

    void SetPosition(const MYHelper::Vector2F& pos);
    void Move(const MYHelper::Vector2F& offset);
    void Rotate(float angle);

    void ToggleSelected();
    bool IsSelected() const;
    void ToggleSelfRotation();

    bool IsHitTest(D2D1_POINT_2F worldPoint, D2D1::Matrix3x2F viewTM);

    D2DTM::Transform* GetTransform();
    void SetParent(BoxObject* parent);
    void DetachFromParent();
    void SetLeader(bool isLeader);

    D2D1_RECT_F GetRect();

private:
    BoxObject() = delete;
    BoxObject(const BoxObject&) = delete;
    BoxObject& operator=(const BoxObject&) = delete;

    D2DTM::Transform m_transform;
    D2D1::Matrix3x2F m_renderTM;
    D2D1_RECT_F m_rect = D2D1::RectF(0.f, 0.f, 100.f, 100.f);
    std::wstring m_name = L"";
    bool m_isSelected = false;
    bool m_isLeader = false;
    bool m_isSelfRotation = false;
    Microsoft::WRL::ComPtr<ID2D1Bitmap1> m_BitmapPtr;
    static int s_id;
};
