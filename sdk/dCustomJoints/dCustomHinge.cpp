/* Copyright (c) <2003-2016> <Newton Game Dynamics>
* 
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
* 
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely
*/



// dCustomHinge.cpp: implementation of the dCustomHinge class.
//
//////////////////////////////////////////////////////////////////////
#include "dCustomJointLibraryStdAfx.h"
#include "dCustomHinge.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_CUSTOM_JOINT(dCustomHinge);

dCustomHinge::dCustomHinge (const dMatrix& pinAndPivotFrame, NewtonBody* const child, NewtonBody* const parent)
	:dCustom6dof(pinAndPivotFrame, child, parent)
	,m_friction(0.0f)
	,m_jointOmega(0.0f)
	,m_spring(0.0f)
	,m_damper(0.0f)
	,m_springDamperRelaxation(0.97f)
	,m_options(0)
{
	m_pitchAxis = 0;
	m_limitsOn = false;
	m_actuatorFlag = false;
	m_setAsSpringDamper = false;
	SetLimits(-45.0f * 3.141592f / 180.0f, 45.0f * 3.141592f / 180.0f);
}

dCustomHinge::dCustomHinge (const dMatrix& pinAndPivotFrameChild, const dMatrix& pinAndPivotFrameParent, NewtonBody* const child, NewtonBody* const parent)
	:dCustom6dof(pinAndPivotFrameChild, pinAndPivotFrameParent, child, parent)
	,m_friction(0.0f)
	,m_jointOmega(0.0f)
	,m_spring(0.0f)
	,m_damper(0.0f)
	,m_springDamperRelaxation(0.97f)
	,m_options(0)
{
	m_pitchAxis = 0;
	m_limitsOn = false;
	m_actuatorFlag = false;
	m_setAsSpringDamper = false;
	SetLimits(-45.0f * 3.141592f / 180.0f, 45.0f * 3.141592f / 180.0f);
}

dCustomHinge::~dCustomHinge()
{
}

void dCustomHinge::Deserialize (NewtonDeserializeCallback callback, void* const userData)
{
	callback (userData, &m_friction, sizeof (m_friction));
	callback (userData, &m_jointOmega, sizeof (m_jointOmega));
	callback (userData, &m_spring, sizeof (m_spring));
	callback (userData, &m_damper, sizeof (m_damper));
	callback (userData, &m_springDamperRelaxation, sizeof (m_springDamperRelaxation));
	callback(userData, &m_options, sizeof(m_options));
}

void dCustomHinge::Serialize (NewtonSerializeCallback callback, void* const userData) const
{
	dCustomJoint::Serialize (callback, userData);

	callback(userData, &m_friction, sizeof(m_friction));
	callback(userData, &m_jointOmega, sizeof(m_jointOmega));
	callback(userData, &m_spring, sizeof(m_spring));
	callback(userData, &m_damper, sizeof(m_damper));
	callback(userData, &m_springDamperRelaxation, sizeof(m_springDamperRelaxation));
	callback(userData, &m_options, sizeof(m_options));
}


void dCustomHinge::EnableLimits(bool state)
{
	m_limitsOn = state;
}

void dCustomHinge::SetLimits(dFloat minAngle, dFloat maxAngle)
{
	SetPitchLimits(minAngle, maxAngle);
}

void dCustomHinge::SetAsSpringDamper(bool state, dFloat springDamperRelaxation, dFloat spring, dFloat damper)
{
	m_setAsSpringDamper = state;
	m_spring = spring;
	m_damper = damper;
	m_springDamperRelaxation = dClamp(springDamperRelaxation, dFloat(0.0f), dFloat(0.999f));
}

dFloat dCustomHinge::GetJointAngle () const
{
	dAssert (0);
	return 0.0f;
//	return m_curJointAngle.GetAngle();
}

dVector dCustomHinge::GetPinAxis () const
{
	dMatrix matrix;
	NewtonBodyGetMatrix (m_body0, &matrix[0][0]);
	return matrix.RotateVector (m_localMatrix0.m_front);
}

