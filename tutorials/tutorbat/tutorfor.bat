@echo off

set HEADER_FILES=NaviAStar.h NaviBase.h NaviBlockMap.h NaviGridEnvUtil.h NaviMapManager.h NaviTileMap.h NaviTypes.h NaviVertexMap.h
for %%i in (%HEADER_FILES%) do (
	echo %%i
)

pause