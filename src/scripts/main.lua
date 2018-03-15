R"luastring"--(
-- DO NOT REMOVE THE ABOVE LINE. It is used to load this file as a C++ string.
-- There is a matching delimiter at the bottom of the file.

-- Set source!
if love._os ~= "Android" and love._os ~= "iOS" then
	love.filesystem.setSource(love.path.getfull(""))
end

-- LOVE Video Extension Project
require("lvep")
-- FiLive! love.handlers hijack!
require("livesim4")


-- DO NOT REMOVE THE NEXT LINE. It is used to load this file as a C++ string.
--)luastring"--"