dFloat dCustomHinge::GetJointOmega () const
{
	return m_jointOmega;
}

void dCustomHinge::SetFriction (dFloat frictionTorque)
{
	m_friction = frictionTorque;
}

dFloat dCustomHinge::GetFriction () const
{
	return m_friction;
}

#if 0
void dCustomHinge::SubmitConstraints(dFloat timestep, int threadIndex)
{
	dMatrix matrix0;
	dMatrix matrix1;
	dFloat sinAngle;
	dFloat cosAngle;

	// calculate the position of the pivot point and the Jacobian direction vectors, in global space. 
	CalculateGlobalMatrix(matrix0, matrix1);

	// Restrict the movement on the pivot point along all tree orthonormal direction
	NewtonUserJointAddLinearRow(m_joint, &matrix0.m_posit[0], &matrix1.m_posit[0], &matrix1.m_front[0]);
	NewtonUserJointSetRowStiffness(m_joint, m_stiffness);
	NewtonUserJointAddLinearRow(m_joint, &matrix0.m_posit[0], &matrix1.m_posit[0], &matrix1.m_up[0]);
	NewtonUserJointSetRowStiffness(m_joint, m_stiffness);
	NewtonUserJointAddLinearRow(m_joint, &matrix0.m_posit[0], &matrix1.m_posit[0], &matrix1.m_right[0]);
	NewtonUserJointSetRowStiffness(m_joint, m_stiffness);

	// two rows to restrict rotation around around the parent coordinate system
	NewtonUserJointAddAngularRow(m_joint, CalculateAngle(matrix0.m_front, matrix1.m_front, matrix1.m_up), &matrix1.m_up[0]);
	NewtonUserJointSetRowStiffness(m_joint, m_stiffness);
	NewtonUserJointAddAngularRow(m_joint, CalculateAngle(matrix0.m_front, matrix1.m_front, matrix1.m_right), &matrix1.m_right[0]);
	NewtonUserJointSetRowStiffness(m_joint, m_stiffness);

	// the joint angle can be determined by getting the angle between any two non parallel vectors
	CalculateAngle (matrix1.m_up, matrix0.m_up, matrix1.m_front, sinAngle, cosAngle);
	m_curJointAngle.Update(cosAngle, sinAngle);

	// save the current joint Omega
	dVector omega0(0.0f);
	dVector omega1(0.0f);
	NewtonBodyGetOmega(m_body0, &omega0[0]);
	if (m_body1) {
		NewtonBodyGetOmega(m_body1, &omega1[0]);
	}
	m_jointOmega = (omega0 - omega1).DotProduct3(matrix1.m_front);

	m_lastRowWasUsed = false;
	if (m_setAsSpringDamper) {
		ApplySpringDamper (timestep, matrix0, matrix1);
	} else {
		SubmitConstraintsFreeDof (timestep, matrix0, matrix1);
	}
}

