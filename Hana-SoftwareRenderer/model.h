#ifndef __MODEL_H__
#define __MODEL_H__
#include <vector>
#include <string>
#include <unordered_map>
#include "tgaimage.h"
#include "mathapi.h"

namespace MAPPData
{
    class ModelInfo;
    class Mesh;
}

class Model
{
public:
    std::vector<Vector3f> verts_;
    std::vector<std::vector<Vector3i> > faces_; // attention, this Vec3i means vertex/uv/normal
    std::unordered_map<int, std::vector<int>> idMap_;
    //std::vector<Vector3i> _triangleIdx;//自定义数据，用于替换原来带法向与纹理坐标的faces_
    //std::vector<Vector3f> norms_;//待删除
    //std::vector<Vector2f> uv_;//待删除

    float xMin = 1.0e30f;
    float xMax = -1.0e30f;
    float yMin = 1.0e30f;
    float yMax = -1.0e30f;
    float zMin = 1.0e30f;
    float zMax = -1.0e30f;

    TGAImage diffusemap_;//待删除
    TGAImage normalmap_;//待删除
    TGAImage specularmap_;//待删除
    void load_texture(std::string filename, const char *suffix, TGAImage &img);
public:
    Model(const char *filename);
    Model(const MAPPData::Mesh& mesh);
    Model(const MAPPData::ModelInfo& modelInfo);

    ~Model();
    int nverts();
    int nfaces();
    Vector3f normal(int iface, int nthvert);
    Vector3f normal(Vector2f uv);
    Vector3f vert(int i);
    Vector3f vert(int iface, int nthvert);
    Vector2f uv(int iface, int nthvert);
    TGAColor diffuse(Vector2f uv);
    float specular(Vector2f uv);
    std::vector<int> face(int idx);
    TGAImage* get_diffuse_map();
    TGAImage* get_normal_map();
    TGAImage* get_specular_map();
};
#endif //__MODEL_H__

