#include "Skeleton.h"
// #include "uMatrixPalette.h"
#include <fstream>
#include <sstream>
#include "include/rapidjson/document.h"
#include <SDL2/SDL_log.h>

bool Skeleton::Load(const std::string& fileName)
{
	std::ifstream file(fileName);
	if (!file.is_open())
	{
		SDL_Log("File not found: Skeleton %s", fileName.c_str());
		return false;
	}

	std::stringstream fileStream;
	fileStream << file.rdbuf();
	std::string contents = fileStream.str();
	rapidjson::StringStream jsonStr(contents.c_str());
	rapidjson::Document doc;
	doc.ParseStream(jsonStr);

	if (!doc.IsObject())
	{
		SDL_Log("Skeleton %s is not valid json", fileName.c_str());
		return false;
	}

	int ver = doc["version"].GetInt();

	// Check the metadata
	if (ver != 1)
	{
		SDL_Log("Skeleton %s unknown format", fileName.c_str());
		return false;
	}

	const rapidjson::Value& bonecount = doc["bonecount"];
	if (!bonecount.IsUint())
	{
		SDL_Log("Skeleton %s doesn't have a bone count.", fileName.c_str());
		return false;
	}

	size_t count = bonecount.GetUint();

	if (count > MAX_SKELETON_BONES)
	{
		SDL_Log("Skeleton %s exceeds maximum bone count.", fileName.c_str());
		return false;
	}

	m_Bones.reserve(count);

	const rapidjson::Value& bones = doc["bones"];
	if (!bones.IsArray())
	{
		SDL_Log("Skeleton %s doesn't have a bone array?", fileName.c_str());
		return false;
	}

	if (bones.Size() != count)
	{
		SDL_Log("Skeleton %s has a mismatch between the bone count and number of bones", fileName.c_str());
		return false;
	}

	Bone temp;

	for (rapidjson::SizeType i = 0; i < count; i++)
	{
		if (!bones[i].IsObject())
		{
			SDL_Log("Skeleton %s: Bone %d is invalid.", fileName.c_str(), i);
			return false;
		}

		const rapidjson::Value& name = bones[i]["name"];
		temp.m_Name = name.GetString();

		const rapidjson::Value& parent = bones[i]["parent"];
		temp.m_Parent = parent.GetInt();

		const rapidjson::Value& bindpose = bones[i]["bindpose"];
		if (!bindpose.IsObject())
		{
			SDL_Log("Skeleton %s: Bone %d is invalid.", fileName.c_str(), i);
			return false;
		}

		const rapidjson::Value& rot = bindpose["rot"];
		const rapidjson::Value& trans = bindpose["trans"];

		if (!rot.IsArray() || !trans.IsArray())
		{
			SDL_Log("Skeleton %s: Bone %d is invalid.", fileName.c_str(), i);
			return false;
		}

		temp.m_LocalBindPose.m_Rotation.x = rot[0].GetDouble();
		temp.m_LocalBindPose.m_Rotation.y = rot[1].GetDouble();
		temp.m_LocalBindPose.m_Rotation.z = rot[2].GetDouble();
		temp.m_LocalBindPose.m_Rotation.w = rot[3].GetDouble();

		temp.m_LocalBindPose.m_Translation.x = trans[0].GetDouble();
		temp.m_LocalBindPose.m_Translation.y = trans[1].GetDouble();
		temp.m_LocalBindPose.m_Translation.z = trans[2].GetDouble();

		m_Bones.emplace_back(temp);
	}

	// Now that we have the bones
	ComputeGlobalInvBindPose();

	return true;
}

// getters
size_t Skeleton::GetNumBones() const { return m_Bones.size(); }
const Bone& Skeleton::GetBone(size_t idx) const { return m_Bones[idx]; }
const std::vector<Bone>& Skeleton::GetBones() const { return m_Bones; }
const std::vector<Matrix4>& Skeleton::GetGlobalInvBindPoses() const { return m_GlobalInvBindPoses; }

// computes the global inverse bind pose for each bone
// called when loading the skeleton
void Skeleton::ComputeGlobalInvBindPose()
{
  // resize to number of bones which automatically fills identity
  m_GlobalInvBindPoses.resize(GetNumBones());

  // step 1: Compute global bind pose for each bone
  // global bind pose for root is just local bind pose
  m_GlobalInvBindPoses[0] = m_Bones[0].m_LocalBindPose.ToMatrix();

  // each remaining bone's global bind pose is its local pose
  // multiplied by its parent's global bind pose
  for(size_t i = 1; i < m_GlobalInvBindPoses.size(); ++i)
  {
    Matrix4 localMat = m_Bones[i].m_LocalBindPose.ToMatrix();
    m_GlobalInvBindPoses[i] = localMat * m_GlobalInvBindPoses[m_Bones[i].m_Parent];
  }

  // setp 2: invert each matrix
  for(size_t i = 0; i < m_GlobalInvBindPoses.size(); ++i)
  {
    m_GlobalInvBindPoses[i].Invert();
  }
}
