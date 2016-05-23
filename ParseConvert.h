#pragma once
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <map>
#include <memory>
#include <algorithm>
#include <regex>
#include <iostream>//delete

class ParseConvert
{
private:
	std::wstring filename, mtlName;
	struct XMFLOAT3;//delete
	struct Vertex;
	struct Materials;
	std::vector<std::wstring> useMtl;
	std::map<std::wstring, std::unique_ptr<Materials>> materials;
	std::vector<XMFLOAT3> verteñis, normals, textÑoords;
	std::vector<Vertex> vertex;
	std::vector<int> meshStartPosition, indecis;
public:
	ParseConvert();
	bool LoadOBJ(const std::wstring &filename);
	bool LoadMTL();
	void Out();//delete
	~ParseConvert();
};

