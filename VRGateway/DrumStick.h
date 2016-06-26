#pragma once
class DrumStick
{
public:
    DrumStick();
    ~DrumStick();

    void SetTipPosition(const Vector3 &pos);
    void SetOriginPosition(const Vector3 &pos);

    const Vector3 &GetTipPosition() const;
    const Vector3 &GetOriginPosition() const;
    float GetLength() const;
    float GetRadius() const;

    void Initialize(float length, float radius, float thickness);
private:
    Vector3 m_tip;
    Vector3 m_origin;
    float m_length;
    float m_radius;
    float m_thickness;
};

