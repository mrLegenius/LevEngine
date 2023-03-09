#pragma once
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include "../Renderer/3D/Mesh.h"

class ObjLoader
{
public:
	std::shared_ptr<Mesh> LoadMesh(const std::string& path)
	{
		std::vector<DirectX::SimpleMath::Vector3> positions;
		std::vector<DirectX::SimpleMath::Vector2> uvs;
		std::vector<DirectX::SimpleMath::Vector3> normals;

		std::vector<uint32_t> position_indices;
		std::vector<uint32_t> uv_indices;
		std::vector<uint32_t> normal_indices;
		std::vector<uint32_t> indices;

		std::stringstream ss;
		std::ifstream file(path);
		std::string line;
		std::string prefix;

		float tempX, tempY, tempZ;
		std::string tempString1, tempString2, tempString3;

		if (!file.is_open())
		{
			std::cout << "[ObjLoader] Can't open " << path << " file" << std::endl;
			return nullptr;
		}

		auto mesh = std::make_shared<Mesh>();

		int verticesCount = 0;

		while(std::getline(file, line))
		{
			ss.clear();
			ss.str(line);
			ss >> prefix;

			if (prefix == "v")
			{
				ss >> tempX >> tempY >> tempZ;
				positions.emplace_back(DirectX::SimpleMath::Vector3(tempX, tempY, tempZ));
			}
			else if (prefix == "vt")
			{
				ss >> tempX >> tempY;
				uvs.emplace_back(DirectX::SimpleMath::Vector2(tempX, tempY));
			}
			else if (prefix == "vn")
			{
				ss >> tempX >> tempY >> tempZ;
				normals.emplace_back(DirectX::SimpleMath::Vector3(tempX, tempY, tempZ));
			}
			else if (prefix == "f")
			{
				int v1, v2, v3, v4, t1, t2, t3, t4, n1, n2, n3, n4;

				if (sscanf_s(ss.str().c_str(),
				"f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d",
							&v1, &t1, &n1, &v2, &t2, &n2, &v3, &t3, &n3, &v4, &t4, &n4) == 12)
				{
					position_indices.emplace_back(v1);
					position_indices.emplace_back(v2);
					position_indices.emplace_back(v3);
					position_indices.emplace_back(v4);

					uv_indices.emplace_back(t1);
					uv_indices.emplace_back(t2);
					uv_indices.emplace_back(t3);
					uv_indices.emplace_back(t4);

					normal_indices.emplace_back(n1);
					normal_indices.emplace_back(n2);
					normal_indices.emplace_back(n3);
					normal_indices.emplace_back(n4);

					indices.emplace_back(verticesCount);
					indices.emplace_back(verticesCount + 1);
					indices.emplace_back(verticesCount + 2);
					indices.emplace_back(verticesCount + 0);
					indices.emplace_back(verticesCount + 2);
					indices.emplace_back(verticesCount + 3);
					verticesCount += 4;
				}

				else if (sscanf_s(ss.str().c_str(),
					"f %d//%d %d//%d %d//%d %d//%d",
					&v1, &n1, &v2, &n2, &v3, &n3, &v4, &n4) == 8)
				{
					std::cout << "successful read of face4x2" << std::endl;
					//TODO
				}

				else if (sscanf_s(ss.str().c_str(),
						"f %d/%d/%d %d/%d/%d %d/%d/%d",
							&v1, &t1, &n1, &v2, &t2, &n2, &v3, &t3, &n3) == 9)
				{
					position_indices.emplace_back(v1);
					position_indices.emplace_back(v2);
					position_indices.emplace_back(v3);

					uv_indices.emplace_back(t1);
					uv_indices.emplace_back(t2);
					uv_indices.emplace_back(t3);

					normal_indices.emplace_back(n1);
					normal_indices.emplace_back(n2);
					normal_indices.emplace_back(n3);

					indices.emplace_back(verticesCount);
					indices.emplace_back(verticesCount + 1);
					indices.emplace_back(verticesCount + 2);
					verticesCount += 3;
				}

				else if (sscanf_s(ss.str().c_str(),
						"f %d//%d %d//%d %d//%d",
							&v1, &n1, &v2, &n2, &v3, &n3) == 6)
				{
					position_indices.emplace_back(v1);
					position_indices.emplace_back(v2);
					position_indices.emplace_back(v3);

					normal_indices.emplace_back(n1);
					normal_indices.emplace_back(n2);
					normal_indices.emplace_back(n3);

					verticesCount += 3;
				}
			}
		}

		for (uint32_t i = 0; i < position_indices.size(); ++i)
		{
			mesh->AddVertex(positions[position_indices[i] - 1]);
			mesh->AddUV(uvs[uv_indices[i] - 1]);
			mesh->AddNormal(normals[normal_indices[i] - 1]);
		}

		for (uint32_t index : indices)
			mesh->AddIndex(index);

		return mesh;
	}
};
