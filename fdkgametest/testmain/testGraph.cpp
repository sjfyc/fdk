#include <fdkgame/Graph.h>

void testGraph()
{
	using namespace fdkgame;
	typedef Graph<int, int> Graph;
	typedef Graph::NodeType Node;
	typedef Graph::EdgeType Edge;
	Graph g;
	for (size_t i = 0; i < 10; ++i)
	{
		g.addNode(i, 2*i);
	}
	FDK_ASSERT(g.getNodes().size() == 10);
	for (size_t i = 0; i < 9; ++i)
	{
		g.addOutEdge(g.getNode(i), g.getNode(i+1), 3*i);
	}
	for (size_t i = 0; i < 9; ++i)
	{
		FDK_ASSERT(g.getNode(i).getOutEdge(i+1).getTargetNodeID() == i+1);
		FDK_ASSERT(g.getNode(i).getOutEdge(i+1).getInfo() == 3*i);
	}
	FDK_ASSERT(g.getNode(2).getOutEdges().size() == 1);
	g.getNode(2).removeOutEdge(g.getNode(2).getOutEdge(3));
	FDK_ASSERT(g.getNode(2).getOutEdges().empty());
	g.removeNode(g.getNode(0));
	FDK_ASSERT(!g.findNode(0));
}