void dCustomHinge::SubmitConstraintsFreeDof(dFloat timestep, const dMatrix& matrix0, const dMatrix& matrix1)
{
	// four possibilities
	dFloat angle = m_curJointAngle.GetAngle();

	if (m_friction != 0.0f) {
		if (m_limitsOn) {
			// friction and limits at the same time
			if (angle < m_minAngle) {
				dFloat relAngle = angle - m_minAngle;

				// tell joint error will minimize the exceeded angle error
				NewtonUserJointAddAngularRow(m_joint, -relAngle, &matrix1.m_front[0]);

				// need high stiffness here
				NewtonUserJointSetRowStiffness(m_joint, 1.0f);

				// allow the joint to move back freely 
				NewtonUserJointSetRowMinimumFriction(m_joint, 0.0f);

				m_lastRowWasUsed = true;
			} else if (angle > m_maxAngle) {
				dFloat relAngle = angle - m_maxAngle;

				// tell joint error will minimize the exceeded angle error
				NewtonUserJointAddAngularRow(m_joint, -relAngle, &matrix1.m_front[0]);

				// need high stiffness here
				NewtonUserJointSetRowStiffness(m_joint, 1.0f);

				// allow the joint to move back freely
				NewtonUserJointSetRowMaximumFriction(m_joint, 0.0f);

				m_lastRowWasUsed = true;
			} else {
				// friction but not limits
				dFloat alpha = m_jointOmega / timestep;
				NewtonUserJointAddAngularRow(m_joint, 0, &matrix1.m_front[0]);
				NewtonUserJointSetRowAcceleration(m_joint, -alpha);
				NewtonUserJointSetRowMinimumFriction(m_joint, -m_friction);
				NewtonUserJointSetRowMaximumFriction(m_joint, m_friction);
				NewtonUserJointSetRowStiffness(m_joint, 1.0f);
				m_lastRowWasUsed = true;
			}
		} else {
			// friction but not limits
			dFloat alpha = m_jointOmega / timestep;
			NewtonUserJointAddAngularRow(m_joint, 0, &matrix1.m_front[0]);
			NewtonUserJointSetRowAcceleration(m_joint, -alpha);
			NewtonUserJointSetRowMinimumFriction(m_joint, -m_friction);
			NewtonUserJointSetRowMaximumFriction(m_joint, m_friction);
			NewtonUserJointSetRowStiffness(m_joint, 1.0f);

			m_lastRowWasUsed = true;
		}
	} else if (m_limitsOn) {
		// only limit are on 
		// the joint angle can be determine by getting the angle between any two non parallel vectors
		if ((m_minAngle > -1.e-4f) && (m_maxAngle < 1.e-4f)) {
			NewtonUserJointAddAngularRow(m_joint, -angle, &matrix1.m_front[0]);
			NewtonUserJointSetRowStiffness(m_joint, 1.0f);
			m_lastRowWasUsed = true;

		} else if (angle < m_minAngle) {
			dFloat relAngle = angle - m_minAngle;

			// tell joint error will minimize the exceeded angle error
			NewtonUserJointAddAngularRow(m_joint, -relAngle, &matrix1.m_front[0]);

			// need high stiffness here
			NewtonUserJointSetRowStiffness(m_joint, 1.0f);

			// allow the joint to move back freely 
			NewtonUserJointSetRowMinimumFriction(m_joint, 0.0f);
			m_lastRowWasUsed = true;
		} else if (angle > m_maxAngle) {
			dFloat relAngle = angle - m_maxAngle;

			// tell joint error will minimize the exceeded angle error
			NewtonUserJointAddAngularRow(m_joint, -relAngle, &matrix1.m_front[0]);

			// need high stiffness here
			NewtonUserJointSetRowStiffness(m_joint, 1.0f);

			// allow the joint to move back freely
			NewtonUserJointSetRowMaximumFriction(m_joint, 0.0f);
			m_lastRowWasUsed = true;
		}
	}
}

void dCustomHinge::ApplySpringDamper (dFloat timestep, const dMatrix& matrix0, const dMatrix& matrix1)
{
	m_lastRowWasUsed = true;
	NewtonUserJointAddAngularRow(m_joint, 0.0f, &matrix1.m_front[0]);
	NewtonUserJointSetRowSpringDamperAcceleration(m_joint, m_springDamperRelaxation, m_spring, m_damper);
}
#endif


