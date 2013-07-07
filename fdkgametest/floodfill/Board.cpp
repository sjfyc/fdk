#include "Board.h"
#include "Util.h"
#include "Font.h"
#include "Game.h"
#include <fdkgame/NaviGridEnvUtil.h>
#pragma warning(disable:4244)

Board::Board()
{
	_Base::resetMap(CELL_COUNT_X, CELL_COUNT_Y);
	annotateMap();
	m_colorComponent = new fdkgame::navi::GridEnvColorComponent(*this);
	double startSecs = util::getSeconds();
	m_colorComponent->refill();
	double endSecs = util::getSeconds();
	util::output("cost %lf", (endSecs-startSecs)*(1024*1024.f)/(CELL_COUNT_X*CELL_COUNT_Y));
	m_connectorComponent = new fdkgame::navi::GridEnvConnectorComponent(*m_colorComponent);
	m_connectorComponent->clear();
}

Board::~Board()
{
	delete m_connectorComponent;
	m_connectorComponent = 0;
	delete m_colorComponent;
	m_colorComponent = 0;
}

bool Board::isValidCoord(const CellCoord& coord) const
{
	return _Base::isValidNodeCoord(coord);
}

void Board::clearBlocks()
{
	_Base::clearObstacles();
}

void Board::setBlock(const CellCoord& coord, bool bBlock)
{
	_Base::setObstacle(toNodeID(coord), bBlock);
}

bool Board::isBlock(const CellCoord& coord) const
{
	return !_Base::isNodeReachable(toNodeID(coord));
}

void Board::draw()
{
	for (int i = 0; i <= CELL_COUNT_Y; ++i) // ºáÏß
	{	
		g_HGE->Gfx_RenderLine(0, i*CELL_SIZE_Y, CELL_COUNT_X*CELL_SIZE_X, i*CELL_SIZE_Y, COLOR_BOARD_LINE);
	}
	for (int i = 0; i <= CELL_COUNT_X; ++i) // ÊúÏß
	{
		g_HGE->Gfx_RenderLine(i*CELL_SIZE_X, 0, i*CELL_SIZE_X, CELL_COUNT_Y*CELL_SIZE_Y, COLOR_BOARD_LINE);
	}
	for (short x = 0; x < CELL_COUNT_X; ++x)
	{
		for (short y = 0; y < CELL_COUNT_Y; ++y)
		{
			if (isBlock(CellCoord(x, y)) )
			{
				util::fillCell(CellCoord(x, y), COLOR_CELL_BLOCK);
			}
		}
	}
}

void Board::drawCellColor()
{
	for (int y = 0; y < CELL_COUNT_Y; ++y)
	{	
		for (int x = 0; x < CELL_COUNT_X; ++x)
		{
			ColorComponent::ColorType color = m_colorComponent->getColorMap()(x, y);
			if (color != ColorComponent::UNCOLORED)
			{
				util::fillCell(CellCoord(x, y), ARGB(255, color * 40, 0, 0));
				g_Font.printf(x*CELL_SIZE_X+2, y*CELL_SIZE_Y+2, HGETEXT_LEFT, "%d", color);				
			}
		}
	}
	g_Font.printf(CELL_SIZE_X, CELL_SIZE_Y, HGETEXT_LEFT, "main color is %d, with %d nodes", 
		m_colorComponent->getMainColor(), 
		m_colorComponent->getNodeCountWithColor(m_colorComponent->getMainColor())
		);
}

void Board::drawCellCoord()
{
	for (int y = 0; y < CELL_COUNT_Y; ++y)
	{	
		for (int x = 0; x < CELL_COUNT_X; ++x)
		{
			g_Font.printf(x*CELL_SIZE_X+2, y*CELL_SIZE_Y+2, HGETEXT_LEFT, "%d,%d", x, y);
		}
	}
}

void Board::drawCellClearanceValue()
{
	for (int y = 0; y < CELL_COUNT_Y; ++y)
	{	
		for (int x = 0; x < CELL_COUNT_X; ++x)
		{
			g_Font.printf(x*CELL_SIZE_X+2, y*CELL_SIZE_Y+2, HGETEXT_LEFT, "%d", getClearanceValue(toNodeID(CellCoord(x, y)) ) );
		}
	}
}

void Board::drawCenterVertex()
{
	for (short x = 0; x < CELL_COUNT_X; ++x)
	{
		for (short y = 0; y < CELL_COUNT_Y; ++y)
		{
			util::fillRectByCenter( util::cellCoordToLocationInCenter(CellCoord(x, y)), Location(2, 2), ARGB(255,122,143,177));
		}
	}
}

void Board::drawConnectors()
{
	for (fdkgame::navi::GridEnvConnectorComponent::Connectors::iterator it = m_connectorComponent->getConnectors().begin();
		it != m_connectorComponent->getConnectors().end(); ++it)
	{
		fdkgame::navi::GridEnvConnectorComponent::Connector* connector = *it;
		std::set<fdkgame::navi::GridEnvConnectorComponent::NodeCoord>::iterator itNode = connector->occupiedNodes.begin();
		for (; itNode != connector->occupiedNodes.end(); ++itNode)
		{
			const fdkgame::navi::GridEnvConnectorComponent::NodeCoord& coord = *itNode;
			util::fillCell(coord, ARGB(255, 255, 0, 255));
		}
	}
	g_Font.printf(CELL_SIZE_X, CELL_SIZE_Y, HGETEXT_LEFT, "connected=%d", 
		m_connectorComponent->isConnected(g_Game.getStartCoord(), g_Game.getTargetCoord()) ? 1 : 0);

	g_Font.printf(CELL_SIZE_X, 2 * CELL_SIZE_Y, HGETEXT_LEFT, "start connect %d node",
		m_connectorComponent->getConnectedNodeCount(g_Game.getStartCoord()) );

	g_Font.printf(CELL_SIZE_X, 3 * CELL_SIZE_Y, HGETEXT_LEFT, "target connect %d node",
		m_connectorComponent->getConnectedNodeCount(g_Game.getTargetCoord()) );

	int firstConnectToTargetNodeFromStart = getFirstConnectedNode(*this, 
		toNodeID(g_Game.getStartCoord()),
		toNodeID(g_Game.getTargetCoord())
		);
	util::fillCell(toNodeCoord(firstConnectToTargetNodeFromStart), ARGB(255, 0, 0, 150) );
}
