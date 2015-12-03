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

GatewayServer = {

}

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

require ("script/mapinfo")
