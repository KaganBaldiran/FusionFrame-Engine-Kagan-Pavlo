#include "Application.hpp"
#include "FusionFrame.h"
//#include <glew.h>
//#include <glfw3.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <memory>
#include <unordered_set>

#define SPEED 24.0f
#define CAMERA_CLOSE_PLANE 0.1f
#define CAMERA_FAR_PLANE 180.0f
const float BlendAmount = 0.04f;
const float InterBlendAmount = 0.04f;

int Application::Run()
{
	const int width = 1000;
	const int height = 1000;

	GLFWwindow* window = FUSIONUTIL::InitializeWindow(width, height,4,6, "FusionFrame Engine");
	
	//FUSIONCORE::InitializeAnimationUniformBuffer();

	FUSIONUTIL::DefaultShaders Shaders;
	FUSIONUTIL::InitializeDefaultShaders(Shaders);

	FUSIONCORE::SHAPES::InitializeShapeBuffers();

	FUSIONCORE::CubeMap cubemap(*Shaders.CubeMapShader);
	FUSIONCORE::ImportCubeMap("Resources/rustig_koppie_puresky_2k.hdr", 1024, cubemap, Shaders.HDRIShader->GetID(), Shaders.ConvolutateCubeMapShader->GetID(), Shaders.PreFilterCubeMapShader->GetID());

	const FUSIONUTIL::VideoMode mode = FUSIONUTIL::GetVideoMode(FUSIONUTIL::GetPrimaryMonitor());
	FUSIONCORE::Gbuffer Gbuffer(mode.width, mode.height);

	FUSIONCORE::FrameBuffer ScreenFrameBuffer(mode.width, mode.height);

	FUSIONCORE::LightIcon = std::make_unique<FUSIONCORE::Model>("Resources/LightIcon.fbx");

	FUSIONCORE::Camera3D camera3d;
	FUSIONCORE::Camera2D camera2d;

	FUSIONCORE::Texture2D ShovelDiffuse("Resources/texture_diffuse.png", FF_TEXTURE_TARGET_GL_TEXTURE_2D, FF_DATA_TYPE_GL_UNSIGNED_BYTE, FF_TEXTURE_FILTER_MODE_GL_LINEAR, FF_TEXTURE_FILTER_MODE_GL_LINEAR, FF_TEXTURE_WRAP_MODE_GL_CLAMP_TO_EDGE, FF_TEXTURE_WRAP_MODE_GL_CLAMP_TO_EDGE, true);
	FUSIONCORE::Texture2D ShovelNormal("Resources/texture_normal.png", FF_TEXTURE_TARGET_GL_TEXTURE_2D, FF_DATA_TYPE_GL_UNSIGNED_BYTE, FF_TEXTURE_FILTER_MODE_GL_LINEAR, FF_TEXTURE_FILTER_MODE_GL_LINEAR, FF_TEXTURE_WRAP_MODE_GL_CLAMP_TO_EDGE, FF_TEXTURE_WRAP_MODE_GL_CLAMP_TO_EDGE, true);
	FUSIONCORE::Texture2D ShovelSpecular("Resources/texture_specular.png", FF_TEXTURE_TARGET_GL_TEXTURE_2D, FF_DATA_TYPE_GL_UNSIGNED_BYTE, FF_TEXTURE_FILTER_MODE_GL_LINEAR, FF_TEXTURE_FILTER_MODE_GL_LINEAR, FF_TEXTURE_WRAP_MODE_GL_CLAMP_TO_EDGE, FF_TEXTURE_WRAP_MODE_GL_CLAMP_TO_EDGE, true);

	FUSIONCORE::Texture2D FloorSpecular("Resources/floor/snow_02_rough_1k.png", FF_TEXTURE_TARGET_GL_TEXTURE_2D, FF_DATA_TYPE_GL_UNSIGNED_BYTE, FF_TEXTURE_FILTER_MODE_GL_LINEAR, FF_TEXTURE_FILTER_MODE_GL_LINEAR, FF_TEXTURE_WRAP_MODE_GL_REPEAT, FF_TEXTURE_WRAP_MODE_GL_REPEAT, true);
	FUSIONCORE::Texture2D FloorNormal("Resources/floor/snow_02_nor_gl_1k.png", FF_TEXTURE_TARGET_GL_TEXTURE_2D, FF_DATA_TYPE_GL_UNSIGNED_BYTE, FF_TEXTURE_FILTER_MODE_GL_LINEAR, FF_TEXTURE_FILTER_MODE_GL_LINEAR, FF_TEXTURE_WRAP_MODE_GL_REPEAT, FF_TEXTURE_WRAP_MODE_GL_REPEAT, true);
	FUSIONCORE::Texture2D FloorAlbedo("Resources/floor/snow_02_diff_1k.png", FF_TEXTURE_TARGET_GL_TEXTURE_2D, FF_DATA_TYPE_GL_UNSIGNED_BYTE, FF_TEXTURE_FILTER_MODE_GL_LINEAR, FF_TEXTURE_FILTER_MODE_GL_LINEAR, FF_TEXTURE_WRAP_MODE_GL_REPEAT, FF_TEXTURE_WRAP_MODE_GL_REPEAT, true);

	FUSIONCORE::Texture2D SofaDiffuse("Resources\\models\\sofa\\textures\\sofa_03_diff_2k.jpg", FF_TEXTURE_TARGET_GL_TEXTURE_2D, FF_DATA_TYPE_GL_UNSIGNED_BYTE, FF_TEXTURE_FILTER_MODE_GL_LINEAR, FF_TEXTURE_FILTER_MODE_GL_LINEAR, FF_TEXTURE_WRAP_MODE_GL_CLAMP_TO_EDGE, FF_TEXTURE_WRAP_MODE_GL_CLAMP_TO_EDGE, true);
	FUSIONCORE::Texture2D SofaNormal("Resources\\models\\sofa\\textures\\sofa_03_nor_gl_2k.jpg", FF_TEXTURE_TARGET_GL_TEXTURE_2D, FF_DATA_TYPE_GL_UNSIGNED_BYTE, FF_TEXTURE_FILTER_MODE_GL_LINEAR, FF_TEXTURE_FILTER_MODE_GL_LINEAR, FF_TEXTURE_WRAP_MODE_GL_CLAMP_TO_EDGE, FF_TEXTURE_WRAP_MODE_GL_CLAMP_TO_EDGE, true);
	FUSIONCORE::Texture2D SofaSpecular("Resources\\models\\sofa\\textures\\sofa_03_rough_2k.jpg", FF_TEXTURE_TARGET_GL_TEXTURE_2D, FF_DATA_TYPE_GL_UNSIGNED_BYTE, FF_TEXTURE_FILTER_MODE_GL_LINEAR, FF_TEXTURE_FILTER_MODE_GL_LINEAR, FF_TEXTURE_WRAP_MODE_GL_CLAMP_TO_EDGE, FF_TEXTURE_WRAP_MODE_GL_CLAMP_TO_EDGE, true);

	FUSIONCORE::Texture2D StoveDiffuse("Resources\\models\\stove\\textures\\electric_stove_diff_2k.jpg", FF_TEXTURE_TARGET_GL_TEXTURE_2D, FF_DATA_TYPE_GL_UNSIGNED_BYTE, FF_TEXTURE_FILTER_MODE_GL_LINEAR, FF_TEXTURE_FILTER_MODE_GL_LINEAR, FF_TEXTURE_WRAP_MODE_GL_CLAMP_TO_EDGE, FF_TEXTURE_WRAP_MODE_GL_CLAMP_TO_EDGE, true);
	FUSIONCORE::Texture2D StoveNormal("Resources\\models\\stove\\textures\\electric_stove_nor_gl_2k.jpg", FF_TEXTURE_TARGET_GL_TEXTURE_2D, FF_DATA_TYPE_GL_UNSIGNED_BYTE, FF_TEXTURE_FILTER_MODE_GL_LINEAR, FF_TEXTURE_FILTER_MODE_GL_LINEAR, FF_TEXTURE_WRAP_MODE_GL_CLAMP_TO_EDGE, FF_TEXTURE_WRAP_MODE_GL_CLAMP_TO_EDGE, true);
	FUSIONCORE::Texture2D StoveSpecular("Resources\\models\\stove\\textures\\electric_stove_rough_2k.jpg", FF_TEXTURE_TARGET_GL_TEXTURE_2D, FF_DATA_TYPE_GL_UNSIGNED_BYTE, FF_TEXTURE_FILTER_MODE_GL_LINEAR, FF_TEXTURE_FILTER_MODE_GL_LINEAR, FF_TEXTURE_WRAP_MODE_GL_CLAMP_TO_EDGE, FF_TEXTURE_WRAP_MODE_GL_CLAMP_TO_EDGE, true);
	FUSIONCORE::Texture2D StoveMetalic("Resources\\models\\stove\\textures\\electric_stove_metal_2k.jpg", FF_TEXTURE_TARGET_GL_TEXTURE_2D, FF_DATA_TYPE_GL_UNSIGNED_BYTE, FF_TEXTURE_FILTER_MODE_GL_LINEAR, FF_TEXTURE_FILTER_MODE_GL_LINEAR, FF_TEXTURE_WRAP_MODE_GL_CLAMP_TO_EDGE, FF_TEXTURE_WRAP_MODE_GL_CLAMP_TO_EDGE, true);

	FUSIONCORE::Texture2D WallDiffuse("Resources\\wall\\textures\\painted_plaster_wall_diff_2k.jpg", FF_TEXTURE_TARGET_GL_TEXTURE_2D, FF_DATA_TYPE_GL_UNSIGNED_BYTE, FF_TEXTURE_FILTER_MODE_GL_LINEAR, FF_TEXTURE_FILTER_MODE_GL_LINEAR, FF_TEXTURE_WRAP_MODE_GL_CLAMP_TO_EDGE, FF_TEXTURE_WRAP_MODE_GL_CLAMP_TO_EDGE, true);
	FUSIONCORE::Texture2D WallNormal("Resources\\wall\\textures\\painted_plaster_wall_nor_dx_2k.jpg", FF_TEXTURE_TARGET_GL_TEXTURE_2D, FF_DATA_TYPE_GL_UNSIGNED_BYTE, FF_TEXTURE_FILTER_MODE_GL_LINEAR, FF_TEXTURE_FILTER_MODE_GL_LINEAR, FF_TEXTURE_WRAP_MODE_GL_CLAMP_TO_EDGE, FF_TEXTURE_WRAP_MODE_GL_CLAMP_TO_EDGE, true);
	FUSIONCORE::Texture2D WallSpecular("Resources\\wall\\textures\\painted_plaster_wall_rough_2k.jpg", FF_TEXTURE_TARGET_GL_TEXTURE_2D, FF_DATA_TYPE_GL_UNSIGNED_BYTE, FF_TEXTURE_FILTER_MODE_GL_LINEAR, FF_TEXTURE_FILTER_MODE_GL_LINEAR, FF_TEXTURE_WRAP_MODE_GL_CLAMP_TO_EDGE, FF_TEXTURE_WRAP_MODE_GL_CLAMP_TO_EDGE, true);

	FUSIONCORE::Texture2D bearDiffuse("Resources\\taunt\\textures\\bear_diffuse.png", FF_TEXTURE_TARGET_GL_TEXTURE_2D, FF_DATA_TYPE_GL_UNSIGNED_BYTE, FF_TEXTURE_FILTER_MODE_GL_LINEAR, FF_TEXTURE_FILTER_MODE_GL_LINEAR, FF_TEXTURE_WRAP_MODE_GL_CLAMP_TO_EDGE, FF_TEXTURE_WRAP_MODE_GL_CLAMP_TO_EDGE, true);
	FUSIONCORE::Texture2D bearNormal("Resources\\taunt\\textures\\bear_normal.png", FF_TEXTURE_TARGET_GL_TEXTURE_2D, FF_DATA_TYPE_GL_UNSIGNED_BYTE, FF_TEXTURE_FILTER_MODE_GL_LINEAR, FF_TEXTURE_FILTER_MODE_GL_LINEAR, FF_TEXTURE_WRAP_MODE_GL_CLAMP_TO_EDGE, FF_TEXTURE_WRAP_MODE_GL_CLAMP_TO_EDGE, true);
	FUSIONCORE::Texture2D bearSpecular("Resources\\taunt\\textures\\bear_roughness.png", FF_TEXTURE_TARGET_GL_TEXTURE_2D, FF_DATA_TYPE_GL_UNSIGNED_BYTE, FF_TEXTURE_FILTER_MODE_GL_LINEAR, FF_TEXTURE_FILTER_MODE_GL_LINEAR, FF_TEXTURE_WRAP_MODE_GL_CLAMP_TO_EDGE, FF_TEXTURE_WRAP_MODE_GL_CLAMP_TO_EDGE, true);

	FUSIONCORE::Texture2D ShrubDiffuse("Resources\\models\\shrub\\textures\\shrub_04_diff_1k.png", FF_TEXTURE_TARGET_GL_TEXTURE_2D, FF_DATA_TYPE_GL_UNSIGNED_BYTE, FF_TEXTURE_FILTER_MODE_GL_LINEAR, FF_TEXTURE_FILTER_MODE_GL_LINEAR, FF_TEXTURE_WRAP_MODE_GL_CLAMP_TO_EDGE, FF_TEXTURE_WRAP_MODE_GL_CLAMP_TO_EDGE, true);
	FUSIONCORE::Texture2D ShrubNormal("Resources\\models\\shrub\\textures\\shrub_04_nor_gl_1k.png", FF_TEXTURE_TARGET_GL_TEXTURE_2D, FF_DATA_TYPE_GL_UNSIGNED_BYTE, FF_TEXTURE_FILTER_MODE_GL_LINEAR, FF_TEXTURE_FILTER_MODE_GL_LINEAR, FF_TEXTURE_WRAP_MODE_GL_CLAMP_TO_EDGE, FF_TEXTURE_WRAP_MODE_GL_CLAMP_TO_EDGE, true);
	FUSIONCORE::Texture2D ShrubSpecular("Resources\\models\\shrub\\textures\\shrub_04_rough_1k.png", FF_TEXTURE_TARGET_GL_TEXTURE_2D, FF_DATA_TYPE_GL_UNSIGNED_BYTE, FF_TEXTURE_FILTER_MODE_GL_LINEAR, FF_TEXTURE_FILTER_MODE_GL_LINEAR, FF_TEXTURE_WRAP_MODE_GL_CLAMP_TO_EDGE, FF_TEXTURE_WRAP_MODE_GL_CLAMP_TO_EDGE, true);
	FUSIONCORE::Texture2D ShrubAlpha("Resources\\models\\shrub\\textures\\shrub_04_alpha_1k.png", FF_TEXTURE_TARGET_GL_TEXTURE_2D, FF_DATA_TYPE_GL_UNSIGNED_BYTE, FF_TEXTURE_FILTER_MODE_GL_LINEAR, FF_TEXTURE_FILTER_MODE_GL_LINEAR, FF_TEXTURE_WRAP_MODE_GL_CLAMP_TO_EDGE, FF_TEXTURE_WRAP_MODE_GL_CLAMP_TO_EDGE, true);

	int shadowMapSize = 512;

	FUSIONCORE::OmniShadowMap ShadowMap0(shadowMapSize, shadowMapSize, 75.0f);
	//FUSIONCORE::OmniShadowMap ShadowMap1(shadowMapSize, shadowMapSize, 75.0f);
	//FUSIONCORE::OmniShadowMap ShadowMap2(shadowMapSize, shadowMapSize, 75.0f);
	//FUSIONCORE::OmniShadowMap ShadowMap3(shadowMapSize, shadowMapSize, 75.0f);

	std::vector<float> shadowCascadeLevels{ CAMERA_FAR_PLANE / 50.0f, CAMERA_FAR_PLANE / 25.0f, CAMERA_FAR_PLANE / 10.0f, CAMERA_FAR_PLANE / 2.0f };
	FUSIONCORE::CascadedDirectionalShadowMap sunShadowMap(2048, 2048, shadowCascadeLevels);

	Vec2<int> WindowSize;
	glm::dvec2 mousePos(0.0f);

	glm::vec3 Target(0.0f);

	camera3d.SetPosition(glm::vec3(12.353, 13.326, 15.2838));
	camera3d.SetOrientation(glm::vec3(-0.593494, -0.648119, -0.477182));

	auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
	std::uniform_real_distribution<float> RandomFloats(-90.0f, 90.0f);
	std::uniform_real_distribution<float> RandomFloatsY(0.0f, 30.0f);
	std::uniform_real_distribution<float> RandomColor(0.0f, 1.0f);
	std::uniform_real_distribution<float> RandomIntensity(400.0f, 600.0f);
	std::default_random_engine engine(seed);

	std::vector<FUSIONCORE::Light> Lights;

	float LightIntensity;
	for (size_t i = 0; i < 10; i++)
	{
		LightIntensity = RandomIntensity(engine);
		Lights.emplace_back(glm::vec3(RandomFloats(engine), RandomFloatsY(engine), RandomFloats(engine)), glm::vec3(RandomColor(engine), RandomColor(engine), RandomColor(engine)), LightIntensity,FF_POINT_LIGHT, LightIntensity / 30.0f);
	}

	FUSIONCORE::Color SunColor(FF_COLOR_AMBER_YELLOW);
	SunColor.Brighter();
	FUSIONCORE::Light Sun(glm::vec3(-0.593494, 0.648119, 0.777182),SunColor.GetRGB(), 5.0f, FF_DIRECTIONAL_LIGHT);

	Shaders.DeferredPBRshader->use();
	FUSIONCORE::UploadLightsShaderUniformBuffer();
	FUSIONCORE::UseShaderProgram(0);
	//FUSIONUTIL::ThreadPool threads(5, 20);
//#define ASYNC
#define NOTASYNC

#ifdef ASYNC
	FUSIONUTIL::Timer stopwatch;
	stopwatch.Set();
	std::unique_ptr<FUSIONOPENGL::Model> model0;
	std::unique_ptr<FUSIONOPENGL::Model> model1;
	std::unique_ptr<FUSIONOPENGL::Model> WateringPot;

	auto import1 = [&]() { model0 = std::make_unique<FUSIONOPENGL::Model>("Resources\\shovel2.obj", true); };
	auto import2 = [&]() { model1 = std::make_unique<FUSIONOPENGL::Model>("Resources\\shovel2.obj", true); };
	auto import3 = [&]() { WateringPot = std::make_unique<FUSIONOPENGL::Model>("Resources\\shovel2.obj", true); };

	std::vector<std::function<void()>> functions;
	functions.push_back(import1);
	functions.push_back(import2);
	functions.push_back(import3);

	threads.enqueue(import1);
	threads.enqueue(import2);
	threads.enqueue(import3);
	/*
	FUSIONUTIL::ExecuteFunctionsAsync(functions);

	// Wait for all shared_futures to finish
	for (auto& shared_future : FUSIONUTIL::shared_futures)
	{

		shared_future.wait();

	}*/

	threads.wait();

	model0->ConstructMeshes(model0->PreMeshDatas);
	model1->ConstructMeshes(model1->PreMeshDatas);
	WateringPot->ConstructMeshes(WateringPot->PreMeshDatas);

	LOG("Duration: " << stopwatch.returnMiliseconds());

#elif defined NOTASYNC
	FUSIONUTIL::Timer stopwatch;
	stopwatch.Set();
	std::unique_ptr<FUSIONCORE::Model> MainCharac = std::make_unique<FUSIONCORE::Model>("Resources\\shovel2.obj");
	std::unique_ptr<FUSIONCORE::Model> model1 = std::make_unique<FUSIONCORE::Model>("Resources\\shovel2.obj");;
	std::unique_ptr<FUSIONCORE::Model> Stove = std::make_unique<FUSIONCORE::Model>("Resources\\models\\stove\\stove.obj");;
	std::unique_ptr<FUSIONCORE::Model> grid = std::make_unique<FUSIONCORE::Model>("Resources\\floor\\grid.obj");
	std::unique_ptr<FUSIONCORE::Model> sofa = std::make_unique<FUSIONCORE::Model>("Resources\\models\\sofa\\model\\sofa.obj");
	std::unique_ptr<FUSIONCORE::Model> wall = std::make_unique<FUSIONCORE::Model>("Resources\\floor\\grid.obj");
	std::unique_ptr<FUSIONCORE::Model> Rock = std::make_unique<FUSIONCORE::Model>("Resources\\models\\RockFormation\\RockFormation.obj");

	//Stove->SetIndirectCommandBuffer(1, 0, 0, 0);

	FUSIONCORE::Model Isaac("C:\\Users\\kbald\\Desktop\\Isaac\\Isaac_low.obj");

	Isaac.GetTransformation().ScaleNoTraceBack(glm::vec3(7.0f));

	grid->GetTransformation().ScaleNoTraceBack({ 8.0f,8.0f ,8.0f });
	grid->GetTransformation().TranslateNoTraceBack({ 0.0f,-1.0f,0.0f });

	Rock->GetTransformation().ScaleNoTraceBack(glm::vec3(1.5f));

	FUSIONCORE::VBO RockInstanceVBO;
	auto RockDistibutedPoints = FUSIONCORE::MESHOPERATIONS::DistributePointsOnMeshSurfaceRandomized(grid->Meshes[0], grid->GetTransformation(), 5, 129);
	FUSIONCORE::MESHOPERATIONS::FillInstanceDataVBO(RockInstanceVBO, RockDistibutedPoints);
	Rock->SetInstanced(RockInstanceVBO, RockDistibutedPoints.size());

	auto RockBoxes = FUSIONPHYSICS::GenerateAABBCollisionBoxesFromInstancedModel(Rock->GetTransformation(), RockDistibutedPoints);

	FUSIONCORE::Model subdModel;
	FUSIONCORE::MESHOPERATIONS::ImportObj("Resources\\subDModel.obj", subdModel);
	subdModel.GetTransformation().ScaleNoTraceBack({ 9.0f,9.0f,9.0f });
	subdModel.GetTransformation().TranslateNoTraceBack({ 22.0f,6.0f,-9.0f });

	auto ImportedModels = FUSIONCORE::ImportMultipleModelsFromDirectory("Resources\\models\\Grasses",false);
	auto shrub = std::move(ImportedModels[1]);
	ImportedModels.erase(ImportedModels.begin() + 1);

	FUSIONCORE::Model Terrain("Resources\\Terrain.obj");
	//FUSIONCORE::MESHOPERATIONS::ImportObj("Resources\\Terrain.obj", Terrain);
	Terrain.GetTransformation().Scale(glm::vec3(250.0f));
	//std::vector<FUSIONPHYSICS::CollisionBox> TerrainCollisionBoxes;

	////FUSIONCORE::MESHOPERATIONS::ExportObj("TerrainExport.obj", Terrain);

	//TerrainCollisionBoxes.reserve(Terrain.Meshes.size());
	//for (size_t i = 0; i < Terrain.Meshes.size(); i++)
	//{
	//	TerrainCollisionBoxes.emplace_back(Terrain.Meshes[i], Terrain.GetTransformation());
	//}

	auto Tower = std::move(ImportedModels[1]);
	ImportedModels.erase(ImportedModels.begin() + 1);
	Tower->GetTransformation().TranslateNoTraceBack({ 39.0f,0.0f,-9.0f });
	FUSIONPHYSICS::CollisionBoxAABB TowerBox(Tower->GetTransformation(), glm::vec3(1.0f));
	Tower->PushChild(&TowerBox);
	Tower->UpdateChildren();
	//FUSIONPHYSICS::MESHOPERATIONS::TestAssimp("Resources\\subDModel.obj", "C:\\Users\\kbald\\Desktop\\subdOrijinalTestAssimp.obj");
	FUSIONCORE::MESHOPERATIONS::LoopSubdivision(subdModel.Meshes[0], 1);
	
	shrub->GetTransformation().ScaleNoTraceBack(glm::vec3(24.0f));
	//FUSIONPHYSICS::MESHOPERATIONS::SmoothObject(subdModel.Meshes[0]);

	/*for (size_t i = 0; i < sofa->Meshes.size(); i++)
	{
		FUSIONPHYSICS::MESHOPERATIONS::LoopSubdivision(sofa->Meshes[i] , 3);
	}*/
	

	

	//FUSIONPHYSICS::MESHOPERATIONS::LoopSubdivision(IMPORTTEST.Meshes[0], 1);

	//FUSIONPHYSICS::MESHOPERATIONS::ExportObj("C:\\Users\\kbald\\Desktop\\TEST2Subd.obj", subdModel);

	//FUSIONPHYSICS::MESHOPERATIONS::LoopSubdivision(grid->Meshes[0], 1);
	//FUSIONPHYSICS::MESHOPERATIONS::LoopSubdivision(grid->Meshes[0], 2);
	//FUSIONPHYSICS::MESHOPERATIONS::LoopSubdivision(grid->Meshes[0], 1);

	/*for (size_t i = 0; i < wall->Meshes.size(); i++)
	{
		wall->Meshes[i].ConstructHalfEdges();
	}*/
	LOG("Duration: " << stopwatch.GetMiliseconds());
#endif // DEBUG
	FUSIONCORE::Material shovelMaterial;
	shovelMaterial.PushTextureMap(TEXTURE_DIFFUSE0, ShovelDiffuse);
	shovelMaterial.PushTextureMap(TEXTURE_NORMAL0, ShovelNormal);
	shovelMaterial.PushTextureMap(TEXTURE_SPECULAR0, ShovelSpecular);

	FUSIONCORE::Material FloorMaterial;
	FloorMaterial.PushTextureMap(TEXTURE_DIFFUSE0, FloorAlbedo);
	FloorMaterial.PushTextureMap(TEXTURE_NORMAL0, FloorNormal);
	FloorMaterial.PushTextureMap(TEXTURE_SPECULAR0, FloorSpecular);
	FloorMaterial.SetTiling(3.0f);

	FUSIONCORE::Material SofaMaterial;
	SofaMaterial.PushTextureMap(TEXTURE_DIFFUSE0, SofaDiffuse);
	SofaMaterial.PushTextureMap(TEXTURE_NORMAL0, SofaNormal);
	SofaMaterial.PushTextureMap(TEXTURE_SPECULAR0, SofaSpecular);

	FUSIONCORE::Material MirrorMaterial;
	MirrorMaterial.PushTextureMap(TEXTURE_DIFFUSE0, StoveDiffuse);
	MirrorMaterial.PushTextureMap(TEXTURE_NORMAL0, StoveNormal);
	MirrorMaterial.PushTextureMap(TEXTURE_SPECULAR0, StoveSpecular);
	MirrorMaterial.PushTextureMap(TEXTURE_METALIC0, StoveMetalic);

	FUSIONCORE::Material WallMaterial;
	WallMaterial.PushTextureMap(TEXTURE_DIFFUSE0, WallDiffuse);
	WallMaterial.PushTextureMap(TEXTURE_NORMAL0, WallNormal);
	WallMaterial.PushTextureMap(TEXTURE_SPECULAR0, WallSpecular);
	WallMaterial.SetTiling(2.0f);

	FUSIONCORE::Material AnimationModelMaterial;
	AnimationModelMaterial.PushTextureMap(TEXTURE_DIFFUSE0, bearDiffuse);
	AnimationModelMaterial.PushTextureMap(TEXTURE_NORMAL0, bearNormal);
	AnimationModelMaterial.PushTextureMap(TEXTURE_SPECULAR0, bearSpecular);

	FUSIONCORE::Material ShrubMaterial;
	ShrubMaterial.PushTextureMap(TEXTURE_DIFFUSE0, ShrubDiffuse);
	ShrubMaterial.PushTextureMap(TEXTURE_NORMAL0, ShrubNormal);
	ShrubMaterial.PushTextureMap(TEXTURE_SPECULAR0, ShrubSpecular);
	ShrubMaterial.PushTextureMap(TEXTURE_ALPHA0, ShrubAlpha);

	model1->GetTransformation().TranslateNoTraceBack({ 0.0f,0.0f,10.0f });
	model1->GetTransformation().ScaleNoTraceBack(glm::vec3(0.15f, 0.15f, 0.15f));
	MainCharac->GetTransformation().ScaleNoTraceBack(glm::vec3(0.15f, 0.15f, 0.15f));
	MainCharac->GetTransformation().RotateNoTraceBack(glm::vec3(0.0f, 1.0f, 0.0f), 90.0f);
	MainCharac->GetTransformation().TranslateNoTraceBack({ 4.0f,1.0f,-10.0f });

	Stove->GetTransformation().ScaleNoTraceBack(glm::vec3(7.0f, 7.0f, 7.0f));
	Stove->GetTransformation().TranslateNoTraceBack({ 0.0f,4.0f,30.0f });
	//Stove->GetTransformation().RotateNoTraceBack(glm::vec3(0.0f, 1.0f, 0.0f), 70.0f);

	wall->GetTransformation().ScaleNoTraceBack(glm::vec3(5.0f, 5.0f, 5.0f));
	wall->GetTransformation().TranslateNoTraceBack({ -60.0f,10.0f,0.0f });
	wall->GetTransformation().RotateNoTraceBack(glm::vec3(0.0f, 0.0f, 1.0f), 90.0f);

	sofa->GetTransformation().ScaleNoTraceBack(glm::vec3(13.0f, 13.0f, 13.0f));
	sofa->GetTransformation().RotateNoTraceBack(glm::vec3(0.0f, 1.0f, 0.0f), 300.0f);
	sofa->GetTransformation().TranslateNoTraceBack({ -10.0f,-1.0f,-20.0f });

	FUSIONPHYSICS::CollisionBoxAABB Box1(model1->GetTransformation(), { 1.0f,1.0f,1.0f });
	FUSIONPHYSICS::CollisionBoxAABB StoveBox(Stove->GetTransformation(), glm::vec3(1.0f));
	FUSIONPHYSICS::CollisionBoxAABB tryBox({ 1.0f,1.0f,1.0f }, { 1.0f,1.0f,1.0f });
	FUSIONPHYSICS::CollisionBoxPlane Plane({ 1.0f,1.0f,1.0f }, { 1.0f,1.0f,1.0f });
	//FUSIONPHYSICS::CollisionBox3DAABB SofaBox(sofa->GetTransformation(), { 0.7f,0.8f,1.0f });
	FUSIONPHYSICS::CollisionBoxAABB SofaBox(sofa->GetTransformation(), glm::vec3(1.0f));
	FUSIONPHYSICS::CollisionBoxPlane Plane2({ 1.0f,1.0f,1.0f }, { 1.0f,1.0f,1.0f });
	FUSIONPHYSICS::CollisionBoxPlane floorBox({ 1.0f,1.0f,1.0f }, { 1.0f,1.0f,1.0f });

	FUSIONCORE::Model IMPORTTEST("Resources\\floor\\grid.obj");
	//FUSIONCORE::MESHOPERATIONS::ImportObj("Resources\\floor\\grid.obj", IMPORTTEST);
	//IMPORTTEST.GetTransformation().Translate({ 7.0f,7.0f,0.0f });
	IMPORTTEST.GetTransformation().ScaleNoTraceBack({ 0.05f,0.05f,0.05f });

	Plane.GetTransformation().Scale({ 7.0f,7.0f ,7.0f });
	Plane.GetTransformation().Translate({ 0.7f,10.0f,0.0f });
	Plane.UpdateAttributes();
	Plane2.GetTransformation().Scale({ 2.0f,2.0f ,2.0f });
	Plane2.GetTransformation().Translate({ 0.7f,0.0f,0.0f });

	floorBox.GetTransformation().Scale(glm::vec3(200.0f));
	floorBox.GetTransformation().Translate({ 0.0f,-1.0f,0.0f });



	//SofaBox.GetTransformation().Translate({ 0.0f,-1.0f,0.0f });

	model1->PushChild(&Box1);
	sofa->PushChild(&SofaBox);
	sofa->UpdateChildren();


	MainCharac->UpdateChildren();

	Stove->PushChild(&StoveBox);
	Stove->UpdateChildren();

	//StoveBox.GetTransformation().ScaleNoTraceBack({ 0.5f,0.8f ,1.0f });
	//StoveBox.GetTransformation().Translate({ 0.4f,-0.2f,-1.3f });

	glm::vec4 BackGroundColor(FUSIONCORE::Color(FF_COLOR_AZURE).GetRGBA());

	Shaders.DeferredPBRshader->use();

	FUSIONCORE::SetEnvironment(*Shaders.DeferredPBRshader, 1.0f, FF_COLOR_CORNFLOWER_BLUE, FF_COLOR_CORNFLOWER_BLUE);
	//FUSIONCORE::SetEnvironmentIBL(*Shaders.DeferredPBRshader, 2.0f, glm::vec3(BackGroundColor.x, BackGroundColor.y, BackGroundColor.z));
	FUSIONCORE::UseShaderProgram(0);

	const double TARGET_FRAME_TIME = 1.0 / TARGET_FPS;
	auto startTimePerSecond = std::chrono::high_resolution_clock::now();
	int fpsCounter = 0;

	glm::vec3 translateVector(0.0f, 0.0f, 0.01f);

	Vec2<int> PrevWindowSize;
	Vec2<int> PrevWindowPos;
	bool IsFullScreen = false;
	float AOamount = 0.5f;

	bool showDebug = false;

	glm::vec3 originalVector(-1.47019e-07, 0, -1);
	glm::vec3 normalizedVector = glm::normalize(originalVector);

	std::cout << "Original Vector: " << Vec3<float>(originalVector) << std::endl;
	std::cout << "Normalized Vector: " << Vec3<float>(normalizedVector) << std::endl;

	std::vector<FUSIONCORE::Model*> models;
	models.push_back(Stove.get());
	models.push_back(sofa.get());
	models.push_back(model1.get());
	models.push_back(grid.get());
	models.push_back(MainCharac.get());
	models.push_back(&IMPORTTEST);
	models.push_back(&subdModel);
	models.push_back(Tower.get());
	models.push_back(shrub.get());
	models.push_back(Rock.get());

	//models.push_back(Rock.get());

	//cubemap.SetCubeMapTexture(ShadowMap0.GetShadowMap());

	std::vector<FUSIONCORE::OmniShadowMap*> shadowMaps;
	//shadowMaps.push_back(&ShadowMap0);
	//shadowMaps.push_back(&ShadowMap1);
	//shadowMaps.push_back(&ShadowMap2);
	//shadowMaps.push_back(&ShadowMap3);

	FUSIONCORE::Model animationModel("Resources\\taunt\\Jumping.fbx", false, true);
	animationModel.GetTransformation().ScaleNoTraceBack({ 0.1f,0.1f,0.1f });
	animationModel.GetTransformation().TranslateNoTraceBack({ 15.0f,-1.0f,0.0f });
	FUSIONCORE::Animation WalkingAnimation("Resources\\taunt\\MutantWalk.fbx", &animationModel);
	FUSIONCORE::Animation IdleAnimation("Resources\\taunt\\OrcIdle.fbx", &animationModel);
	//FUSIONCORE::Animation JumpingAnimation("Resources\\taunt\\Jumping.fbx", &animationModel);
	//FUSIONCORE::Animation RunningAnimation("Resources\\taunt\\Running.fbx", &animationModel);
	FUSIONCORE::Animator animator(&IdleAnimation);

	FUSIONCORE::Model Capsule("Resources\\Sphere.obj");
	FUSIONPHYSICS::CollisionBox Capsule0(Capsule.Meshes[0], Capsule.GetTransformation());
	Capsule0.GetTransformation().ScaleNoTraceBack(glm::vec3(6.0f, 5.5f, 6.0f));
	Capsule0.GetTransformation().TranslateNoTraceBack({ 15.0f,5.0f,0.0f});
	animationModel.PushChild(&Capsule0);
	Capsule0.GetTransformation().TranslateNoTraceBack({ 0.0f,-1.0f,0.0f });
	models.push_back(&animationModel);

	std::vector<FUSIONCORE::Object*> ObjectInstances;
	ObjectInstances.push_back(&Capsule0);
	ObjectInstances.push_back(&TowerBox);
	ObjectInstances.push_back(&SofaBox);
	ObjectInstances.push_back(&StoveBox);
	ObjectInstances.push_back(&Box1);
	ObjectInstances.push_back(&Plane);
	ObjectInstances.push_back(&tryBox);

	ObjectInstances.reserve(RockBoxes.size());
	for (size_t i = 0; i < RockBoxes.size(); i++)
	{
		ObjectInstances.push_back(RockBoxes[i].get());
	}

	FUSIONCORE::VBO instanceVBO;
	auto DistibutedPoints = FUSIONCORE::MESHOPERATIONS::DistributePointsOnMeshSurfaceRandomized(grid->Meshes[0], grid->GetTransformation(), 1000, 118);
	FUSIONCORE::MESHOPERATIONS::FillInstanceDataVBO(instanceVBO, DistibutedPoints);
	shrub->SetInstanced(instanceVBO, DistibutedPoints.size() / 4);
	/*FUSIONCORE::VBO TowerinstanceVBO;
	auto TowerDistibutedPoints = FUSIONCORE::MESHOPERATIONS::DistributePointsOnMeshSurface(grid->Meshes[0], grid->GetTransformation(), 4, 109);
	FUSIONCORE::MESHOPERATIONS::FillInstanceDataVBO(TowerinstanceVBO, TowerDistibutedPoints);*/

	float deltaTime = 0.0f;
	float lastFrame = 0.0f;
	
	glm::vec3 AnimationModelInterpolationDirection = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 AnimationModelDirection = glm::vec3(0.0f, 0.0f, 1.0f);
	bool Moving = false;
	int WalkingSide = -1;
	float IdleWalkingBlendCoeff = 0.0f;
	float InterpolatingBlendCoeff = 0.0f;

	float JumpingBlendCoeff = 0.0f;

	float PreviousZoom = 0.0f;

	camera3d.SetMinMaxZoom(true,-15.0f, 15.0f);
	camera3d.SetZoomSensitivity(3.0f);

	FUSIONPHYSICS::QuadNode headNode;

	FUSIONPHYSICS::ParticleEmitter emitter0(10000,*Shaders.ParticleInitializeShader, 
		glm::vec4(1.0f, 1.0f, 1.0f, 0.9f),
		glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
		glm::vec3(-30.3f,10.0f, -30.3f),
		glm::vec3(30.3f, 70.0f, 30.3f),
		glm::vec3(-0.2f),
		glm::vec3(0.2f, -0.8f,0.2f),
		glm::vec3(-0.5f),
		glm::vec3(0.5f, -3.0f, 0.5f),
		glm::vec3(1.0f),
		1.0f,
		20.0f,
		0.0001f);

	FUSIONCORE::WorldTransform particleTransform;
	particleTransform.Scale(glm::vec3(0.003f));
	particleTransform.Rotate(glm::vec3(1.0f,0.0f,0.0f),90);

	//std::shared_ptr<FILE> screenCaptureFile;

	FUSIONCORE::Model* PixelModel = nullptr;

	while (!FUSIONUTIL::WindowShouldClose(window))
	{
		float currentFrame = FUSIONUTIL::GetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		
		//LOG("FPS: " << 1.0f / deltaTime);

		auto start_time = std::chrono::high_resolution_clock::now();

		tryBox.GetTransformation().Translate({ 0.0f,0.0f,std::sin(time(0)) / 5.0f });
		tryBox.UpdateAttributes();

		Plane.GetTransformation().Rotate({ 0.0f,1.0f,0.0f }, std::sin(time(0)));
		Plane.GetTransformation().Translate({ 0.0f,std::sin(time(0)) / 10.0f,0.0f });
		Plane.UpdateAttributes();


		Stove->UpdateChildren();
		//SofaBox.UpdateAttributes();
		sofa->UpdateChildren();
		Tower->UpdateChildren();

		model1->GetTransformation().Rotate({ 0.0f,1.0f,0.0f }, std::sin(time(0)));
		model1->UpdateChildren();

		animationModel.UpdateChildren();

		bool Collision = false;
		glm::vec3 direction;

		shrub->GetTransformation().RotateNoTraceBack({ 1.0f,0.0f,1.0f}, std::sin(time(0)) * 0.1f);

		FUSIONPHYSICS::UpdateQuadTreeWorldPartitioning(headNode, ObjectInstances,2,5);
		auto UniqueQuadObjects = Capsule0.GetUniqueQuadsObjects();
		glm::vec3 CapsuleQuadCenter = Capsule0.GetAssociatedQuads()[0]->Center;
		emitter0.GetTransformation().Position = { (CapsuleQuadCenter.x + Capsule0.GetTransformation().Position.x) * 0.5f,Capsule0.GetTransformation().Position.y , (CapsuleQuadCenter.z + Capsule0.GetTransformation().Position.z) * 0.5f };

		//LOG("UNIQUE BOX COUNT: " << UniqueQuadObjects.size());
		for (const auto& Box : UniqueQuadObjects)
		{
			auto QuadModel = Box->DynamicObjectCast<FUSIONPHYSICS::CollisionBox*>();
			if (QuadModel != nullptr)
			{
				auto CollisionResponse = FUSIONPHYSICS::IsCollidingSAT(Capsule0,*QuadModel);
				if (CollisionResponse.first)
				{
					Collision = true;
					glm::vec3 ObjectPosition = FUSIONCORE::TranslateVertex(Capsule0.GetTransformation().GetModelMat4(), *Capsule0.GetTransformation().OriginPoint) -
						                       FUSIONCORE::TranslateVertex(QuadModel->GetTransformation().GetModelMat4(), *QuadModel->GetTransformation().OriginPoint);
					direction = glm::normalize(ObjectPosition);
					//direction = CollisionResponse.second;
				}
			}
		}

		static bool FirstFloorTouch = true;
		if (!FUSIONPHYSICS::IsCollidingSAT(floorBox, Capsule0).first)
		{
			if (FirstFloorTouch)
			{
				animationModel.GetTransformation().Translate({ 0.0f,-0.01f,0.0f });
			}
			else
			{
				animationModel.GetTransformation().Translate({ 0.0f,-0.3f,0.0f });
			}
		}
		else
		{
			if (FirstFloorTouch)
			{
				FirstFloorTouch = false;
			}
		}


		auto Front = glm::normalize(glm::vec3(camera3d.Orientation.x,0.0f,camera3d.Orientation.z));
		auto Back = -Front;
		auto Right = glm::normalize(glm::cross(Front, camera3d.GetUpVector()));
		auto Left = -Right;

		//LOG("direction: " << Vec3<float>(direction));
		if (Collision)
		{
			if (glm::dot(-direction, camera3d.GetUpVector()) < 0.0f)
			{
				if (FirstFloorTouch)
				{
					animationModel.GetTransformation().Translate({ 0.0f,0.01f,0.0f });
				}
				else
				{
					animationModel.GetTransformation().Translate({ 0.0f,0.3f,0.0f });
				}
			}
		
			//animationModel.GetTransformation().Translate(glm::normalize(glm::vec3(direction.x, 0.0f, direction.z)) * SPEED * 0.5f * deltaTime);
			direction = -direction;
			const float E = 0.0f;
			/*if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && glm::dot(direction, Front) <= E)
			{
				Moving = true;
				animationModel.GetTransformation().Translate(Front * SPEED * deltaTime);
			}
			else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && glm::dot(direction, Back) <= E)
			{
				Moving = true;
				animationModel.GetTransformation().Translate(Back * SPEED * deltaTime);
			}
			else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS && glm::dot(direction, Right) <= E)
			{
				Moving = true;
				animationModel.GetTransformation().Translate(Right * SPEED * deltaTime);
			}
			else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS && glm::dot(direction, Left) <= E)
			{
				Moving = true;
				animationModel.GetTransformation().Translate(Left * SPEED * deltaTime);
			}
			else
			{
				Moving = false;
			}*/

			if (FUSIONUTIL::GetKey(window, FF_KEY_UP) == FF_GLFW_PRESS && glm::dot(direction, Front) <= E)
			{
				if (WalkingSide != 0)
				{
					AnimationModelInterpolationDirection = AnimationModelDirection;
					InterpolatingBlendCoeff = 0.0f;
				}
				WalkingSide = 0;
				Moving = true;
				AnimationModelDirection = glm::normalize(glm::mix(AnimationModelInterpolationDirection, { Front.x , 0.0f , -Front.z }, InterpolatingBlendCoeff));
				animationModel.GetTransformation().RotationMatrix = glm::lookAt(glm::vec3(0.0f), AnimationModelDirection, camera3d.GetUpVector());
				animationModel.GetTransformation().Translate(Front * SPEED * deltaTime);
			}
			else if (FUSIONUTIL::GetKey(window, FF_KEY_DOWN) == FF_GLFW_PRESS && glm::dot(direction, Back) <= E)
			{
				if (WalkingSide != 1)
				{
					AnimationModelInterpolationDirection = AnimationModelDirection;
					InterpolatingBlendCoeff = 0.0f;
				}
				WalkingSide = 1;
				Moving = true;
				AnimationModelDirection = glm::normalize(glm::mix(AnimationModelInterpolationDirection, { Back.x , 0.0f , -Back.z }, InterpolatingBlendCoeff));
				animationModel.GetTransformation().RotationMatrix = glm::lookAt(glm::vec3(0.0f), AnimationModelDirection, camera3d.GetUpVector());
				animationModel.GetTransformation().Translate(Back * SPEED * deltaTime);
			}
			else if (FUSIONUTIL::GetKey(window, FF_KEY_RIGHT) == FF_GLFW_PRESS && glm::dot(direction, Right) <= E)
			{
				if (WalkingSide != 2)
				{
					AnimationModelInterpolationDirection = AnimationModelDirection;
					InterpolatingBlendCoeff = 0.0f;
				}
				WalkingSide = 2;
				Moving = true;
				AnimationModelDirection = glm::normalize(glm::mix(AnimationModelInterpolationDirection, { Right.x , 0.0f , -Right.z }, InterpolatingBlendCoeff));
				animationModel.GetTransformation().RotationMatrix = glm::lookAt(glm::vec3(0.0f), AnimationModelDirection, camera3d.GetUpVector());
				animationModel.GetTransformation().Translate(Right * SPEED * deltaTime);
			}
			else if (FUSIONUTIL::GetKey(window, FF_KEY_LEFT) == FF_GLFW_PRESS && glm::dot(direction, Left) <= E)
			{
				if (WalkingSide != 3)
				{
					AnimationModelInterpolationDirection = AnimationModelDirection;
					InterpolatingBlendCoeff = 0.0f;
				}
				WalkingSide = 3;
				Moving = true;
				AnimationModelDirection = glm::normalize(glm::mix(AnimationModelInterpolationDirection, { Left.x , 0.0f , -Left.z }, InterpolatingBlendCoeff));
				animationModel.GetTransformation().RotationMatrix = glm::lookAt(glm::vec3(0.0f), AnimationModelDirection, camera3d.GetUpVector());
				animationModel.GetTransformation().Translate(Left * SPEED * deltaTime);
			}
			else
			{
				Moving = false;
			}

		}
		else
		{
			if (FUSIONUTIL::GetKey(window, FF_KEY_UP) == FF_GLFW_PRESS)
			{
				if (WalkingSide != 0)
				{
					AnimationModelInterpolationDirection = AnimationModelDirection;
					InterpolatingBlendCoeff = 0.0f;
				}
				WalkingSide = 0;
				Moving = true;
				AnimationModelDirection = glm::normalize(glm::mix(AnimationModelInterpolationDirection, { Front.x , 0.0f , -Front.z }, InterpolatingBlendCoeff));
				animationModel.GetTransformation().RotationMatrix = glm::lookAt(glm::vec3(0.0f), AnimationModelDirection, camera3d.GetUpVector());
				animationModel.GetTransformation().Translate(Front * SPEED * deltaTime);
			}
			else if (FUSIONUTIL::GetKey(window, FF_KEY_DOWN) == FF_GLFW_PRESS)
			{
				if (WalkingSide != 1)
				{
					AnimationModelInterpolationDirection = AnimationModelDirection;
					InterpolatingBlendCoeff = 0.0f;
				}
				WalkingSide = 1;
				Moving = true;
				AnimationModelDirection = glm::normalize(glm::mix(AnimationModelInterpolationDirection, { Back.x , 0.0f , -Back.z }, InterpolatingBlendCoeff));
				animationModel.GetTransformation().RotationMatrix = glm::lookAt(glm::vec3(0.0f), AnimationModelDirection, camera3d.GetUpVector());
				animationModel.GetTransformation().Translate(Back * SPEED * deltaTime);
			}
			else if (FUSIONUTIL::GetKey(window, FF_KEY_RIGHT) == FF_GLFW_PRESS)
			{
				if (WalkingSide != 2)
				{
					AnimationModelInterpolationDirection = AnimationModelDirection;
					InterpolatingBlendCoeff = 0.0f;
				}
				WalkingSide = 2;
				Moving = true;
				AnimationModelDirection = glm::normalize(glm::mix(AnimationModelInterpolationDirection, { Right.x , 0.0f , -Right.z }, InterpolatingBlendCoeff));
				animationModel.GetTransformation().RotationMatrix = glm::lookAt(glm::vec3(0.0f), AnimationModelDirection, camera3d.GetUpVector());
				animationModel.GetTransformation().Translate(Right * SPEED * deltaTime);
			}
			else if (FUSIONUTIL::GetKey(window, FF_KEY_LEFT) == FF_GLFW_PRESS)
			{
				if (WalkingSide != 3)
				{
					AnimationModelInterpolationDirection = AnimationModelDirection;
					InterpolatingBlendCoeff = 0.0f;
				}
				WalkingSide = 3;
				Moving = true;
				AnimationModelDirection = glm::normalize(glm::mix(AnimationModelInterpolationDirection, { Left.x , 0.0f , -Left.z }, InterpolatingBlendCoeff));
				animationModel.GetTransformation().RotationMatrix = glm::lookAt(glm::vec3(0.0f), AnimationModelDirection, camera3d.GetUpVector());
				animationModel.GetTransformation().Translate(Left * SPEED * deltaTime);
			}
			else
			{
				Moving = false;
			}
		}

		
		if (Moving)
		{
			IdleWalkingBlendCoeff += BlendAmount;
			InterpolatingBlendCoeff += InterBlendAmount;
		}
		else
		{
			if (IdleWalkingBlendCoeff > 0.0f)
			{
				IdleWalkingBlendCoeff -= BlendAmount;
			}
			if (InterpolatingBlendCoeff > 0.0f)
			{
				InterpolatingBlendCoeff -= InterBlendAmount;
			}
		}
		IdleWalkingBlendCoeff = glm::clamp(IdleWalkingBlendCoeff, 0.0f, 1.0f);
		InterpolatingBlendCoeff = glm::clamp(InterpolatingBlendCoeff, 0.0f, 1.0f);


		static bool AllowJump = false;
		static bool AllowReset = true;
		if (!AllowReset && stopwatch.GetSeconds() >= 2.0f)
		{
			AllowJump = false;
			AllowReset = true;
		}
		if (FUSIONUTIL::GetKey(window, FF_KEY_SPACE) == FF_GLFW_PRESS && !AllowJump && AllowReset)
		{
			AllowJump = true;
			AllowReset = false;
		}
		if (AllowReset)
		{
			stopwatch.Reset();
		}
		if (AllowJump)
		{
			if (stopwatch.GetSeconds() >= 1.0f)
			{
				AllowJump = false;
			}
			animationModel.GetTransformation().Translate(camera3d.GetUpVector() * 0.5f);
		}
		
		animator.UpdateBlendedAnimation(&IdleAnimation, &WalkingAnimation, IdleWalkingBlendCoeff, deltaTime);

		/*static bool AllowPressF = true;
		if (!AllowPressF && FUSIONUTIL::GetKey(window, FF_KEY_F) == FF_GLFW_RELEASE)
		{
			AllowPressF = true;
		}
		if (FUSIONUTIL::GetKey(window, FF_KEY_F) == FF_GLFW_PRESS && AllowPressF)
		{
			AllowPressF = false;
			IsFullScreen = !IsFullScreen;
			if (!IsFullScreen)
			{
				glfwGetWindowPos(window, &PrevWindowPos.x, &PrevWindowPos.y);
				PrevWindowSize(WindowSize);
				const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
				glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, mode->refreshRate);
			}
			else
			{
				const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
				glfwSetWindowMonitor(window, NULL, PrevWindowPos.x, PrevWindowPos.y, PrevWindowSize.x, PrevWindowSize.y, mode->refreshRate);
			}

		}*/

		//glfwGetWindowSize(window, &WindowSize.x, &WindowSize.y);
		FUSIONUTIL::GetWindowSize(window, WindowSize.x, WindowSize.y);
		/*if (direction != glm::vec3(0.0f))
		{
		  camera3d.Orientation = direction;
		}*/
		camera3d.UpdateCameraMatrix(90.0f, (float)WindowSize.x / (float)WindowSize.y, CAMERA_CLOSE_PLANE, CAMERA_FAR_PLANE, WindowSize);
		camera3d.SetTarget(&animationModel, 30.0f, { 0.0f,10.0f,0.0f });
		camera3d.HandleInputs(window, WindowSize, FF_CAMERA_LAYOUT_INDUSTRY_STANDARD, 0.06f);

		camera2d.UpdateCameraMatrix({ 0.0f,0.0f,0.0f }, 1.0f, WindowSize);
		
		//camera3d.UpdateCameraClusters(*Shaders.CameraClusterComputeShader, *Shaders.CameraLightCullingComputeShader);
		/*glm::vec4 SomePoint = glm::vec4(-0.0f, 0.0f, -20.1f, 1.0f);
		SomePoint = glm::inverse(camera3d.viewMat) * SomePoint;
		Isaac.GetTransformation().TranslationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(SomePoint));*/
		
		emitter0.UpdateParticleEmitter(*Shaders.ParticleUpdateComputeShader, *Shaders.ParticleSpawnComputeShader, deltaTime);

		//ShadowMap0.Draw(*Shaders.OmniShadowMapShader, Lights[0], models, camera3d);
		//ShadowMap1.Draw(*Shaders.OmniShadowMapShader, Lights[1], models, camera3d);
		//ShadowMap2.Draw(*Shaders.OmniShadowMapShader, Lights[2], models, camera3d);
		//ShadowMap3.Draw(*Shaders.OmniShadowMapShader, Lights[3], models, camera3d);

		sunShadowMap.Draw(Shaders, camera3d, models,Sun);

		Gbuffer.Bind();
		FUSIONUTIL::GLClearColor(glm::vec4(0.0f));
		//glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		FUSIONUTIL::GLClear(FF_CLEAR_BUFFER_BIT_GL_COLOR_BUFFER_BIT | FF_CLEAR_BUFFER_BIT_GL_DEPTH_BUFFER_BIT);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//glEnable(GL_DEPTH_TEST);
		FUSIONUTIL::EnableDepthTest();

		//glViewport(0, 0, WindowSize.x, WindowSize.y);
		FUSIONUTIL::GLviewport(0, 0, WindowSize.x, WindowSize.y);

		glm::vec2 PrevMousePos = glm::vec2(mousePos.x, mousePos.y);
		//glfwGetCursorPos(window, &mousePos.x, &mousePos.y);
		FUSIONUTIL::GetCursorPosition(window, mousePos.x, mousePos.y);
		glm::vec2 CurrentMousePos = glm::vec2(mousePos.x, mousePos.y);
		
		std::function<void()> shaderPrepe = [&]() {};
		std::function<void()> shaderPrepe1 = [&]() {};
		std::function<void()> shaderPrepe2 = [&]() {};

		auto animationMatrices = animator.GetFinalBoneMatrices();
		//IMPORTTEST.Draw(camera3d, *Shaders.GbufferShader, shaderPrepe, cubemap, shovelMaterial, shadowMaps, AOamount);
		//shrub->DrawDeferredInstanced(camera3d, *Shaders.InstancedGbufferShader, shaderPrepe, ShrubMaterial,instanceVBO,DistibutedPoints.size());
		shrub->DrawDeferredInstancedImportedMaterial(camera3d, *Shaders.InstancedGbufferShader, shaderPrepe, instanceVBO, DistibutedPoints.size());
		//if (FUSIONCORE::IsModelInsideCameraFrustum(*model1, camera3d))
		
		{
			Rock->DrawDeferredInstancedImportedMaterial(camera3d, *Shaders.InstancedGbufferShader, shaderPrepe, RockInstanceVBO, RockDistibutedPoints.size());
		}
		{
			model1->DrawDeferred(camera3d, *Shaders.GbufferShader, shaderPrepe, shovelMaterial);
		}
		MainCharac->DrawDeferred(camera3d, *Shaders.GbufferShader, shaderPrepe, shovelMaterial);
		grid->DrawDeferred(camera3d, *Shaders.GbufferShader, shaderPrepe, FloorMaterial);

		if (FUSIONCORE::IsModelInsideCameraFrustumSphere(*Stove, camera3d, 0.3f))
		{
			Stove->DrawDeferred(camera3d, *Shaders.GbufferShader, shaderPrepe, MirrorMaterial);
			//Stove->DrawDeferredIndirect(camera3d, *Shaders.GbufferShader, shaderPrepe, MirrorMaterial);

		}
	
		for (size_t i = 0; i < ImportedModels.size(); i++)
		{
			ImportedModels[i]->DrawDeferredImportedMaterial(camera3d, *Shaders.GbufferShader, shaderPrepe);
		}

		if (FUSIONCORE::IsModelInsideCameraFrustumSphere(*Tower, camera3d, 0.3f))
		{
			Tower->DrawDeferredImportedMaterial(camera3d, *Shaders.GbufferShader, shaderPrepe);

		}
		Isaac.DrawDeferredImportedMaterial(camera3d, *Shaders.GbufferShader, shaderPrepe);

	    animationModel.DrawDeferred(camera3d, *Shaders.GbufferShader, shaderPrepe, AnimationModelMaterial, animationMatrices);
		//if (FUSIONCORE::IsObjectQuadInsideCameraFrustum(RockBox, camera3d))
		

		//Terrain.DrawDeferredImportedMaterial(camera3d, *Shaders.GbufferShader, shaderPrepe, AOamount);
		wall->DrawDeferred(camera3d, *Shaders.GbufferShader, shaderPrepe, WallMaterial);

		//if (!Collision)
		//{
		//}
		
		
		FUSIONCORE::Material redMaterial(0.3f, 0.6f, { 1.0f,0.0f,0.0f,1.0f });
		subdModel.DrawDeferred(camera3d, *Shaders.GbufferShader, shaderPrepe, redMaterial);
		if (FUSIONCORE::IsModelInsideCameraFrustumSphere(*sofa, camera3d , 0.3f))
		{
			sofa->DrawDeferred(camera3d, *Shaders.GbufferShader, shaderPrepe, SofaMaterial);
		}
		Capsule.DrawDeferred(camera3d, *Shaders.GbufferShader, shaderPrepe, FUSIONCORE::Material());
		
		Gbuffer.Unbind();
		ScreenFrameBuffer.Bind();
		FUSIONUTIL::GLClearColor(glm::vec4(0.0f,0.0f,0.0f,1.0f));
		FUSIONUTIL::GLClear(FF_CLEAR_BUFFER_BIT_GL_COLOR_BUFFER_BIT | FF_CLEAR_BUFFER_BIT_GL_DEPTH_BUFFER_BIT);
		//Gbuffer.DrawSSR(camera3d, *Shaders.SSRshader, [&]() {}, WindowSize);
		Gbuffer.Draw(camera3d, *Shaders.DeferredPBRshader, [&]() {}, WindowSize, shadowMaps,sunShadowMap, cubemap,FF_COLOR_VOID, 0.3f);

		if (FUSIONUTIL::GetMouseKey(window, FF_GLFW_MOUSE_BUTTON_RIGHT) == FF_GLFW_PRESS)
		{
			auto Pixel = FUSIONCORE::ReadFrameBufferPixel(mousePos.x, mousePos.y, FF_FRAMEBUFFER_MODEL_ID_IMAGE_ATTACHMENT, FF_PIXEL_FORMAT_GL_RED, { WindowSize.x, WindowSize.y });
			PixelModel = FUSIONCORE::GetModel(Pixel.GetRed());
		}

		//glViewport(0, 0, WindowSize.x, WindowSize.y);
		FUSIONUTIL::GLviewport(0, 0, WindowSize.x, WindowSize.y);

		auto gbufferSize = Gbuffer.GetFBOSize();
		FUSIONCORE::CopyDepthInfoFBOtoFBO(Gbuffer.GetFBO(), { gbufferSize.x ,gbufferSize.y }, ScreenFrameBuffer.GetFBO());
		ScreenFrameBuffer.Bind();

		emitter0.DrawParticles(*Shaders.ParticleRenderShader, grid->Meshes[0], particleTransform, camera3d);
		cubemap.Draw(camera3d, WindowSize.Cast<float>());



		if (PixelModel)
		{
			if (FUSIONUTIL::GetMouseKey(window, FF_GLFW_MOUSE_BUTTON_LEFT) == FF_GLFW_PRESS)
			{
			   glm::vec4 PrevMouseWorldPos = glm::inverse(camera3d.viewMat) * glm::vec4(PrevMousePos.x, PrevMousePos.y,0.0f, 1.0f);
			   glm::vec4 CurrentMouseWorldPos = glm::inverse(camera3d.viewMat) * glm::vec4(CurrentMousePos.x, CurrentMousePos.y,0.0f, 1.0f);
			   glm::vec3 DeltaMouse = CurrentMouseWorldPos - PrevMouseWorldPos;
			   PixelModel->GetTransformation().Translate(glm::vec3(DeltaMouse.x,-DeltaMouse.y,DeltaMouse.z) / 10.0f);
			}
			
			FUSIONUTIL::GLPolygonMode(FF_CULL_FACE_MODE_GL_FRONT_AND_BACK, FF_GL_LINE);
			//glPolygonMode(FF_CULL_FACE_MODE_GL_FRONT_AND_BACK, FF_GL_LINE);
			PixelModel->DrawDeferred(camera3d, *Shaders.LightShader, shaderPrepe, FUSIONCORE::Material());
			FUSIONUTIL::GLPolygonMode(FF_CULL_FACE_MODE_GL_FRONT_AND_BACK, FF_GL_FILL);
			//glPolygonMode(FF_CULL_FACE_MODE_GL_FRONT_AND_BACK, FF_GL_FILL);
		}
#ifdef ENGINE_DEBUG

		static bool AllowD = true;
		if (FUSIONUTIL::IsKeyPressedOnce(window, FF_KEY_D, AllowD))
		{
			showDebug = !showDebug;
		}
		if (showDebug)
		{
			for (size_t i = 0; i < Lights.size(); i++)
			{
				Lights[i].Draw(camera3d, *Shaders.LightShader);
			}
			/*Capsule0.DrawBoxMesh(camera3d, *Shaders.LightShader);
			Box1.DrawBoxMesh(camera3d, *Shaders.LightShader);
			SofaBox.DrawBoxMesh(camera3d, *Shaders.LightShader);
			StoveBox.DrawBoxMesh(camera3d, *Shaders.LightShader);
			tryBox.DrawBoxMesh(camera3d, *Shaders.LightShader);
			Plane.DrawBoxMesh(camera3d, *Shaders.LightShader);
			Plane2.DrawBoxMesh(camera3d, *Shaders.LightShader);
			floorBox.DrawBoxMesh(camera3d, *Shaders.LightShader);*/

			for (auto& Box : ObjectInstances)
			{
				Box->DynamicObjectCast<FUSIONPHYSICS::CollisionBox*>()->DrawBoxMesh(camera3d, *Shaders.LightShader);
			}
			/*for (auto& Box : TerrainCollisionBoxes)
			{
				Box.DrawBoxMesh(camera3d, *Shaders.LightShader);
			}*/

			//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			FUSIONUTIL::GLPolygonMode(FF_CULL_FACE_MODE_GL_FRONT_AND_BACK, FF_GL_LINE);
			subdModel.DrawDeferred(camera3d, *Shaders.GbufferShader, shaderPrepe, FUSIONCORE::Material());
			FUSIONUTIL::GLPolygonMode(FF_CULL_FACE_MODE_GL_FRONT_AND_BACK, FF_GL_FILL);
			//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			FUSIONPHYSICS::VisualizeQuadTree(headNode, camera3d, *Shaders.LightShader, FF_COLOR_RED);
		}
#endif

		//glDisable(GL_DEPTH_TEST);
			

		//FUSIONCORE::SHAPES::DrawRectangleTextured(bearNormal, {0.0f,1.0f}, {1.0f,0.1f}, 0.0f, camera2d, *Shaders.ShapeTexturedShader);
		
		
	    //FUSIONCORE::SHAPES::DrawRectangle(glm::vec4(glm::vec3(FF_COLOR_CHARCOAL), 0.5f), { 0.0f,1.0f }, { 3.0f,0.1f }, 0.0f, camera2d, Shaders);
		
		
		//FUSIONCORE::SHAPES::DrawRectangle(FF_COLOR_BLUSH_PINK, { 0.5f,0.0f }, { 0.5f,0.5f }, 0.0f, camera2d,Shaders);
		//FUSIONCORE::SHAPES::DrawTriangle(FF_COLOR_AQUAMARINE, { 0.0f,0.0f }, { 0.5f,0.5f }, 0.0f, camera2d, *Shaders.ShapeBasicShader);
		//FUSIONCORE::SHAPES::DrawTriangleTextured(bearNormal, { 0.0f,0.0f }, { 0.5f,0.5f }, 0.0f, camera2d, *Shaders.ShapeTexturedShader);
		//FUSIONCORE::SHAPES::DrawHexagon(FF_COLOR_AMETHYST, { 0.0f,0.0f }, { 0.5f,0.5f }, 0.0f, camera2d, Shaders);
		//FUSIONCORE::SHAPES::DrawHexagonTextured(FloorAlbedo, { 0.0f,0.0f }, { 0.5f,0.5f }, 0.0f, camera2d,Shaders);
		//glEnable(GL_DEPTH_TEST);

		ScreenFrameBuffer.Unbind();
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		FUSIONUTIL::GLBindFrameBuffer(FF_GL_FRAMEBUFFER, 0);
		ScreenFrameBuffer.Draw(camera3d, *Shaders.FBOShader, [&]() {}, WindowSize,true,0.7f,0.1f, 5.0f,1.7f,1.6f);

		
		//glfwPollEvents();
		FUSIONUTIL::PollEvents();
		//glfwSwapBuffers(window);
		FUSIONUTIL::SwapBuffers(window);

		auto end_time = std::chrono::high_resolution_clock::now();
		auto elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count() / 1e6;

		if (elapsed_time < TARGET_FRAME_TIME) {

			std::this_thread::sleep_for(std::chrono::microseconds(static_cast<long long>((TARGET_FRAME_TIME - elapsed_time) * 1e6)));
		}

		fpsCounter++;
		auto overAllElapsedPerSecond = std::chrono::duration_cast<std::chrono::seconds>(end_time - startTimePerSecond).count();

		if (overAllElapsedPerSecond >= 1.0)
		{
			double fps = fpsCounter / overAllElapsedPerSecond;
			fpsCounter = 0;
			startTimePerSecond = std::chrono::high_resolution_clock::now();
		}

		if (!Moving)
		{
			AnimationModelInterpolationDirection = AnimationModelDirection;
			InterpolatingBlendCoeff = 0.0f;
		}

	}

	FUSIONUTIL::DisposeDefaultShaders(Shaders);

	ScreenFrameBuffer.clean();
	Gbuffer.clean();
	
	shovelMaterial.Clean();
	FloorMaterial.Clean();
	SofaMaterial.Clean();
	MirrorMaterial.Clean();
	WallMaterial.Clean();
	AnimationModelMaterial.Clean();
	ShrubMaterial.Clean();

	//glfwTerminate();

	//LOG_INF("Window terminated!");
	FUSIONUTIL::TerminateWindow();
	return 0;
}

float Application::RoundNonZeroToOne(float input)
{
	float result = 0.0f;
	if (glm::abs(input) > glm::epsilon<float>())
	{
		if (input < 0.0f)
		{
			result = -1.0f;
		}
		else if (input > 0.0f)
		{
			result = 1.0f;
		}
	}
	return result;
}