void dCustomHinge::SubmitConstraintsFreeDof(int freeDof, const dMatrix& matrix0, const dMatrix& matrix1, dFloat timestep, int threadIndex)
{
	dAssert (freeDof == 1);

	dVector omega0(0.0f);
	dVector omega1(0.0f);
	NewtonBodyGetOmega(m_body0, &omega0[0]);
	if (m_body1) {
		NewtonBodyGetOmega(m_body1, &omega1[0]);
	}
	m_jointOmega = (omega0 - omega1).DotProduct3(matrix1.m_front);

//m_friction = 0;
//m_limitsOn = false;
//m_setAsSpringDamper = 1;
//m_spring = 1000;
//m_damper = 10.0f;
//m_springDamperRelaxation = 0.97f;

	if (m_setAsSpringDamper) {
		NewtonUserJointAddAngularRow(m_joint, -GetPitch(), &matrix0.m_front[0]);
		NewtonUserJointSetRowSpringDamperAcceleration(m_joint, m_springDamperRelaxation, m_spring, m_damper);
	} else {
		if (m_limitsOn) {
			if (m_friction != 0.0f) {
				SubmitConstraintsFrictionAndLimit(matrix0, matrix1, timestep);
			} else {
				SubmitConstraintsLimitsOnly(matrix0, matrix1, timestep);
			}
		} else {
			if (m_friction != 0.0f) {
				SubmitConstraintsFrictionOnly(matrix0, matrix1, timestep);
			} 
		}
	}
}

void dCustomHinge::SubmitConstraintsLimitsOnly(const dMatrix& matrix0, const dMatrix& matrix1, dFloat timestep)
{
	dFloat angle = GetPitch();
	if (angle < m_pitch.m_minAngle) {
		dFloat relAngle = m_pitch.m_minAngle - angle;
		NewtonUserJointAddAngularRow(m_joint, relAngle, &matrix0.m_front[0]);
		NewtonUserJointSetRowStiffness(m_joint, m_stiffness);
		NewtonUserJointSetRowMinimumFriction(m_joint, 0.0f);
	} else if (angle > m_pitch.m_maxAngle) {
		dFloat relAngle = m_pitch.m_maxAngle - angle;
		NewtonUserJointAddAngularRow(m_joint, relAngle, &matrix0.m_front[0]);
		NewtonUserJointSetRowStiffness(m_joint, m_stiffness);
		NewtonUserJointSetRowMaximumFriction(m_joint, 0.0f);
	}
}

void dCustomHinge::SubmitConstraintsFrictionOnly(const dMatrix& matrix0, const dMatrix& matrix1, dFloat timestep)
{
	dFloat alpha = m_jointOmega / timestep;
	NewtonUserJointAddAngularRow(m_joint, 0, &matrix1.m_front[0]);
	NewtonUserJointSetRowAcceleration(m_joint, -alpha);
	NewtonUserJointSetRowStiffness(m_joint, m_stiffness);
	NewtonUserJointSetRowMinimumFriction(m_joint, -m_friction);
	NewtonUserJointSetRowMaximumFriction(m_joint, m_friction);
}

void dCustomHinge::SubmitConstraintsFrictionAndLimit(const dMatrix& matrix0, const dMatrix& matrix1, dFloat timestep)
{

	dFloat angle = GetPitch();
	if (angle < m_pitch.m_minAngle) {
		dFloat relAngle = m_pitch.m_minAngle - angle;
		NewtonUserJointAddAngularRow(m_joint, relAngle, &matrix0.m_front[0]);
		NewtonUserJointSetRowStiffness(m_joint, m_stiffness);
		NewtonUserJointSetRowMinimumFriction(m_joint, 0.0f);
	} else if (angle > m_pitch.m_maxAngle) {
		dFloat relAngle = m_pitch.m_maxAngle - angle;
		NewtonUserJointAddAngularRow(m_joint, relAngle, &matrix0.m_front[0]);
		NewtonUserJointSetRowStiffness(m_joint, m_stiffness);
		NewtonUserJointSetRowMaximumFriction(m_joint, 0.0f);
	} else {
		// friction but not limits
		dFloat alpha = m_jointOmega / timestep;
		NewtonUserJointAddAngularRow(m_joint, 0, &matrix1.m_front[0]);
		NewtonUserJointSetRowAcceleration(m_joint, -alpha);
		NewtonUserJointSetRowStiffness(m_joint, m_stiffness);
		NewtonUserJointSetRowMinimumFriction(m_joint, -m_friction);
		NewtonUserJointSetRowMaximumFriction(m_joint, m_friction);
	}
}
