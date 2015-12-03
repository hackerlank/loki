ScenesInfo = {
	count = 10,
	[21] = { country = 2, mapid={102,101,104} },
	[22] = { country = 3, mapid={102,101,104,103} },
	[23] = { country = 4, mapid={102,101,104} },
	[24] = { country = 5, mapid={102,101,104} },
	[25] = { country = 7, mapid={102,101,104} },
	[26] = { country = 8, mapid={102,101,104} },
	[27] = { country = 9, mapid={102,101,104} },
	[28] = { country = 10, mapid={102,101,104} },
	[29] = { country = 11, mapid={102,101,104} },
	[30] = { country = 12, mapid={102,101,104} },
	[32] = { country = 6, mapid={102,101,104} },
}

function initSceneManager()
	local serverid = get_sub_service():get_id()
	--print("server id=", serverid)								   
	local maps = ScenesInfo[serverid]
	local sm = getSceneManager()	--SceneManager

	if type(maps) == "table" and type(maps.country) == "number" and type(maps.mapid) == "table" then
		for i,v in pairs(maps.mapid) do
			--print("load map:",i,v)
			local scene = sm:loadScene(0, maps.country, v)
			sm:loadedSceneNotifySession(scene)
		end
	end
	return true
end

function StartServer()
	initSceneManager()
	print("init in script ok")
	return true
end
