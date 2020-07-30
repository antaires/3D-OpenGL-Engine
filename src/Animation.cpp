#include "Animation.h"
#include "Skeleton.h"
#include <fstream>
#include <sstream>
#include "include/rapidjson/document.h"
#include <SDL2/SDL_log.h>

bool Animation::Load(const std::string& fileName)
{
	std::ifstream file(fileName);
	if (!file.is_open())
	{
		SDL_Log("File not found: Animation %s", fileName.c_str());
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
		SDL_Log("Animation %s is not valid json", fileName.c_str());
		return false;
	}

	int ver = doc["version"].GetInt();

	// Check the metadata
	if (ver != 1)
	{
		SDL_Log("Animation %s unknown format", fileName.c_str());
		return false;
	}

	const rapidjson::Value& sequence = doc["sequence"];
	if (!sequence.IsObject())
	{
		SDL_Log("Animation %s doesn't have a sequence.", fileName.c_str());
		return false;
	}

	const rapidjson::Value& frames = sequence["frames"];
	const rapidjson::Value& length = sequence["length"];
	const rapidjson::Value& bonecount = sequence["bonecount"];

	if (!frames.IsUint() || !length.IsDouble() || !bonecount.IsUint())
	{
		SDL_Log("Sequence %s has invalid frames, length, or bone count.", fileName.c_str());
		return false;
	}

	m_NumFrames = frames.GetUint();
	m_Duration = length.GetDouble();
	m_NumBones = bonecount.GetUint();
	m_FrameDuration = m_Duration / (m_NumFrames - 1);

	m_Tracks.resize(m_NumBones);

	const rapidjson::Value& tracks = sequence["tracks"];

	if (!tracks.IsArray())
	{
		SDL_Log("Sequence %s missing a tracks array.", fileName.c_str());
		return false;
	}

	for (rapidjson::SizeType i = 0; i < tracks.Size(); i++)
	{
		if (!tracks[i].IsObject())
		{
			SDL_Log("Animation %s: Track element %d is invalid.", fileName.c_str(), i);
			return false;
		}

		size_t boneIndex = tracks[i]["bone"].GetUint();

		const rapidjson::Value& transforms = tracks[i]["transforms"];
		if (!transforms.IsArray())
		{
			SDL_Log("Animation %s: Track element %d is missing transforms.", fileName.c_str(), i);
			return false;
		}

		BoneTransform temp;

		if (transforms.Size() < m_NumFrames)
		{
			SDL_Log("Animation %s: Track element %d has fewer frames than expected.", fileName.c_str(), i);
			return false;
		}

		for (rapidjson::SizeType j = 0; j < transforms.Size(); j++)
		{
			const rapidjson::Value& rot = transforms[j]["rot"];
			const rapidjson::Value& trans = transforms[j]["trans"];

			if (!rot.IsArray() || !trans.IsArray())
			{
				SDL_Log("Skeleton %s: Bone %d is invalid.", fileName.c_str(), i);
				return false;
			}

			temp.m_Rotation.x = rot[0].GetDouble();
			temp.m_Rotation.y = rot[1].GetDouble();
			temp.m_Rotation.z = rot[2].GetDouble();
			temp.m_Rotation.w = rot[3].GetDouble();

			temp.m_Translation.x = trans[0].GetDouble();
			temp.m_Translation.y = trans[1].GetDouble();
			temp.m_Translation.z = trans[2].GetDouble();

			m_Tracks[boneIndex].emplace_back(temp);
		}
	}

	return true;
}

// getters
size_t Animation::GetNumBones() const { return m_NumBones; }
float Animation::GetDuration() const { return m_Duration; }
float Animation::GetFrameDuration() const { return m_FrameDuration; }

// fills the provided vector with the global (current) pose matrices
// for each bone at the specified time in the animation
void Animation::GetGlobalPoseAtTime(std::vector<Matrix4>& outPoses
  , const class Skeleton* inSkeleton, float inTime) const
  {
  	if (outPoses.size() != m_NumBones)
  	{
  		outPoses.resize(m_NumBones);
  	}

  	// Figure out the current frame index and next frame
  	// (This assumes inTime is bounded by [0, AnimDuration]
  	size_t frame = static_cast<size_t>(inTime / m_FrameDuration);
  	size_t nextFrame = frame + 1;
  	// Calculate fractional value between frame and next frame
  	float pct = inTime / m_FrameDuration - frame;

  	// Setup the pose for the root
  	if (m_Tracks[0].size() > 0)
  	{
  		// Interpolate between the current frame's pose and the next frame
  		BoneTransform interp = BoneTransform::Interpolate(m_Tracks[0][frame],
  			m_Tracks[0][nextFrame], pct);
  		outPoses[0] = interp.ToMatrix();
  	}
  	else
  	{
  		outPoses[0] = Matrix4::Identity;
  	}

  	const std::vector<Skeleton::Bone>& bones = inSkeleton->GetBones();
  	// Now setup the poses for the rest
  	for (size_t bone = 1; bone < m_NumBones; bone++)
  	{
  		Matrix4 localMat; // (Defaults to identity)
  		if (m_Tracks[bone].size() > 0)
  		{
  			BoneTransform interp = BoneTransform::Interpolate(m_Tracks[bone][frame],
  				m_Tracks[bone][nextFrame], pct);
  			localMat = interp.ToMatrix();
  		}

  		outPoses[bone] = localMat * outPoses[bones[bone].m_Parent];
  	}
  }
