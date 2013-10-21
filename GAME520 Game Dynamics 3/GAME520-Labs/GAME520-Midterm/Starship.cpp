#include "Starship.h"
#include <iostream>

Starship::Starship(SolidSphere* CommandCenter, SolidCylinder* MainBody, SolidCylinder* WarpDriveL, SolidCylinder* WarpDriveR)
{
	this->CommandCenter = CommandCenter;
	this->MainBody = MainBody;
	this->WarpDriveL = WarpDriveL;
	this->WarpDriveR = WarpDriveR;
}

void printMatrix(btMatrix3x3* matrix)
{
	for (int i = 0; i < 3; i++)
	{
		std::cout << '[';
		btVector3 row = matrix->getRow(i);
		std::cout << row.getX() << ", ";
		std::cout << row.getY() << ", ";
		std::cout << row.getZ();
		std::cout << "]\n";

	}
}


Starship::~Starship(void)
{
	delete CommandCenter;
	delete MainBody;
	delete WarpDriveL;
	delete WarpDriveR;
}

btMatrix3x3* Starship::CalculateInertiaTensor()
{
	btMatrix3x3* inertiaTensor = new btMatrix3x3();
	inertiaTensor->setIdentity();

	//Assign Shapes to a Array
	SimpleShape* Shapes [4] = {CommandCenter, MainBody, WarpDriveL, WarpDriveR};

	btVector3 CenterOfMass = btVector3(0,0,0);
	float totalMass = 0;
	for (int i = 0; i < 4; i++)
	{
		totalMass += Shapes[i]->mMass;
		CenterOfMass += *(Shapes[i]->CoMOffset) * (Shapes[i]->mMass);
	}

	CenterOfMass /= totalMass;

	std::cout << "Center of Mass- x: " << CenterOfMass.getX() << ", y: " << CenterOfMass.getY() << ", z: " << CenterOfMass.getZ() << "\n\n\n";

	for (int i = 0; i < 4; i++)
	{
		btVector3 CenterofMassDiff = *(Shapes[i]->CoMOffset) - CenterOfMass;
		float dx = CenterofMassDiff.getX();
		float dy = CenterofMassDiff.getY();
		float dz = CenterofMassDiff.getZ();

		std::cout << Shapes[i]->mName << " CoM Diff  x: " << dx << " y: " << dy << " z: " << dz << '\n'; 

		btMatrix3x3 compoundInertiaTensor;

		btMatrix3x3 shapeInertiaTensor = *(Shapes[i]->CalculateLocalInertia());
		
		std::cout << '\n';
		std::cout << Shapes[i]->mName <<  " LOCAL INERTIA MATRIX" << '\n';
		printMatrix(&shapeInertiaTensor);
		std::cout << '\n';

		float Ixx = shapeInertiaTensor.getRow(0).getX() + (Shapes[i]->mMass * ((dy * dy) + (dz * dz)));
		float Iyy = shapeInertiaTensor.getRow(1).getY() + (Shapes[i]->mMass * ((dx * dx) + (dz * dz)));
		float Izz = shapeInertiaTensor.getRow(2).getZ() + (Shapes[i]->mMass * ((dx * dx) + (dy * dy)));
		float Ixy = shapeInertiaTensor.getRow(0).getY() + (Shapes[i]->mMass * dx * dy);
		float Ixz = shapeInertiaTensor.getRow(0).getZ() + (Shapes[i]->mMass * dx * dz);
		float Iyz = shapeInertiaTensor.getRow(1).getZ() + (Shapes[i]->mMass * dz * dy);

		
		
		compoundInertiaTensor = btMatrix3x3(
							  Ixx, -Ixy, -Ixz,
							 -Ixy,  Iyy, -Iyz,
							 -Ixz, -Iyz,  Izz);

		std::cout << Shapes[i]->mName << " COMPOUND INERTIA MATRIX" << "\n";
		printMatrix(&compoundInertiaTensor);
		std::cout << "\n\n";
		*inertiaTensor += compoundInertiaTensor;


	}

	return inertiaTensor;
}

float Starship::CalculateMass(void)
{
	float total = 0.0f;
	total += CommandCenter->mMass;
	total += MainBody->mMass;
	total += WarpDriveL->mMass;
	total += WarpDriveR->mMass;

	return total;
}