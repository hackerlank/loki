function GetTableSize(t)
	if type(t) == "table" then
		return #t
	else
		return 0
	end
end


Common = {
	super_ip = "127.0.0.1",
	super_port = "10000",

	hostname="tcp://127.0.0.1:3306",
	user="root",
	password="fuck@123",
	database="loki",
}

LoginServer = {
	login_port = "10010",
	login_ip = "127.0.0.1",
	server_port = "10011",
	server_ip = "127.0.0.1",
}

SuperServer = {

	login_ip = "127.0.0.1",
	login_port="10011",
}

ServerList = {
	{id= 1, _type= 1, name="Super", ip="127.0.0.1", port=10000, extip="127.0.0.1", extport=10000, nettype=0},
	{id=11, _type=11, name="Record", ip="127.0.0.1", port=11001, extip="127.0.0.1", extport=11001, nettype=0},
	{id=12, _type=12, name="Bill", ip="127.0.0.1", port=12001, extip="127.0.0.1", extport=12001, nettype=0},
	{id=20, _type=20, name="Session", ip="127.0.0.1", port=20001, extip="127.0.0.1", extport=20001, nettype=0},
	{id=21, _type=21, name="Scene", ip="127.0.0.1", port=21001, extip="127.0.0.1", extport=21001, nettype=0},
	{id=22, _type=21, name="Scene", ip="127.0.0.1", port=21002, extip="127.0.0.1", extport=21002, nettype=0},
	{id=40, _type=22, name="Gate", ip="127.0.0.1", port=22001, extip="127.0.0.1", extport=22001, nettype=0},
	{id=41, _type=22, name="Gate", ip="127.0.0.1", port=22002, extip="127.0.0.1", extport=22002, nettype=0}
}

GatewayServer = {

}

require ("script/json")

if type(Json) ~= "table" then
	error("can not load Json lib")
	return
end

local function dump_value(o, indent)
	local t = type(o)
	if t == "nil" then
		return
	end

	local sp = "  "
	local ssp = ""
	for i = 2, indent do 
		ssp = ssp..sp 
	end

	if t == "table" then
		for i,v in pairs(o) do
			if type(v) ~= "table" then
				print(ssp..i,":", v)
			else
				dump_value(v, indent + 1)
			end
		end
	else
		print(ssp..o)
	end
end

function MsgParse(msg, len)
	--[[
	if msg == "dofile" then
		dofile("script/json.lua")
		print("dofile")
		return
	end
	]]
	--print("type msg = ", type(msg), msg)
	local ret, cmd = pcall(Json.Decode, msg)
	if not ret then
		print("parse error ", cmd)
		return false
	end
	if type(cmd) ~= "table" then
		print("Parse error cmd = ", cmd)
		return
	end
	--[[
	if cmd.id == nil then
		return false
	end
	]]

	dump_value(cmd, 1)

	return true
end
