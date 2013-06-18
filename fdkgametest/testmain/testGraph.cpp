#include <fdkgame/Graph.h>

void testGraph()
{	
	using namespace fdkgame;
	struct NodeInfo
	{
		NodeInfo(int v) : value(v) {}
		int value;
	};
	struct EdgeInfo
	{
		EdgeInfo(int v) : value(v) {}
		int value;
	};
	typedef Graph<NodeInfo, EdgeInfo> Graph;
	typedef Graph::NodeType Node;
	typedef Graph::EdgeType Edge;
	Graph g;
	g.addNode(0, NodeInfo(1));
	g.addNode(1, NodeInfo(2));
	FDK_ASSERT(g.getNode(0).getID() == 0);
	FDK_ASSERT(g.getNode(0).getInfo().value == 1);
	FDK_ASSERT(g.getNode(1).getID() == 1);
	FDK_ASSERT(g.getNode(1).getInfo().value == 2);
	g.getNode(1).getInfo().value = 3;
	FDK_ASSERT(g.getNode(1).getInfo().value == 3);
	//g.addNode(1, NodeInfo(2)); ASSERT
	g.addEdge(g.getNode(0), g.getNode(1), EdgeInfo(1-0));
	FDK_ASSERT(&g.getNode(0).getOutEdge(g.getNode(1)) == &g.getNode(1).getInEdge(g.getNode(0)));
	FDK_ASSERT(&g.getNode(0).getOutEdge(g.getNode(1)) == &g.getEdge(g.getNode(0), g.getNode(1)));
	FDK_ASSERT(&g.getNode(0).getOutEdge(g.getNode(1)) == *g.getEdges().begin());
	FDK_ASSERT(&g.getEdge(g.getNode(0), g.getNode(1)).getStartNode() == &g.getNode(0));
	FDK_ASSERT(&g.getEdge(g.getNode(0), g.getNode(1)).getTargetNode() == &g.getNode(1));
	g.removeEdge(g.getEdge(g.getNode(0), g.getNode(1)));
	FDK_ASSERT(!g.findEdge(g.getNode(0), g.getNode(1)));
	FDK_ASSERT(g.getEdges().empty());
	FDK_ASSERT(g.getNode(0).getOutEdges().empty());
	FDK_ASSERT(g.getNode(1).getInEdges().empty());
	g.addEdge(g.getNode(0), g.getNode(1), EdgeInfo(3));
	FDK_ASSERT(g.getEdge(g.getNode(0), g.getNode(1)).getInfo().value == 3);
	g.removeNode(g.getNode(0));
	FDK_ASSERT(!g.findNode(0));
	FDK_ASSERT(g.findNode(1));
	FDK_ASSERT(g.getEdges().empty());	
	FDK_ASSERT(g.getNode(1).getInEdges().empty());
	g.addEdge(g.getNode(1), g.getNode(1), EdgeInfo(5));
	FDK_ASSERT(g.getEdge(g.getNode(1), g.getNode(1)).getInfo().value == 5);
	g.getEdge(g.getNode(1), g.getNode(1)).getInfo().value = 7;
	FDK_ASSERT(g.getEdge(g.getNode(1), g.getNode(1)).getInfo().value == 7);
	FDK_ASSERT(g.getEdges().size() == 1);
	FDK_ASSERT(&g.getEdge(g.getNode(1), g.getNode(1)).getStartNode() == &g.getNode(1));
	FDK_ASSERT(&g.getEdge(g.getNode(1), g.getNode(1)).getTargetNode() == &g.getNode(1));
	FDK_ASSERT(&g.getNode(1).getInEdge(g.getNode(1)) == &g.getNode(1).getOutEdge(g.getNode(1)));
	g.removeNode(g.getNode(1));
	FDK_ASSERT(g.getEdges().empty());	
	FDK_ASSERT(g.getNodes().empty());
	g.addNode(0, NodeInfo(1));
	g.addNode(1, NodeInfo(2));
	g.addNode(2, NodeInfo(3));
	g.addEdge(g.getNode(0), g.getNode(0), EdgeInfo(0));
	g.addEdge(g.getNode(0), g.getNode(1), EdgeInfo(1));
	g.addEdge(g.getNode(0), g.getNode(2), EdgeInfo(2));
	g.addEdge(g.getNode(1), g.getNode(2), EdgeInfo(3));
	g.addEdge(g.getNode(2), g.getNode(0), EdgeInfo(4));
	FDK_ASSERT(g.getNodes().size() == 3);
	FDK_ASSERT(g.getEdges().size() == 5);
	FDK_ASSERT(g.getNode(0).getOutEdges().size() == 3);
	FDK_ASSERT(g.getNode(0).getInEdges().size() == 2);
	FDK_ASSERT(g.getNode(1).getOutEdges().size() == 1);
	FDK_ASSERT(g.getNode(1).getInEdges().size() == 1);
	FDK_ASSERT(g.getNode(2).getOutEdges().size() == 1);
	FDK_ASSERT(g.getNode(2).getInEdges().size() == 2);
	g.removeNode(g.getNode(0));
	FDK_ASSERT(g.getNode(1).getOutEdges().size() == 1);
	FDK_ASSERT(g.getNode(1).getInEdges().size() == 0);
	FDK_ASSERT(g.getNode(2).getOutEdges().size() == 0);
	FDK_ASSERT(g.getNode(2).getInEdges().size() == 1);
	g.clear();
	FDK_ASSERT(g.getNodes().empty());
	FDK_ASSERT(g.getEdges().empty());
}
