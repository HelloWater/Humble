require("init")
local tcp2 = require("deftcp")

function initTask()
	
end

function destroyTask()
	
end

local function test1(sock, sockType, netMsg)
	tcp2.Response(sock, 25, netMsg)
end
regIProto(25, test1)

local function test2(sock, sockType, netMsg)
	tcp2.Response(sock, 26, netMsg)
end
regIProto(26, test2)
