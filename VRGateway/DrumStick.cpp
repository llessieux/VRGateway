#include "stdafx.h"
#include "DrumStick.h"


DrumStick::DrumStick() :
    m_length(0),
    m_radius(0),
    m_thickness(0)
{
}


DrumStick::~DrumStick()
{
}

void DrumStick::Initialize(float length, float radius, float thickness)
{
    m_length = length;
    m_radius = radius;
    m_thickness = thickness;
}

void DrumStick::SetTipPosition(const Vector3 &pos)
{
    m_tip = pos;
}

void DrumStick::SetOriginPosition(const Vector3 &pos)
{
    m_origin = pos;
}

const Vector3 &DrumStick::GetTipPosition() const
{
    return m_tip;
}

const Vector3 &DrumStick::GetOriginPosition() const
{
    return m_origin;
}

float DrumStick::GetLength() const
{
    return m_length;
}

float DrumStick::GetRadius() const
{
    return m_radius;
}