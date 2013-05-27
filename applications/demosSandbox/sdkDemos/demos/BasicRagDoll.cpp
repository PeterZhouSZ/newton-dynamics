/* Copyright (c) <2009> <Newton Game Dynamics>
* 
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
* 
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely
*/

#include <toolbox_stdafx.h>
#include "SkyBox.h"
#include "DemoMesh.h"
#include "DemoCamera.h"
#include "NewtonDemos.h"
#include "PhysicsUtils.h"
#include "TargaToOpenGl.h"
#include "CustomRagDoll.h"
#include "DemoEntityManager.h"
#include "toolBox/DebugDisplay.h"
#include "CustomSkeletonTransformManager.h"



struct RAGDOLL_BONE_DEFINITION
{
	char m_boneName[32];
	char m_shapeType[32];

	dFloat m_shapePitch;
	dFloat m_shapeYaw;
	dFloat m_shapeRoll;

	dFloat m_shape_x;
	dFloat m_shape_y;
	dFloat m_shape_z;

	dFloat m_radio;
	dFloat m_height;

	dFloat m_mass;
	dFloat m_coneAngle;
	dFloat m_minTwistAngle;
	dFloat m_maxTwistAngle;

	dFloat m_pitchLimit;
	dFloat m_yawLimit;
	dFloat m_rollLimit;

	int m_collideWithNonImmidiateBodies;
};


static RAGDOLL_BONE_DEFINITION skeletonRagDoll[] =
{
	{"Bip01_Pelvis",	 "capsule", 0.0f, 0.0f, -90.0f, 0.0f, 0.0f, 0.01f, 0.07f, 0.16f, 20.0f,  0.0f,  -0.0f,    0.0f, 0.0f,  0.0f,  0.0f, 1}, 

	{"Bip01_Spine",		 "capsule", 0.0f, 0.0f, -90.0f, 0.0f, 0.0f, 0.06f, 0.07f, 0.14f, 20.0f,  0.0f,  -0.0f,    0.0f, 0.0f,  0.0f,  0.0f, 1}, 
	{"Bip01_Spine1",	 "capsule", 0.0f, 0.0f, -90.0f, 0.0f, 0.0f, 0.06f, 0.07f, 0.12f, 20.0f,  0.0f,  -0.0f,    0.0f, 0.0f,  0.0f,  0.0f, 1}, 
	{"Bip01_Spine2",	 "capsule", 0.0f, 0.0f, -90.0f, 0.0f, 0.0f, 0.06f, 0.07f, 0.08f, 20.0f,  0.0f,  -0.0f,    0.0f, 0.0f,  0.0f,  0.0f, 1}, 

//	{"Bip01_Neck",		 "capsule", 0.0f, 90.0f, 0.0f, 0.0f, 0.0f, 0.05f, 0.03f, 0.04f, 20.0f,  0.0f,  -0.0f,    0.0f, 0.0f,  0.0f,  0.0f, 1}, 
	{"Bip01_Head",		 "sphere", 0.0f, 90.0f, 0.0f, 0.0f, 0.0f, 0.09f, 0.09f, 0.0f, 20.0f,  0.0f,  -0.0f,    0.0f, 0.0f,  0.0f,  0.0f, 1}, 

	{"Bip01_L_UpperArm", "capsule", 0.0f, 90.0f,  0.0f, 0.0f, 0.0f, 0.12f, 0.03f, 0.23f, 10.0f,  0.0f,  -0.0f,    0.0f, 0.0f,  0.0f,  0.0f, 1}, 
//	{"Bip01_L_Forearm",  "capsule", 0.0f, 90.0f,  0.0f, 0.0f, 0.0f, 0.12f, 0.03f, 0.23f, 10.0f,  0.0f,  -0.0f,    0.0f, 0.0f,  0.0f,  0.0f, 1}, 
	{"Bip01_L_Hand",  "convexhull", 0.0f, 00.0f,  0.0f, 0.0f, 0.0f, 0.00f, 0.00f, 0.00f, 10.0f,  0.0f,  -0.0f,    0.0f, 0.0f,  0.0f,  0.0f, 1}, 

	{"Bip01_R_UpperArm", "capsule", 0.0f, 90.0f,  0.0f, 0.0f, 0.0f, 0.12f, 0.03f, 0.23f, 10.0f,  0.0f,  -0.0f,    0.0f, 0.0f,  0.0f,  0.0f, 1}, 
//	{"Bip01_R_Forearm",  "capsule", 0.0f, 90.0f,  0.0f, 0.0f, 0.0f, 0.12f, 0.03f, 0.23f, 10.0f,  0.0f,  -0.0f,    0.0f, 0.0f,  0.0f,  0.0f, 1}, 
	{"Bip01_R_Hand",  "convexhull", 0.0f, 00.0f,  0.0f, 0.0f, 0.0f, 0.00f, 0.00f, 0.00f, 10.0f,  0.0f,  -0.0f,    0.0f, 0.0f,  0.0f,  0.0f, 1}, 
	
//	{"Bip01_L_Thigh", "capsule", 0.0f, 90.0f,  0.0f, 0.0f, 0.0f, 0.19f, 0.05f, 0.34f, 10.0f,  0.0f,  -0.0f,    0.0f, 0.0f,  0.0f,  0.0f, 1}, 
//	{"Bip01_L_Calf", "capsule", 0.0f, 90.0f,  0.0f, 0.0f, 0.0f, 0.19f, 0.05f, 0.34f, 10.0f,  0.0f,  -0.0f,    0.0f, 0.0f,  0.0f,  0.0f, 1}, 
//	{"Bip01_L_Foot",  "convexhull", 0.0f, 00.0f,  0.0f, 0.0f, 0.0f, 0.00f, 0.00f, 0.00f, 10.0f,  0.0f,  -0.0f,    0.0f, 0.0f,  0.0f,  0.0f, 1}, 
	
//	{"Bip01_R_Thigh", "capsule", 0.0f, 90.0f,  0.0f, 0.0f, 0.0f, 0.19f, 0.05f, 0.34f, 10.0f,  0.0f,  -0.0f,    0.0f, 0.0f,  0.0f,  0.0f, 1}, 
//	{"Bip01_R_Calf", "capsule", 0.0f, 90.0f,  0.0f, 0.0f, 0.0f, 0.19f, 0.05f, 0.34f, 10.0f,  0.0f,  -0.0f,    0.0f, 0.0f,  0.0f,  0.0f, 1}, 
//	{"Bip01_R_Foot",  "convexhull", 0.0f, 00.0f,  0.0f, 0.0f, 0.0f, 0.00f, 0.00f, 0.00f, 10.0f,  0.0f,  -0.0f,    0.0f, 0.0f,  0.0f,  0.0f, 1}, 
	
	
};



