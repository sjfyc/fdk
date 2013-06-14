#ifndef __FDKGAME_FINDPATH_FINDPATHALGO_H_INCLUDE__
#define __FDKGAME_FINDPATH_FINDPATHALGO_H_INCLUDE__
#include <fdkgame/Base.h>
#include <vector>

namespace fdk { namespace game 
{
	class FindPathAlgo
	{
	public:
		virtual ~FindPathAlgo() {}
		virtual bool findPath(const FindPathEnv& env, int startNodeID, int targetNodeID) = 0;
		virtual const vector<int>& getPath() const = 0;
	};
}}

#endif
