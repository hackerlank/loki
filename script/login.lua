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

--[[
ServerList =
{
	--id, type, name, ip, port, extip, extport, nettype
	{id=1,_type=1, name="SuperServer", ip="127.0.0.1", port=10002, extip="127.0.0.1", extport=10002, nettype=1},
	{id=20,_type=20, name="SessionServer", ip="127.0.0.1", port=10005, extip="127.0.0.1", extport=10005, nettype=1},
	{id=21,_type=21,  name="ScenesServer", ip="127.0.0.1", port=10006, extip="127.0.0.1", extport=10006, nettype=1},
	{id=22,_type=21,  name="ScenesServer", ip="127.0.0.1", port=10007, extip="127.0.0.1", extport=10007, nettype=1},
	{id=2200,_type=22,name="GatewayServer", ip="127.0.0.1", port=10010, extip="127.0.0.1", extport=10010, nettype=1},
	{id=2201,_type=22,name="GatewayServer", ip="127.0.0.1", port=10011, extip="127.0.0.1", extport=10011, nettype=1},
	{id=11,_type=11,name="RecordServer", ip="127.0.0.1", port=10100, extip="127.0.0.1", extport=10100, nettype=1},
	{id=12,_type=12,name="BillServer", ip="127.0.0.1", port=11100, extip="127.0.0.1", extport=11100, nettype=1},
}
]]

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
