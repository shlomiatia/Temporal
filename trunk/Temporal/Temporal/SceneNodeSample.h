#ifndef SCENENODESAMPLE_H
#define SCENENODESAMPLE_H

#include "NumericPair.h"
#include "Hash.h"

namespace Temporal
{
	class SceneNodeSample
	{
	public:
		SceneNodeSample(const Hash& id, const Vector& translation, float rotation) : _id(id), _translation(translation), _rotation(rotation) {}
	private:
		const Hash _id;
		const Vector _translation;
		const float _rotation;

		SceneNodeSample(const SceneNodeSample&);
		SceneNodeSample& operator=(const SceneNodeSample&);
	};
}
#endif