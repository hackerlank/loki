CommandMap = { m = {}, M = {} }

function GetMessageNameByID(id)
	local n = CommandMap.m[id]
	if n ~= nil then
		return n
	else
		return ""
	end
end

function GetMessageIDByName(name)
	local id = CommandMap.M[name]
	if id ~= nil then
		return id
	else
		return 0
	end
end


function CommandMap.Register(id, name)
	CommandMap.m[id] = name
	CommandMap.M[name] = id
end
