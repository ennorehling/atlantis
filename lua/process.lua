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
    if atlantis.read_orders("orders." .. turn)~=0 then
        print("error reading orders")
        return
    end
    atlantis.process()
    atlantis.turn = turn+1
    if atlantis.write_game("data/"..(turn+1)..".atl")~=0 then
        print("error writing game data")
        return
    end
    for f in atlantis.factions.all do
        atlantis.write_reports(f)
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
