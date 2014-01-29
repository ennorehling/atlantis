require "atlantis"

function main(turn)
    if atlantis.read_config("config.json")~=0 then
        print("error reading configuration file")
        return
    end
    print("current turn is " .. turn)
    atlantis.turn = turn
    if atlantis.read_game("data/"..turn..".atl")~=0 then
        print("error reading game data")
        return
    end
    for r in atlantis.regions.all do
        local rname = r.terrain
        if r.name~=nil then
            rname = r.name .. ", " .. rname
        end
        print(rname .. "(" .. r.x .. "," .. r.y .. ")")
        for u in r.units do
            print(" - " .. u.name .. "(" .. u.id .. "), " .. u.faction.name)
        end
    end
    for f in atlantis.factions.all do
        print(f.name .. " (" .. f.id .. "), " .. f.email)
    end
end

function read_turn()
    local f = io.open("turn", "r")
    local t = f:read("*line")
    f:close()
    return tonumber(t)
end

turn = tonumber(arg[1]) or read_turn()
main(turn)
