﻿#pragma once
#include "ConstDefine.h"

#include "mathapi.h"
#include "gameobject.h"
#include "camera.h"
#include "platform.h"

#define ORTHO

static Color AMBIENT = Color(54.f / 255, 58.f / 255, 66.f / 255);
//static Color LightColor = Color(255.f / 255, 244.f / 255, 214.f / 255);


namespace MAPPData
{
    class Mesh;
    class ModelInfo;
}

class DrawModel
{
public:
    //GameObject* light;
    GameObject_StaticModel* gameobject;
    Material* material;
    ShaderData* shader_data;
    DrawData* shadow_draw_data;
    DrawData* draw_data;
    //RenderBuffer* shdaow_map;
    IShader* shader;
    //ShadowShader* shadow_shader;


    DrawModel(/*GameObject* light, */GameObject_StaticModel* go, Material* material, IShader* shader)
    {
        /*this->light = light;*/
        this->gameobject = go;
        this->material = material;
        this->shader = shader;

        shader_data = new ShaderData();
        //shader_data->matrial = material;

        //shadow_draw_data = nullptr;
        //shadow_shader = nullptr;
        //shdaow_map = nullptr;

        draw_data = new DrawData();
        draw_data->model = gameobject->model;
        draw_data->shader = shader;
        draw_data->shader->shader_data = shader_data;
    }

    ~DrawModel()
    {
        delete shader_data;
        delete draw_data;
        //if (shadow_shader) delete shadow_shader;
        //if (shdaow_map)  delete shdaow_map;
        if (shadow_draw_data)  delete shadow_draw_data;
    }

    void draw(Camera* camera, RenderBuffer* frameBuffer/*, bool enable_shadow*/)
    {
        //camera->set_transform(Vector3f(0, 0, gameobject->model->zMax + 1.0f), Vector3f(0, 0, 0));
        Matrix4x4 view_matrix = camera->get_view_matrix();
        
#ifdef ORTHO
        float cameraDistance = sqrt(camera->get_position().x * camera->get_position().x + camera->get_position().y * camera->get_position().y + camera->get_position().z * camera->get_position().z);
        
        float x_center = (gameobject->model->xMin + gameobject->model->xMax) * 0.5;
        float y_center = (gameobject->model->yMin + gameobject->model->yMax) * 0.5;

        float boxWidth = std::max(gameobject->model->xMax - gameobject->model->xMin, gameobject->model->yMax - gameobject->model->yMin)*0.5 + cameraDistance;

        Matrix4x4 projection_matrix = camera->get_ortho_proj_matrix(

            x_center-boxWidth, x_center+boxWidth, y_center-boxWidth, y_center+boxWidth,-FAR ,FAR

            //gameobject->model->xMin - 1.f, gameobject->model->xMax + 1.f,
            //gameobject->model->yMin - 1.f, gameobject->model->yMax + 1.f,
            //gameobject->model->zMin - 10.f, gameobject->model->zMax + 10.f

            /*gameobject->model->xMin - cameraDistance, gameobject->model->xMax + cameraDistance,
            gameobject->model->yMin - cameraDistance, gameobject->model->yMax + cameraDistance,
            gameobject->model->zMin - cameraDistance, gameobject->model->zMax + cameraDistance*/
        );
#endif // ORTHO
#ifndef ORTHO
        Matrix4x4 projection_matrix = camera->get_proj_matrix();
#endif // !ORTHO

        Matrix4x4 model_matrix = gameobject->GetModelMatrix();
        Matrix4x4 model_matrix_I = model_matrix.invert();

        shader_data->view_Pos = camera->get_position();
        //shader_data->light_dir = (light->transform.position - camera->get_target_position()).normalize();
        //shader_data->light_color = LightColor;
        shader_data->ambient = AMBIENT;
        shader_data->model_matrix = model_matrix;
        shader_data->model_matrix_I = model_matrix_I;
        shader_data->view_matrix = view_matrix;
        shader_data->projection_matrix = projection_matrix;
        float aspect = (float)frameBuffer->width / (float)frameBuffer->height;
        //shader_data->light_vp_matrix = orthographic(aspect, 1, 0, 5) * lookat(light->transform.position, camera->get_target_position(), Vector3f(0, 1, 0));
        shader_data->camera_vp_matrix = projection_matrix * view_matrix;
        //shader_data->enable_shadow = enable_shadow;

        //if (enable_shadow)
        //{
        //    if (!shadow_draw_data)
        //    {
        //        shadow_shader = new ShadowShader();
        //        shdaow_map = new RenderBuffer(frameBuffer->width, frameBuffer->height);
        //        shadow_draw_data = new DrawData();
        //        shadow_draw_data->model = gameobject->model;
        //        shadow_draw_data->shader = shadow_shader;
        //        shadow_draw_data->shader->shader_data = shader_data;
        //        shadow_draw_data->render_buffer = shdaow_map;
        //    }

        //    graphics_draw_triangle(shadow_draw_data);
        //    shader_data->shadow_map = shdaow_map;
        //}

        draw_data->render_buffer = frameBuffer;
        graphics_draw_triangle(draw_data);


        //if (enable_shadow)
        //{
        //    shdaow_map->renderbuffer_clear_color(Color::Black);
        //    shdaow_map->renderbuffer_clear_depth(std::numeric_limits<float>::max());
        //}
    }
};

struct ShaderInfo
{
    const char* des;
    IShader* shader;
};

class Scene
{
public:
    //GameObject* light;
    Camera* camera;
    RenderBuffer* frameBuffer;
    char text[500];
    //bool enable_shadow;
    Scene(RenderBuffer* frameBuffer);
    ~Scene();

    virtual void tick(float delta_time);
    virtual void on_key_input(keycode_t key, int pressed);
    char* get_text();
};

class SingleModelScene :public Scene
{
private:
    GameObject_StaticModel* gameobject;
    Material* material;
    ShaderInfo shaderInfos[6];
    DrawModel* draw_model;
    int cur_shader_index;
public:
    SingleModelScene(const char* file_name, RenderBuffer* render_Buffer);
    SingleModelScene(const MAPPData::ModelInfo& modelInfo, RenderBuffer* render_Buffer);
    ~SingleModelScene();

    void tick(float delta_time) override;
    void on_key_input(keycode_t key, int pressed) override;
};