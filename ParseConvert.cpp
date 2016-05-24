#include "ParseConvert.h"

struct ParseConvert::XMFLOAT3 {
	float x;
	float y;
	float z;
	XMFLOAT3() {}
	XMFLOAT3(float x, float y, float z) :x(x), y(y), z(z) {}
	bool operator==(const XMFLOAT3 &xmlfloat3) const {
		return (this->x == xmlfloat3.x) && (this->y == xmlfloat3.y) && (this->z == xmlfloat3.z);
	}
};

struct ParseConvert::Vertex {
	XMFLOAT3 pos;
	XMFLOAT3 textCoord;
	XMFLOAT3 normals;
	Vertex() {}
	Vertex(XMFLOAT3 &pos, XMFLOAT3 &normals, XMFLOAT3 &textCoord) :pos(pos), normals(normals), textCoord(textCoord) {}
	bool operator==(const Vertex &vertex)const {
		return (vertex.normals == this->normals) && (vertex.pos == this->pos) && (vertex.textCoord == this->textCoord);
	}
};

struct ParseConvert::Materials {
	bool hasTexture;
	bool hasNormMap;
	float range;
	float transparancy;
	std::wstring diffuseMap;
	std::wstring normalMap;
	XMFLOAT3 ambient;
	XMFLOAT3 diffuse;
	XMFLOAT3 default—olor;
	XMFLOAT3 transparencyFactor;
	Materials() {}
	Materials(float &range, float &transparancy, XMFLOAT3 &ambient, XMFLOAT3 &diffuse, XMFLOAT3 &default—olor, XMFLOAT3 &transparencyFactor):
		range(range),
		transparancy(transparancy),
		ambient(ambient),
		diffuse(diffuse),
		default—olor(default—olor),
		transparencyFactor(transparencyFactor),
		hasTexture(false),
		hasNormMap(false){}
};

void ParseConvert::SplitFacesOBJ(std::wstring &data, int *bufferArray) {
	auto first_position = data.find_first_of('/');
	auto second_position = data.find_last_of('/');
	bufferArray[0] = stoi(data.substr(0, first_position));
	if (first_position == std::wstring::npos) {
		bufferArray[1] = -1;
		bufferArray[2] = -1;
	}
	else if (second_position != first_position && second_position != (first_position + 1)) {
		bufferArray[1] = stoi(data.substr(first_position + 1, second_position - first_position - 1));
		bufferArray[2] = stoi(data.substr(second_position + 1, std::wstring::npos));
	}
	else if (second_position != first_position && second_position == (first_position + 1)) {
		bufferArray[1] = stoi(data.substr(second_position + 1, std::wstring::npos));
		bufferArray[2] = -1;
	}
	else {
		bufferArray[1] = -1;
		bufferArray[2] = stoi(data.substr(second_position + 1, std::wstring::npos));
	}
}

ParseConvert::ParseConvert()
{
}

bool ParseConvert::LoadOBJ(const std::wstring &filename) {
	std::wifstream input(filename);
	std::wstring buffer;
	std::wstringstream strstream;
	float x, y, z;
	int x_buf, y_buf, z_buf;
	if (input) {
		while (input) {
			input >> buffer;
			if (buffer == L"#") {
				getline(input, buffer);
			}
			else if (buffer == L"g") {//‰Ó‰ÂÎ‡Ú¸
				this->meshStartPosition.push_back(indecis.size());
			}
			else if (buffer == L"mtllib") { //check mtllib
				input >> buffer;
				this->mtlName = buffer;
			}
			else if (buffer == L"v") { //search vertecis
				input >> x >> y >> z;
				this->verteÒis.push_back(XMFLOAT3(x, y, z));
			}
			else if (buffer == L"vn") { // search normals
				input >> x >> y >> z;
				this->normals.push_back(XMFLOAT3(x, y, z));
			}
			else if (buffer == L"vt") { // search text coordinates
				input >> x >> y >> z;
				this->text—oords.push_back(XMFLOAT3(x, y, z));// ‰Ó‡·ÓÚ‡Ú¸ Ë Û·‡Ú¸ z
			}
			else if (buffer == L"usemtl") {
				input >> buffer;
				this->useMtl.push_back(buffer);
			}
			else if (buffer == L"f") { // calculate normals
				for (size_t i(0); i < 3; i++) { //read each Vertex for triangle
					input >> buffer;
					strstream.str(buffer);
					getline(strstream, buffer, L'/'); // vertecis
					x_buf = std::stoi(buffer);
					if (x_buf < 0) {
						x_buf *= (-1);
					}
					getline(strstream, buffer, L'/'); // texture coordinates
					y_buf = std::stoi(buffer);
					if (y_buf < 0) {
						y_buf *= (-1);
					}
					getline(strstream, buffer, L'/'); // normals 
					z_buf = std::stoi(buffer);
					if (z_buf < 0) {
						z_buf *= (-1);
					}
					;
					auto findResult = find(vertex.begin(), vertex.end(), Vertex (verteÒis[x_buf - 1], normals[z_buf - 1], text—oords[y_buf - 1]));
					if (findResult == vertex.end()) { //have not such vertex
						this->vertex.push_back(Vertex(verteÒis[x_buf - 1], normals[z_buf - 1], text—oords[y_buf - 1]));// add new triangle
						this->indecis.push_back(vertex.size() - 1);
					}
					else {//already have vertex
						this->indecis.push_back(findResult - vertex.begin());
					}
					strstream.clear();
					strstream.ignore(strstream.rdbuf()->in_avail());
				}
			}
		}
		if (!this->LoadMTL()) {
			std::cerr << "MTL wasn't loaded";
		}
		return true;
	}
	else {
		return false;
	}
}

