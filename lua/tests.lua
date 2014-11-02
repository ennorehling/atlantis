local num_tests = 0
local failed = 0

local function assert_equal(a, b, msg)
if (a~=b) then
    failed = failed + 1
    m = "expected " .. a .. " = " .. b
    if msg~=nil then
        m = m .. ", " ..msg
    end
    print(m)
end
num_tests = num_tests + 1
end

local function assert_object(obj, methods, msg)
for i, v in ipairs(methods) do
    assert_equal(type(obj['.get'][v]), 'function', msg)
end
end

require 'atlantis'
assert_object(atlantis.region, {'x', 'y', 'money', 'id', 'terrain', 'name', 'units', 'peasants'})
assert_object(atlantis.unit, {'region', 'money', 'id', 'number', 'name'})
assert_object(atlantis.faction, {'id', 'email', 'name'})
assert_equal(type(atlantis), 'table')
assert_equal(type(atlantis.region), 'table')
assert_equal(type(atlantis.regions), 'table')
assert_equal(type(atlantis.regions.get), 'function')
assert_equal(type(atlantis.regions.create), 'function')
assert_equal(type(atlantis.regions['.get'].all), 'function')
assert_equal(type(atlantis.unit), 'table')
assert_equal(type(atlantis.faction), 'table')
assert_equal(type(atlantis.factions), 'table')
assert_equal(type(atlantis.factions.get), 'function')
assert_equal(type(atlantis.factions.create), 'function')
assert_equal(type(atlantis.factions['.get'].all), 'function')
assert_equal(type(atlantis.read_config), 'function')
assert_equal(type(atlantis.read_orders), 'function')
assert_equal(type(atlantis.read_game), 'function')
assert_equal(type(atlantis.write_game), 'function')
assert_equal(type(atlantis.write_reports), 'function')
assert_equal(type(atlantis.free_game), 'function')
assert_equal(type(atlantis.process), 'function')
print(failed .. " tests failed out of " .. num_tests .. ".")
assert(failed==0)
