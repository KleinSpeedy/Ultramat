local https = require("ssl.https")
local ltn12 = require("ltn12")
local json = require("dkjson")

local function split_version(vers)
	local major, minor, bugfix = vers:match("(%d+)%.(%d+)%.(%d+)")
	return tonumber(major), tonumber(minor), tonumber(bugfix)
end

local function is_new_version_available(current, latest)
	local maj1, min1, bugf1 = split_version(current)
	local maj2, min2, bugf2 = split_version(latest)

	if maj1 < maj2 then
		return true
	elseif maj1 <= maj2 and min1 < min2 then
		return true
	elseif maj1 <= maj2 and min1 <= min2 and bugf1 < bugf2 then
		return true
	end

	return false
end

local function get_latest_release_version(repo)
	local url = string.format("https://api.github.com/repos/KleinSpeedy/%s/releases/latest", repo)
	local response = {}

	local _, status = https.request({
		url = url,
		sink = ltn12.sink.table(response),
		protocol = "tlsv1_2",
	})

	if status == 200 then
		local body = table.concat(response)
		local data, _, err = json.decode(body)
		if err then
			print(err)
			return nil
		end
		if data and data.tag_name then
			return data.tag_name:match("v?(%d+%.%d+%.%d+)")
		end
	end

	return nil
end

function is_firmware_update_available(major, minor, bugfix, repo)
	if not repo:match("Ultramat") and not repo:match("Ultramat-Pico") then
		return false
	end

	local latest = get_latest_release_version(repo)
	if latest == nil then
		return false
	end

	local current = string.format("v%d.%d.%d", major, minor, bugfix)
	local ret = is_new_version_available(current, latest)
	return ret
end
