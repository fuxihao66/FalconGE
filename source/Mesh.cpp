#include "Mesh.h"


Mesh::Mesh() {
	numVert = 0;
	numFace = 0;
}

Mesh::Mesh(std::shared_ptr<TriangleGeo> tri) {
	triangleList.push_back(tri);
	numFace = 1;
}

Mesh::Mesh(const std::string& path) {

	// read
	happly::PLYData plyIn(path);
	std::vector<std::array<double, 3>> vPos = plyIn.getVertexPositions();
	std::vector<std::vector<size_t>> fInd = plyIn.getFaceIndices<size_t>();

	numVert = vPos.size();
	numFace = fInd.size();
	//numFace = 100;

	triangleList.resize(numFace);

	// TODO: 内存占用会增大，需要修改bbox的接口
	for (size_t faceIndex = 0; faceIndex < numFace; faceIndex++) {
		triangleList[faceIndex] = std::make_shared<TriangleGeo>(vPos[fInd[faceIndex][0]], vPos[fInd[faceIndex][1]], vPos[fInd[faceIndex][2]]);
	}
	
}

void Mesh::Rotate(double xAngle, double yAngle, double zAngle) {
	for (auto it = triangleList.begin(); it != triangleList.end(); it++) {
		(*it)->Rotate(xAngle, yAngle, zAngle);
	}
}
void Mesh::Translate(double xDistance, double yDistance, double zDistance) {
	for (auto it = triangleList.begin(); it != triangleList.end(); it++) {
		(*it)->Translate(xDistance, yDistance, zDistance);
	}
}

/*
double Mesh::TriangleInsec(const Ray& r, size_t faceId) const {
	const Point& norm = normals[faceId];
	if (norm.dot(r.dir) == 0)
		return -1;
	const Point& a = vertices[faceIndices[faceId].x];
	const Point& b = vertices[faceIndices[faceId].y];
	const Point& c = vertices[faceIndices[faceId].z];

	double t = (a - r.ori).dot(norm) / r.dir.dot(norm);

	if (t <= 0)
		return t;

	if (PointInsideTriangle(r.ori + t * r.dir, a, b, c))
		return t;
	else
		return -1;

}

*/
//const Vec3<double> Mesh::GetNormal(const Vec3<double>& pos) const {
//
//}
bool Mesh::Intersect(const Ray& r, double& t, Vec3<double>& norm) const {
	//double local_t = 0xFFFFF;
	//size_t selectedFace = -1;


	//for (size_t i = 0; i < numFace; i++) {
	//	double temp_t = TriangleInsec(r, i);

	//	if (temp_t < EPSILON)  // 在发出点相交或不相交
	//		continue;

	//	if (local_t > temp_t) {
	//		selectedFace = i;
	//		local_t = temp_t;
	//	}
	//}

	//if (selectedFace == -1)
	//	return false;
	//else {
	//	t = local_t;
	//	norm = normals[selectedFace];
	//}
	return false;
}
//bool Mesh::Contain(const Vec3<double>& point) const {
//
//}

double Mesh::SurfaceArea() const {
	return 0.0;
}
Point Mesh::RandomSampleOnSurf() const {
	return Point();
}