bool ParseConvert::LoadMTL() {
	std::wifstream input(this->mtlName);
	std::wstring buffer;
	std::map <std::wstring, std::unique_ptr<Materials>>::iterator matIter(materials.end());
	if (input) {
		while (input) {
			input >> buffer;
			if (buffer == L"#") {
				getline(input, buffer);
			}
			else if (buffer == L"newmtl") {
				input >> buffer;
				this->materials.insert(std::pair<std::wstring, std::unique_ptr<Materials>>(buffer, std::make_unique<Materials>()));
				matIter = this->materials.find(buffer);
			}
			else if (buffer == L"Ns") {
				if (matIter != materials.end()) {
					input >> matIter->second->range;
				}
			}
			else if (buffer == L"Tr") {
				if (matIter != materials.end()) {
					input >> matIter->second->transparancy;
				}
			}
			else if (buffer == L"Tf") {
				if (matIter != materials.end()) {
					input >> matIter->second->transparencyFactor.x >> matIter->second->transparencyFactor.y >> matIter->second->transparencyFactor.z;
				}
			}
			else if (buffer == L"Ka") {
				if (matIter != materials.end()) {
					input >> matIter->second->ambient.x >> matIter->second->ambient.y >> matIter->second->ambient.z;
				}
			}
			else if (buffer == L"Kd") {
				if (matIter != materials.end()) {
					input >> matIter->second->diffuse.x >> matIter->second->diffuse.y >> matIter->second->diffuse.z;
				}
			}
			else if (buffer == L"Ks") {
				if (matIter != materials.end()) {
					input >> matIter->second->default—olor.x >> matIter->second->default—olor.y >> matIter->second->default—olor.z;
				}
			}
			else if (buffer == L"map_Kd") {
				if (matIter != materials.end()) {
					input >> matIter->second->diffuseMap;
					matIter->second->hasTexture = true;
				}
				else if (buffer == L"map_bump" || buffer == L"bump") {
					if (matIter != materials.end()) {
						input >> matIter->second->normalMap;
						matIter->second->hasNormMap = true;
					}

				}
			}
		}
		return true;
	}
	else {
		return false;
	}
}

void ParseConvert::Out() {
	std::ofstream out("check.txt");
	for (const auto &element : this->vertex) {
		out << element.pos.x << ',' << element.pos.y << ',' << element.pos.z << ',';
		out << element.normals.x << ',' << element.normals.y << ',' << element.normals.z << ',';
		out << element.textCoord.x << ',' << element.textCoord.y << ',' << element.textCoord.z << '\n';
	}
	for (const auto &element : this->materials) {
		out << element.second->range << '\n';
		out << element.second->transparancy << '\n';
		out << element.second->transparencyFactor.x << ',' << element.second->transparencyFactor.y << ',' << element.second->transparencyFactor.z << '\n';
		out << element.second->ambient.x << ',' << element.second->ambient.y << ',' << element.second->ambient.z << '\n';
		out << element.second->diffuse.x << ',' << element.second->diffuse.y << ',' << element.second->diffuse.z << '\n';
		out << element.second->default—olor.x << ',' << element.second->default—olor.y << ',' << element.second->default—olor.z << '\n';

	}
	size_t count(0);
	for (const auto &element : this->indecis) {
		if (count == 3) {
			out << '\n';
			count = 0;
		}
		out << element << ' ';
		count++;
	}
	out << '\n';
	std::wcout << useMtl[0] << L'\n';
	out.close();
}
ParseConvert::~ParseConvert()
{
	verteÒis.clear();
	verteÒis.shrink_to_fit();
	text—oords.clear();
	text—oords.shrink_to_fit();
	indecis.clear();
	indecis.shrink_to_fit();
	normals.clear();
	normals.shrink_to_fit();
	useMtl.clear();
	useMtl.shrink_to_fit();
	vertex.clear();
	vertex.shrink_to_fit();
	materials.clear();
}
