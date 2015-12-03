local Map = MapBase::new{id= 102}

--跳转点
Map.waypoint = {
	{ source={x=10,y=10,width=5,height=5}, dest={country=0, mapid=102, x=10,y=10, width=5,height=5} },
}

--区域定义
Map.zonedefs = {
	{x=544,y=157,witdh=10,height=13, kind=9 color=1},
	{x=1,y=2,width=2,height=4, kind=9 color=1},
}

--地图npc
Map.npcs = {
	{ id=5094, name="车夫", x=332, y=340, width=1,height=1, num=1, interval=3600},
	{ id=99, name="高级杂货商", x=334, y=349, interval=3600},
}


Map:init()
