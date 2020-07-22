#pragma once
#include <vector>
#include "happly.h"


class Mesh: public Geometry
{
private:
	UINT numVert;
	UINT numFace;
	//std::vector<Vec3<size_t>> faceIndices;
	//std::vector<Point> vertices;
	//std::vector<Point> normals;// norm of every face

	std::vector<std::shared_ptr<TriangleGeo>> triangleList;

	//double TriangleInsec(const Ray&,size_t faceId) const;
public:
	Mesh();
	Mesh(std::shared_ptr<TriangleGeo> tri);
	Mesh(const std::string& path);
	virtual bool Intersect(const Ray&, double&, Vec3<double>&) const;

	//virtual const Vec3<double> GetNormal(const Vec3<double>& pos) const;
	//virtual bool Contain(const Vec3<double>& point) const;
	virtual double SurfaceArea() const;
	virtual Point RandomSampleOnSurf() const;

	virtual void Rotate(double xAngle, double yAngle, double zAngle);
	virtual void Translate(double xDistance, double yDistance, double zDistance);

	virtual std::shared_ptr<BoundingBox> GenBox() const;
};

