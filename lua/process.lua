require "atlantis"

function main(turn)
    if atlantis.read_config("config.json")~=0 then
        print("error reading configuration file")
    end
    print("current turn is " .. turn)
    atlantis.read_game(turn)
    atlantis.read_orders("orders." .. turn)
    atlantis.process()
    atlantis.write_game(turn+1)
    for f in atlantis.factions.all do
        atlantis.write_report(f)
    end
end

function read_turn()
    local f = io.open("turn", "r")
    local t = f:read("*line")
    f:close()
    return tonumber(t)
end

atlantis.turn = tonumber(arg[1]) or read_turn()
main(atlantis.turn)