class RagDollManager: public CustomSkeletonTransformManager
{
	public: 
	RagDollManager (DemoEntityManager* const scene)
		:CustomSkeletonTransformManager (scene->GetNewton())
	{
	}

	void GetDimentions(DemoEntity* const bodyPart, dVector& origin, dVector& size) const
	{	
		DemoMesh* const mesh = bodyPart->GetMesh();

		dFloat* const array = mesh->m_vertex;
		dVector pmin( 1.0e20f,  1.0e20f,  1.0e20f, 0.0f);
		dVector pmax(-1.0e20f, -1.0e20f, -1.0e20f, 0.0f);

		for (int i = 0; i < mesh->m_vertexCount; i ++) {
			dFloat x = array[i * 3 + 0];
			dFloat y = array[i * 3 + 1];
			dFloat z = array[i * 3 + 2];

			pmin.m_x = x < pmin.m_x ? x : pmin.m_x;
			pmin.m_y = y < pmin.m_y ? y : pmin.m_y;
			pmin.m_z = z < pmin.m_z ? z : pmin.m_z;
									  
			pmax.m_x = x > pmax.m_x ? x : pmax.m_x;
			pmax.m_y = y > pmax.m_y ? y : pmax.m_y;
			pmax.m_z = z > pmax.m_z ? z : pmax.m_z;
		}

		size = (pmax - pmin).Scale (0.5f);
		origin = (pmax + pmin).Scale (0.5f);
		origin.m_w = 1.0f;
	}


	NewtonCollision* MakeSphere(DemoEntity* const bodyPart, const RAGDOLL_BONE_DEFINITION& definition) const
	{
		dVector size;
		dVector origin;
		dMatrix matrix (GetIdentityMatrix());

		matrix.m_posit.m_x = definition.m_shape_x;
		matrix.m_posit.m_y = definition.m_shape_y;
		matrix.m_posit.m_z = definition.m_shape_z;
		return NewtonCreateSphere(GetWorld(), definition.m_radio, 0, &matrix[0][0]);

	}

	NewtonCollision* MakeCapsule(DemoEntity* const bodyPart, const RAGDOLL_BONE_DEFINITION& definition) const
	{
		dVector size;
		dVector origin;
		dMatrix matrix (dPitchMatrix(definition.m_shapePitch * 3.141592f / 180.0f) * dYawMatrix(definition.m_shapeYaw * 3.141592f / 180.0f) * dRollMatrix(definition.m_shapeRoll * 3.141592f / 180.0f));

		matrix.m_posit.m_x = definition.m_shape_x;
		matrix.m_posit.m_y = definition.m_shape_y;
		matrix.m_posit.m_z = definition.m_shape_z;
		return NewtonCreateCapsule (GetWorld(), definition.m_radio, definition.m_height, 0, &matrix[0][0]);
	}

