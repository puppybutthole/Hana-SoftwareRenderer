﻿#include <iostream>
#include <fstream>
#include <sstream>
#include "model.h"
#include "MAPPModelInfo/Mesh.h"
#include "MAPPModelInfo/ModelInfo.h"
Model::Model(const char* filename) : verts_(), faces_()/*, norms_(), uv_(), diffusemap_(), normalmap_(), specularmap_()*/
{
    std::ifstream in;
    in.open(filename, std::ifstream::in);
    if (in.fail()) return;
    std::string line;
    while (!in.eof())
    {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;
        if (!line.compare(0, 2, "v "))
        {
            iss >> trash;
            Vector3f v;
            //for (int i = 0; i < 3; i++) iss >> v[i];
            iss >> v[0];
            xMax = std::max(xMax, v[0]);
            xMin = std::min(xMin, v[0]);

            iss >> v[1];
            yMax = std::max(yMax, v[1]);
            yMin = std::min(yMin, v[1]);

            iss >> v[2];
            zMax = std::max(zMax, v[2]);
            zMin = std::min(zMin, v[2]);

            verts_.push_back(v);
        }
        /*else if (!line.compare(0, 3, "vn ")) {
            iss >> trash >> trash;
            Vector3f n;
            for (int i = 0; i < 3; i++) iss >> n[i];
            norms_.push_back(n);
        }
        else if (!line.compare(0, 3, "vt ")) {
            iss >> trash >> trash;
            Vector2f uv;
            for (int i = 0; i < 2; i++) iss >> uv[i];
            uv_.push_back(uv);
        }*/
        else if (!line.compare(0, 2, "f "))
        {
            std::string tmpStr = iss.str();

            if (tmpStr.find('/') != std::string::npos)
            {
                std::vector<Vector3i> f;
                Vector3i tmp;
                iss >> trash;
                while (iss >> tmp[0] >> trash >> tmp[1] >> trash >> tmp[2])
                {
                    for (int i = 0; i < 3; i++) tmp[i]--; // in wavefront obj all indices start at 1, not zero
                    f.push_back(tmp);
                }
                faces_.push_back(f);
            }
            else
            {
                Vector3i pointIdx;
                iss >> trash;

                iss >> pointIdx[0] >> pointIdx[1] >> pointIdx[2];
                //_triangleIdx.emplace_back(pointIdx);

                std::vector<Vector3i> f(3);

                for (int i=0;i<3;i++)
                {
                    pointIdx[i]--;
                    f[i][0] = pointIdx[i];
                }
                faces_.push_back(f);
            }
        }
    }
    //std::cerr << "# v# " << verts_.size() << " f# " << faces_.size() << " vt# " << uv_.size() << " vn# " << norms_.size() << std::endl;
    /*load_texture(filename, "_diffuse.tga", diffusemap_);
    load_texture(filename, "_nm_tangent.tga", normalmap_);
    load_texture(filename, "_spec.tga", specularmap_);*/
    return;
}

Model::Model(const MAPPData::Mesh& mesh)
{
    for (const auto& ver:mesh.m_points)
    {
        Vector3f v;
        v[0] = ver[0];
        v[1] = ver[1];
        v[2] = ver[2];

        xMax = std::max(xMax, v[0]);
        xMin = std::min(xMin, v[0]);

        yMax = std::max(yMax, v[1]);
        yMin = std::min(yMin, v[1]);

        zMax = std::max(zMax, v[2]);
        zMin = std::min(zMin, v[2]);

        verts_.push_back(v);
    }
    for (const auto& triangle : mesh.m_triangles)
    {
        const auto& index = triangle._pointIndex;
        std::vector<Vector3i> f(3);
        f[0][0] = index[0];
        f[2][0] = index[1];
        f[1][0] = index[2];
        faces_.push_back(f);
    }
}

Model::Model(const MAPPData::ModelInfo& modelInfo)
{
    const auto& mesh = modelInfo.m_mesh;
    for (const auto& ver : mesh.m_points)
    {
        Vector3f v;
        v[0] = ver[0];
        v[1] = ver[1];
        v[2] = ver[2];

        xMax = std::max(xMax, v[0]);
        xMin = std::min(xMin, v[0]);

        yMax = std::max(yMax, v[1]);
        yMin = std::min(yMin, v[1]);

        zMax = std::max(zMax, v[2]);
        zMin = std::min(zMin, v[2]);

        verts_.push_back(v);
    }
    for (const auto& triangle : mesh.m_triangles)
    {
        const auto& index = triangle._pointIndex;
        std::vector<Vector3i> f(3);
        f[0][0] = index[0];
        f[2][0] = index[1];
        f[1][0] = index[2];
        faces_.push_back(f);

        Vector3f n;
        n[0] = triangle._normal[0];
        n[1] = triangle._normal[1];
        n[2] = triangle._normal[2];
        faceNormal_.push_back(n);
    }
    const auto& faceData = modelInfo.GetFaceDataManager();
    idMap_ = faceData.m_faceIDToTriangleIndex;
}

Model::~Model() {}

int Model::nverts()
{
    return (int)verts_.size();
}

int Model::nfaces()
{
    return (int)faces_.size();
}

std::vector<int> Model::face(int idx)
{
    std::vector<int> face;
    for (int i = 0; i < (int)faces_[idx].size(); i++) face.push_back(faces_[idx][i][0]);
    return face;
}

Vector3f Model::vert(int i)
{
    return verts_[i];
}

Vector3f Model::vert(int iface, int nthvert)
{
    return verts_[faces_[iface][nthvert][0]];
}

void Model::load_texture(std::string filename, const char* suffix, TGAImage& img)
{
    std::string texfile(filename);
    size_t dot = texfile.find_last_of(".");
    if (dot != std::string::npos)
    {
        texfile = texfile.substr(0, dot) + std::string(suffix);
        //std::cerr << "texture file " << texfile << " loading " << (img.read_tga_file(texfile.c_str()) ? "ok" : "failed") << std::endl;
        img.read_tga_file(texfile.c_str());
        img.flip_vertically();
    }
}

//TGAColor Model::diffuse(Vector2f uvf)
//{
//    Vector2i uv(uvf[0] * diffusemap_.get_width(), uvf[1] * diffusemap_.get_height());
//    return diffusemap_.get(uv[0], uv[1]);
//}
//
//Vector3f Model::normal(Vector2f uvf)
//{
//    Vector2i uv(uvf[0] * normalmap_.get_width(), uvf[1] * normalmap_.get_height());
//    TGAColor c = normalmap_.get(uv[0], uv[1]);
//    Vector3f res;
//    for (int i = 0; i < 3; i++)
//        res[2 - i] = (float)c[i] / 255.f * 2.f - 1.f;
//    return res;
//}

//Vector2f Model::uv(int iface, int nthvert)
//{
//    return uv_[faces_[iface][nthvert][1]];
//}

//float Model::specular(Vector2f uvf)
//{
//    Vector2i uv(uvf[0] * specularmap_.get_width(), uvf[1] * specularmap_.get_height());
//    return specularmap_.get(uv[0], uv[1])[0] / 1.f;
//}

//Vector3f Model::normal(int iface, int nthvert)
//{
//    int idx = faces_[iface][nthvert][2];
//    return norms_[idx].normalize();
//}

//TGAImage* Model::get_diffuse_map()
//{
//    return &diffusemap_;
//}
//
//TGAImage* Model::get_normal_map()
//{
//    return &normalmap_;
//}
//TGAImage* Model::get_specular_map()
//{
//    return &specularmap_;
//}