class GridLookingMap {
private:
	size_t _xMaxIndex;
	size_t _yMaxIndex;
	size_t _zMaxIndex;
	std::vector<std::vector<std::shared_ptr<BoundingBox>>> _map;
	size_t xyz2LocalIndex(size_t xIndex, size_t yIndex, size_t zIndex) {
		return zIndex * _xMaxIndex * _yMaxIndex + yIndex * _xMaxIndex + xIndex;
	}
public:
	GridLookingMap(size_t _x, size_t _y, size_t _z) {
		_xMaxIndex = _x;
		_yMaxIndex = _y;
		_zMaxIndex = _z;
		_map.resize(_xMaxIndex * _yMaxIndex * _zMaxIndex);
		/*std::cout << "size is " << _map.size() << std::endl;
		std::cout << _xMaxIndex << " " << _yMaxIndex << " " << _zMaxIndex << std::endl;*/
	}
	void AddBoxAtIndex(size_t xIndex, size_t yIndex, size_t zIndex, std::shared_ptr<BoundingBox> ptr) {
		_map[xyz2LocalIndex(xIndex, yIndex, zIndex)].push_back(ptr);
	}
	std::vector<std::shared_ptr<BoundingBox>>& GetBoxInIndex(size_t xIndex, size_t yIndex, size_t zIndex) {
		//std::cout << xIndex << " " << yIndex << " " << zIndex << std::endl;
		return _map[xyz2LocalIndex(xIndex, yIndex, zIndex)];
	}
};
std::shared_ptr<BoundingBox> Mesh::GenBox() const {

	
	auto bigBox = std::make_shared<BoundingBox>();
	std::vector<std::shared_ptr<BoundingBox>> bbList(numFace);
	for (size_t faceIndex = 0; faceIndex < numFace; faceIndex++) {
		bbList[faceIndex] = triangleList[faceIndex]->GenBox();
		bigBox->Update(bbList[faceIndex]);
	}

	std::vector<std::shared_ptr<BoundingBox>> current;
	current.push_back(bigBox);
	// TODO: adaptive细分

	size_t maxLayer = 7;

	for (size_t layer = 0; layer < maxLayer; layer++) { // 最多10层嵌套
		std::vector<std::shared_ptr<BoundingBox>> temp;
		for (size_t boxIndex = 0; boxIndex < current.size(); boxIndex++) {
			// 切成8个
			double xMid = (current[boxIndex]->xMin + current[boxIndex]->xMax) / 2.0;
			double yMid = (current[boxIndex]->yMin + current[boxIndex]->yMax) / 2.0;
			double zMid = (current[boxIndex]->zMin + current[boxIndex]->zMax) / 2.0;

			auto b1 = std::make_shared<BoundingBox>(current[boxIndex]->xMin, xMid, current[boxIndex]->yMin, yMid, current[boxIndex]->zMin, zMid);
			auto b2 = std::make_shared<BoundingBox>(xMid, current[boxIndex]->xMax, current[boxIndex]->yMin, yMid, current[boxIndex]->zMin, zMid);
			auto b3 = std::make_shared<BoundingBox>(current[boxIndex]->xMin, xMid, yMid, current[boxIndex]->yMax, current[boxIndex]->zMin, zMid);
			auto b4 = std::make_shared<BoundingBox>(current[boxIndex]->xMin, xMid, current[boxIndex]->yMin, yMid, zMid, current[boxIndex]->zMax);
			auto b5 = std::make_shared<BoundingBox>(xMid, current[boxIndex]->xMax, yMid, current[boxIndex]->yMax, current[boxIndex]->zMin, zMid);
			auto b6 = std::make_shared<BoundingBox>(xMid, current[boxIndex]->xMax, current[boxIndex]->yMin, yMid, zMid, current[boxIndex]->zMax);
			auto b7 = std::make_shared<BoundingBox>(current[boxIndex]->xMin, xMid, yMid, current[boxIndex]->yMax, zMid, current[boxIndex]->zMax);
			auto b8 = std::make_shared<BoundingBox>(xMid, current[boxIndex]->xMax, yMid, current[boxIndex]->yMax, zMid, current[boxIndex]->zMax);


			temp.push_back(b1); current[boxIndex]->AddChildren(b1);
			temp.push_back(b2); current[boxIndex]->AddChildren(b2);
			temp.push_back(b3); current[boxIndex]->AddChildren(b3);
			temp.push_back(b4); current[boxIndex]->AddChildren(b4);
			temp.push_back(b5); current[boxIndex]->AddChildren(b5);
			temp.push_back(b6); current[boxIndex]->AddChildren(b6);
			temp.push_back(b7); current[boxIndex]->AddChildren(b7);
			temp.push_back(b8); current[boxIndex]->AddChildren(b8);
			
			
		}
		current = std::move(temp);
	}

	std::cout << "built" << std::endl;

	// 对于每个最小bb 生成一张查找表
	size_t range = pow(2, maxLayer);
	double boxXWidth = (bigBox->xMax - bigBox->xMin) / range;
	double boxYWidth = (bigBox->yMax - bigBox->yMin) / range;
	double boxZWidth = (bigBox->zMax - bigBox->zMin) / range;
	GridLookingMap glmp(range, range, range);

	for (size_t faceIndex = 0; faceIndex < numFace; faceIndex++) {
		size_t xIndexStart = floor((bbList[faceIndex]->xMin - bigBox->xMin) / boxXWidth);
		size_t xIndexEnd = floor((bbList[faceIndex]->xMax - bigBox->xMin) / boxXWidth);
		size_t yIndexStart = floor((bbList[faceIndex]->yMin - bigBox->yMin) / boxYWidth);
		size_t yIndexEnd = floor((bbList[faceIndex]->yMax - bigBox->yMin) / boxYWidth);
		size_t zIndexStart = floor((bbList[faceIndex]->zMin - bigBox->zMin) / boxZWidth);
		size_t zIndexEnd = floor((bbList[faceIndex]->zMax - bigBox->zMin) / boxZWidth);
		for (size_t xIndex = xIndexStart; xIndex <= std::min(xIndexEnd, range-1); xIndex++)
			for (size_t yIndex = yIndexStart; yIndex <= std::min(yIndexEnd, range-1); yIndex++)
				for (size_t zIndex = zIndexStart; zIndex <= std::min(zIndexEnd, range-1); zIndex++) {
					
					glmp.AddBoxAtIndex(xIndex, yIndex, zIndex, bbList[faceIndex]);
				}
	}

	for (size_t boxIndex = 0; boxIndex < current.size(); boxIndex++) {
		size_t bXIndex = floor(((current[boxIndex]->xMax + current[boxIndex]->xMin) / 2.0 - bigBox->xMin) / boxXWidth);
		size_t bYIndex = floor(((current[boxIndex]->yMax + current[boxIndex]->yMin) / 2.0 - bigBox->yMin) / boxYWidth);
		size_t bZIndex = floor(((current[boxIndex]->zMax + current[boxIndex]->zMin) / 2.0 - bigBox->zMin) / boxZWidth);
		auto childs = glmp.GetBoxInIndex(bXIndex, bYIndex, bZIndex);
		for (auto child : childs) {
			current[boxIndex]->AddChildren(child);
		}
	}


	//for (size_t boxIndex = 0; boxIndex < current.size(); boxIndex++) {
	//	//parentOfLeaf[boxIndex] = std::make_shared<BoundingBox>();
	//	for (size_t faceIndex = 0; faceIndex < numFace; faceIndex++) {
	//		if (current[boxIndex]->IntersectWithBox(bbList[faceIndex])) {
	//			current[boxIndex]->AddChildren(bbList[faceIndex]);
	//			// 不用update
	//		}
	//	}
	//}

	return bigBox;

	/*while (bbList.size() > 1) {
		std::vector<std::shared_ptr<BoundingBox>> newBBList(ceil(bbList.size()/10.0));
		for (size_t index = 0; index < newBBList.size(); index++) {
			newBBList[index] = std::make_shared<BoundingBox>();
			for (size_t count = index * 10; count < std::min(index * 10 + 10, bbList.size()); count++) {
				newBBList[index]->Update(bbList[count]);
				newBBList[index]->AddChildren(bbList[count]);
			}
		}

		bbList = std::move(newBBList);
	}
	

	bbList[0]->PrintBox();

	return bbList[0];*/

}