	NewtonCollision* MakeBox(DemoEntity* const bodyPart) const
	{
		dAssert (0);
//		dVector size;
//		dVector origin;
//		dMatrix matrix (GetIdentityMatrix());
//		GetDimentions(bone, matrix.m_posit, size);
//		return NewtonCreateBox (nWorld, 2.0f * size.m_x, 2.0f * size.m_y, 2.0f * size.m_z, 0, &matrix[0][0]);
		return NULL;
	}

/*
	void ApplyBoneMatrix (int boneIndex, void* userData, const dMatrix& matrix) const
	{
		if (boneIndex == 0) {
			dBone* boneNode = (dBone*) userData;
			dMatrix rootMatrix (matrix);
			while (boneNode->GetParent()) {
				rootMatrix = boneNode->GetMatrix().Inverse() * rootMatrix;
				boneNode = boneNode->GetParent();
			}
			m_model->SetMatrix(rootMatrix);
		} else {

			dBone* boneNode = (dBone*) userData;
			// check if the parent of this bone is also a bone, body1 is the parentBone
			dBone* parentBone = (dBone*) NewtonBodyGetUserData (GetParentBone (boneIndex));
			if (boneIndex && (boneNode->GetParent() != parentBone)) {
				// this is not and immediate bone calculate the offset matrix
				dBone* parent;
				parent = boneNode->GetParent();
				dMatrix offset (parent->GetMatrix());
				for (parent = parent->GetParent(); parent != parentBone; parent = parent->GetParent()) {
					offset = offset * parent->GetMatrix();
				}

				dMatrix localMatrix (matrix * offset.Inverse());
				boneNode->SetMatrix (localMatrix);

			} else {
				boneNode->SetMatrix (matrix);
			}
		}

	}
*/

	NewtonCollision* MakeConvexHull(DemoEntity* const bodyPart) const
	{
		dVector points[1024 * 16];

		DemoMesh* const mesh = bodyPart->GetMesh();
		dAssert (mesh->m_vertexCount && (mesh->m_vertexCount < sizeof (points)/ sizeof (points[0])));

		// go over the vertex array and find and collect all vertices's weighted by this bone.
		dFloat* const array = mesh->m_vertex;
		for (int i = 0; i < mesh->m_vertexCount; i ++) {
			points[i].m_x = array[i * 3 + 0];
			points[i].m_y = array[i * 3 + 1];
			points[i].m_z = array[i * 3 + 2];
		}

		return NewtonCreateConvexHull (GetWorld(), mesh->m_vertexCount, &points[0].m_x, sizeof (dVector), 1.0e-3f, 0, NULL);
	}


	NewtonBody* CreateRagDollBodyPart (DemoEntity* const bodyPart, const RAGDOLL_BONE_DEFINITION& definition) 
	{
		NewtonCollision* shape = NULL;
		if (!strcmp (definition.m_shapeType, "sphere")) {
			shape = MakeSphere (bodyPart, definition);
		} else if (!strcmp (definition.m_shapeType, "capsule")) {
			shape = MakeCapsule(bodyPart, definition);
		} else if (!strcmp (definition.m_shapeType, "box")) {
			shape = MakeBox (bodyPart);
		} else {
			shape = MakeConvexHull(bodyPart);
		}

		// calculate the bone matrix
		dMatrix matrix (bodyPart->CalculateGlobalMatrix());

		// find the index of the bone parent
//		int parentIndeIndex = -1;
//		for (dBone* parentNode = bone->GetParent(); parentNode && (parentIndeIndex == -1); parentNode = parentNode->GetParent()) {
//			for (int i = 0; i <  GetBoneCount(); i ++) {
//				if (parentNode == NewtonBodyGetUserData (GetBone (i))) {
//					parentIndeIndex = i;
//					break;
//				}
//			}
//		} 
//		dMatrix pinAndPivot (dPitchMatrix (definition.m_pitch * 3.141592f / 180.0f) * dYawMatrix (definition.m_yaw * 3.141592f / 180.0f) * dRollMatrix (definition.m_roll * 3.141592f / 180.0f));
//		pinAndPivot = pinAndPivot * rootMatrix;
//		int boneIndex = AddBone (nWorld, parentIndeIndex, (void*) bone, rootMatrix, definition.m_mass, pinAndPivot, shape);
//		SetCollisionState (boneIndex, definition.m_collideWithNonImmidiateBodies);
//		SetBoneConeLimits (boneIndex, definition.m_coneAngle * 3.141592f / 180.0f);
//		SetBoneTwistLimits (boneIndex, definition.m_minTwistAngle * 3.141592f / 180.0f, definition.m_maxTwistAngle * 3.141592f / 180.0f);

		NewtonWorld* const world = GetWorld();

		// create the rigid body that will make this bone
		NewtonBody* const bone = NewtonCreateDynamicBody (world, shape, &matrix[0][0]);

		// calculate the moment of inertia and the relative center of mass of the solid
		NewtonBodySetMassProperties (bone, definition.m_mass, shape);

		// save the user data with the bone body (usually the visual geometry)
		NewtonBodySetUserData(bone, bodyPart);

		// set the bod part force and torque call back to the gravity force, skip the transform callback
//		NewtonBodySetForceAndTorqueCallback (bone, PhysicsApplyGravityForce);


		// destroy the collision helper shape 
		NewtonDestroyCollision (shape);
		return bone;
	}


	void CreateRagDoll (const dMatrix& location, const DemoEntity* const model, RAGDOLL_BONE_DEFINITION* const definition, int defintionCount)
	{
		NewtonWorld* const world = GetWorld(); 
		DemoEntityManager* const scene = (DemoEntityManager*) NewtonWorldGetUserData(world);

		// make a clone of the mesh 
		DemoEntity* const ragDollEntity = (DemoEntity*) model->CreateClone();
		scene->Append(ragDollEntity);

		// build the ragdoll with rigid bodies connected by joints
		// create a transform controller
		CustomSkeletonTransformController* const controller = CreateTransformController (ragDollEntity);

		// add the root bone
		DemoEntity* const rootEntity = (DemoEntity*) ragDollEntity->Find (definition[0].m_boneName);
		NewtonBody* const rootBone = CreateRagDollBodyPart (rootEntity, definition[0]);
		int boneIndex = controller->AddBone (rootBone);

		int stackIndex;
		int parentBoneIndex[32];
		DemoEntity* childEntities[32];
		stackIndex = 0;
		for (DemoEntity* child = rootEntity->GetChild(); child; child = child->GetSibling()) {
			parentBoneIndex[stackIndex] = boneIndex;
			childEntities[stackIndex] = child;
			stackIndex ++;
		}

		// walk model hierarchic adding all children designed as rigid body bones. 
		while (stackIndex) {
			stackIndex --;
			DemoEntity* const entity = childEntities[stackIndex];
			int parentIndex = parentBoneIndex[stackIndex];

			const char* const name = entity->GetName().GetStr();
			for (int i = 0; i < defintionCount; i ++) {
				if (!strcmp (definition[i].m_boneName, name)) {
					NewtonBody* const bone = CreateRagDollBodyPart (entity, definition[i]);
					parentIndex = controller->AddBone (bone, parentIndex);
					break;
				}
			}

			for (DemoEntity* child = entity->GetChild(); child; child = child->GetSibling()) {
				parentBoneIndex[stackIndex] = boneIndex;
				childEntities[stackIndex] = child;
				stackIndex ++;
			}
		}

		// set the collision mask
		// note this container work best with a material call back for setting bit field 




//		matrix.m_posit = FindFloor (scene->GetNewton(), point, 10.0f);

	}
};


void DescreteRagDoll (DemoEntityManager* const scene)
{
	// load the sky box
	scene->CreateSkyBox();
	CreateLevelMesh (scene, "flatPlane.ngd", true);

	// load a skeleton mesh for using as a ragdoll manager
	DemoEntity ragDollModel(GetIdentityMatrix(), NULL);
	ragDollModel.LoadNGD_mesh ("skeleton.ngd", scene->GetNewton());

	dVector origin (-10.0f, 2.0f, 0.0f, 0.0f);

	//  create a skeletal transform controller for controlling ragdoll
	RagDollManager* const manager = new RagDollManager (scene);

	dMatrix matrix (GetIdentityMatrix());

	for (int x = 0; x < 1; x ++) {
		for (int z = 0; z < 1; z ++) {
			matrix.m_posit = origin + dVector (x * 3.0f + 5.0f, 0.0f, z * 3.0f, 1.0f);
			manager->CreateRagDoll (matrix, &ragDollModel, skeletonRagDoll, sizeof (skeletonRagDoll) / sizeof (skeletonRagDoll[0]));
		}
	}
	
	dQuaternion rot;
	scene->SetCameraMatrix(rot, origin);
}



